#include <string.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define LARGURA_PASSARO 5
#define ALTURA_PASSARO 3
#define GRAVIDADE 0.05
#define FORCA_PULO -0.50
#define LARGURA_OBSTACULO 5
#define GAP_OBSTACULO 8 
#define NUM_OBSTACULOS 3

int* x;
double* y;
double* velocidade;
int score = 0;
int TopScore = 0;
int jogoIniciado = 0;
int pulando = 0;

typedef struct {
    int x;
    int gap_y;
} Obstaculo;

Obstaculo* obstaculos[NUM_OBSTACULOS];
char matrizObstaculos[SCREEN_WIDTH][SCREEN_HEIGHT];

void printarPassaro() {
    screenSetColor(YELLOW, YELLOW);
    screenGotoxy(*x, (int)(*y));
    printf("___( o)> ");
    screenGotoxy(*x, (int)(*y) + 1);
    printf("  \\    )");
    screenSetColor(WHITE, BLACK);
}

void gameOver() {
    if (score > TopScore) {
        TopScore = score;
        FILE *file = fopen("TopScore.txt", "w");
        if (file) {
            fprintf(file, "%d\n", TopScore);
            fclose(file);
        }
    }
    screenSetColor(RED, BLACK);
    screenGotoxy(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2);
    printf("Game Over");
    screenGotoxy(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2 + 1);
    printf("TopScore: %d", TopScore);
    screenUpdate();
    usleep(5000 * 1000); 
}

void iniciarObstaculos() {
    for (int i = 0; i < NUM_OBSTACULOS; i++) {
        obstaculos[i] = (Obstaculo*)malloc(sizeof(Obstaculo));
        obstaculos[i]->x = SCREEN_WIDTH + i * (SCREEN_WIDTH / NUM_OBSTACULOS);
        obstaculos[i]->gap_y = rand() % (SCREEN_HEIGHT - GAP_OBSTACULO - 4) + 2;
    }
}

void liberarObstaculos() {
    for (int i = 0; i < NUM_OBSTACULOS; i++) {
        free(obstaculos[i]);
    }
}

void moverObstaculos() {
    for (int i = 0; i < NUM_OBSTACULOS; i++) {
        if (!pulando) {
            continue;
        }
        obstaculos[i]->x--;
        if (obstaculos[i]->x < 0) {
            obstaculos[i]->x = SCREEN_WIDTH;
            obstaculos[i]->gap_y = rand() % (SCREEN_HEIGHT - GAP_OBSTACULO - 4) + 2;
        }
    }
}

void desenharObstaculos() {
    screenSetColor(GREEN, BLACK);
    memset(matrizObstaculos, 0, sizeof(matrizObstaculos)); 
    for (int i = 0; i < NUM_OBSTACULOS; i++) {
        if (obstaculos[i]->x >= 0 && obstaculos[i]->x < SCREEN_WIDTH) {
            for (int y = 0; y < SCREEN_HEIGHT; y++) {
                if (y < obstaculos[i]->gap_y || y > obstaculos[i]->gap_y + GAP_OBSTACULO) {
                    for (int w = 0; w < LARGURA_OBSTACULO; w++) {
                        if (obstaculos[i]->x + w < SCREEN_WIDTH) {
                            matrizObstaculos[obstaculos[i]->x + w][y] = 1;
                        }
                    }
                }
            }
        }
    }
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            if (matrizObstaculos[x][y]) {
                screenGotoxy(x, y);
                printf("#");
            }
        }
    }
    screenSetColor(WHITE, BLACK);
}

int verificarColisao() {
    for (int i = 0; i < NUM_OBSTACULOS; i++) {
        if (*x + LARGURA_PASSARO >= obstaculos[i]->x && *x <= obstaculos[i]->x + LARGURA_OBSTACULO) {
            if (*y < obstaculos[i]->gap_y || *y + ALTURA_PASSARO > obstaculos[i]->gap_y + GAP_OBSTACULO) {
                return 1;
            }
        }
    }
    return 0;
}

void atualizarPontuacao() {
    for (int i = 0; i < NUM_OBSTACULOS; i++) {
        if (obstaculos[i]->x == *x) {
            score++;
        }
    }
}

void printarPontuacao() {
    screenSetColor(WHITE, BLACK);
    screenGotoxy(SCREEN_WIDTH / 2 - 5, 1);
    printf("Score: %d", score);
}

void lerMaiorPontuacao() {
    FILE *file = fopen("TopScore.txt", "r");
    if (file) {
        fscanf(file, "%d", &TopScore);
        fclose(file);
    } else {
        TopScore = 0;
    }
}

int main() {
    x = (int*)malloc(sizeof(int));
    y = (double*)malloc(sizeof(double));
    velocidade = (double*)malloc(sizeof(double));

    if (!x || !y || !velocidade) {
        fprintf(stderr, "Erro de alocação de memória\n");
        return 1;
    }

    *x = (SCREEN_WIDTH / 2) - 20;
    *y = SCREEN_HEIGHT / 2;
    *velocidade = 0;

    static int ch = 0;

    screenInit(1);
    keyboardInit();
    timerInit(50);
    iniciarObstaculos();
    lerMaiorPontuacao();

    while (ch != 10) {
        screenClear();
        screenDrawBorders();
        moverObstaculos();
        desenharObstaculos();
        printarPassaro();
        printarPontuacao();

        if (jogoIniciado && pulando) {
            *velocidade += GRAVIDADE;
            *y += *velocidade;

            if (*y >= SCREEN_HEIGHT - 2) {
                *y = SCREEN_HEIGHT - 2;
                *velocidade = 0;
                gameOver();
                break;
            }

            if (*y < 1) {
                *y = 1;
                *velocidade = 0;
            }

            if (verificarColisao()) {
                gameOver();
                break;
            }

            atualizarPontuacao();
        }

        screenUpdate();

        if (keyhit()) {
            ch = readch();
            if (ch == 27) {
                if (keyhit() && readch() == '[') {
                    switch (readch()) {
                        case 'A':
                            *velocidade = FORCA_PULO;
                            jogoIniciado = 1;
                            pulando = 1;
                            break;
                    }
                }
            } else if (ch == 10) {
                break;
            }
        }

        usleep(150 * 1000); 
    }

    free(x);
    free(y);
    free(velocidade);
    liberarObstaculos();

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}

