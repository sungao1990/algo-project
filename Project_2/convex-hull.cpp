#include <iostream>
#include <stdlib.h>
#include "gfx.hpp"
#include <vector>
#include <map>
#include <math.h>
#include <time.h>

#define POINT_QTY 500
#define pi M_PI

using namespace std;
class Point;

vector<Point> Points;
map<Point*,Point*> hull_vectors;   //key is the starting point, value is the ending point
vector<Point*> hull_points;
bool plot_hull(false);

double Ang_Norm(double a)
{
	if (a>pi)
	{
		while(a>pi)
		{
			a-=2*pi;
		}
	}
	else if(a<-pi)
	{
		while(a<-pi)
		{
			a+=2*pi;
		}
	}
	
	return a;
}

class Point
{
public:
	Point(double x, double y):
		x(x),y(y)
	{}
	
	bool Left_Side_Of(vector<Point>::iterator it1, vector<Point>::iterator it2)
	{
		double x1,x2,y1,y2,a1,a2,da;
		x1 = it1->x;
		y1 = it1->y;
		x2 = it2->x;
		y2 = it2->y;
		
		a1 = atan2(y2-y1, x2-x1);
		//cout << "a1=" << a1 << endl;
		a2 = atan2(y-y1, x-x1);
		//cout << "a2=" << a2 << endl;
		da = Ang_Norm(a2-a1);
		//cout << "da=" << da << endl;
		if (da<0)
			return false;
		else
			return true;
	}
	
public:
	double x;
	double y;
};


double rand_gen(double range)
{
	return (double)rand()/(double)RAND_MAX*range;
}





static void cb_draw()
{
	gfx::set_view(-0.5, -0.5, 1.5, 1.5);
	gfx::set_pen (3.0, 0.0, 0.0, 0.0, 1.0); // width, red, green, blue, alpha
	for (vector<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
	{
		gfx::draw_point(it->x, it->y);
	}
	if (plot_hull)
	{
		gfx::set_pen (1.0, 1.0, 0.0, 0.0, 1.0); // width, red, green, blue, alpha
		for (map<Point*,Point*>::iterator it=hull_vectors.begin(); it!=hull_vectors.end(); it++)
		{
			gfx::draw_line (it->first->x, it->first->y, it->second->x, it->second->y);
		}
		plot_hull = false;
	}
}

static void cb_mouse (double mx, double my, int flags)
{
	
}

static void cb_convex_hull_slow()
{
	hull_points.clear();
	hull_vectors.clear();
	
	clock_t start, finish;
    double duration;
    
    start = clock();
	
	//each vector starting from it1 and directing to it2
	for (vector<Point>::iterator it1=Points.begin(); it1!=Points.end(); it1++)
	{
		for (vector<Point>::iterator it2=Points.begin(); it2!=Points.end(); it2++)
		{
			if(it1 != it2)
			{
				bool is_hull_vector(true);
				//it3 is all the other points (excluding it1 and it2)
				for (vector<Point>::iterator it3=Points.begin(); it3!=Points.end(); it3++)
				{
					if(it3!=it1 && it3!=it2)
					{
						if (!it3->Left_Side_Of(it1,it2))
						{
							is_hull_vector = false;
							break;
						}
					}
				}
				if (is_hull_vector)
				{
					hull_vectors[&(*it1)] = &(*it2);
				}
			}
		}
	}
	plot_hull =true;
	
	
	hull_points.push_back(hull_vectors.begin()->first);
	hull_points.push_back(hull_vectors.begin()->second);
	
	while(1)
	{
		Point* Next = hull_vectors[hull_points.back()];
		if(Next == hull_points.front())
			break;
		hull_points.push_back(Next);
	}
	
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "Time: " << duration << "s" << endl;
	
	cout << "Hull length: " << hull_points.size() << endl;
	for(vector<Point*>::iterator it=hull_points.begin(); it!=hull_points.end(); it++)
	{
		
		cout << "(" << (*it)->x << "," << (*it)->y << ")";
		if(it!=hull_points.end()-1)
		{
			cout << "->" << endl;
		}
	}
	cout << endl;
}



static void cb_convex_hull_enhanced()
{
	
}



static void cb_shuffle()
{
	for (vector<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
	{
		double x = rand_gen(1);
		double y = rand_gen(1);
		it->x = x;
		it->y = y;
	}
}








int main()
{
	// Generate some random points
	for (int i=0; i<POINT_QTY; i++)
	{
		double x = rand_gen(1);
		double y = rand_gen(1);
		Points.push_back(Point(x,y));
	}
	
	// display them
	//cout << "Points Generated:" << endl;
	//for (vector<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
	//{
		//cout << "(" << it->x << ", " << it->y << ")" << endl;
	//}
	
		
	// This just enables (rather verbose) debug messages from the gfx
	// wrapper.  It is optional but can be rather useful.
	//gfx::debug (&cout);
	gfx::add_button("convex hull", cb_convex_hull_slow);
	gfx::add_button("convex hull ++", cb_convex_hull_enhanced);
	gfx::add_button("shuffle", cb_shuffle);

	gfx::main("convex hull slow", cb_draw, cb_mouse);

	return 0;
}
