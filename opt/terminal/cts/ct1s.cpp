#include "cv.h"
#include "highgui.h"
#include <cvblobs/BlobResult.h>
#include <stdio.h>
#include <stdio.h>  
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>

int fd;
void int_serial(void)
{
  if ((fd=serialOpen("/dev/ttyAMA0",115200))<0)
   {
        printf("unable to open serial device\n");
   }
  if (wiringPiSetup()==-1)
  {
  	printf("unable to start wiringPi\n");
  }
}

void sendchar(char varchar)
{
   int val=varchar;
   fflush(stdout);
   serialPutchar(fd,varchar);
  // printf("send number is: %d\n",val);
}

 int main() {
  CvPoint pt1,pt2;
  CvRect regt;
  CvPoint cir_center;
  CvPoint frame_center;
  CvPoint A,B,C,D;
  CvPoint temp;
  int_serial();
  double angle,spinsize;
  int cir_radius=1; 
  int frame_width=160, frame_height=120;
  unsigned int sendBuf;

   CvCapture* capture = cvCaptureFromCAM( -1 );
   if ( !capture ) {
     fprintf(stderr, "ERROR: capture is NULL \n" );
     getchar();
     return -1;
   }
  cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,frame_width);// 120x160 
  cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,frame_height);
  cvSetCaptureProperty(capture, CV_CAP_PROP_FPS,10);
  //cvNamedWindow( "mywindow", CV_WINDOW_AUTOSIZE );
   // Show the image captured from the camera in the window and repeat
   while ( 1 ) {
     // Get one frame
     IplImage* frame = cvQueryFrame( capture );
     if ( !frame ) {
       fprintf( stderr, "ERROR: frame is null...\n" );
       getchar();
       break;
     }
     int modfheight, modfwidth;

     modfheight = frame->height;
     modfwidth = frame->width;
     // create modified frame with 1/4th the original size
     IplImage* modframe = cvCreateImage(cvSize((int)(modfwidth/4),(int)(modfheight/4)),frame->depth,frame->nChannels); //cvCreateImage(size of frame, depth, noofchannels)
     cvResize(frame, modframe,CV_INTER_LINEAR);
     // create HSV(Hue, Saturation, Value) frame
     IplImage* hsvframe = cvCreateImage(cvGetSize(modframe),8, 3);
     cvCvtColor(modframe, hsvframe, CV_BGR2HSV); //cvCvtColor(input frame,outputframe,method)
     // create a frame within threshold.
     IplImage* threshframe = cvCreateImage(cvGetSize(hsvframe),8,1);
     cvInRangeS(hsvframe,cvScalar(10, 180, 130),cvScalar(40, 240, 245),threshframe); //cvInRangeS(input frame, cvScalar(min range),cvScalar(max range),output frame)
     // created dilated image
     IplImage* dilframe = cvCreateImage(cvGetSize(threshframe),8,1);
     cvDilate(threshframe,dilframe,NULL,2); //cvDilate(input frame, output frame, mask, number of times to dilate)

     CBlobResult blobs;
     blobs = CBlobResult(dilframe,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs.Filter(blobs,B_EXCLUDE,CBlobGetArea(),B_LESS,50);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob;
     blobs.GetNthBlob(CBlobGetArea(),0,biggestblob); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1.x = biggestblob.MinX()*4;
     pt1.y = biggestblob.MinY()*4;
     pt2.x = biggestblob.MaxX()*4;
     pt2.y = biggestblob.MaxY()*4;
     cir_center.x=(pt1.x+pt2.x)/2;
     cir_center.y=(pt1.y+pt2.y)/2;
     frame_center.x=frame_width/2;
     frame_center.y=frame_height/2;
     A.x=frame_center.x-4;
     A.y=frame_center.y;
     B.x=frame_center.x+4;
     B.y=frame_center.y;
     C.y=frame_center.y-4;
     C.x=frame_center.x;
     D.y=frame_center.y+4;
     D.x=frame_center.x;
     cvRectangle(frame,pt1,pt2,cvScalar(255,0,0),1,8,0); // draw rectangle around the biggest blob
     cvCircle( frame, cir_center, cir_radius, cvScalar(0,255,255), 1, 8, 0 ); // center point of the rectangle
     cvLine(frame, A, B,cvScalar(255,0,255),2,8,0);
     cvLine(frame, C, D,cvScalar(255,0,255),2,8,0);
     if (cir_center.x!=0&&cir_center.y!=0){
     spinsize=sqrt((cir_center.x-frame_center.x)*(cir_center.x-frame_center.x) +(cir_center.y-frame_center.y)*(cir_center.y-frame_center.y));
     angle = atan2((double)cir_center.y-frame_center.y,(double)cir_center.x-frame_center.x);
     //printf("%f, %f \n",angle*180/3.1416,spinsize/10);
     temp.x=(int)(frame_center.x+spinsize/5*cos(angle+3.1416/4));
     temp.y=(int)(frame_center.y+spinsize/5*sin(angle+3.1415/4));
     cvLine(frame, temp, frame_center,cvScalar(0,255,0),1,8,0);	

     temp.x=(int)(frame_center.x+spinsize/5*cos(angle-3.1416/4));
     temp.y=(int)(frame_center.y+spinsize/5*sin(angle-3.1415/4));
     cvLine(frame, temp, frame_center,cvScalar(0,255,0),1,8,0);	
	
     cvLine(frame, cir_center, frame_center,cvScalar(0,255,0),1,8,0);
     sendBuf=253;
     sendchar(sendBuf);
     sendBuf=cir_center.x;
     sendchar(sendBuf);
     sendBuf=254;
     sendchar(sendBuf);
     sendBuf=cir_center.y;
     sendchar(sendBuf);
    printf("x: %d; y: %d\n",cir_center.x,cir_center.y);
}

    // cvShowImage( "mywindow", frame); // show output image
     if ( (cvWaitKey(10) & 255) == 27 ) break;
   }
   cvReleaseCapture( &capture );
   cvDestroyWindow( "mywindow" );
   return 0;
 }

