#include <queue>

#include <fstream>

#include "arv_binaria.h"
using namespace std;

int precedencia(char op);
void faz_postfix(stack<char> &postfix, string eq);												//faz postfix
void monta_arv(node *ptr, stack<char> &postfix);												//monta a arvore
void printLevelOrder(node *root);																//printa a arvore
void pinta_arv(node *root);


typedef struct transistor{
	char tipo;
	int	num;
	int pos = 0;
	string drain;
	char gate;
	string source;
	transistor(char tipo_, int num_, string drain_, char gate_, string source_)
	{
		tipo = tipo_;
		num = num_;
		drain = drain_;
		gate = gate_;
		source = source_;
	}
	transistor(char tipo_, int num_, string drain_, char gate_, string source_, int pos_)
	{
		tipo = tipo_;
		num = num_;
		drain = drain_;
		gate = gate_;
		source = source_;
		pos = pos_;
	}
}transistor;

typedef struct net{
	string nome;
	int inicio;
	int fim;
	int linha;
	net(string nome_, int inicio_, int fim_, int linha_)
	{
		nome = nome_;
		inicio = inicio_;
		fim = fim_;
		linha = linha_;
	}
}net;
	
int net_number = 1; 																			//guarda o menor numero disponivel para criar nova net
int trans_number = 1;																			//guarda o menor numero disponivel para criar transistor

//funções de arvore
void quebra_portas(string eq);																	//chama faz_postfix e monta_arvore
//binaria
int precedencia(char op);
void faz_postfix(stack<char> &postfix, string eq);												//faz postfix
void monta_arv(node *ptr, stack<char> &postfix);												//monta a arvore
void printLevelOrder(node *root);																//printa a arvore
void pinta_arv(node *root);																		//pinta a arvore (faz algoritmo de Uehara e Cleemput
void retorna_ordem(node *root, queue<char> &ordem);		
//n-aria
void converte(node* root, q_node *&new_root);
void percorreEImprime(q_node* &root, const std::string& prefix = "", bool isLast = true);
void ordena(q_node *root);								

//netlist
//n-aria
void faz_netlist_ordenado(list<transistor*> &trans_list, q_node*& root, stack<int> &bott, stack<int> &top, char op, int ordem);							//faz netlist usando arvoe n-aria ordenada
void faz_netlist_ordenado_p(list<transistor*> &trans_list, q_node*& root, stack<int> &bott, stack<int> &top, char op, int ordem);
void remove_pseudo(list<transistor*> &trans_list);																				//remove pseudos no inicio e fim
int e_paralelo(transistor* trans, list<transistor*> &trans_list);																//diz se pseudo esta em paralelo com alguem
void subs (string substituir, string substituto, list<transistor*> &trans_list);												//substitui nets
//binaria
void escreve(list<transistor*> trans_list);	//concerta a saida (remove net a mais criado quando expressão mas externa e +) e escreve em .spice
int place_transistores(list<transistor*> &trans_list);						// faz o placement dos transistores (escreve posições nas transistor chains
void left_edge(list<transistor*> trans_list, queue<net> &nets);				//

void clean_stack(stack<int> &stack);

node raiz;
q_node* q_raiz;




