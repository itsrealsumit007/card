#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define NAME_LENGTH 50
#define LEADERBOARD_SIZE 3
#define MAX_HINTS 3
#define DIFFICULTY_LEVELS 3
#define STATS_FILE "gamestats.txt"

typedef struct {
    int rows;
    int cols;
    int totalCards;
} Difficulty;

typedef struct {
    int totalGames;
    int totalMoves;
    int bestScore;
    double bestTime;
} GameStats;

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
GameStats stats[DIFFICULTY_LEVELS];

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

void updateStats(int difficulty, int moves, double elapsedTime) {
    stats[difficulty].totalGames++;
    stats[difficulty].totalMoves += moves;
    if (stats[difficulty].bestScore == 0 || moves < stats[difficulty].bestScore) {
        stats[difficulty].bestScore = moves;
    }
    if (stats[difficulty].bestTime == 0 || elapsedTime < stats[difficulty].bestTime) {
        stats[difficulty].bestTime = elapsedTime;
    }
}

void saveStats() {
    FILE *file = fopen(STATS_FILE, "w");
    if (file) {
        for (int i = 0; i < DIFFICULTY_LEVELS; i++) {
            fprintf(file, "%d %d %d %.2f\n", stats[i].totalGames, stats[i].totalMoves, stats[i].bestScore, stats[i].bestTime);
        }
        fclose(file);
    }
}

void loadStats() {
    FILE *file = fopen(STATS_FILE, "r");
    if (file) {
        for (int i = 0; i < DIFFICULTY_LEVELS; i++) {
            fscanf(file, "%d %d %d %lf\n", &stats[i].totalGames, &stats[i].totalMoves, &stats[i].bestScore, &stats[i].bestTime);
        }
        fclose(file);
    }
}

void displayStats() {
    printf("Game Statistics:\n");
    for (int i = 0; i < DIFFICULTY_LEVELS; i++) {
        printf("%s Difficulty:\n", i == 0 ? "Easy" : (i == 1 ? "Medium" : "Hard"));
        printf("Total Games Played: %d\n", stats[i].totalGames);
        printf("Total Moves: %d\n", stats[i].totalMoves);
        if (stats[i].totalGames > 0) {
            printf("Average Moves per Game: %.2f\n", (double)stats[i].totalMoves / stats[i].totalGames);
            printf("Best Score (Least Moves): %d\n", stats[i].bestScore);
            printf("Fastest Time: %.2f seconds\n", stats[i].bestTime);
        }
        printf("\n");
    }
}

void revealHint(int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (!revealed[i][j]) {
                for (int k = 0; k < rows; k++) {
                    for (int l = 0; l < cols; l++) {
                        if (!revealed[k][l] && cards[i][j] == cards[k][l] && (i != k || j != l)) {
                            revealed[i][j] = true;
                            revealed[k][l] = true;
                            printBoard(rows, cols);
                            sleep(2);
                            revealed[i][j] = false;
                            revealed[k][l] = false;
                            return;
                        }
                    }
                }
            }
        }
    }
}

void playGame(int difficulty) {
    int rows = difficulties[difficulty].rows;
    int cols = difficulties[difficulty].cols;
    int totalCards = difficulties[difficulty].totalCards;
    int moves = 0, matchedPairs = 0;
    time_t startTime, endTime;

    hints = MAX_HINTS;

    initializeBoard(rows, cols, totalCards);
    resetRevealed(rows, cols);
    clearScreen();
    printBoard(rows, cols);

    time(&startTime);

    while (matchedPairs < totalCards) {
        int firstRow, firstCol, secondRow, secondCol;
        
        printf("\nEnter coordinates of first card (row column): ");
        scanf("%d %d", &firstRow, &firstCol);

        while (!isValidInput(firstRow - 1, firstCol - 1, rows, cols)) {
            printf("Invalid input. Enter coordinates of first card (row column): ");
            scanf("%d %d", &firstRow, &firstCol);
        }

        revealed[firstRow - 1][firstCol - 1] = true;
        clearScreen();
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

        if (cards[firstRow - 1][firstCol - 1] == cards[secondRow - 1][secondCol - 1]) {
            matchedPairs++;
        } else {
            sleep(2);
            revealed[firstRow - 1][firstCol - 1] = false;
            revealed[secondRow - 1][secondCol - 1] = false;
        }

        moves++;
        clearScreen();
        printBoard(rows, cols);
    }

    time(&endTime);
    double elapsedTime = difftime(endTime, startTime);

    printf("\nCongratulations %s! You've completed the game in %d moves!\n", playerName, moves);
    printf("Time taken: %.2f seconds\n", elapsedTime);

    updateLeaderboard(moves, elapsedTime);
    updateStats(difficulty, moves, elapsedTime);
    saveStats();
}

int main() {
    int choice;

    printf("Enter your name: ");
    fgets(playerName, NAME_LENGTH, stdin);
    playerName[strcspn(playerName, "\n")] = '\0';

    memset(bestScores, 0, sizeof(bestScores));
    memset(bestTimes, 0, sizeof(bestTimes));
    memset(stats, 0, sizeof(stats));
    loadStats();

    while (1) {
        printf("Memory Game\n");
        printf("1. Play Game (Easy)\n");
        printf("2. Play Game (Medium)\n");
        printf("3. Play Game (Hard)\n");
        printf("4. Load Game\n");
        printf("5. View Leaderboard\n");
        printf("6. View Game Statistics\n");
        printf("7. Use Hint (%d hints remaining)\n", hints);
        printf("8. Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        int difficultyIndex = choice - 1;
        if (difficultyIndex >= 0 && difficultyIndex <= 2) {
            playGame(difficultyIndex);
        } else {
            switch (choice) {
                case 4: {
                    int moves, matchedPairs, rows, cols;
                    time_t startTime;
                    if (loadGame(&moves, &matchedPairs, &startTime, &rows, &cols)) {
                        printf("Game loaded successfully!\n");
                        playGame(rows * cols / 8 - 1);
                    }
                    break;
                }
                case 5:
                    displayLeaderboard();
                    break;
                case 6:
                    displayStats();
                    break;
                case 7:
                    if (hints > 0) {
                        hints--;
                        revealHint(difficulties[0].rows, difficulties[0].cols);  // Assuming easy difficulty for hint
                    } else {
                        printf("No hints remaining.\n");
                    }
                    break;
                case 8:
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
