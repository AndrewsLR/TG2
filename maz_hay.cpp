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
CTC* Edge_Trail(q_node*& leaf)
{
    Trail trail1(leaf->tipo, 1);
    Trail trail2(leaf->tipo, 2);
    TC tc1;
    TC tc2;
    tc1.trails.push_back(trail1);
    tc2.trails.push_back(trail2);
    tc1.cover_type = 1;
    tc2.cover_type = 2;
    CTC* cover= new CTC(tc1);
    cover->add_cover(tc2);
    return cover;
}

CTC* Node_CTC(char type, list<CTC*> child_covers)
{
    CTC* this_ctc = new CTC();
    //se cover for tipo 0, criar cover nova apenas com concactenacao; significa que era uma lista de edges
    //caso contrario, cria combinacoes mantendo trails
    list<CTC*>::iterator it;
    list<CTC*>::iterator it2;
    //Cria nested loop para testar todas as combinacoes
    for(it = child_covers.begin(); it != child_covers.end(); it++)  // for each CTC
    {
        for(TC it_tc : (*it)->covers) //for each TC
        {
            for(it2 = next(it); it2 != child_covers.end(); it2++) //for each other CTC
            {
                //(*it)->print_covers();
                //(*it2)->print_covers();
                for(TC it2_tc : (*it2)->covers)                 //for each other TC
                {
                    if(it_tc.cover_type == 0)                  // if an edge TC || EACH CONCATENATION HAVE TO BE SELF CONTAINED, TRY CONCATENING EVEN AFTER A SUCCESSEFUL CONCATENATION, add a concatenate function?
                    {
                        TC temp_tc1(it_tc.trails.front());
                        TC temp_tc2(it_tc.trails.back());
                        CTC* temp_ctc = Concatenate(temp_tc1, it2_tc, type);
                        for(TC it_ctc: temp_ctc->covers)
                        {
                            int insert = this_ctc->add_cover(it_ctc);
                            if(insert)
                                cout<<"Adicionado"<<endl;
                            
                        }
                        CTC* temp2_ctc = Concatenate(temp_tc2, it2_tc, type);
                        for(TC it_ctc: temp2_ctc->covers)
                        {
                            int insert = this_ctc->add_cover(it_ctc);
                            if(insert)
                                cout<<"Adicionado"<<endl;
                        }
                    }
                     else //caso sem cover edge
                    {
                        CTC* temp_ctc = Concatenate(it_tc, it2_tc, type);
                        for(TC it_ctc: temp_ctc->covers)
                        {
                            int insert = this_ctc->add_cover(it_ctc);
                            if(insert)
                                cout<<"Adicionado"<<endl;
                        }
                    }
                }
            }
        }
    }
    return this_ctc;
}

CTC* Trail_Trace(q_node*& root)
{
    cout<<"INICIANDO TRAIL TRACE"<<endl;
    CTC* trails;
    if(root->filhos.size() < 1)
    {
        cout<<"Nodo folha, fazendo Edge"<<endl;
        trails = Edge_Trail(root);
        cout << "RETORNOU " << trails->covers.front().trails.front().input.front() << " E " << trails->covers.front().trails.back().input.front() << endl;
    }
    else
    {
        cout<<"NAO E FOLHA"<<endl;
        list<CTC*> child_covers;
        for(q_node* filho : root->filhos)
        {
            child_covers.push_back(Trail_Trace(filho));
        }
        trails = Node_CTC(root->tipo, child_covers);
    }
    return trails;
}

CTC* Concatenate(TC cover1, TC cover2, int type)
{
    //implementacao provavelmente errada, apos achar uma concatenacao, segue com ela ate o final
    CTC* concatenate = new CTC();
    for(Trail it1_trail : cover1.trails)
    {
        for(Trail it2_trail : cover2.trails)
        {
            cout<<"COMBINANDO ";
            for(char it : it1_trail.input)
                cout<<it<<",";
            cout<<" TIPO "<<it1_trail.trail_type<<" COM ";
            for(char it : it2_trail.input)
                cout<<it<<",";
            cout<<" TIPO "<<it2_trail.trail_type<<endl;
            int cover_type;
            if(type == '*')
                cover_type = mult_sum[it1_trail.trail_type - 1][it2_trail.trail_type - 1];
            else
                cover_type = sum_mult[it1_trail.trail_type - 1][it2_trail.trail_type - 1];
            TC tmp;
            if(cover_type > 11)             //if cover has multiple trails
            {
                tmp.cover_type = cover_type;
                tmp.trails.push_back(it1_trail);
                tmp.trails.push_back(it2_trail);
                concatenate->add_cover(tmp);
            }
            else                                                                    //if cover has one trail
            {
                Trail temp;
                temp.trail_type = cover_type;
                for(char input : it1_trail.input)
                    temp.input.push_back(input);
                 for(char input : it2_trail.input)
                    temp.input.push_back(input);
                temp.input.push_back(it2_trail.input.front());
                tmp.trails.push_back(temp);
                tmp.cover_type = cover_type;
                it1_trail = temp;
                concatenate->add_cover(tmp);
            }                

        }
    }
    concatenate->print_covers();
    return concatenate;
}