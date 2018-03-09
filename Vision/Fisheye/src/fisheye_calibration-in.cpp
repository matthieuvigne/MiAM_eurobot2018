/// \file fisheye_calibration.cpp
/// \author Rodolphe Dubois
/// \version 1.0
/// \date 07/03/2018
/// \brief Example for the calibration of one fisheye camera from
/// input images with chessboards. The code below is inspired from
///	sourisgh/fisheye-stereo-calibration and from
/// http://aishack.in/tutorials/calibrating-undistorting-opencv-oh-yeah/

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main(){
	
	//!Load the images
	std::cout << "Loading the images...";
	std::vector<cv::Mat> img_list;
	img_list.push_back(cv::imread( "${CMAKE_SOURCE_DIR}/data/calibration_images/chessboard_resized-0.jpg", CV_LOAD_IMAGE_COLOR));
	img_list.push_back(cv::imread( "${CMAKE_SOURCE_DIR}/data/calibration_images/chessboard_resized-1.jpg", CV_LOAD_IMAGE_COLOR));
	img_list.push_back(cv::imread( "${CMAKE_SOURCE_DIR}/data/calibration_images/chessboard_resized-2.jpg", CV_LOAD_IMAGE_COLOR));
	img_list.push_back(cv::imread( "${CMAKE_SOURCE_DIR}/data/calibration_images/chessboard_resized-3.jpg", CV_LOAD_IMAGE_COLOR));
	img_list.push_back(cv::imread( "${CMAKE_SOURCE_DIR}/data/calibration_images/chessboard_resized-4.jpg", CV_LOAD_IMAGE_COLOR));
	img_list.push_back(cv::imread( "${CMAKE_SOURCE_DIR}/data/calibration_images/chessboard_resized-5.jpg", CV_LOAD_IMAGE_COLOR));
	img_list.push_back(cv::imread( "${CMAKE_SOURCE_DIR}/data/calibration_images/chessboard_resized-6.jpg", CV_LOAD_IMAGE_COLOR));
	img_list.push_back(cv::imread( "${CMAKE_SOURCE_DIR}/data/calibration_images/chessboard_resized-7.jpg", CV_LOAD_IMAGE_COLOR));
	img_list.push_back(cv::imread( "${CMAKE_SOURCE_DIR}/data/calibration_images/chessboard_resized-8.jpg", CV_LOAD_IMAGE_COLOR));
	int numBoards = img_list.size();
	std::cout << "OK" << std::endl;

	//! Convert images to grayscale
	std::cout << "Convert images to grayscale...";
	std::vector<cv::Mat> gray_img_set;
	for( cv::Mat& image : img_list ){
		cv::Mat gray;
		cv::cvtColor(image,gray,cv::COLOR_BGR2GRAY);
		gray_img_set.push_back(gray);
	}
	std::cout << "OK" << std::endl;

	//! Chessboard characteristics
	const int board_width = 7;
	const int board_height = 9;
	const float square_size = 0.022;
	cv::Size board_size( board_width, board_height );

	//! Load image points
	std::cout << "Load image points...";
	std::vector<std::vector<cv::Point2f>> image_points;
	std::vector<std::vector<cv::Point3f>> object_points;

	std::vector<cv::Point2f> corners;
	std::vector<cv::Point3f> objects;
	for(int i=0; i<board_height; ++i){
		for( int j=0; j<board_width; ++j){
			objects.push_back(cv::Point3f(double( (float)j * square_size ), double( (float)i * square_size ), 0));
		}
	}
	std::cout << "OK" << std::endl;

	std::cout << "Find the chessboards...";
	for(const cv::Mat& image : img_list){
		
		//!Convert image to grayscale
		cv::Mat gray_image;
		cv::cvtColor(image,gray_image,cv::COLOR_BGR2GRAY);
		
		//!Find the chessboard in the images
		bool found = cv::findChessboardCorners(image, board_size, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		if(found){
			cv::cornerSubPix(gray_image, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			cv::drawChessboardCorners(gray_image, board_size, corners, found);
			image_points.push_back(corners);
      object_points.push_back(objects);
			imshow("win1", image);
			imshow("win2", gray_image);
			int key = cv::waitKey(0);
		}
	}
	std::cout << "OK" << std::endl;

	//!Calibrate the fisheye camera
	std::cout << "Calibrate the fisheye camera...";
	cv::Mat intrinsic = cv::Mat(3, 3, CV_32FC1);
	cv::Mat distCoeffs;
	std::vector<cv::Mat> rvecs;
	std::vector<cv::Mat> tvecs;

	cv::fisheye::calibrate(
		object_points,
		image_points,
		img_list.at(0).size(),
		intrinsic,
		distCoeffs,
		rvecs,
		tvecs,
		cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC 
			| cv::fisheye::CALIB_CHECK_COND 
			| cv::fisheye::CALIB_FIX_SKEW);

	std::string out_file = "calibration_v3.txt";
	cv::FileStorage fs1(out_file, cv::FileStorage::WRITE);
	fs1 << "K" << intrinsic;
	fs1 << "D" << distCoeffs;
	fs1 << "R" << rvecs;
	fs1 << "T" << tvecs;

	std::cout << "OK" << std::endl;

	//! Undistort the images
	std::cout << "Undistort the images...";
	cv::Mat imageUndistorted;
	for(const cv::Mat& image : img_list){
		cv::fisheye::undistortImage(image, imageUndistorted, intrinsic, distCoeffs);
		imshow("win1", image);
		imshow("win2", imageUndistorted);
		cv::waitKey(0);
	}
	std::cout << "OK" << std::endl;

}
