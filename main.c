#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_CAIXAS 5

typedef struct Cliente {
    char nome[100];
    char cpf[12];
    int prioridade;
    int itens;
    struct Cliente *prox;
} Cliente;

typedef struct Caixa {
    int numero;
    int aberto;
    Cliente *fila;
} Caixa;

Caixa caixas[MAX_CAIXAS];

void inicializar_caixas();
int validar_cpf(const char *cpf);
Cliente* criar_cliente(char *nome, char *cpf, int prioridade, int itens);
void inserir_cliente(int num_caixa, Cliente *cliente);
void atender_cliente(int num_caixa);
void fechar_caixa(int num_caixa);
void listar_clientes();
void listar_status_caixas();
void menu();

void inicializar_caixas() {
    for (int i = 0; i < MAX_CAIXAS; i++) {
        caixas[i].numero = i + 1;
        caixas[i].aberto = 1;
        caixas[i].fila = NULL;
    }
}

int validar_cpf(const char *cpf) {
    if (strlen(cpf) != 11) return 0;
    for (int i = 0; i < 11; i++) {
        if (!isdigit(cpf[i])) return 0;
    }
    return 1;
}

Cliente* criar_cliente(char *nome, char *cpf, int prioridade, int itens) {
    Cliente *novo = (Cliente*)malloc(sizeof(Cliente));
    strcpy(novo->nome, nome);
    strcpy(novo->cpf, cpf);
    novo->prioridade = prioridade;
    novo->itens = itens;
    novo->prox = NULL;
    return novo;
}

void inserir_cliente(int num_caixa, Cliente *cliente) {
    if (!caixas[num_caixa - 1].aberto) {
        printf("Erro: Caixa %d está fechado.\n", num_caixa);
        free(cliente);
        return;
    }
    Cliente **fila = &caixas[num_caixa - 1].fila;
    if (!*fila || (*fila)->prioridade > cliente->prioridade) {
        cliente->prox = *fila;
        *fila = cliente;
    } else {
        Cliente *atual = *fila;
        while (atual->prox && atual->prox->prioridade <= cliente->prioridade) {
            atual = atual->prox;
        }
        cliente->prox = atual->prox;
        atual->prox = cliente;
    }
}

void atender_cliente(int num_caixa) {
    if (!caixas[num_caixa - 1].fila) {
        printf("Nenhum cliente na fila do caixa %d.\n", num_caixa);
        return;
    }
    Cliente *removido = caixas[num_caixa - 1].fila;
    caixas[num_caixa - 1].fila = removido->prox;
    printf("Cliente %s atendido no caixa %d.\n", removido->nome, num_caixa);
    free(removido);
}

void fechar_caixa(int num_caixa) {
    if (!caixas[num_caixa - 1].aberto) {
        printf("Caixa %d já está fechado.\n", num_caixa);
        return;
    }
    caixas[num_caixa - 1].aberto = 0;
    printf("Caixa %d fechado.\n", num_caixa);
}

void listar_clientes() {
    for (int i = 0; i < MAX_CAIXAS; i++) {
        printf("Caixa %d (%s):\n", caixas[i].numero, caixas[i].aberto ? "Aberto" : "Fechado");
        Cliente *c = caixas[i].fila;
        while (c) {
            printf("- %s (CPF: %s, Prioridade: %d, Itens: %d)\n", c->nome, c->cpf, c->prioridade, c->itens);
            c = c->prox;
        }
    }
}

void listar_status_caixas() {
    printf("Status dos Caixas:\n");
    for (int i = 0; i < MAX_CAIXAS; i++) {
        printf("Caixa %d - %s\n", caixas[i].numero, caixas[i].aberto ? "Aberto" : "Fechado");
    }
}

void menu() {
    int opcao;
    inicializar_caixas();
    do {
        printf("\n1. Cadastrar Cliente\n2. Atender Cliente\n3. Fechar Caixa\n4. Listar Clientes\n5. Status dos Caixas\n0. Sair\nEscolha: ");
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida!\n");
            while (getchar() != '\n');
            continue;
        }
        switch (opcao) {
            case 1: {
                char nome[100], cpf[12];
                int prioridade, itens, num_caixa;
                printf("Nome: "); scanf("%s", nome);
                printf("CPF: "); scanf("%s", cpf);
                if (!validar_cpf(cpf)) {
                    printf("CPF inválido!\n");
                    break;
                }
                printf("Prioridade (1-Alta, 2-Média, 3-Baixa): ");
                scanf("%d", &prioridade);
                printf("Número de Itens: "); scanf("%d", &itens);
                printf("Número do Caixa (1-5): "); scanf("%d", &num_caixa);
                inserir_cliente(num_caixa, criar_cliente(nome, cpf, prioridade, itens));
                break;
            }
            case 2:
                { int num_caixa;
                printf("Número do Caixa: "); scanf("%d", &num_caixa);
                atender_cliente(num_caixa);
                }
                break;
            case 3:
                { int num_caixa;
                printf("Fechar Caixa: "); scanf("%d", &num_caixa);
                fechar_caixa(num_caixa);
                }
                break;
            case 4:
                listar_clientes();
                break;
            case 5:
                listar_status_caixas();
                break;
        }
    } while (opcao != 0);
}

int main() {
    menu();
    return 0;
}
