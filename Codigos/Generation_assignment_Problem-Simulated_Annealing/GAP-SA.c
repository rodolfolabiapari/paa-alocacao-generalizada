/*
 * Trabalho de Projeto e Análise de Algoritmo
 * Período 16.1
 * 
 * Desenvolver Metaheurísticas para o Problema de Alocação Generalizada
 * 
 * Algoritmo: Simulated Annealing.
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

int TEMPERATURA     = 0; // Temperatura inicial do processo
int MAXIteracoes    = 0; // Quantidade de iterações daquela temperatura
int static iteracoes_static = 1;
char IMPRIMIR       = 0; // Variável que permite impressão na tela.
FILE * out          = 0;



void Le_Instancia(char * path) ;
void Le_Parametros(char *) ;
void Imprime_Status(int i, double t, Solucao * m, Solucao * a, Solucao * p) ;
void Teste_Aceita_Nova_Solucao(Solucao ** atual, Solucao * proxima) ;
void Aceita_Nova_Solucao(Solucao ** atual, Solucao * proxima) ;
void Desaloca_Solucao(Solucao ** p) ;
Solucao * Instancia_Solucao_Aleatoria() ;
void Imprime_Solucao(Solucao * ind) ;




/*
 * Procedimento que realiza a alteração da temperatura por meio de método 
 *	logaritmo.
 */
void Atualiza_Temperatura(double * t) {
	*t = 0.99 * *t;
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
	int i = 0, j = 0, k = 0, agente_atual = 0, 
	tarefa_escolhida1 = 0, tarefa_escolhida2 = 0, 
	quant_alteracoes = 0, max = 0, min = 0;
	
	// Copia os valores do atual
	for (i = 0; i < QUANT_TAREFAS; i++) {
		(*proxima)->tarefas[i] = atual->tarefas[i];
	}
	
	// Define uma quantidade de alterações pra gerar o vizinho
	quant_alteracoes = 2 ;
	
	// Altera o indivíduo
	for (i = 0; i < quant_alteracoes; i++) {

		tarefa_escolhida1 = random() % QUANT_TAREFAS;
		agente_atual = (*proxima)->tarefas[tarefa_escolhida1]; 

		do {
			(*proxima)->tarefas[tarefa_escolhida1] = random() % QUANT_AGENTES;
		} while ((*proxima)->tarefas[tarefa_escolhida1] == agente_atual);
	}
	
	// Avalia a solução vizinha gerada
	Avalia_Solucao(*proxima);
}



/*
 * Procedimento de recozimento simulado, baseado no Lopes 2008.
 */
Solucao * Simulated_Annealing() {
	int iteracoes = 0;
	double fator_Boltzmann = 0, temperatura = 0, delta = 0, condicao_parada = 0, aceitacao_aleatoria = 0;
	Solucao * melhor_s = 0, * atual_s = 0, * possivel_s = 0;
	
	// Define valores iniciais
	temperatura     = TEMPERATURA;
	condicao_parada = 0.5;
	
	// Instancia soluções aleatórias para início de execução
	melhor_s   = Instancia_Solucao_Aleatoria();
	atual_s    = Instancia_Solucao_Aleatoria();
	possivel_s = Instancia_Solucao_Aleatoria();
	
	// verifica se alguma solução aleatória gerada é boa
	Teste_Aceita_Nova_Solucao(&melhor_s, atual_s);
	Teste_Aceita_Nova_Solucao(&melhor_s, possivel_s);
	
	// Enquanto tiver temperatura suficiente
	while (temperatura > condicao_parada) {
		
		// Aperfeiçoa a solução desta temperatura.
		while(iteracoes++ < MAXIteracoes) {
			// Gera um novo vizinho
			Gera_Vizinho(atual_s, &possivel_s);

			delta = possivel_s->avaliacao - atual_s->avaliacao;
			
			// verifica se a solução atual é válida
			if (delta < 0) {
				// Se sim aceita.
				Aceita_Nova_Solucao(&atual_s, possivel_s);
				
				// Verifica se é melhor que a melhor
				Teste_Aceita_Nova_Solucao(&melhor_s, possivel_s);
				
			} else {
				// calcula fator Boltzmann
				aceitacao_aleatoria = random() / ((double)(RAND_MAX));
				fator_Boltzmann = exp(- (delta / (double) temperatura));
				
				if (aceitacao_aleatoria < fator_Boltzmann) {
					Aceita_Nova_Solucao(&atual_s, possivel_s);
				} 
			}
		}
		
		// Atualiza temperatura
		Atualiza_Temperatura(&temperatura);
		iteracoes = 0;
	}

	// Desaloca soluções
	Desaloca_Solucao(&atual_s);
	Desaloca_Solucao(&possivel_s);
	
	return melhor_s;
}


