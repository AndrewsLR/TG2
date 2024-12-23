#include <iostream>
#include <stack>
#include <string>
#include <list>
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
	int cor = 0;					//0 = preto-entrada real, 1 = mistoP- preto no topo, 2 = mistoB - branco no topo, 3 = branco-pseudo entrada, 4 = indefinido
	list<struct q_node*> filhos;
}q_node;


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

void converte(node* root, q_node*& new_root) 			//logica vai ate o fundo criando nodo, volta conectando ao pai
{
    if (!root) return; 

    if (!new_root) 
	{
        new_root = new q_node;
        new_root->tipo = root->tipo;
    }

														// Se o nodo binario e n-ario forem iguais, se os filhos forem mesma operacao do pai, vai mais fundo usando o pai
    if (root->tipo == new_root->tipo) 					// caso contrario, vai mais fundo com um novo nodo
	{
        if (root->esquerda) 
		{
            if (root->esquerda->tipo == root->tipo) 
			{
                converte(root->esquerda, new_root);
            } 
			else 
			{
                q_node* child = nullptr;
                converte(root->esquerda, child);
                new_root->filhos.push_back(child);				//conecta novo nodo ao pai
            }
        }

																//mesma logica para a direita
        if (root->direita) {
            if (root->direita->tipo == root->tipo) 
			{
                converte(root->direita, new_root);
            } 
			else 
			{
                q_node* sibling = nullptr;
                converte(root->direita, sibling);
                new_root->filhos.push_back(sibling);
            }
        }
    } 
	else 
	{
															// se o nodo binario e n-ario forem diferentes, cria novo nodo
        q_node* new_child = new q_node;
        new_child->tipo = root->tipo;

        if (root->esquerda) 								// vai para a esquerda com novo nodo e conecta na volta
		{
            q_node* left_child = nullptr;
            converte(root->esquerda, left_child);
            new_child->filhos.push_back(left_child);
        }

        if (root->direita) 									//igual na esquerda
		{
            q_node* right_child = nullptr;
            converte(root->direita, right_child);
            new_child->filhos.push_back(right_child);
        }

        new_root->filhos.push_back(new_child);				// conecta devolta na raiz
    }
}




void percorreEImprime(q_node* &root, const std::string& prefix, bool isLast) {
    if (!root) return; // Caso base: nó nulo

    // Imprime o nó atual
    std::cout << prefix << (isLast ? "└── " : "├── ") << root->tipo <<" "<<root->cor<<"\n";

    // Prefixo para os filhos
    std::string newPrefix = prefix + (isLast ? "    " : "│   ");

    // Itera sobre os filhos
    auto it = root->filhos.begin();
    for (; it != root->filhos.end(); ++it) {
        bool isLastChild = (std::next(it) == root->filhos.end());
        percorreEImprime(*it, newPrefix, isLastChild);
    }
}

	
void ordena(q_node *root)
{
	for(q_node* filho : root->filhos)
	{
		ordena(filho);
	}
	
	if(root->tipo == '+' || root->tipo == '*')
	{
		list<q_node*>::iterator it = root->filhos.begin();
		list<q_node*>::reverse_iterator it2 = root->filhos.rbegin();
		list<q_node*>::iterator ordem = root->filhos.begin();
		
		if((root->filhos.size() % 2) == 0) 								//numero par de filhos
		{
			q_node* pseudo = new q_node;
			pseudo->tipo = 'Z';
			pseudo->cor = 3;
			root->filhos.push_front(pseudo);
		}
		
		
		while(it != root->filhos.end())									//coloca brancos para o inicio, guardando primeira posição livre em ordem
		{
			if((*it)->cor == 3)
			{
				root->filhos.insert(ordem,*it);
				it = root->filhos.erase(it);
				ordem++;
			}
			else
				it++;
		}
		//cout<<"PASSOU LOOP1"<<endl;
		//cout<<"ORDEM APONTA PARA: "<<(*ordem)->tipo<<endl;
		it = ordem;
		int flag_mix = 0;												//se ja encontrou um misto, proximo misto é invertido
		while(it != root->filhos.end())									//coloca mistos
		{
			if((*it)->cor == 2 && flag_mix == 0)
			{
				//cout<<"CAIU AQUI quando "<<(*it)->tipo<<endl;
				root->filhos.insert(ordem,*it);
				it = root->filhos.erase(it);
				ordem++;
				flag_mix = 1;
			}
			else 
			{
				if((*it)->cor == 2 && flag_mix == 1)
				{
					//cout<<"CAIU NESSE quando "<<(*it)->tipo<<endl;
					(*it)->cor = 1;
					(*it)->filhos.reverse();
					root->filhos.push_back(*it);
					it = root->filhos.erase(it);
					
					flag_mix = 0;
				}
				else
					it++;
			}
			
				
		}
		//cout<<"PASSOU LOOP2"<<endl;
		it = root->filhos.begin();
																		//define cor do nodo
		if((*it)->cor == (*it2)->cor)									//se forem iguais, so pode ser branco (3) ou preto (0)
		{
			if((*it)->cor == 0)
			{
				root->cor = 0;
			}
			else
				root->cor = 3;
		}
		else															//se forem diferentes, pode ser mistoP(1), mistoB(2) ou branco(3)
		{
				if((*it)->cor == 0)
					root->cor = 1;
				else 
					if((*it)->cor == 3 && (*it2)->cor == 1)
						root->cor = 3;
					else
						root->cor = 2;
		}
	}
	
	return;
}
	

