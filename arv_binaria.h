#include <iostream>
#include <stack>
#include <string>
using namespace std;

typedef struct node{

    char tipo;
	char pai;
    int cor = 4;					//0 = preto-entrada real, 1 = mistoP- preto no topo, 2 = mistoB - branco no topo, 3 = branco-pseudo entrada, 4 = indefinido
    struct node* esquerda = nullptr;
    struct node* direita = nullptr;
	
}node;

typedef struct q_node{

    char tipo;				
    struct q_node* filho1 = nullptr;
    struct q_node* filho2 = nullptr;
	struct q_node* filho3 = nullptr;
    struct q_node* filho4 = nullptr;
}q_node;

void cmp_adiciona_filho(q_node* new_root, q_node* filho, node* root);

int precedencia(char op)
{
	if(op == '*')
		return 2;
	if(op == '+')
		return 1;
	return 0;
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
	return;
}

void monta_arv(node *ptr, stack<char>& postfix)
{
	/*cout<<" postfix : ";
	while(!postfix.empty())
	{
		 cout<< postfix.top()<<",";
		postfix.pop();
	}
	cout<<endl;
	return;*/
	if(postfix.top() == '+' || postfix.top() == '*')
	{
		ptr->tipo = postfix.top();
		postfix.pop();
		ptr->direita = new node;
		ptr->direita->pai = ptr->tipo;
		monta_arv(ptr->direita,postfix);
		ptr->esquerda = new node;
		ptr->esquerda->pai = ptr->tipo;
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
	//0 = preto-entrada real, 1 = mistoP- preto no topo, 2 = mistoB - branco no topo, 3 = branco-pseudo entrada, 4 = indefinido Prioridade pra quem deve ser filho da esquerda
	//enquanto tiver filhos que sao operacoes, desce para aquele filho, se filhos forem ambos in, cria pseudo, pinta nodo, retorna
	//ao retornar criar pseudo, verifica cor de filhos, roda algoritmo, se necessario faz flip em filho
	// na realidade sempre existe um pseudo, apenas compara cores dos filhos e decide cor, mudar cor caso necessario (flip)
	if(root->direita->tipo == '*' || root->direita->tipo == '+')
	{
		pinta_arv(root->direita);
	}
	
	if(root->esquerda->tipo == '+' || root->esquerda->tipo == '*')
	{
		pinta_arv(root->esquerda);
	}
	// vai ate o fundo da arvore
	else
	{
		root->cor = 2; //mistob = branco no topo
	}
	if(root->esquerda->cor != 4 && root->direita->cor != 4)
	{
		if(root->esquerda->cor == root->direita->cor)	//se as cores do filhos forem iguais
		{
			if(root->esquerda->cor == 0)				//se pretos, se torna misto com branco primeiro
			{
				root->cor = 2;
			}
			else
			{
				if(root->direita->cor == 2)				//se forem mistos, faz flip do da direita e se torna branco
				{
					root->direita->cor = 1;
				}
				
				root->cor = 3;
			}
		}
		else
		{
			if(root->direita->cor > root->esquerda->cor) 	//troca de lugar baseado na prioridade do primeiro comentario da funcao
				{
					node *temp = root->esquerda;
					root->esquerda = root->direita;
					root->direita = temp;
					root->cor = 2;							//se torna misto com branco primeiro
				}
			else
			{
				root->cor = 2;
			}
			
		}
	} 
	return;
}

void converte(node* root, q_node*& new_root) {
   q_node *temp = nullptr;
   new_root = temp;

	if(root->esquerda->tipo == '+' || root->esquerda->tipo == '*')		//se o filho da esquerda for operação, vai mais fundo
	{
		converte(root->esquerda,new_root);
		
		if(root->tipo != new_root->tipo)								//quando volta, se a operação do filho for diferente, adiciona como filho, se for igual, ignora, tornado o filho o novo root
		{
			temp = new q_node;
			temp->tipo = root->tipo;
			temp->filho1 = new_root;
			new_root = temp;
			
		}
	}

	else
	{
		if(root->direita->tipo == '+' || root->direita->tipo == '*')		//se apenas o filho da esquerda for in, coloca ele como filho
		{
			cout<<"NAO ERA PRA TA AQUI"<<endl;
			if(new_root->filho3 != nullptr)
			{
				new_root->filho4 = new_root->filho3;
				new_root->filho3 = new_root->filho2;
				new_root->filho2 = new_root->filho1;
				temp = new q_node;
				temp->tipo = root->direita->tipo;
				new_root->filho1 = temp;
			}
			else if(new_root->filho2 != nullptr)
			{
				new_root->filho3 = new_root->filho2;
				new_root->filho2 = new_root->filho1;
				temp = new q_node;
				temp->tipo = root->direita->tipo;
				new_root->filho1 = temp;
			}
			else if(new_root->filho1 != nullptr)
			{
				new_root->filho2 = new_root->filho1;
				temp = new q_node;
				temp->tipo = root->direita->tipo;
				new_root->filho1 = temp;
			}
		}
	}
	
	if(root->direita->tipo == '+' || root->direita->tipo == '*')
	{
		if(new_root == nullptr)												// vai mais fundo pra direita com primeiro nodo vazio
		{
			converte(root->direita,new_root);
			cmp_adiciona_filho(new_root, new_root, root);
		}	
		else if(new_root->filho1 == nullptr)
		{
			converte(root->direita,new_root->filho1);
			cmp_adiciona_filho(new_root, new_root->filho1, root);
		}
		else if(new_root->filho2 == nullptr)
		{
			converte(root->direita,new_root->filho2);
			cmp_adiciona_filho(new_root, new_root->filho2, root);
		}
		else if(new_root->filho3 == nullptr)
		{
			converte(root->direita,new_root->filho3);
			cmp_adiciona_filho(new_root, new_root->filho3, root);
		}
		else
		{
			converte(root->direita,new_root->filho4);
			cmp_adiciona_filho(new_root, new_root->filho4, root);
		}
		
	}
		
	else 
	{
		if(new_root != nullptr)
		{
			if(new_root->filho1 == nullptr)
			{
				cout<<"AQUI 1 x"<<endl;
				new_root->filho1 = new q_node;
				new_root->filho1->tipo = root->direita->tipo;
			}
			else if(new_root->filho2 == nullptr)
			{
				cout<<"AQUI 2 x"<<endl;
				new_root->filho2 = new q_node;
				new_root->filho2->tipo = root->direita->tipo;
			}
			else if(new_root->filho3 == nullptr)
			{
				cout<<"AQUI 3 x"<<endl;
				new_root->filho3 = new q_node;
				new_root->filho3->tipo = root->direita->tipo;
			}
			else
			{
				cout<<"AQUI 4 x"<<endl;
				new_root->filho4 = new q_node;
				new_root->filho4->tipo = root->direita->tipo;
			}
			
		}
	}

	if(root->direita->tipo != '+' && root->direita->tipo != '*' && root->esquerda->tipo != '+' && root->esquerda->tipo != '*') // se ambos os filhos forem in, cria nodo
	{
		new_root = new q_node;
		new_root->tipo = root->tipo;
		new_root->filho1 = new q_node;
		new_root->filho2 = new q_node;
		new_root->filho1->tipo = root->esquerda->tipo;
		new_root->filho2->tipo = root->direita->tipo;
		new_root->filho3 = nullptr;
		new_root->filho4 = nullptr;
	}
	
	
	return;
}

void percorreEImprime(q_node* &root, const std::string& prefix, bool isLast) {
    if (!root) return;

    std::cout << prefix << (isLast ? "└── " : "├── ") << root->tipo << "\n";

    std::string newPrefix = prefix + (isLast ? "    " : "│   ");

    q_node* filhos[] = {root->filho1, root->filho2, root->filho3, root->filho4};
    int count = 0;

    for (auto filho : filhos) {
        if (filho) count++;
    }

    for (int i = 0, printed = 0; i < 4; i++) {
        if (filhos[i]) {
            percorreEImprime(filhos[i], newPrefix, ++printed == count);
        }
    }
}

void cmp_adiciona_filho(q_node* new_root, q_node* filho, node* root) //função para adicionar nodos da direita, separado em função por que depende qual filho
{
	q_node *temp = nullptr;
	if(root->tipo != filho->tipo) 
		{
			if(new_root->filho1 == nullptr)
			{
				temp = new q_node;
				temp->tipo = root->tipo;
				temp->filho1 = filho;
				new_root = temp;
			}
			else if(new_root->filho2 == nullptr)
			{
				temp = new q_node;
				temp->tipo = root->tipo;
				temp->filho2 = filho;
				new_root = temp;
			}
			else if(new_root->filho3 == nullptr)
			{
				temp = new q_node;
				temp->tipo = root->tipo;
				temp->filho3 = filho;
				new_root = temp;
			}
			else
			{
				temp = new q_node;
				temp->tipo = root->tipo;
				temp->filho4 = filho;
				new_root = temp;
			}
			
		}
		else		//se forem iguais, verifica se existe filho in na esquerda, adiciona ele e rouba filhos do filho da direita
		{
				if(root->esquerda->tipo != '+' && root->esquerda->tipo != '*')
				{
					if(new_root->filho1 == nullptr)
					{
						new_root->filho1 = new q_node;
						new_root->filho1->tipo = root->esquerda->tipo;
					}
					else if(new_root->filho2 == nullptr)
					{
						new_root->filho2 = new q_node;
						new_root->filho2->tipo = root->esquerda->tipo;
					}
					else if(new_root->filho3 == nullptr)
					{
						new_root->filho3 = new q_node;
						new_root->filho3->tipo = root->esquerda->tipo;
					}
					else
					{
						new_root->filho4 = new q_node;
						new_root->filho4->tipo = root->esquerda->tipo;
					}
				}
				else //PROBLEMA AQUI
				{
					//rouba filho do filho da esquerda
					if(new_root->filho1 == nullptr)
					{
						new_root->filho1 = new q_node;
						new_root->filho1->tipo = root->esquerda->esquerda->tipo;
					}
					else if(new_root->filho2 == nullptr)
					{
						new_root->filho2 = new q_node;
						new_root->filho2->tipo = root->esquerda->esquerda->tipo;
					}
					else if(new_root->filho3 == nullptr)
					{
						new_root->filho3 = new q_node;
						new_root->filho3->tipo = root->esquerda->esquerda->tipo;
					}
					else
					{
						new_root->filho4 = new q_node;
						new_root->filho4->tipo = root->esquerda->esquerda->tipo;
					}
					
					//rouba filho da direita do filho
					if(new_root->filho1 == nullptr)
					{
						new_root->filho1 = new q_node;
						new_root->filho1->tipo = root->esquerda->direita->tipo;
					}
					else if(new_root->filho2 == nullptr)
					{
						new_root->filho2 = new q_node;
						new_root->filho2->tipo = root->esquerda->direita->tipo;
					}
					else if(new_root->filho3 == nullptr)
					{
						new_root->filho3 = new q_node;
						new_root->filho3->tipo = root->esquerda->direita->tipo;
					}
					else
					{
						new_root->filho4 = new q_node;
						new_root->filho4->tipo = root->esquerda->direita->tipo;
					}
				}
		}
}
	


