#ifndef __TS_CAMERA_H__
#define __TS_CAMERA_H__

#include <gst/gst.h>
#include <gst/app/app.h>
#include <iostream>
#include <condition_variable>
#include <tr1/memory>
#include <unistd.h>
#include <sys/time.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/types_c.h"
#include "BufManager.h"

std::string GetLocalTimeWithMs(void);

using namespace cv;

class TsCamera;

class TsMulitGstCamPlayer
{
public:
    static void GstEnvInit(){gst_init (NULL, NULL);}
    static GstElement *GetPipeline(){return g_pPipeline;}
    static void GstEnvDeinit()
    {
        if(g_pPipeline!=NULL)
        {
            gst_element_set_state (g_pPipeline, GST_STATE_NULL);
            gst_object_unref (g_pPipeline);
            g_pPipeline = NULL;
        }
    }

private:
    static GstElement *g_pPipeline;
};

class TsCamera
{
public:
    TsCamera();
    ~TsCamera();

    void BuildPipeLine(bool rtsp);
    void Init(GstElement *pipeline);
    void Deinit();
    void SetName(std::string name);
    std::string GetName();
    std::string GetUri();
    void SetUri(std::string uri);
    void CatureNoWait(std::shared_ptr<GstSample>& dst);
    bool m_bPrintStreamInfo;
    std::mutex _gstTimerMut;
    std::condition_variable _gstTimerCond;
    void gst_pull_block();
private:
    std::string m_strline;
    bool m_bInited;
    static void onEOS(GstAppSink *appsink, void *user_data);
    static GstFlowReturn onPreroll(GstAppSink *appsink, void *user_data);
    static GstFlowReturn onBuffer(GstAppSink *appsink, void *user_data);

    static gboolean MY_BUS_CALLBACK (GstBus *bus, GstMessage *message, gpointer data);

    GstElement *m_pPipeline;
    GstElement *m_pSink;
    GstBus *m_pBus;

    int m_nFramecount;
    struct timeval m_nFramerate_time_start;

    std::string m_strName;
    std::string m_strUri;
    int m_nFramerate;
    std::shared_ptr<BufManager<GstSample> > frame_cache;

};


#endif
