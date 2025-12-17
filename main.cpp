#include <queue>
#include <fstream>
#include "arv_bi_n.cpp"
#define INT_MAX 2147483647
using namespace std;

typedef struct transistor{
	char tipo;
	char al;																						// operação relacionada (* ou +)
	int	num;
	int pos = 0;
	int ordem = 0;																					//se foi colocalo na ordem 1 ou 0 (inicia no drain ou source)
	int volta = 0;
	string drain;
	char gate;
	string source;
	transistor(char tipo_, int num_, string drain_, char gate_, string source_,char al_)
	{
		tipo = tipo_;
		num = num_;
		drain = drain_;
		gate = gate_;
		source = source_;
		al = al_;
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
int trans_pos = 1;
void quebra_portas(string eq);																	//chama faz_postfix e monta_arvore					

//netlist
void print_trans(list<transistor*> trans_list);
int faz_netlist_ordenado(list<transistor*> &trans_list, q_node*& root, int origem, bool ordem);							//faz netlist usando arvoe n-aria ordenada
int faz_netlist_ordenado_p(list<transistor*> &trans_list, q_node*& root, int origem, bool ordem);
void subs (string substituir, string substituto, list<transistor*> &trans_list);												//substitui nets
string saida_serie(list<transistor*> &trans_list, q_node*& root);																	//procura saida em raiz com operacao serie
string saida_paralelo(list<transistor*> &trans_list, q_node*& root);																//procura saida em raiz com operacao paralelo
string saida_serie_p(list<transistor*> &trans_list, q_node*& root);																	//procura saida em raiz com operacao serie
string saida_paralelo_p(list<transistor*> &trans_list, q_node*& root);																//procura saida em raiz com operacao paralelo
void escreve(list<transistor*> trans_list, string eq);	//concerta a saida (remove net a mais criado quando expressão mas externa e +) e escreve em .spice

void clean_stack(stack<int> &stack);
int left_edge_true(list<transistor*> trans_list, queue<net> &nets); //Calcula comprimento de todas as nets, faz left edge

node raiz;
q_node* q_raiz;




int main(int argc, char *argv[])					// TEM QUE ESTAR NO FORMATO (a*(b+c*(d+e))), SEM INVERSORES
{
	if(argc != 2)
	{
		cout<<"Use a equação boolenana como argumento"<<endl;
		exit(1);
	}			
	list<transistor*> trans_list_n;							//transistores N
	list<transistor*> trans_list_p;							//transistores P
	queue<net> nets_n;
	string eq = argv[1];
	cout <<"A equação é: "<< eq <<endl;
	ofstream file;
	file.open("Netlists.spice", std::ios::app);
	file<<"A equação é: " << "!"+ eq<<endl;
	file.close();
	file.open("./netlists/!"+eq+".cdl");
	file<<"*!"<<eq<<endl;
	file.close();

	//faz binaria
	if(eq.at(0) != '(')
	{
		if(eq.at(0) == 'a')
		{
			cout<<"INPUT a"<<endl;
			file.open("saida.txt", std::ios::app);
			file<<"!"+eq<<" "<<"0"<<" "<<"1"<<endl;
			file.close();
			exit(0);
		}
		else
		{
			exit(EXIT_FAILURE);
		}
	}
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
	int ordem = 1;																				//Ordem 1 é subindo
	faz_netlist_ordenado(trans_list_n, q_raiz, 0, ordem);
	print_trans(trans_list_n);
	
	trans_pos= 1;

	cout<<endl<<"Netlist Pullup:"<<endl;
	ordem = 0;																					//Ordem 0 é descendo
	faz_netlist_ordenado_p(trans_list_p, q_raiz, 0, ordem);
	print_trans(trans_list_p);

	//DECIDE SAIDAS
	string saida_n;
	string saida_p;
	if(q_raiz->tipo == '*')
	{
		saida_n = saida_serie(trans_list_n, q_raiz);
		saida_p = saida_paralelo_p(trans_list_p, q_raiz);
	}
	else
	{
		saida_n = saida_paralelo(trans_list_n, q_raiz);
		saida_p = saida_serie_p(trans_list_p, q_raiz);
	}
	
	cout<<"SAIDA N EM "<<saida_n<<endl;
	cout<<"SAIDA P EM "<<saida_p<<endl;
	//escreve(trans_list_p,eq);																									//TESTE
	subs(saida_p,saida_n, trans_list_p);
	trans_list_n.splice(trans_list_n.end(),trans_list_p);
	file.open("Nets.txt", std::ios::app);
	file<<"!"+eq<<endl;
	int linhas = left_edge_true(trans_list_n, nets_n);
	subs(saida_n,"Z",trans_list_n);
	escreve(trans_list_n,eq);
	if (!file.is_open()) {
    std::cerr << "Failed to open file." << std::endl;
	}
	while(!nets_n.empty())
	{
		if(nets_n.front().nome == saida_n)
			nets_n.front().nome = "Z";

		file<<nets_n.front().nome<<" "<<nets_n.front().inicio<<" "<<nets_n.front().fim<<" "<<"Linha "<<nets_n.front().linha<<endl;
		nets_n.pop();
	}
	file.close();
	int num_gaps = 0;
	conta_gaps(q_raiz, &num_gaps, 2);
	file.open("saida.txt", std::ios::app);
	file<<"!"+eq<<" "<<num_gaps<<" "<<linhas<<endl;
	file.close();
	list<transistor*>::iterator it = trans_list_n.begin();
	cout<<"LISTA FINAL:"<<endl;
	print_trans(trans_list_n);

	return 0;
}

void quebra_portas(string eq)		
{									
	stack<char> postfix;									//onde ficara a versao postfix da expressao
	faz_postfix(postfix, eq);
	monta_arv(&raiz, postfix);
	
	return;
}

void escreve(list<transistor*> trans_list, string eq)
{
	ofstream file;
	ofstream f;
	ofstream f_sep;
	file.open("Netlists.spice", std::ios::app);
	f_sep.open("./netlists/!"+eq+".cdl", std::ios::app);
	f.open("netlists_test.cdl", std::ios::app);
	list<transistor*>::iterator it = trans_list.begin();
	list<char> in_out;
	int trans_num = 0;
	while(it != trans_list.end())
	{
		//if((*it)->gate != 'Z')
		//{
			if((*it)->tipo == 'n')
			{
				file<<"M"<<(*it)->num<<" "<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<" " <<"VSS "<<(*it)->tipo<<"fet"<<endl;
				in_out.push_front((*it)->gate);
				trans_num++;
			}
			if((*it)->tipo == 'p')
			{
				file<<"M"<<(*it)->num<<" "<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<" " <<"VDD "<<(*it)->tipo<<"fet"<<endl;
				trans_num++;
			}
		//}
		it++;
	}
	it = trans_list.begin();
	in_out.push_back('Z');
	list<char>::iterator it2 = in_out.begin();
	f<<".SUBCKT !"<<eq<<" ";
	f_sep<<".SUBCKT !"<<eq<<" ";
	while(it2 != in_out.end())
	{
		f<<*it2<<" ";
		f_sep<<*it2<<" ";
		it2++;
	}
	f<<"VDD VSS"<<endl;
	f_sep<<"VDD VSS"<<endl;
	while(it != trans_list.end())
	{
		//if((*it)->gate != 'Z')
		//{
			if((*it)->tipo == 'n')
			{
				f<<"M"<<(*it)->num<<" "<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<" VSS" <<" "<<"NMOS_RVT "<<"L=6n NFIN=3 M=1"<<endl;
				f_sep<<"M"<<(*it)->num<<" "<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<" VSS" <<" "<<"NMOS_RVT "<<"L=6n NFIN=3 M=1"<<endl;
			}
			if((*it)->tipo == 'p')
			{
				f<<"M"<<(*it)->num<<" "<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<" VDD" <<" "<<"PMOS_RVT "<<"L=6n NFIN=3 M=1"<<endl;
				f_sep<<"M"<<(*it)->num<<" "<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<" VDD" <<" "<<"PMOS_RVT "<<"L=6n NFIN=3 M=1"<<endl;
			}
		//}
		it++;
	}
	f<<"* PLACE_BEGIN !"<<eq<<endl;
	it = trans_list.begin();
	int p_tran_n = 2;
	int p_tran_p = 2;
	while(it != trans_list.end())
	{
		if((*it)->gate != 'Z')
		{
			if((*it)->tipo == 'n')
			{
				f<<"* M"<<(*it)->num<<" "<<(*it)->pos+1<< " 0"<<endl;
			}
			if((*it)->tipo == 'p')
			{
				f<<"* M"<<(*it)->num<<" "<<(*it)->pos+1<< " 1"<<endl;
			}
		}
		it++;
	}
	f<<"* PLACE_END !"<<eq<<endl;
	file.close();
	f<<".ENDS"<<endl;
	f_sep<<".ENDS"<<endl;
	f.close();
	f_sep.close();
	return;
}

void print_trans(list<transistor*> trans_list)
{
	for(transistor* it : trans_list)
	{
		cout<<"M"<<it->num<<" "<<it->drain<<" "<<it->gate<<" "<<it->source<<" pos:"<<it->pos<<endl;
	}
	return;
}

int faz_netlist_ordenado(list<transistor*> &trans_list, q_node*& root, int origem, bool ordem)
{
	//Define se filho e série ou paralelo

	//Paralelo - cria origem e destino
	//Se filho for OP, passa origem, ao retornar substitui destino, libera net_num, troca ordem
	//Se for folha, Cria transistor se não for pseudo, troca ordem

	//Série - usa origem
	//Se filho for OP, passa origem, ao voltar, atualiza origem
	//Se filho for folha, cria transistor se não for pseudo, atualiza origem

	if(!root)
		return INT_MAX;

	if(root->tipo == '+')
	{
		int dest;
		dest = net_number;
		net_number++;

		for(q_node* filho : root->filhos)
		{
			if(filho->tipo == '*')
			{
				//filho retorna net onde terminou
				int dest_filho = faz_netlist_ordenado(trans_list, filho, origem, ordem);
				if(dest == 0)
				{
					subs( "n"+to_string(dest_filho), "VSS", trans_list);
				}
				else
				{
					subs( "n"+to_string(dest_filho), "n"+to_string(dest), trans_list);
				}
				if(dest == net_number-1)
					net_number--;
			}
			else
			{
				if(filho->tipo != 'Z')
				{
					//cria transistor
					transistor *temp;				
					if(origem == 0)
					{
						temp = new transistor('n', trans_number, "VSS", filho->tipo, "n"+to_string(dest), filho->al);
						temp->ordem = ordem;
						temp->pos = trans_pos;
						trans_pos = trans_pos+2;
					}
					else
					{
						if(dest == 0)
						{
							temp = new transistor('n', trans_number, "n"+to_string(origem), filho->tipo, "VSS", filho->al);
							temp->ordem = ordem;
							temp->pos = trans_pos;
							trans_pos = trans_pos+2;
						}
						else
						{
							temp = new transistor('n', trans_number, "n"+to_string(origem), filho->tipo, "n"+to_string(dest), filho->al);
							temp->ordem = ordem;
							temp->pos = trans_pos;
							trans_pos = trans_pos+2;
						}
					}
					trans_number++;
					trans_list.push_back(temp);
				}
				else
				{
					//se for paralelo, não for o primeiro, e ja nao existir um gap, coloca gap
					if(trans_pos != 1 && trans_pos-1 != trans_list.back()->pos)
						trans_pos++;
				}
			}
			//troca ordem/troca origem e destino
			ordem = !ordem;
			int tmp = origem;
			origem = dest;
			dest = tmp;
		}
	}
	else
		if(root->tipo == '*')
		{
			for(q_node* filho : root->filhos)
			{
				if(filho->tipo == '+')
				{
					//filho retorna destino
					int dest_filho = faz_netlist_ordenado(trans_list, filho, origem, ordem);
					
					origem = dest_filho;
				}
				else
				{
					if(filho->tipo != 'Z')
					{
						transistor *temp;
						if(origem == 0)
						{
							temp = new transistor('n', trans_number, "VSS", filho->tipo, "n"+to_string(net_number), filho->al);
							temp->pos = trans_pos;
							trans_pos = trans_pos+2;
						}
						else
						{
							temp = new transistor('n', trans_number, "n"+to_string(origem), filho->tipo, "n"+to_string(net_number), filho->al);
							temp->pos = trans_pos;
							trans_pos = trans_pos+2;
						}
						origem = net_number;
						net_number++;
						trans_number++;
						trans_list.push_back(temp);
					}
				}
			}
		}
	//retorna net onde terminou
	return origem;
}

int faz_netlist_ordenado_p(list<transistor*> &trans_list, q_node*& root, int origem, bool ordem)
{
	//Define se filho e série ou paralelo

	//Paralelo - cria origem e destino
	//Se filho for OP, passa origem, ao retornar substitui destino, libera net_num, troca ordem
	//Se for folha, Cria transistor se não for pseudo, troca ordem

	//Série - usa origem
	//Se filho for OP, passa origem, ao voltar, atualiza origem
	//Se filho for folha, cria transistor se não for pseudo, atualiza origem

	if(!root)
		return INT_MAX;

	if(root->tipo == '*')
	{
		int dest;
		dest = net_number;
		net_number++;

		for(q_node* filho : root->filhos)
		{
			if(filho->tipo == '+')
			{
				//filho retorna net onde terminou
				int dest_filho = faz_netlist_ordenado_p(trans_list, filho, origem, ordem);
				if(dest == 0)
				{
					subs( "n"+to_string(dest_filho), "VDD", trans_list);
				}
				else
				{
					subs( "n"+to_string(dest_filho), "n"+to_string(dest), trans_list);
				}
				net_number--;
			}
			else
			{
				if(filho->tipo != 'Z')
				{
					//cria transistor
					transistor *temp;				
					if(origem == 0)
					{
						temp = new transistor('p', trans_number, "VDD", filho->tipo, "n"+to_string(dest), filho->al);
						temp->ordem = ordem;
						temp->pos = trans_pos;
						trans_pos = trans_pos+2;
					}
					else
					{
						if(dest == 0)
						{
							temp = new transistor('p', trans_number, "n"+to_string(origem), filho->tipo, "VDD", filho->al);
							temp->ordem = ordem;
							temp->pos = trans_pos;
							trans_pos = trans_pos+2;
						}
						else
						{
							temp = new transistor('p', trans_number, "n"+to_string(origem), filho->tipo, "n"+to_string(dest), filho->al);
							temp->ordem = ordem;
							temp->pos = trans_pos;
							trans_pos = trans_pos+2;
						}
					}
					trans_number++;
					trans_list.push_back(temp);
				}
				else
				{
					//se for paralelo, não for o primeiro, e ja nao existir um gap, coloca gap
					if(trans_pos != 1 && trans_pos-1 != trans_list.back()->pos)
						trans_pos++;
				}
			}
			//troca ordem/troca origem e destino
			ordem = !ordem;
			int tmp = origem;
			origem = dest;
			dest = tmp;
		}
	}
	else
		if(root->tipo == '+')
		{
			for(q_node* filho : root->filhos)
			{
				if(filho->tipo == '*')
				{
					//filho retorna destino
					int dest_filho = faz_netlist_ordenado_p(trans_list, filho, origem, ordem);
					
					origem = dest_filho;
				}
				else
				{
					if(filho->tipo != 'Z')
					{
						transistor *temp;
						if(origem == 0)
						{
							temp = new transistor('p', trans_number, "VDD", filho->tipo, "n"+to_string(net_number), filho->al);
							temp->pos = trans_pos;
							trans_pos = trans_pos+2;
						}
						else
						{
							temp = new transistor('p', trans_number, "n"+to_string(origem), filho->tipo, "n"+to_string(net_number), filho->al);
							temp->pos = trans_pos;
							trans_pos = trans_pos+2;
						}
						origem = net_number;
						net_number++;
						trans_number++;
						trans_list.push_back(temp);
					}
				}
			}
		}
	return origem;
}

void subs (string substituir, string substituto, list<transistor*> &trans_list)
{
	list<transistor*>::iterator it = trans_list.begin();
	while(it != trans_list.end())
	{
		if((*it)->drain == substituir)
		{
			//cout<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<" ANTES DA SUB"<<endl;
			(*it)->drain = substituto;
			//cout<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<" DEPOIS DA SUB"<<endl;
		}
			
		
		if((*it)->source == substituir)
		{
			//cout<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<" ANTES DA SUB"<<endl;
			(*it)->source = substituto;
			//cout<<(*it)->drain<<" "<<(*it)->gate<<" "<<(*it)->source<<" DEPOIS DA SUB"<<endl;
		}

		
		it++;
	}
	return;
}

string saida_serie(list<transistor*> &trans_list, q_node*& root)
{
	cout<<"ENTROU EM SAIDA SERIE"<<endl;
	list<q_node*>::reverse_iterator it = root->filhos.rbegin();

	if((*it)->tipo == 'Z')														//Se o ultimo filho for pseudo, ignora
		it++;

	if((*it)->tipo != '+')																//Se o ultimo filho for folha, retorna net source
	{
		list<transistor*>::reverse_iterator tran = trans_list.rbegin();
		return (*tran)->source;
	}
	else
	{	
		return saida_paralelo(trans_list, *it);											//se for OP, usa ultimo filho como raiz pra saida_paralelo
	}
	
}

string saida_serie_p(list<transistor*> &trans_list, q_node*& root)
{
	cout<<"ENTROU EM SAIDA SERIE"<<endl;
	list<q_node*>::reverse_iterator it = root->filhos.rbegin();

	if((*it)->tipo == 'Z')														//Se o ultimo filho for pseudo, ignora
		it++;

	if((*it)->tipo != '*')																//Se o ultimo filho for folha, retorna net source
	{
		list<transistor*>::reverse_iterator tran = trans_list.rbegin();
		return (*tran)->source;
	}
	else
	{	
		return saida_paralelo_p(trans_list, *it);											//se for OP, usa ultimo filho como raiz pra saida_paralelo
	}
	
}

string saida_paralelo(list<transistor*> &trans_list, q_node*& root)
{
	bool ordem = 1;
	for(q_node* filho : root->filhos)
	{
		if(filho->tipo != '*' && filho->tipo != 'Z')													//se algum filho for folha real, retorna net de acordo com ordem
		{
			list<transistor*>::iterator tran = trans_list.begin();

			while((*tran)->gate != filho->tipo)															//encontra transistor na lista
			{
				tran++;
			}

			if(ordem == 1)																				//retorna net
			{
				return (*tran)->source;
			}
			else
			{
				return (*tran)->drain;
			}

		
		}
		ordem = !ordem;																					//guarda ordem do filho
	}

	ordem = 1;
	string saida;
	for(q_node* filho : root->filhos)
	{
		if(filho->tipo == '*' && ordem == 1)
		{
			saida = saida_serie(trans_list, filho);
		}
		ordem = !ordem;	
	}
	return saida;
}

string saida_paralelo_p(list<transistor*> &trans_list, q_node*& root)
{
	bool ordem = 0;
	for(q_node* filho : root->filhos)
	{
		if(filho->tipo != '+' && filho->tipo != 'Z')													//se algum filho for folha real, retorna net de acordo com ordem
		{
			list<transistor*>::iterator tran = trans_list.begin();

			while((*tran)->gate != filho->tipo)															//encontra transistor na lista
			{
				tran++;
			}

			if(ordem == 1)																				//retorna net
			{
				return (*tran)->drain;
			}
			else
			{
				return (*tran)->source;
			}

		
		}
		ordem = !ordem;																					//guarda ordem do filho
	}

	ordem = 0;
	string saida;
	for(q_node* filho : root->filhos)
	{
		if(filho->tipo == '+' && ordem == 0)
		{
			saida = saida_serie_p(trans_list, filho);
		}
		ordem = !ordem;	
	}
	return saida;
}


int intersecta(net net1, net net2) // Returns 1 if there is an intersection between nets
{
    // Check if the intervals overlap
    if (net1.inicio <= net2.fim && net1.fim >= net2.inicio)
        return 1;
    else
        return 0;
}

int left_edge_true(list<transistor*> trans_list, queue<net> &nets)
{
	int livre = 1;
	int linha = 1;
	net *temp;
	list<transistor*>::iterator it;
	list<net> nets_;										//nets antes de serem distribuidas pelas linhas							
	for(int i = 0; i < net_number; i++)
	{
		int min = INT_MAX;
		int max = 1;
		it = trans_list.begin();
		
		while(it != trans_list.end())
		{
			if((*it)->gate != 'Z')
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
			}
			it++;
		}
		
		if(min != INT_MAX && max != 1 && (max - min > 1))										// ignora min e max defaults e nets de distancia 1(drain e source podem dividir difusao)
		{
			if(livre <= min)
			{
				temp = new net("n"+to_string(i), min, max, 0);
				nets_.push_front(*temp);
				//livre = max+1;
			}
			else
			{
				temp = new net("n"+to_string(i), min, max, 0);
				nets_.push_front(*temp);
				//livre = max+1;
			}
		}
		
	}
	int watermark = INT_MAX;										//quebrar em uma função? isso precisa ser executado toda nova linha
	list<net>::iterator it_nets = nets_.begin();
	list<net>::iterator it_left;
	while (it_nets != nets_.end()) 						// procura o net que inicia mais proximo do watermark, sendo maior que ele
	{
		//cout<<"NET "<<it_nets->nome<<"com inicio em "<<it_nets->inicio<<endl;
		if (watermark > it_nets->inicio) 				
		{
			watermark = it_nets->inicio;
			it_left = it_nets;
		} 
		it_nets++;
	}
	it_left->linha = linha;
	watermark = it_left->fim;
	nets.push(*it_left);
	nets_.erase(it_left);

	while (!nets_.empty()) 
	{
		bool found = false;
		list<net>::iterator it_nets = nets_.begin();
		list<net>::iterator it_left;						// guarda o net mais a esquerda de cada iteração
		
		while (it_nets != nets_.end()) 						// procura o net que inicia mais proximo do watermark, sendo maior que ele
		{
			//cout<<"NET "<<it_nets->nome<<"com inicio em "<<it_nets->inicio<<endl;
			if (watermark < it_nets->inicio) 				
			{
				watermark = it_nets->inicio;
				it_left = it_nets;
				found = 1;
			} 
			it_nets++;
		}
			if(!found)											//se nao encontrou, cria nova linha e reseta watermark
		{
			linha++;
			watermark = INT_MAX;
			list<net>::iterator it_nets = nets_.begin();
			list<net>::iterator it_left;
			while (it_nets != nets_.end()) 						// procura o net que inicia mais proximo do watermark, sendo maior que ele
			{
				//cout<<"NET "<<it_nets->nome<<"com inicio em "<<it_nets->inicio<<endl;
				if (watermark > it_nets->inicio) 				
				{
					watermark = it_nets->inicio;
					it_left = it_nets;
				} 
				it_nets++;
			}
			it_left->linha = linha;
			watermark = it_left->fim;
			nets.push(*it_left);
			nets_.erase(it_left);
		}
		
		else												//se achou, coloca net na linha e remove da lista e adiciona na lista final
		{
			it_left->linha = linha;
			watermark = it_left->fim;
			nets.push(*it_left);
			nets_.erase(it_left);
			found = 0;
		}
		//cout<<linha<<endl;
	}
	return linha;
}