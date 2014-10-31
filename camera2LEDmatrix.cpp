#include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

using namespace std; 
 
int main (int argc, char **argv){
   
    int w = 640;
    int h = 480;

    int w_led = 32;
    int h_led = 16;

    raspicam::RaspiCam_Cv Camera;
    cv::Mat orgImage;
    cv::Mat croppedImage;
    cv::Mat ledImage;

    //set camera params
    Camera.set(CV_CAP_PROP_FORMAT, CV_8UC4);
    Camera.set(CV_CAP_PROP_FRAME_WIDTH, w);
    Camera.set(CV_CAP_PROP_FRAME_HEIGHT, h);

    cv::Rect croppingRect(0, (h-h_led*w/w_led)/2, 
                           w, h_led*w/w_led);

    //Open camera
    if (!Camera.open()){
        cerr<<"Error opening the camera"<<endl;
        return -1;
    }

    // Setup UART
    int uart = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

    if (uart == -1){
        cerr<<"Error opening UART"<<endl;
        Camera.release(); 
        return -1;
    }
    struct termios options;
    tcgetattr(uart, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart, TCIFLUSH);
    tcsetattr(uart, TCSANOW, &options);

    char buf[4];

    //Start capture
    while(true){
        Camera.grab();
        Camera.retrieve(orgImage);
        croppedImage = orgImage(croppingRect);
        cv::resize(croppedImage, ledImage, cv::Size(w_led, h_led), 0, 0); 

        for(int j=0; j<h_led ; j++) {
            cv::Vec3b* ptr = ledImage.ptr<cv::Vec3b>(j);
            for(int i=0; i<w_led; i++) {
                cv::Vec3b bgr = ptr[i];
                bgr[0] >>= 4;
                bgr[1] >>= 4;
                bgr[2] >>= 4;
                sprintf(buf, "%x%x%x", bgr[2], bgr[1], bgr[0]);
                write(uart, buf, 3);
            }
        } 
        buf[0] = '.';
        write(uart, buf, 1);

        usleep(100000);
    }
    Camera.release();
    close(uart);

    return 0;
}

