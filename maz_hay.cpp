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
    cout<<"ENTROU EM NODE CTCT"<<endl;
    CTC* this_ctc = new CTC();
    //se cover for tipo 0, criar cover nova apenas com concactenacao; significa que era uma lista de edges
    //caso contrario, cria combinacoes mantendo trails
    list<CTC*>::iterator it;
    list<CTC*>::iterator it2;
    //Cria nested loop para testar todas as combinacoes
    for(it = child_covers.begin(); it != child_covers.end(); it++)  // for each CTC
    {
            CTC temp;
            for(it2 = next(it); it2 != child_covers.end(); it2++) //for each other CTC
            {
                for(TC it_tc : (*it)->covers) //for each TC
                {   
                //(*it)->print_covers();
                //(*it2)->print_covers();
                    for(TC it2_tc : (*it2)->covers)                 //for each other TC
                    {
                        //cout<<"------------------------------------------TC INDO PARA CONCATENATE--------------------------------------------------------------------"<<endl;
                       // it_tc.print_trails();
                        CTC* temp_ctc = Concatenate(it_tc, it2_tc, type);
                        //Aqui seria necessario concatenar esses novos TCs tambem no loop
                        //E SE A FUNCAO CONCATENATE LIDAR COM CTCS, ELA MESMA PODERIA GARANTIR ITERACAO SOBRE NOVOS TCS GERADOS
                        //Criar ctcs descartaveis e iterar sobre eles?
                        for(TC it_ctc: temp_ctc->covers)
                        {
                            int insert = temp.add_cover(it_ctc);
                            //if(insert)
                            //cout<<"Adicionado"<<endl;
                        }
                        //cout<<"------------------------------------------TEMP APOS CONCATENATE--------------------------------------------------------------------"<<endl;
                        //temp.print_covers();
                    }
                }
            }
            for(TC it_tc : temp.covers)
            {
                cout<<"ENTROU NO TEMP"<<endl;
                for(CTC* it_ctc : child_covers)                 //for each other TC
                {
                    for(TC it2_tc: it_ctc->covers)
                    {
                        cout<<"MANDANDO PARA CONCATENATE"<<endl;
                        it_tc.print_trails();
                        cout<<"E TAMEBEM"<<endl;
                        it2_tc.print_trails();
                        CTC* temp_ctc = Concatenate(it_tc, it2_tc, type);
                        //for(TC it3_tc: temp_ctc->covers)
                        //{
                        //int insert = this_ctc->add_cover(it3_tc);
                        //if(insert)
                        //cout<<"Adicionado"<<endl;
                       // }
                       cout<<"------------------------------------------Temp apos cada concatenate--------------------------------------------------------------------"<<endl;
                        if(temp_ctc != nullptr)
                        {    
                            temp_ctc->print_covers();
                            for(TC it_tc : temp_ctc->covers)
                            {
                                this_ctc->add_cover(it_tc);
                            }
                        }
                        //this_ctc->print_covers();
                    }
                   
                }
            }
            //cout<<"------------------------------------------this_ctc APOS CONCATENATE--------------------------------------------------------------------"<<endl;
            //this_ctc->print_covers();
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
    list<TC> tc_list;
    //retorna nullptr se tiverem mesmo input
    for(Trail it1_trail: cover1.trails)
    {
        for(Trail it2_trail : cover2.trails)
        {
            for(char input1 : it1_trail.input)
            {
                for(char input2 : it2_trail.input)
                {
                    if(input1 == input2)
                        return nullptr;
                }
            }
        }
    }
    
    for(Trail it1_trail : cover1.trails)
    {
        //1 TC para cada it1_ trail
        //se não concatenar, adiciona it2_trail ao TC e segue tentando concatenar
        // se concatenar, adiciona ao TC a concatenacao e todas as trails de it2_trail que sobraram
        //se ao terminar interacao nao concatenou, adciona it1_trail ao TC
        //Cria novo TC
        //TC tc_it1;
        //tc_list.push_front(tc_it1);
        bool concatenated = false;
        for(Trail it2_trail : cover2.trails)
        {
            if(concatenated == false)
            {
                int cover_type;
                if(type == '*')
                    cover_type = mult_sum[it1_trail.trail_type - 1][it2_trail.trail_type - 1];
                else
                    cover_type = sum_mult[it1_trail.trail_type - 1][it2_trail.trail_type - 1];

                if(cover_type > 11)
                {
                    Trail *temp = new Trail();
                    temp->trail_type = it2_trail.trail_type;
                    for(char input : it2_trail.input)
                        temp->input.push_back(input);
                    TC* tc_temp = new TC(*temp);
                    tc_list.push_front(*tc_temp);
                }
                else
                {
                    cout<<"VAI CONCATENAR"<<endl;
                    Trail *temp = new Trail();
                    temp->trail_type = cover_type;
                    for(char input : it1_trail.input)
                        temp->input.push_back(input);
                    for(char input : it2_trail.input)
                        temp->input.push_back(input);
                    TC* tc_temp = new TC(*temp);
                    tc_list.push_front(*tc_temp);
                    concatenated = true;
                    //adiciona todas as trails com inputs diferentes
                    for(Trail it1_copy : cover1.trails)
                    {
                        for(TC it_tc : tc_list)
                        {
                            for(Trail it_trail : it_tc.trails)
                            {
                                if(it1_copy.same_input(it_trail) == 0)
                                {
                                   tc_list.front().trails.push_front(it1_copy); 
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                //tc_list.front().trails.push_back(it2_trail);
            }         

        }
        if(concatenated == false && !tc_list.empty())
        {
            tc_list.front().trails.push_front(it1_trail);
        }
    }
    //tenta concatenar concatenacoes
    CTC teste;
    //ctc termina vazio, por que?
    cout<<"------------------------------------------Trails do cover1--------------------------------------------------------------------"<<endl;
    cover1.print_trails();
    cout<<"------------------------------------------Trails do cover2--------------------------------------------------------------------"<<endl;
    cover2.print_trails();
    cout<<"------------------------------------------TCs da lista--------------------------------------------------------------------"<<endl;
    for(TC it_tc : tc_list)
        it_tc.print_trails();
                   
    //adiciona TCs ao CTC
    for(TC it_tc : tc_list)
    {
        cout<<"ANTES DE CRASHAR"<<endl;
        if(it_tc.trails.empty() == 0)
            cout<<"TESTE VAZIO"<<endl;
        concatenate->add_cover(it_tc);
    }
    //concatenate->print_covers();
    //cout<<"//////////////////////////////////////////////////////////////////////////////////////////"<<endl;
    return concatenate;
}