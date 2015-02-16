#include "ImageProcessor.h"

static ImageProcessor* processor = nullptr;//单例

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

vector<Vec4i> ImageProcessor::RemoveDuplicateLines(vector<Vec4i> lines, int thetaPrecision, int interceptPrecision)
{
	vector<Vec2f> formulae;
	vector<Vec4i> optimizedLines;

	for (size_t i = 0; i < lines.size(); i++)
	{
		int x1 = lines[i][0];
		int y1 = lines[i][1];
		int x2 = lines[i][2];
		int y2 = lines[i][3];
		float theta, intercept;
		bool isDuplicateLine = false;
		if (y2 == y1)
		{
			theta = 0; intercept = y1;//horizontal
		}
		else if (x2 == x1)
		{
			theta = 90; intercept = x1;//vertical
		}
		else
		{
			theta = atan2(y2 - y1, x2 - x1) * 180 / CV_PI;
			if (abs(theta)> 45)
				intercept = x1 - y1 / tan(theta / 180 * CV_PI);//"vertical", use x as intercept
			else
				intercept = y1 - x1 * tan(theta / 180 * CV_PI);//"horizontal", use y as intercept
		}

		for (auto iterator = formulae.begin(); iterator != formulae.end(); iterator++)
		{
			float theta2 = (*iterator)[0];
			float intercept2 = (*iterator)[1];
			if (abs(theta - theta2) <= thetaPrecision && abs(intercept - intercept2) <= interceptPrecision)
			{
				isDuplicateLine = true;
				break;
			}
		}
		if (!isDuplicateLine)
		{
			formulae.push_back(Vec2f(theta, intercept));
			optimizedLines.push_back(Vec4i(x1, y1, x2, y2));
		}
		else
		{
			//cout << theta << "," << intercept << " is duplicate." << endl;
		}
	}
	return optimizedLines;
}

vector<Vec2f> ImageProcessor::AddUndetectedLines(vector<Vec2f> lines)
{
	vector<Vec2f> optimizedLines;
	//TODO 去除那些没有相似者的线段
	for (auto iterator = lines.begin(); iterator != lines.end();)
	{
		float theta = (*iterator)[0];
		if (theta < 0)theta += 180;
		int similarLineCount = 0;

		for (auto iterator2 = lines.begin(); iterator2 != lines.end(); iterator2++)
		{
			float theta2 = (*iterator2)[0];
			if (theta2 < 0)theta2 += 180;
			if (abs(theta - theta2) <= 10)
			{
				similarLineCount++;
			}
		}
		if (similarLineCount <= 3)
			iterator = lines.erase(iterator);
		else
			iterator++;
	}
	//TODO 检测那些相似线段中间有没有缺的（or直接求交点，再补充缺失的交点？）
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
			theta = 0; intercept = y1;//horizontal
		}
		else if (x2 == x1)
		{
			theta = 90; intercept = x1;//vertical
		}
		else
		{
			theta = atan2(y2 - y1, x2 - x1) * 180 / CV_PI;
			if (theta> 45 || theta < -45)
				intercept = x1 - y1 / tan(theta / 180 * CV_PI);//"vertical", use x as intercept
			else
				intercept = y1 - x1 * tan(theta / 180 * CV_PI);//"horizontal", use y as intercept
		}
		formulae.push_back(Vec2f(theta, intercept));
	}

	sort(formulae.begin(), formulae.end(), vec2fcomp);
	for (size_t i = 0; i < formulae.size(); i++)
	{
		float theta = formulae[i][0];
		float intercept = formulae[i][1];
		cout << "theta = " << cvRound(theta) << ",intercept = " << cvRound(intercept) << endl;
	}
	return formulae;
}


