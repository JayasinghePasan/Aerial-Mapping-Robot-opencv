//#include <iostream>
//#include <opencv2/opencv.hpp>
//
//// Define the function to perform perspective transform
//cv::Mat perspect_transform(const cv::Mat& img, const std::vector<cv::Point2f>& src, const std::vector<cv::Point2f>& dst) {
//    // Get transform matrix using cv::getPerspectiveTransform()
//    cv::Mat M = cv::getPerspectiveTransform(src, dst);
//
//    // Warp image using cv::warpPerspective()
//    cv::Mat warped;
//    cv::warpPerspective(img, warped, M, img.size(), cv::INTER_LINEAR);
//
//    // Return the result
//    return warped;
//}
//
//// Define the function for color thresholding
//cv::Mat color_thresh(const cv::Mat& img, const std::vector<int>& rgb_thresh) {
//    // Create an array of zeros, single channel
//    cv::Mat color_select = cv::Mat::zeros(img.size(), CV_8UC1);
//
//    // Require that each pixel be above all three threshold values in RGB
//    cv::Mat above_thresh = (img > cv::Scalar(rgb_thresh[0], rgb_thresh[1], rgb_thresh[2]));
//
//    // Index the array of zeros with the boolean array and set to 255
//    color_select.setTo(255, above_thresh);
//
//    // Return the binary image
//    return color_select;
//}
//
//// Define the function to convert rover space pixels to world space
//std::pair<std::vector<float>, std::vector<float>> rover_coords(const cv::Mat& binary_img) {
//    // Identify nonzero pixels
//    std::vector<int> ypos, xpos;
//    cv::findNonZero(binary_img, ypos);
//
//    // Calculate pixel positions with reference to the rover position being at the center bottom of the image.
//    std::vector<float> x_pixel(ypos.size()), y_pixel(ypos.size());
//    for (size_t i = 0; i < ypos.size(); ++i) {
//        x_pixel[i] = static_cast<float>(binary_img.rows - ypos[i]);
//        y_pixel[i] = static_cast<float>(xpos[i] - binary_img.cols / 2);
//    }
//
//    return std::make_pair(x_pixel, y_pixel);
//}
//
//// Define the function to rotate pixels
//std::pair<std::vector<float>, std::vector<float>> rotate_pix(const std::vector<float>& xpix, const std::vector<float>& ypix, float yaw) {
//    // Convert yaw to radians
//    float yaw_rad = yaw * CV_PI / 180;
//
//    // Apply rotation
//    std::vector<float> xpix_rotated(xpix.size()), ypix_rotated(ypix.size());
//    for (size_t i = 0; i < xpix.size(); ++i) {
//        xpix_rotated[i] = xpix[i] * cos(yaw_rad) - ypix[i] * sin(yaw_rad);
//        ypix_rotated[i] = xpix[i] * sin(yaw_rad) + ypix[i] * cos(yaw_rad);
//    }
//
//    return std::make_pair(xpix_rotated, ypix_rotated);
//}
//
//// Define the function to translate pixels
//std::pair<std::vector<float>, std::vector<float>> translate_pix(const std::vector<float>& xpix_rot, const std::vector<float>& ypix_rot, float xpos, float ypos, float scale) {
//    // Apply scaling and translation
//    std::vector<float> xpix_translated(xpix_rot.size()), ypix_translated(ypix_rot.size());
//    for (size_t i = 0; i < xpix_rot.size(); ++i) {
//        xpix_translated[i] = xpix_rot[i] / scale + xpos;
//        ypix_translated[i] = ypix_rot[i] / scale + ypos;
//    }
//
//    return std::make_pair(xpix_translated, ypix_translated);
//}
//
//int main() {
//    // Read the sample image
//    cv::Mat image = cv::imread("sample.jpg");
//
//    // Other parameters
//    float rover_yaw = static_cast<float>(std::rand() % 360);
//    float rover_xpos = static_cast<float>(std::rand() % 160 + 20);
//    float rover_ypos = static_cast<float>(std::rand() % 160 + 20);
//
//    // Calibration box coordinates
//    std::vector<cv::Point2f> source = { {14, 140}, {301, 140}, {200, 96}, {118, 96} };
//    std::vector<cv::Point2f> destination = { {image.cols / 2 - 5, image.rows - 6},
//                                             {image.cols / 2 + 5, image.rows - 6},
//                                             {image.cols / 2 + 5, image.rows - 16},
//                                             {image.cols / 2 - 5, image.rows - 16} };
//
//    // Perform warping and color thresholding
//    cv::Mat warped = perspect_transform(image, source, destination);
//    cv::Mat colorsel = color_thresh(warped, { 160, 160, 160 });
//
//    // Extract navigable terrain pixels
//    std::pair<std::vector<float>, std::vector<float>> rover_coords_result = rover_coords(colorsel);
//    std::vector<float> xpix = rover_coords_result.first;
//    std::vector<float> ypix = rover_coords_result.second;
//
//    // Other parameters
//    float scale = 10;
//    int world_size = 200;
//
//    // Get navigable pixel positions in world coords
//    std::pair<std::vector<float>, std::vector<float>> rotate_result = rotate_pix(xpix, ypix, rover_yaw);
//    std::pair<std::vector<float>, std::vector<float>> translate_result = translate_pix(rotate_result.first, rotate_result.second, rover_xpos, rover_ypos, scale);
//
//    std::vector<float> x_world = translate_result.first;
//    std::vector<float> y_world = translate_result.second;
//
//    // Clip to world_size
//    for (size_t i = 0; i < x_world.size(); ++i) {
//        x_world[i] = std::max(0, std::min(world_size - 1, static_cast<int>(x_world[i])));
//        y_world[i] = std::max(0, std::min(world_size - 1, static_cast<int>(y_world[i])));
//    }
//
//    // Add pixel positions to worldmap
//    cv::Mat worldmap = cv::Mat::zeros(world_size, world_size, CV_32SC1);
//    for (size_t i = 0; i < x_world.size(); ++i) {
//        worldmap.at<int>(y_world[i], x_world[i]) += 1;
//    }
//
//    // Print results
//    std::cout << "Xpos = " << rover_xpos << " Ypos = " << rover_ypos << " Yaw = " << rover_yaw << std::endl;
//
//    return 0;
//}
