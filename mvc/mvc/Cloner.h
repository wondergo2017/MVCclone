#pragma once
#include "pre.h"
void on_mouse(int e, int x, int y, int flags, void *img);
void getborder(const string &f,const string & output);
void PutXY(Mat & m, int x, int y, const Vec3f & c);
inline void PutXY(Mat &m, const Point & p, const Vec3f & c)
{
	PutXY(m, p.x, p.y,c);
}
Vec3f GetRGB(Mat& m, int x, int y);
inline Vec3f GetRGB(Mat& m, const Point &p)
{
	return GetRGB(m, p.x, p.y);
}

struct Iface
{
	Point p[3];
};
class Coord
{
public:
	bool isValid()
	{
		return !lambda.empty();
	}
	vector<double>lambda;

};

class Cloner
{
	
private:
	Mat src;
	Mat tar;
	Mat res;
	vector<Point> bordervec;

	CDT cdt;
	vector<Vertex_handle> handlevec;

	Point org;
	int sh, sw;

	Coord ** coords=nullptr;
	vector<Iface> faces;

	vector<vector<Point> > meshvec;//the first three points are the face's

	vector<Vec3f> borderrgb;
	vector<Vec3f > tarborderrgb;
protected:
	Point ToTarget(const Point& p)
	{
		return p + org;
	}
	bool IsValid(const Point & p)
	{
		return p.x >= 0 && p.x < sw && p.y >= 0 && p.y < sh;
	}
	vector<double > Getlambda(Point p, const vector<Point> & curve)
	{
		vector<double > lambda(curve.size());

		int cagesize = curve.size();
		for (int i = 0; i < cagesize; i++)
		{
			lambda[i] = 0;
		}
		for (int i = 0; i < cagesize; i++)
		{
			double p1x = curve[i].x;

			double p1y = curve[i].y;

			int i2 = (int)(i + 1) % cagesize;

			double p2x = curve[i2].x;

			double p2y = curve[i2].y;

			double dx1 = (p1x - p.x); double dy1 = (p1y - p.y);

			double dx2 = (p2x - p.x); double dy2 = (p2y - p.y);

			double len1 = sqrt((double)dx1*dx1 + dy1 * dy1);

			if (len1 < 1) {

				for (int j = 0; j< cagesize; ++j) { lambda[j] = 0; }

				lambda[i] = 1;

				return lambda;

			}

			double len2 = sqrt((double)dx2*dx2 + dy2 * dy2);

			if (len2 < 1) {

				for (int j = 0; j< cagesize; ++j) { lambda[j] = 0; }

				lambda[i2] = 1;

				return lambda;

			}

			double cosine = abs(((dx1*dx2) + (dy1*dy2)) / (len1*len2));
			double tet = acos(cosine);
			double tana2 = tan(tet / 2);
			lambda[i] +=  tana2 / len1;

			lambda[i2] +=  tana2 / len2;
		}
		//normalize
		double sum = 0;
		for (auto e : lambda)
		{
			sum += e;
		}
		for (int i = 0; i < lambda.size(); i++)
		{
			lambda[i] /= sum;
		}
		return lambda;
	}
	vector<double >	Getlambda(Point p, const Iface& face)
	{
		vector<Point> vtmp;
		for (int i = 0; i < 3; i++)
		{
			vtmp.push_back(face.p[i]);
		}
		return Getlambda(p, vtmp);
	}
	bool isintriangle(const Point& m,const Point &p1,const Point &p2,const Point & p3)
	{
		Vec3f m1((p1 - m).x,(p1-m).y,0);
		Vec3f m2((p2 - m).x, (p2 - m).y,0); 
		Vec3f m3((p3 - m).x, (p3 - m).y,0);
		Vec3f mm1 = m1.cross(m2);
		Vec3f mm2 = m2.cross(m3);
		Vec3f mm3 = m3.cross(m1);
		float s1 = mm1.dot(mm2);
		float s2 = mm2.dot(mm3);
		float s3 = mm3.dot(mm1);
		if (s1 > 0 && s2 > 0 && s3 > 0 || s1 < 0 && s2 < 0 && s3 < 0)return true;
		return false;
	}
	bool isintriangle(const Point& m, const Iface & f)
	{
		return isintriangle(m, f.p[0], f.p[1], f.p[2]);
	}
	void Floodfill(Point pnow,Mat &Floodmat, vector<Point> &border, vector<Point> &inerpoints) {
		if (!IsValid(pnow) || GetRGB(Floodmat, pnow) == FLOODBORDER || GetRGB(Floodmat, pnow) == FLOODOUT|| GetRGB(Floodmat, pnow)==FLOODED) {
			return;
		}
		queue<Point> q;
		Point attempts[] = { Point(0,1),Point(1,0),Point(-1,0),Point(0,-1) };
		q.push(pnow);
		while (!q.empty())
		{
			Point pnow = q.front();
			q.pop();
			for (int i = 0; i < 4; i++)
			{
				Point tmp = pnow + attempts[i];
				if ((!IsValid(tmp))|| GetRGB(Floodmat, tmp) == FLOODOUT|| GetRGB(Floodmat, tmp)==FLOODED) {
					continue;
				}
				if (GetRGB(Floodmat, tmp) == FLOODBORDER)
				{
					PutXY(Floodmat, tmp, FLOODED);
					border.push_back(tmp);
					continue;
				}
				q.push(tmp);
				inerpoints.push_back(tmp);
				PutXY(Floodmat, tmp, FLOODED);
			}
		}
	}
	void Floodfill(Point pnow, Mat&Floodmat)
	{
		queue<Point> q;
		Point attempts[] = { Point(0,1),Point(1,0),Point(-1,0),Point(0,-1) };
		if (!IsValid(pnow)||GetRGB(Floodmat, pnow) == FLOODBORDER||GetRGB(Floodmat,pnow)==FLOODOUT) {
			return;
		}
		q.push(pnow);
		while (!q.empty())
		{
			Point pnow = q.front();
			q.pop();
			for (int i = 0; i < 4; i++)
			{
				Point tmp = pnow + attempts[i];
				if ((!IsValid(tmp)) || GetRGB(Floodmat, tmp) == FLOODBORDER || GetRGB(Floodmat, tmp) == FLOODOUT) {
					continue;
				}
				q.push(tmp);
				PutXY(Floodmat, tmp, FLOODOUT);
			}
		}
	}
	void SaveCoord(const string &f,int lambdasize)
	{
		cerr << "saving coord!" << endl;
		freopen(f.c_str(), "w", stdout);
		cout << sw << " " << sh << " " << lambdasize << endl;
		for (int x = 0; x < sw; x++)
		{
			for (int y = 0; y < sh; y++)
			{
				if (coords[y][x].isValid())
				{
					cout << "T ";
					for (auto e : coords[y][x].lambda)
					{
						cout << e << " ";
					}
					cout << endl;
				}
				else {
					cout << "F" << endl;
				}
			}
		}
	}
	
