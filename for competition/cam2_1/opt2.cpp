#include "cv.h"
#include "highgui.h"
#include <cvblobs/BlobResult.h>
#include <stdio.h>
#include <stdio.h>  
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
//#include "ImageBuffer.h"
int serial;
int openSerial( const char *path ) {
int port;
struct termios options;
port = open( path, O_RDWR | O_NOCTTY | O_NDELAY );
if( port == -1 ) {
printf( "Unable to open port %s\n", path );
return -1;
}
fcntl( port, F_SETFL, 0 );
tcgetattr( port, &options );
cfsetispeed( &options, B57600 );
cfsetospeed( &options, B57600 );
tcsetattr( port, TCSANOW, &options );
return port;
}

 int main() {
  CvPoint pt1b,pt2b, pt1t,pt2t,pt1,pt2,cir_center;
  int tempwidth,tempheight;
  CvRect regt,rectROIbot1,rectROItop1;
  rectROItop1=cvRect(0,0,80,10);
  rectROIbot1=cvRect(0,50,80,10);
  CvPoint b_cir_center,t_cir_center;
  CvPoint frame_center;
  CvPoint A,B,C,D;
  CvPoint temp;
  double angle,spinsize;
  int cir_radius=1; 
  int frame_width=160, frame_height=120;
  IplImage* frame1;
  IplImage* frame2;
  IplImage* threshframe1,threshframe2;
  IplImage* hsvframe1,hsvframe2;
  IplImage* threshframebot1;
  IplImage* threshframetop1;
  IplImage* modframe1,modframe2;
  IplImage* dilframetop1;
  IplImage* dilframe2;
  IplImage* dilframebot1;
  int moddiv=2,seq=0,seqdiv=2;
  int release=0, rmax=100;
  int modfheight, modfwidth;
  unsigned char sendBuf;
  serial = openSerial("/dev/ttyACM0");
  if (serial == -1)
  serial = openSerial("/dev/ttyACM1");
  if (serial == -1)
  serial = openSerial("/dev/ttyACM2"); 
  if (serial == -1)
  serial = openSerial("/dev/ttyACM3");	
  if (serial == -1)
  serial = openSerial("/dev/ttyACM4");	
  if (serial == -1)
  serial = openSerial("/dev/ttyACM5");
  if (serial == -1)
  serial = openSerial("/dev/ttyACM6"); 
  if (serial == -1)
  serial = openSerial("/dev/ttyACM7");	
  if (serial == -1)
  serial = openSerial("/dev/ttyACM8");	
  if( serial == -1 ) {
  return -1;  }
   CvCapture* capture1 = cvCaptureFromCAM( 1 );
   CvCapture* capture2 = cvCaptureFromCAM( 2 );
   if ( !capture1 ) {
     fprintf(stderr, "ERROR: capture 1 is NULL \n" );
     getchar();
     return -1;
   }

    if ( !capture2 ) {
     fprintf(stderr, "ERROR: capture 2 is NULL \n" );
     getchar();
     return -1;
   }
  cvSetCaptureProperty(capture1,CV_CAP_PROP_FRAME_WIDTH,frame_width);// 120x160 
  cvSetCaptureProperty(capture1,CV_CAP_PROP_FRAME_HEIGHT,frame_height);
  cvNamedWindow( "mywindow", CV_WINDOW_AUTOSIZE );

   cvSetCaptureProperty(capture2,CV_CAP_PROP_FRAME_WIDTH,frame_width);// 120x160 
  cvSetCaptureProperty(capture2,CV_CAP_PROP_FRAME_HEIGHT,frame_height);
  cvNamedWindow( "mywindow2", CV_WINDOW_AUTOSIZE );

   while ( 1 ) {
     // Get one frame
////////////////////////______++++++++++++++++=============>>>>>>>>>>>
      frame1 = cvQueryFrame( capture1 );
     if ( !frame1 ) {
       fprintf( stderr, "ERROR: frame 1 is null...\n" );
       getchar();
       break;
     }
         frame2 = cvQueryFrame( capture2 );
     if ( !frame2 ) {
       fprintf( stderr, "ERROR: frame 1 is null...\n" );
       getchar();
       break;
     }
     modfheight = frame1->height;
     modfwidth = frame1->width;
      modframe1 = cvCreateImage(cvSize((int)(modfwidth/moddiv),(int)(modfheight/moddiv)),frame1->depth,frame1->nChannels); //cvCreateImage(size of frame, depth, noofchannels)
     cvResize(frame1, modframe1,CV_INTER_LINEAR);
     // create HSV(Hue, Saturation, Value) frame
      hsvframe1 = cvCreateImage(cvGetSize(modframe1),8, 3);
      
     cvCvtColor(modframe1, hsvframe1, CV_BGR2HSV); //cvCvtColor(input frame,outputframe,method)

      threshframe1 = cvCreateImage(cvGetSize(hsvframe1),8,1);
    // cvInRangeS(hsvframe,cvScalar(0, 180, 140),cvScalar(15, 230, 235),threshframe); //cvInRangeS(input frame, cvScalar(min range),cvScalar(max range),output frame) red
     cvInRangeS(hsvframe1,cvScalar(70, 180, 40),cvScalar(100, 230, 90),threshframe1); //cvInRangeS(input frame, cvScalar(min range),cvScalar(max range),output frame)
     
      threshframebot1=cvCloneImage(threshframe1);
      cvSetImageROI(threshframebot1,rectROIbot1);

      threshframetop1=cvCloneImage(threshframe1);
      cvSetImageROI(threshframetop1,rectROItop1);
 
//////////////////////////////////////////////////////////////////////////////////////////
    if (seq==0) {
      threshframebot1=cvCloneImage(threshframe1);
      cvSetImageROI(threshframebot1,rectROIbot1);
     dilframebot1 = cvCreateImage(cvGetSize(threshframebot1),8,1);
     cvDilate(threshframebot1,dilframebot1,NULL,2); //cvDilate(input frame,

   //  tempwidth=cvGetSize(dilframebot).width;
   //  tempheight=cvGetSize(dilframebot).height;
   //  printf("dilframe: %d, %d \n",tempwidth,tempheight);
     CBlobResult blobs_bot1;
     blobs_bot1 = CBlobResult(dilframebot1,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs_bot1.Filter(blobs_bot1,B_EXCLUDE,CBlobGetArea(),B_LESS,50);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob_bot1;
     blobs_bot1.GetNthBlob(CBlobGetArea(),0,biggestblob_bot1); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1b.x = biggestblob_bot1.MinX()*moddiv;
     pt1b.y = biggestblob_bot1.MinY()*moddiv+100;
     pt2b.x = biggestblob_bot1.MaxX()*moddiv;
     pt2b.y = biggestblob_bot1.MaxY()*moddiv+100;
     b_cir_center.x=(pt1b.x+pt2b.x)/2;
     b_cir_center.y=(pt1b.y+pt2b.y)/2;}
//////////////////////////////////////////////////////////////////////////////////////////
    if(seq==seqdiv){
      threshframetop1=cvCloneImage(threshframe1);
      cvSetImageROI(threshframetop1,rectROItop1);
      dilframetop1 = cvCreateImage(cvGetSize(threshframetop1),8,1);
     cvDilate(threshframetop1,dilframetop1,NULL,2); //cvDilate(input frame,
     CBlobResult blobs_top1;
     blobs_top1 = CBlobResult(dilframetop1,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs_top1.Filter(blobs_top1,B_EXCLUDE,CBlobGetArea(),B_LESS,50);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob_top1;
     blobs_top1.GetNthBlob(CBlobGetArea(),0,biggestblob_top1); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1t.x = biggestblob_top1.MinX()*moddiv;
     pt1t.y = biggestblob_top1.MinY()*moddiv;
     pt2t.x = biggestblob_top1.MaxX()*moddiv;
     pt2t.y = biggestblob_top1.MaxY()*moddiv;
     t_cir_center.x=(pt1t.x+pt2t.x)/2;
     t_cir_center.y=(pt1t.y+pt2t.y)/2;}
//////////////////////////////////////////////////////////////////////////////////////
   if(seq==seqdiv+2) {
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
     cvRectangle(frame1,pt1t,pt2t,cvScalar(255,0,0),1,8,0);
     cvRectangle(frame1,pt1b,pt2b,cvScalar(255,0,0),1,8,0); // draw rectangle around the biggest blob
     //cvRectangle(frame,pt1,pt2,cvScalar(255,0,0),1,8,0);
     cvCircle( frame1, b_cir_center, cir_radius, cvScalar(0,255,255), 1, 8, 0 ); // center point of the rectangle
     cvLine(frame1, A, B,cvScalar(255,0,255),2,8,0);
     cvLine(frame1, C, D,cvScalar(255,0,255),2,8,0);
    

    if (b_cir_center.x!=0&&b_cir_center.y!=100) 
    {
    cvLine(frame1, b_cir_center, frame_center,cvScalar(0,255,0),1,8,0);
    }

    if(t_cir_center.x!=0&&t_cir_center.y!=0)
     {
     cvLine(frame1, frame_center, t_cir_center,cvScalar(255,255,0),1,8,0);
     }
     if ((b_cir_center.x!=0&&b_cir_center.y!=100)&&(t_cir_center.x!=0&&t_cir_center.y!=0)) 
     {
     cvLine(frame1, b_cir_center, t_cir_center,cvScalar(0,255,255),1,8,0);
     printf("%d, %d, %d, %d\n",t_cir_center.x,t_cir_center.y,b_cir_center.x,b_cir_center.y);
     }
		
}
    seq++;
    seq=seq%(seqdiv+4);
     cvShowImage( "mywindow", frame1); // show output image
     cvShowImage( "bot", threshframebot1);
     cvShowImage( "top", threshframetop1);
////////////////////////////////////**************>>>>>>>>>>>>>>

     modfheight = frame2->height;
     modfwidth = frame2->width;
     // create modified frame with 1/4th the original size
     IplImage* modframe2 = cvCreateImage(cvSize((int)(modfwidth/4),(int)(modfheight/4)),frame2->depth,frame2->nChannels); //cvCreateImage(size of frame, depth, noofchannels)
     cvResize(frame2, modframe2,CV_INTER_LINEAR);
     // create HSV(Hue, Saturation, Value) frame
     IplImage* hsvframe2 = cvCreateImage(cvGetSize(modframe2),8, 3);
     cvCvtColor(modframe2, hsvframe2, CV_BGR2HSV); //cvCvtColor(input frame,outputframe,method)
     // create a frame within threshold.
     IplImage* threshframe2 = cvCreateImage(cvGetSize(hsvframe2),8,1);
     cvInRangeS(hsvframe2,cvScalar(10, 180, 130),cvScalar(40, 240, 245),threshframe2); //cvInRangeS(input frame, cvScalar(min range),cvScalar(max range),output frame)
     // created dilated image
     IplImage* dilframe2 = cvCreateImage(cvGetSize(threshframe2),8,1);
     cvDilate(threshframe2,dilframe2,NULL,2); //cvDilate(input frame, output frame, mask, number of times to dilate)

     CBlobResult blobs2;
     blobs2 = CBlobResult(dilframe2,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs2.Filter(blobs2,B_EXCLUDE,CBlobGetArea(),B_LESS,50);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob2;
     blobs2.GetNthBlob(CBlobGetArea(),0,biggestblob2); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1.x = biggestblob2.MinX()*4;
     pt1.y = biggestblob2.MinY()*4;
     pt2.x = biggestblob2.MaxX()*4;
     pt2.y = biggestblob2.MaxY()*4;
     cir_center.x=(pt1.x+pt2.x)/2;
     cir_center.y=(pt1.y+pt2.y)/2;
     frame_center.x=frame_width/2;
     frame_center.y=frame_height/2;
     cvRectangle(frame2,pt1,pt2,cvScalar(255,0,0),1,8,0); // draw rectangle around the biggest blob
     cvCircle( frame2, cir_center, cir_radius, cvScalar(0,255,255), 1, 8, 0 ); // center point of the rectangle
     cvLine(frame2, A, B,cvScalar(255,0,255),2,8,0);
     cvLine(frame2, C, D,cvScalar(255,0,255),2,8,0);
     if (cir_center.x!=0&&cir_center.y!=0){
          //printf("%f, %f \n",angle*180/3.1416,spinsize/10);
	
     cvLine(frame2, cir_center, frame_center,cvScalar(0,255,0),1,8,0);
     sendBuf=88;
     write(serial, &sendBuf,1);
     sendBuf=cir_center.x;
     write(serial, &sendBuf,1);
     sendBuf=89;
     write(serial, &sendBuf,1);
     sendBuf=cir_center.y;
     write(serial, &sendBuf,1);
    
    //printf("%d %d %f\n",cir_center.x,cir_center.y, angle*180/3.1415);
     //sendvalue(serial, angle*180/3.1416);
     //cvCircle( frame, frame_center, cir_radius, cvScalar(0,255,255), 2, 8, 0 );
}

     cvShowImage( "mywindow2", frame2); // show output image
     //remove higher bits using AND operator
     if ( (cvWaitKey(10) & 255) == 27  ) break;

   }
   // Release the capture device housekeeping
   cvReleaseCapture( &capture1 );
   cvReleaseCapture( &capture2 );
   //v4l.flush();
   cvDestroyWindow( "mywindow" );
   cvDestroyWindow( "mywindow2" );
return 0;
 }


