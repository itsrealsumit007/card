#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 4
#define COLS 4
#define TOTAL_CARDS (ROWS * COLS / 2)

char cards[ROWS][COLS];

void initializeBoard() {
    char symbols[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    int symbolCount[8] = {0};

    srand(time(NULL));

    for (int i = 0; i < TOTAL_CARDS * 2; ) {
        int row = rand() % ROWS;
        int col = rand() % COLS;
        
        if (cards[row][col] == '\0' && symbolCount[i / 2] < 2) {
            cards[row][col] = symbols[i / 2];
            symbolCount[i / 2]++;
            i++;
        }
    }
}

void printBoard() {
    system("cls || clear");
    
    printf("Memory Game\n\n");

    printf("   ");
    for (int col = 0; col < COLS; col++) {
        printf("%2d ", col + 1);
    }
    printf("\n");

    for (int row = 0; row < ROWS; row++) {
        printf("%2d ", row + 1);
        for (int col = 0; col < COLS; col++) {
            if (cards[row][col] == '\0') {
                printf("   ");
            } else {
                printf("[%c]", cards[row][col]);
            }
        }
        printf("\n");
    }
}

int main() {
    initializeBoard();
    printBoard();

    return 0;
}
