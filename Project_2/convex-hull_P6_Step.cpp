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
vector<Point*> hull_points;
list<Point*> L_upper;
list<Point*> L_lower;
Point* P_Watching_1;
Point* P_Watching_2;
Point* P_Watching_3;
bool highlight(false);
bool plot_hull(false);
bool step_finish(false);
bool plot_step(false);
bool reset(false);
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
		list<Point*>::iterator it_last = L_upper.end();
		it_last--;
		for(list<Point*>::iterator it=L_upper.begin(); it!=L_upper.end(); it++)
		{
			gfx::set_pen (1.0, 0.0, 0.0, 1.0, 1.0); // width, red, green, blue, alpha
			gfx::draw_arc ((*it)->x, (*it)->y, 0.5, 0, 2*pi);
			if(it != it_last)
			{
				list<Point*>::iterator it_next = it;
				it_next++;
				gfx::set_pen (1.0, 0.0, 0.0, 1.0, 1.0);
				gfx::draw_line((*it)->x, (*it)->y, (*it_next)->x, (*it_next)->y);
			}
		}
		
		it_last = L_lower.end();
		it_last--;
		for(list<Point*>::iterator it=L_lower.begin(); it!=L_lower.end(); it++)
		{
			gfx::set_pen (1.0, 0.0, 0.0, 1.0, 1.0); // width, red, green, blue, alpha
			gfx::draw_arc ((*it)->x, (*it)->y, 0.5, 0, 2*pi);
			if(it != it_last)
			{
				list<Point*>::iterator it_next = it;
				it_next++;
				gfx::set_pen (1.0, 0.0, 0.0, 1.0, 1.0);
				gfx::draw_line((*it)->x, (*it)->y, (*it_next)->x, (*it_next)->y);
			}
		}
		
		
		if(highlight)
		{
			highlight = false;
			gfx::set_pen (2.0, 1.0, 1.0, 0.0, 1.0);
			gfx::draw_arc (P_Watching_1->x, P_Watching_1->y, 0.5, 0, 2*pi);
			gfx::draw_arc (P_Watching_2->x, P_Watching_2->y, 0.5, 0, 2*pi);
			gfx::draw_arc (P_Watching_3->x, P_Watching_3->y, 0.5, 0, 2*pi);
			if(Right_Turn(P_Watching_1,P_Watching_2,P_Watching_3))
			{
				gfx::set_pen (2.0, 0.0, 1.0, 0.0, 1.0);
			}
			else
			{
				gfx::set_pen (2.0, 1.0, 0.0, 0.0, 1.0);
			}
			gfx::draw_line (P_Watching_1->x, P_Watching_1->y, P_Watching_2->x, P_Watching_2->y);
			gfx::draw_line (P_Watching_2->x, P_Watching_2->y, P_Watching_3->x, P_Watching_3->y);
		}
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





static void cb_convex_hull_P6()
{
	L_upper.clear();
	L_lower.clear();
	hull_points.clear();
	
	double t_start, t_finish;
    
    t_start = get_time_usec();
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
	
	t_finish = get_time_usec();
	
	Print_Hull();
	
	cout << endl;
	
	Show_running_time(t_start, t_finish);
	
	plot_hull =true;
}





static void cb_init()
{
	hull_points.clear();
	L_upper.clear();
	L_lower.clear();
	reset = true;
	cout << "initialization done!" << endl;
}




static void cb_step()
{
	static char state = 0;  // 0:init    1:add new p to L_u/L_l    2:check or delete points according to the right-turn rule    3:finish
	static Point* P1;
	static Point* P2;
	static Point* P3;
	static list<Point*>::iterator it_l;
	static map<double,Point*>::iterator it_map_p;
	static map<double,Point*> Points_SortedInX;
	
	plot_step = true;
	
	if(reset)
	{
		reset = false;
		state = 0;
	}
	
	switch (state)
	{
		case 0:
		{
			Points_SortedInX.clear();
			step_finish = false;
	
			for (vector<Point>::iterator it=Points.begin(); it!=Points.end(); it++)
			{
				Points_SortedInX[it->x] = &(*it);
			}
			it_map_p = Points_SortedInX.begin();
			L_upper.push_back(it_map_p->second); it_map_p++;
			L_upper.push_back(it_map_p->second); it_map_p++;
			state = 1;
			break;
		}
		case 1:
		{
			STATE_1:
			L_upper.push_back(it_map_p->second);
			it_l = L_upper.end(); it_l--;
			P3 = *it_l; it_l--;
			P2 = *it_l; it_l--;
			P1 = *it_l;
			highlight = true;
			P_Watching_1 = P1;
			P_Watching_2 = P2;
			P_Watching_3 = P3;
			state = 2;
			break;
		}
		case 2:
		{
			if(L_upper.size()>2 && !(Right_Turn(P1,P2,P3)))
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
					highlight = true;
					P_Watching_1 = P1;
					P_Watching_2 = P2;
					P_Watching_3 = P3;
				}
			}
			else
			{
				it_map_p++;
				if(it_map_p == Points_SortedInX.end())
				{
					state = 3;
					cout << "Upper list done!" << endl;
				}
				else
				{
					state = 1;
					goto STATE_1;
				}
			}
			break;
		}
		case 3:
		{
			it_map_p = Points_SortedInX.end(); it_map_p--;
			L_lower.push_back(it_map_p->second); it_map_p--;
			L_lower.push_back(it_map_p->second); it_map_p--;
			state = 4;
			break;
		}
		case 4:
		{
			STATE_4:
			L_lower.push_back(it_map_p->second);
			it_l = L_lower.end(); it_l--;
			P3 = *it_l; it_l--;
			P2 = *it_l; it_l--;
			P1 = *it_l;
			highlight = true;
			P_Watching_1 = P1;
			P_Watching_2 = P2;
			P_Watching_3 = P3;
			state = 5;
			break;
		}
		case 5:
		{
			if(L_lower.size()>2 && !(Right_Turn(P1,P2,P3)))
			{
				it_l = L_lower.end();
				it_l--; it_l--;
				L_lower.erase(it_l);
		
				if(L_lower.size()>2)
				{
					it_l = L_lower.end(); it_l--;
					P3 = *it_l; it_l--;
					P2 = *it_l; it_l--;
					P1 = *it_l;
					highlight = true;
					P_Watching_1 = P1;
					P_Watching_2 = P2;
					P_Watching_3 = P3;
				}
			}
			else
			{
				it_map_p--;
				if(it_map_p == --Points_SortedInX.begin())
				{
					state = 6;
				}
				else
				{
					state = 4;
					goto STATE_4;
				}
			}
			break;
		}
		case 6:
		{
			cout << "Lower list done" << endl;
			state = 7;
			break;
		}
		case 7:
		{
			cout << "All finished!" << endl;
			break;
		}
	}
}





static void cb_shuffle()
{
	hull_points.clear();
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
	gfx::add_button("convex hull P6", cb_convex_hull_P6);
	gfx::add_button("reset",cb_init);
	gfx::add_button("step",cb_step);
	gfx::add_button("shuffle", cb_shuffle);

	gfx::main("convex hull slow", cb_draw, cb_mouse);

	return 0;
}
