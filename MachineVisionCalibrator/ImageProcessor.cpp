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
		int similarLineCount = 1;
		//�ҳ����Ƶ��߶�
		for (auto iterator2 = optimizedLines.begin(); iterator2 != optimizedLines.end(); iterator2++)
		{
			float theta2 = (*iterator2)[0];
			float intercept2 = (*iterator2)[1];
			if (abs(theta - theta2) <= thetaPrecision)
			{
				similarLineCount++;
			}
		}
		if (similarLineCount < threshold)
		{
			iterator = optimizedLines.erase(iterator);
		}
	}

	return optimizedLines;
}

/*
	��ֱ�߰�����ָ�Ϊ�Ĳ��֡�
	������45��Ϊ�ֽ��߷ָ�Ϊ�����ߡ�
	Ȼ���ҳ��������ߣ��Դ˷ָ��������ߡ�
	��ÿ�����ߣ����������������ߵĽ��㣬�ҳ����佻������ĺ��ߣ��Ƚ�theta��
	theta�ϴ��Ϊ�Һ��ߣ���С��Ϊ����ߡ�
*/
vector<Vec2f>* ImageProcessor::GroupOrientalLines(vector<Vec2f> lines)
{
	vector<Vec2f>* splitedLines = new vector<Vec2f>[4];//0~3:leftVertical,rightVertical,leftHorizontal,rightHorizontal
	vector<Vec2f> verticalLines,horizontalLines;

	for (size_t i = 0; i < lines.size(); i++)
	{
		float theta = lines[i][0];
		float intercept = lines[i][1];
		if (theta > 45 || theta < -45)
			verticalLines.push_back(Vec2f(theta, intercept));
		else
			horizontalLines.push_back(Vec2f(theta, intercept));
	}

	sort(verticalLines.begin(), verticalLines.end(), interceptcomp);
	for (size_t i = 0; i < verticalLines.size(); i++)
	{
		if (i >= verticalLines.size() / 2)
			splitedLines[0].push_back(verticalLines.at(i));
		else if (i < verticalLines.size() / 2)
			splitedLines[1].push_back(verticalLines.at(i));
	}

	for (size_t i = 0; i < horizontalLines.size(); i++)
	{
		float minDistance = 65535;
		int position = 0;//0 for error, 1 for left, 2 for right

		float theta = verticalLines[verticalLines.size() / 2][0];
		float intercept = verticalLines[verticalLines.size() / 2][1];
		float theta2 = horizontalLines[i][0];
		float intercept2 = horizontalLines[i][1];
		Point interscetionPoint = GetIntersectionPoint(theta,intercept,theta2,intercept2);

		for (size_t j = 0; j < horizontalLines.size(); j++)
		{
			if (j == i)continue;
			float theta3 = horizontalLines[j][0];
			float intercept3 = horizontalLines[j][1];
			Point interscetionPoint2 = GetIntersectionPoint(theta, intercept, theta3, intercept3);
			float distance = pow(interscetionPoint.x - interscetionPoint2.x, 2) + pow(interscetionPoint.y - interscetionPoint2.y, 2);
			if (distance < minDistance)
			{
				minDistance = distance;
				if (theta2 > theta3)
					position = 1;
				else
					position = 2;
			}
		}
		if (position == 1)
			splitedLines[2].push_back(horizontalLines.at(i));
		else if (position == 2)
			splitedLines[3].push_back(horizontalLines.at(i));
	}

	return splitedLines;
}

/*
	�����©��ֱ�ߡ�
	���ȼ���ÿһ�ߵ�ƽ��ֱ�߼�ࡣ
	��������Դ���ƽ����࣬������©ֱ�ߡ����п�����©���������δ���
	���������С��ƽ����࣬���Ƕ���ֱ�ߡ������׹���ûʱ����ˣ�
*/
vector<Vec2f>* ImageProcessor::AddUndetectedLines(vector<Vec2f>* lines)
{
	vector<Vec2f>* optimizedLines = lines;
	//put all elementes in optimizedLines[0](leftVertical) to optimizedLines[1](rightVertical)
	for (auto iterator = optimizedLines[0].begin(); iterator != optimizedLines[0].end(); iterator++)
	{
		float theta = (*iterator)[0];
		float intercept = (*iterator)[1];
		optimizedLines[1].push_back(Vec2f(theta, intercept));
	}
	//sort lines
	sort(optimizedLines[1].begin(), optimizedLines[1].end(), interceptcomp);
	sort(optimizedLines[2].begin(), optimizedLines[2].end(), interceptcomp);
	sort(optimizedLines[3].begin(), optimizedLines[3].end(), interceptcomp);
	//check line gaps
	for (int i = 1; i < 3; i++)
	{
		for (auto iterator = optimizedLines[i].begin(); iterator != optimizedLines[i].end() - 1; iterator++)
		{
			int lineGap = (*iterator)[1] - (*(iterator+1))[1];
			int cmpLineGap = 0;
			
			if (iterator == optimizedLines[i].begin())
			{
				cmpLineGap = (*(iterator + 1))[1] - (*(iterator + 2))[1];
			}
			else if (iterator == optimizedLines[i].end() - 2)
			{
				cmpLineGap = (*(iterator - 1))[1] - (*iterator)[1];
			}
			else
			{
				cmpLineGap = ((*(iterator - 1))[1] - (*iterator)[1] + (*(iterator + 1))[1] - (*(iterator + 2))[1]) / 2;
			}
			int linesNeedToBeAdded = (int)((lineGap / (float)cmpLineGap) - 0.5);//should have a better way to determine it
			//add lines (if needed)
			Vec2f currentLine = (*iterator);
			Vec2f nextLine = (*(iterator + 1));
			for (int j = 1; j <= linesNeedToBeAdded; j++)
			{
				vector<Vec2f> linesToMerge;
				linesToMerge.push_back(currentLine);
				linesToMerge.push_back(nextLine);
				Vec2f lineToAdd = MergeLines(linesToMerge);
				lineToAdd[1] = (lineToAdd[1] - nextLine[1]) / (currentLine[1] - nextLine[1]) * 2 * j / (linesNeedToBeAdded + 1);
				lineToAdd[1] = nextLine[1] + (currentLine[1] - nextLine[1]) * lineToAdd[1];
				iterator = optimizedLines[i].insert(iterator + 1, lineToAdd);
			}
		}
	}
	//split vertical lines again
	vector<Vec2f> verticalLines = optimizedLines[1];
	optimizedLines[0].clear();
	optimizedLines[1].clear();
	for (size_t i = 0; i < verticalLines.size(); i++)
	{
		if (i > verticalLines.size() / 2)
			optimizedLines[0].push_back(verticalLines.at(i));
		else if (i < verticalLines.size() / 2)
			optimizedLines[1].push_back(verticalLines.at(i));
	}

	return optimizedLines;
}

