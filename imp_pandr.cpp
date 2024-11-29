#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <list>
#include <fstream>
using namespace std;


typedef struct node{

    char tipo;
    int cor = 4;					//0 = preto-entrada real, 1 = branco-pseudo entrada,2 = mistoP- preto no topo, 3 = mistoB - branco no topo, 4 = indefinido
    struct node* esquerda = nullptr;
    struct node* direita = nullptr;
	
}node;

typedef struct transistor{
	char tipo;
	int	num;
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
}transistor;
	
int net_number = 1; 																			//guarda o menor numero disponivel para criar nova net
int trans_number = 1;																			//guarda o menor numero disponivel para criar transistor
void quebra_portas(string eq);																	//chama faz_postfix e monta_arvore
int precedencia(char op);
void faz_postfix(stack<char> &postfix, string eq);												//faz postfix
void monta_arv(node *ptr, stack<char> &postfix);												//monta a arvore
void printLevelOrder(node *root);																//printa a arvore
void pinta_arv(node *root);																		//pinta a arvore (faz algoritmo de Uehara e Cleemput
void retorna_ordem(node *root, stack<char> &ordem);												//


void faz_netlist(node *ptr, stack<int> &net_n, list<transistor*> &trans_list, queue<int> &fix, stack<int> &bott);					//faz o netlist e coloca em uma lista
void concerta_e_escreve(string subs, stack<int> &net_n, list<transistor*> trans_list, list<string> subs_list);	//concerta a saida (remove net a mais criado quando expressão mas externa e +) e escreve em .spice

node raiz;

int main(int argc, char *argv[])					// TEM QUE ESTAR NO FORMATO (a*(b+c*(d+e))), SEM INVERSORES
{
	if(argc != 2)
	{
		cout<<"Use a equação boolenana como argumento"<<endl;
		exit(1);
	}
	
	stack<char> ordem;										//onde retornara a ordem das entradas + gaps(ordem contraria
	stack<int> net_n;										//usado para conectar as nets durante a criacao do netlist, ao final tem o net de saida + o net a ser trocado pelo de saida
	queue<int> fix;
	stack<int> bott;										// para guardar a net de baixo quando houver nodo com 2 OP * HAHAHAHA
	list<transistor*> trans_list;							//todos os transistores criados
	list<string> subs_list;									//lista de nomes de net a serem trocados
	string eq = argv[1];
	cout <<"A equação é: "<< eq <<endl;
	
	quebra_portas(eq);
	string subs ="0";
	pinta_arv(&raiz);
	printLevelOrder(&raiz);
	retorna_ordem(&raiz, ordem);
	cout <<" ordem : ";
	while(!ordem.empty())
	{
		 cout<< ordem.top()<<",";
		ordem.pop();
	}
	cout<<endl;
	faz_netlist(&raiz, net_n, trans_list, fix, bott);	
	
	if(!fix.empty())
	{
		subs = "n"+to_string(fix.back());
	}
	while(fix.size() > 1)
	{
			subs_list.push_back("n"+to_string(fix.front()));
			cout<<"CONTEUDO FINAL DO STACK"<<fix.front()<<endl;
			fix.pop();
	}
	
	if(trans_list.empty())
	{
		cout<<"LISTA VAZIA";
		exit(1);
	}
		
	concerta_e_escreve(subs, net_n, trans_list, subs_list);
	
	return 0;
}

void quebra_portas(string eq)		
{									
	stack<char> postfix;									//onde ficara a versao postfix da expressao
	faz_postfix(postfix, eq);
	monta_arv(&raiz, postfix);
	
	return;
}

void faz_postfix(stack<char>& postfix, string eq)
{
    stack<char> op;
    
    for(char& c : eq)
    {
        if (isalnum(c))
        {
            postfix.push(c);
        }
        else if (c == '(')
        {
            op.push(c);
        }
        else if (c == ')')
        {
            while (!op.empty() && op.top() != '(')
            {
                postfix.push(op.top());
                op.pop();
            }
            op.pop();
        }
        else if (c == '+' || c == '*' || c == '-' || c == '/')
        {

            while (!op.empty() && op.top() != '(' && precedencia(op.top()) >= precedencia(c))
            {
                postfix.push(op.top());
                op.pop();
            }
            op.push(c);
        }
    }

    while (!op.empty())
    {
        if (op.top() != '(')
            postfix.push(op.top());
        op.pop();
    }
}


