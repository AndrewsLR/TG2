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
TC *Edge_Trail(q_node*& leaf)
{
    Trail trail1(leaf->tipo, 1);
    Trail trail2(leaf->tipo, 2);
    TC* trails = new TC();
    trails->trails.push_back(trail1);
    trails->trails.push_back(trail2);
    trails->cover_type = 0;
    return trails;
}

TC* Node_CTC(char type, list<TC*> child_trails)
{
    TC* CTC = new TC();
    //se cover for tipo 0, criar cover nova apenas com concactenacao; significa que era uma lista de edges
    //caso contrario, cria combinacoes mantendo trails
    list<TC*>::iterator it;
    list<TC*>::iterator it2;
    //Cria nested loop para testar todas as combinacoes
    for(it = child_trails.begin(); it != child_trails.end(); it++)
    {
        for(it2 = it++; it2 != child_trails.end(); it2++)
        {
            //se for cover de edges, cria novo trail e procura se ja existe um do mesmo tipo e mesmo tamanho
            if((*it)->cover_type == 0)
            {
                if(type == '*')
                {
                    Trail temp((*it)->trails.front().input, mult_sum[(*it)->trails.front().trail_type][(*it2)->trails.front().trail_type]);
                    int found = 0;
                    for(TC* child : child_trails)
                    {
                        if (child->cover_type == temp.trail_type)
                        {
                            if(child->trails.size() == 1)
                                found = 1;
                        }
                    }
                    //se nao existir adiciona como nova trail
                    if(found == 0)
                    {
                        TC* tmp = new TC(temp);
                        child_trails.push_back(tmp);
                    }
                    //concatenate
                    //create new cover with concatenation
                    //if child_trails does not have cover type or is bigger, add, remove bigger
                }
            }
            else
            {
                //concatenate all combination of trials
                //add the new/smaller ones
                for(Trials* trail1 : (*it)->trails)
                {
                    for(Trials* trail2 : (*it2)->trails)
                    {
                        //if needs to add:
                        //if new type, modify trial1
                        //if cant concatenate, add trail
                        //update cover_type
                    }
                }
            }
        }
    }
    return CTC;t, mo
}

TC* Trail_Trace(q_node*& root)
{
    cout<<"INICIANDO TRAIL TRACE"<<endl;
    TC *trails = new TC();
    if(root->filhos.size() < 1)
    {
        cout<<"Nodo folha, fazendo Edge"<<endl;
        trails = Edge_Trail(root);
        cout << "RETORNOU " << trails->trails.front().trail_type << " E " << trails->trails.back().trail_type << endl;
    }
    else
    {
        list<TC*> child_trails;
        for(q_node* filho : root->filhos)
        {
            child_trails.push_back(Trail_Trace(filho));
        }
        trails = Node_CTC(root->tipo, child_trails);
    }
    return trails;
}