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
vector<Vec2f>* splitedLines;
vector<Point> interscetionPoints;
void OnChangeCannyParameter(int, void*);
void OnChangeHoughParameter(int, void*);
void PostProcess();

int main(int argc, char** argv)
{
	cout << "11061093 Zhangyang Hu\nWelcome to machine vision feature point detect.\n"<<endl;
	const char* filename = argc >= 2 ? argv[1] : "image.jpg";
	clock_t startTime, finishTime;
	bool showSourceImage = false;
	bool showEdgeImage = false;
	bool showTransformedImage = false;
	bool showResult = true;
	bool outputResult = true;

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
#if DEBUG
		GUIManager::Instance()->CreateTrackBar("Threshold", "Edge Image", &canny_threshold, 10, OnChangeCannyParameter);
		GUIManager::Instance()->CreateTrackBar("Multiplier", "Edge Image", &canny_multiplier, 10, OnChangeCannyParameter);
#endif
		GUIManager::Instance()->ShowImage("Edge Image", edgeImage);
	}

	detectedLines = ImageProcessor::Instance()->HoughLineTransformP(edgeImage, hough_minvote * 10, parameterReference / 56, parameterReference / 14);
	if (showTransformedImage)
	{
		GUIManager::Instance()->CreateWindow("Transformed Image");
#if DEBUG
		GUIManager::Instance()->CreateTrackBar("MinVote", "Optimized Image", &hough_minvote, 10, OnChangeHoughParameter);
		GUIManager::Instance()->CreateTrackBar("MinLength", "Optimized Image", &hough_minlength, 10, OnChangeHoughParameter);
		GUIManager::Instance()->CreateTrackBar("MaxGap", "Optimized Image", &hough_maxgap, 20, OnChangeHoughParameter);
#endif
		GUIManager::Instance()->DrawLines(detectedImage, detectedLines, Scalar(0, 0, 255), parameterReference / 1000 + 1);
		GUIManager::Instance()->ShowImage("Transformed Image", detectedImage);
	}
	finishTime = clock();
	cout << "Transform Done. Total Time: " << (float)(finishTime - startTime) / CLOCKS_PER_SEC << "s." << endl;
	PostProcess();
	finishTime = clock();
	cout << "PostProcess Done. Total Time: " <<(float)(finishTime - startTime) / CLOCKS_PER_SEC <<"s." <<endl;
	if (showResult)
	{
		GUIManager::Instance()->CreateWindow("Optimized Image");
		GUIManager::Instance()->DrawLines(detectedImage, splitedLines[0], Scalar(0, 0, 255), parameterReference / 1000 + 1);
		GUIManager::Instance()->DrawLines(detectedImage, splitedLines[1], Scalar(0, 0, 255), parameterReference / 1000 + 1);
		GUIManager::Instance()->DrawLines(detectedImage, splitedLines[2], Scalar(0, 0, 255), parameterReference / 1000 + 1);
		GUIManager::Instance()->DrawLines(detectedImage, splitedLines[3], Scalar(0, 0, 255), parameterReference / 1000 + 1);
		GUIManager::Instance()->DrawPoints(detectedImage, interscetionPoints, Scalar(255, 0, 0), parameterReference / 200);
		GUIManager::Instance()->ShowImage("Optimized Image", detectedImage);
	}
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
	detectedLines = ImageProcessor::Instance()->HoughLineTransformP(edgeImage, hough_minvote * 10, hough_minlength * 10, hough_maxgap * 10);
	GUIManager::Instance()->DrawLines(sourceImage, detectedLines, Scalar(0, 0, 255), parameterReference / 1000 + 1);
	GUIManager::Instance()->ShowImage("Transformed Image", sourceImage);
}

void PostProcess()
{
	sourceImage.copyTo(detectedImage);
	optimizedLines = ImageProcessor::Instance()->TransformLineFormula(detectedLines);
	optimizedLines = ImageProcessor::Instance()->MergeDuplicateLines(optimizedLines, 5, parameterReference / 56);
	optimizedLines = ImageProcessor::Instance()->RemoveIndependentLines(optimizedLines, 10, 5);
	splitedLines = ImageProcessor::Instance()->GroupOrientalLines(optimizedLines);
	splitedLines = ImageProcessor::Instance()->AddUndetectedLines(splitedLines);
	interscetionPoints = ImageProcessor::Instance()->GetIntersectionPoints(splitedLines);
}