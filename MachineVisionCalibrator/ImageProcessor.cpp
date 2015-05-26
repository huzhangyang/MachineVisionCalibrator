#include "ImageProcessor.h"
#include "Global.h"

static ImageProcessor* processor = nullptr;//����

static int vec2fcomp(Vec2f p1, Vec2f p2)
{
	if (p1[0] == p2[0])
		return p1[1] > p2[1];
	else
		return p1[0] > p2[0];
}

static int interceptcomp(Vec2f p1, Vec2f p2)
{
	return p1[1] > p2[1];
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

vector<Vec2f> ImageProcessor::HoughLineTransform(Mat sourceImage, int threshold)
{
	vector<Vec2f> lines;
	HoughLines(sourceImage, lines, 1, CV_PI / 180, threshold, 0, 0);
	return lines;
}

vector<Vec4i> ImageProcessor::HoughLineTransformP(Mat sourceImage, int minVote, int minLength, int maxGap)
{
	vector<Vec4i> lines;
	HoughLinesP(sourceImage, lines, 1, CV_PI / 180, minVote, minLength, maxGap);
	return lines;
}

/*
	ת��ֱ�߹�ʽ��ֱ���ɸ��ʻ���任��õĹ�ʽ����Vec4i���߶θ�ʽ��ʾ�ģ�����ת��ΪVec2f��ֱ�߱�ʾ��
	ʹ��theta����ֱ����X��ļнǣ���һ������(0,90)������������(0,-90)����
	ʹ��intercept����ؾࡣ����0~45��-45��ʹ��Y��ؾ࣬����ʹ��X��ؾࡣ
	��ֱ�߰���theta��intercept��˳���������
*/
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
			if (theta > 45 || theta < -45)
				intercept = x1 - y1 / tan(theta / 180 * CV_PI);//"vertical", use x as intercept
			else
				intercept = y1 - x1 * tan(theta / 180 * CV_PI);//"horizontal", use y as intercept
		}
		formulae.push_back(Vec2f(theta, intercept));
	}
	//output tranformed line formula
#if DEBUG
	for (size_t i = 0; i < formulae.size(); i++)
	{
		float theta = formulae[i][0];
		float intercept = formulae[i][1];
		cout << "theta = " << cvRound(theta) << ",intercept = " << cvRound(intercept) << endl;
	}
#endif
	return formulae;
}

/*
	�ϲ��ظ���ֱ�ߡ�
	��ÿһ����δʹ�á�ֱ�ߣ�����ȫ�֣�Ѱ�����ظ��ߡ����ҵ������ظ��ߵ㼯���뼯�ϣ������ظ��߱��Ϊ��ʹ�ù�����
	�����󣬶�ֱ�߼����ظ��߽�����ϡ�
*/
vector<Vec2f> ImageProcessor::MergeDuplicateLines(vector<Vec2f> lines, int thetaPrecision, int interceptPrecision)
{
	vector<Vec2f> optimizedLines;
	vector<Vec2f> duplicateLines;
	vector<Vec2f> mergedLines;

	for (auto iterator = lines.begin(); iterator != lines.end(); iterator++)
	{
		float theta = (*iterator)[0];
		float intercept = (*iterator)[1];
		int isMerged = false;

		for (auto i = mergedLines.begin(); i != mergedLines.end(); i++)
		{
			if (mergedLines.empty())break;
			float _theta = (*i)[0];
			float _intercept = (*i)[1];
			if (theta == _theta && intercept == _intercept)
			{// this line is merged
				isMerged = true;
				break;
			}	
		}
		if (isMerged)
			continue;
		duplicateLines.push_back(Vec2f(theta, intercept));
		mergedLines.push_back(Vec2f(theta, intercept));
		for (auto iterator2 = iterator + 1; iterator2 != lines.end(); iterator2++)
		{
			float theta2 = (*iterator2)[0];
			float intercept2 = (*iterator2)[1];
			if (abs(theta - theta2) <= thetaPrecision && abs(intercept - intercept2) <= interceptPrecision)
			{
				duplicateLines.push_back(Vec2f(theta2, intercept2));
				mergedLines.push_back(Vec2f(theta2, intercept2));
				//break;
			}
		}
		optimizedLines.push_back(MergeLines(duplicateLines));
		duplicateLines.clear();
	}

	return optimizedLines;
}

