#include "cv.h" 
#include "highgui.h" 
#include <cvblob/BlobResult.h>
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

 int main() {
  CvPoint pt1,pt2;
  CvRect regt;
  CvCapture* capture = cvCaptureFromCAM(0);
//CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);
   if ( !capture ) {
     fprintf(stderr, "ERROR: capture is NULL \n" );
     getchar();
     return -1;
   }
   // Create a window in which the captured images will be presented
   //cvNamedWindow( "mywindow", CV_WINDOW_AUTOSIZE );
int serial;

serial = openSerial("/dev/ttyACM0");
if( serial == -1 ) {
return -1;
}
   // Show the image captured from the camera in the window and repeat
   while ( 1 ) {
     // Get one frame
     IplImage* frame = cvQueryFrame( capture );
     if ( !frame ) {
       fprintf( stderr, "ERROR: frame is null...\n" );
       getchar();
       break;
     }

     int modfheight, modfwidth,A,B,X_Cord,Y_Cord;
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
     cvInRangeS(hsvframe,cvScalar(30,25, 150),cvScalar(60, 90, 220),threshframe); //cvInRangeS(input frame, cvScalar(min range),cvScalar(max range),output frame)
     // created dilated image
     IplImage* dilframe = cvCreateImage(cvGetSize(threshframe),8,1);
     cvDilate(threshframe,dilframe,NULL,2); //cvDilate(input frame, output frame, mask, number of times to dilate)

     CBlobResult blobs; 
     blobs = CBlobResult(dilframe,NULL,0); // CBlobresult(inputframe, mask, threshold) Will filter all white parts of image
     blobs.Filter(blobs,B_EXCLUDE,CBlobGetArea(),B_LESS,50);//blobs.Filter(input, cond, criteria, cond, const) Filter all images whose area is less than 50 pixels
     CBlob biggestblob;
     blobs.GetNthBlob(CBlobGetArea(),0,biggestblob); //GetNthBlob(criteria, number, output) Get only  the largest blob based on CblobGetArea()
     // get 4 points to define the rectangle
     pt1.x = biggestblob.MinX()*4;
     pt1.y = biggestblob.MinY()*4;
     pt2.x = biggestblob.MaxX()*4;
     pt2.y = biggestblob.MaxY()*4;
     cvRectangle(frame,pt1,pt2,cvScalar(255,0,0),1,8,0); // draw rectangle around the biggest blob

 // Servo actuation
	A = modfheight;
	B = modfwidth;
	X_Cord = ((B/2)-(pt1.x+((pt2.x-pt1.x)/2)));
	Y_Cord = ((A/2)-(pt1.y+((pt2.y-pt1.y)/2)));
	char sendBuf;
//printf("%d",pt1.x);
if((abs(X_Cord)>((pt2.x-pt1.x)/2)) && biggestblob.Area() > 0.5){
	X_Cord = X_Cord > 0 ? 30 : 10;
}else{
X_Cord = 20;
}
if((abs(Y_Cord)>((pt2.y-pt1.y)/2))&& biggestblob.Area() > 0.5) {
	Y_Cord = Y_Cord < 0 ? 3 : 1;
}else{
Y_Cord = 2;
}
	if( X_Cord != 20 || Y_Cord != 2 ) {

sendBuf = X_Cord + Y_Cord;

write( serial, &sendBuf, 1 );

}



     cvShowImage( "mywindow", frame); // show output image
     // Do not release the frame!
     //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
     //remove higher bits using AND operator
     if ( (cvWaitKey(10) & 255) == 27 ) break;
   }
   // Release the capture device housekeeping
close( serial );
   cvReleaseCapture( &capture );
   cvDestroyWindow( "mywindow" );
   return 0;
 }