int main(int argc, char *argv[])					// TEM QUE ESTAR NO FORMATO (a*(b+c*(d+e))), SEM INVERSORES
{
	if(argc != 2)
	{
		cout<<"Use a equação boolenana como argumento"<<endl;
		exit(1);
	}
	
	stack<int> bott;										// para guardar a net de baixo quando houver nodo com 2 OP * HAHAHAHA
	stack<int> top;											// para guardar net de topo					
	list<transistor*> trans_list_n;							//transistores P
	list<transistor*> trans_list_p;							//transistores P
	queue<net> nets_n;
	string eq = argv[1];
	cout <<"A equação é: "<< eq <<endl;
	ofstream file;
	file.open("Netlists.spice", std::ios::app);
	file<<"A equação é :" << eq<<endl;
	file.close();
	
	//faz binaria
	quebra_portas(eq);
	pinta_arv(&raiz);
	cout<<"Arvore binaria:"<<endl;
	printLevelOrder(&raiz);
	
	//Converte para n-aria
	converte(&raiz, q_raiz);
	cout<<"Arvore N-aria sem pseudo"<<endl;
	percorreEImprime(q_raiz);
	ordena(q_raiz);
	cout<<"Arvore N-aria ja ordenada"<<endl;
	percorreEImprime(q_raiz);
	
	//faz netlist
	cout<<"Netlist Pulldown:"<<endl;
	faz_netlist_ordenado(trans_list_n, q_raiz, bott, top, '0', 0);
	
	clean_stack(bott);
	clean_stack(top);
	
	cout<<endl<<"Netlist Pullup:"<<endl;
	faz_netlist_ordenado_p(trans_list_p, q_raiz, top, bott, '0', 0);
	
	
	remove_pseudo(trans_list_n);
	remove_pseudo(trans_list_p);
	
	list<transistor*>::reverse_iterator saida = trans_list_n.rbegin();
	string saida_n = (*saida)->source;
	saida = trans_list_p.rbegin();
	string saida_p = (*saida)->source;
	subs(saida_p,saida_n, trans_list_p);
	
	cout<<endl<<"Pulldown apos remover pseudos das bordas:"<<endl;
	for(transistor* it : trans_list_n)
	{
		cout<<"M"<<it->num<<" "<<it->drain<<" "<<it->gate<<" "<<it->source<<" posicao: "<<it->pos<<endl;
	}
	//troca saida do pullup para a mesmo do pulldown
	int gaps_n;
	int gaps_p;
	gaps_n = place_transistores(trans_list_n);
	gaps_p = place_transistores(trans_list_p);
	
	file.open("Nets_e_gaps.txt", std::ios::app);
	file<<"A equação é :" << eq<<endl;
	file<<"Número de gaps :" << gaps_n + gaps_p<<endl;
	
	cout<<"Ordenamento do Pulldown com posicao:"<<endl;
	for(transistor* it : trans_list_n)
	{
		cout<<"M"<<it->num<<" "<<it->drain<<" "<<it->gate<<" "<<it->source<<" posicao: "<<it->pos<<endl;
	}
	
	cout<<endl<<"Ordenamento do Pullup com posicao:"<<endl;
	for(transistor* it : trans_list_p)
	{
		cout<<"M"<<it->num<<" "<<it->drain<<" "<<it->gate<<" "<<it->source<<" posicao: "<<it->pos<<endl;
	}
	
	escreve(trans_list_n);
	escreve(trans_list_p);
	left_edge(trans_list_n, nets_n);
	if (!file.is_open()) {
    std::cerr << "Failed to open file." << std::endl;
	}
	file<<"Nets:" <<endl;
	while(!nets_n.empty())
	{
		file<<nets_n.front().nome<<" "<<nets_n.front().inicio<<"-"<<nets_n.front().fim<<" "<<"Linha "<<nets_n.front().linha<<endl;
		nets_n.pop();
	}
	file.close();
	return 0;
}

void quebra_portas(string eq)		
{									
	stack<char> postfix;									//onde ficara a versao postfix da expressao
	faz_postfix(postfix, eq);
	monta_arv(&raiz, postfix);
	
	return;
}

void escreve(list<transistor*> trans_list)
{
	ofstream file;
	file.open("Netlists.spice", std::ios::app);
	list<transistor*>::iterator it = trans_list.begin();
	while(it != trans_list.end())
	{
		if((*it)->tipo == 'n')
		file<<"M"<<(*it)->num<<" "<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<" " <<"GND "<<(*it)->tipo<<"fet"<<endl;
		if((*it)->tipo == 'p')
		file<<"M"<<(*it)->num<<" "<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<" " <<"VDD "<<(*it)->tipo<<"fet"<<endl;
		it++;
	}
	file.close();
	return;
}

