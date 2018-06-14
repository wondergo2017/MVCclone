
#include "Cloner.h"



int main()
{
	string source = "source-apple2.jpg";
	string border = "border-apple";
	string target = "target-blanket.jpg";

	
	//getborder(source,border);
	Cloner c1;
	c1.loadsource(source);
	//c1.showsource();
	
	c1.getborder(border);
	//c1.showborder();
	c1.getmesh();
	//c1.showmesh();
	c1.getcoord("coord");
	cerr << "input orgx orgy:" << endl;
	//c1.loadcoord("coord");
	int orgx, orgy;
	cin >> orgx >> orgy;
	c1.setOrg(Point(orgx, orgy));
	c1.loadtarget(target);
	c1.clone();

	c1.showres();
	c1.showorigin();
	c1.showmeshinres();

	system("pause");
	return 0;
}
