#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <set>
#include <map>
#include <list>
#include <iterator>
#include <time.h>
#include <math.h>
#include <stdio.h>

using namespace std;

#define FirstPart true
#define SecondPart false

int qty_edge = 0;

class vertex;
class edge;


class edge : public map<vertex*, string>
{
public:

	void insert(vertex* c, string b)
	{
		edge::iterator it_edge = find(c);
		if(it_edge == end())    // not found, new neighbor
		{
			(*this)[c] = b;
			qty_edge++;
		}
		else                    // found, merge the book string
		{
			string book = it_edge->second;
			book = book + "  /  " + b;
			it_edge->second = book;
		}
	}
};




class vertex
{
public:
	vertex():
		name(""), path(nullptr), edges(nullptr)
	{}

	vertex(string s):
		name(s), path(nullptr), edges(nullptr)
	{}
	vertex(string s, vertex* p, edge* e):
		name(s), path(p), edges(e)
	{}

	list<vertex*> expand(map<string,vertex*> & graph, set<vertex*> & visited_vertex)
	{
		list<vertex*> new_Vertex;
		edge::iterator it = edges->begin();
		for( ; it!=edges->end(); it++)
		{
			if(visited_vertex.find(it->first) == visited_vertex.end())  // not found, take it, otherwise ignore it
			{
				new_Vertex.push_back(it->first);
				it->first->path = this;                                 // record the path
			}
		}
		return new_Vertex;
	}


public:
	string name;
	vertex* path;
	edge* edges;
};



void tokenlines(string& line, bool first_part)
{
	string::iterator it;
	for(it=line.begin(); it!=line.end(); ++it)
  {
	  if (*it == '\t')  break;
  }
	if(first_part)
	{
		line.erase(it,line.end());
	}
	else
	{
		line.erase(line.begin(),it+1);
	}


	line.erase(line.begin());
	line.erase(line.end()-1);

}




