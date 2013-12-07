#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "gfx.hpp"
#include <vector>
#include <list>
#include <map>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#define POINT_QTY 10
#define pi M_PI


using namespace std;
class Point;

vector<Point> Points;
map<Point*,Point*> hull_vectors;   //key is the starting point, value is the ending point
vector<Point*> hull_points;
Point* P1_watching;
Point* P2_watching;
vector<Point>::iterator it1_watching;
vector<Point>::iterator it2_watching;
bool plot_hull(false);
bool plot_step(false);
double x_min, x_max, y_min, y_max;
bool is_hull_vector(true);
bool step_finish(false);


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



double get_time_usec()
{
	struct timeval tv;
	struct timezone tz;
	struct tm *tm;
	gettimeofday(&tv, &tz);
	tm=localtime(&tv.tv_sec);
	
	double h = (double)tm->tm_hour;
	double m = (double)tm->tm_min;
	double s = (double)tm->tm_sec;
	double u = (double)tv.tv_usec;
	
	return (h*60*60*1000000+m*60*1000000+s*1000000+u);
}

void Show_running_time(double t1, double t2)
{
	if(t1>t2)
	{
		t2 += 24*60*60*1000000;
	}
	double duration = t2-t1;
	if(duration >= 1000000)
	{
		int s  = (int)(duration / 1000000);
		double us = duration-(double)s*1000000;
		cout << "Time: " << s << " " << us << "us" << endl;
	}
	else
	{
		cout << "Time: " << duration << "us" << endl;
	}
	cout << "--------------------------------------------------" << endl;
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
	
	
	
	if(plot_step)
	{
		plot_step = false;
		for (map<Point*,Point*>::iterator it=hull_vectors.begin(); it!=hull_vectors.end(); it++)
		{
			gfx::set_pen (1.0, 0.0, 1.0, 0.0, 1.0); // blue
			gfx::draw_line (it->first->x, it->first->y, it->second->x, it->second->y);
		}
		if(!step_finish)
		{
			gfx::set_pen (1.0, 1.0, 0.0, 1.0, 1.0);
			gfx::draw_arc (P1_watching->x, P1_watching->y, 0.5, 0, 2*pi);
			gfx::set_pen (1.0, 0.0, 1.0, 1.0, 1.0);
			gfx::draw_arc (P2_watching->x, P2_watching->y, 0.5, 0, 2*pi);
			if(is_hull_vector)
			{
				gfx::set_pen (1.0, 0.0, 1.0, 0.0, 1.0); // green
			}
			else
			{
				gfx::set_pen (1.0, 1.0, 0.0, 0.0, 1.0); // red
			}
			double x1, y1, x2, y2;
			if(P1_watching->x == P2_watching->x)
			{
				x1 = P1_watching->x;
				x2 = P2_watching->x;
				y1 = y_min-10;
				y2 = y_max+10;
			}
			else
			{
				double k = (P2_watching->y-P1_watching->y)/(P2_watching->x-P1_watching->x);
				x1 = x_min-10;
				x2 = x_max+10;
				y1 = P1_watching->y+k*(x1-P1_watching->x);
				y2 = P1_watching->y+k*(x2-P1_watching->x);
			}
			gfx::draw_line (x1, y1, x2, y2);
		}
	}
	
	
	
	
	if (plot_hull)
	{
		plot_hull = false;
		for (vector<Point*>::iterator it=hull_points.begin(); it!=hull_points.end(); it++)
		{
			gfx::set_pen (1.0, 1.0, 0.0, 0.0, 1.0); // width, red, green, blue, alpha
			gfx::draw_arc ((*it)->x, (*it)->y, 0.5, 0, 2*pi);
			
			gfx::set_pen (1.0, 0.0, 0.0, 1.0, 1.0); // width, red, green, blue, alpha
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
	}
}

static void cb_mouse (double mx, double my, int flags)
{
	
}

static void cb_convex_hull_P3()
{
	hull_points.clear();
	hull_vectors.clear();
	
	double t_start, t_finish;
    
    t_start = get_time_usec();
	
	//each vector starting from it1 and directing to it2
	for (vector<Point>::iterator it1=Points.begin(); it1!=Points.end(); it1++)
	{
		for (vector<Point>::iterator it2=Points.begin(); it2!=Points.end(); it2++)
		{
			if(it1 != it2)
			{
				is_hull_vector = true;
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
	
	t_finish = get_time_usec();
	
	Print_Hull();
	
	cout << endl;
	
	Show_running_time(t_start, t_finish);
	
	plot_hull =true;
}





static void cb_init()
{
	hull_points.clear();
	hull_vectors.clear();
	it1_watching = Points.begin();
	it2_watching = Points.begin();
	step_finish = false;
	cout << "initialization done!" << endl;
}





static void cb_step()
{
	plot_step = true;
	if(step_finish == false)
	{
		bool sth_is_done(false);
		while(!sth_is_done)
		{
			P1_watching = &(*it1_watching);
			P2_watching = &(*it2_watching);
			if(it1_watching != it2_watching)
			{
				sth_is_done = true;
				is_hull_vector = true;
				//it3 is all the other points (excluding it1 and it2)
				for (vector<Point>::iterator it3=Points.begin(); it3!=Points.end(); it3++)
				{
					if(it3!=it1_watching && it3!=it2_watching)
					{
						if (!it3->Left_Side_Of(it1_watching,it2_watching))
						{
							is_hull_vector = false;
							break;
						}
					}
				}
				if (is_hull_vector)
				{
					hull_vectors[&(*it1_watching)] = &(*it2_watching);
				}
			}
	
			it2_watching++;
			if(it2_watching==Points.end())
			{
				it2_watching = Points.begin();
				it1_watching++;
				if(it1_watching==Points.end())
				{
					step_finish = true;
				}
			}
		}
	}
	else
	{
		cout << "Finished!" << endl;
	}
}







static void cb_shuffle()
{
	hull_points.clear();
	hull_vectors.clear();
		
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
	cb_init();
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
	
	cb_init();
	
	
	// This just enables (rather verbose) debug messages from the gfx
	// wrapper.  It is optional but can be rather useful.
	//gfx::debug (&cout);
	gfx::add_button("convex hull P3", cb_convex_hull_P3);
	gfx::add_button("shuffle", cb_shuffle);
	gfx::add_button("reset",cb_init);
	gfx::add_button("step",cb_step);
	gfx::main("convex hull slow", cb_draw, cb_mouse);

	return 0;
}
