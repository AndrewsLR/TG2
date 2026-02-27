#include <iostream>
#include <stack>
#include <string>
#include <list>

using namespace std;

typedef struct trail{
	list<char> input;
	int trail_type;
	trail() = default;
	trail(char input_, int type)
	{
		input.push_back(input_);
		trail_type = type;
	}
}Trail;

typedef struct trail_cover{
	list<Trail> trails;
	int cover_type;
	trail_cover() = default;
	trail_cover(list<Trail> trails_, int type)
	{
		trails = trails_;
		cover_type = type;
	}
	trail_cover(Trail trails_)
	{
		trails.push_back(trails_);
		cover_type = trails_.trail_type;
	}
	void print_trails()
	{
		for(Trail it: trails)
		{
			cout<<"Trail ";
			for(char inputs: it.input)
			{
				cout<<inputs<<", ";
			}
			cout<<"Tipo "<<it.trail_type<<endl;
		}
	}
}TC;

typedef struct complete_trail_cover{
	list<TC> covers;
	complete_trail_cover() = default;
	complete_trail_cover(list<TC> covers_)
	{
		covers = covers_;
	}
	complete_trail_cover(TC covers_)
	{
		covers.push_back(covers_);
	}
	int add_cover(TC cover)
	{
		list<TC>::iterator it_cover;
		for(it_cover = covers.begin(); it_cover != covers.end(); it_cover++)
		{
			if(it_cover->cover_type == cover.cover_type)			//if a cover of same type exists and is smaller, dont add
			{
				if(it_cover->trails.size() <= cover.trails.size())
					return 0;
				else
					if(it_cover->trails.size() > cover.trails.size())
					{
						covers.erase(it_cover);
					}
			}
		}
		covers.push_back(cover);
		return 1;
	}
	void print_covers()
	{
		for(TC it: covers)
		{
			cout<<"TrailCover tipo "<<it.cover_type<<" com Trails:"<<endl;
			it.print_trails();
		}
	}
}CTC;

//Mazias-Hayes
CTC* Edge_Trail(q_node*& leaf);
CTC* Node_CTC(char type, list<CTC*> child_covers);
CTC* Trail_Trace(q_node*& root);
CTC* Concatenate(TC cover1, TC cover2, int type);