/*
	�Ƴ�������ֱ�ߡ�
	����ÿһ��ֱ�ߣ�Ѱ���������ߣ�theta����ģ��������߹��ٵģ�Ϊ������ֱ�ߣ�ɾȥ��
*/
vector<Vec2f> ImageProcessor::RemoveIndependentLines(vector<Vec2f> lines, int thetaPrecision, int threshold)
{
	vector<Vec2f> optimizedLines = lines;
	
	for (auto iterator = optimizedLines.begin(); iterator != optimizedLines.end(); iterator++)
	{
		float theta = (*iterator)[0];
		float intercept = (*iterator)[1];
		vector<Vec2f> similarLines;
		similarLines.push_back(Vec2f(theta, intercept));
		//�ҳ����Ƶ��߶�
		for (auto iterator2 = optimizedLines.begin(); iterator2 != optimizedLines.end(); iterator2++)
		{
			float theta2 = (*iterator2)[0];
			float intercept2 = (*iterator2)[1];
			if (abs(theta - theta2) <= thetaPrecision)
			{
				similarLines.push_back(Vec2f(theta2, intercept2));
			}
		}
		if (similarLines.size() < threshold)
		{
			iterator = optimizedLines.erase(iterator);
		}
	}

	return optimizedLines;
}

/*
	�����©��ֱ�ߡ�
	������Ҫ��취��֪ƽ��ֱ�߼�࣬��Ҫ�ٶ������еı�Եֱ���������ı�Եֱ�ߡ�
	ʹ�����������������ߣ���֪����ƽ��ֱ�߼�࣬������Ϊ��������ں���ƽ��ֱ�߼�ࡣ
	�����С��ƽ����࣬���Ƕ���ֱ�ߡ���������ƽ����࣬������©ֱ�ߡ�
	����������⻹��Ҫ�ֿ�����
*/
vector<Vec2f> ImageProcessor::AddUndetectedLines(vector<Vec2f> lines)
{
	vector<Vec2f> optimizedLines;

	// calculate avgLineGap
	int maxIntercept = -65535;
	int minIntercept = 65535;
	for (size_t i = 0; i < lines.size(); i++)
	{
		float theta = lines[i][0];
		float intercept = lines[i][1];
		if (theta > 45 || theta < -45)//vertical
		{
			if (intercept < minIntercept)minIntercept = intercept;
			else if (intercept > maxIntercept)maxIntercept = intercept;
		}
	}
	int avgLineGap = (maxIntercept - minIntercept) / 20;
	//check line gaps
	sort(lines.begin(), lines.end(), vec2fcomp);
	return lines;
}