int place_transistores(list<transistor*> &trans_list)	
{
	//transistores ocupam duas posicoes, uma para o drain e outra para o gate, iniciando na posicao 1
	//gaps ocupam apenas uma posicao, logo toda vez que mudar de impar para par, houve um gap
	
	list<transistor*>::iterator it = trans_list.begin();
	list<transistor*>::iterator ant = trans_list.begin();										//guarda transistor anterior
	int pos = 1;
	int gaps = 0;
	while(it != trans_list.end())
	{
		if((*it)->gate == 'Z')																	//se for pseudo, remove todos, adiciona 1 na posiçao
		{
			while((*it)->gate == 'Z')
			{
				it = trans_list.erase(it);
			}
			pos++;
			ant = it;
			gaps++;	
		}
		else
		{
			if(ant != it)
			{
				if(((*it)->drain == (*ant)->drain && (*it)->source == (*ant)->source))			//alguns casos continuavam com orientação errada, aqui concerta isso (remendo)
				{
					string temp = (*it)->drain;
					(*it)->drain = (*it)->source;
					(*it)->source = temp;
				}
			}
			(*it)->pos = pos;
			pos+=2;
			ant = it;
			it++;
		}
		
	}
	
	return gaps;
}

void left_edge(list<transistor*> trans_list, queue<net> &nets)
{
	int livre = 1;
	int linha = 1;
	net *temp;
	list<transistor*>::iterator it;
	for(int i = 0; i < net_number; i++)
	{
		int min = 9999;
		int max = 0;
		it = trans_list.begin();
		
		while(it != trans_list.end())
		{
			if((*it)->drain == "n"+to_string(i))											//se encontrar a net
			{
				if((*it)->pos < min)
					min = (*it)->pos;
				if((*it)->pos > max)
					max = (*it)->pos;
			}
			
			if((*it)->source == "n"+to_string(i))											//se encontrar a net
			{
				if((*it)->pos+1 < min)
					min = (*it)->pos+1;
				if((*it)->pos+1 > max)
					max = (*it)->pos+1;
			}
			it++;
		}
		
		if(min != 9999 && max != 0 && (max - min > 1))										// ignora min e max defaults e nets de distancia 1(drain e source podem dividir difusao)
		{
			if(livre <= min)
			{
				temp = new net("NET"+to_string(i), min, max, linha);
				nets.push(*temp);
				livre = max+1;
			}
			else
			{
				linha++;
				temp = new net("NET"+to_string(i), min, max, linha);
				nets.push(*temp);
				livre = max+1;
			}
		}
		
	}
	
	
	return;
}


void printLevelOrder(node *root) {		//da internet : https://www.geeksforgeeks.org/how-to-print-data-in-binary-tree-level-by-level-in-cpp/
        if (root == nullptr) return;  
        
        // Queue to store nodes for level order traversal
        queue<node*> q;  
        // Enqueue the root node
        q.push(root);    
        while (!q.empty()) {
            // Get the current level size
            int levelSize = q.size();  
            // Process all nodes at the current level
            for (int i = 0; i < levelSize; ++i) {
                node* node = q.front();  
                cout << node->tipo<< node->cor << " ";  
                q.pop();  
                
                // Enqueue esquerda child if it exists
                if (node->esquerda != nullptr)
                    q.push(node->esquerda);
                
                // Enqueue direita child if it exists
                if (node->direita != nullptr)
                    q.push(node->direita);
            }
            // Print new line after each level is processed
            cout << endl;  
        }
    }
	
