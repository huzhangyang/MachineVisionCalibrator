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
		float theta = lines[i][0], intercept = lines[i][1];
		if (theta == 0)
		{
			line(sourceImage, Point(0, intercept), Point(sourceImage.cols, intercept), color, thickness, CV_AA);//ºáÏß
		}
		else if (theta == 90)
		{
			line(sourceImage, Point(intercept, 0), Point(intercept, sourceImage.rows), color, thickness, CV_AA);//ÊúÏß
		}
		else
		{
			if (theta > 45 || theta < -45)
				line(sourceImage, Point(intercept, 0), Point(intercept + sourceImage.rows / tan(theta / 180 * CV_PI), sourceImage.rows), color, thickness, CV_AA);//x as intercept
			else
				line(sourceImage, Point(0, intercept), Point(sourceImage.cols, intercept + sourceImage.cols * tan(theta / 180 * CV_PI)), color, thickness, CV_AA);//y as intercept
		}
	}
}

void GUIManager::DrawPoints(Mat sourceImage, vector<Point> points, Scalar color, int thickness)
{
	for (int i = 0; i < points.size(); i++)
	{
		circle(sourceImage, points[i], thickness, color, CV_FILLED);
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