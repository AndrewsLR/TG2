#include "arv_bi_n.h"

using namespace std;

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
		list<q_node*>::iterator ordem = root->filhos.begin();
		
		if((root->filhos.size() % 2) == 0) 								//numero par de filhos, coloca pseudo
		{
			q_node* pseudo = new q_node;
			pseudo->tipo = 'Z';
			pseudo->al = root->tipo;
			pseudo->cor = 3;
			root->filhos.push_front(pseudo);
			root->cor = 2;
		}
		list<q_node*>::iterator set_al = root->filhos.begin();								//guarda operacao em cada filho
		for(set_al = root->filhos.begin(); set_al != root->filhos.end(); set_al++)
		{
			(*set_al)->al = root->tipo;
		}
		

		while(it != root->filhos.end())									//coloca pseudos para o inicio, guardando primeira posição livre em ordem
		{

			cout<<"AQUI"<<endl;
			if((*it)->cor == 3)
			{
				if(it != ordem)
				{
					auto it_next = next(it);
					root->filhos.splice(ordem,root->filhos,it);
					it = it_next;
					//root->filhos.insert(ordem,*it);
					//it = root->filhos.erase(it);
					//q_node temp;
					//temp.tipo = (*ordem)->tipo;											//usando essas copias com temp da erro em (a*(b+c*(d+e+f*g)))
					//temp.cor = (*ordem)->cor;
					//temp.filhos = (*ordem)->filhos;

					//(*ordem)->tipo = (*it)->tipo;
					//(*ordem)->cor = (*it)->cor;
					//(*ordem)->filhos = (*it)->filhos;

					//(*it)->tipo = temp.tipo;
					//(*it)->cor = temp.cor;
					//(*it)->filhos = temp.filhos;
					//ordem++;
					//it++;
				}
				else
				{
					ordem++;
					it++;
				}
			}
			else
				it++;
		}
		//cout<<"PASSOU LOOP1"<<endl;
		it = ordem;
		int flag_mix = 0;
		
		while(it != root->filhos.end())
		{
			cout<<"HAHA"<<endl;
			it++;
		}
		it = ordem;																	//se ja encontrou um misto, proximo misto é invertido
		while(it != root->filhos.end())									//coloca mistos
		{
			cout<< "Ordenando "<<root->tipo<<endl;
			cout<<"IMEDIATAMENTE DEPOIS"<<endl;
			if(it == root->filhos.end())
					cout<<"IT APONTA PRO FIM"<<endl;
			//cout<<"a cor do it e "<<(*it)->cor<<endl; // IT NAO E ACESSAVEL AQUI
			if((*it)->cor == 2 && flag_mix == 0)
			{
				cout<<"TESTE 1"<<endl;
				//cout<<"Primeiro filho misto "<<(*it)->tipo<<endl;
				//cout<<"Ultima entrada "<<(*it)->filhos.back()->tipo<<endl;
				if(ordem != it)
				{
					auto it_next = next(it);
					root->filhos.splice(ordem,root->filhos,it);
					it = it_next;
					//root->filhos.insert(ordem,*it);									//Usando insert e erase, da erro em (a*(b+c+d*e+f*g*h))
					//it = root->filhos.erase(it);
					//q_node temp;
					//temp.tipo = (*ordem)->tipo;											//usando essas copias com temp da erro em (a*(b+c*(d+e+f*g)))
					//temp.cor = (*ordem)->cor;
					//temp.filhos = (*ordem)->filhos;

					//(*ordem)->tipo = (*it)->tipo;
					//(*ordem)->cor = (*it)->cor;
					//(*ordem)->filhos = (*it)->filhos;

					//(*it)->tipo = temp.tipo;
					//(*it)->cor = temp.cor;
					//(*it)->filhos = temp.filhos;
					//it++;
					//ordem++;
					cout<<"ELEMENTO APAGADO"<<endl;
				}
				else
				{
					ordem++;
					it++;
				}
				flag_mix = 1;
				//MUDANÇA AQUI
				list<q_node*>::iterator busca = it;
				while(busca != root->filhos.end() && ordem != root->filhos.end())			//insere todos os reais após ordenar o primeiro misto
				{
					cout<<"Procurando entradas para ordenar"<<endl;
					//cout<<"Buscando  "<<(*busca)->tipo<<"da cor "<<(*busca)->cor<<endl;
					//cout<<"Ordem  "<<(*ordem)->tipo<<endl;
					if((*busca)->cor == 0)
					{
						cout<< "Achou entrada "<<(*busca)->tipo<<endl;
						if(busca != ordem)
						{
							auto it_next = next(busca);
							root->filhos.splice(ordem,root->filhos,busca);
							busca = it_next;
							//root->filhos.insert(ordem, *busca);
							//cout<<"TESTANDO ANTES: "<<(*busca)->tipo<<endl;
							//busca = root->filhos.erase(busca);
							//cout<<"TESTANDO DEPOIS: "<<(*busca)->tipo<<endl;
						}
						else
						{
							ordem++;
							busca++;
						}
							
						
					}
					else
					{
						busca++;
					}
					//cout<<"Fim do loop"<<endl;
				}
				//it = ordem;
				cout<<"Saiu do primeiro misto"<<endl;
				if(it == root->filhos.end())
					cout<<"IT APONTA PRO FIM"<<endl;
			}
			else 
			{
				cout<<"ENTOU NO ELSE"<<endl;
				if(!(*it)->filhos.empty())
					cout<<"Chegou logo antes de entrar "<<(*it)->tipo<< "Com ultimo filho "<<(*it)->filhos.back()->tipo<<endl;
				if((*it)->cor == 2 && flag_mix == 1)
				{
					cout<<"Segundo misto "<<(*it)->tipo<<endl;
					(*it)->cor = 1;
					inverte((*it)->filhos);
					(*it)->filhos.reverse();
					//(*it)->cor = 1;
					//root->filhos.push_back(*it);
					//it = root->filhos.erase(it);
					
					flag_mix = 0;
					ordem++;
					it++;
				}
				else
				{
					it++;
					cout<<"INCREMENTOU IT NO ELSE"<<endl;
				}	
			}
			cout<<"FIM DA ITERACAO"<<endl;
			if(it == root->filhos.end())
					cout<<"IT APONTA PRO FIM"<<endl;
		}
		cout<<"PASSOU LOOP2"<<endl;
		it = root->filhos.begin();
		list<q_node*>::reverse_iterator it2 = root->filhos.rbegin();																//define cor do nodo
		if((*it)->cor == (*it2)->cor)									//se forem iguais, so pode ser branco (3) ou preto (0)
		{
				root->cor = (*it)->cor;
		}
		else															//se forem diferentes, pode ser mistoP(1), mistoB(2) ou branco(3)
		{
					if((*it)->cor == 2 && (*it2)->cor == 1)
						root->cor = 3;
					else
						if((*it)->cor == 3 && (*it2)->cor == 1)
							root->cor = 3;
						else
							root->cor = 2;
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

void inverte(list<q_node*> &filhos)
{
	list<q_node*>::iterator it = filhos.begin();
	while(it != filhos.end())
	{
		(*it)->filhos.reverse();
		if(!(*it)->filhos.empty())
			inverte((*it)->filhos);
		it++;
	}
	return;
}