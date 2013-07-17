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

CvScalar black1=cvScalar(20, 90, 30);
CvScalar black2=cvScalar(120, 230, 110);
CvScalar yel1=cvScalar(20, 70, 155);
CvScalar yel2=cvScalar(60, 190, 260);
CvScalar Yel1=cvScalar(20, 70, 155);
CvScalar Yel2=cvScalar(60, 190, 260);

double speed;
int moddiv=2;
int seq=0,seqdiv=1;
int size=25,outsize=10;  
double kp=80,ki=0,kd=0;
double e=0, e1=0,e2=0;
double eu=0;
double kp2=0.5;
double base_speed=60;
double speed_r, speed_l;
double u=0, u_1=0, d_u=0;
int stage_index;
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

void sendchar(char varchar)
{
   int buf=varchar;
   fflush(stdout);
   write(serial,&buf,1);
   //printf("send number is: %d\n",val);
}

void processCam1(CvPoint pt)
{
  e= atan2((double)pt.x-80, (double)120-pt.y);
  speed_r = base_speed*cos(e) + kp*e;
  speed_l = base_speed*cos(e) - kp*e;
  int l=(int)speed_l+125;
  int r=(int)speed_r+125;
  printf("left: %5.2f; right: %5.2f\n",speed_l,speed_r);
  sendchar(253); 
  sendchar(l);
  sendchar(r);
  sendchar((l+r)%256);
}

void processCam2(CvPoint pt)//up and down
{
  eu=90-pt.y;
  speed =  kp2*eu;
  int l=(int)speed+125;
  printf("Up/Down: %5.2f\n",speed);
  sendchar(254); 
  sendchar(l);
//  sendchar(l)
  sendchar((l)%256);
}

