#include <string.h>
#include <math.h>
#include <glib.h>
#include <stdlib.h>
#include <thread>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <chrono>

#include "libyuv/convert.h"
#include "libyuv/planar_functions.h"
#include "libyuv/scale_argb.h"
#include "MultPlayer.h"
#include "common.h"

#include <time.h>


extern std::vector<TsCamera*> _listTsCam;
static int m_framerate=0;


GstElement *TsMulitGstCamPlayer::g_pPipeline = NULL;


static void deleterGstSample(GstSample* x) {
    if(x != NULL)
    {
        gst_sample_unref (x);
    }

}


TsCamera::TsCamera()
{
    m_bInited = false;
    m_bPrintStreamInfo = false;
    m_strName = "unkown";
}
TsCamera::~TsCamera()
{
    m_bInited = false;
    m_bPrintStreamInfo = false;
}
void TsCamera::Init(GstElement *pipeline)
{
    GError *error = NULL;

    if (!m_bInited)
    {
        printf("[Camera %s]not enable or init str pipeline\n", GetName().c_str());
        goto exit;
    }

    frame_cache = std::make_shared<BufManager<GstSample> > ();


    m_nFramecount = 0;
    m_nFramerate_time_start.tv_sec = 0;
    m_nFramerate_time_start.tv_usec = 0;

    m_pPipeline = pipeline;
    m_pPipeline = gst_parse_launch (m_strline.c_str(), &error);

    if (error != NULL) {
        printf ("[Camera %s]could not construct pipeline: %s\n", GetName().c_str(), error->message);
        g_clear_error (&error);
        goto exit;
    }

    /* get sink */
    m_pSink = gst_bin_get_by_name (GST_BIN (m_pPipeline), GetName().c_str());

    gst_app_sink_set_emit_signals((GstAppSink*)m_pSink, true);
    gst_app_sink_set_drop((GstAppSink*)m_pSink, true);
    gst_app_sink_set_max_buffers((GstAppSink*)m_pSink, 1);
    gst_base_sink_set_last_sample_enabled(GST_BASE_SINK(m_pSink), true);
    gst_base_sink_set_max_lateness(GST_BASE_SINK(m_pSink), 0);
    {//avoid goto check
    GstAppSinkCallbacks callbacks = { onEOS, onPreroll, onBuffer };
    gst_app_sink_set_callbacks (GST_APP_SINK(m_pSink), &callbacks, reinterpret_cast<void *>(this), NULL);
    }

    /* Putting a Message handler */
    m_pBus = gst_pipeline_get_bus (GST_PIPELINE (m_pPipeline));
    gst_bus_add_watch (m_pBus, MY_BUS_CALLBACK, reinterpret_cast<void *>(this));
    gst_object_unref (m_pBus);

    /* Run the pipeline */
    printf ("[Camera %s]Playing: %s\n", GetName().c_str(), GetUri().c_str());
    gst_element_set_state (m_pPipeline, GST_STATE_PLAYING);

    return;
exit:
    if(m_pSink!=NULL)
    {
    }
    if(m_pPipeline!=NULL)
    {
        gst_element_set_state (m_pPipeline, GST_STATE_NULL);
        gst_object_unref (m_pPipeline);
        m_pPipeline = NULL;
    }
    m_bInited = false;
}
void TsCamera::Deinit()
{
    m_bInited = false;
    if(m_pSink!=NULL)
    {
    }
    if(m_pPipeline!=NULL)
    {
        gst_element_set_state (m_pPipeline, GST_STATE_NULL);
        gst_object_unref (m_pPipeline);
        m_pPipeline = NULL;
    }

    printf ("[Camera %s]Eliminating Pipeline\n", GetName().c_str());
}
// onEOS
void TsCamera::onEOS(GstAppSink *appsink, void *user_data)
{
    TsCamera *dec = reinterpret_cast<TsCamera *>(user_data);
    printf("[Camera %s]gstreamer decoder onEOS\n", dec->GetName().c_str());
}

// onPreroll
GstFlowReturn TsCamera::onPreroll(GstAppSink *appsink, void *user_data)
{
    TsCamera *dec = reinterpret_cast<TsCamera *>(user_data);
    printf("[Camera %s]gstreamer decoder onPreroll\n", dec->GetName().c_str());
    return GST_FLOW_OK;
}

