#include "face_detect.h"
#include <string.h>
#include <math.h>
#include <glib.h>
#include <stdlib.h>
#include <thread>
#include <iostream>
#include <deque>
#include <sys/time.h>
#include <unistd.h>
#include <chrono>

#include "libyuv/convert.h"
#include "libyuv/planar_functions.h"
#include "libyuv/scale_argb.h"
#include "param_ops.h"
#include "MultPlayer.h"

#include <time.h>

#define DEBUG_INFO
#define MAX_CAMERA_NUM  6 
static int camera_num=6;
std::shared_ptr<BufManager<cv::Mat> > rgb_frame_cache;
std::vector<TsCamera*> _listTsCam;

static void thread_convert(void)
{
	printf("%s start..",__FUNCTION__);
	int width = 640;//1920;
	int height = 360;//1088;

	int m = 0;

        int num_w = sqrt(_listTsCam.size());
        num_w = num_w + (pow(num_w, 2) < _listTsCam.size() ? 1 : 0);
        int num_h = _listTsCam.size()/num_w + (_listTsCam.size()%num_w > 0 ? 1 :0);

    double elapsed_time = 0.0;
    struct timeval nframerate_time_end, time_start;
	int m_nDetectFramecount=0;
    gettimeofday(&time_start, nullptr);

	while(true)
    {
        std::shared_ptr<cv::Mat> imgframe;
        int left = 0,top = 0;

    	cv::Mat imageShow(Size(width*num_w, height*num_h), CV_8UC3);
        imgframe = std::make_shared<cv::Mat>(imageShow);
			
        for(int k  = 0; k < _listTsCam.size(); k++)
        {
            std::shared_ptr<GstSample> sample;
            GstBuffer *buffer = NULL;
            GstCaps *caps = NULL;
            const GstStructure *info = NULL;
            GstMapInfo map;
            int sample_width = 0;
            int sample_height = 0;
            left = k%num_w *(width);
            top = k/num_w *(height);
            TsCamera* pCam = _listTsCam.at(k);

				pCam->CatureNoWait(sample);
				if(sample == NULL  || sample.get() == NULL)
					continue;
				buffer = gst_sample_get_buffer(sample.get());
				if(buffer == NULL)
					continue;

				gst_buffer_map (buffer, &map, GST_MAP_READ);

				caps = gst_sample_get_caps(sample.get());
				if(caps == NULL)
				{
					printf ("[Camera %s]get caps is null\n", pCam->GetName().c_str());
					continue;
				}

				info = gst_caps_get_structure(caps, 0);
				if(info == NULL)
				{
					printf ("[Camera %s]get info is null\n", pCam->GetName().c_str());
					continue;
				}


				gst_structure_get_int(info, "width", &sample_width);
				gst_structure_get_int(info, "height", &sample_height);
				unsigned char *ybase = (unsigned char *)map.data;
				unsigned char *vubase = &map.data[sample_width * sample_height];
				unsigned char rgb24[sample_width*sample_height*3];

				libyuv::NV12ToRGB24(ybase, sample_width, vubase, sample_width, rgb24, sample_width*3, sample_width, sample_height);
				cv::Mat tmpmat(height, width, CV_8UC3, (unsigned char *)rgb24, cv::Mat::AUTO_STEP);
				tmpmat.copyTo(imageShow(cv::Rect(left,top,width,height)));

				gst_buffer_unmap(buffer, &map);
				// show caps on first frame
				if (!pCam->m_bPrintStreamInfo)
				{
					printf ("[Camera %s]%s\n", pCam->GetName().c_str(), gst_caps_to_string(caps));
					pCam->m_bPrintStreamInfo = true;
				}

	}
	rgb_frame_cache->feed(imgframe);

	usleep(40000);
#ifdef DEBUG_INFO
	m_nDetectFramecount++; 
	gettimeofday(&nframerate_time_end, nullptr);
	elapsed_time = (nframerate_time_end.tv_sec - time_start.tv_sec) * 1000 +
		(nframerate_time_end.tv_usec - time_start.tv_usec) / 1000;
	if(elapsed_time > 1000*10)
		{
			printf("[convert] convertframerate=%f\n", GetLocalTimeWithMs().c_str(),
					m_nDetectFramecount*1000/elapsed_time);
			memcpy(&time_start,&nframerate_time_end,sizeof(struct timeval));
			m_nDetectFramecount = 0;
		}
#endif
    }
}


static void thread_show(void)
{
    double elapsed_time = 0.0;
    struct timeval nframerate_time_end, time_start;
	int m_nDetectFramecount=0;
    gettimeofday(&time_start, nullptr);
    while(true)
    {
        std::shared_ptr<cv::Mat> imgframe;
        int num_w = sqrt(_listTsCam.size());
        num_w = num_w + (pow(num_w, 2) < _listTsCam.size() ? 1 : 0);
        int num_h = _listTsCam.size()/num_w + (_listTsCam.size()%num_w > 0 ? 1 :0);
        int width = 640;
        int height = 360;
        int left = 0,top = 0;

        imgframe = rgb_frame_cache->fetch();
        if(imgframe == NULL || imgframe.get() == NULL)
        {
            usleep(40*1000);
            continue;
        }

        cv::Mat showframe;
        cv::resize(*imgframe, showframe, cv::Size(1920,1080), 0, 0, cv::INTER_LINEAR);
        cv::imshow("sink", showframe);
        cv::waitKey(1);
#ifdef DEBUG_INFO
        usleep(40000);
		m_nDetectFramecount++; 
    gettimeofday(&nframerate_time_end, nullptr);
    elapsed_time = (nframerate_time_end.tv_sec - time_start.tv_sec) * 1000 +
                                (nframerate_time_end.tv_usec - time_start.tv_usec) / 1000;
    if(elapsed_time > 1000*10)
    {
      printf("[show] showframerate=%f\n", GetLocalTimeWithMs().c_str(),
                            m_nDetectFramecount*1000/elapsed_time);
	  memcpy(&time_start,&nframerate_time_end,sizeof(struct timeval));
      m_nDetectFramecount = 0;
    }
#endif
    }
}


int main(int argc, char *argv[])
{

	int count=0,i=0;
	char config_file[1024] = {0};
	count = MAX_CAMERA_NUM;
	if(argc> 1)
	{
		sprintf(config_file, "%s",argv[1]);;
	}
	else
		printf("error params\n ");

	rgb_frame_cache = std::make_shared<BufManager<cv::Mat> > ();
	TsMulitGstCamPlayer::GstEnvInit();

	ST_CameraConf stCameraConf;
	for(i=0; i<count; i++)
	{
		memset(&stCameraConf, 0, sizeof(stCameraConf));
		sprintf(stCameraConf.camera_dic, "decoder_%d",i);
		camera_param_load(config_file,&stCameraConf);

		TsCamera *pCam = new TsCamera();
		pCam->SetName(stCameraConf.cameraName);
		pCam->SetUri(stCameraConf.path/*"rtsp://admin:thundersoft106@10.0.4.222:554"*/);
		if(stCameraConf.rtsp)
			pCam->BuildPipeLine(true);
		else
			pCam->BuildPipeLine(false);
		pCam->Init(TsMulitGstCamPlayer::GetPipeline());
		_listTsCam.push_back(pCam);
		camera_num++;
	} 
	std::thread yuvconvertThread(thread_convert);

	std::thread showThread(thread_show);


    showThread.join();
    while(true) sleep(3);

    TsMulitGstCamPlayer::GstEnvDeinit();
    return 0;
}



