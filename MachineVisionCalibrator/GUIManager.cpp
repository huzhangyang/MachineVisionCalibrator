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
		/*int x1 = lines[i][0];
		int y1 = lines[i][1];
		int x2 = lines[i][2];
		int y2 = lines[i][3];
		float k = (y2 - y1) / (float)(x2 - x1);
		if (k < 0)*/
		line(sourceImage, Point(l[0], l[1]), Point(l[2], l[3]), color, thickness, CV_AA);
	}
}

void GUIManager::DrawLinesFormula(Mat sourceImage, vector<Vec2f> formulae, Scalar color, int thickness)
{
	for (size_t i = 0; i < formulae.size(); i++)
	{
		float k = formulae[i][0];
		float b = formulae[i][1];
		int xmax = sourceImage.cols;
		int ymax = sourceImage.rows;
		//if (k < 0)
		line(sourceImage, Point(xmax, k * xmax + b), Point((ymax-b) / k, ymax), color, thickness, CV_AA);
	}
}

void GUIManager::ShowImage(string title, Mat sourceImage)
{
	namedWindow(title, WINDOW_NORMAL);
	imshow(title, sourceImage);
	waitKey();
}