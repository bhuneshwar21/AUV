Color-Track
===========

color tracking using opencv in c++

This project contains codes to perform color tracking in ubuntu. It has been tested on Xubuntu 12.10 with OpenCV 2.3.1 with the cvbloblib.

Please download the blob extraction library at 

opencv.willowgarage.com/wiki/cvBlobsLib

And add the library as required. 

colortest1.cpp : C++ code for colortracking
build_all.sh : bash command for calling compiler to compile all .c and .cpp files in folder. 

Run procedure

Copy build_all.sh and colortest1.cpp to a folder of your choice

For linux users

Open a terminal at the foler.
./build_all.sh
./colortest1


If the above step fails, make sure you have the required permission in the folder and the file is set to executable
Terminal will show an error for *.c files. Ignore this as there are no .c files in the folder. 
There should not be any errors regarding .cpp files
You will find an executable named colortest1 in the folder

Project details at 
https://sites.google.com/site/narasimhaweb
