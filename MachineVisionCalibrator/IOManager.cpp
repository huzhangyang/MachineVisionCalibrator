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
	}
	return image;
}
