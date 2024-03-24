/*
 * Trabalho de Projeto e Análise de Algoritmo
 * Período 16.1
 * 
 * Desenvolver Metaheurísticas para o Problema de Alocação Generalizada
 * 
 * Algoritmo: GRASP.
 * Data: 01/08/2016.
 * Distribuição Livre, desde que referenciando o autor.
 * 
 * Professor: Haroldo Santos
 * 
 * Autor do Trabalho: Rodolfo Labiapari Mansur Guimarães
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>




/*
 * Estrutura de dados para armazenamento das informações de conjunto de 
 *	soluções.
 * 
 * excesso - valor com a capacidade atual de cada agente desta solução
 * tarefas - Vetor com tamanho Tarefas onde cada endereço é indicado o agente
 *	responsável por tal
 * avaliação - Valor fitness da solução (sum_excesso + custo) * penalidade
 * custo - custo total desta solução
 */
typedef struct Struct_Solucao {
	int * excesso;
	int * tarefas;
	double avaliacao;
	double custo;
} Solucao;

int QUANT_TAREFAS   = 0; // Quantidade de tarefas
int QUANT_AGENTES   = 0; // Quantidade de agentes
int * CAPAC_AGENTES = 0; // Vetor com capacidade máxima de cada gente.
int ** RECURSOS_A_T = 0; // Matriz de recursos[a,t]
int ** CUSTO_A_T    = 0; // Matriz de custo [a,t]

char IMPRIMIR = 0; // Variável que permite impressão na tela.
int SECONDS   = 0; // Tempo lido pelo arquivo de configuração
int MAXIteracoes = 0; // Quantidade de iterações daquela temperatura
FILE * out    = 0;
time_t endwait, start; // Variáveis de tempo para cálculo do intervalo de 
	//tempo de execução



void Le_Instancia(char * path) ;
void Le_Parametros(char *) ;
void Imprime_Solucao(Solucao * ind) ;
void Desaloca_Solucao(Solucao ** p) ;


void Imprime_Status(int tempo, int it, Solucao * mg, Solucao * a, Solucao * v) {
	printf("T:%10d\t", tempo);
	printf("I:%10d\t", it);
	printf("Best(c,a):%9.0lf|%9.0lf\t", mg->custo, mg->avaliacao);
	
	printf("AVA(c,a):At:%9.0lf|%9.0lf\tProx:%9.0lf|%9.0lf\n", a->custo, a->avaliacao, v->custo, v->avaliacao);
}

/*
 * Procedimento que avalia a solução atual.
 * 
 * Se a solução é factível, retorna seu custo real.
 * Se infactível retorna o custo encontrado * 100000
 */
double Avalia_Solucao(Solucao * sol) {
	int i = 0, capacidade_agentes[QUANT_AGENTES], sum_recursos = 0;
	double custo = 0; char solucao_invalida = 0;
	
	// Define a capacidade inicial utilizada de cada agente com 0
	for (i = 0; i < QUANT_AGENTES; i++)
		capacidade_agentes[i] = 0;

	// Realiza os cálculos de custo e capacidade
	for (i = 0; i < QUANT_TAREFAS; i++) {
		custo += CUSTO_A_T[sol->tarefas[i]][i];
		capacidade_agentes[sol->tarefas[i]] += RECURSOS_A_T[sol->tarefas[i]][i];
	}

	// Verifica se algum agente passou sua capacidade máxima
	for (i = 0; i < QUANT_AGENTES; i++) {
		sol->excesso[i] = capacidade_agentes[i];
		sum_recursos += capacidade_agentes[i];
	
		// Se sim define esta solução como inválida
		if (capacidade_agentes[i] > CAPAC_AGENTES[i])
			solucao_invalida = 1;
	}
	
	sol->custo = custo;
	
	// Caso a solução foi excedida, altera a avaliação do indivíduo tornando-o
		// pior.
	if (solucao_invalida) 
		sol->avaliacao = ((double) sum_recursos ) * 100000;
	else {
		sol->avaliacao = ((double) sum_recursos + custo);
	}
	
	return sol->avaliacao;
}



/*
 * Procedimento de geração de indivíduos por meio do procedimento shift. 
 * 
 * Um detalhe a se atentar é que é realizado shift em somente 1 tarefa da 
 *	solução.
 */
void Gera_Vizinho(Solucao * atual, Solucao ** proxima) {
	int i, agente_atual, tarefa_escolhida1, quant_alteracoes;
	
	for (i = 0; i < QUANT_TAREFAS; i++) {
		(*proxima)->tarefas[i] = atual->tarefas[i];
	}
	quant_alteracoes = 2 ;
	
	for (i = 0; i < quant_alteracoes; i++) {

		tarefa_escolhida1 = random() % QUANT_TAREFAS;

		agente_atual = (*proxima)->tarefas[tarefa_escolhida1]; 

		do {
			(*proxima)->tarefas[tarefa_escolhida1] = random() % QUANT_AGENTES;
		} while ((*proxima)->tarefas[tarefa_escolhida1] == agente_atual);
	}
	
	Avalia_Solucao(*proxima);
}



