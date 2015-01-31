#ifndef __IMAGEPROCESSOR_H__
#define __IMAGEPROCESSOR_H__

#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

class ImageProcessor{
public:
	static ImageProcessor* Instance();
	Mat CannyEdgeDetect(Mat sourceImage, int threshold);
	Mat ConvertColorToGray(Mat sourceImage);
	vector<Vec4i> ProbablisticHoughLines(Mat sourceImage, int minVote, int minLength, int maxGap);
	vector<Vec2f> CalculateLineFormula(vector<Vec4i> lines);

private:
	ImageProcessor();
};
#endif 