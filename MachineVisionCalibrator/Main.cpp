#include "Global.h"

int main(int argc, char** argv)
{
	const char* filename = argc >= 2 ? argv[1] : "pic.jpg";
	Mat sourceImage, edgeImage;
	vector<Vec4i> detectedLines;
	vector<Vec2f> detectedFormulae;

	sourceImage = IOManager::Instance()->ReadImage(filename);
	edgeImage = ImageProcessor::Instance()->CannyEdgeDetect(sourceImage, 50);
	detectedLines = ImageProcessor::Instance()->ProbablisticHoughLines(edgeImage, 20, 200, 100);
	detectedFormulae = ImageProcessor::Instance()->CalculateLineFormula(detectedLines);
	GUIManager::Instance()->DrawLines(sourceImage, detectedLines, Scalar(0, 0, 255), 2);
	GUIManager::Instance()->DrawLinesFormula(sourceImage, detectedFormulae, Scalar(0, 255, 0), 2);
	GUIManager::Instance()->ShowImage("HoughLine", sourceImage);

	return 0;
}