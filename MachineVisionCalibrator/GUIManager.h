#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

class GUIManager{
public:
	static GUIManager* Instance();
	void DrawLines(Mat sourceImage, vector<Vec4i> lines, Scalar color, int thickness);
	void DrawLines(Mat sourceImage, vector<Vec2f> lines, Scalar color, int thickness);
	void DrawPoints(Mat sourceImage, vector<Point>points, Scalar color, int thickness);
	void CreateWindow(string title);
	void ShowImage(string title, Mat sourceImage);
	void CreateTrackBar(string name, string windowTitle, int* value, int maxvalue, TrackbarCallback onChange = 0);
private:
	GUIManager();
};
#endif 