/*
	��ȡֱ�ߵĽ���
	���߷ֳ��ĶΣ����ֱ������ߺ��������ߣ��ұߺ��������ߵĽ���
*/
vector<Point> ImageProcessor::GetIntersectionPoints(vector<Vec2f> lines)
{
	vector<Point> interscetionPoints;
	vector<Vec2f> verticalLines;
	vector<Vec2f> leftVerticalLines;
	vector<Vec2f> rightVerticalLines;
	vector<Vec2f> leftHorizontalLines;
	vector<Vec2f> rightHorizontalLines;
	//split lines into vertical and horizontal
	for (size_t i = 0; i < lines.size(); i++)
	{
		float theta = lines[i][0];
		float intercept = lines[i][1];
		if (theta >= 0 && theta <= 45)
		{
			rightHorizontalLines.push_back(Vec2f(theta, intercept));
		}
		else if (theta <= 0 && theta >= -45)
		{
			leftHorizontalLines.push_back(Vec2f(theta, intercept));
		}
		else
			verticalLines.push_back(Vec2f(theta, intercept));//put it here first, split later
	}
	//sort and erase edge lines
	sort(rightHorizontalLines.begin(), rightHorizontalLines.end(), interceptcomp);
	sort(leftHorizontalLines.begin(), leftHorizontalLines.end(), interceptcomp);
	sort(verticalLines.begin(), verticalLines.end(), interceptcomp);
	if (rightHorizontalLines.size() > CROSS_COUNT)
		rightHorizontalLines.erase(rightHorizontalLines.begin());
	if (rightHorizontalLines.size() > CROSS_COUNT)
		rightHorizontalLines.pop_back();
	if (leftHorizontalLines.size() > CROSS_COUNT)
		leftHorizontalLines.erase(leftHorizontalLines.begin());
	if (leftHorizontalLines.size() > CROSS_COUNT)
		leftHorizontalLines.pop_back();
	if (verticalLines.size() > CROSS_COUNT)
		verticalLines.erase(verticalLines.begin());
	if (verticalLines.size() > CROSS_COUNT)
		verticalLines.pop_back();
	//split vertical lines
	for (size_t i = 0; i < verticalLines.size(); i++)
	{
		if (i < verticalLines.size() / 2)
			leftVerticalLines.push_back(verticalLines.at(i));
		else if (i > verticalLines.size() / 2)
			rightVerticalLines.push_back(verticalLines.at(i));
	}
	//check if each type of lines is at its correct count
	if (leftHorizontalLines.size() != CROSS_COUNT)
		cout << "leftHorizontalLines Count = " << leftHorizontalLines.size() << ", was expecting " << CROSS_COUNT << ", might output wrong result." << endl;
	if (rightHorizontalLines.size() != CROSS_COUNT)
		cout << "rightHorizontalLines Count = " << rightHorizontalLines.size() << ", was expecting " << CROSS_COUNT << ", might output wrong result." << endl;
	if (leftVerticalLines.size() != CROSS_COUNT)
		cout << "leftVerticalLines Count = " << leftVerticalLines.size() << ", was expecting " << CROSS_COUNT << ", might output wrong result." << endl;
	if (rightVerticalLines.size() != CROSS_COUNT)
		cout << "rightVerticalLines Count = " << rightVerticalLines.size() << ", was expecting " << CROSS_COUNT << ", might output wrong result." << endl;
	//calculate interscetion points
	for (size_t i = 0; i < leftVerticalLines.size(); i++)
	{
		if (leftVerticalLines[i][0] == 90)
		{
			float intercept = leftVerticalLines[i][1];
			for (size_t j = 0; j < leftHorizontalLines.size(); j++)
			{
				float k2 = tan(leftHorizontalLines[j][0] / 180 * CV_PI);//k =tan(theta)
				float b2 = leftHorizontalLines[j][1];//intercept = b

				float x = intercept;
				float y = k2 * x + b2;
				interscetionPoints.push_back(Point(x, y));
#if DEBUG
				cout << "(" << cvRound(x) << "," << cvRound(y) << ") ";
#endif
			}
		}
		else
		{
			float k1 = tan(leftVerticalLines[i][0] / 180 * CV_PI);//k =tan(theta)
			float b1 = -leftVerticalLines[i][1] * k1;//intercept = -b/k
			for (size_t j = 0; j < leftHorizontalLines.size(); j++)
			{
				float k2 = tan(leftHorizontalLines[j][0] / 180 * CV_PI);//k =tan(theta)
				float b2 = leftHorizontalLines[j][1];//intercept = b

				float x = (b2 - b1) / (k1 - k2);
				float y = k1 * x + b1;
				interscetionPoints.push_back(Point(x, y)); 
#if DEBUG
				cout << "(" << cvRound(x) << "," << cvRound(y) << ") ";
#endif
			}
		}
#if DEBUG
		cout << endl;
#endif
	}
	for (size_t i = 0; i < rightVerticalLines.size(); i++)
	{
		if (rightVerticalLines[i][0] == 90)
		{
			float intercept = rightVerticalLines[i][1];
			for (size_t j = 0; j < rightHorizontalLines.size(); j++)
			{
				float k2 = tan(rightHorizontalLines[j][0] / 180 * CV_PI);//k =tan(theta)
				float b2 = rightHorizontalLines[j][1];//intercept = b

				float x = intercept;
				float y = k2 * x + b2;
				interscetionPoints.push_back(Point(x, y));
#if DEBUG
				cout << "(" << cvRound(x) << "," << cvRound(y) << ") ";
#endif
			}
		}
		else
		{
			float k1 = tan(rightVerticalLines[i][0] / 180 * CV_PI);//k =tan(theta)
			float b1 = -rightVerticalLines[i][1] * k1;//intercept = -b/k
			for (size_t j = 0; j < rightHorizontalLines.size(); j++)
			{
				float k2 = tan(rightHorizontalLines[j][0] / 180 * CV_PI);//k =tan(theta)
				float b2 = rightHorizontalLines[j][1];//intercept = b

				float x = (b2 - b1) / (k1 - k2);
				float y = k1 * x + b1;
				interscetionPoints.push_back(Point(x, y));
#if DEBUG
				cout << "(" << cvRound(x) << "," << cvRound(y) << ") ";
#endif
			}
		}
#if DEBUG
		cout << endl;
#endif
	}
	return interscetionPoints;
}

Vec2f ImageProcessor::MergeLines(vector<Vec2f> lines)
{
	float x1 = 0;
	float y1 = 0;
	float x2 = 0;
	float y2 = 0;

	for (int i = 0; i < lines.size(); i++)
	{
		float theta = lines[i][0], intercept = lines[i][1];
		if (theta == 0)
		{
			x1 += 0;
			y1 += intercept;
			x2 += ImageWidth;
			y2 += intercept;
		}
		else if (theta == 90)
		{
			x1 += intercept;
			y1 += 0;
			x2 += intercept;
			y2 += ImageHeight;
		}
		else
		{
			if (theta > 45 || theta < -45)
			{
				x1 += intercept;
				y1 += 0;
				x2 += intercept + ImageHeight / tan(theta / 180 * CV_PI);
				y2 += ImageHeight;
			}
			else
			{
				x1 += 0;
				y1 += intercept;
				x2 += ImageWidth;
				y2 += intercept + ImageWidth * tan(theta / 180 * CV_PI);
			}
		}
	}
	x1 /= lines.size();
	y1 /= lines.size();
	x2 /= lines.size();
	y2 /= lines.size();

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
		if (theta > 45 || theta < -45)
			intercept = x1 - y1 / tan(theta / 180 * CV_PI);//"vertical", use x as intercept
		else
			intercept = y1 - x1 * tan(theta / 180 * CV_PI);//"horizontal", use y as intercept
	}
	return Vec2f(theta, intercept);
}

