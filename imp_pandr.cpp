#include <iostream>
#include <string>
#include <stack>
#include <queue>
using namespace std;


typedef struct node{

    char tipo;
    int cor = 4;					//0 = preto-entrada real, 1 = branco-pseudo entrada,2 = mistoP- preto no topo, 3 = mistoB - branco no topo, 4 = indefinido
    struct node* esquerda = nullptr;
    struct node* direita = nullptr;
	
}node;

int net_number = 0; 									//guarda o menor numero disponivel para criar nova net
void quebra_portas(string eq);
void faz_postfix(stack<char> &postfix, string eq);
void monta_arv(node *ptr, stack<char> &postfix);
void printLevelOrder(node *root);
void pinta_arv(node *root);
void faz_netlist(node *ptr, stack<int> &net_n);
void retorna_ordem(node *root, stack<char> &ordem);

node raiz;

int main(int argc, char *argv[])					// TEM QUE ESTAR NO FORMATO (a*(b+c*(d+e))), SEM INVERSORES
{

	if(argc != 2)
	{
		cout<<"Use a equação boolenana como argumento"<<endl;
		exit(1);
	}

	string eq = argv[1];
	cout <<"A equação é: "<< eq <<endl;
	
	quebra_portas(eq);
	return 0;
}
//(a*(b+c))
void quebra_portas(string eq)		
{									
	stack<char> postfix;
	stack<char> ordem;
	stack<int> net_n;
	faz_postfix(postfix, eq);
	monta_arv(&raiz, postfix);
	pinta_arv(&raiz);
	//printLevelOrder(&raiz);
	retorna_ordem(&raiz, ordem);
	cout <<" ordem : ";
	while(!ordem.empty())
	{
		 cout<< ordem.top()<<",";
		ordem.pop();
	}
	cout<<endl;
	faz_netlist(&raiz, net_n);
	cout<<"SAIDA: n"<< net_n.top()<<endl;
	
		return;
}

void faz_postfix(stack<char>& postfix, string eq)
{
	stack<char> op;
	for(char& c : eq)											//Shunting yard algorithm
	{
		if (c != '(' && c != ')' && c != '+' && c != '*')
		{
			postfix.push(c);
		}
		else{
			if( c != ')')
				op.push(c);
			else
			{
				while (op.top() != '(')
				{
					if( op.top() == '+' || op.top() == '*')
					{
						postfix.push(op.top());
					}
					op.pop();
				}
				op.pop();
			}
		}
	}
	if(!op.empty())
	{
		postfix.push(op.top());
		op.pop();
	}
}

void monta_arv(node *ptr, stack<char>& postfix)
{
	/*cout <<" postfix : ";
	while(!postfix.empty())
	{
		 cout<< postfix.top()<<",";
		postfix.pop();
	}
	cout<<endl;*/
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

void faz_netlist(node *ptr, stack<int>& net_n)							// vai ate as folhas, monta ligacoes com GND (implementando apenas pulldown por enquanto)
{																		// conecta folhas, cria net e guarda no stack, olha stack antes de criar (para nao repetir nome)
																		// em nao folhas, cria net se necessario (serie), utiliza nets do stack e tira do stack
	if(ptr->esquerda->tipo == '+' || ptr->esquerda->tipo == '*')		// verifica se pode ir mais fundo na arvore
	{
		faz_netlist(ptr->esquerda, net_n);
	}
	if(ptr->direita->tipo == '+' || ptr->direita->tipo == '*')
	{
		if(ptr->tipo == '+')
			net_n.pop();
		faz_netlist(ptr->direita, net_n);
	}
	if(ptr->direita->tipo != '+' && ptr->direita->tipo != '*' && ptr->esquerda->tipo != '+' && ptr->esquerda->tipo != '*') //se nao poder ir mais fundo e ambos of filhos forem IN
	{
		if(ptr->tipo == '*')																					//se for AND, em serie (pulldown)
		{
			if(net_n.empty())																					//se o stack estiver fazio, crio apartir do n0, do contrario, cria do stack+1
			{	
				cout<<"n"<<net_number<<" "<<ptr->esquerda->tipo<<" GND"<<endl;
				net_number++;
				cout<<"n"<<net_number<<" "<<ptr->direita->tipo<<" n"<<net_number-1<<endl;
				net_number++;
				net_n.push(net_number-1);
			}
			else
			{
				cout<<"n"<<net_number<<" "<<ptr->esquerda->tipo<<" GND"<<endl;
				net_number++;
				cout<<"n"<<net_number<<" "<<ptr->direita->tipo<<" n"<<net_number-1<<endl;
				net_number++;
				net_n.pop();
				net_n.push(net_number-1);
			}
		}
		if(ptr->tipo == '+')																				//
		{
			if(net_n.empty())
			{
				cout<<"n"<<net_number<<" "<<ptr->esquerda->tipo<<" GND"<<endl;
				cout<<"n"<<net_number<<" "<<ptr->direita->tipo<<" GND"<<endl;
				net_number++;
				net_n.push(net_number-1);
			}
			else
			{
				cout<<"n"<<net_number<<" "<<ptr->esquerda->tipo<<" n"<<net_n.top()<<endl;
				cout<<"n"<<net_number<<" "<<ptr->direita->tipo<<" n"<<net_n.top()<<endl;
				net_number++;
				net_n.pop();
				net_n.push(net_number-1);
			}
		}
	}
	else
		if(ptr->esquerda->tipo != '+' && ptr->esquerda->tipo != '*')
		{
			if(ptr->tipo == '*')
			{
				cout<<"n"<<net_number<<" "<<ptr->esquerda->tipo<<" n"<<net_n.top()<<endl;
				net_number++;
				net_n.pop();
				net_n.push(net_number-1);
			}
			if(ptr->tipo == '+')
			{
				cout<<"n"<<net_n.top()<<" "<<ptr->esquerda->tipo<<" GND"<<endl;
			}
		}
		else
			if(ptr->direita->tipo != '+' && ptr->direita->tipo != '*')
			{
				if(ptr->tipo == '*')
				{
					cout<<"n"<<net_number<<" "<<ptr->direita->tipo<<" n"<<net_n.top()<<endl;
					net_number++;
					net_n.pop();
					net_n.push(net_number-1);
				}
				if(ptr->tipo == '+')
				{
					cout<<"n"<<net_n.top()<<" "<<ptr->direita->tipo<<" GND"<<endl;
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
	
