#include "ImageProcessor.h"

static ImageProcessor* processor = nullptr;//µ¥Àý

ImageProcessor::ImageProcessor()
{

}

ImageProcessor* ImageProcessor::Instance()
{
	if (!processor)
	{
		processor = new ImageProcessor();
	}
	return processor;
}

Mat ImageProcessor::CannyEdgeDetect(Mat sourceImage, int threshold)
{
	Mat outputImage;
	Canny(sourceImage, outputImage, threshold, threshold * 3, 3);
	return outputImage;
}

Mat ImageProcessor::ConvertColorToGray(Mat sourceImage)
{
	Mat outputImage;
	cvtColor(sourceImage, outputImage, CV_GRAY2BGR);
	return outputImage;
}

vector<Vec4i> ImageProcessor::ProbablisticHoughLines(Mat sourceImage, int minVote, int minLength, int maxGap)
{
	vector<Vec4i> lines;
	HoughLinesP(sourceImage, lines, 1, CV_PI / 180, minVote, minLength, maxGap);
	return lines;
}

vector<Vec2f> ImageProcessor::CalculateLineFormula(vector<Vec4i> lines)
{
	vector<Vec2f> formulae;

	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		float k, b;
		k = (l[1] - l[3]) / (float)(l[0] - l[2]);
		b = ((l[1] - k * l[0]) + (l[3] - k * l[2])) / (float)2;
		formulae.push_back(Vec2f(k,b));
	}
}