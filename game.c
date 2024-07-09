#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define NAME_LENGTH 50
#define LEADERBOARD_SIZE 3
#define MAX_HINTS 3

typedef struct {
    int rows;
    int cols;
    int totalCards;
} Difficulty;

const Difficulty difficulties[] = {
    {4, 4, 8},    // Easy: 4x4 grid, 8 cards
    {4, 5, 10},   // Medium: 4x5 grid, 10 cards
    {4, 6, 12}    // Hard: 4x6 grid, 12 cards
};

char cards[6][6];  // Maximum grid size for hard level
bool revealed[6][6];
char playerName[NAME_LENGTH];
int bestScores[LEADERBOARD_SIZE];
char bestPlayers[LEADERBOARD_SIZE][NAME_LENGTH];
double bestTimes[LEADERBOARD_SIZE];
int hints = MAX_HINTS;

void initializeBoard(int rows, int cols, int totalCards) {
    char symbols[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'};
    int symbolCount[12] = {0};

    srand(time(NULL));

    for (int i = 0; i < totalCards * 2; ) {
        int row = rand() % rows;
        int col = rand() % cols;
        
        if (cards[row][col] == '\0' && symbolCount[i / 2] < 2) {
            cards[row][col] = symbols[i / 2];
            symbolCount[i / 2]++;
            i++;
        }
    }
}

void resetRevealed(int rows, int cols) {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            revealed[row][col] = false;
        }
    }
}

void printBoard(int rows, int cols) {
    system("cls || clear");
    
    printf("Memory Game\n\n");

    printf("   ");
    for (int col = 0; col < cols; col++) {
        printf("%2d ", col + 1);
    }
    printf("\n");

    for (int row = 0; row < rows; row++) {
        printf("%2d ", row + 1);
        for (int col = 0; col < cols; col++) {
            if (revealed[row][col]) {
                printf("[%c]", cards[row][col]);
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }
}

bool isValidInput(int row, int col, int rows, int cols) {
    return row >= 0 && row < rows && col >= 0 && col < cols && !revealed[row][col];
}

void clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

void saveGame(int moves, int matchedPairs, time_t startTime, int rows, int cols) {
    FILE *file = fopen("savegame.txt", "w");
    if (file) {
        fprintf(file, "%d %d %d %ld\n", moves, matchedPairs, hints, startTime);
        fprintf(file, "%d %d\n", rows, cols);
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                fprintf(file, "%c", cards[row][col]);
            }
        }
        fprintf(file, "\n");
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                fprintf(file, "%d", revealed[row][col]);
            }
        }
        fclose(file);
        printf("Game saved successfully!\n");
    } else {
        printf("Failed to save the game.\n");
    }
}

bool loadGame(int *moves, int *matchedPairs, time_t *startTime, int *rows, int *cols) {
    FILE *file = fopen("savegame.txt", "r");
    if (file) {
        fscanf(file, "%d %d %d %ld\n", moves, matchedPairs, &hints, startTime);
        fscanf(file, "%d %d\n", rows, cols);
        for (int row = 0; row < *rows; row++) {
            for (int col = 0; col < *cols; col++) {
                fscanf(file, " %c", &cards[row][col]);
            }
        }
        for (int row = 0; row < *rows; row++) {
            for (int col = 0; col < *cols; col++) {
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

void playGame(int rows, int cols, int totalCards) {
    int moves = 0;
    int matchedPairs = 0;
    int firstRow, firstCol, secondRow, secondCol;
    time_t startTime, endTime;

    resetRevealed(rows, cols);
    initializeBoard(rows, cols, totalCards);
    time(&startTime);

    while (matchedPairs < totalCards) {
        printf("\nEnter coordinates of first card (row column): ");
        scanf("%d %d", &firstRow, &firstCol);

        while (!isValidInput(firstRow - 1, firstCol - 1, rows, cols)) {
            printf("Invalid input. Enter coordinates of first card (row column): ");
            scanf("%d %d", &firstRow, &firstCol);
        }

        clearScreen();
        revealed[firstRow - 1][firstCol - 1] = true;
        printBoard(rows, cols);

        printf("\nEnter coordinates of second card (row column): ");
        scanf("%d %d", &secondRow, &secondCol);

        while (!isValidInput(secondRow - 1, secondCol - 1, rows, cols) || (firstRow == secondRow && firstCol == secondCol)) {
            printf("Invalid input. Enter coordinates of second card (row column): ");
            scanf("%d %d", &secondRow, &secondCol);
        }

        clearScreen();
        revealed[secondRow - 1][secondCol - 1] = true;
        printBoard(rows, cols);

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
            printBoard(rows, cols);
        }

        printf("\nMoves: %d\n", moves);
        printf("Matched Pairs: %d\n", matchedPairs);
        printf("Pairs left: %d\n", totalCards - matchedPairs);
        printf("Hints left: %d\n", hints);
        
        if (hints > 0) {
            printf("Press 'H' for a hint or any other key to continue: ");
            char hintChoice;
            scanf(" %c", &hintChoice);
            if (hintChoice == 'H' || hintChoice == 'h') {
                hints--;
                for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < cols; j++) {
                        if (!revealed[i][j]) {
                            for (int k = 0; k < rows; k++) {
                                for (int l = 0; l < cols; l++) {
                                    if (!revealed[k][l] && (i != k || j != l) && cards[i][j] == cards[k][l]) {
                                        revealed[i][j] = revealed[k][l] = true;
                                        printBoard(rows, cols);
                                        sleep(2);
                                        revealed[i][j] = revealed[k][l] = false;
                                        clearScreen();
                                        printBoard(rows, cols);
                                        i = rows;  // Break out of both loops
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
        printf("1. Play Game (Easy)\n");
        printf("2. Play Game (Medium)\n");
        printf("3. Play Game (Hard)\n");
        printf("4. Load Game\n");
        printf("5. View Leaderboard\n");
        printf("6. Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        int difficultyIndex = choice - 1;
        if (difficultyIndex >= 0 && difficultyIndex <= 2) {
            int rows = difficulties[difficultyIndex].rows;
            int cols = difficulties[difficultyIndex].cols;
            int totalCards = difficulties[difficultyIndex].totalCards;
            switch (choice) {
                case 1:
                case 2:
                case 3:
                    playGame(rows, cols, totalCards);
                    break;
            }
        } else {
            switch (choice) {
                case 4: {
                    int moves, matchedPairs, rows, cols;
                    time_t startTime;
                    if (loadGame(&moves, &matchedPairs, &startTime, &rows, &cols)) {
                        printf("Game loaded successfully!\n");
                        playGame(rows, cols, rows * cols / 2);
                    }
                    break;
                }
                case 5:
                    displayLeaderboard();
                    break;
                case 6:
                    printf("Exiting the game. Goodbye!\n");
                    exit(0);
                default:
                    printf("Invalid choice. Please try again.\n");
            }
        }

        printf("\nPress Enter to return to the main menu.\n");
        getchar();
        getchar();
        clearScreen();
    }

    return 0;
}
