#include<opencv2/opencv.hpp>
#include<iostream>
#include<stdio.h>
#include<unistd.h>
#include<math.h>
using namespace cv;

int v_lin[700],v_rot[700],orient=0;
double temp_orient;

int main(){
    Mat image(1000,1000, CV_8UC3, Scalar(255,255,255));
    Mat dst(1000,1000, CV_8UC3, Scalar(255,255,255));
    Mat par= Mat(2, 3, CV_64FC1);;

//*************************generate speeds****************************    
    for(int i=0;i<700;i++){
        v_lin[i]=5;
    }
    for(int i=0;i<200;i++){
        v_rot[i] = 0;
    }
    for(int i=200;i<400;i++){
        v_rot[i] = 1;
    }
        for(int i=400;i<700;i++){
        v_rot[i] = -1;
    }

    Point pt = Point(200,500); //init point
    for(int i=0;i<700;i++){
//**********************window translation**************************************
        if(pt.y<50){
            par.at<double>(0,0)=  1;  //p1
            par.at<double>(1,0)=  0;  //p2;
            par.at<double>(0,1)=  0; //p3;
            par.at<double>(1,1)=  1; //p4;
            par.at<double>(0,2)=  0;  //p5;
            par.at<double>(1,2)=  10;   //p6;
            warpAffine(image,dst,par, image.size(),INTER_LINEAR, BORDER_CONSTANT, Scalar(255,255,255));
            image = dst;
            pt.y = pt.y+10;          
        }else if(pt.y>950){
            par.at<double>(0,0)=  1;  //p1
            par.at<double>(1,0)=  0;  //p2;
            par.at<double>(0,1)=  0; //p3;
            par.at<double>(1,1)=  1; //p4;
            par.at<double>(0,2)=  0;  //p5;
            par.at<double>(1,2)=  -10;   //p6;
            warpAffine(image,dst,par, image.size(),INTER_LINEAR, BORDER_CONSTANT, Scalar(255,255,255));
            image = dst;
            pt.y = pt.y-10;         
        }else if(pt.x<50){
            par.at<double>(0,0)=  1;  //p1
            par.at<double>(1,0)=  0;  //p2;
            par.at<double>(0,1)=  0; //p3;
            par.at<double>(1,1)=  1; //p4;
            par.at<double>(0,2)=  10;  //p5;
            par.at<double>(1,2)=  0;   //p6;
            warpAffine(image,dst,par, image.size(),INTER_LINEAR, BORDER_CONSTANT, Scalar(255,255,255));
            image = dst;
            pt.x = pt.x+10;          
        }else if(pt.x>950){
            par.at<double>(0,0)=  1;  //p1
            par.at<double>(1,0)=  0;  //p2;
            par.at<double>(0,1)=  0; //p3;
            par.at<double>(1,1)=  1; //p4;
            par.at<double>(0,2)=  -10;  //p5;
            par.at<double>(1,2)=  0;   //p6;
            warpAffine(image,dst,par, image.size(),INTER_LINEAR, BORDER_CONSTANT, Scalar(255,255,255));
            image = dst;
            pt.x = pt.x-10;        
        }
//*************************************************************************************************
        circle(image, pt, 6, Scalar(255,255,255), FILLED, LINE_8); //clear previous positios
        circle(image, pt, 6, Scalar(0,102,204), FILLED, LINE_8); // print trajectory

//************************************calculate new positions**********************
        orient = orient + v_rot[i];
        temp_orient = orient * M_PI / 180; //radians
        pt.x = (int)(pt.x + v_lin[i] * cos(temp_orient));
        pt.y = (int)(pt.y - v_lin[i] * sin(temp_orient));
//************************************PRINT NEW POSITION***************************
        circle(image, pt, 6, Scalar(0,0,0), FILLED, LINE_8);
        imshow("Start project with opencv", image);
        waitKey(10); //delay 10ms
    } //end simulation
    waitKey(0);
    return 0;
}