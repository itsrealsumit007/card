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
#define MAX_HINTS 3

char cards[ROWS][COLS];
bool revealed[ROWS][COLS];
char playerName[NAME_LENGTH];
int bestScores[LEADERBOARD_SIZE];
char bestPlayers[LEADERBOARD_SIZE][NAME_LENGTH];
double bestTimes[LEADERBOARD_SIZE];
int hints = MAX_HINTS;

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

void saveGame(int moves, int matchedPairs, time_t startTime) {
    FILE *file = fopen("savegame.txt", "w");
    if (file) {
        fprintf(file, "%d %d %d %ld\n", moves, matchedPairs, hints, startTime);
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                fprintf(file, "%c", cards[row][col]);
            }
        }
        fprintf(file, "\n");
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                fprintf(file, "%d", revealed[row][col]);
            }
        }
        fclose(file);
        printf("Game saved successfully!\n");
    } else {
        printf("Failed to save the game.\n");
    }
}

bool loadGame(int *moves, int *matchedPairs, time_t *startTime) {
    FILE *file = fopen("savegame.txt", "r");
    if (file) {
        fscanf(file, "%d %d %d %ld\n", moves, matchedPairs, &hints, startTime);
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                fscanf(file, " %c", &cards[row][col]);
            }
        }
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                fscanf(file, " %d", (int *)&revealed[row][col]);
            }
        }
        fclose(file);
        return true;
    } else {
        printf("No saved game found.\n");
        return false;
    }
}

void updateLeaderboard(int moves, double elapsedTime) {
    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        if (moves < bestScores[i] || bestScores[i] == 0) {
            for (int j = LEADERBOARD_SIZE - 1; j > i; j--) {
                bestScores[j] = bestScores[j - 1];
                strcpy(bestPlayers[j], bestPlayers[j - 1]);
                bestTimes[j] = bestTimes[j - 1];
            }
            bestScores[i] = moves;
            strcpy(bestPlayers[i], playerName);
            bestTimes[i] = elapsedTime;
            break;
        }
    }
}

void displayLeaderboard() {
    printf("Leaderboard:\n");
    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        if (bestScores[i] != 0) {
            printf("%d. %s - %d moves - %.2f seconds\n", i + 1, bestPlayers[i], bestScores[i], bestTimes[i]);
        } else {
            printf("%d. ---\n", i + 1);
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
        printf("Hints left: %d\n", hints);
        
        if (hints > 0) {
            printf("Press 'H' for a hint or any other key to continue: ");
            char hintChoice;
            scanf(" %c", &hintChoice);
            if (hintChoice == 'H' || hintChoice == 'h') {
                hints--;
                for (int i = 0; i < ROWS; i++) {
                    for (int j = 0; j < COLS; j++) {
                        if (!revealed[i][j]) {
                            for (int k = 0; k < ROWS; k++) {
                                for (int l = 0; l < COLS; l++) {
                                    if (!revealed[k][l] && (i != k || j != l) && cards[i][j] == cards[k][l]) {
                                        revealed[i][j] = revealed[k][l] = true;
                                        printBoard();
                                        sleep(2);
                                        revealed[i][j] = revealed[k][l] = false;
                                        clearScreen();
                                        printBoard();
                                        i = ROWS;  // Break out of both loops
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    time(&endTime);
    double elapsedTime = difftime(endTime, startTime);

    printf("\nCongratulations %s! You've completed the game in %d moves!\n", playerName, moves);
    printf("Time taken: %.2f seconds\n", elapsedTime);

    updateLeaderboard(moves, elapsedTime);
}

int main() {
    int choice;

    printf("Enter your name: ");
    fgets(playerName, NAME_LENGTH, stdin);
    playerName[strcspn(playerName, "\n")] = '\0';

    memset(bestScores, 0, sizeof(bestScores));
    memset(bestTimes, 0, sizeof(bestTimes));

    while (1) {
        printf("Memory Game\n");
        printf("1. Play Game\n");
        printf("2. Load Game\n");
        printf("3. View Leaderboard\n");
        printf("4. Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                playGame();
                break;
            case 2: {
                int moves, matchedPairs;
                time_t startTime;
                if (loadGame(&moves, &matchedPairs, &startTime)) {
                    printf("Game loaded successfully!\n");
                    playGame();
                }
                break;
            }
            case 3:
                displayLeaderboard();
                break;
            case 4:
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
