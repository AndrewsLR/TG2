#include "maz_hay.h"

int mult_sum [12] [12] = {
    {28, 4, 1, 30, 11, 10, 11, 11, 10, 11, 11, 0},
    {3, 29, 27, 2, 11, 9, 11, 11, 11, 9, 11, 0},
    {27, 2, 3, 29, 11, 9, 11, 11, 9, 11, 11, 0},
    {1, 30, 28, 4, 11, 10, 11, 11, 11, 10, 11, 0},
    {7, 8, 7, 8, 11, 11, 11, 11, 11, 11, 11, 0},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 8, 7, 11, 11, 11, 11, 11, 11, 11, 11, 0},
    {7, 11, 11, 8, 11, 11, 11, 11, 11, 11, 11, 0},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {0, 0, 0, 0, 11, 0, 11, 11, 0, 0, 11, 0},
};

int sum_mult [12] [12] = {
    {28, 4, 1, 24, 11, 10, 11, 11, 10, 28, 11, 0},
    {3, 29, 11, 2, 11, 9, 11, 11, 11, 9, 11, 0},
    {23, 2, 3, 13, 11, 9, 11, 11, 9, 23, 11, 0},
    {1, 11, 11, 4, 11, 10, 11, 11, 11, 10, 11, 0},
    {7, 8, 7, 8, 11, 11, 11, 11, 11, 11, 11, 0},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {28, 8, 7, 24, 11, 11, 11, 11, 11, 28, 11, 0},
    {7, 11, 11, 8, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
    {0, 0, 0, 0, 11, 0, 11, 11, 0, 0, 0, 0},
};
//entradas 0 na tabela sigifica que um dos covers possui multiplas tracks, implicando em tentar concatenar com cada uma de suas tracks

//Mazias-Hayes
list<TC*> Edge_Trail(q_node*& leaf)
{
    TC *trail1 = new TC(leaf->tipo, 1);
    TC *trail2 = new TC(leaf->tipo, 2);
    list<TC*> trails;
    trails.push_back(trail1);
    trails.push_back(trail2);
    return trails;
}
list<TC*> Node_CTC(q_node root, char op);
list<TC*> Trail_Trace(q_node*& root)
{
    cout<<"INICIANDO TRAIL TRACE"<<endl;
    list<TC*> trails;
    if(root->filhos.size() < 1)
    {
        cout<<"Nodo folha, fazendo Edge"<<endl;
        trails = Edge_Trail(root);
        cout << "RETORNOU " << trails.front()->input.front() << " E " << trails.back()->input.front() << endl;
    }
    else
    {
        for(q_node* filho : root->filhos)
        {
            list<TC*> temp = Trail_Trace(filho, root->tipo)
        }
        Node_CTC(root, trails)
    }
    return trails;
}