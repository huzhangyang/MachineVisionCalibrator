#include "GUIManager.h"

static GUIManager* manager = nullptr;//µ¥Àý

GUIManager::GUIManager()
{

}

GUIManager* GUIManager::Instance()
{
	if (!manager)
	{
		manager = new GUIManager();
	}
	return manager;
}

void GUIManager::DrawLines(Mat sourceImage, vector<Vec4i> lines, Scalar color, int thickness)
{
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(sourceImage, Point(l[0], l[1]), Point(l[2], l[3]), color, thickness, CV_AA);
	}
}

void GUIManager::DrawLines(Mat sourceImage, vector<Vec2f> lines, Scalar color, int thickness)
{
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(sourceImage, pt1, pt2, color, thickness, CV_AA);
	}
}

void GUIManager::CreateWindow(string title)
{
	namedWindow(title, WINDOW_NORMAL);
}

void GUIManager::ShowImage(string title, Mat sourceImage)
{
	imshow(title, sourceImage);
}

void GUIManager::CreateTrackBar(string name, string windowTitle, int* value, int maxvalue, TrackbarCallback onChange)
{
	createTrackbar(name, windowTitle, value, maxvalue, onChange);
}