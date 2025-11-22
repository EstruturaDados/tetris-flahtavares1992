#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Definimos a capacidade máxima da fila de peças futuras (Array Circular)
#define MAX_FILA 5 

// Variável global para rastrear o ID único de cada peça gerada
int ID_GLOBAL = 0; 

// --- ESTRUTURAS DE DADOS ---

// 1. Struct do Elemento: A Peça
// Contém o tipo e o ID, que representam a ordem de criação da peça.
typedef struct {
    char nome;  // Caractere que representa o tipo da peça ('I', 'O', 'T', 'L', etc.)
    int id;     // Número inteiro único (-1 é usado para indicar falha/vazio)
} Peca;

// 2. Struct da Fila (Circular)
// Contém o vetor de peças e os ponteiros de controle.
typedef struct {
    Peca itens[MAX_FILA]; // Array que armazena os elementos
    int inicio;           // Índice de onde o elemento será retirado (dequeue)
    int fim;              // Índice de onde o novo elemento será inserido (enqueue)
    int total;            // Contador de elementos na fila
} Fila;


// --- PROTÓTIPOS DAS FUNÇÕES (MODULARIZAÇÃO) ---
void inicializarFila(Fila *f);
int filaCheia(Fila *f);
int filaVazia(Fila *f);

// Funções de Operação (Core FIFO)
void inserirPeca(Fila *f, Peca p);
Peca removerPeca(Fila *f);
void exibirFila(Fila *f);

// Funções Auxiliares
Peca gerarPeca();


// =================================================================
// MAIN FUNCTION - FLUXO DE EXECUÇÃO
// =================================================================
int main() {
    // Inicializa o gerador de números aleatórios para o tipo de peça
    srand(time(NULL)); 
    
    Fila fila_pecas;
    inicializarFila(&fila_pecas);
    
    int opcao;
    
    // Inicializa a fila preenchendo-a até o máximo (5 peças)
    while (fila_pecas.total < MAX_FILA) {
        inserirPeca(&fila_pecas, gerarPeca());
    }

    printf("--- Simulador de Fila de Peças do Tetris Stack (Nível Novato) ---\n");
    printf("Fila inicializada com %d peças.\n", MAX_FILA);

    do {
        printf("\n-----------------------------------------\n");
        exibirFila(&fila_pecas); // Exibe o estado atual da fila
        printf("-----------------------------------------\n");
        
        printf("1. Jogar Peça (Dequeue)\n");
        printf("2. Inserir Nova Peça (Enqueue)\n"); 
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        
        // Proteção contra leitura de caracteres/erros
        if (scanf("%d", &opcao) != 1) {
            opcao = -1; 
            while (getchar() != '\n'); // Limpa buffer
        }

        switch (opcao) {
            case 1: {
                // Jogar uma peça (remoção)
                Peca jogada = removerPeca(&fila_pecas);
                if (jogada.id != -1) {
                    printf("-> PEÇA JOGADA: [%c%d]\n", jogada.nome, jogada.id);
                    
                    // Requisito: A cada remoção, insere uma nova para manter a fila cheia (fluxo contínuo)
                    if (!filaCheia(&fila_pecas)) {
                         inserirPeca(&fila_pecas, gerarPeca());
                         printf("-> Nova peça adicionada ao final para manter o fluxo.\n");
                    }
                }
                break;
            }
            case 2:
                // Inserir nova peça (para testar o limite e a circularidade)
                inserirPeca(&fila_pecas, gerarPeca());
                break;
            case 0:
                printf("Saindo do simulador. Peças restantes na fila: %d\n", fila_pecas.total);
                break;
            default:
                printf("Opção inválida ou erro de entrada.\n");
        }
    } while (opcao != 0);

    return 0;
}


// =================================================================
// IMPLEMENTAÇÕES DAS FUNÇÕES
// =================================================================

// Inicializa a Fila
void inicializarFila(Fila *f) {
    f->inicio = 0;
    f->fim = 0;
    f->total = 0;
}

// Verifica se a Fila está Cheia
int filaCheia(Fila *f) {
    // A fila está cheia quando o número total de elementos atinge a capacidade máxima.
    return f->total == MAX_FILA;
}

// Verifica se a Fila está Vazia
int filaVazia(Fila *f) {
    // A fila está vazia quando o número total de elementos é zero.
    return f->total == 0;
}

// Gera uma nova peça com ID único e tipo aleatório
Peca gerarPeca() {
    // Tipos de peças padrão do Tetris
    char tipos[] = {'I', 'O', 'T', 'L', 'S', 'Z', 'J'};
    int num_tipos = sizeof(tipos) / sizeof(tipos[0]);
    
    Peca nova;
    nova.nome = tipos[rand() % num_tipos]; // Sorteia um tipo de peça
    nova.id = ID_GLOBAL++;                 // Atribui e incrementa o ID global
    return nova;
}

// Operação ENQUEUE: Adiciona um elemento ao final da fila
void inserirPeca(Fila *f, Peca p) {
    if (filaCheia(f)) {
        printf("Erro: Fila cheia. Não é possível inserir a peça.\n");
        return;
    }

    // Insere a nova peça no índice 'fim'
    f->itens[f->fim] = p; 
    
    // ATUALIZAÇÃO CIRCULAR (Lógica Modular): Calcula o novo 'fim' usando o operador módulo (%).
    // Isso faz com que o índice volte a 0 ao atingir o limite do array, reaproveitando espaços[cite: 520].
    f->fim = (f->fim + 1) % MAX_FILA; 
    
    f->total++;
}

// Operação DEQUEUE: Remove o elemento da frente da fila
Peca removerPeca(Fila *f) {
    Peca vazia = {'X', -1}; // Peça de retorno padrão para indicar falha/vazio
    
    if (filaVazia(f)) {
        printf("Erro: Fila vazia. Nenhuma peça para jogar.\n");
        return vazia;
    }
    
    // Armazena a peça que está na frente (início)
    Peca removida = f->itens[f->inicio];
    
    // ATUALIZAÇÃO CIRCULAR (Lógica Modular): O novo 'inicio' é calculado com o operador módulo (%).
    f->inicio = (f->inicio + 1) % MAX_FILA; 
    
    f->total--;
    return removida;
}

// Exibe o estado atual da fila (Percorrimento)
void exibirFila(Fila *f) {
    printf("Fila de Peças Futuras (%d/%d): ", f->total, MAX_FILA);
    
    if (filaVazia(f)) {
        printf("[VAZIA]\n");
        return;
    }

    int i = f->inicio;
    int count = 0;
    
    // Itera do INÍCIO até o FIM, respeitando a ordem FIFO e a lógica circular.
    while (count < f->total) {
        printf("[%c%d]", f->itens[i].nome, f->itens[i].id);
        i = (i + 1) % MAX_FILA; // Move para o próximo índice circularmente
        count++;
        if (count < f->total) {
            printf(" ");
        }
    }
    printf("\n");
}
