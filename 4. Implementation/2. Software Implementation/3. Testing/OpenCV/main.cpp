/* To compile: g++ main.cpp -o output `pkg-config --cflags opencv --libs opencv` */

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

// #define RASPBERRY_PI    1

int main(int, char**)
{

    /* Initialize frame */
    Mat frame;

    /* Initialize video capture */
    VideoCapture cap;

    // Select camera and API backend
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API

    /* Open selected camera using selected API backend */
    cap.open(deviceID, apiID);

    /* Check for success in opening video capture stream */
    if (!cap.isOpened()) {

#ifdef RASPBERRY_PI
        printk("ERROR! Unable to open camera\n");
#else
        cerr << "ERROR! Unable to open camera\n";
#endif
        return -1;
    }

#ifdef RASPBERRY_PI
        printk("Camera initialized\n");
#else
        cout << "Started grabbing frames" << endl << "Press any key to terminate" << endl;
#endif
    
    /* Change exposure */
    cap.set(CAP_PROP_EXPOSURE, /*value*/);

    while(1) {

        /* Wait for a new camera frame and store it into 'frame' */
        cap.read(frame);

        /* Check for success in retrieving frame */
        if (frame.empty()) {

#ifdef RASPBERRY_PI
        printk("ERROR! blank frame grabbed\n");
#else
        cerr << "ERROR! blank frame grabbed\n";
#endif
            break;
        }

#ifndef RASPBERRY_PI
        /* Show live and wait for a key with timeout long enough to show images */
        imshow("Live", frame);
        if (waitKey(5) >= 0)
            break;
#endif
    }

    // The camera will be deinitialized automatically in the VideoCapture destructor
    
    return 0;
}