int main ()
{
	set<string> characters;
	map<string, list<string> > books;
	map<string, list<string> > ch_bo;
	clock_t start, finish;
	double duration;


	start = clock();
	cout << "Reading File..." << endl;
	string line;
	ifstream myfile ("labeled_edges.tsv");
	if (!myfile.is_open())
		cout << "Unable to open file";
	else
	{
		while ( getline (myfile,line) )
		{
			string character = line;
			string book = line;

			tokenlines(character,FirstPart);
			tokenlines(book,SecondPart);

			characters.insert(character);

			if(books.find(book) == books.end())   // not found
			{
				list<string> l;
				books.insert(pair<string,list<string> >(book,l));
			}
			books[book].push_back(character);

			if(ch_bo.find(character) == ch_bo.end())
			{
				list<string> l;
				ch_bo.insert(pair<string,list<string> >(character,l));
			}
			ch_bo[character].push_back(book);

		}
		myfile.close();
		printf("Finish reading!\t\t%d books found\t%d characters found.\n", (int)books.size(), (int)characters.size());
		finish = clock();
		duration = (double)(finish - start) / CLOCKS_PER_SEC;
		printf( "%f seconds cost\n\n", duration );





		//Build graph
		start = clock();
		cout << "Building Graph..." << endl;
		map<string,vertex*> graph;
		set<string>::iterator it_ch = characters.begin();
		for( ; it_ch!=characters.end(); it_ch++)
		{
			string graph_key = *it_ch;
			edge* connected_edge;
			map<string,vertex*>::iterator it_graph = graph.find(graph_key);
			if(it_graph == graph.end())  //not found, create a new one
			{
				vertex* v = new vertex(graph_key,nullptr,nullptr);
				edge* e = new edge();
				graph[graph_key] = v;    //add
				v->edges = e;
				connected_edge = e;
			}
			else                                 //found, then create the memory space for edges
			{
				edge* e = new edge();
				it_graph->second->edges = e;
				connected_edge = e;
			}

			list<string> connected_books = ch_bo[graph_key];
			list<string>::iterator it_ch_bo = connected_books.begin();
			for( ; it_ch_bo!=connected_books.end(); it_ch_bo++)
			{
				list<string> neighbours = books[*it_ch_bo];
				list<string>::iterator it_nei = neighbours.begin();
				for( ; it_nei!=neighbours.end(); it_nei++)
				{
					if(*it_nei != graph_key)
					{
						it_graph = graph.find(*it_nei);
						if(it_graph == graph.end())  //Neighbor vertex not found, create a new one
						{
							vertex* v = new vertex(*it_nei,nullptr,nullptr);
							graph[*it_nei] = v;      // also create a new element in graph
							connected_edge->insert(v,*it_ch_bo);
						}
						else                                   //found, just assign
						{
							connected_edge->insert(it_graph->second, *it_ch_bo);
						}
					}
				}
			}
		}
		cout << "Graph finished!\t\t" << graph.size() << " vertices created\t" << endl;
		finish = clock();
		duration = (double)(finish - start) / CLOCKS_PER_SEC;
		printf( "%f seconds cost\n\n\n", duration );




		//search
		while(1)
		{
			// get the input from user
			string start_ch;
			string target_ch;
			cout << "Type two characters to start searching... First one please..." << endl;

			while(1)
			{
				char str[100];
				cin.getline(str,100);
				start_ch = string(str);
				if(find(characters.begin(), characters.end(), start_ch) == characters.end())
				{
					cout << "Oops! Starting character doesn't exist ... Check and retype please..." << endl;
				}
				else
				{
					cout << "Got it! Now the target one please..." << endl;
					break;
				}
			}

			while(1)
			{
				char str[100];
				cin.getline(str,100);
				target_ch = string(str);
				if(find(characters.begin(), characters.end(), target_ch) == characters.end())
				{
					cout << "Oops! Target character doesn't exist... Check and retype please..." << endl;
				}
				else
				{
					cout << "Got it!" << endl;
					break;
				}
			}


			start = clock();
			cout << "Searching..." << endl;
			set<vertex*> visited_vertex;
			map<string,vertex*>::iterator it_g = graph.find(start_ch);
			vertex* start_v = it_g->second;
			it_g->second->path = nullptr;
			it_g = graph.find(target_ch);
			vertex* end_v;

			list<vertex*> Queue;
			Queue.push_back(start_v);
			visited_vertex.insert(start_v);
			bool finished = false;

			int vertex_cnt = 0;
			while(1)
			{
				vertex_cnt++;
				if(Queue.empty()) break;
				vertex* Head = Queue.front();
				Queue.erase(Queue.begin());
				list<vertex*> newNodes = Head->expand(graph, visited_vertex);
				if(!newNodes.empty())
				{
					for(list<vertex*>::iterator it_exn=newNodes.begin(); it_exn!=newNodes.end(); it_exn++)
					{
						if((*it_exn)->name == target_ch)    // reached!
						{
							finished = true;
							end_v = *it_exn;
							break;
						}
						else
						{
							Queue.push_back(*it_exn);
							visited_vertex.insert(*it_exn);
						}
					}
				}
				if(finished) break;
			}

			// Print Result
			if(finished)
			{
				cout << "Finished!... Result:" << endl;
				list<string> hist;
				vertex* v_tmp = end_v;
				while(1)
				{
					if(v_tmp->path == nullptr) break;
					else
					{
						string s1 = v_tmp->path->name;
						string bb = (*(v_tmp->edges))[v_tmp->path];
						string s = "(" + s1 + ")" + " -> " + bb + " -> ";
						hist.push_front(s);

						v_tmp = v_tmp->path;
					}
				}
				list<string>::iterator it_hist = hist.begin();
				for( ; it_hist!=hist.end(); it_hist++)
				{
					cout << *it_hist << endl;
				}
				cout << "(" << end_v->name << ")" << endl;
				cout << "length: " << hist.size() << ",    " << vertex_cnt << " vertices were searched.\t\t";
				finish = clock();
				duration = (double)(finish - start) / CLOCKS_PER_SEC;
				printf( "%f seconds cost\n\n\n", duration );
			}
			else
			{
				cout << "Failed!" << "    " << vertex_cnt << " vertices were searched.\t\t";
				finish = clock();
				duration = (double)(finish - start) / CLOCKS_PER_SEC;
				printf( "%f seconds cost\n\n\n", duration );
			}
		}
	}
	return 0;
}