void TsCamera::gst_pull_block()
{
	std::unique_lock<std::mutex> lk(_gstTimerMut);
}

// onBuffer
GstFlowReturn TsCamera::onBuffer(GstAppSink *appsink, void *user_data)
{
    TsCamera *dec = NULL;
    GstSample *sample = NULL;
    double elapsed_time = 0.0;
    struct timeval nframerate_time_end;
    dec = reinterpret_cast<TsCamera *>(user_data);
    if(dec == NULL || appsink == NULL)
    {
        printf ("[Camera %s]decode or appsink is null\n", dec->GetName().c_str());
        return GST_FLOW_OK;
    }
    dec->gst_pull_block();
    if(!dec->m_nFramecount)gettimeofday(&dec->m_nFramerate_time_start, nullptr);
    dec->m_nFramecount++;

	sample = gst_base_sink_get_last_sample(GST_BASE_SINK(appsink));
    if(sample != NULL)
    {
         dec->frame_cache->feed(std::shared_ptr<GstSample>(sample, deleterGstSample));
    }


    gettimeofday(&nframerate_time_end, nullptr);
    elapsed_time = (nframerate_time_end.tv_sec - dec->m_nFramerate_time_start.tv_sec) * 1000 +
                                (nframerate_time_end.tv_usec - dec->m_nFramerate_time_start.tv_usec) / 1000;
    if(elapsed_time > 1000)
    {
		if(dec->m_nFramecount*1000/elapsed_time < m_framerate)
      printf("[Camera %s %s]TVMF framerate=%f\n", dec->GetName().c_str(), GetLocalTimeWithMs().c_str(),
                            dec->m_nFramecount*1000/elapsed_time);
      dec->m_nFramecount = 0;
    }

    return GST_FLOW_OK;
}
void TsCamera::BuildPipeLine(bool rtsp)
{
    std::ostringstream cameraPath;
        if(rtsp)
        {
            cameraPath << "rtspsrc location=" << GetUri() << " latency=0 tcp-timeout=500 drop-on-latency=true ntp-sync=true" << " ! ";
            cameraPath << "queue ! rtp" << "h264" << "depay ! "<< "h264" << "parse ! queue ! qtivdec ! ";
            cameraPath << "videoscale ! video/x-raw,width=640,height=360 ! appsink name=" << GetName() << " sync=false  max-lateness=0 max-buffers=1 drop=true";
        }
        else
        {
            cameraPath << "filesrc location=" << GetUri() << " " << " ! ";
            cameraPath << "qtdemux name=demux demux.  ! queue ! h264parse ! qtivdec !" ;
            cameraPath << " videoscale ! video/x-raw,width=640,height=360  ! appsink name=" << GetName() << " sync=false  max-lateness=0 max-buffers=1 drop=true";
        }
    m_strline = cameraPath.str();
    printf("[Camera %s]gstreamer decoder pipeline string:%s\n", GetName().c_str(), m_strline.c_str());
    m_bInited = true;
}
void TsCamera::SetName(std::string name)
{
    m_strName = name;
}
std::string TsCamera::GetName()
{
    return m_strName;
}
void TsCamera::SetUri(std::string uri)
{
    m_strUri = uri;
}
std::string TsCamera::GetUri()
{
    return m_strUri;
}

void TsCamera::CatureNoWait(std::shared_ptr<GstSample>& dst)
{
    if (!m_bInited)
    {
        printf("[Camera %s]not enable or init str pipeline\n", GetName().c_str());
        return;
    }

    dst = frame_cache->fetch();
}


gboolean TsCamera::MY_BUS_CALLBACK (GstBus *bus, GstMessage *message, gpointer data) {
    TsCamera *dec = reinterpret_cast<TsCamera *>(data);
    switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR: {
      GError *err;
      gchar *debug;

      gst_message_parse_error (message, &err, &debug);
      printf ("[Camera %s]Error: %s\n", dec->GetName().c_str(), err->message);
      g_error_free (err);
      g_free (debug);
      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      break;
    default:
      /* unhandled message */
      break;
    }
    /* we want to be notified again the next time there is a message
    * on the bus, so returning TRUE (FALSE means we want to stop watching
    * for messages on the bus and our callback should not be called again)
    */
    return TRUE;
}



