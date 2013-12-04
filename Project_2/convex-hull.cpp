#include <iostream>
#include <stdlib.h>
#include "gfx.hpp"
#include <vector>
#include <list>
#include <map>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#define POINT_QTY 20
#define pi M_PI


using namespace std;
class Point;

vector<Point> Points;
map<Point*,Point*> hull_vectors;   //key is the starting point, value is the ending point
vector<Point*> hull_points;
list<Point*> L_upper;
list<Point*> L_lower;
bool plot_hull(false);
double x_min, x_max, y_min, y_max;



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










bool Right_Turn(Point* P1, Point* P2, Point* P3)
{
	double a1,a2,da;
	a1 = atan2(P2->y-P1->y, P2->x-P1->x);
	a2 = atan2(P3->y-P2->y, P3->x-P2->x);
	da = Ang_Norm(a2-a1);
	if(da > 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}




double rand_gen (double range)
{
	static struct timeval t0 = { 0, 0 };
	if (0 == t0.tv_sec)
	{
		gettimeofday (&t0, NULL);
		srand (t0.tv_usec);
	}
	return (double)rand()/(double)RAND_MAX*range;
}



double rand_gen (double minval, double maxval)
{
  	static struct timeval t0 = { 0, 0 };
	if (0 == t0.tv_sec)
	{
		gettimeofday (&t0, NULL);
		srand (t0.tv_usec);
	}
	
	if(minval > maxval)
	{
		double temp = minval;
		minval = maxval;
		maxval = temp;
	}
	
	if(minval == maxval)
	{
		return minval;
	}
	else
	{
		double range = maxval - minval;
		return (double)rand()/(double)RAND_MAX*range+minval;
	}
}



void Print_Hull()
{
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




static void cb_draw()
{
	
	
	gfx::set_view(x_min-0.5, y_min-0.5, x_max+0.5, y_max+0.5);
	
	
	gfx::set_pen (10.0, 0.0, 0.0, 0.0, 1.0); // width, red, green, blue, alpha
	for (vector<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
	{
		gfx::draw_point(it->x, it->y);
	}
	
	if (plot_hull)
	{
		gfx::set_pen (1.0, 1.0, 0.0, 0.0, 1.0); // width, red, green, blue, alpha
		
		for (vector<Point*>::iterator it=hull_points.begin(); it!=hull_points.end(); it++)
		{
			if(it!=--hull_points.end())
			{
				vector<Point*>::iterator next = it + 1;
				gfx::draw_line((*it)->x, (*it)->y, (*next)->x, (*next)->y);
			}
			else
			{
				vector<Point*>::iterator next = hull_points.begin();
				gfx::draw_line((*it)->x, (*it)->y, (*next)->x, (*next)->y);
			}
		}
		
		plot_hull = false;
	}
	
	/*
	gfx::set_pen (15.0, 1.0, 0.0, 0.0, 1.0);
	for(list<Point*>::iterator it=L_upper.begin(); it!=L_upper.end(); it++)
	{
		gfx::draw_point((*it)->x, (*it)->y);
	}
	
	gfx::set_pen (15.0, 0.0, 1.0, 0.0, 1.0);
	for(list<Point*>::iterator it=L_lower.begin(); it!=L_lower.end(); it++)
	{
		gfx::draw_point((*it)->x, (*it)->y);
	}*/
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
	
	Print_Hull();
	
	plot_hull =true;
}



static void cb_convex_hull_enhanced()
{
	L_upper.clear();
	L_lower.clear();
	hull_points.clear();
	clock_t start, finish;
    double duration;
    
    start = clock();
    
    map<double,Point*> Points_SortedInX;
    for (vector<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
    {
    	Points_SortedInX[it->x] = &(*it);
    }
	
	
	map<double,Point*>::iterator it_map_p = Points_SortedInX.begin();
	L_upper.push_back(it_map_p->second); it_map_p++;
	L_upper.push_back(it_map_p->second); it_map_p++;
	
	for ( ; it_map_p!=Points_SortedInX.end(); it_map_p++)
	{
		L_upper.push_back(it_map_p->second);
		list<Point*>::iterator it_l = L_upper.end(); it_l--;
		Point* P3 = *it_l; it_l--;
		Point* P2 = *it_l; it_l--;
		Point* P1 = *it_l;
		while(L_upper.size()>2 && !(Right_Turn(P1,P2,P3)))
		{
			it_l = L_upper.end();
			it_l--; it_l--;
			L_upper.erase(it_l);
			
			if(L_upper.size()>2)
			{
				it_l = L_upper.end(); it_l--;
				P3 = *it_l; it_l--;
				P2 = *it_l; it_l--;
				P1 = *it_l;
			}
		}
	}
	
	
	it_map_p = Points_SortedInX.end(); it_map_p--;
	L_lower.push_back(it_map_p->second); it_map_p--;
	L_lower.push_back(it_map_p->second); it_map_p--;
	for ( ; it_map_p!=--Points_SortedInX.begin(); it_map_p--)
	{
		L_lower.push_back(it_map_p->second);
		list<Point*>::iterator it_l = L_lower.end(); it_l--;
		Point* P3 = *it_l; it_l--;
		Point* P2 = *it_l; it_l--;
		Point* P1 = *it_l;
		while(L_lower.size()>2 && !(Right_Turn(P1,P2,P3)))
		{
			it_l = L_lower.end();
			it_l--; it_l--;
			L_upper.erase(it_l);
			
			if(L_lower.size()>2)
			{
				it_l = L_lower.end(); it_l--;
				P3 = *it_l; it_l--;
				P2 = *it_l; it_l--;
				P1 = *it_l;
			}
		}
	}
	
	L_lower.erase(L_lower.begin());
	L_lower.erase(--L_lower.end());
	
	L_upper.splice(L_upper.end(),L_lower);
	
	for (list<Point*>::iterator it=L_upper.begin(); it!=L_upper.end(); it++)
	{
		hull_points.push_back(*it);
	}
	
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "Time: " << duration << "s" << endl;
	
	Print_Hull();
	
	plot_hull =true;
}



static void cb_shuffle()
{
	hull_points.clear();
	hull_vectors.clear();
	L_upper.clear();
	L_lower.clear();
	
	for (vector<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
	{
		double Rand_C_r  = rand_gen(rand_gen(5));
		double Rand_C_th = rand_gen(rand_gen(4*pi));
		double r = rand_gen(rand_gen(10));
		double th = rand_gen(rand_gen(4*pi));
		double xx = Rand_C_r*cos(Rand_C_th) + r*cos(th);
		double yy = Rand_C_r*sin(Rand_C_th) + r*sin(th);
		it->x = xx;
		it->y = yy;
	}
	
	x_min = Points.begin()->x;
	x_max = Points.begin()->x; 
	y_min = Points.begin()->y; 
	y_max = Points.begin()->y;
	
	for (vector<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
	{
		if(x_min > it->x) x_min = it->x;
		if(x_max < it->x) x_max = it->x;
		if(y_min > it->y) y_min = it->y;
		if(y_max < it->y) y_max = it->y;
	}
	
}






int main()
{
	// Generate some random points
	for (int i=0; i<POINT_QTY; i++)
	{
		double Rand_C_r  = rand_gen(rand_gen(5));
		double Rand_C_th = rand_gen(rand_gen(4*pi));
		double r = rand_gen(rand_gen(10));
		double th = rand_gen(rand_gen(4*pi));
		double xx = Rand_C_r*cos(Rand_C_th) + r*cos(th);
		double yy = Rand_C_r*sin(Rand_C_th) + r*sin(th);
		Points.push_back(Point(xx,yy));
	}
	
	x_min = Points.begin()->x;
	x_max = Points.begin()->x; 
	y_min = Points.begin()->y; 
	y_max = Points.begin()->y;
	
	for (vector<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
	{
		if(x_min > it->x) x_min = it->x;
		if(x_max < it->x) x_max = it->x;
		if(y_min > it->y) y_min = it->y;
		if(y_max < it->y) y_max = it->y;
	}
	
	
	
		
	// This just enables (rather verbose) debug messages from the gfx
	// wrapper.  It is optional but can be rather useful.
	//gfx::debug (&cout);
	gfx::add_button("convex hull", cb_convex_hull_slow);
	gfx::add_button("convex hull ++", cb_convex_hull_enhanced);
	gfx::add_button("shuffle", cb_shuffle);

	gfx::main("convex hull slow", cb_draw, cb_mouse);

	return 0;
}