int precedencia(char op)
{
	if(op == '*')
		return 2;
	if(op == '+')
		return 1;
	return 0;
}

void monta_arv(node *ptr, stack<char>& postfix)
{
	cout<<" postfix : ";
	while(!postfix.empty())
	{
		 cout<< postfix.top()<<",";
		postfix.pop();
	}
	cout<<endl;
	return;
	if(postfix.top() == '+' || postfix.top() == '*')
	{
		ptr->tipo = postfix.top();
		postfix.pop();
		ptr->direita = new node;
		monta_arv(ptr->direita,postfix);
		ptr->esquerda = new node;
		monta_arv(ptr->esquerda,postfix);
		
	}
	else
	{
		ptr->tipo = postfix.top();
		ptr->cor = 0;
		postfix.pop();
	}
	
	return;
}

void pinta_arv(node *root)
{
	//enquanto tiver filhos que sao operacoes, desce para aquele filho, se filhos forem ambos in, cria pseudo, pinta nodo, retorna
	//ao retornar criar pseudo, verifica cor de filhos, roda algoritmo, se necessario flip, cascata flip para filhos mistos
	// ao terminar percorre arvore da esquerda para a direita para obter ordem TALVEZ OUTRA FUNCAO
	// na realidade sempre existe um pseudo, apenas compara cores dos filhos e decide cor, mudar cor caso necessario (flip)
	if(root->direita->tipo == '*' || root->direita->tipo == '+')
	{
		pinta_arv(root->direita);
	}
	
	if(root->esquerda->tipo == '+' || root->esquerda->tipo == '*')
	{
		pinta_arv(root->esquerda);
	}
	else
	{
		root->cor = 3; //mistob = branco no topo
	}
	if(root->esquerda->cor != 4 && root->direita->cor != 4)
	{
		if(root->esquerda->cor == root->direita->cor)
		{
			if(root->esquerda->cor == 0)
			{
				root->cor = 3; // mistob = branco no topo
			}
			else
			{
				if(root->direita->cor == 3)
				{
					root->direita->cor = 2;
				}
				
				root->cor = 1;
			}
		}
		else
		{
			root->cor = 3; //mistob = branco no topo
		}
	} 
	return;
}

void retorna_ordem(node *root, stack<char> &ordem) 	//se porta :coloca quebra no stack (tipo da porta) se iniciar em branco e navega filhos esquerda direita
{													//se in : coloca in no stack
	if(root->tipo == '+' || root->tipo == '*')
	{
		if(root->cor == 3 || root->cor == 1)
		{
			ordem.push(root->tipo);
			retorna_ordem(root->esquerda, ordem);
			retorna_ordem(root->direita, ordem);
		}
		else
		{
			retorna_ordem(root->direita, ordem);
			retorna_ordem(root->esquerda, ordem);
			ordem.push(root->tipo);
		}
	}
	else
	{
		ordem.push(root->tipo);
	}
	return;
}

