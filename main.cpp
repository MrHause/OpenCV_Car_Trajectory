#include<opencv2/opencv.hpp>
#include<iostream>
#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include<string>
#include<fstream>
using namespace cv;



Point pt = Point(200,500); //main point
Mat image(1000,1000, CV_8UC3, Scalar(255,255,255)); //full window
Mat dst(1000,1000, CV_8UC3, Scalar(255,255,255)); //temp window to perfor functions
Point circle_pt = Point(200,400); //point for clearing position (punkt narożnika wyświetlającego obraz auta)
Point pt2 = Point(200,420); //init point for clearing rectangle (przeciwległy narożnik)
Point trajectory_pt = Point(200,400);

static Mat car;
Mat trajectory;
Mat car_ref; 

int thickness =-1,lineType=8, shift=0;
float distance;
int x_trans,y_trans;

void sim_init(){
    car = imread("auto.png",1);
    car_ref = car;
    trajectory_pt.x = 200;
    trajectory_pt.y = 400+car.rows/2;
    distance = car.rows/2;
}

//rotate object of the car based on rot angle of the car
Mat rotate(Mat src, double angle, int *x_pos, int *y_pos)
{
    Mat dst;
    Point2f pt(src.cols/2., src.rows/2.);  
    //Point2f pt(0,0);  
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    Rect bbox = RotatedRect(Point2f(), src.size(), angle).boundingRect2f();
    // adjust transformation matrix
    r.at<double>(0,2) += bbox.width/2.0 - src.cols/2.0;
    r.at<double>(1,2) += bbox.height/2.0 - src.rows/2.0;
    warpAffine(src, dst, r, bbox.size(),INTER_LINEAR, BORDER_CONSTANT, Scalar(255,255,255));
    *x_pos = r.at<double>(0,2);
    *y_pos = r.at<double>(1,2);
    return dst;
}

//function to translate window when car is close to edge
Mat translate_window(Mat image, Point &pt, Mat car, float &pos_x, float &pos_y){
        Mat dst;
        static int x_right,y_top,x_lef,y_down;
        Mat par= Mat(2, 3, CV_64FC1);
        dst = image;
            if(pt.y<120){
            par.at<double>(0,0)=  1;  //p1
            par.at<double>(1,0)=  0;  //p2;
            par.at<double>(0,1)=  0; //p3;
            par.at<double>(1,1)=  1; //p4;
            par.at<double>(0,2)=  0;  //p5;
            par.at<double>(1,2)=  10;   //p6;
            warpAffine(image,dst,par, image.size(),INTER_LINEAR, BORDER_CONSTANT, Scalar(255,255,255));
            image = dst;
            pt.y = pt.y+10;
            pos_y = pos_y+10;          
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
            pos_y = pos_y-10;        
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
            pos_x = pos_x+10;
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
            pos_x = pos_x-10;
        }
        return image;
}

int show_position(float read_x, float read_y, float read_roll){
        double temp_orient;
        float diff;
        static float prev_x,prev_y;
        static float pos_x,pos_y;
        static int count;
        count++;
        //calculate cleating posiotions
        circle_pt.x = pt.x-x_trans;
        circle_pt.y = pt.y-y_trans;
        pt2.x = circle_pt.x+car.cols;
        pt2.y = circle_pt.y+car.rows;
        rectangle(image, circle_pt, pt2, Scalar(255,255,255),thickness,lineType,shift); //clear previous position
        //rectangle(image, pt, pt2, Scalar(255,255,255),thickness,lineType,shift); //clear previous position      
        if(count==1){ //init
            pt.x = 500 + read_x*10;
            pt.y = 400 + read_y*10;
            pos_x = pt.x;
            pos_y = pt.y;
        }else{
            diff = read_x*10 - prev_x;
            pos_x += diff;
            pt.x = pos_x;
            diff = read_y*10 - prev_y;

            pos_y += diff;
            pt.y = pos_y;
            prev_x = read_x*10;
            prev_y = read_y*10;  
        }
        
        image = translate_window(image, pt, car, pos_x,pos_y);
        pt.x = pos_x;
        pt.y = pos_y;

        temp_orient = read_roll* M_PI / 180; //radians
        trajectory_pt.x = distance*sin(temp_orient) + pt.x;
        trajectory_pt.y = distance*cos(temp_orient) + pt.y;
        circle(image, trajectory_pt, 32, Scalar(0,102,204), FILLED, LINE_8); // print trajectory

        car = rotate(car_ref,read_roll, &x_trans, &y_trans);
        car.copyTo(image(Rect(pt.x-x_trans,pt.y-y_trans,car.cols,car.rows)));
        //car.copyTo(image(Rect(pt.x,pt.y,car.cols,car.rows)));
        imshow("Start project with opencv", image);
        waitKey(1);

        return -1;
    }
    


int main(){
    sim_init();

//*****************************read data******************
    std::ifstream read_file("randomFullModelData.csv");
    if(!read_file.is_open()) std::cout<<"erro file";
    std::string read_x,read_y,read_roll,read_temp;

//*************************generate speeds****************************    
/*
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
    */
//**********************Load CAR IMAGE************************************

//***********************************************************************




    while(read_file.peek()!=EOF){
    //*************************get data**************************
        getline(read_file,read_temp,',');
        getline(read_file,read_temp,',');
        getline(read_file,read_temp,',');
        getline(read_file,read_temp,',');
        getline(read_file,read_temp,',');
        getline(read_file,read_temp,',');
        getline(read_file,read_temp,',');
        getline(read_file,read_temp,',');
        getline(read_file,read_temp,',');
        getline(read_file,read_temp,',');
        getline(read_file,read_temp,',');
        getline(read_file,read_temp,',');

        getline(read_file,read_x,',');
        getline(read_file,read_y,',');
        getline(read_file,read_roll,'\n');

        show_position(std::stof(read_x),std::stof(read_y),std::stof(read_roll));
    }
        //std::cout<<"x: "<<read_x<<std::endl;
        //std::cout<<"y: "<<read_y<<std::endl;
//*************************************************************

/*
    for(int i=0;i<700;i++){
//************************************Clear PREVIOUS POSITIONS************************************
        circle_pt.x = pt.x-x_trans;
        circle_pt.y = pt.y-y_trans;
        pt2.x = circle_pt.x+car.cols;
        pt2.y = circle_pt.y+car.rows;
        rectangle(image, circle_pt, pt2, Scalar(255,255,255),thickness,lineType,shift); //clear previous position
//**********************window translation**************************************
       //image = translate_window(image, pt, car);
//*************************************************************************************************
//************************************calculate new positions*************************************
        orient = orient + v_rot[i];
        temp_orient = orient * M_PI / 180; //radians
        trajectory_pt.x = distance*sin(temp_orient) + pt.x;
        trajectory_pt.y = distance*cos(temp_orient) + pt.y;
        circle(image, trajectory_pt, 32, Scalar(0,102,204), FILLED, LINE_8); // print trajectory

        pt.x = (int)(pt.x + v_lin[i] * cos(temp_orient));
        pt.y = (int)(pt.y - v_lin[i] * sin(temp_orient));
        car = rotate(car_ref,orient, &x_trans, &y_trans);
        
//**************************************************************************************************
//************************************PRINT NEW POSITION*******************************************
        car.copyTo(image(Rect(pt.x-x_trans,pt.y-y_trans,car.cols,car.rows)));
        //circle(image, pt, 6, Scalar(0,0,0), FILLED, LINE_8);
        
        imshow("Start project with opencv", image);
        waitKey(10); //delay 10ms
    } //end simulation
    */
    waitKey(0);
    return 0;
}