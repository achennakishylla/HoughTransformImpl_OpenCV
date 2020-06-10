#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <iostream>
#include <time.h>
#include <dirent.h>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	// Date: 06/10/2020
	//Initialize directory
	std::string inputDirectory = "C:\\Users\\achen\\Desktop\\Spring 2020\\Master's Thesis\\Code\\Hough Transform Results\\Dataset";
	std::string outputDirectory = "C:\\Users\\achen\\Desktop\\Spring 2020\\Master's Thesis\\Code\\Hough Transform Results\\Results\\5_line_detection";
	std::string dir_GB = "C:\\Users\\achen\\Desktop\\Spring 2020\\Master's Thesis\\Code\\Hough Transform Results\\Results\\2_Gaussian Blur";
	std::string dir_Sobel = "C:\\Users\\achen\\Desktop\\Spring 2020\\Master's Thesis\\Code\\Hough Transform Results\\Results\\3_Sobel";
	std::string dir_resize = "C:\\Users\\achen\\Desktop\\Spring 2020\\Master's Thesis\\Code\\Hough Transform Results\\Results\\1_Resize";
	std::string dir_cvtColor = "C:\\Users\\achen\\Desktop\\Spring 2020\\Master's Thesis\\Code\\Hough Transform Results\\Results\\4_CvtColor";

	//Open directory
	DIR *directory = opendir(inputDirectory.c_str());
	struct dirent *_dirent = NULL;

	//Check if directory exists
	if (directory == NULL)
	{
		printf("Cannot open Input Folder\n");
		return 1;
	}

	//load images from input directory
	while ((_dirent = readdir(directory)) != NULL)
	{
		std::string fileName = inputDirectory + "\\" + std::string(_dirent->d_name);
		cv::Mat rawImage = cv::imread(fileName.c_str(),0);
		if (rawImage.data == NULL)
		{
			printf("Cannot Open Image\n");
			continue;
		}

		// Add any image filter here
		Mat src, dst, cdst, dst1;

		//resize image
		resize(rawImage, src, Size(960, 540));
		fileName = dir_resize + "\\" + std::string(_dirent->d_name);
		cv::imwrite(fileName.c_str(), src);

		//perform gaussian blur on the source image after resize
		GaussianBlur(src, dst1, Size(3, 3), 0);
		fileName = dir_GB + "\\" + std::string(_dirent->d_name);
		cv::imwrite(fileName.c_str(), dst1);

		//perform sobel on image after gaussian blur
		//Sobel(dst1, dst, CV_8U, 1, 1, 7);
		Canny(dst1, dst, 50, 50, 3);
		fileName = dir_Sobel + "\\" + std::string(_dirent->d_name);
		cv::imwrite(fileName.c_str(), dst);

		//perform cvtColor on the image after Sobel
		cvtColor(dst, cdst, CV_GRAY2BGR);
		fileName = dir_cvtColor + "\\" + std::string(_dirent->d_name);
		cv::imwrite(fileName.c_str(), cdst);

		//declare a vector for storing line coordinates
		vector<Vec2f> lines;

		//Run Houghlines() and check the runtime
		clock_t tStart = clock();
		HoughLines(dst, lines, 1, CV_PI / 180, 150, 0, 0);
		printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);

		// draw lines
		for (size_t i = 0; i < lines.size(); i++)
		{
			float rho = lines[i][0], theta = lines[i][1];
			Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a * rho, y0 = b * rho;
			pt1.x = cvRound(x0 + 1000 * (-b));
			pt1.y = cvRound(y0 + 1000 * (a));
			pt2.x = cvRound(x0 - 1000 * (-b));
			pt2.y = cvRound(y0 - 1000 * (a));
			line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
		}

		// end image filter

		fileName = outputDirectory + "\\" + std::string(_dirent->d_name);
		cv::imwrite(fileName.c_str(), cdst);
	} closedir(directory);

/*
	Mat src1 = imread(argv[1], 0);
	Mat src, dst, cdst;
	Mat dst1;
	//resize(src1, src, Size(960, 540)); // for NAL.jpg
	//Add a gaussian blur here
	GaussianBlur(src1, dst1, Size(15, 15), 0);
	//Canny(dst1, dst, 50, 50, 3); //for input.png , radon_input.png
	Sobel(dst1, dst, CV_8U, 1, 1, 3);
	//Canny(src, dst, 50, 50, 3); // for desk.jpg
	//Canny(src, dst, 50, 50, 3); //for NAL.jpg
	//Canny(src, dst, 50, 400, 3); // for TDP.jpg
	//Canny(src, dst, 50, 300, 3); // for BT.jpg
	//Canny(src, dst, 50, 20, 3); // for GA.jpg
	//Canny(src, dst, 50, 300, 3); // for TM.jpg
	namedWindow("canny", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("canny", dst * 10);
	cvtColor(dst, cdst, CV_GRAY2BGR);

	vector<Vec2f> lines;
	// detect lines
	//HoughLines(dst, lines, 1, CV_PI / 180, 50, 0, 0); // for input.png
	//Check how fast the call to houghlines
	clock_t tStart = clock();
	HoughLines(dst, lines, 1, CV_PI / 8192, 100, 0, 0);
	//HoughLines(dst, lines, 1, CV_PI / 180, 300, 0, 0); // for radon_input.png, NAL.jpg, TDP.jpg
	printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
	//HoughLines(dst, lines, 1, CV_PI / 180, 100, 0, 0); // for BT.jpg
	//HoughLines(dst, lines, 1, CV_PI / 180, 300, 0, 0); // for GA.jpg
	//HoughLines(dst, lines, 1, CV_PI / 180, 170, 0, 0); // for TM.jpg

	// draw lines
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
	}

	namedWindow("source", WINDOW_NORMAL); // Create a window for display.
	imshow("source", src1);
	namedWindow("detected lines", WINDOW_NORMAL);
	imshow("detected lines", cdst);
	imwrite("input.png", cdst);
	//imwrite("National_Architecture_Library.jpg", cdst);
*/
	waitKey();
	return 0;


}