void faz_netlist_ordenado(list<transistor*> &trans_list, q_node*& root, stack<int> &bott, stack<int> &top, char op, int ordem)
{
	transistor *temp;
	if(!root)
		return;
	
	if(root->tipo == '*')															//se *, guarda net de topo em bott, se +, guarda nets em top e bott
	{
		int cont = 0;
		for(q_node* filho : root->filhos)
		{
			
			if(filho->tipo == '*' || filho->tipo == '+')
			{
				faz_netlist_ordenado(trans_list, filho, bott, top, root->tipo, ordem);
			}
			else
			{
				cont++;
				if(top.empty())										//se o bott estiver vazio, conecta no GND e cria novo net
				{
					//cout<<"TOP VAZIO"<<endl;
					cout<<"M"<<trans_number<<" GND"<<" "<<filho->tipo<<" n"<<net_number<<endl;
					temp = new transistor('n', trans_number, "GND", filho->tipo, "n"+to_string(net_number));
					trans_list.push_back(temp);
					top.push(net_number);																		//adiciona novo net em bott
				}
				else
				{
					//cout<<"TOP NAO VAZIO"<<endl;
					cout<<"M"<<trans_number<<" n"<<top.top()<<" "<<filho->tipo<<" n"<<net_number<<endl;
					temp = new transistor('n', trans_number, "n"+to_string(top.top()), filho->tipo, "n"+to_string(net_number));
					trans_list.push_back(temp);
					top.push(net_number);
				}
				
				trans_number++;
				net_number++;
			}

		}
		if(op == '+')																						//quando a op anterior e OR, remove todos os nets criados do stack
		{																									//se houve transistor naquele OR, conecta de volta no bot dele
			for(int i = 0; i < cont; i++)
				top.pop();
			list<transistor*>::reverse_iterator it = trans_list.rbegin();

			if(bott.empty())
			{
				(*it)->source = "GND";
			}
			else
			{
				(*it)->source = "n"+to_string(bott.top());
			}
			cout<<"TROCA: ";
			cout<<"M"<<(*it)->num<<" "<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<endl;
			net_number--;
		}
	}
	//o primeiro cara deve estar tipo em serie, so deve fechar o paralelo quando terminar
	if(root->tipo == '+')																						//se *, guarda net de topo em bott, se +, guarda nets em top e bott
	{
		int primeiro = 0;
		int cont = 0;
		for(q_node* filho : root->filhos)
		{
			
			cont++;
			if(filho->tipo == '*' || filho->tipo == '+')
			{
				faz_netlist_ordenado(trans_list, filho, bott, top, root->tipo, ordem);
				if(ordem == 0)																					//faz o zig-zag quando portas estiverem em paralelo
					ordem = 1;
				else
					ordem = 0;
				//cout<<"TROCOU ORDEM"<<endl;

			}
			else
			{
				if(primeiro == 0)
				{
					if(op == '*' && !top.empty())
					{
						cout<<"M"<<trans_number<<" n"<<top.top()<<" "<<filho->tipo<<" n"<<net_number<<endl;
						temp = new transistor('n', trans_number, "n"+to_string(top.top()), filho->tipo, "n"+to_string(net_number));
						trans_list.push_back(temp);
						bott.push(top.top());
						top.pop();
						top.push(net_number);
						net_number++;
						primeiro = 1;
					}
					else
					{
						if(bott.empty() && top.empty())																		//se o bott e o top estiverem vazios, primeiro do paralelo
						{
							cout<<"M"<<trans_number<<" GND"<<" "<<filho->tipo<<" n"<<net_number<<endl;
							temp = new transistor('n', trans_number, "GND", filho->tipo, "n"+to_string(net_number));
							trans_list.push_back(temp);
							top.push(net_number);																			//bott continua sendo GND, top é novo net
							net_number++;
						}
						else																								
						{																									
							if(!bott.empty() && !top.empty())
							{
								cout<<"M"<<trans_number<<" n"<<bott.top()<<" "<<filho->tipo<<" n"<<top.top()<<endl;								
								temp = new transistor('n', trans_number, "n"+to_string(bott.top()), filho->tipo, "n"+to_string(top.top()));
								trans_list.push_back(temp);
							}
							
							else
								if(bott.empty())
								{
									cout<<"M"<<trans_number<<" GND"<<" "<<filho->tipo<<" n"<<top.top()<<endl;								
									temp = new transistor('n', trans_number, "GND", filho->tipo, "n"+to_string(top.top()));
									trans_list.push_back(temp);
								}
								else
									if(top.empty())
									{
										cout<<"M"<<trans_number<<" n"<<bott.top()<<" "<<filho->tipo<<" n"<<net_number<<endl;								
										temp = new transistor('n', trans_number, "n"+to_string(bott.top()), filho->tipo, "n"+to_string(net_number));
										trans_list.push_back(temp);
										top.push(net_number);
										net_number++;
									}
								
						}
					}

				}
				else
				{
					if(ordem == 0)																							//faz o zig-zag quando portas estiverem em paralelo
						ordem = 1;
					else
						ordem = 0;
					//cout<<"TROCOU ORDEM"<<endl;
					
					if(ordem == 0)
					{
						if(bott.empty() && top.empty())																		//se o bott e o top estiverem vazios, primeiro do paralelo
						{
							cout<<"M"<<trans_number<<" GND"<<" "<<filho->tipo<<" n"<<net_number<<endl;
							temp = new transistor('n', trans_number, "GND", filho->tipo, "n"+to_string(net_number));
							trans_list.push_back(temp);
							top.push(net_number);																			//bott continua sendo GND, top é novo net
							net_number++;
						}
						else																								
						{																									
							if(!bott.empty() && !top.empty())
							{
								cout<<"M"<<trans_number<<" n"<<bott.top()<<" "<<filho->tipo<<" n"<<top.top()<<endl;								
								temp = new transistor('n', trans_number, "n"+to_string(bott.top()), filho->tipo, "n"+to_string(top.top()));
								trans_list.push_back(temp);
							}
							
							else
								if(bott.empty())
								{
									cout<<"M"<<trans_number<<" GND"<<" "<<filho->tipo<<" n"<<top.top()<<endl;								
									temp = new transistor('n', trans_number, "GND", filho->tipo, "n"+to_string(top.top()));
									trans_list.push_back(temp);
								}
								else
									if(top.empty())
									{
										cout<<"M"<<trans_number<<" n"<<bott.top()<<" "<<filho->tipo<<" n"<<net_number<<endl;								
										temp = new transistor('n', trans_number, "n"+to_string(bott.top()), filho->tipo, "n"+to_string(net_number));
										trans_list.push_back(temp);
										top.push(net_number);
										net_number++;
									}
								
						}
				
					}
					if(ordem == 1)
					{
						if(bott.empty() && top.empty())																		//se o bott e o top estiverem vazios, primeiro do paralelo
						{
							cout<<"M"<<trans_number<<" n"<<net_number<<" "<<filho->tipo<<" GND"<<endl;
							temp = new transistor('n', trans_number, "n"+to_string(net_number), filho->tipo, "GND");
							trans_list.push_back(temp);
							top.push(net_number);																			//bott continua sendo GND, top é novo net
							net_number++;
						}
						else																								
						{																									
							if(!bott.empty() && !top.empty())
							{
								cout<<"M"<<trans_number<<" n"<<top.top()<<" "<<filho->tipo<<" n"<<bott.top()<<endl;								
								temp = new transistor('n', trans_number, "n"+to_string(top.top()), filho->tipo, "n"+to_string(bott.top()));
								trans_list.push_back(temp);
							}
									
							else
								if(top.empty())
								{
									cout<<"M"<<trans_number<<" GND"<<" "<<filho->tipo<<" n"<<top.top()<<endl;								
									temp = new transistor('n', trans_number, "n"+to_string(top.top()), filho->tipo, "GND");
									trans_list.push_back(temp);
								}
								else
									if(bott.empty())
									{
										cout<<"M"<<trans_number<<" n"<<net_number<<" "<<filho->tipo<<" n"<<bott.top()<<endl;								
										temp = new transistor('n', trans_number, "n"+to_string(net_number), filho->tipo, "n"+to_string(bott.top()));
										trans_list.push_back(temp);
										top.push(net_number);
										net_number++;
									}
												
						}
					}
				}
			}
			trans_number++;
		}
		if(primeiro == 1)
			bott.pop();
	}
	return;
}

