#include "Global.h"

int canny_threshold = 4;
int canny_multiplier = 4;

int hough_minvote = 5;
int hough_minlength = 5;
int hough_maxgap = 20;

Mat sourceImage, edgeImage, detectedImage;
vector<Vec4i> detectedLines;
vector<Vec2f> optimizedLines;
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


	detectedImage = sourceImage;
	detectedLines = ImageProcessor::Instance()->HoughLineTransformP(edgeImage, hough_minvote * 10, hough_minlength * 10, hough_maxgap * 10);
	optimizedLines = ImageProcessor::Instance()->TransformLineFormula(detectedLines);
	GUIManager::Instance()->DrawLines(detectedImage, optimizedLines, Scalar(0, 0, 255), 2);
	GUIManager::Instance()->CreateWindow("Detected Image");
	//GUIManager::Instance()->CreateTrackBar("MinVote", "Detected Image", &hough_minvote, 10, OnChangeHoughParameter);
	//GUIManager::Instance()->CreateTrackBar("MinLength", "Detected Image", &hough_minlength, 10, OnChangeHoughParameter);
	//GUIManager::Instance()->CreateTrackBar("MaxGap", "Detected Image", &hough_maxgap, 20, OnChangeHoughParameter);
	GUIManager::Instance()->ShowImage("Detected Image", detectedImage);



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
	detectedImage = sourceImage;
	detectedLines = ImageProcessor::Instance()->HoughLineTransformP(edgeImage, hough_minvote * 10, hough_minlength * 10, hough_maxgap * 10);
	GUIManager::Instance()->DrawLines(detectedImage, detectedLines, Scalar(0, 0, 255), 2);
	GUIManager::Instance()->ShowImage("Detected Image", detectedImage);
	
}