/*
 * Procedimento que instancia uma nova solução com seus valores totalmente 
 *	aleatórios.
 */
Solucao * Instancia_Solucao() {
	Solucao * s;
	
	s = calloc(1, sizeof(Solucao));
	
	s->tarefas = calloc(QUANT_TAREFAS, sizeof(int));
	s->excesso = calloc(QUANT_AGENTES, sizeof(int));
	
	// Defini-o como uma solução inicial ruim
	s->avaliacao = INT_MAX;
	s->avaliacao = INT_MAX;
	
	// Retorna a solução
	return s;
}


/*
 * Procedimento que instancia uma nova solução com seus valores totalmente 
 *	aleatórios.
 */
void Gera_Solucao_Aleatoria(Solucao ** s) {
	int i;
	
	for (i = 0; i < QUANT_TAREFAS; i++) {
		(*s)->tarefas[i] = random() % QUANT_AGENTES;
	}
	
	Avalia_Solucao(*s);
}


/*
 * Procedimento que copia as informações de uma solução para a outra de forma
 *	a aceitar aquela solução.
 */
char Teste_Aceita_Nova_Solucao(Solucao ** atual, Solucao * proxima) {
	int i;
	
	// Verifica se a solução atual é melhor que a atual.
	if ((*atual)->avaliacao > 2 * 100000 || 
				((proxima->avaliacao < 2 * 100000) && 
				(proxima->custo < (*atual)->custo))) {
	
		// Copia a solução
		for (i = 0; i < QUANT_TAREFAS; i++) {
			(*atual)->tarefas[i] = proxima->tarefas[i];
			if (i < QUANT_AGENTES)
				(*atual)->excesso[i] = proxima->excesso[i];
		}

		(*atual)->avaliacao = proxima->avaliacao;
		(*atual)->custo     = proxima->custo;
	
		// Retorna true se tiver aceitado.		
		return 1;
	} else {
		return 0;
	}
}


/*
 * Procedimento que copia as informações de uma solução para a outra de forma
 *	a aceitar aquela solução.
 */
void Aceita_Nova_Solucao(Solucao ** atual, Solucao * proxima) {
	int i = 0;
	
	// Copia a solução 
	for (i = 0; i < QUANT_TAREFAS; i++) {
		(*atual)->tarefas[i] = proxima->tarefas[i];
		if (i < QUANT_AGENTES)
			(*atual)->excesso[i] = proxima->excesso[i];
	}

	(*atual)->avaliacao = proxima->avaliacao;
	(*atual)->custo     = proxima->custo;
}


/*
 * Método Guloso Randômico
 */
void GreedyRandomizedConstruction(Solucao ** s, float alfa) {
	int i = 0, j = 0, min = 0, max = 0, fator = 0;
	
	// Para cada tarefa
	for (i = 0; i < QUANT_TAREFAS; i++) {
		min = max = 0;

		// Encontra os valores máximos e mínimos dos agentes
		for (j = 1; j < QUANT_AGENTES; j++) {
			if (RECURSOS_A_T[j][i] + CUSTO_A_T[j][i] < RECURSOS_A_T[min][i] + CUSTO_A_T[min][i])
				min = j;
			
			if (RECURSOS_A_T[j][i] + CUSTO_A_T[j][i] > RECURSOS_A_T[max][i] + CUSTO_A_T[max][i])
				max = j;
		}
		
		// Calcula um fator de acordo com o valor alfa
		fator = RECURSOS_A_T[min][i] + alfa * (RECURSOS_A_T[max][i] - RECURSOS_A_T[min][i]);

		// procura o agente que tem maior proximidade com o fator
		min = 0;
		for (j = 1; j < QUANT_AGENTES; j++) {
			if (abs(RECURSOS_A_T[j][i] - fator) < abs(RECURSOS_A_T[min][i] - fator))
				min = j;
		}
		
		// Atribui a esta tarefa
		(*s)->tarefas[i] = min;
	}
	
	// Avalia a solução gerada
	Avalia_Solucao(*s);
}
	

/*
 * Método GRASP
 */
