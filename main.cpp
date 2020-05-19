#include<opencv2/opencv.hpp>
#include<iostream>
#include<stdio.h>
#include<unistd.h>
#include<math.h>
using namespace cv;

int v_lin[700],v_rot[700],orient=0;
double temp_orient;

Mat rotate(Mat src, double angle)
{
    Mat dst;
    Point2f pt(src.cols/2., src.rows/2.);    
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    Rect2f bbox = RotatedRect(Point2f(), src.size(), angle).boundingRect2f();
    // adjust transformation matrix
    r.at<double>(0,2) += bbox.width/2.0 - src.cols/2.0;
    r.at<double>(1,2) += bbox.height/2.0 - src.rows/2.0;
    warpAffine(src, dst, r, bbox.size(),INTER_LINEAR, BORDER_CONSTANT, Scalar(255,255,255));
    return dst;
}

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
//**********************Load CAR IMAGE************************************
    Mat car;
    Mat car_ref;
    Mat trajectory;
    car = imread("auto.png",1);
    car_ref = car; 
//***********************************************************************
    Point pt = Point(200,500); //init point
    Point circle_pt = Point(pt.x+(int)car.cols/2,pt.y+(int)car.rows/2);
    Point pt2 = Point(200,490); //init point for clearing rectangle
    int thickness =-1,lineType=8, shift=0;
    float distance = car.rows/2;
    for(int i=0;i<700;i++){
//************************TESTING*********************************************
        
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
        }else if(pt.y>(1000-car.rows-20)){
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
        }else if(pt.x>(1000-car.cols-50)){
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
//************************************Clear PREVIOUS POSITIONS************************************
        pt2.x = pt.x+car.cols;
        pt2.y = pt.y+car.rows;
//***********************************CALCUTE TRAJECTORY POINT**********************************
        circle_pt.x = pt.x;
        circle_pt.y = pt.y + distance;
        rectangle(image, pt, pt2, Scalar(255,255,255),thickness,lineType,shift);
        //circle(image, pt, 6, Scalar(0,102,204), FILLED, LINE_8); // print trajectory
//*************************************************************************************************
//************************************calculate new positions*************************************
        
        orient = orient + v_rot[i];
        temp_orient = orient * M_PI / 180; //radians
        pt.x = (int)(pt.x + v_lin[i] * cos(temp_orient));
        pt.y = (int)(pt.y - v_lin[i] * sin(temp_orient));
        car = rotate(car_ref,orient);
//**************************************************************************************************
//************************************PRINT NEW POSITION*******************************************
        car.copyTo(image(Rect(pt.x,pt.y,car.cols,car.rows)));
        circle(image, pt, 6, Scalar(0,0,0), FILLED, LINE_8);
        imshow("Start project with opencv", image);
        waitKey(10); //delay 10ms
    } //end simulation
    waitKey(0);
    return 0;
}