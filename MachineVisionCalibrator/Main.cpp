#include "Global.h"

int canny_threshold = 4;
int canny_multiplier = 4;

int hough_minvote = 5;
int hough_minlength = 5;
int hough_maxgap = 20;

Mat sourceImage, edgeImage, detectedImage;
vector<Vec4i> detectedLines;
vector<Vec2f> optimizedLines;
vector<Point> interscetionPoints;
void OnChangeCannyParameter(int, void*);
void OnChangeHoughParameter(int, void*);

int main(int argc, char** argv)
{
	const char* filename = argc >= 2 ? argv[1] : "test2.jpg";
	bool showSourceImage = false;
	bool showEdgeImage = false;
	bool outputResult = false;

	sourceImage = IOManager::Instance()->ReadImage(filename);
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
	//GUIManager::Instance()->CreateTrackBar("MinVote", "Optimized Image", &hough_minvote, 10, OnChangeHoughParameter);
	//GUIManager::Instance()->CreateTrackBar("MinLength", "Optimized Image", &hough_minlength, 10, OnChangeHoughParameter);
	//GUIManager::Instance()->CreateTrackBar("MaxGap", "Optimized Image", &hough_maxgap, 20, OnChangeHoughParameter);

	OnChangeHoughParameter(0, 0);//execute callback at start

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
	detectedLines = ImageProcessor::Instance()->HoughLineTransformP(edgeImage, hough_minvote * 10, hough_minlength * 10, hough_maxgap * 10);
	optimizedLines = ImageProcessor::Instance()->TransformLineFormula(detectedLines, false);
	optimizedLines = ImageProcessor::Instance()->MergeDuplicateLines(optimizedLines, 5, 50);
	//optimizedLines = ImageProcessor::Instance()->RemoveIndependentLines(optimizedLines);
	//optimizedLines = ImageProcessor::Instance()->AddUndetectedLines(optimizedLines);
	interscetionPoints = ImageProcessor::Instance()->GetIntersectionPoints(optimizedLines);
	GUIManager::Instance()->DrawLines(detectedImage, optimizedLines, Scalar(0, 0, 255), 2);
	GUIManager::Instance()->DrawPoints(detectedImage, interscetionPoints, Scalar(255, 0, 0));
	GUIManager::Instance()->ShowImage("Optimized Image", detectedImage);
}