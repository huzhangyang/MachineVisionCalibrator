#include "Global.h"

int main(int argc, char** argv)
{
	const char* filename = argc >= 2 ? argv[1] : "pic.jpg";
	Mat sourceImage, edgeImage;
	vector<Vec4i> detectedLines;

	sourceImage = IOManager::Instance()->ReadImage(filename);
	edgeImage = ImageProcessor::Instance()->CannyEdgeDetect(sourceImage, 50);
	detectedLines = ImageProcessor::Instance()->ProbablisticHoughLines(edgeImage, 50, 50, 10);
	GUIManager::Instance()->DrawLines(sourceImage, detectedLines, Scalar(0, 0, 255), 5);
	GUIManager::Instance()->ShowImage("HoughLine", sourceImage);

	return 0;
}