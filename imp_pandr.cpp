#include <iostream>
#include <string>
#include <stack>
#include <queue>
using namespace std;


typedef struct node{

    string tipo;
    struct node* esquerda = nullptr;
    struct node* direita = nullptr;
	
}node;

void quebra_portas(string eq);
void faz_postfix(stack<char> &postfix, string eq);
void monta_arv(node *ptr, stack<char> &postfix);
void printLevelOrder(node *root);

node raiz;

int main(int argc, char *argv[])					// argumento 1 atraso INV, argumento 2 atraso AND
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
	//cout<< "antes "<<raiz.tipo<<endl;
	//cout<< "antes "<<raiz.*direita.tipo<<endl;
	faz_postfix(postfix, eq);
	monta_arv(&raiz, postfix);
	//cout<<"depois "<<raiz.tipo;
	//cout<< "depois "<<raiz.direita->tipo<<endl;
	printLevelOrder(&raiz);
	
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
		postfix.pop();
	}
	
	return;
}


void printLevelOrder(node *root) {
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
                cout << node->tipo << " ";  
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
    
    //TESTE
    //Teste 2
    //
    //
    //
    //
    //
	
