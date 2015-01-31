#ifndef __IOMANAGER_H__
#define __IOMANAGER_H__

#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

class IOManager{
public:
	static IOManager* Instance();
	Mat ReadImage(string filename);

private:
	IOManager();
};
#endif 