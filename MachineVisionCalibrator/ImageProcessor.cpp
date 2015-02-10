#include "ImageProcessor.h"

static ImageProcessor* processor = nullptr;//µ¥Àý

static int vec2fcomp(Vec2f p1, Vec2f p2)
{
	if (p1[0] == p2[0])
		return p1[1] > p2[1];
	else
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

Mat ImageProcessor::CannyEdgeDetect(Mat sourceImage, int threshold, int multiplier)
{
	Mat outputImage;
	Canny(sourceImage, outputImage, threshold, threshold * multiplier, 3);
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

	for (size_t i = 0; i < lines.size(); i++)
	{
		int x1 = lines[i][0];
		int y1 = lines[i][1];
		int x2 = lines[i][2];
		int y2 = lines[i][3];
		float theta, intercept;
		if (y2 == y1)
		{
			theta = 0; intercept = y1;
		}
		else if (x2 == x1)
		{
			theta = 255; intercept = x1;
		}
		else
		{
			theta = atan2(y2 - y1, x2 - x1);
			int b1 = y1 - x1 * tan(theta);
			int b2 = y2 - x2 * tan(theta);
			intercept = (b1 + b2) / 2;
		}

		formulae.push_back(Vec2f(theta, intercept));
	}

	sort(formulae.begin(), formulae.end(), vec2fcomp);
	for (auto iterator = formulae.begin(); iterator != formulae.end();)
	{
		float theta = (*iterator)[0];
		float intercept = (*iterator)[1];

		for (auto iterator2 = iterator + 1; iterator2 != formulae.end();)
		{
			float theta2 = (*iterator2)[0];
			float intercept2 = (*iterator2)[1];
			if (theta == 0 || theta == 255)
			{
				if ((theta == theta2) && abs(intercept - intercept2) <= 10)
					iterator2 = formulae.erase(iterator2);
				else
					iterator2++;
			}	
			else if (abs(theta - theta2) <= CV_PI / 180 && abs(intercept - intercept2) * cos(theta) <= 100)
			{
				iterator2 = formulae.erase(iterator2);
			}
			else
			{
				iterator2++;
			}
		}
		iterator++;
	}
	for (size_t i = 0; i < formulae.size(); i++)
	{
		float theta = formulae[i][0];
		float intercept = formulae[i][1];
		cout << "theta = " << theta * 180 / CV_PI << ",intercept = " << intercept * cos(theta) << endl;
	}
	return formulae;
}