	void SaveMesh(const string &f)
	{
		cerr << "saving mesh!" << endl;
		freopen(f.c_str(), "w", stdout);
		for (auto face : meshvec)
		{
			cout << face.size() << endl;
			for (auto e : face)
			{
				cout << e.x << " " << e.y << " ";
			}
			cout << endl;
		}
		freopen("CON", "w", stdout);
	}
	void LoadMesh(const string &f)
	{
		cerr << "Load mesh£¡" << endl;
		freopen(f.c_str(), "r", stdin);
		int size = 0;
		while (scanf("%d", &size) != EOF)
		{
			vector<Point> tmp;
			int xtmp, ytmp;
			for (int i = 0; i < size; i++)
			{
				cin >> xtmp;
				cin >> ytmp;
				tmp.push_back(Point(xtmp, ytmp));
			}
			meshvec.push_back(tmp);
		}
	}
	void LoadCoord(const string &f)
	{
		cerr << "loading coord!" << endl;
		freopen(f.c_str(), "r", stdin);
		int sw, sh, lambdasize;
		cin >> sw >> sh >> lambdasize;
		for (int x = 0; x < sw; x++)
		{
			for (int y = 0; y < sh; y++)
			{
				string valid;
				cin >> valid;
				if (valid == "T")
				{
					double tmp;
					for (int i = 0; i < lambdasize; i++)
					{
						cin >> tmp;
						coords[y][x].lambda.push_back(tmp);
					}
				}
				else {
					continue;
				}
			}
		}
	}

public:

