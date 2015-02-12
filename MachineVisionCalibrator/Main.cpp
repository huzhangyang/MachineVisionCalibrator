#include "Global.h"

int canny_threshold = 4;
int canny_multiplier = 4;

int hough_minvote = 5;
int hough_minlength = 5;
int hough_maxgap = 20;

Mat sourceImage, edgeImage, detectedImage;
vector<Vec4i> detectedLines,optimizedLines;
void OnChangeCannyParameter(int, void*);
void OnChangeHoughParameter(int, void*);

int main(int argc, char** argv)
{
	const char* filename = argc >= 2 ? argv[1] : "test.jpg";

	sourceImage = IOManager::Instance()->ReadImage(filename);
	//GUIManager::Instance()->CreateWindow("Source Image");
	//GUIManager::Instance()->ShowImage("Source Image", sourceImage);

	edgeImage = ImageProcessor::Instance()->CannyEdgeDetect(sourceImage, canny_threshold * 10, canny_multiplier);
	//GUIManager::Instance()->CreateWindow("Edge Image");
	//GUIManager::Instance()->CreateTrackBar("Threshold", "Edge Image", &canny_threshold, 10, OnChangeCannyParameter);
	//GUIManager::Instance()->CreateTrackBar("Multiplier", "Edge Image", &canny_multiplier, 10, OnChangeCannyParameter);
	//GUIManager::Instance()->ShowImage("Edge Image", edgeImage);

	GUIManager::Instance()->CreateWindow("Optimized Image");
	//GUIManager::Instance()->CreateTrackBar("MinVote", "Optimized Image", &hough_minvote, 10, OnChangeHoughParameter);
	//GUIManager::Instance()->CreateTrackBar("MinLength", "Optimized Image", &hough_minlength, 10, OnChangeHoughParameter);
	//GUIManager::Instance()->CreateTrackBar("MaxGap", "Optimized Image", &hough_maxgap, 20, OnChangeHoughParameter);
	sourceImage.copyTo(detectedImage);
	detectedLines = ImageProcessor::Instance()->HoughLineTransformP(edgeImage, hough_minvote * 10, hough_minlength * 10, hough_maxgap * 10);
	optimizedLines = ImageProcessor::Instance()->RemoveDuplicateLines(detectedLines);
	//GUIManager::Instance()->DrawLines(detectedImage, optimizedLines, Scalar(0, 0, 255), 2);
	GUIManager::Instance()->DrawLines(detectedImage, ImageProcessor::Instance()->TransformLineFormula(optimizedLines), Scalar(0, 0, 255), 2);
	GUIManager::Instance()->ShowImage("Optimized Image", detectedImage);

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
	optimizedLines = ImageProcessor::Instance()->RemoveDuplicateLines(detectedLines);
	GUIManager::Instance()->DrawLines(detectedImage, optimizedLines, Scalar(0, 0, 255), 2);
	GUIManager::Instance()->ShowImage("Optimized Image", detectedImage);
	
}