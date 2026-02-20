#include <iostream>
#include <stack>
#include <string>
#include <list>

using namespace std;

typedef struct trail{
	char input;
	int trail_type;
	trail(char input_, int type)
	{
		input = input_;
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
}TC;

//Mazias-Hayes
TC* Edge_Trail(q_node*& leaf, TC trails);
TC* Node_CTC(char type, list<list<TC*>> child_trails);
TC* Trail_Trace(q_node*& root);