void faz_netlist_ordenado_p(list<transistor*> &trans_list, q_node*& root, stack<int> &bott, stack<int> &top, char op, int ordem)
{
	transistor *temp;
	if(!root)
		return;
	
	if(root->tipo == '+')															//se *, guarda net de topo em bott, se +, guarda nets em top e bott
	{
		int cont = 0;
		for(q_node* filho : root->filhos)
		{
			
			if(filho->tipo == '*' || filho->tipo == '+')
			{
				faz_netlist_ordenado_p(trans_list, filho, bott, top, root->tipo, ordem);
			}
			else
			{
				cont++;
				if(top.empty())										//se o bott estiver vazio, conecta no GND e cria novo net
				{
					//cout<<"TOP VAZIO"<<endl;
					cout<<"M"<<trans_number<<" VDD"<<" "<<filho->tipo<<" n"<<net_number<<endl;
					temp = new transistor('p', trans_number, "VDD", filho->tipo, "n"+to_string(net_number));
					trans_list.push_back(temp);
					top.push(net_number);																		//adiciona novo net em bott
				}
				else
				{
					//cout<<"TOP NAO VAZIO"<<endl;
					cout<<"M"<<trans_number<<" n"<<top.top()<<" "<<filho->tipo<<" n"<<net_number<<endl;
					temp = new transistor('p', trans_number, "n"+to_string(top.top()), filho->tipo, "n"+to_string(net_number));
					trans_list.push_back(temp);
					top.push(net_number);
				}
				
				trans_number++;
				net_number++;
			}

		}
		if(op == '*')																						//quando a op anterior e OR, remove todos os nets criados do stack
		{																									//se houve transistor naquele OR, conecta de volta no bot dele
			for(int i = 0; i < cont; i++)
				top.pop();
			list<transistor*>::reverse_iterator it = trans_list.rbegin();

			if(bott.empty())
			{
				(*it)->source = "VDD";
			}
			else
			{
				(*it)->source = "n"+to_string(bott.top());
			}
			cout<<"TROCA: ";
			cout<<"M"<<(*it)->num<<" "<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<endl;
			net_number--;
		}
	}
	//o primeiro cara deve estar tipo em serie, so deve fechar o paralelo quando terminar
	if(root->tipo == '*')																						//se *, guarda net de topo em bott, se +, guarda nets em top e bott
	{
		int primeiro = 0;
		int cont = 0;
		for(q_node* filho : root->filhos)
		{
			
			cont++;
			if(filho->tipo == '*' || filho->tipo == '+')
			{
				faz_netlist_ordenado_p(trans_list, filho, bott, top, root->tipo, ordem);
				if(ordem == 0)																					//faz o zig-zag quando portas estiverem em paralelo
					ordem = 1;
				else
					ordem = 0;
				//cout<<"TROCOU ORDEM"<<endl;

			}
			else
			{
				if(primeiro == 0)
				{
					if(op == '+' && !top.empty())
					{
						cout<<"M"<<trans_number<<" n"<<top.top()<<" "<<filho->tipo<<" n"<<net_number<<endl;
						temp = new transistor('p', trans_number, "n"+to_string(top.top()), filho->tipo, "n"+to_string(net_number));
						trans_list.push_back(temp);
						bott.push(top.top());
						top.pop();
						top.push(net_number);
						net_number++;
						primeiro = 1;
					}
					else
					{
						if(bott.empty() && top.empty())																		//se o bott e o top estiverem vazios, primeiro do paralelo
						{
							cout<<"M"<<trans_number<<" VDD"<<" "<<filho->tipo<<" n"<<net_number<<endl;
							temp = new transistor('p', trans_number, "VDD", filho->tipo, "n"+to_string(net_number));
							trans_list.push_back(temp);
							top.push(net_number);																			//bott continua sendo GND, top é novo net
							net_number++;
						}
						else																								
						{																									
							if(!bott.empty() && !top.empty())
							{
								cout<<"M"<<trans_number<<" n"<<bott.top()<<" "<<filho->tipo<<" n"<<top.top()<<endl;								
								temp = new transistor('p', trans_number, "n"+to_string(bott.top()), filho->tipo, "n"+to_string(top.top()));
								trans_list.push_back(temp);
							}
							
							else
								if(bott.empty())
								{
									cout<<"M"<<trans_number<<" VDD"<<" "<<filho->tipo<<" n"<<top.top()<<endl;								
									temp = new transistor('p', trans_number, "VDD", filho->tipo, "n"+to_string(top.top()));
									trans_list.push_back(temp);
								}
								else
									if(top.empty())
									{
										cout<<"M"<<trans_number<<" n"<<bott.top()<<" "<<filho->tipo<<" n"<<net_number<<endl;								
										temp = new transistor('p', trans_number, "n"+to_string(bott.top()), filho->tipo, "n"+to_string(net_number));
										trans_list.push_back(temp);
										top.push(net_number);
										net_number++;
									}
								
						}
					}

				}
				else
				{
					if(ordem == 0)																							//faz o zig-zag quando portas estiverem em paralelo
						ordem = 1;
					else
						ordem = 0;
					//cout<<"TROCOU ORDEM"<<endl;
					
					if(ordem == 0)
					{
						if(bott.empty() && top.empty())																		//se o bott e o top estiverem vazios, primeiro do paralelo
						{
							cout<<"M"<<trans_number<<" VDD"<<" "<<filho->tipo<<" n"<<net_number<<endl;
							temp = new transistor('p', trans_number, "VDD", filho->tipo, "n"+to_string(net_number));
							trans_list.push_back(temp);
							top.push(net_number);																			//bott continua sendo GND, top é novo net
							net_number++;
						}
						else																								
						{																									
							if(!bott.empty() && !top.empty())
							{
								cout<<"M"<<trans_number<<" n"<<bott.top()<<" "<<filho->tipo<<" n"<<top.top()<<endl;								
								temp = new transistor('p', trans_number, "n"+to_string(bott.top()), filho->tipo, "n"+to_string(top.top()));
								trans_list.push_back(temp);
							}
							
							else
								if(bott.empty())
								{
									cout<<"M"<<trans_number<<" VDD"<<" "<<filho->tipo<<" n"<<top.top()<<endl;								
									temp = new transistor('p', trans_number, "VDD", filho->tipo, "n"+to_string(top.top()));
									trans_list.push_back(temp);
								}
								else
									if(top.empty())
									{
										cout<<"M"<<trans_number<<" n"<<bott.top()<<" "<<filho->tipo<<" n"<<net_number<<endl;								
										temp = new transistor('p', trans_number, "n"+to_string(bott.top()), filho->tipo, "n"+to_string(net_number));
										trans_list.push_back(temp);
										top.push(net_number);
										net_number++;
									}
								
						}
				
					}
					if(ordem == 1)
					{
						if(bott.empty() && top.empty())																		//se o bott e o top estiverem vazios, primeiro do paralelo
						{
							cout<<"M"<<trans_number<<" n"<<net_number<<" "<<filho->tipo<<" VDD"<<endl;
							temp = new transistor('p', trans_number, "n"+to_string(net_number), filho->tipo, "GND");
							trans_list.push_back(temp);
							top.push(net_number);																			//bott continua sendo GND, top é novo net
							net_number++;
						}
						else																								
						{																									
							if(!bott.empty() && !top.empty())
							{
								cout<<"M"<<trans_number<<" n"<<top.top()<<" "<<filho->tipo<<" n"<<bott.top()<<endl;								
								temp = new transistor('p', trans_number, "n"+to_string(top.top()), filho->tipo, "n"+to_string(bott.top()));
								trans_list.push_back(temp);
							}
									
							else
								if(top.empty())
								{
									cout<<"M"<<trans_number<<" VDD"<<" "<<filho->tipo<<" n"<<top.top()<<endl;								
									temp = new transistor('p', trans_number, "n"+to_string(top.top()), filho->tipo, "GND");
									trans_list.push_back(temp);
								}
								else
									if(bott.empty())
									{
										cout<<"M"<<trans_number<<" n"<<net_number<<" "<<filho->tipo<<" n"<<bott.top()<<endl;								
										temp = new transistor('p', trans_number, "n"+to_string(net_number), filho->tipo, "n"+to_string(bott.top()));
										trans_list.push_back(temp);
										top.push(net_number);
										net_number++;
									}
												
						}
					}
				}
			}
			trans_number++;
		}
		if(primeiro == 1)
			bott.pop();
	}
	return;
}

