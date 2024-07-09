#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define ROWS 4
#define COLS 4
#define TOTAL_CARDS (ROWS * COLS / 2)
#define NAME_LENGTH 50
#define LEADERBOARD_SIZE 3

char cards[ROWS][COLS];
bool revealed[ROWS][COLS];
char playerName[NAME_LENGTH];
int bestScores[LEADERBOARD_SIZE];
char bestPlayers[LEADERBOARD_SIZE][NAME_LENGTH];

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

void resetRevealed() {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            revealed[row][col] = false;
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
            if (revealed[row][col]) {
                printf("[%c]", cards[row][col]);
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }
}

bool isValidInput(int row, int col) {
    return row >= 0 && row < ROWS && col >= 0 && col < COLS && !revealed[row][col];
}

void clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

void updateLeaderboard(int moves) {
    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        if (moves < bestScores[i] || bestScores[i] == 0) {
            for (int j = LEADERBOARD_SIZE - 1; j > i; j--) {
                bestScores[j] = bestScores[j - 1];
                strcpy(bestPlayers[j], bestPlayers[j - 1]);
            }
            bestScores[i] = moves;
            strcpy(bestPlayers[i], playerName);
            break;
        }
    }
}

void playGame() {
    int moves = 0;
    int matchedPairs = 0;
    int firstRow, firstCol, secondRow, secondCol;
    time_t startTime, endTime;

    resetRevealed();
    initializeBoard();
    time(&startTime);

    while (matchedPairs < TOTAL_CARDS) {
        printf("\nEnter coordinates of first card (row column): ");
        scanf("%d %d", &firstRow, &firstCol);

        while (!isValidInput(firstRow - 1, firstCol - 1)) {
            printf("Invalid input. Enter coordinates of first card (row column): ");
            scanf("%d %d", &firstRow, &firstCol);
        }

        clearScreen();
        revealed[firstRow - 1][firstCol - 1] = true;
        printBoard();

        printf("\nEnter coordinates of second card (row column): ");
        scanf("%d %d", &secondRow, &secondCol);

        while (!isValidInput(secondRow - 1, secondCol - 1) || (firstRow == secondRow && firstCol == secondCol)) {
            printf("Invalid input. Enter coordinates of second card (row column): ");
            scanf("%d %d", &secondRow, &secondCol);
        }

        clearScreen();
        revealed[secondRow - 1][secondCol - 1] = true;
        printBoard();

        moves++;

        if (cards[firstRow - 1][firstCol - 1] == cards[secondRow - 1][secondCol - 1]) {
            printf("\nMatch found!\n");
            matchedPairs++;
        } else {
            printf("\nNo match. Cards will flip back. Press Enter to continue.\n");
            getchar();
            getchar();
            revealed[firstRow - 1][firstCol - 1] = false;
            revealed[secondRow - 1][secondCol - 1] = false;
            clearScreen();
            printBoard();
        }

        printf("\nMoves: %d\n", moves);
        printf("Matched Pairs: %d\n", matchedPairs);
        printf("Pairs left: %d\n", TOTAL_CARDS - matchedPairs);
    }

    time(&endTime);
    double elapsedTime = difftime(endTime, startTime);

    printf("\nCongratulations %s! You've completed the game in %d moves!\n", playerName, moves);
    printf("Time taken: %.2f seconds\n", elapsedTime);

    updateLeaderboard(moves);
}

void displayLeaderboard() {
    printf("Leaderboard:\n");
    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        if (bestScores[i] != 0) {
            printf("%d. %s - %d moves\n", i + 1, bestPlayers[i], bestScores[i]);
        } else {
            printf("%d. ---\n", i + 1);
        }
    }
}

int main() {
    int choice;

    printf("Enter your name: ");
    fgets(playerName, NAME_LENGTH, stdin);
    playerName[strcspn(playerName, "\n")] = '\0';

    memset(bestScores, 0, sizeof(bestScores));

    while (1) {
        printf("Memory Game\n");
        printf("1. Play Game\n");
        printf("2. View Leaderboard\n");
        printf("3. Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                playGame();
                break;
            case 2:
                displayLeaderboard();
                break;
            case 3:
                printf("Exiting the game. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }

        printf("\nPress Enter to return to the main menu.\n");
        getchar();
        getchar();
        clearScreen();
    }

    return 0;
}
