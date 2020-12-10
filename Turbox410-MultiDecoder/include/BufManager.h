#ifndef __BUFMANAGER_H__
#define __BUFMANAGER_H__

#include <iostream>
#include <mutex>
#include <atomic>
#include <memory>
#include <list>
#include <gst/gst.h>
#include <gst/app/app.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/types_c.h"


//using namespace std;
//namespace buffer

template<typename T>
class BufManager {
public:
    BufManager();
    ~BufManager();
    void feed(std::shared_ptr<T> pending);
    std::shared_ptr<T> front();
    std::shared_ptr<T> fetch();
private:
    std::atomic<bool> swap_ready;
    std::mutex swap_mtx;
    std::shared_ptr<T> front_sp;
    std::shared_ptr<T> back_sp;
public:
    std::string debug_info;

};
#define TT
//typedef BufManager<cv::Mat> MatBufManager;
//typedef BufManager<GstSample> GstBufManager;
//#include "BufManager.tpp"
#endif