void processOut()  //up and down
{  
   sendchar(254);
   sendchar(250);
  // sendchar(250);
   sendchar(250%256); //get out the pool
}
 int main() {
  CvPoint pt1b,pt2b, pt1t,pt2t,pt1,pt2,cir_center,pt1y,pt2y,y_cir_center,pt1r,pt2r,pt1l,pt2l,l_cir_center,r_cir_center,fb_center;
  int tempwidth,tempheight;
  fb_center.x=80;fb_center.y=120;
  CvRect regt,rectROIbot1,rectROItop1,rectROIlef1,rectROIrgt1;
  rectROItop1=cvRect(0,0,80,10);
  rectROIbot1=cvRect(0,50,80,10);
  rectROIlef1=cvRect(0,10,20,40);
  rectROIrgt1=cvRect(60,10,20,40);
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
  IplImage* threshframergt1;
  IplImage* threshframelef1;
  IplImage* modframe1,modframe2;
  IplImage* dilframetop1;
  IplImage* dilframe2;
  IplImage* dilframebot1;
  IplImage* dilframergt1;
  IplImage* dilframelef1;
  IplImage* dilframeyel1;
  int moddiv=2,seq=0,seqdiv=2;
  int release=0, rmax=100;
  int modfheight, modfwidth;
  unsigned char sendBuf;
  int checky[size],county=0;  
  int stage=1;
  int i,j;
  int sum;
  for (i=0;i<size;i++)
  {checky[i]=0;
  }
  int countout=0;
  int out[outsize];
  for (j=0;j<outsize;j++)
  {out[j]=0;
  }
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
   CvCapture* capture1 = cvCaptureFromCAM( 2 );
   CvCapture* capture2 = cvCaptureFromCAM( 1 );
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
     cvInRangeS(hsvframe1,black1,black2,threshframe1); //cvInRangeS(input frame, cvScalar(min range),cvScalar(max range),output frame)
      IplImage* threshyel1=cvCreateImage(cvGetSize(hsvframe1),8,1);
      cvInRangeS(hsvframe1,yel1,yel2,threshyel1); 
    
      threshframebot1=cvCloneImage(threshframe1);
      cvSetImageROI(threshframebot1,rectROIbot1);

      threshframetop1=cvCloneImage(threshframe1);
      cvSetImageROI(threshframetop1,rectROItop1);
      
      threshframergt1=cvCloneImage(threshframe1);
      cvSetImageROI(threshframergt1,rectROIrgt1);

      threshframelef1=cvCloneImage(threshframe1);
      cvSetImageROI(threshframelef1,rectROIlef1); 
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
     b_cir_center.y=(pt1b.y+pt2b.y)/2;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     threshframergt1=cvCloneImage(threshframe1);
     cvSetImageROI(threshframergt1,rectROIrgt1);
     dilframergt1 = cvCreateImage(cvGetSize(threshframergt1),8,1);
     cvDilate(threshframergt1,dilframergt1,NULL,2); //cvDilate(input frame,

     CBlobResult blobs_rgt1;
     blobs_rgt1 = CBlobResult(dilframergt1,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs_rgt1.Filter(blobs_rgt1,B_EXCLUDE,CBlobGetArea(),B_LESS,50);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob_rgt1;
     blobs_rgt1.GetNthBlob(CBlobGetArea(),0,biggestblob_rgt1); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1r.x = biggestblob_rgt1.MinX()*moddiv+120;
     pt1r.y = biggestblob_rgt1.MinY()*moddiv+20;
     pt2r.x = biggestblob_rgt1.MaxX()*moddiv+120;
     pt2r.y = biggestblob_rgt1.MaxY()*moddiv+20;
     r_cir_center.x=(pt1r.x+pt2r.x)/2;
     r_cir_center.y=(pt1r.y+pt2r.y)/2;
}
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
     t_cir_center.y=(pt1t.y+pt2t.y)/2;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      threshframelef1=cvCloneImage(threshframe1);
      cvSetImageROI(threshframelef1,rectROIlef1);
      dilframelef1 = cvCreateImage(cvGetSize(threshframelef1),8,1);
     cvDilate(threshframelef1,dilframelef1,NULL,2); //cvDilate(input frame,
     CBlobResult blobs_lef1;
     blobs_lef1 = CBlobResult(dilframelef1,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs_lef1.Filter(blobs_lef1,B_EXCLUDE,CBlobGetArea(),B_LESS,50);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob_lef1;
     blobs_lef1.GetNthBlob(CBlobGetArea(),0,biggestblob_lef1); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1l.x = biggestblob_lef1.MinX()*moddiv;
     pt1l.y = biggestblob_lef1.MinY()*moddiv+20;
     pt2l.x = biggestblob_lef1.MaxX()*moddiv;
     pt2l.y = biggestblob_lef1.MaxY()*moddiv+20;
     l_cir_center.x=(pt1l.x+pt2l.x)/2;
     l_cir_center.y=(pt1l.y+pt2l.y)/2;	
}
//////////////////////////////////////////////////////////////////////////////////////
   if(seq==seqdiv+2) 
   {
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
     cvRectangle(frame1,pt1r,pt2r,cvScalar(255,0,0),1,8,0);
     cvRectangle(frame1,pt1l,pt2l,cvScalar(255,0,0),1,8,0);
     cvRectangle(frame1,pt1b,pt2b,cvScalar(255,0,0),1,8,0); // draw rectangle around the biggest blob
     //cvRectangle(frame,pt1,pt2,cvScalar(255,0,0),1,8,0);
     //cvCircle( frame, b_cir_center, cir_radius, cvScalar(0,255,255), 1, 8, 0 ); // center point of the rectangle
     cvLine(frame1, A, B,cvScalar(255,0,255),2,8,0);
     cvLine(frame1, C, D,cvScalar(255,0,255),2,8,0);
    
    if(t_cir_center.x!=0&&t_cir_center.y!=0)
    {
      if(l_cir_center.x!=0&&l_cir_center.y!=20) // if left point exist
      {
        if (r_cir_center.x!=120&&r_cir_center.y!=20)  // left and right both exist
        {
         if (b_cir_center.x!=0&&b_cir_center.y!=100) // 4 points exist 
           {
           int outsum=0; 
//           printf("out loop\n");
           out[countout]=1;
           countout++; countout=countout%outsize;
           for(j=0;j<outsize;j++)
           outsum=outsum+out[j];
           if (outsum>=7)
              {printf("OUT!!!\t");
            //   sendchar(251);sendchar('o');sendchar('u');sendchar('t');
               for(j=0;j<outsize;j++)
                 {out[j]=0;}
              }
           }   
        }
       }
     out[countout]=0; countout++; countout=countout%outsize;
     cvLine(frame1, fb_center, t_cir_center,cvScalar(255,255,0),1,8,0);
     //sendchar(253);sendchar(t_cir_center.x);sendchar(t_cir_center.y);
     //sendchar(254);
     //printf("top:(%3d, %3d)\n",t_cir_center.x,t_cir_center.y);
     processCam1(t_cir_center);
     
    }
   else
   {  out[countout]=0; countout++; countout=countout%outsize;
      if(l_cir_center.x!=0&&l_cir_center.y!=20) // if left point exist
      {
        if (r_cir_center.x!=120&&r_cir_center.y!=20)  // left and right both exist
        {    
             if (l_cir_center.y<r_cir_center.y)  //compare y of left and right
             {//left higher
             cvLine(frame1, fb_center, l_cir_center,cvScalar(255,255,0),1,8,0);
             //sendchar(253);sendchar(l_cir_center.x);sendchar(l_cir_center.y);
             //sendchar(254);
             //printf("lef:(%3d, %3d)\n",l_cir_center.x,l_cir_center.y);
             processCam1(l_cir_center);
             }else{//right higher
             cvLine(frame1, fb_center, r_cir_center,cvScalar(255,255,0),1,8,0);
             //sendchar(253);sendchar(r_cir_center.x);sendchar(r_cir_center.y);
             //sendchar(254);
             //printf("rgt:(%3d, %3d)\n",r_cir_center.x,r_cir_center.y);
             processCam1(r_cir_center);
             }
       }else //only left exist
       {
        cvLine(frame1, fb_center, l_cir_center,cvScalar(255,255,0),1,8,0);
        //sendchar(253);sendchar(l_cir_center.x);sendchar(l_cir_center.y);
        //sendchar(254);
        //printf("lef:(%3d, %3d)\n",l_cir_center.x,l_cir_center.y);
        processCam1(l_cir_center);
       }
     }else // if left not exist
      {
       if(r_cir_center.x!=120&&r_cir_center.y!=20) // if right exit
         {// output right
       cvLine(frame1, fb_center, r_cir_center,cvScalar(255,255,0),1,8,0);
       //sendchar(253);sendchar(r_cir_center.x);sendchar(r_cir_center.y);
       //sendchar(254);
       //printf("rgt:(%3d, %3d)\n",r_cir_center.x,r_cir_center.y);
       processCam1(r_cir_center);
         }else if (b_cir_center.x!=0&&b_cir_center.y!=100)  // if right doesnot exit
           {
            cvLine(frame1, fb_center,b_cir_center,cvScalar(255,255,0),1,8,0);
            processCam1(b_cir_center);            
           }
      }
   }

     dilframeyel1 = cvCreateImage(cvGetSize(threshyel1),8,1);
     cvDilate(threshyel1,dilframeyel1,NULL,2); //cvDilate(input frame,
     CBlobResult blobs_yel1;
     blobs_yel1 = CBlobResult(dilframeyel1,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs_yel1.Filter(blobs_yel1,B_EXCLUDE,CBlobGetArea(),B_LESS,100);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob_yel1;
     blobs_yel1.GetNthBlob(CBlobGetArea(),0,biggestblob_yel1); //GetNthBlob(criteria, number, output) Get only the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1y.x = biggestblob_yel1.MinX()*moddiv;
     pt1y.y = biggestblob_yel1.MinY()*moddiv;
     pt2y.x = biggestblob_yel1.MaxX()*moddiv;
     pt2y.y = biggestblob_yel1.MaxY()*moddiv;
     cvRectangle(frame1,pt1y,pt2y,cvScalar(255,0,0),1,8,0);
     y_cir_center.x=(pt1y.x+pt2y.x)/2;
     y_cir_center.y=(pt1y.y+pt2y.y)/2;	
     if(y_cir_center.x!=0&&y_cir_center.y!=0)
     {   
        sum=0;
        printf("inside ");
        checky[county]=1;county++;
        county=county%size;
        for (i=0;i<size;i++)
         {sum=sum+checky[i];}
        if (sum>=(int)size*0.9)
         { 
          stage_index=1;
         }
         
     }else 
     {
      checky[county]=0;county++;county=county%size;
      sum = 0;
      for (i=0;i<size;i++)
         {sum=sum+checky[i];}
      if (stage_index==1&&sum<(int)size*0.2)
         {
           stage_index=0;
           printf("stage passed %d\n",stage);
           stage++;
          // sendchar(252);sendchar('s');sendchar('t');sendchar('g');
         }
     }
    // printf("county: %d\n",county);
 } // seq==2 
    seq++;
    seq=seq%(seqdiv+4);
     cvShowImage( "mywindow", frame1); // show output image
    // cvShowImage( "bot", threshframebot1);
    // cvShowImage( "top", threshframetop1);
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
     cvInRangeS(hsvframe2,Yel1,Yel2,threshframe2); //cvInRangeS(input frame, cvScalar(min range),cvScalar(max range),output frame)
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
     frame_center.y=frame_height/4*3;
     cvRectangle(frame2,pt1,pt2,cvScalar(255,0,0),1,8,0); // draw rectangle around the biggest blob
     cvCircle( frame2, cir_center, cir_radius, cvScalar(0,255,255), 1, 8, 0 ); // center point of the rectangle
     cvLine(frame2, A, B,cvScalar(255,0,255),2,8,0);
     cvLine(frame2, C, D,cvScalar(255,0,255),2,8,0);
     if (cir_center.x!=0&&cir_center.y!=0){
          //printf("%f, %f \n",angle*180/3.1416,spinsize/10);
	
     cvLine(frame2, cir_center, frame_center,cvScalar(0,255,0),1,8,0);
     processCam2(cir_center);
    
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


