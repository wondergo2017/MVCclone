#include "Cloner.h"


bool isdown = false;
vector<Point >bordervec;
void on_mouse(int e, int x, int y, int flags, void * img)
{
	if (isdown&&e == CV_EVENT_MOUSEMOVE)
	{
		cerr << x << " " << y << endl;
		bordervec.push_back(Point(x, y));
		((Mat*)img)->at<Vec3f>(y, x) = RED;
		imshow("source", *(Mat*)img);
	}
	else if (e == CV_EVENT_LBUTTONDOWN) {
		isdown = true;
	}
	else if (e == CV_EVENT_LBUTTONUP)
	{
		isdown = false;
	}

}
void getborder(const string &f,const string &output)
{
	Mat tmp = imread(f);
	tmp.convertTo(tmp, CV_32FC3, 1 / 255.);
	namedWindow("source");
	setMouseCallback("source", on_mouse, (void*)(&tmp));
	imshow("source", tmp);
	waitKey();
	
	set<Point> uniset;
	stack<Point> unistack;
	//unique
	for (auto e : bordervec) {
		unistack.push(e);
	}
	bordervec.clear();
	while (!unistack.empty())
	{
		Point tmp = unistack.top();
		if (find(bordervec.begin(), bordervec.end(), tmp) == bordervec.end())
		{
			bordervec.push_back(tmp);
		}
		unistack.pop();
	}
	//to file
	freopen(output.c_str(), "w", stdout);
	for (auto e : bordervec)
	{
		cout << e.x << " " << e.y << endl;
	}
	freopen("CON", "w", stdout);
}
void PutXY(Mat & m, int x, int y, const Vec3f & c)
{
	m.at<Vec3f>(y, x) = c;
}
Vec3f GetRGB(Mat& m, int x, int y)
{
	return m.at<Vec3f>(y, x);
}
Cloner::Cloner()
{
	//namedWindow("source");
	//namedWindow("target");
}


Cloner::~Cloner()
{
}

void Cloner::loadsource(const string & f)
{
	cerr << "source load!" << endl;
	src = imread(f);
	src.convertTo(src, CV_32FC3, 1 / 255.);
	sw = src.size().width;
	sh = src.size().height;
	
	cerr << "h:" << sh << " w:" << sw << endl;

	if (coords == nullptr)
	{
		coords = new Coord*[sh];
		for (int i = 0; i < sh; i++)
		{
			coords[i] = new Coord[sw];
		}
	}
	else
	{
		cerr << "get coord unconcerned!" << endl;
	}
}

void Cloner::loadtarget(const string & f)
{
	cerr << "target load!" << endl;
	tar = imread(f);
	tar.convertTo(tar, CV_32FC3, 1 / 255.);
	
}

void Cloner::getborder(const string & f)
{
	cerr << "getborder" << endl;
	freopen(f.c_str(), "r", stdin);
	int x, y;
	while (scanf("%d%d", &x, &y) != EOF)
	{
		bordervec.push_back(Point(x, y));
		//cout << x << " " << y << endl;
	}
	freopen("CON", "r", stdin);
}
