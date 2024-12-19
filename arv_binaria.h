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
   q_node *temp;
	if(root->esquerda->tipo == '+' || root->esquerda->tipo == '*')
	{
		converte(root->esquerda,new_root);
		if(root->tipo != new_root->tipo)
		{
			temp = new q_node;
			temp->tipo = root->tipo;
			temp->filho1 = new_root;
			new_root = temp;
		}
		else
		{
			if(root->direita->tipo != '+' && root->direita->tipo != '*')
				if(new_root->filho3 == nullptr)
				{
					new_root->filho3 = new q_node;
					new_root->filho3->tipo = root->direita->tipo;
				}
				else
				{
					new_root->filho4 = new q_node;
					new_root->filho4->tipo = root->direita->tipo;
				}
		}
	}
	if(root->direita->tipo == '+' || root->direita->tipo == '*')
	{
		converte(root->direita,new_root);
	}
	else if(new_root != nullptr)
	{
		if(new_root->filho1 == nullptr)
		{
			new_root->filho1 = new q_node;
			new_root->filho1->tipo = root->direita->tipo;
		}
		else if(new_root->filho2 == nullptr)
		{
			new_root->filho2 = new q_node;
			new_root->filho2->tipo = root->direita->tipo;
		}
		else if(new_root->filho3 == nullptr)
		{
			new_root->filho3 = new q_node;
			new_root->filho3->tipo = root->direita->tipo;
		}
		else if(new_root->filho4 == nullptr)
		{
			new_root->filho4 = new q_node;
			new_root->filho4->tipo = root->direita->tipo;
		}
	}
	if(root->direita->tipo != '+' && root->direita->tipo != '*' && root->esquerda->tipo != '+' && root->esquerda->tipo != '*')
	{
		new_root = new q_node;
		new_root->tipo = root->tipo;
		new_root->filho1 = new q_node;
		new_root->filho2 = new q_node;
		new_root->filho1->tipo = root->esquerda->tipo;
		new_root->filho2->tipo = root->direita->tipo;
	}
	
	
	return;
}



void percorreEImprime(q_node* &root) {
    if (!root) return; // Caso base: se o nó atual for nulo, encerra a recursão.

    // Imprime o tipo do nó atual.
    std::cout << root->tipo << " ";

    // Chama a função recursivamente para os filhos (se existirem).
    if (root->filho1) percorreEImprime(root->filho1);
    if (root->filho2) percorreEImprime(root->filho2);
    if (root->filho3) percorreEImprime(root->filho3);
    if (root->filho4) percorreEImprime(root->filho4);
}

