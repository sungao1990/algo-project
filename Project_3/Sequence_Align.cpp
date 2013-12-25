#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
using namespace std;

#define GAP_START (-2)
#define GAP_EXTEND (-8)

class table_element;


map<char,map<char,int> > BLOSUM50;



class table_element
{
public:
	table_element():
		value(0), row(0), column(0), previous(nullptr)
	{}
	
	table_element(int r, int c):
		value(0), row(r), column(c), previous(nullptr)
	{}
	
	bool previous_is_up()
	{
		if (previous->column==column)
			return true;
		else
			return false;
	}
	
	bool previous_is_left()
	{
		if (previous->row==row)
			return true;
		else
			return false;
	}
		
public:
	int value;
	int row;
	int column;
	table_element* previous;
};



void Print_Table(vector<vector<table_element> > & table)
{
	int width  = table.size();
	int height = table.front().size();
	for (int i=0; i<width; i++)
	{
		for (int j=0; j<height; j++)
		{
			cout << table[i][j].value << "\t";
		}
		cout << endl;
	}
}


int max(int x, int y, int z)
{
	int max = x;
	if (max < y)
		max = y;
	if (max < z)
		max = z;
	return max;
}


list<table_element*> Generate_Path(vector<vector<table_element> > & table)
{
	list<table_element*> path;
	path.push_front(&(table[table.size()-1][table.front().size()-1]));
	
	while(1)
	{
		table_element* next = path.front()->previous;
		path.push_front(next);
		if (next->row==0 && next->column==0)
			break;
	}
	
	return path;
}

void Print_Path(list<table_element*> & path)
{
	for (list<table_element*>::iterator it=path.begin(); it!=path.end(); it++)
	{
		cout << "(" << (*it)->row << "," << (*it)->column << ")";
		if (it!=--path.end())
		{
			cout << " -> ";
		}
	}
	cout << endl;
}


string align_first(string s, list<table_element*> & path)
{
	string s_aligned = "";
	int previous_c, current_c, cnt(0);
	for (list<table_element*>::iterator it=path.begin(); it!=path.end(); it++)
	{
		if (it==path.begin())
		{
			previous_c = (*it)->column;
		}
		else
		{
			current_c = (*it)->column;
			if (current_c==previous_c)
			{
				s_aligned += "_";
			}
			else
			{
				s_aligned += s.at(cnt);
				cnt++;
			}
			previous_c = current_c;
		}
	}
	return s_aligned;
}

string align_second(string s, list<table_element*> & path)
{
	string s_aligned = "";
	int previous_r, current_r, cnt(0);
	for (list<table_element*>::iterator it=path.begin(); it!=path.end(); it++)
	{
		if (it==path.begin())
		{
			previous_r = (*it)->row;
		}
		else
		{
			current_r = (*it)->row;
			if (current_r==previous_r)
			{
				s_aligned += "_";
			}
			else
			{
				s_aligned += s.at(cnt);
				cnt++;
			}
			previous_r = current_r;
		}
	}
	return s_aligned;
}

int main(int argc, char** argv)
{
	string s1 = argv[1];
	string s2 = argv[2];
	cout << "First  String: " << s1 << endl;
	cout << "Second String: " << s2 << endl << endl;
	
	// read file, create matrix
	string line;
	ifstream myfile ("blosum50.txt");
	if (!myfile.is_open())
		cout << "Unable to open file" << endl;
	else
	{
		getline (myfile,line);    // first line, get all kinds of letters
		map<int, char> letter;
		set<char> letter_set;
		int cnt(0);
		for(string::iterator it_first_line=line.begin(); it_first_line!=line.end(); it_first_line++)
		{
			if(*it_first_line != '\t')
			{
				cnt++;
				letter[cnt] = *it_first_line;
				letter_set.insert(*it_first_line);
			}
		}
		
		// check if the input string is valid
		for (string::iterator it=s1.begin(); it!=s1.end(); it++)
		{
			if(letter_set.find(*it)==letter_set.end())  //not found
			{
				cout << "First string invalid... Letter '"<< *it << "' not found..." << endl;
				return 0;
			}
		}
		for (string::iterator it=s2.begin(); it!=s2.end(); it++)
		{
			if(letter_set.find(*it)==letter_set.end())  //not found
			{
				cout << "Second string invalid... Letter '"<< *it << "' not found..." << endl;
				return 0;
			}
		}
		
		
		while ( getline (myfile,line) )
		{
			char row_char;
			cnt = 0;
			for(string::iterator it_line=line.begin(); it_line!=line.end(); it_line++)
			{
				if(it_line == line.begin())
				{
					row_char = *it_line;
				}
				else
				{
					if(*it_line != '\t')
					{
						cnt++;
						int value;
						if (*it_line == '-')
						{
							it_line++;
							value = -1*(*it_line-48);
						}
						else
						{
							value = *it_line-48;
						}
						char column_char = letter[cnt];
						BLOSUM50[row_char][column_char] = value;
					}
				}
			}
		}
		
		
		// build the table
		vector<vector<table_element> > table;
		for (int i=0; i<s2.length()+1; i++)
		{
			vector<table_element> subtable;
			for (int j=0; j<s1.length()+1; j++)
			{
				subtable.push_back(table_element(i,j));
			}
			table.push_back(subtable);
		}
		
		
		table[0][0].value = 0;
		for (int j=1; j<s1.length()+1; j++)
		{
			table[0][j].value = GAP_START + GAP_EXTEND*(j-1);
			table[0][j].previous = &(table[0][j-1]);
		}
		for (int i=1; i<s2.length()+1; i++)
		{
			table[i][0].value = GAP_START + GAP_EXTEND*(i-1);
			table[i][0].previous = &(table[i-1][0]);
		}
		
		
		for (int i=1; i<s2.length()+1; i++)
		{
			for (int j=1; j<s1.length()+1; j++)
			{
				//cout << i-1 << "\t" << j-1 << endl;
				
				int gain1 = table[i-1][j-1].value + BLOSUM50[s2.at(i-1)][s1.at(j-1)];
				
				int gain2;
				if (table[i-1][j].previous_is_up())
				{
					gain2 = table[i-1][j].value + GAP_EXTEND;
				}
				else
				{
					gain2 = table[i-1][j].value + GAP_START;
				}
				
				int gain3;
				if (table[i][j-1].previous_is_left())
				{
					gain3 = table[i][j-1].value + GAP_EXTEND;
				}
				else
				{
					gain3 = table[i][j-1].value + GAP_START;
				}
				
				table[i][j].value = max(gain1, gain2, gain3);
				if (table[i][j].value == gain1)
				{
					table[i][j].previous = &(table[i-1][j-1]);
				}
				else if (table[i][j].value == gain2)
				{
					table[i][j].previous = &(table[i-1][j]);
				}
				else
				{
					table[i][j].previous = &(table[i][j-1]);
				}
			}
		}
		
		Print_Table(table);
		cout << endl;
		
		list<table_element*> path = Generate_Path(table);
		Print_Path(path);
		cout << endl;
		
		string s1_aligned = align_first(s1,path);
		string s2_aligned = align_second(s2,path);
		
		cout << "Align Result:" << endl;
		cout << s1_aligned << endl;
		cout << s2_aligned << endl << endl;
	}
	
	return 0;
}
