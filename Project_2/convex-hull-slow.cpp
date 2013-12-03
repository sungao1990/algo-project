#include <iostream>
#include <stdlib.h>
#include "gfx.hpp"
#include <list>

#define POINT_QTY 20

using namespace std;
class Point;

list<Point> Points;


class Point
{
public:
	Point(double x, double y):
		x(x),y(y)
	{}
	
	
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
	gfx::set_pen (10.0, 0.0, 0.0, 0.0, 1.0); // width, red, green, blue, alpha
	for (list<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
	{
		gfx::draw_point(it->x, it->y);
	}
}

static void cb_mouse (double mx, double my, int flags)
{
	
}

static void cb_convex_hull()
{
	
}

static void cb_shuffle()
{
	for (list<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
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
	cout << "Points Generated:" << endl;
	for (list<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
	{
		cout << "(" << it->x << ", " << it->y << ")" << endl;
	}
	
		
	// This just enables (rather verbose) debug messages from the gfx
	// wrapper.  It is optional but can be rather useful.
	gfx::debug (&cout);
	gfx::add_button("convex hull", cb_convex_hull);
	gfx::add_button("shuffle", cb_shuffle);

	gfx::main("convex hull slow", cb_draw, cb_mouse);

	return 0;
}