/*
 * Procedimento principal
 */
int main(int argc, char** argv) {
	Solucao * solve;

	printf("\n/*");
	printf("\n * Trabalho de Projeto e Análise de Algoritmo");
 	printf("\n * Período 16.1");
 	printf("\n * ");
 	printf("\n * Desenvolver Metaheurísticas para o Problema de Alocação Generalizada");
 	printf("\n * ");
 	printf("\n * Algoritmo: Simulated Annealing.");
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
	
	out = fopen ("simulated_annealing.txt", "a");
	
	solve = Simulated_Annealing();

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
void Le_Parametros(char * conf) {
	FILE * f;

	f = fopen(conf, "r");

	if (f) {
		fscanf(f, "%d", &TEMPERATURA);
		fscanf(f, "%d", &MAXIteracoes);
		
		fclose(f);
		
	} else {
		printf("Erro ao ler Configuração!\n");
		exit(-1);
	}
}



/*
 * Procedimento de impressão do status atual da execução.
 */
void Imprime_Status(int it, double t, Solucao * m, Solucao * a, Solucao * p) {
	int i, sum = 0;
	
	for (i = 0; i < QUANT_AGENTES; i++)
		sum += m->excesso[i];
	
	printf("I:%7d\t", it);
	printf("T:%7.1lf\t", t);
	printf("Best(c,a,sum):%9.0lf|%9.0lf|%d\t", m->custo, m->avaliacao, sum);
	
	printf("AVA(c,a):At:%9.0lf|%9.0lf\tProx:%9.0lf|%9.0lf\n", a->custo, a->avaliacao, p->custo, p->avaliacao);
}



/*
 * Procedimento que copia as informações de uma solução para a outra de forma
 *	a aceitar aquela solução.
 */
void Teste_Aceita_Nova_Solucao(Solucao ** atual, Solucao * proxima) {
	int i = 0;
	
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
	}
}



/*
 * Procedimento que copia as informações de uma solução para a outra de forma
 *	a aceitar aquela solução.
 */
void Aceita_Nova_Solucao(Solucao ** atual, Solucao * proxima) {
	int i;
	
	for (i = 0; i < QUANT_TAREFAS; i++) {
		(*atual)->tarefas[i] = proxima->tarefas[i];
		if (i < QUANT_AGENTES)
			(*atual)->excesso[i] = proxima->excesso[i];
	}

	(*atual)->avaliacao = proxima->avaliacao;
	(*atual)->custo     = proxima->custo;
}



/*
 * Procedimento que libera a memória da solução utilizada.
 */
void Desaloca_Solucao(Solucao ** p) {
	
	free((*p)->tarefas);
	free((*p)->excesso);
	
	free(*p);
}



/*
 * Procedimento que instancia uma nova solução com seus valores totalmente 
 *	aleatórios.
 */
Solucao * Instancia_Solucao_Aleatoria() {
	int i;
	Solucao * s;
	
	s = calloc(1, sizeof(Solucao));
	
	s->tarefas = calloc(QUANT_TAREFAS, sizeof(int));
	s->excesso = calloc(QUANT_AGENTES, sizeof(int));
	
	for (i = 0; i < QUANT_TAREFAS; i++) {
		s->tarefas[i] = random() % QUANT_AGENTES;
	}
	
	Avalia_Solucao(s);
	
	return s;
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