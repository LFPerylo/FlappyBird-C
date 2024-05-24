#include <string.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define BIRD_WIDTH 5
#define BIRD_HEIGHT 3
#define GRAVITY 0.05
#define JUMP_STRENGTH -0.50
#define OBSTACLE_WIDTH 5
#define OBSTACLE_GAP 6
#define NUM_OBSTACLES 3

int* x;
double* y;
double* velocity;
int score = 0;
int topScore = 0;
int gameStarted = 0;
int jumping = 0;

typedef struct {
    int x;
    int gap_y;
} Obstacle;

Obstacle* obstacles[NUM_OBSTACLES];
char obstacleMatrix[SCREEN_WIDTH][SCREEN_HEIGHT];

void delay(unsigned int milliseconds)
{
    usleep(milliseconds * 1000);
}

void printBird()
{
    screenSetColor(CYAN, DARKGRAY);
    screenGotoxy(*x, (int)(*y));
    printf(" @> ");
    screenGotoxy(*x, (int)(*y) + 1);
    printf("/o \\");
}

void drawBorders()
{
    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        screenGotoxy(x, 0);
        putchar('*');
        screenGotoxy(x, SCREEN_HEIGHT - 1);
        putchar('*');
    }
    for (int y = 1; y < SCREEN_HEIGHT - 1; y++)
    {
        screenGotoxy(0, y);
        putchar('*');
        screenGotoxy(SCREEN_WIDTH - 1, y);
        putchar('*');
    }
}

void gameOver()
{
    if (score > topScore) {
        topScore = score;
        FILE *file = fopen("topscore.txt", "w");
        if (file) {
            fprintf(file, "%d\n", topScore);
            fclose(file);
        }
    }

    screenSetColor(RED, BLACK);
    screenGotoxy(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2);
    printf("Game Over");
    screenGotoxy(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2 + 1);
    printf("Top Score: %d", topScore);
    screenUpdate();
    delay(5000);
}

void initObstacles()
{
    for (int i = 0; i < NUM_OBSTACLES; i++)
    {
        obstacles[i] = (Obstacle*)malloc(sizeof(Obstacle));
        obstacles[i]->x = SCREEN_WIDTH + i * (SCREEN_WIDTH / NUM_OBSTACLES);
        obstacles[i]->gap_y = rand() % (SCREEN_HEIGHT - OBSTACLE_GAP - 4) + 2;
    }
}

void freeObstacles()
{
    for (int i = 0; i < NUM_OBSTACLES; i++)
    {
        free(obstacles[i]);
    }
}

void updateObstacleMatrix()
{
    memset(obstacleMatrix, 0, sizeof(obstacleMatrix));
    for (int i = 0; i < NUM_OBSTACLES; i++)
    {
        if (obstacles[i]->x >= 0 && obstacles[i]->x < SCREEN_WIDTH)
        {
            for (int y = 0; y < SCREEN_HEIGHT; y++)
            {
                if (y < obstacles[i]->gap_y || y > obstacles[i]->gap_y + OBSTACLE_GAP)
                {
                    for (int w = 0; w < OBSTACLE_WIDTH; w++)
                    {
                        if (obstacles[i]->x + w < SCREEN_WIDTH)
                        {
                            obstacleMatrix[obstacles[i]->x + w][y] = 1;
                        }
                    }
                }
            }
        }
    }
}

void moveObstacles()
{
    for (int i = 0; i < NUM_OBSTACLES; i++)
    {
        if (!jumping) {
            continue;
        }
        obstacles[i]->x--;
        if (obstacles[i]->x < 0)
        {
            obstacles[i]->x = SCREEN_WIDTH;
            obstacles[i]->gap_y = rand() % (SCREEN_HEIGHT - OBSTACLE_GAP - 4) + 2;
        }
    }
}

void drawObstacles()
{
    screenSetColor(GREEN, BLACK);
    updateObstacleMatrix();
    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        for (int y = 0; y < SCREEN_HEIGHT; y++)
        {
            if (obstacleMatrix[x][y])
            {
                screenGotoxy(x, y);
                printf("#");
            }
        }
    }
}

int checkCollision()
{
    for (int i = 0; i < NUM_OBSTACLES; i++)
    {
        if (*x + BIRD_WIDTH >= obstacles[i]->x && *x <= obstacles[i]->x + OBSTACLE_WIDTH)
        {
            if (*y < obstacles[i]->gap_y || *y + BIRD_HEIGHT > obstacles[i]->gap_y + OBSTACLE_GAP)
            {
                return 1;
            }
        }
    }
    return 0;
}

void updateScore()
{
    for (int i = 0; i < NUM_OBSTACLES; i++)
    {
        if (obstacles[i]->x == *x)
        {
            score++;
        }
    }
}

void printScore()
{
    screenSetColor(WHITE, BLACK);
    screenGotoxy(SCREEN_WIDTH / 2 - 5, 1);
    printf("Score: %d", score);
}

void readTopScore()
{
    FILE *file = fopen("topscore.txt", "r");
    if (file) {
        fscanf(file, "%d", &topScore);
        fclose(file);
    } else {
        topScore = 0;
    }
}

int main()
{
    x = (int*)malloc(sizeof(int));
    y = (double*)malloc(sizeof(double));
    velocity = (double*)malloc(sizeof(double));

    if (!x || !y || !velocity) {
        fprintf(stderr, "Erro de alocação de memória\n");
        return 1;
    }

    *x = (SCREEN_WIDTH / 2) - 20;
    *y = SCREEN_HEIGHT / 2;
    *velocity = 0;

    static int ch = 0;

    screenInit(1);
    keyboardInit();
    timerInit(50);
    initObstacles();
    readTopScore();

    drawBorders();

    while (ch != 10)
    {
        screenClear();
        drawBorders();
        moveObstacles();
        drawObstacles();
        printBird();
        printScore();

        if (gameStarted && jumping)
        {
            *velocity += GRAVITY;
            *y += *velocity;

            if (*y >= SCREEN_HEIGHT - 2)
            {
                *y = SCREEN_HEIGHT - 2;
                *velocity = 0;
                gameOver();
                break;
            }

            if (*y < 1)
            {
                *y = 1;
                *velocity = 0;
            }

            if (checkCollision())
            {
                gameOver();
                break;
            }

            updateScore();
        }

        screenUpdate();

        if (keyhit())
        {
            ch = readch();
            if (ch == 27) {
                if (keyhit() && readch() == '[') {
                    switch (readch()) {
                        case 'A':
                            *velocity = JUMP_STRENGTH;
                            gameStarted = 1;
                            jumping = 1;
                            break;
                    }
                }
            } else if (ch == 10) {
                break;
            }
        }

        delay(150);
    }

    free(x);
    free(y);
    free(velocity);
    freeObstacles();

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}