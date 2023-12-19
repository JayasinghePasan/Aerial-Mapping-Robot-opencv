#include "dataPoint.h"

dataPoint::dataPoint(string line)
{
	string column;
	vector <string> columns;
	istringstream lineStream(line);
	while (getline(lineStream, column, ';'))
	{
		columns.push_back(column);
	}
	
	Path = columns[0];

	SteerAngle =stod(columns[1]);

	Throttle = stod(columns[2]); 
	Brake    = stod(columns[3]);
	Speed    = stod(columns[4]);
	
	X_pos    = stod(columns[5]);
	Y_pos    = stod(columns[6]);
	
	Pitch    = stod(columns[7]);
	Yaw      = stod(columns[8]); 
	Roll     = stod(columns[9]);

}


void dataPoint::superImpose(Mat &worldmap)
{
	Mat img;
	img = imread(Path);
	
	imshow("Image", img); 
	waitKey(1);
	
	Mat imgFloor = Mat::zeros(img.size(), CV_8UC1);
	Mat imgWarp  = Mat::zeros(imgFloor.size(), CV_8UC1);

	colorDetect(img, imgFloor);
	imshow("Floor", imgFloor);
	waitKey(1);

	warp(imgFloor, imgWarp);
	imshow("Bird Eye", imgWarp);
	waitKey(1);

	// get non-zero points as vectors so that we can apply rotation and translation operations
	pair<vector<float>, vector<float>> rover_coords_result = rover_coords(imgWarp);
	vector<float> xpix = rover_coords_result.first;
	vector<float> ypix = rover_coords_result.second;


	// Rotation
	pair<vector<float>, vector<float>> rotate_result    = rotate_pix(xpix, ypix, SteerAngle);
	
	// Translation
	float scale = 10;
	pair<vector<float>, vector<float>> translate_result = translate_pix(rotate_result.first, rotate_result.second, X_pos, Y_pos, scale);

	std::vector<float> x_world = translate_result.first;
	std::vector<float> y_world = translate_result.second;


	// Clip to world_size
	for (size_t i = 0; i < x_world.size(); ++i) {
		x_world[i] = max(0, min(worldmap.rows - 1, static_cast<int>(x_world[i])));
		y_world[i] = max(0, min(worldmap.rows - 1, static_cast<int>(y_world[i])));
	}

	// Add pixel positions to worldmap
	for (size_t i = 0; i < x_world.size(); ++i) {
		worldmap.at<int>(y_world[i], x_world[i]) += 1;
	}

	cv::Mat worldmap_display;
	cv::convertScaleAbs(worldmap, worldmap_display);
	cv::imshow("Final World Map", worldmap_display);
}

// Apply color filter to identify floor area
void dataPoint::colorDetect(Mat &img, Mat &imgFloor)
{
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	//color range for color detect
	Scalar lower(hMin, sMin, vMin);
	Scalar upper(hMax, sMax, vMax);

	inRange(imgHSV, lower, upper, imgFloor);
}

// Warp the image to birds eye view
void dataPoint::warp(Mat& imgFloor, Mat& imgWarp)
{
	float x_off = 40;
	float y_off = 40;
	Point2f dstPnts[4] =
	{
		{static_cast<float>(imgFloor.cols / 2.0 - x_off),static_cast<float>(imgFloor.rows / 2.0 + y_off)},
		{static_cast<float>(imgFloor.cols / 2.0 - x_off),static_cast<float>(imgFloor.rows / 2.0 - y_off)},
		{static_cast<float>(imgFloor.cols / 2.0 + x_off),static_cast<float>(imgFloor.rows / 2.0 - y_off)},
		{static_cast<float>(imgFloor.cols / 2.0 + x_off),static_cast<float>(imgFloor.rows / 2.0 + y_off)}
	};

	Mat perspect_matrix = getPerspectiveTransform(srcPnts, dstPnts);
	warpPerspective(imgFloor, imgWarp, perspect_matrix, imgWarp.size());
}

// Convert rover space coordinates to world coordinates
pair<vector<float>, vector<float>> dataPoint::rover_coords(const Mat &imgWarp) 
{
	Mat nonzero_pixels;
	findNonZero(imgWarp, nonzero_pixels);

	vector<float> x_pixel(nonzero_pixels.rows), y_pixel(nonzero_pixels.rows);
	
	for (int i = 0; i < nonzero_pixels.rows; ++i) 
	{
		x_pixel[i] = static_cast<float>(imgWarp.rows - nonzero_pixels.at<Point>(i).y);
		y_pixel[i] = static_cast<float>(nonzero_pixels.at<Point>(i).x - imgWarp.cols / 2);
	}

	return make_pair(x_pixel, y_pixel);
}

// Rotate nonzero points
pair<vector<float>, vector<float>> dataPoint::rotate_pix(const vector<float>&xpix, const vector<float>&ypix, float yaw)
{
	// Convert yaw to radians
	float yaw_rad = yaw * CV_PI / 180;

	// Apply rotation
	vector<float> xpix_rotated(xpix.size()), ypix_rotated(ypix.size());
	for (size_t i = 0; i < xpix.size(); ++i) {
		xpix_rotated[i] = xpix[i] * cos(yaw_rad) - ypix[i] * sin(yaw_rad);
		ypix_rotated[i] = xpix[i] * sin(yaw_rad) + ypix[i] * cos(yaw_rad);
	}

	return make_pair(xpix_rotated, ypix_rotated);
}

// Add translation
pair<vector<float>, vector<float>> dataPoint::translate_pix(const vector<float>&xpix_rot, const vector<float>&ypix_rot, float xpos, float ypos, float scale) {

	vector<float> xpix_translated(xpix_rot.size()), ypix_translated(ypix_rot.size());
	for (size_t i = 0; i < xpix_rot.size(); ++i) {
		xpix_translated[i] = xpix_rot[i] / scale + xpos;
		ypix_translated[i] = ypix_rot[i] / scale + ypos;
	}

	return make_pair(xpix_translated, ypix_translated);
}