#include "ImageProcessor.h"

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
vector<Vec2f> ImageProcessor::TransformLineFormula(vector<Vec4i> lines, bool sortLines)
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

	if (sortLines)
		sort(formulae.begin(), formulae.end(), vec2fcomp);

	//output tranformed line formula
	for (size_t i = 0; i < formulae.size(); i++)
	{
		float theta = formulae[i][0];
		float intercept = formulae[i][1];
		cout << "theta = " << cvRound(theta) << ",intercept = " << cvRound(intercept) << endl;
	}
	return formulae;
}

/*
	�ϲ��ظ���ֱ�ߡ�
*/
vector<Vec2f> ImageProcessor::MergeDuplicateLines(vector<Vec2f> lines, int thetaPrecision, int interceptPrecision)
{
	vector<Vec2f> optimizedLines;

	for (auto iterator = lines.begin(); iterator != lines.end(); iterator++)
	{
		float theta = (*iterator)[0];
		float intercept = (*iterator)[1];
		bool isDuplicateLine = false;
		for (auto iterator = optimizedLines.begin(); iterator != optimizedLines.end(); iterator++)
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
		{//TODO ����duplicateline��Ӧ�����ȡƽ��ֵ
			optimizedLines.push_back(Vec2f(theta, intercept));
		}
		else
		{
			//cout << theta << "," << intercept << " is duplicate." << endl;
		}
	}

	return optimizedLines;
}

/*
	�Ƴ�������ֱ�ߡ�
*/
vector<Vec2f> ImageProcessor::RemoveIndependentLines(vector<Vec2f> lines)
{
	vector<Vec2f> optimizedLines = lines;
	//TODO ȥ����Щû�������ߵ��߶Σ�Ӧ���и��õ��㷨
	for (auto iterator = lines.begin(); iterator != lines.end(); iterator++)
	{
		float theta = (*iterator)[0];
		float intercept = (*iterator)[1];
		if (theta < 0)theta += 180;
		vector<Vec2f> similarLines;
		similarLines.push_back(Vec2f(theta, intercept));
		//�ҳ����Ƶ��߶�
		for (auto iterator2 = lines.begin(); iterator2 != lines.end(); iterator2++)
		{
			float theta2 = (*iterator2)[0];
			float intercept2 = (*iterator2)[1];
			if (theta2 < 0)theta2 += 180;
			if (abs(theta - theta2) <= 10)
			{
				similarLines.push_back(Vec2f(theta2, intercept2));
			}
		}
		if (similarLines.size() > 3)
		{
			vector<float>distances;
			float averageDistance;
			sort(similarLines.begin(), similarLines.end(), interceptcomp);
			for (int i = 0; i < similarLines.size() - 1; i++)
			{
				distances.push_back(similarLines[i + 1][1] - similarLines[i][1]);
			}
			for (auto iterator2 = distances.begin() + 1; iterator2 != distances.end(); iterator2++)
			{
				float distance1 = *iterator2 - 1;
				float distance2 = *iterator2;
				for (int j = 0; j < distances.size(); j++)
				{
					if (abs(distances[j] - distance1) > 10 && abs(distances[j] - distance2) > 10)
					{//����һ���ߵ�ǰ����붼������������Ҫ�̣������Ƕ������
						iterator2 = distances.erase(iterator2);
						break;
					}

				}
			}
		}
	}

	return optimizedLines;
}


/*
	�����©��ֱ�ߡ�
*/
vector<Vec2f> ImageProcessor::AddUndetectedLines(vector<Vec2f> lines)
{
	vector<Vec2f> optimizedLines;
	//TODO �����Щ�����߶��м���û��ȱ��
	cout << "Lines:" << optimizedLines.size() << endl;
	return optimizedLines;
}

/*
��ȡֱ�ߵĽ���
*/
vector<Point> ImageProcessor::GetIntersectionPoints(vector<Vec2f> lines)
{
	vector<Point> interscetionPoints;
	vector<Vec2f> verticalLines;
	vector<Vec2f> horizontalLines;
	//split lines into vertical and horizontal
	//TODOӦ�ý��߷ֳ��ĶΣ����ֱ����
	for (size_t i = 0; i < lines.size(); i++)
	{
		float theta = lines[i][0];
		float intercept = lines[i][1];
		if (theta> 45 || theta < -45)
			verticalLines.push_back(Vec2f(theta, intercept));
		else
			horizontalLines.push_back(Vec2f(theta, intercept));
	}
	//calculate interscetion points
	for (size_t i = 0; i < verticalLines.size(); i++)
	{
		if (verticalLines[i][0] == 90)
		{
			float intercept = verticalLines[i][1];
			for (size_t j = 0; j < horizontalLines.size(); j++)
			{
				float k2 = tan(horizontalLines[j][0] / 180 * CV_PI);//k =tan(theta)
				float b2 = horizontalLines[j][1];//intercept = b

				float x = intercept;
				float y = k2* x + b2;
				interscetionPoints.push_back(Point(x, y));
				cout << "(" << cvRound(x) << "," << cvRound(y) << ") ";
			}
		}
		else
		{
			float k1 = tan(verticalLines[i][0] / 180 * CV_PI);//k =tan(theta)
			float b1 = -verticalLines[i][1] * k1;//intercept = -b/k
			for (size_t j = 0; j < horizontalLines.size(); j++)
			{
				float k2 = tan(horizontalLines[j][0] / 180 * CV_PI);//k =tan(theta)
				float b2 = horizontalLines[j][1];//intercept = b

				float x = (b2 - b1) / (k1 - k2);
				float y = k1* x + b1;
				interscetionPoints.push_back(Point(x, y));
				cout << "(" << cvRound(x) << "," << cvRound(y) << ") ";
			}
		}
		cout << endl;
	}
	return interscetionPoints;
}


