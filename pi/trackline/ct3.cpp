#include "cv.h"
#include "highgui.h"
#include <cvblobs/BlobResult.h>
#include <stdio.h>
#include <stdio.h>  
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

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

IplImage* crop( IplImage* src ) 
{ 
   // Must have dimensions of output image 
   IplImage* cropped = cvCreateImage(cvSize(120,20), src->depth, src->nChannels ) ;

   // Say what the source region is 
   cvSetImageROI( src,  cvRect( 0,120-20, 160,20 ) ); 

   // Do the copy 
   cvCopy( src, cropped ); 
   cvResetImageROI( src ); 

   return cropped; 
} 
/*
CvPoint Top(IplImage* dilframe, IplImage, threshframe)
{   
    CvPoint pt1, pt2, centre;
    ROIrect=CvRect(0,0,80,5);
    cvSetImageROI(dilframe,ROIrect);
    IplImage* dilframeROI=cvCreateImage(cvGetSize(threshframe),8,1);
    CBlobResult blobs;
    blobs = CBlobResult(dilframe,NULL,0); 
    blobs.Filter(blobs,B_EXCLUDE,CBlobGetArea(),B_LESS,50);
    CBlob biggestblob;
    blobs.GetNthBlob(CBlobGetArea(),0,biggestblob); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
	     // get 4 points to define the rectangle
    pt1.x = biggestblob.MinX()*4;
    pt1.y = 120;//biggestblob.MinY()*4;
    pt2.x = biggestblob.MaxX()*4;
    pt2.y = 100;//biggestblob.MaxY()*4;
}
*/
CvPoint Bottom(IplImage* dilframe)
{
     CvPoint center,pt1,pt2;
     CBlobResult blobs;
     blobs = CBlobResult(dilframe,NULL,0); 
     blobs.Filter(blobs,B_EXCLUDE,CBlobGetArea(),B_LESS,50);
     CBlob biggestblob;
     blobs.GetNthBlob(CBlobGetArea(),0,biggestblob); 
     pt1.x = biggestblob.MinX();
     pt1.y = biggestblob.MinY();
     pt2.x = biggestblob.MaxX();
     pt2.y = biggestblob.MaxY();
     center.x= (int)(pt1.x+pt2.x)/2;
     center.y= (int)(pt1.y+pt2.y)/2+90;
     return center;
}

 int main() {
  CvPoint pt1b,pt2b, pt1t,pt2t,ptarry[4];
  int tempwidth,tempheight;
  CvRect regt,rectROIbot,rectROItop;
  rectROItop=cvRect(0,0,160,20);
  rectROIbot=cvRect(0,100,160,20);
  CvPoint b_cir_center,t_cir_center;
  CvPoint frame_center;
  CvPoint A,B,C,D;
  CvPoint temp;
  double angle,spinsize;
  int cir_radius=1; 
  int frame_width=160, frame_height=120;
  unsigned char sendBuf;/*
  int serial;
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
return -1;
}*/
   //CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
   CvCapture* capture = cvCaptureFromCAM(-1 );
   if ( !capture ) {
     fprintf(stderr, "ERROR: capture is NULL \n" );
     getchar();
     return -1;
   }
  cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,frame_width);// 120x160 
  cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,frame_height);
 // cvSetCaptureProperty(capture, CV_CAP_PROP_FPS,10);
//  cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,5);  
 // Create a window in which the captured images will be presented
   cvNamedWindow( "mywindow", CV_WINDOW_AUTOSIZE );
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
     IplImage* modframe = cvCreateImage(cvSize((int)(modfwidth),(int)(modfheight)),frame->depth,frame->nChannels); //cvCreateImage(size of frame, depth, noofchannels)
     cvResize(frame, modframe,CV_INTER_LINEAR);
     // create HSV(Hue, Saturation, Value) frame
     IplImage* hsvframe = cvCreateImage(cvGetSize(modframe),8, 3);
      
     cvCvtColor(modframe, hsvframe, CV_BGR2HSV); //cvCvtColor(input frame,outputframe,method)

     IplImage* threshframe = cvCreateImage(cvGetSize(hsvframe),8,1);
    // cvInRangeS(hsvframe,cvScalar(0, 180, 140),cvScalar(15, 230, 235),threshframe); //cvInRangeS(input frame, cvScalar(min range),cvScalar(max range),output frame) red
     cvInRangeS(hsvframe,cvScalar(70, 180, 40),cvScalar(100, 230, 90),threshframe); //cvInRangeS(input frame, cvScalar(min range),cvScalar(max range),output frame)
     
     IplImage* threshframebot=cvCloneImage(threshframe);
     IplImage* threshframetop=cvCloneImage(threshframe);
     cvSetImageROI(threshframebot,rectROIbot);
     cvSetImageROI(threshframetop,rectROItop);