void remove_pseudo(list<transistor*> &trans_list)
{
	list<transistor*>::iterator it = trans_list.begin();
	
	while(it != trans_list.end())
	{
		if((*it)->gate != 'Z')											//para de remover os primeiros assim que encontra input
			break;
		if(e_paralelo(*it,trans_list))								//se estiver em paralelo, so remove
			it = trans_list.erase(it);
		else															//se estiver em serie, muda nomes do anterior e posterior
		{
			subs((*it)->source, (*it)->drain, trans_list);
			it = trans_list.erase(it);
		}	
	}
	
	list<transistor*>::reverse_iterator it2 = trans_list.rbegin();				
	while(it2 != trans_list.rend())
	{
		if((*it2)->gate != 'Z')														//para de remover os ultimos assim que encontra input
			break;
		if(e_paralelo(*it2,trans_list))												//se estiver em paralelo, so remove
		{
																					// Convert reverse iterator to forward iterator
			it2 = decltype(it2)(trans_list.erase(std::next(it2).base()));
		}
		
		else																		//se estiver em serie, muda nomes do anterior e posterior
		{
			subs((*it2)->source, (*it2)->drain, trans_list);
			auto to_erase = std::next(it2).base();								 	// Convert reverse iterator to forward iterator
			it2 = decltype(it2)(trans_list.erase(std::next(it2).base()));
		}	
	}
	
	while(it != trans_list.end())													//remove pseudo do meio em serie, os em paralelo sao gaps e sao tratados no placement
	{
		if((*it)->gate == 'Z')
		{
			if(!e_paralelo(*it,trans_list))
			{
				subs((*it)->source, (*it)->drain, trans_list);
				it = trans_list.erase(it);
			}
			else
			{
				it++;
			}
		}
		else
		{
			it++;
		}
	}
	
	
}

int e_paralelo(transistor* trans, list<transistor*> &trans_list)
{
	for(transistor* comp : trans_list)
	{
		if(trans->drain == comp->drain && trans->source == comp->source && trans->gate != comp->gate)
			return 1;
		else
			if(trans->drain == comp->source && trans->source == comp->drain && trans->gate != comp->gate)
				return 1;
	}
	
	return 0;
}

void subs (string substituir, string substituto, list<transistor*> &trans_list)
{
	list<transistor*>::iterator it = trans_list.begin();
	while(it != trans_list.end())
	{
		if((*it)->drain == substituir)
			(*it)->drain = substituto;
		
		if((*it)->source == substituir)
			(*it)->source = substituto;
		
		it++;
	}
	return;
}

void clean_stack(stack<int> &pilha)
{
	while(!pilha.empty())
		pilha.pop();
	return;
}