#include <ctime>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

using namespace std; 
 
int main (int argc, char **argv){

    if(argc < 2){
        cout <<" Usage: image2LEDmatrix imagefile [option]" << endl;
        return -1;
    }

    cv::Mat orgImage = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);

    if(!orgImage.data){
        cout <<  "Could not open or find the image" << endl;
        return -1;
    }

    int mode = 0;

    if(argc >= 3){
        mode = atoi(argv[2]);
    }

    int w = orgImage.size().width;
    int h = orgImage.size().height;

    int w_led = 32;
    int h_led = 16;

    cv::Mat resizedImage;
    cv::Mat croppedImage;
    cv::Mat ledImage;

    if(mode == 0){ // whole mode
        if(((double)w)/h < ((double)w_led)/h_led){
            cv::resize(orgImage, resizedImage, cv::Size(w*h_led/h, h_led), 0, 0, cv::INTER_AREA); 
            ledImage = cv::Mat::zeros(cv::Size(w_led, h_led), CV_8UC3); 
            cv::Rect rect((w_led-w*h_led/h)/2, 0, w*h_led/h, h_led);
            cv::Mat subImage=ledImage(rect);
            resizedImage.copyTo(subImage);
        }else{
            cv::resize(orgImage, resizedImage, cv::Size(w_led, h*w_led/w), 0, 0, cv::INTER_AREA); 
            ledImage = cv::Mat::zeros(cv::Size(w_led, h_led), CV_8UC3); 
            cv::Rect rect(0, (h_led-h*w_led/w)/2, w_led, h*w_led/w);
            cv::Mat subImage=ledImage(rect);
            resizedImage.copyTo(subImage);
        }
    }else{ // cropping mode
        if(((double)w)/h < ((double)w_led)/h_led){
            cv::Rect croppingRect(0, (h-h_led*w/w_led)/2, w, h_led*w/w_led);
            croppedImage = orgImage(croppingRect);
            cv::resize(croppedImage, ledImage, cv::Size(w_led, h_led), 0, 0, cv::INTER_AREA); 
        }else{
            cv::Rect croppingRect((w-w_led*h/h_led)/2, 0, w_led*h/h_led, h);
            croppedImage = orgImage(croppingRect);
            cv::resize(croppedImage, ledImage, cv::Size(w_led, h_led), 0, 0, cv::INTER_AREA); 

        }
    }

    // Setup UART
    int uart = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

    if (uart == -1){
        cerr<<"Error opening UART"<<endl;
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

    close(uart);

    return 0;
}

