#include "IOManager.h"

static IOManager* manager = nullptr;//µ¥Àý

IOManager::IOManager()
{

}

IOManager* IOManager::Instance()
{
	if (!manager)
	{
		manager = new IOManager();
	}
	return manager;
}

Mat IOManager::ReadImage(string filename)
{
	Mat image = imread(filename);
	if (image.empty())
	{
		cout << "can not open " << filename << endl;
		exit(0);
	}
	return image;
}

void IOManager::OutputResult(vector<Point> points, string filename)
{
	ofstream output(filename);
	if (!output)
	{
		cout << "can not open " << filename << endl;
		exit(0);
	}
	for (size_t i = 0; i < points.size(); i++)
	{
		int x = cvRound(points[i].x);
		int y = cvRound(points[i].y);
		output << "(" << x << "," << y << ") ";
		if((i+1) % 9 == 0)
			output << endl;
	}
	output.close();
}