Solucao * GRASP() {
	int i = 0;
	Solucao * melhor_global = 0, * atual_s = 0, * vizinha_s = 0;
	
	// Instancia soluções
	melhor_global  = Instancia_Solucao();
	atual_s        = Instancia_Solucao();
	vizinha_s      = Instancia_Solucao();
	
	// Inicia o contador de tempo
	start   = time(NULL);
    endwait = start + SECONDS;
	
	do {
		// Gera um novo indivíduo
		GreedyRandomizedConstruction(&atual_s, 0.5);
		
		// Verifica sua avaliação
		Teste_Aceita_Nova_Solucao(&melhor_global, atual_s);
		
		// Gera N vizinhos e verifica suas avaliações
		for (i = 0; i < MAXIteracoes; i++) {
			Gera_Vizinho(atual_s, &vizinha_s);
			
			if (vizinha_s->avaliacao < atual_s->avaliacao)
				Aceita_Nova_Solucao(&atual_s, vizinha_s);
		}
		
		// Testa se é a melhor solução encontrada
		Teste_Aceita_Nova_Solucao(&melhor_global, atual_s);	
		
		// Verifica se ainda possui tempo
		start = time(NULL);
	} while (start < endwait);
	
	Desaloca_Solucao(&atual_s);
	Desaloca_Solucao(&vizinha_s);

	return melhor_global;
}




/*
 * 
 */
int main(int argc, char** argv) {
	Solucao * solve;

	printf("\n/*");
	printf("\n * Trabalho de Projeto e Análise de Algoritmo");
 	printf("\n * Período 16.1");
 	printf("\n * ");
 	printf("\n * Desenvolver Metaheurísticas para o Problema de Alocação Generalizada");
 	printf("\n * ");
 	printf("\n * Algoritmo: GRASP.");
 	printf("\n * Data: 01/08/2016.");
 	printf("\n * Distribuição Livre, desde que referenciando o autor.");
 	printf("\n * ");
 	printf("\n * Professor: Haroldo Santos");
 	printf("\n * ");
 	printf("\n * Autor do Trabalho: Rodolfo Labiapari Mansur Guimarães");
 	printf("\n */");
	
	if (argc != 4) {
		printf("\n\nErro nos parâmetros! Quantidade lida: %d\t Quantidade requerida: %d.", argc, 4);
		printf("\nnome_programa arq_configuracao arq_instancia seed\n\n");
		exit(-1);
	}
 	printf("\n\nExecutando...\n");
	
	char * instancia = argv[2];
	char * configuracao = argv[1];
	srand(atoi(argv[3]));
	
	Le_Instancia(instancia);
	
	Le_Parametros(configuracao);

	out = fopen ("grasp.txt", "a");
	
	solve = GRASP();

	Imprime_Solucao(solve);
	Desaloca_Solucao(&solve);
	fclose(out);

	return (EXIT_SUCCESS);
}





/*
 * Procedimento que realiza a leitura da instância problema.
 */
void Le_Instancia(char * path) {
	int i, j;
	FILE * f = fopen(path, "r");

	if (f) {
		fscanf(f, "%d", &QUANT_AGENTES);
		fscanf(f, "%d", &QUANT_TAREFAS);

		CAPAC_AGENTES = calloc(QUANT_AGENTES, sizeof (int));

		RECURSOS_A_T = calloc(QUANT_AGENTES, sizeof (int*));
		CUSTO_A_T = calloc(QUANT_AGENTES, sizeof (int*));

		for (i = 0; i < QUANT_AGENTES; i++) {
			RECURSOS_A_T[i] = calloc(QUANT_TAREFAS, sizeof (int));
			CUSTO_A_T[i] = calloc(QUANT_TAREFAS, sizeof (int));
		}

		for (i = 0; i < QUANT_AGENTES; i++) {
			for (j = 0; j < QUANT_TAREFAS; j++) {
				fscanf(f, "%d", &CUSTO_A_T[i][j]);
			}
		}

		for (i = 0; i < QUANT_AGENTES; i++) {
			for (j = 0; j < QUANT_TAREFAS; j++) {
				fscanf(f, "%d", &RECURSOS_A_T[i][j]);
			}
		}

		for (i = 0; i < QUANT_AGENTES; i++)
			fscanf(f, "%d", &CAPAC_AGENTES[i]);
		
		fclose(f);

	} else {
		printf("Erro ao ler Instância!\n");
		exit(-2);
	}

}



/*
 * Procedimento que realiza a leitura dos parâmetros de configuração do 
 *	algoritmo.
 */
void Le_Parametros(char * config) {
	FILE * f;

	f = fopen(config, "r");

	if (f) {
		fscanf(f, "%d", &SECONDS);
		fscanf(f, "%d", &MAXIteracoes);
		
		fclose(f);
		
	} else {
		printf("Erro ao ler Configuração!\n");
		exit(-1);
	}
}




/*
 * Procedimento que informa o resultado obtido.
 */
void Imprime_Solucao(Solucao * ind) {
	int i = 0;
	
	fprintf(out, "\n%10lf\t", ind->custo);
	printf("\n%10lf\t", ind->custo);	
	
	for (i = 0; i < QUANT_TAREFAS; i++) {
		fprintf(out, "%3d ", ind->tarefas[i]);
		printf("%3d ", ind->tarefas[i]);
	}
}



/*
 * Procedimento que libera a memória da solução utilizada.
 */
void Desaloca_Solucao(Solucao ** p) {
	
	free((*p)->tarefas);
	free((*p)->excesso);
	
	free(*p);
}