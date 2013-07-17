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
  CvPoint pt1b,pt2b, pt1t,pt2t,pt1r,pt2r, pt1l,pt2l,fb_center;
  int tempwidth,tempheight;
  CvRect regt,rectROIbot,rectROItop,rectROIlef,rectROIrgt;
  rectROItop=cvRect(0,0,80,10);
  rectROIbot=cvRect(0,50,80,10);
  rectROIlef=cvRect(0,10,10,40);
  rectROIrgt=cvRect(70,10,10,40);
  fb_center.x=80;fb_center.y=120;
  CvPoint b_cir_center,t_cir_center,r_cir_center,l_cir_center;
  CvPoint frame_center;
  CvPoint A,B,C,D;
  CvPoint temp;
  double angle,spinsize;
  int cir_radius=1; 
  int frame_width=160, frame_height=120;
  IplImage* frame;
  IplImage* threshframe;
  IplImage* hsvframe;
  IplImage* threshframebot;
  IplImage* threshframetop;
  IplImage* threshframergt;
  IplImage* threshframelef;
  IplImage* modframe;
  IplImage* dilframetop;
  IplImage* dilframebot;
  IplImage* dilframergt;
  IplImage* dilframelef;
  int moddiv=2,seq=0,seqdiv=2;
  int release=0, rmax=100;
  int modfheight, modfwidth;
  unsigned char sendBuf;
   CvCapture* capture = cvCaptureFromCAM( 1 );
   if ( !capture ) {
     fprintf(stderr, "ERROR: capture is NULL \n" );
     getchar();
     return -1;
   }
  cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,frame_width);// 120x160 
  cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,frame_height);
  cvNamedWindow( "mywindow", CV_WINDOW_AUTOSIZE );
   while ( 1 ) {
     // Get one frame
      frame = cvQueryFrame( capture );
     if ( !frame ) {
       fprintf( stderr, "ERROR: frame is null...\n" );
       getchar();
       break;
     }

     modfheight = frame->height;
     modfwidth = frame->width;
      modframe = cvCreateImage(cvSize((int)(modfwidth/moddiv),(int)(modfheight/moddiv)),frame->depth,frame->nChannels); //cvCreateImage(size of frame, depth, noofchannels)
     cvResize(frame, modframe,CV_INTER_LINEAR);
     // create HSV(Hue, Saturation, Value) frame
      hsvframe = cvCreateImage(cvGetSize(modframe),8, 3);
      
     cvCvtColor(modframe, hsvframe, CV_BGR2HSV); //cvCvtColor(input frame,outputframe,method)

      threshframe = cvCreateImage(cvGetSize(hsvframe),8,1);
    // cvInRangeS(hsvframe,cvScalar(0, 180, 140),cvScalar(15, 230, 235),threshframe); //cvInRangeS(input frame, cvScalar(min range),cvScalar(max range),output frame) red
     cvInRangeS(hsvframe,cvScalar(60, 40, 40),cvScalar(130, 120, 100),threshframe); //cvInRangeS(input frame, cvScalar(min range),cvScalar(max range),output frame)
     
      threshframebot=cvCloneImage(threshframe);
      cvSetImageROI(threshframebot,rectROIbot);

      threshframetop=cvCloneImage(threshframe);
      cvSetImageROI(threshframetop,rectROItop);

      threshframergt=cvCloneImage(threshframe);
      cvSetImageROI(threshframergt,rectROIrgt);

      threshframelef=cvCloneImage(threshframe);
      cvSetImageROI(threshframelef,rectROIlef);
 
//////////////////////////////////////////////////////////////////////////////////////////
    if (seq==0) {
      threshframebot=cvCloneImage(threshframe);
      cvSetImageROI(threshframebot,rectROIbot);
     dilframebot = cvCreateImage(cvGetSize(threshframebot),8,1);
     cvDilate(threshframebot,dilframebot,NULL,2); //cvDilate(input frame,

   //  tempwidth=cvGetSize(dilframebot).width;
   //  tempheight=cvGetSize(dilframebot).height;
   //  printf("dilframe: %d, %d \n",tempwidth,tempheight);
     CBlobResult blobs_bot;
     blobs_bot = CBlobResult(dilframebot,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs_bot.Filter(blobs_bot,B_EXCLUDE,CBlobGetArea(),B_LESS,50);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob_bot;
     blobs_bot.GetNthBlob(CBlobGetArea(),0,biggestblob_bot); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1b.x = biggestblob_bot.MinX()*moddiv;
     pt1b.y = biggestblob_bot.MinY()*moddiv+100;
     pt2b.x = biggestblob_bot.MaxX()*moddiv;
     pt2b.y = biggestblob_bot.MaxY()*moddiv+100;
     b_cir_center.x=(pt1b.x+pt2b.x)/2;
     b_cir_center.y=(pt1b.y+pt2b.y)/2;
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     threshframergt=cvCloneImage(threshframe);
     cvSetImageROI(threshframergt,rectROIrgt);
     dilframergt = cvCreateImage(cvGetSize(threshframergt),8,1);
     cvDilate(threshframergt,dilframergt,NULL,2); //cvDilate(input frame,

     CBlobResult blobs_rgt;
     blobs_rgt = CBlobResult(dilframergt,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs_rgt.Filter(blobs_rgt,B_EXCLUDE,CBlobGetArea(),B_LESS,50);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob_rgt;
     blobs_rgt.GetNthBlob(CBlobGetArea(),0,biggestblob_rgt); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1r.x = biggestblob_rgt.MinX()*moddiv+140;
     pt1r.y = biggestblob_rgt.MinY()*moddiv+20;
     pt2r.x = biggestblob_rgt.MaxX()*moddiv+140;
     pt2r.y = biggestblob_rgt.MaxY()*moddiv+20;
     r_cir_center.x=(pt1r.x+pt2r.x)/2;
     r_cir_center.y=(pt1r.y+pt2r.y)/2;

}
//////////////////////////////////////////////////////////////////////////////////////////
    if(seq==1){
      threshframetop=cvCloneImage(threshframe);
      cvSetImageROI(threshframetop,rectROItop);
      dilframetop = cvCreateImage(cvGetSize(threshframetop),8,1);
     cvDilate(threshframetop,dilframetop,NULL,2); //cvDilate(input frame,
     CBlobResult blobs_top;
     blobs_top = CBlobResult(dilframetop,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs_top.Filter(blobs_top,B_EXCLUDE,CBlobGetArea(),B_LESS,50);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob_top;
     blobs_top.GetNthBlob(CBlobGetArea(),0,biggestblob_top); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1t.x = biggestblob_top.MinX()*moddiv;
     pt1t.y = biggestblob_top.MinY()*moddiv;
     pt2t.x = biggestblob_top.MaxX()*moddiv;
     pt2t.y = biggestblob_top.MaxY()*moddiv;
     t_cir_center.x=(pt1t.x+pt2t.x)/2;
     t_cir_center.y=(pt1t.y+pt2t.y)/2;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      threshframelef=cvCloneImage(threshframe);
      cvSetImageROI(threshframelef,rectROIlef);
      dilframelef = cvCreateImage(cvGetSize(threshframelef),8,1);
     cvDilate(threshframelef,dilframelef,NULL,2); //cvDilate(input frame,
     CBlobResult blobs_lef;
     blobs_lef = CBlobResult(dilframelef,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs_lef.Filter(blobs_lef,B_EXCLUDE,CBlobGetArea(),B_LESS,50);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob_lef;
     blobs_lef.GetNthBlob(CBlobGetArea(),0,biggestblob_lef); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1l.x = biggestblob_lef.MinX()*moddiv;
     pt1l.y = biggestblob_lef.MinY()*moddiv+20;
     pt2l.x = biggestblob_lef.MaxX()*moddiv;
     pt2l.y = biggestblob_lef.MaxY()*moddiv+20;
     l_cir_center.x=(pt1l.x+pt2l.x)/2;
     l_cir_center.y=(pt1l.y+pt2l.y)/2;
	}

  //}
//////////////////////////////////////////////////////////////////////////////////////
   if(seq==2) {
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
     cvRectangle(frame,pt1r,pt2r,cvScalar(255,0,0),1,8,0);
     cvRectangle(frame,pt1l,pt2l,cvScalar(255,0,0),1,8,0);
     cvRectangle(frame,pt1b,pt2b,cvScalar(255,0,0),1,8,0); // draw rectangle around the biggest blob
     //cvRectangle(frame,pt1,pt2,cvScalar(255,0,0),1,8,0);
     cvCircle( frame, b_cir_center, cir_radius, cvScalar(0,255,255), 1, 8, 0 ); // center point of the rectangle
     cvLine(frame, A, B,cvScalar(255,0,255),2,8,0);
     cvLine(frame, C, D,cvScalar(255,0,255),2,8,0);
    

    if(t_cir_center.x!=0&&t_cir_center.y!=0)
     {
     cvLine(frame, fb_center, t_cir_center,cvScalar(255,255,0),1,8,0);
     printf("top:(%3d, %3d)\n",r_cir_center.x,r_cir_center.y);
     }

    if(l_cir_center.x!=0&&l_cir_center.y!=20)
     {
     if (l_cir_center.y<r_cir_center.y && (t_cir_center.x==0&&t_cir_center.y==0))
	{      
	cvLine(frame, fb_center, l_cir_center,cvScalar(255,255,0),1,8,0);
        printf("lef:(%3d, %3d)\n",l_cir_center.x,l_cir_center.y);
	}
     }
    if(r_cir_center.x!=140&&r_cir_center.y!=20)
     {
     if (r_cir_center.y<l_cir_center.y && (t_cir_center.x==0&&t_cir_center.y==0))
	{      
	cvLine(frame, fb_center, r_cir_center,cvScalar(255,255,0),1,8,0);
        printf("rgt:(%3d, %3d)\n",r_cir_center.x,r_cir_center.y);
	}
     }
/*
     if(r_cir_center.x!=0&&r_cir_center.y!=20)
     {
     if (r_cir_center.y<l_cir_center.y && (t_cir_center.x==0&&t_cir_center.y==0))
     cvLine(frame, fb_center, r_cir_center,cvScalar(255,255,0),1,8,0);
     }
     if ((b_cir_center.x!=0&&b_cir_center.y!=100)&&(t_cir_center.x!=0&&t_cir_center.y!=0)) 
     {
     cvLine(frame, b_cir_center, t_cir_center,cvScalar(0,255,255),1,8,0);
     printf("%d, %d, %d, %d\n",t_cir_center.x,t_cir_center.y,b_cir_center.x,b_cir_center.y);
     }	*/
  }
    seq++;
    seq=seq%(3);
     cvShowImage( "mywindow", frame); // show output image
     cvShowImage( "bot", threshframebot);
     cvShowImage( "top", threshframetop);
     cvShowImage( "right", threshframergt);
     cvShowImage( "left", threshframelef);
   cvReleaseImage(&frame);
   /*cvReleaseImage(&threshframe);
   cvReleaseImage(&hsvframe);
   cvReleaseImage(&threshframebot);
   cvReleaseImage(&modframe);
   cvReleaseImage(&dilframetop);
   cvReleaseImage(&dilframebot);*/
     //remove higher bits using AND operator
     if ( (cvWaitKey(10) & 255) == 27  ) break;
   }
   // Release the capture device housekeeping
   cvReleaseCapture( &capture );
   //v4l.flush();
   cvDestroyWindow( "mywindow" );
   
return 0;
 }


