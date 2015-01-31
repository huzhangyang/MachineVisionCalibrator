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
		line(sourceImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), thickness, CV_AA);
	}
}

void GUIManager::ShowImage(string title, Mat sourceImage)
{
	namedWindow(title, WINDOW_NORMAL);
	imshow(title, sourceImage);
	waitKey();
}