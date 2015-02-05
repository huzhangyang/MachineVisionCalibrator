#include "ImageProcessor.h"

static ImageProcessor* processor = nullptr;//µ¥Àý

static int vec2fcomp(Vec2f p1, Vec2f p2)
{
	return p1[0] > p2[0];
}

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

vector<Vec4i> ImageProcessor::HoughLineTransformP(Mat sourceImage, int minVote, int minLength, int maxGap)
{
	vector<Vec4i> lines;
	HoughLinesP(sourceImage, lines, 1, CV_PI / 180, minVote, minLength, maxGap);
	return lines;
}

vector<Vec2f> ImageProcessor::HoughLineTransform(Mat sourceImage, int threshold)
{
	vector<Vec2f> lines;
	HoughLines(sourceImage, lines, 1, CV_PI / 180, threshold, 0, 0);
	return lines;
}

vector<Vec2f> ImageProcessor::TransformLineFormula(vector<Vec4i> lines)
{
	vector<Vec2f> formulae;
	//this is not right. use polar coordinates for the line formula.

	for (size_t i = 0; i < lines.size(); i++)
	{
		int x1 = lines[i][0];
		int y1 = lines[i][1];
		int x2 = lines[i][2];
		int y2 = lines[i][3];
		float k, b;
		k = (y2 - y1) / (float)(x2 - x1);
		b = (y1 + y2 - k * (x1 + x2)) * 0.5;
		formulae.push_back(Vec2f(k, b));
	}

	sort(formulae.begin(), formulae.end(), vec2fcomp);
	for (size_t i = 0; i < formulae.size(); i++)
	{ 
		float k = formulae[i][0];
		float b = formulae[i][1];
		if (b > 0)
			cout << "y= " << k << " x + " << (int)b << endl;
		else
			cout << "y= " << k << " x - " << abs((int)b) << endl;
	}
	return formulae;
}


