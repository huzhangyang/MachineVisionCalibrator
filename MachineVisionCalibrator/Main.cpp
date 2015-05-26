#include "Global.h"

int canny_threshold = 4;
int canny_multiplier = 4;

int hough_minvote = 5;
int hough_minlength = 5;
int hough_maxgap = 20;

int ImageHeight;
int ImageWidth;
int parameterReference;

Mat sourceImage, edgeImage, detectedImage;
vector<Vec4i> detectedLines;
vector<Vec2f> optimizedLines;
vector<Point> interscetionPoints;
void OnChangeCannyParameter(int, void*);
void OnChangeHoughParameter(int, void*);

int main(int argc, char** argv)
{
	cout << "11061093 Zhangyang Hu\nWelcome to machine vision feature point detect.\n"<<endl;
	const char* filename = argc >= 2 ? argv[1] : "test.jpg";
	clock_t startTime, finishTime;
	bool showSourceImage = false;
	bool showEdgeImage = false;
	bool outputResult = false;

	sourceImage = IOManager::Instance()->ReadImage(filename);
	ImageHeight = sourceImage.rows;
	ImageWidth = sourceImage.cols;
	parameterReference = sqrt(ImageHeight * ImageWidth);
	cout << "File:" << filename << "\nSize :" << ImageWidth << "*" << ImageHeight << "\nExecuting..." << endl;
	startTime = clock();
	if (showSourceImage)
	{
		GUIManager::Instance()->CreateWindow("Source Image");
		GUIManager::Instance()->ShowImage("Source Image", sourceImage);
	}

	edgeImage = ImageProcessor::Instance()->CannyEdgeDetect(sourceImage, canny_threshold * 10, canny_multiplier);
	if (showEdgeImage)
	{
		GUIManager::Instance()->CreateWindow("Edge Image");
		GUIManager::Instance()->CreateTrackBar("Threshold", "Edge Image", &canny_threshold, 10, OnChangeCannyParameter);
		GUIManager::Instance()->CreateTrackBar("Multiplier", "Edge Image", &canny_multiplier, 10, OnChangeCannyParameter);
		GUIManager::Instance()->ShowImage("Edge Image", edgeImage);
	}

	GUIManager::Instance()->CreateWindow("Optimized Image");
#if DEBUG
	GUIManager::Instance()->CreateTrackBar("MinVote", "Optimized Image", &hough_minvote, 10, OnChangeHoughParameter);
	GUIManager::Instance()->CreateTrackBar("MinLength", "Optimized Image", &hough_minlength, 10, OnChangeHoughParameter);
	GUIManager::Instance()->CreateTrackBar("MaxGap", "Optimized Image", &hough_maxgap, 20, OnChangeHoughParameter);
#endif
	OnChangeHoughParameter(0, 0);//execute callback at start

	finishTime = clock();
	cout << "Done. Time: " <<(float)(finishTime - startTime) / CLOCKS_PER_SEC <<"s." <<endl;
	if (outputResult)
		IOManager::Instance()->OutputResult(interscetionPoints, "out.txt");
	waitKey();
	return 0;
}

void OnChangeCannyParameter(int, void*)
{
	edgeImage = ImageProcessor::Instance()->CannyEdgeDetect(sourceImage, canny_threshold * 10, canny_multiplier);
	GUIManager::Instance()->ShowImage("Edge Image", edgeImage);
}

void OnChangeHoughParameter(int, void*)
{
	sourceImage.copyTo(detectedImage);
	detectedLines = ImageProcessor::Instance()->HoughLineTransformP(edgeImage, hough_minvote * 10, parameterReference / 56, parameterReference / 14);
	optimizedLines = ImageProcessor::Instance()->TransformLineFormula(detectedLines);
	optimizedLines = ImageProcessor::Instance()->MergeDuplicateLines(optimizedLines, 5, parameterReference / 56);
	optimizedLines = ImageProcessor::Instance()->RemoveIndependentLines(optimizedLines, 10, 5);
	optimizedLines = ImageProcessor::Instance()->AddUndetectedLines(optimizedLines);
	interscetionPoints = ImageProcessor::Instance()->GetIntersectionPoints(optimizedLines);
	GUIManager::Instance()->DrawLines(detectedImage, optimizedLines, Scalar(0, 0, 255), parameterReference / 1000 + 1);
	GUIManager::Instance()->DrawPoints(detectedImage, interscetionPoints, Scalar(255, 0, 0), parameterReference / 200);
	GUIManager::Instance()->ShowImage("Optimized Image", detectedImage);
}