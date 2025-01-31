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
int contar_caixas_abertos();
int existem_clientes_em_filas();
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

    // Verifica se há clientes em filas
    if (existem_clientes_em_filas() && contar_caixas_abertos() == 1) {
        printf("Erro: Não é possível fechar o último caixa aberto enquanto houver clientes para atender.\n");
        return;
    }

    // Verifica se há outro caixa aberto para transferir os clientes
    int outro_caixa = -1;
    for (int i = 0; i < MAX_CAIXAS; i++) {
        if (caixas[i].aberto && caixas[i].numero != num_caixa) {
            outro_caixa = caixas[i].numero;
            break;
        }
    }

    if (outro_caixa == -1) {
        printf("Erro: Não há outro caixa aberto para transferir os clientes.\n");
        return;
    }

    // Transferir clientes para outro caixa aberto
    Cliente *c = caixas[num_caixa - 1].fila;
    while (c) {
        Cliente *prox = c->prox;
        inserir_cliente(outro_caixa, c);
        c = prox;
    }

    caixas[num_caixa - 1].fila = NULL;
    caixas[num_caixa - 1].aberto = 0;
    printf("Caixa %d fechado. Clientes transferidos para o caixa %d.\n", num_caixa, outro_caixa);
}

int contar_caixas_abertos() {
    int count = 0;
    for (int i = 0; i < MAX_CAIXAS; i++) {
        if (caixas[i].aberto) count++;
    }
    return count;
}

int existem_clientes_em_filas() {
    for (int i = 0; i < MAX_CAIXAS; i++) {
        if (caixas[i].fila != NULL) return 1;
    }
    return 0;
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
            printf("Entrada inválida! Digite um número.\n");
            while (getchar() != '\n'); // Limpa o buffer de entrada
            continue;
        }
        switch (opcao) {
            case 1: {
                char nome[100], cpf[12];
                int prioridade, itens, num_caixa;
                printf("Nome: "); scanf("%s", nome);
                printf("CPF: "); scanf("%s", cpf);
                if (!validar_cpf(cpf)) {
                    printf("CPF inválido! Deve conter exatamente 11 dígitos.\n");
                    break;
                }
                printf("Prioridade (1-Alta, 2-Média, 3-Baixa): ");
                if (scanf("%d", &prioridade) != 1 || prioridade < 1 || prioridade > 3) {
                    printf("Prioridade inválida! Digite um valor entre 1 e 3.\n");
                    while (getchar() != '\n'); // Limpa o buffer de entrada
                    break;
                }
                printf("Número de Itens: ");
                if (scanf("%d", &itens) != 1 || itens < 0) {
                    printf("Número de itens inválido! Digite um valor positivo.\n");
                    while (getchar() != '\n'); // Limpa o buffer de entrada
                    break;
                }
                printf("Número do Caixa (1-5): ");
                if (scanf("%d", &num_caixa) != 1 || num_caixa < 1 || num_caixa > MAX_CAIXAS) {
                    printf("Número do caixa inválido! Digite um valor entre 1 e %d.\n", MAX_CAIXAS);
                    while (getchar() != '\n'); // Limpa o buffer de entrada
                    break;
                }
                inserir_cliente(num_caixa, criar_cliente(nome, cpf, prioridade, itens));
                break;
            }
            case 2: {
                int num_caixa;
                printf("Número do Caixa: ");
                if (scanf("%d", &num_caixa) != 1 || num_caixa < 1 || num_caixa > MAX_CAIXAS) {
                    printf("Número do caixa inválido! Digite um valor entre 1 e %d.\n", MAX_CAIXAS);
                    while (getchar() != '\n'); // Limpa o buffer de entrada
                    break;
                }
                atender_cliente(num_caixa);
                break;
            }
            case 3: {
                int num_caixa;
                printf("Fechar Caixa: ");
                if (scanf("%d", &num_caixa) != 1 || num_caixa < 1 || num_caixa > MAX_CAIXAS) {
                    printf("Número do caixa inválido! Digite um valor entre 1 e %d.\n", MAX_CAIXAS);
                    while (getchar() != '\n'); // Limpa o buffer de entrada
                    break;
                }
                fechar_caixa(num_caixa);
                break;
            }
            case 4:
                listar_clientes();
                break;
            case 5:
                listar_status_caixas();
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 0);
}

int main() {
    menu();
    return 0;
}