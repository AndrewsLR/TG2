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

    char tipo = '0';
	char al = '0';						//preenchido apenas em pseudos, diz se operacao ligada e * ou +
	int cor = 0;						//0 = preto-entrada real, 1 = mistoP- preto no topo, 2 = mistoB - branco no topo, 3 = branco-pseudo entrada, 4 = indefinido
	list<struct q_node*> filhos;
}q_node;

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