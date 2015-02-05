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
	void ShowImage(string title, Mat sourceImage);
private:
	GUIManager();
};
#endif 