	Cloner();
	~Cloner();
	void setOrg(const Point &p)
	{
		org = p;
	}
	void loadsource(const string & f);
	void showsource()
	{
		namedWindow("source");
		imshow("source",src);
		waitKey();
	}
	void loadtarget(const string & f);
	void getborder(const string &f);
	void showborder()
	{
		Mat tmp;
		src.copyTo(tmp);
		for (auto e : bordervec)
		{
			PutXY(tmp, e.x, e.y, RED);
		}
		namedWindow("border");
		imshow("border", tmp);
		waitKey();
	}
	void getmesh()
	{
		handlevec.clear();
		for (auto e : bordervec)
		{
			handlevec.push_back(Vertex_handle(cdt.insert(Pointcdt(e.x, e.y))));
		}
		for (int i = 0, size = handlevec.size(); i < size; i++)
		{
			cdt.insert_constraint(handlevec[i], handlevec[(i + 1) % size]);
		}
		std::cerr << "Number of vertices: " << cdt.number_of_vertices() << std::endl;
		std::cerr << "Meshing the triangulation with default criterias..."
			<< std::endl;
		Mesher mesher(cdt);
		mesher.refine_mesh();
		std::cerr << "Number of vertices: " << cdt.number_of_vertices() << std::endl;
		std::cerr << "Meshing with new criterias..." << std::endl;
		// 0.125 is the default shape bound. It corresponds to abound 20.6 degree.
		// 0.5 is the upper bound on the length of the longuest edge.
		// See reference manual for Delaunay_mesh_size_traits_2<K>.
		//mesher.set_criteria(Criteria(0.125, 0.5));
		//mesher.refine_mesh();
		std::cerr << "Number of vertices: " << cdt.number_of_vertices() << std::endl;
	}
	void showmesh()
	{
		cerr << "show mesh!" << endl;
		Mat tmp;
		src.copyTo(tmp);
		for (auto it = cdt.all_faces_begin(); it != cdt.all_faces_end(); it++)
		{
			Point p[3];
			for (int i = 0; i < 3; i++)
			{
				Pointcdt tmp = it->vertex(i)->point();
				p[i] = Point(tmp.x(), tmp.y());
			}
			for (int i = 0; i < 3; i++)
			{
				if (IsValid(p[i]) && IsValid(p[(i + 1) % 3]))
				{
					line(tmp, p[i], p[(i + 1) % 3], RED);
				}
			}
		}
		namedWindow("mesh in src");
		imshow("mesh in src", tmp);
		waitKey();
	}
	void getcoord(const string & f)
	{
		cerr << "getcoord!" << endl;
		Mat floodmat(sh, sw, CV_32FC3, Scalar(0, 0, 0));
		//0 get all the finite faces
		for (auto it = cdt.all_faces_begin(); it != cdt.all_faces_end(); it++)
		{
			//get the finite faces
			int isfinite = true;
			Point p[3];
			for (int i = 0; i < 3; i++)
			{
				Pointcdt tmp = it->vertex(i)->point();
				p[i] = Point(tmp.x(), tmp.y());
				if (!IsValid(p[i])) {
					isfinite = false;
					break;
				}
			}
			if (!isfinite) continue;
			Iface ftmp;
			for (int i = 0; i < 3; i++)
			{
				ftmp.p[i] = p[i];
			}
			faces.push_back(ftmp);
		}
		//1 get all the points and the egdes on the flood mat
		for (auto face : faces)
		{
			for (int i = 0; i < 3; i++)
			{
				line(floodmat, face.p[i], face.p[(i + 1) % 3], FLOODBORDER);
			}
		}
		//2 flood outside
		Floodfill(Point(0, 0), floodmat);
		//3 flood to get the lambda
		//3.1 get coords of the mesh points
		for (auto face : faces)
		{
			for (int i = 0; i < 3; i++)
			{
				Point *p = face.p;
				if (!coords[p[i].y][p[i].x].isValid())// if not done
				{
					coords[p[i].y][p[i].x].lambda = Getlambda(p[i], bordervec);
				}
			}
		}
		//3.2 get coords of the inerpoints
		vector<Point> border;
		vector<Point> inerpoints;
		for (int x = 0; x < sw; x++)
		{
			for (int y = 0; y < sh; y++)
			{
				border.clear();
				inerpoints.clear();
				Floodfill(Point(x, y), floodmat, border, inerpoints);
				if (!inerpoints.empty())
				{
					//1 find the face the points belong to
					Point leader;
					Iface fleader;
					
					leader = inerpoints[0];
					for (auto face : faces)
					{
						if (isintriangle(leader, face))
						{
							fleader = face;
							break;
						}
					}
					//2 get the mesh
					vector<Point >tmp;
					for (int i = 0; i < 3; i++)
					{
						tmp.push_back(fleader.p[i]);
					}
					for (auto p : inerpoints)
					{
						tmp.push_back(p);
					}
					for (auto p : border)
					{
						tmp.push_back(p);
					}
					meshvec.push_back(tmp);
					//else {//border is not empty
					//	cout << "border to find fleader,unconcerned!" << endl;
					//	continue;
					//}
					////2 get the lamba of the inerpoints
					//for (auto e : inerpoints)
					//{
					//	vector<double> tmp = Getlambda(e, fleader);
					//	vector<double> &co = coords[e.y][e.x].lambda;
					//	Point *p = fleader.p;
					//	co = coords[p[0].y][p[0].x].lambda;
					//	for (int i = 0; i < co.size(); i++)
					//	{
					//		co[i] *= tmp[0];
					//		for (int j = 1; j < 3; j++)
					//		{
					//			co[i] += tmp[j] * coords[p[j].y][p[j].x].lambda[i];
					//		}
					//	}
					//}
					////3 get the lamba of the borderpoints
					//for (auto e : border)
					//{
					//	vector<double> tmp = Getlambda(e, fleader);
					//	vector<double> &co = coords[e.y][e.x].lambda;
					//	Point *p = fleader.p;
					//	co = coords[p[0].y][p[0].x].lambda;
					//	for (int i = 0; i < co.size(); i++)
					//	{
					//		co[i] *= tmp[0];
					//		for (int j = 1; j < 3; j++)
					//		{
					//			co[i] += tmp[j] * coords[p[j].y][p[j].x].lambda[i];
					//		}
					//	}
					//}
				}
			}
		}
		//4 get all the special points
		for (int x = 0; x < sw; x++)
		{
			for (int y = 0; y < sh; y++)
			{
				if (GetRGB(floodmat, x, y) == FLOODBORDER)
				{
					coords[y][x].lambda = Getlambda(Point(x,y), bordervec);
				}
			}
		}
		//save
		//SaveCoord(f, bordervec.size());
		//SaveMesh(f + "-mesh");
		namedWindow("Flood");
		imshow("Flood", floodmat);
		waitKey();
	}
	void loadcoord(const string &f) {
		LoadCoord(f);
		LoadMesh(f + "-mesh");
	}
	void showtarget()
	{
		namedWindow("target");
		imshow("target", tar);
		waitKey();
	}

