#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<stdio.h>
#include<math.h>
using namespace cv;
using namespace std;

int main()
{
	Mat src = imread("input.png", 0);
	double t = getTickCount();
	Mat dst = src.clone();
	dst.convertTo(dst, CV_32FC1);
	cv ::Canny(src, dst, 50, 65, 3);
	int angle = 360;
	Mat radon_image = Mat(dst.rows, angle, CV_32FC1);
	int center = dst.rows / 2;

	float shift0[] = { 1, 0, -center,
						0, 1, -center,
						0, 0, 1 };
	float shift1[] = { 1, 0, center,
						0, 1, center,
						0, 0, 1 };
	Mat m0 = Mat(3, 3, CV_32FC1, shift0);
	Mat m1 = Mat(3, 3, CV_32FC1, shift1);
	float *theta = new  float[angle]; // rotation angle
	for (int t = 0; t < 360; t++)
	{
		theta[t] = t * CV_PI / angle;
		float R[] = { cos(theta[t]), sin(theta[t]), 0,
					 -sin(theta[t]), cos(theta[t]), 0,
					 0, 0, 1 };
		Mat mR = Mat(3, 3, CV_32FC1, R);
		Mat rotation = m1 * mR*m0;
		Mat rotated;
		warpPerspective(dst, rotated, rotation, Size(dst.rows, dst.cols), WARP_INVERSE_MAP);
		for (int j = 0; j < rotated.cols; j++)
		{
			float *p1 = radon_image.ptr<float>(j);
			for (int i = 0; i < rotated.rows; i++)
			{
				float *p2 = rotated.ptr<float>(i);
				p1[t] += p2[j];
			}
		}


	}
	cout << (double)(getTickCount() - t) / getTickFrequency() << endl;
	normalize(radon_image, radon_image, 0, 1, CV_MINMAX);
	cv::namedWindow("Radon Image", WINDOW_FREERATIO);
	cv::imshow("Radon Image", radon_image);
	cv::waitKey(0);
	//namedWindow("Radon Transform");
	//imshow("Radon Transform", radon_image);
	//waitKey(0);
	destroyAllWindows();

}