/*
	��ȡֱ�ߵĽ���
	���߷ֳ��ĶΣ����ֱ������ߺ��������ߣ��ұߺ��������ߵĽ���
*/
vector<Point> ImageProcessor::GetIntersectionPoints(vector<Vec2f>* lines)
{
	vector<Point> interscetionPoints;
	vector<Vec2f> leftVerticalLines = lines[0];
	vector<Vec2f> rightVerticalLines = lines[1];
	vector<Vec2f> leftHorizontalLines = lines[2];
	vector<Vec2f> rightHorizontalLines = lines[3];
	//sort and erase edge lines
	sort(rightHorizontalLines.begin(), rightHorizontalLines.end(), interceptcomp);
	sort(leftHorizontalLines.begin(), leftHorizontalLines.end(), interceptcomp);
	sort(leftVerticalLines.begin(), leftVerticalLines.end(), interceptcomp);
	sort(rightVerticalLines.begin(), rightVerticalLines.end(), interceptcomp);
	//donot always erase horizontal lines, check if condition is proper
	if (rightHorizontalLines.size() > CROSS_COUNT)
		rightHorizontalLines.erase(rightHorizontalLines.begin());
	if (rightHorizontalLines.size() > CROSS_COUNT)
		rightHorizontalLines.pop_back();
	if (leftHorizontalLines.size() > CROSS_COUNT)
		leftHorizontalLines.erase(leftHorizontalLines.begin());
	if (leftHorizontalLines.size() > CROSS_COUNT)
		leftHorizontalLines.pop_back();
	//always remove vertical lines,since edge vertical lines is always likely to be detected
		leftVerticalLines.pop_back();
		rightVerticalLines.erase(rightVerticalLines.begin());
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
		float theta = leftVerticalLines[i][0];
		float intercept = leftVerticalLines[i][1];
		for (size_t j = 0; j < leftHorizontalLines.size(); j++)
		{
			float theta2 = leftHorizontalLines[j][0];
			float intercept2 = leftHorizontalLines[j][1];
			Point intersectionPoint = GetIntersectionPoint(theta, intercept, theta2, intercept2);
			interscetionPoints.push_back(intersectionPoint);
#if DEBUG
			cout << "(" << cvRound(intersectionPoint.x) << "," << cvRound(intersectionPoint.y) << ") ";
#endif
		}
#if DEBUG
		cout << endl;
#endif
	}
	for (size_t i = 0; i < rightVerticalLines.size(); i++)
	{
		float theta = rightVerticalLines[i][0];
		float intercept = rightVerticalLines[i][1];
		for (size_t j = 0; j < rightHorizontalLines.size(); j++)
		{
			float theta2 = rightHorizontalLines[j][0];
			float intercept2 = rightHorizontalLines[j][1];
			Point intersectionPoint = GetIntersectionPoint(theta, intercept, theta2, intercept2);
			interscetionPoints.push_back(intersectionPoint);
#if DEBUG
			cout << "(" << cvRound(intersectionPoint.x) << "," << cvRound(intersectionPoint.y) << ") ";
#endif
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

Point ImageProcessor::GetIntersectionPoint(float theta, float intercept, float theta2, float intercept2)
{
	Point interscetionPoint;
	float k1 = tan(theta / 180 * CV_PI);//k =tan(theta)
	float b1 = -intercept * k1;//intercept = -b/k
	float k2 = tan(theta2 / 180 * CV_PI);//k =tan(theta)
	float b2 = intercept2;//intercept = b

	if (theta == 90)
	{
		float x = intercept;
		float y = k2 * x + b2;
		interscetionPoint = Point(x, y);
	}
	else
	{
		float x = (b2 - b1) / (k1 - k2);
		float y = k1 * x + b1;
		interscetionPoint = Point(x, y);
	}
	return interscetionPoint;
}

