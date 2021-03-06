/*****************************************************************************
*                                                                            *
*  OpenNI 2.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
#include <stdio.h>
#include <OpenNI.h>


#include "OniSampleUtilities.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms
#define MAX_DEPTH 10000

using namespace openni;
using namespace cv;

int main()
{
    float m_pDepthHist[MAX_DEPTH];
	Status rc = OpenNI::initialize();
	if (rc != STATUS_OK)
	{
		printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
		return 1;
	}

	Device device;
	rc = device.open(ANY_DEVICE);
	if (rc != STATUS_OK)
	{
		printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
		return 2;
	}

	VideoStream depth;

	if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
	{
		rc = depth.create(device, SENSOR_DEPTH);
		if (rc != STATUS_OK)
		{
			printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
			return 3;
		}
	}

	rc = depth.start();
	if (rc != STATUS_OK)
	{
		printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
		return 4;
	}

	VideoFrameRef frame;

	while (!wasKeyboardHit())
	{
		int changedStreamDummy;
		VideoStream* pStream = &depth;
		rc = OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy, SAMPLE_READ_WAIT_TIMEOUT);
		if (rc != STATUS_OK)
		{
			printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
			continue;
		}

		rc = depth.readFrame(&frame);
		if (rc != STATUS_OK)
		{
			printf("Read failed!\n%s\n", OpenNI::getExtendedError());
			continue;
		}

		if (frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM)
		{
			printf("Unexpected frame format\n");
			continue;
		}

		DepthPixel* pDepth = (DepthPixel*)frame.getData();
		calculateHistogram(m_pDepthHist, MAX_DEPTH, frame);

		int middleIndex = (frame.getHeight()+1)*frame.getWidth()/2;

		printf("[%08llu] ===>  %8d\n", (long long)frame.getTimestamp(), pDepth[middleIndex]);
		int height_frame = frame.getHeight();
		int width_frame = frame.getWidth();
		printf(" Resolution %d   %d\n ", height_frame, width_frame);
		Mat depth_frame = cv::Mat::zeros(height_frame, width_frame, CV_8UC1);
        
		int rowSize = frame.getStrideInBytes() / sizeof(openni::DepthPixel);
		const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*)frame.getData();
		printf("size of row: %i \n", rowSize);
		for (int i=0;i<height_frame;i++){
			const openni::DepthPixel* pDepth = pDepthRow;
			for (int j=0;j<width_frame;j++, ++pDepth){
			 	short  new_value = static_cast<short> ((*pDepth));
				unsigned int value_depth = ((*pDepth)*255)/1770; 
				if (((*pDepth)>0) && ((*pDepth)<910)){
					//depth_frame.at<unsigned short>(i, j) = static_cast<unsigned short> ((new_value));
					//depth_frame.at<unsigned short>(i, j) = ((new_value));
					//depth_frame.at<uchar>(i, j) =  (((*pDepth)*12));
					depth_frame.at<uchar>(i, j) =  value_depth;
					//depth_frame.at<uchar>(i, j) = 255;
					//depth_frame.at<uchar>(i, j) = pDepth[i*height_frame + j] & 0xff;
				}else{
					//depth_frame.at<uchar>(i, j) = pDepth[i*height_frame + j];
					depth_frame.at<uchar>(i, j) = 255;
				}
				// depth_frame.at<unsigned int>(i, j) = pDepth[i*height_frame + j];
				int nHistValue = m_pDepthHist[*pDepth]; 
				//printf(" jojojo message \n ");
				//depth_frame.at<uchar>(i, j) = (uchar) ((static_cast<float> (*pDepth))*256/10000);
				//depth_frame.at<unsigned short>(i, j) =  (((*pDepth)*12));
			}
			pDepthRow += rowSize;
		}
                
		//Draw depth image
		imshow("Cursed window",depth_frame);
		medianBlur(depth_frame, depth_frame, 3);
		imshow("median filter",depth_frame);
		GaussianBlur(depth_frame, depth_frame, cv::Size(9, 9), 0, 0);
		imshow("Improved window", depth_frame);
		waitKey(10);
		
	}

	depth.stop();
	depth.destroy();
	device.close();
	OpenNI::shutdown();

	return 0;
}
