            #include <string.h>
            #include "screen.h"
            #include "keyboard.h"
            #include "timer.h"
            #include <unistd.h> 

            #define BIRD_WIDTH 5
            #define BIRD_HEIGHT 3
            #define GRAVITY 0.05 // Gravidade reduzida
            #define JUMP_STRENGTH -1.5

            int x = (SCREEN_WIDTH / 2) - 20; 
            double y = SCREEN_HEIGHT / 2;   
            double velocity = 0;             
           
            void delay(unsigned int milliseconds)
            {
                usleep(milliseconds * 1000); 
            }

            void printBird()
            {
                screenSetColor(CYAN, DARKGRAY);
                screenGotoxy(x, (int)y);
                printf(" @< ");
                screenGotoxy(x, (int)y + 1);
                printf("/o \\");
            }

            void printKey(int ch)
            {
                screenSetColor(YELLOW, DARKGRAY);
                screenGotoxy(35, 22);
                printf("Key code :");

                screenGotoxy(34, 23);
                printf("            ");

                if (ch == 27)
                    screenGotoxy(36, 23);
                else
                    screenGotoxy(39, 23);

                printf("%d ", ch);
                while (keyhit())
                {
                    printf("%d ", readch());
                }
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
                screenSetColor(RED, BLACK);
                screenGotoxy(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2);
                printf("Game Over");
                screenUpdate();
                delay(5000); 
            }

            int main()
            {
                static int ch = 0;

                screenInit(1);
                keyboardInit();
                timerInit(50);

                drawBorders();

                while (ch != 10)
                {
                   
                    velocity += GRAVITY;
                    y += velocity;

                    
                    if (y >= SCREEN_HEIGHT - 2)
                    {
                        y = SCREEN_HEIGHT - 2;
                        velocity = 0;
                        gameOver();
                        break; 
                    }

                    
                    if (y < 1)
                    {
                        y = 1;
                        velocity = 0;
                    }

                    
                    screenClear();

                    
                    drawBorders();

                    
                    printBird();
                    screenUpdate();

                    
                    if (keyhit())
                    {
                        ch = readch();
                        if (ch == 27) { 
                            if (keyhit() && readch() == '[') {
                                switch (readch()) {
                                    case 'A': 
                                        velocity = JUMP_STRENGTH;
                                        break;
                                    
                                }
                            }
                        } else if (ch == 10) {
                            break; 
                        }
                        printKey(ch);
                    }

                   
                    delay(150); 
                }

                keyboardDestroy();
                screenDestroy();
                timerDestroy();

                return 0;
            }