//////////////////////////////////////////////////////////////////////////////////////////
     IplImage* dilframebot = cvCreateImage(cvGetSize(threshframebot),8,1);
     cvDilate(threshframebot,dilframebot,NULL,2); //cvDilate(input frame,
     CBlobResult blobs_bot;
     blobs_bot = CBlobResult(dilframebot,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs_bot.Filter(blobs_bot,B_EXCLUDE,CBlobGetArea(),B_LESS,50);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob_bot;
     blobs_bot.GetNthBlob(CBlobGetArea(),0,biggestblob_bot); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1b.x = biggestblob_bot.MinX();
     pt1b.y = biggestblob_bot.MinY()+100;
     pt2b.x = biggestblob_bot.MaxX();
     pt2b.y = biggestblob_bot.MaxY()+100;
     b_cir_center.x=(pt1b.x+pt2b.x)/2;
     b_cir_center.y=(pt1b.y+pt2b.y)/2;
//////////////////////////////////////////////////////////////////////////////////////////
     IplImage* dilframetop = cvCreateImage(cvGetSize(threshframetop),8,1);
     cvDilate(threshframetop,dilframetop,NULL,2); //cvDilate(input frame,
     CBlobResult blobs_top;
     blobs_top = CBlobResult(dilframetop,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs_top.Filter(blobs_top,B_EXCLUDE,CBlobGetArea(),B_LESS,50);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob_top;
     blobs_top.GetNthBlob(CBlobGetArea(),0,biggestblob_top); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1t.x = biggestblob_top.MinX();
     pt1t.y = biggestblob_top.MinY();
     pt2t.x = biggestblob_top.MaxX();
     pt2t.y = biggestblob_top.MaxY();
     t_cir_center.x=(pt1t.x+pt2t.x)/2;
     t_cir_center.y=(pt1t.y+pt2t.y)/2;
//////////////////////////////////////////////////////////////////////////////////////

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
     cvRectangle(frame,pt1t,pt2t,cvScalar(255,0,0),1,8,0);
     cvRectangle(frame,pt1b,pt2b,cvScalar(255,0,0),1,8,0); // draw rectangle around the biggest blob
     //cvRectangle(frame,pt1,pt2,cvScalar(255,0,0),1,8,0);
     cvCircle( frame, b_cir_center, cir_radius, cvScalar(0,255,255), 1, 8, 0 ); // center point of the rectangle
     cvLine(frame, A, B,cvScalar(255,0,255),2,8,0);
     cvLine(frame, C, D,cvScalar(255,0,255),2,8,0);
    
	 if ((b_cir_center.x!=0&&b_cir_center.y!=0)||(t_cir_center.x!=0&&t_cir_center.y!=0))
	{
	
     cvLine(frame, b_cir_center, frame_center,cvScalar(0,255,0),1,8,0);
     cvLine(frame, b_cir_center, t_cir_center,cvScalar(0,255,255),1,8,0);
     cvLine(frame, frame_center, t_cir_center,cvScalar(255,255,0),1,8,0);
 
/*
     sendBuf=88;
     write(serial, &sendBuf,1);
     sendBuf=cir_center.x;
     write(serial, &sendBuf,1);
     sendBuf=89;
     write(serial, &sendBuf,1);
     sendBuf=cir_center.y;
     write(serial, &sendBuf,1);
    */
    //printf("%d %d %f\n",cir_center.x,cir_center.y, angle*180/3.1415);
     //sendvalue(serial, angle*180/3.1416);
     //cvCircle( frame, frame_center, cir_radius, cvScalar(0,255,255), 2, 8, 0 );
	}

     cvShowImage( "mywindow", frame); // show output image
     //cvShowImage( "bot", threshframebot);
     //cvShowImage( "top", threshframetop);
   //  cvShowImage("croped",cropped);
     //cvShowImage( "mywindow3", dilframeROI);
     // Do not release the frame!
     //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
     //remove higher bits using AND operator
     if ( (cvWaitKey(10) & 255) == 27 ) break;
   }
   // Release the capture device housekeeping
   cvReleaseCapture( &capture );
   cvDestroyWindow( "mywindow" );
   return 0;
 }

