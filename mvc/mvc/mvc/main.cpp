
#include "Cloner.h"



int main()
{
	//getborder("source.jpg","border");
	Cloner c1;
	
	
	c1.loadsource("source.jpg");
	//c1.showsource();
	
	c1.getborder("border");
	//c1.showborder();
	c1.getmesh();
	//c1.showmesh();
	c1.getcoord("coord");

	//c1.loadcoord("coord");
	c1.setOrg(Point(400, 300));
	c1.loadtarget("target.jpg");
	c1.clone();
	c1.showres();
	c1.showorigin();
	c1.showmeshinres();

	system("pause");
	return 0;
}
