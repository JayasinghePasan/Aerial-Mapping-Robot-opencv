#pragma once

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

using namespace std;
using namespace cv;

class dataPoint
{
private:
	string Path;
	double SteerAngle, Throttle, Brake, Speed;
	double X_pos, Y_pos;
	double Pitch, Yaw, Roll;


	// color range for color detect
	int hMin = 0, sMin = 0, vMin = 194;
	int hMax = 255, sMax = 255, vMax = 255;

	// initial warp points for warp perspective
	Point2f srcPnts[4] = {{14,140}, {118,96}, {200,96}, {302,140}};

	void colorDetect(Mat &img, Mat& imgFloor);
	void warp(Mat &imgFloor, Mat &imgWarp);

	pair<vector<float>, vector<float>> rover_coords(const Mat& binary_img);
	pair<vector<float>, vector<float>> rotate_pix(const vector<float>& xpix, const vector<float>& ypix, float yaw);
	pair<vector<float>, vector<float>> translate_pix(const vector<float>& xpix_rot, const vector<float>& ypix_rot, float xpos, float ypos, float scale);

public:
	// constructor
	dataPoint(string line);
	void superImpose(Mat &map);

	
};

