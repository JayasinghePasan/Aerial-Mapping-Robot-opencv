
 //opencv
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "dataPoint.h"

using namespace std;
using namespace cv;

 //Calibration Functions
void manualColorPicker(Mat img);
void manualWarpPicker(Mat img);

int worldd_size = 500;
Mat worldmap = cv::Mat::zeros(worldd_size, worldd_size, CV_32SC1);



int main()
{
	// ------- Color Calibration ----------
	if (false)
	{
		Mat img = imread("C:/Users/pasan/Desktop/MarsSimulator/Mars Photo Analiser/MappingRobot/MappingRobot/IMG/robocam_2023_12_18_16_28_53_857.jpg");
		manualColorPicker(img);
		return 0;
	}

	 //------- Warp Calibration -----------
	if (false)
	{
		Mat img = imread("C:/Users/pasan/Desktop/MarsSimulator/Mars Photo Analiser/MappingRobot/MappingRobot/IMG/Calibration/grid.jpg");
		manualWarpPicker(img);
		return 0;
	}

	const string csvPath = "IMG/robot_log.csv";
	ifstream csvFile(csvPath);

	if (!csvFile.is_open()) { cout << "Failed to open CSV file !!"; return 1; }


	 //Reading lines
	string line;
	bool firstLine = true;

	while (getline(csvFile, line))
	{
		 //To skip the first line
		if (firstLine) { firstLine = false;  continue; }

		dataPoint*pnt = new dataPoint(line);
		pnt->superImpose(worldmap);
		delete pnt;
	}

	cv::imshow("Final World Map", worldmap);
	cv::waitKey(0);

	return 0;
}






void manualColorPicker(Mat img)
{
	Mat imgHSV, mask, imgColor;
	int hmin = 0, smin = 0, vmin = 0;
	int hmax = 179, smax = 255, vmax = 255;

	namedWindow("TrackBars", (640, 200));
	createTrackbar("Hue Min", "TrackBars", &hmin, 179);
	createTrackbar("Sat Min", "TrackBars", &smin, 179);
	createTrackbar("Val Min", "TrackBars", &vmin, 255);

	createTrackbar("Hue Max", "TrackBars", &hmax, 255);
	createTrackbar("Sat Max", "TrackBars", &smax, 255);
	createTrackbar("Val Max", "TrackBars", &vmax, 255);

	while (true)
	{
		cvtColor(img, imgHSV, COLOR_BGR2HSV);

		Scalar lower(hmin, smin, vmin);
		Scalar upper(hmax, smax, vmax);

		inRange(imgHSV, lower, upper, mask);
		 hmin, smin, vmin, hmax, smax, vmax;
		imshow("Image", img);
		imshow("Mask", mask);
		waitKey(0);
	}
}

void manualWarpPicker(Mat imgFloor)
{
	Mat imgWarp;

	float x_off = 40;
	float y_off = 40;

	Point2f srcPnts[4] = { {14,140}, {118,96}, {200,96}, {302,140} };
	Point2f dstPnts[4] =
	{
		{static_cast<float>(imgFloor.cols / 2.0 - x_off),static_cast<float>(imgFloor.rows / 2.0 + y_off)},
		{static_cast<float>(imgFloor.cols / 2.0 - x_off),static_cast<float>(imgFloor.rows / 2.0 - y_off)},
		{static_cast<float>(imgFloor.cols / 2.0 + x_off),static_cast<float>(imgFloor.rows / 2.0 - y_off)},
		{static_cast<float>(imgFloor.cols / 2.0 + x_off),static_cast<float>(imgFloor.rows / 2.0 + y_off)}
	};

	Mat perspect_matrix = getPerspectiveTransform(srcPnts, dstPnts);
	warpPerspective(imgFloor, imgWarp, perspect_matrix, imgWarp.size());

	imshow("Image", imgFloor);
	imshow("Warp", imgWarp);
	waitKey(0);
}