void faz_netlist(node *ptr, stack<int>& net_n,list<transistor*> &trans_list, queue<int> &fix, stack<int> &bott)							// vai ate as folhas, monta ligacoes com GND (implementando apenas pulldown por enquanto)
{																		// conecta folhas, cria net e guarda no stack, olha stack antes de criar (para nao repetir nome)
																		// em nao folhas, cria net se necessario (serie), utiliza nets do stack e tira do stack
	int topo_esq = 0;
	int topo_dir = 0;
	transistor *temp;
	
	if(ptr->esquerda->tipo == '+' || ptr->esquerda->tipo == '*')		// verifica se pode ir mais fundo na arvore
	{
		
		faz_netlist(ptr->esquerda, net_n, trans_list, fix, bott);
	}
	if(ptr->direita->tipo == '+' || ptr->direita->tipo == '*')
	{
		if(ptr->tipo == '+' && (ptr->esquerda->tipo == '+' || ptr->esquerda->tipo == '*'))											
		{
			
			if(!net_n.empty())
			{
				cout<<"INDO PRO OUTRO LADO COM: "<<net_n.top()<<endl;
				topo_esq = net_n.top();
				net_n.pop();
			}	
		}

		if(ptr->tipo == '*' && (ptr->esquerda->tipo == '+' || ptr->esquerda->tipo == '*'))
		{
			bott.push((net_n.top()));
		}
			faz_netlist(ptr->direita, net_n, trans_list, fix, bott);
			if(!net_n.empty())
				topo_dir = net_n.top();
			if(ptr->tipo == '*' && (ptr->esquerda->tipo == '+' || ptr->esquerda->tipo == '*'))
			{
				bott.pop();
			}
		
	}
	if(ptr->direita->tipo != '+' && ptr->direita->tipo != '*' && ptr->esquerda->tipo != '+' && ptr->esquerda->tipo != '*') //se nao poder ir mais fundo e ambos of filhos forem IN
	{
		
		if(ptr->tipo == '*')																					//se for AND, em serie (pulldown)
		{
			if(net_n.empty())																					//olha na variavel global net_number numero disponivel quando necessario criar net
			{	
					cout<<"n"<<net_number<<" "<<ptr->esquerda->tipo<<" GND"<<endl;									//coloca 2 transistores em serie ligado ao GND (stack vazio)
					temp = new transistor('n', trans_number, "n"+to_string(net_number), ptr->esquerda->tipo, "GND");
					trans_list.push_back(temp);
					trans_number++;
					net_number++;
					cout<<"n"<<net_number<<" "<<ptr->direita->tipo<<" n"<<net_number-1<<endl;
					temp = new transistor('n', trans_number, "n"+to_string(net_number), ptr->direita->tipo, "n"+to_string(net_number-1));
					trans_list.push_back(temp);
					trans_number++;
					net_number++;
					net_n.push(net_number-1);
				
			}
			else
			{
					cout<<"n"<<net_number<<" "<<ptr->esquerda->tipo<<" n"<<net_n.top()<<endl;									//2 transistores em serie ligados ao net no topo do stack
					temp = new transistor('n', trans_number, "n"+to_string(net_number), ptr->esquerda->tipo,"n"+to_string(net_n.top()));
					trans_list.push_back(temp);
					trans_number++;
					net_number++;
					net_n.pop();
					cout<<"n"<<net_number<<" "<<ptr->direita->tipo<<" n"<<net_number-1<<endl;
					temp = new transistor('n', trans_number, "n"+to_string(net_number), ptr->direita->tipo, "n"+to_string(net_number-1));
					trans_list.push_back(temp);
					trans_number++;
					net_number++;
					net_n.push(net_number-1);
			}
		}
		if(ptr->tipo == '+')																				//mesmo que AND, porém em paralelo
		{
			if(net_n.empty())
			{
					cout<<"n"<<net_number<<" "<<ptr->esquerda->tipo<<" GND"<<endl;
					temp = new transistor('n', trans_number, "n"+to_string(net_number), ptr->esquerda->tipo, "GND");
					trans_list.push_back(temp);
					trans_number++;
					cout<<"n"<<net_number<<" "<<ptr->direita->tipo<<" GND"<<endl;
					temp = new transistor('n', trans_number, "n"+to_string(net_number), ptr->direita->tipo, "GND");
					trans_list.push_back(temp);
					trans_number++;
					net_number++;
					net_n.push(net_number-1);
			}
			else
			{
				cout<<"n"<<net_number<<" "<<ptr->esquerda->tipo<<" n"<<net_n.top()<<endl;
				temp = new transistor('n', trans_number, "n"+to_string(net_number), ptr->esquerda->tipo, "n"+to_string(net_n.top()));
				trans_list.push_back(temp);
				trans_number++;
				cout<<"n"<<net_number<<" "<<ptr->direita->tipo<<" n"<<net_n.top()<<endl;
				temp = new transistor('n', trans_number, "n"+to_string(net_number), ptr->direita->tipo, "n"+to_string(net_n.top()));
				trans_list.push_back(temp);
				trans_number++;
				net_number++;
				net_n.push(net_number-1);
			}
		}
	}
	else
		if(ptr->esquerda->tipo != '+' && ptr->esquerda->tipo != '*')									//quando tiver apenas um filho, conecta em paralelo ou serie de acordo com a op
		{
			if(ptr->tipo == '*' && !net_n.empty())																		//olha para o stack para saber em quem conectar
			{
				cout<<"n"<<net_number<<" "<<ptr->esquerda->tipo<<" n"<<net_n.top()<<endl;
				temp = new transistor('n', trans_number, "n"+to_string(net_number), ptr->esquerda->tipo, "n"+to_string(net_n.top()));
				trans_list.push_back(temp);
				trans_number++;
				net_number++;
				net_n.pop();
				net_n.push(net_number-1);
			}
			if(ptr->tipo == '+' && !net_n.empty())
			{
					if(bott.empty())
					{
						cout<<"n"<<net_n.top()<<" "<<ptr->esquerda->tipo<<" GND"<<endl;
						temp = new transistor('n', trans_number, "n"+to_string(net_n.top()), ptr->esquerda->tipo, "GND");
					}
					else
					{
						cout<<"n"<<net_n.top()<<" "<<ptr->esquerda->tipo<<" n"<<bott.top()<<endl;
						temp = new transistor('n', trans_number, "n"+to_string(net_n.top()), ptr->esquerda->tipo, "n"+to_string(bott.top()));
					}
					trans_list.push_back(temp);
					trans_number++;
			}
		}
		else
			if(ptr->direita->tipo != '+' && ptr->direita->tipo != '*')
			{
				if(ptr->tipo == '*' && !net_n.empty())
				{
					cout<<"n"<<net_number<<" "<<ptr->direita->tipo<<" n"<<net_n.top()<<endl;
					temp = new transistor('n', trans_number, "n"+to_string(net_number), ptr->direita->tipo, "n"+to_string(net_n.top()));
					trans_list.push_back(temp);
					trans_number++;
					net_number++;
					net_n.pop();
					net_n.push(net_number-1);
				}
				if(ptr->tipo == '+' && !net_n.empty())
				{
					if(bott.empty())
					{
						cout<<"n"<<net_n.top()<<" "<<ptr->direita->tipo<<" GND"<<endl;
						temp = new transistor('n', trans_number, "n"+to_string(net_n.top()), ptr->direita->tipo, "GND");
					}
					else
					{
						cout<<"n"<<net_n.top()<<" "<<ptr->direita->tipo<<" n"<<bott.top()<<endl;
						temp = new transistor('n', trans_number, "n"+to_string(net_n.top()), ptr->direita->tipo, "n"+to_string(bott.top()));
					}
						trans_list.push_back(temp);
						trans_number++;
				}
			}
		else																							//quando ambos os
			{
				if(ptr->tipo == '+' && topo_esq != 0)
				{	
					
					cout<<"n"<<topo_esq<<" = n"<<topo_dir<<endl;
					
					if(fix.empty())
					{
						fix.push(topo_dir);
					}	
					fix.push(topo_esq);
				}
				
				while(net_n.size() > 1)
					{
						int temp = net_n.top();
						net_n.pop();
						net_n.pop();
						net_n.push(temp);
					}
			}
	
	return;
}

void concerta_e_escreve(string subs, stack<int> &net_n, list<transistor*> trans_list, list<string> subs_list)
{
	ofstream file;
	file.open("TESTE.spice");
	list<transistor*>::iterator it = trans_list.begin();
	list<string>::iterator it2 = subs_list.begin();
	while(it != trans_list.end())
	{
		while(it2 != subs_list.end())
		{
			if((*it)->drain == *it2)
			{
				(*it)->drain = subs;
			}
			if((*it)->source == *it2)
			{
				(*it)->source = subs;
			}
			it2++;
		}
		it2 = subs_list.begin();
		if((*it)->tipo == 'n')
		file<<"M"<<(*it)->num<<" "<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<" " <<"GND "<<(*it)->tipo<<"fet"<<endl;
		it++;
	}
	file.close();
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
	
