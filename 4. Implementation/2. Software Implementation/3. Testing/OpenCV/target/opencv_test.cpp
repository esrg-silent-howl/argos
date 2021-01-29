#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <string>
#include <stdio.h>

using namespace cv;
using namespace std;

#define HOME 	"/home/argos"
#define IMAGES	HOME "/img"

bool setup_dir() {
	
	return system("mkdir " IMAGES) == 0;
}

int main(int, char**)
{

    /* Initialize frame */
    Mat frame;

    /* Initialize video capture */
    VideoCapture cap;

    // Select camera and API backend
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API

	uint32_t frame_count;

	/* Setup images directory */
	setup_dir();
	cout << "Directory is set up\n";

    /* Open selected camera using selected API backend */
    cap.open(deviceID, apiID);

    /* Check for success in opening video capture stream */
    if (!cap.isOpened()) {

        cout << "ERROR! Unable to open camera\n";
        return -1;
    }

	cout << "Camera initialized\n";

    /* Change exposure */
    // cap.set(CAP_PROP_EXPOSURE, /*value*/);

    while(1) {

        /* Wait for a new camera frame and store it into 'frame' */
        cap.read(frame);

        /* Check for success in retrieving frame */
        if (frame.empty()) {

        	cout << "ERROR! blank frame grabbed\n";
            break;
        }

        /* Show live and wait for a key with timeout long enough to show images */
        cv::imwrite(IMAGES + to_string(frame_count++) + ".jpg", frame);
        // imshow("Live", frame);
        //if (waitKey(5) >= 0)
        //    break;


    }

    // The camera will be deinitialized automatically in the VideoCapture destructor
    
    return 0;
}
