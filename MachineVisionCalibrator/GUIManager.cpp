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
			if (theta > 45)
				line(sourceImage, Point(intercept, 0), Point(intercept + sourceImage.rows / tan(theta / 180 * CV_PI), sourceImage.rows), color, thickness, CV_AA);//x as intercept
			else if (theta > 0)
				line(sourceImage, Point(0, intercept), Point(sourceImage.cols, intercept + sourceImage.cols * tan(theta / 180 * CV_PI)), color, thickness, CV_AA);//y as intercept
			else if (theta > -45)
				line(sourceImage, Point(0, intercept), Point(-intercept / tan(theta / 180 * CV_PI), 0), color, thickness, CV_AA);//y as intercept
			else
				line(sourceImage, Point(intercept, 0), Point(0, -intercept * tan(theta / 180 * CV_PI)), color, thickness, CV_AA);//x as intercept
		}
	}
}

void GUIManager::CreateWindow(string title)
{
	namedWindow(title, WINDOW_NORMAL);
}

void GUIManager::ShowImage(string title, Mat sourceImage)
{
	//resizeWindow(title, 800, 600);
	imshow(title, sourceImage);

}

void GUIManager::CreateTrackBar(string name, string windowTitle, int* value, int maxvalue, TrackbarCallback onChange)
{
	createTrackbar(name, windowTitle, value, maxvalue, onChange);
}