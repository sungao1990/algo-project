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

#define POINT_QTY 100
#define pi M_PI


using namespace std;
class Point;

vector<Point> Points;
map<Point*,Point*> hull_vectors;   //key is the starting point, value is the ending point
vector<Point*> hull_points;
double a_plot_step;
bool finish(false);
bool reset(false);
bool plot_step(false);
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
	
	
	if (plot_step)
	{
		plot_step = false;
		for (vector<Point*>::iterator it=hull_points.begin(); it!=hull_points.end(); it++)
		{
			gfx::set_pen (1.0, 1.0, 0.0, 0.0, 1.0); // width, red, green, blue, alpha
			gfx::draw_arc ((*it)->x, (*it)->y, 0.5, 0, 2*pi);
		}
		if(hull_points.size()>1)
		{
			for (vector<Point*>::iterator it=hull_points.begin(); it!=hull_points.end(); it++)
			{
				if(it!=--hull_points.end())
				{
					vector<Point*>::iterator next = it + 1;
					gfx::draw_line((*it)->x, (*it)->y, (*next)->x, (*next)->y);
				}
			}
		}
		if(!finish)
		{
			double x = hull_points.back()->x;
			double y = hull_points.back()->y;
			cout << "x=" << x << ", y=" << y << endl;
			double x1,x2,y1,y2;
			if(a_plot_step==pi/2 || a_plot_step==-pi/2)
			{
				x1 = x;
				x2 = x;
				y1 = y_min-10;
				y2 = y_max+10;
			}
			else
			{
				x1 = x_min-10;
				x2 = x_max+10;
				y1 = tan(a_plot_step)*(x1-x)+y;
				y2 = tan(a_plot_step)*(x2-x)+y;
			}
			gfx::set_pen (1.0, 0.0, 0.0, 1.0, 1.0); // width, red, green, blue, alpha
			gfx::draw_line (x1, y1, x2, y2);
			
			cout << a_plot_step << endl;
			cout << x1 << "\t" << y1 << "\t" << x2 << "\t" << y2 << endl;
		}
		else
		{
			gfx::draw_line(hull_points.back()->x, hull_points.back()->y, hull_points.front()->x, hull_points.front()->y);
		}
		//cout << "finish=" << finish << endl;
	}
	
	
	
	if (plot_hull)
	{
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
		
		plot_hull = false;
	}
}

static void cb_mouse (double mx, double my, int flags)
{
	
}





static void cb_convex_hull_1_7()
{
	hull_points.clear();
	double t_start, t_finish;
    
    t_start = get_time_usec();
    
    Point* P_rm = &(*Points.begin());
    vector<Point>::iterator it_rm = Points.begin();
    // find the rightmost point
    for (vector<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
    {
    	if(P_rm->x < it->x)
    	{
    		P_rm = &(*it);
    		it_rm = it;
    	}
    }
    hull_points.push_back(P_rm);
    
    double a_ref = -pi/2;
    
    Point* P = P_rm;
    Point* P_next;
    double da_m;
    
    while(1)
    {
    	da_m = -pi;
		for (vector<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
		{
			if(&(*it)!=P)
			{
				double a = atan2(it->y-P->y,it->x-P->x);
				double da = Ang_Norm(a-a_ref);
				if(da>da_m && da<=0)
				{
					P_next = &(*it);
					da_m = da;
				}
			}
		}
		if(P_next == P_rm)  break;
		hull_points.push_back(P_next);
		a_ref = atan2(P_next->y-P->y,P_next->x-P->x);
		P = P_next;
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
	reset = true;
	finish = false;
	cout << "initialization done!" << endl;
}




static void cb_step()
{
	static char state = 0;
	static Point* P_rm;
	static Point* P;
    static Point* P_next;
    static double a_ref;
	plot_step = true;
	
	if(reset)
	{
		reset = false;
		state = 0;
	}
	
	switch(state)
	{
		case 0:
		{
			P_rm = &(*Points.begin());
			
			// find the rightmost point
			for (vector<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
			{
				if(P_rm->x < it->x)
				{
					P_rm = &(*it);
				}
			}
			hull_points.push_back(P_rm);
			P = P_rm;
			a_ref = -pi/2;
			a_plot_step = a_ref;
			state = 1;
			break;
		}
		case 1:
		{
			double da_m = -pi;
			for (vector<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
			{
				if(&(*it)!=P)
				{
					double a = atan2(it->y-P->y,it->x-P->x);
					double da = Ang_Norm(a-a_ref);
					if(da>da_m && da<=0)
					{
						P_next = &(*it);
						da_m = da;
					}
				}
			}
			if(P_next == P_rm)
			{
				state = 2;
				//goto STATE_2;
			}
			else
			{
				hull_points.push_back(P_next);
				a_ref = atan2(P_next->y-P->y,P_next->x-P->x);
				P = P_next;
				a_plot_step = a_ref;
			}
			break;
		}
		case 2:
		{
			//STATE_2:
			finish = true;
			cout << "finish!" << endl;
		}
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
	gfx::add_button("convex hull 1.7", cb_convex_hull_1_7);
	gfx::add_button("reset",cb_init);
	gfx::add_button("step",cb_step);
	gfx::add_button("shuffle", cb_shuffle);

	gfx::main("convex hull slow", cb_draw, cb_mouse);

	return 0;
}
