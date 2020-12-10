#ifndef __FACE_DETECT_H__
#define __FACE_DETECT_H__

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


using namespace cv;


class gstYUVDataTmp
{
public:
    unsigned char *data;
    int size;
    int width;
    int height;
    gstYUVDataTmp(unsigned char *data, int size, int width, int height)
    {
        this->data = (unsigned char *)malloc(size);
        memcpy(this->data, data, size);
        this->size = size;
        this->width = width;
        this->height = height;
    }
    ~gstYUVDataTmp()
    {
        if(data)
        {
            free(this->data);
            this->data = NULL;
        }
    }

};


#endif
