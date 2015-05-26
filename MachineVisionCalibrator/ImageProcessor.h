#ifndef __IMAGEPROCESSOR_H__
#define __IMAGEPROCESSOR_H__

#include <opencv2\opencv.hpp>
#include <math.h>

using namespace cv;
using namespace std;

class ImageProcessor{
public:
	static ImageProcessor* Instance();
	Mat CannyEdgeDetect(Mat sourceImage, int threshold, int multiplier);
	Mat ConvertColorToGray(Mat sourceImage);
	vector<Vec2f> HoughLineTransform(Mat sourceImage, int threshold);
	vector<Vec4i> HoughLineTransformP(Mat sourceImage, int minVote, int minLength, int maxGap);
	vector<Vec2f> TransformLineFormula(vector<Vec4i> lines);
	vector<Vec2f> MergeDuplicateLines(vector<Vec2f> lines, int thetaPrecision, int interceptPrecision);
	vector<Vec2f> RemoveIndependentLines(vector<Vec2f> lines, int thetaPrecision, int threshold);
	vector<Vec2f>* GroupOrientalLines(vector<Vec2f> lines);
	vector<Vec2f>* AddUndetectedLines(vector<Vec2f>* splitedLines);
	vector<Point> GetIntersectionPoints(vector<Vec2f>* splitedLines);
private:
	Vec2f MergeLines(vector<Vec2f> lines);
	Point GetIntersectionPoint(float theta, float intercept, float theta2, float intercept2);
	ImageProcessor();
};
#endif 