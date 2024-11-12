Implementar o método.

Entra com equação eqn. Sai netlist ordenado. Roteia na grade e diz quantas linhas de roteamento usou. Target: todas as células da 44-6.genlib.

Objetivos:
a) se tiver mais de um roteamento com numero de quebras equivalente (min x) ve se tem diferenca do custo de roteamento (min Y). Sem regras, grid tosco.
b) Classificar cada célula por número de trilhas mínimo necessário para roteamento.

Etapas
Recebe expressão por argumento, converte em postfix e guarda em uma árvore binaria
Aplica algoritmo
Faz placement
Faz roteamento
Classifica

TEM QUE ESTAR NO FORMATO (a*(b+c*(d+e))), SEM INVERSORES
