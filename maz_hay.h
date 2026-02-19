#include <iostream>
#include <stack>
#include <string>
#include <list>

using namespace std;

typedef struct trail_cover{
	list<char> input;
	list<int> trail_type;
	int cover_type;
	trail_cover(char input_, int type)
	{
		input.push_back(input_);
		trail_type.push_back(type);
        cover_type = type;
	}
}TC;

//Mazias-Hayes
list<TC*> Edge_Trail(q_node*& leaf, TC trails);
list<TC*> Node_CTC(q_node*& root, TC trails);
list<TC*> Trail_Trace(q_node*& root);