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
    struct node* filho1 = nullptr;
    struct node* filho2 = nullptr;
	struct node* filho3 = nullptr;
    struct node* filho4 = nullptr;
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

/*void converte(node* root, q_node *new_root)
//desce ate as folhas, volta fundindo em nova arvore
{
	q_node *temp;
	if(root->esquerda == '+' || '*')
	{
		converte(root->esquerda);
	}
	if(root->tipo == new_root->tipo)
	{
		

	}
	else
	{
		temp = new q_node;
	}
	if(root->direita == '+' || '*')
	{
		converte(root->direita);
	}

}*/
