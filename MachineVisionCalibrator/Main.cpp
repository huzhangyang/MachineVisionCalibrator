#include "Global.h"

int canny_threshold = 35;
int canny_multiplier = 4;

int hough_minvote = 50;
int hough_minlength = 100;
int hough_maxgap = 20;

Mat sourceImage, edgeImage, detectedImage;
vector<Vec4i> detectedLines;
void OnChangeCannyParameter(int, void*);
void OnChangeHoughParameter(int, void*);

int main(int argc, char** argv)
{
	const char* filename = argc >= 2 ? argv[1] : "pic.jpg";

	sourceImage = IOManager::Instance()->ReadImage(filename);
	//GUIManager::Instance()->CreateWindow("Source Image");
	//GUIManager::Instance()->ShowImage("Source Image", sourceImage);

	edgeImage = ImageProcessor::Instance()->CannyEdgeDetect(sourceImage, canny_threshold, canny_multiplier);
	//GUIManager::Instance()->CreateWindow("Edge Image");
	//GUIManager::Instance()->CreateTrackBar("Threshold", "Edge Image", &canny_threshold, 200, OnChangeCannyParameter);
	//GUIManager::Instance()->CreateTrackBar("Multiplier", "Edge Image", &canny_multiplier, 10, OnChangeCannyParameter);
	//GUIManager::Instance()->ShowImage("Edge Image", edgeImage);


	detectedImage = sourceImage;
	detectedLines = ImageProcessor::Instance()->HoughLineTransformP(edgeImage, hough_minvote, hough_minlength, hough_maxgap);
	GUIManager::Instance()->DrawLines(detectedImage, detectedLines, Scalar(0, 0, 255), 2);
	GUIManager::Instance()->CreateWindow("Detected Image");
	GUIManager::Instance()->CreateTrackBar("MinVote", "Detected Image", &hough_minvote, 100, OnChangeHoughParameter);
	GUIManager::Instance()->CreateTrackBar("MinLength", "Detected Image", &hough_minlength, 200, OnChangeHoughParameter);
	GUIManager::Instance()->CreateTrackBar("MaxGap", "Detected Image", &hough_maxgap, 100, OnChangeHoughParameter);
	GUIManager::Instance()->ShowImage("Detected Image", detectedImage);

	waitKey();
	return 0;
}

void OnChangeCannyParameter(int, void*)
{
	edgeImage = ImageProcessor::Instance()->CannyEdgeDetect(sourceImage, canny_threshold, canny_multiplier);
	GUIManager::Instance()->ShowImage("Edge Image", edgeImage);
}

void OnChangeHoughParameter(int, void*)
{
	detectedLines = ImageProcessor::Instance()->HoughLineTransformP(edgeImage, hough_minvote, hough_minlength, hough_maxgap);
	GUIManager::Instance()->DrawLines(detectedImage, detectedLines, Scalar(0, 0, 255), 2);
	GUIManager::Instance()->ShowImage("Detected Image", detectedImage);
}