	void clone()
	{
		//0 get membrane mat
		//prepare the res
		tar.copyTo(res);
		Mat membrane;
		src.copyTo(membrane);
		for (int i = 0; i < sw; i++)
		{
			for (int j = 0; j < sh; j++)
			{
				PutXY(membrane, i, j, BLACK);
			}
		}
		//1 get the rgb of the border of the target
		for (auto p:bordervec)
		{
			tarborderrgb.push_back(GetRGB(tar, ToTarget(p)));
		}
		for (auto p : bordervec)
		{
			borderrgb.push_back(GetRGB(src, p));
		}
		//2 get the rgb of the meshpoint of the source
		for (auto face:meshvec)
		{
			vector<Point> p;
			for (int i = 0; i < 3; i++)
			{
				p.push_back(face[i]);
				vector<double> &lambda = coords[p[i].y][p[i].x].lambda;
				//cerr << "size:" << lambda.size() << " " << tarborderrgb.size() << endl;
				Vec3f rgb(BLACK);
				for (int j = 0; j < lambda.size(); j++)
				{
					rgb += lambda[j] * (tarborderrgb[j]-borderrgb[j]);
				}
				PutXY(membrane,p[i], rgb);
			}
			//3 get the rgb of the other points of the source
			for (int i = 3; i < face.size(); i++)
			{
				Point &pp = face[i];
				//if (GetRGB(membrane, pp) != BLACK) continue;
				vector<double> lambda = Getlambda(pp, p);
				Vec3f rgb(BLACK);
				for (int j = 0; j < lambda.size(); j++)
				{
					rgb += lambda[j] * GetRGB(membrane, p[j]);
				}
				//PutXY(tar, ToTarget(pp), rgb + GetRGB(src, pp));
				PutXY(membrane, pp, rgb );
			}
		}

		//4 get the special points rgbs
		for (int x = 0; x < sw; x++)
		{
			for (int y = 0; y < sh; y++)
			{
				if (GetRGB(membrane, x, y) == BLACK && coords[y][x].isValid())
				{
					vector<double> &lambda = coords[y][x].lambda;
					Vec3f rgb(BLACK);
					for (int j = 0; j < lambda.size(); j++)
					{
						rgb += lambda[j] * (tarborderrgb[j] - borderrgb[j]);
					}
					PutXY(membrane, x,y, rgb);
				}
			}
		}
		//5 add the rgb of the membrane to the target
		for (int x = 0; x < sw; x++)
		{
			for (int y = 0; y < sh; y++)
			{
				Vec3f mc = GetRGB(membrane, x, y);
				if(mc!=BLACK||coords[y][x].isValid())
				PutXY(tar, ToTarget(Point(x,y)), mc+GetRGB(src,x,y));
			}
		}
		namedWindow("membrane");
		imshow("membrane", membrane);
		waitKey();
		
	}
};

