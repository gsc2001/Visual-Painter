// Visual painter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

bool sortAreas(const vector<Point>& cnt1, const vector<Point>& cnt2) {
	return (contourArea(cnt1) - contourArea(cnt2)) > 0;
}

int main()
{
	Mat src, frame, hsv, kernel, mask1, mask, drawing, invDrawing, fg, bg;
	vector<vector<Point>> contours;
	vector<Point> cnt;
	vector<Vec4i> hierarchy;
	bool newLine = true;
	Point2f prevCenter, center;
	float radius;
	kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));

	VideoCapture cap(0);
	int width = cap.get(CAP_PROP_FRAME_WIDTH);
	int height = cap.get(CAP_PROP_FRAME_HEIGHT);

	drawing = Mat::zeros(height, width, CV_8UC3);
	Mat greenSceen(height, width, CV_8UC3, Scalar(0, 0, 255));

	if (!cap.isOpened()) {
		cerr << "Error opeing webcam";
		return 1;
	}

	while (1) {
		if (!cap.read(src)) {
			cerr << "Received a blank frame";
			break;
		}
		flip(src, frame, 1);
		cvtColor(frame, hsv, COLOR_BGR2HSV);
		inRange(hsv, Scalar(0, 120, 70), Scalar(10, 255, 255), mask);
		inRange(hsv, Scalar(170, 120, 70), Scalar(180, 255, 255), mask1);
		mask = mask + mask1;
		erode(mask, mask1, kernel);
		morphologyEx(mask1, mask, 2, kernel);
		dilate(mask, mask1, kernel);

		findContours(mask1, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		sort(contours.begin(), contours.end(), sortAreas);
		if (contours.size() >= 1 && contourArea(contours[0]) > 200) {
			cnt = contours[0];
			minEnclosingCircle(cnt, center, radius);
			if ((center.x < 20) && (height - center.y < 20)) {
				drawing = Mat::zeros(height, width, CV_8UC3);
			}
			circle(frame, center, radius, Scalar(0, 255, 0));

			if (newLine) {
				prevCenter = center;
				newLine = false;
			}
			else {
				line(drawing, prevCenter, center, Scalar(255, 255, 255), 3);
				prevCenter = center;
			}
		}
		else {
			newLine = true;
		}

		greenSceen.copyTo(frame, drawing);

		imshow("Mask", mask1);
		imshow("drawing", drawing);
		imshow("Camera Image", frame);
		int key = waitKey(1);
		if (key == 115) {
			drawing = Mat::zeros(height, width, CV_8UC3);
		}
		else if (key >= 0) {
			break;
		}
	}
	cap.release();
	destroyAllWindows();
}

