/* 
 * File:   Lab9.c
 * Author: Davis
 *
 * Created on November 9, 2014, 5:23 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct blockScore {
    int vertical, horizontal, diagonalDown, diagonalUp, totalScore;
} BlockScore, *BlockScorePointer;

typedef struct potentialMove {
    int row, col, score;
} PotentialMove;

void initializeBoard(char board[21][21], int *n);
void printBoard(char board[21][21], int n);
void measurePosition(char board[21][21], int n);
void getBlockedSquares(char board[21][21], int n);
void enterMoves(char board[21][21], int n, char compColor);
bool isOccupied(char board[21][21], int row, int col);
int findLongest(char board[21][21], int n, int row, int col, char color);
void computerMove(char board[21][21], int n, char compColor);
void findWinner(char board[21][21], int n, bool *noWinner);
int getVerticalScore(char board[21][21], int n, int row, int col, char color);
int getHorizontalScore(char board[21][21], int n, int row, int col, char color);
int getDiagonalDownScore(char board[21][21], int n, int row, int col, char color);
int getDiagonalUpScore(char board[21][21], int n, int row, int col, char color);
void findWinningScore(char board[21][21], int n, BlockScore scoreBoard[21][21], BlockScore scoreBoard2[21][21], int *row, int *col);
bool fiveLongTrap(char board[21][21], int n, int row, int col, char color);
void scoreBoards(char board[21][21], int n, BlockScore compScore[21][21], BlockScore opponentScore[21][21], char compColor, char opponentColor);
void scoreSquare(char board[21][21], int n, int row, int col, BlockScore scoreBoard[21][21], char color);
bool surroundedByOwnColor(char board[21][21], int n, int row, int col, char color);
void secondaryAlterations(int n, BlockScore firstBoard[21][21], BlockScore secondBoard[21][21]);
void generalTrapMaker(char board[21][21], int n, int row, int col, BlockScore scoreBoard[21][21], char color);
void findTop3Moves(char board[21][21], int n, BlockScore scoreBoard[21][21], PotentialMove moves[3]);
void evaluateTop3Moves(char board[21][21], int n, BlockScore scoreBoard[21][21], char color, int *finalRow, int *finalCol);
int numMoves(char board[21][21], int n);

/*
 * Flow:
 * 1) scans user input for board dimension
 * 2) scans user input for computer-color
 * 3) scans user input for blocked squares
 * 4) game-play
 */
int main(int argc, char** argv) {
    // Declares variable for game-play (e.g. a board of max dimensions 21x21 & n for board dimension)
    int n;
    char board[21][21], compColor = 'B';
    
    initializeBoard(board, &n);
    
    // Prompts for computer color
    printf("Computer playing B or W?: ");
    scanf(" %c", &compColor);
    
    getBlockedSquares(board, n);
    
    enterMoves(board, n, compColor);
    
    //measurePosition(board, n);

    return (EXIT_SUCCESS);
}

void printBoard(char board[21][21], int n) {
    // Outer loop for rows, inner loop for columns
    int i;
    for (i = 0; i < n; i++) {
        int j;
        for (j = 0; j < n; j++)
            printf("%-2c", board[i][j]);
        // New line after each row
        printf("\n");
    }
}

void initializeBoard(char board[21][21], int *n) {
    // Prompts user for dimension input
    printf("Enter board dimensions (n), n>=6 and n<=21: ");
    scanf("%d", n);
    // Fills board with U
    int i;
    for (i = 0; i < *n; i++) {
        int j;
        for (j = 0; j < *n; j++)
            board[i][j] = 'U';
    }
    
    printBoard(board, *n);
}

/*
 * Function that facilitates the user entry of blocked 'R' squares
 */
void getBlockedSquares(char board[21][21], int n) {
    // Declares coordinate variables
    int row, col;
    
    do {
        // Prompts for input
        printf("Enter position (ROW COL) of blocked square; (-1 -1) to finish: ");
        scanf("%d %d", &row, &col);
        
        // Checks validity of the input
        if (row < n && row >= 0 && col < n && col >= 0) {
            // Checks occupied status
            if (board[row][col] != 'R') {
                board[row][col] = 'R';
                printBoard(board, n);
            }
            else
                printf("Position already occupied, try again\n");
        }
        else if (!(row == -1 && col == -1))
            printf("Out of range row or column\n");
    }
    while (!(row == -1 && col == -1));
}

/*
 * Function that allows for the entry of moves and toggles moves
 */
void enterMoves(char board[21][21], int n, char compColor) {
    // Declares integers row and col for player input
    int row, col;
    bool noWinner = true;
    
    // Declares variables corresponding to who's turn it is
    char turn = 'B', playerColor = (compColor == 'B') ? 'W' : 'B';
    
    do {
        // Gets computer's turn
        if (turn == compColor && noWinner) {
            computerMove(board, n, compColor);
            findWinner(board, n, &noWinner);
            turn = (turn == 'B') ? 'W' : 'B'; // switch turns
        }
        
        // Gets player's turn
        if (turn == playerColor && noWinner) {
            if (playerColor == 'B')
                printf("Enter Black Move (ROW COL): ");
            else if (playerColor == 'W')
                printf("Enter White Move (ROW COL): ");
            scanf("%d %d", &row, &col);
            // Checks validity
            if (!(row == -1 && col == -1) && !(row < 0 || row >= n || col < 0 || col >= n)) {
                // Checks if blocked or taken
                if (!isOccupied(board, row, col)) {
                    board[row][col] = playerColor;
                    printBoard(board, n);
                    findWinner(board, n, &noWinner);
                    turn = (turn == 'B') ? 'W' : 'B'; // switch turns
                }
            }
            else if (!(row == -1 && col == -1)) {
                printf("Out of range row or column\n");
            }
        }
    }
    while (!(row == -1 && col == -1) && noWinner);
}

bool isOccupied(char board[21][21], int row, int col) {
    // Checks if blocked and prints statement if it is
    if (board[row][col] == 'B' || board[row][col] == 'R' || board[row][col] == 'W') {
        printf("That square is already occupied or blocked\n");
        return true;
    }
    else
        return false;
}

/*
 * Gives a square a score based on the potential of a given direction to make 6 in a row
 * Scoring follows these rules:
 * 1) If there is potential to make a chain of 6 in the given direction, the score will be set to a base of 1
 * 2) For every piece that's already contributing to creating 6 in a row, 1 will be added to the score
 * 3) If there is absolutely no potential for 6 in a row, 0 will be returned
 * 4) No more than 6 in either direction will be counted
 * This format/method applies to the next 3 functions
 */
int getVerticalScore(char board[21][21], int n, int row, int col, char color) {
    char pieceColor = color;
    char opposingColor = (pieceColor == 'B') ? 'W' : 'B'; 

    int verticalCounter = 1, potentialCounter = 1;
    int x, y, i; // x for up, y for down, i to make sure that the loop never runs longer than 6 times
    bool keepScanningUp = true;
    for (x = row-1, i = 0; x >= 0 && keepScanningUp && i < 5; x--, i++) { // Looks to the left
        if (board[x][col] == opposingColor || board[x][col] == 'R')
            keepScanningUp = false; // ends loop if it hits a blocked square
        else {
            potentialCounter++; // potential counter increases if a U is scanned
            if (board[x][col] == pieceColor)
                verticalCounter++;
        }
    }
    i = 0; // resets i
    bool keepScanningDown = true;
    for (y = row+1, i = 0; y < n && keepScanningDown && i < 5; y++, i++) { // Looks to the right
        if (board[y][col] == opposingColor || board[y][col] == 'R')
            keepScanningDown = false;
        else {
            potentialCounter++;
            if (board[y][col] == pieceColor)
                verticalCounter++;
        }
    }
    
    if (potentialCounter < 6) // if the vertical doesn't have potential for 6 in a row, return 0
        return 0;
    else if (findLongest(board, n, row, col, color) >= 6)
        return 7000; // arbitrarily high score in case this is a winning square
    else 
        return verticalCounter;
}

int getHorizontalScore(char board[21][21], int n, int row, int col, char color) {
    char pieceColor = color;
    char opposingColor = (pieceColor == 'B') ? 'W' : 'B'; 

    int horizontalCounter = 1, potentialCounter = 1;
    int i, j, q; // i for left, j for right, n to make sure that the loop never runs longer than 6 times
    bool keepScanningLeft = true;
    for (i = col-1, q = 0; i >= 0 && keepScanningLeft && q < 5; i--, q++) { // Looks to the left
        if (board[row][i] == opposingColor || board[row][i] == 'R')
            keepScanningLeft = false; // ends loop if it hits a blocked square
        else {
            potentialCounter++; // potential counter increases if a U is scanned
            if (board[row][i] == pieceColor)
                horizontalCounter++;
        }
    }
    q = 0; 
    bool keepScanningRight = true;
    for (j = col+1, q = 0; j < n && keepScanningRight && q < 5; j++, q++) { // Looks to the right
        if (board[row][j] == opposingColor || board[row][j] == 'R')
            keepScanningRight = false;
        else {
            potentialCounter++;
            if (board[row][j] == pieceColor)
                horizontalCounter++;
        }
    }
    
    if (potentialCounter < 6) // if the vertical doesn't have potential for 6 in a row, return 0
        return 0;
    else if (findLongest(board, n, row, col, color) >= 6)
        return 7000; // arbitrarily high score in case this is a winning square
    else
        return horizontalCounter;
}

int getDiagonalDownScore(char board[21][21], int n, int row, int col, char color) {
    char pieceColor = color;
    char opposingColor = (pieceColor == 'B') ? 'W' : 'B'; 
    
    int leftToRightDiagonalCounter = 1, potentialCounter = 1;
    // Looks down and to the right
    bool keepScanningDiagonalDownRight = true;
    int m, l, i; // m is row, l is column, i to keep the loops from running more than 6 times
    for (m = row+1, l = col+1, i = 0; m < n && l < n && keepScanningDiagonalDownRight && i < 5; m++, l++, i++) { // turnery operator tells program how many times to loop
        if (board[m][l] == opposingColor || board[m][l] == 'R')
            keepScanningDiagonalDownRight = false;
        else {
            potentialCounter++;
            if (board[m][l] == pieceColor)
                leftToRightDiagonalCounter++;
        }
    }
    i = 0;
    bool keepScanningDiagonalUpLeft = true;
    int h, k; // h is row, k is column
    for (h = row-1, k = col-1, i = 0; h >= 0 && k >= 0 && keepScanningDiagonalUpLeft && i < 5; h--, k--, i++) {
        if (board[h][k] == opposingColor || board[h][k] == 'R')
            keepScanningDiagonalUpLeft = false;
        else {
            potentialCounter++; 
            if (board[h][k] == pieceColor)
                leftToRightDiagonalCounter++;
        }
    }
    
    if (potentialCounter < 6) // if the vertical doesn't have potential for 6 in a row, return 0
        return 0;
    else if (findLongest(board, n, row, col, color) >= 6)
        return 7000; // arbitrarily high score in case this is a winning square
    else
        return leftToRightDiagonalCounter;
}

int getDiagonalUpScore(char board[21][21], int n, int row, int col, char color) {
    char pieceColor = color;
    char opposingColor = (pieceColor == 'B') ? 'W' : 'B'; 
    
    int rightToLeftDiagonalCounter = 1, potentialCounter = 1;
    // Looks down and to the right
    bool keepScanningDiagonalDownLeft = true;
    int q, w, i; // q is row, w is column, i to keep the loops from running more than 6 times
    for (q = row+1, w = col-1, i = 0; q < n && w >= 0 && keepScanningDiagonalDownLeft && i < 5; q++, w--, i++) { // turnery operator tells program how many times to loop
        if (board[q][w] == opposingColor || board[q][w] == 'R')
            keepScanningDiagonalDownLeft = false;
        else {
            potentialCounter++;
            if (board[q][w] == pieceColor)
                rightToLeftDiagonalCounter++;
        }
    }
    i = 0;
    bool keepScanningDiagonalUpRight = true;
    int r, t; // r is row, t is column
    for (r = row-1, t = col+1, i = 0; r >= 0 && t < n && keepScanningDiagonalUpRight && i < 5; r--, t++, i++) {
        if (board[r][t] == opposingColor || board[r][t] == 'R')
            keepScanningDiagonalUpRight = false;
        else {
            potentialCounter++;
            if (board[r][t] == pieceColor)
                rightToLeftDiagonalCounter++;
        }
    }
    
    if (potentialCounter < 6) // if the vertical doesn't have potential for 6 in a row, return 0
        return 0;
    else if (findLongest(board, n, row, col, color) >= 6)
        return 7000; // arbitrarily high score in case this is a winning square
    else
        return rightToLeftDiagonalCounter;
}

/*
 * returns the longest chain that can be made at a given spot of specified color
 */
int findLongest(char board[21][21], int n, int row, int col, char color) {
    // Declares variables that store the longest length in that direction
    int horizontal, vertical, diagonalLeftToRight, diagonalRightToLeft;
    //char pieceColor = occupiedBy(board, row, col);
    char pieceColor = color;
    
    /* Finds horizontal
     */
    int horizontalCounter = 1, i, j;
    // Looks to the left
    bool keepScanningLeft = true;
    for (i = col-1; i >= 0 && keepScanningLeft; i--) {
        if (board[row][i] == pieceColor)
            horizontalCounter++;
        else
            keepScanningLeft = false;
    }
    // Looks to the right
    bool keepScanningRight = true;
    for (j = col+1; j < n && keepScanningRight; j++) {
        if (board[row][j] == pieceColor)
            horizontalCounter++;
        else
            keepScanningRight = false;
    }
    
    /* Finds Vertical
     */
    int verticalCounter = 1, x, y;
    // Looks up
    bool keepScanningUp = true;
    for (x = row-1; x >= 0 && keepScanningUp; x--) {
        if (board[x][col] == pieceColor)
            verticalCounter++;
        else
            keepScanningUp = false;
    }
    // Looks to the right
    bool keepScanningDown = true;
    for (y = row+1; y < n && keepScanningDown; y++) {
        if (board[y][col] == pieceColor)
            verticalCounter++;
        else
            keepScanningDown = false;
    }
    
    /* Finds left to right diagonal
     */
    int leftToRightDiagonalCounter = 1;
    // Looks down and to the right
    bool keepScanningDiagonalDownRight = true;
    int m, l; // m is row, l is column
    for (m = row+1, l = col+1; m < n && l < n && keepScanningDiagonalDownRight; m++, l++) { // turnery operator tells program how many times to loop
        if (board[m][l] == pieceColor) 
            leftToRightDiagonalCounter++;
        else 
            keepScanningDiagonalDownRight = false;
    }
    bool keepScanningDiagonalUpLeft = true;
    int h, k; // h is row, k is column
    for (h = row-1, k = col-1; h >= 0 && k >= 0 && keepScanningDiagonalUpLeft; h--, k--) {
        if (board[h][k] == pieceColor) 
            leftToRightDiagonalCounter++;
        else 
            keepScanningDiagonalUpLeft = false;
    }
    
    /* Finds right to left diagonal
     */
    int rightToLeftDiagonalCounter = 1;
    // Looks down and to the left
    bool keepScanningDiagonalDownLeft = true;
    int q, w; // q is row, w is column
    for (q = row+1, w = col-1; q < n && w >= 0 && keepScanningDiagonalDownLeft; q++, w--) { // turnery operator tells program how many times to loop
        if (board[q][w] == pieceColor) 
            rightToLeftDiagonalCounter++;
        else 
            keepScanningDiagonalDownLeft = false;
    }
    bool keepScanningDiagonalUpRight = true;
    int r, t; // r is row, t is column
    for (r = row-1, t = col+1; r >= 0 && t < n && keepScanningDiagonalUpRight; r--, t++) {
        if (board[r][t] == pieceColor) 
            rightToLeftDiagonalCounter++;
        else
            keepScanningDiagonalUpRight = false;
    }
    
    // Returns largest variable
    return (int) fmax((double)horizontalCounter, fmax((double)verticalCounter, fmax((double)leftToRightDiagonalCounter, (double)rightToLeftDiagonalCounter)));
}

void measurePosition(char board[21][21], int n) {
    // Declares row and col variables
    int row, col;
    
    do {
        // Prompts for coordinate input
        printf("Enter position (ROW COL) of square to measure; (-1 -1) to finish: ");
        scanf("%d %d", &row, &col);
        if (!(row == -1 && col == -1)) {
            if (row < n && row >= 0 && col < n && col >= 0) {
                char color = 'B';
                printf("Score of %c at position (%d,%d) is %d\n", color, row, col, getVerticalScore(board, n, row, col, color));
            }
            else if (!(row == -1 && col == -1)) 
                printf("Out of range row or column\n");
        }
    }
    while (!(row == -1 && col == -1));
}

/*
 * Checks to see if there is a winner, and ends the game if there is
 * If there are no more unoccupied spaces left, then the game will end with a draw
 */
void findWinner(char board[21][21], int n, bool *noWinner) {
    // Scans through board to find longest values
    int l, longest = 0, emptyCounter = 0;
    char winner;
    for (l = 0; l < n; l++) { // Scan rows
        int k;
        for (k = 0; k < n; k++) { // Scan columns
            // if B or W && is the longest thus far
            if ((board[l][k] == 'B' || board[l][k] == 'W') && (findLongest(board, n, l, k, board[l][k]) > longest)) {
                longest = findLongest(board, n, l, k, board[l][k]);
                winner = board[l][k];
            }
            else if (board[l][k] == 'U')
                emptyCounter++;
        }
    }
    
    // prints winner depending on the situation
    if (winner == 'B' && longest >= 6) {
        printf("\nBlack player wins.\n");
        *noWinner = false;
    }
    else if (winner == 'W' && longest >=6) {
        printf("\nWhite player wins.\n");
        *noWinner = false;
    }
    else if (emptyCounter == 0) {
        printf("\nDraw!\n");
        *noWinner = false;
    }
}

/*
 * This function takes changes row and col variables into the coordinates with the highest score
 */
void findWinningScore(char board[21][21], int n, BlockScore scoreBoard[21][21], BlockScore scoreBoard2[21][21], int *row, int *col) {
    // nested loop to scan through the scoreBoard
    int row1, col1, row2, col2;
    int player1Largest = -1, player2Largest = -1;
    int i;
    for (i = 0; i < n; i++) {
        int j;
        for (j = 0; j < n; j++) {
            if (scoreBoard[i][j].totalScore > player1Largest && board[i][j] == 'U') {
                player1Largest = scoreBoard[i][j].totalScore;
                row1 = i;
                col1 = j;
            }
            if (scoreBoard2[i][j].totalScore > player2Largest && board[i][j] == 'U') {
                player2Largest = scoreBoard2[i][j].totalScore;
                row2 = i;
                col2 = j;
            }
        }
    }
    
    if (player2Largest >= player1Largest) { // assuming player 2 is the opponent
        *row = row2;
        *col = col2;
    }
    else {
        *row = row1;
        *col = col1;
    }
}

/*
 * returns true if a five long trap can be made at ab unoccupied spot
 * These 3 conditions must hold:
 * 1) a chain of 5 is made
 * 2) the score in the direction of the chain of 5 must be greater than 4 (5 is the minimum)
 * 3) both ends of the chain must be unoccupied
 */
bool fiveLongTrap(char board[21][21], int n, int row, int col, char color) {
    if (findLongest(board, n, row, col, color) == 5) {
        BlockScore score;
        int highest;
        score.vertical = getVerticalScore(board, n, row, col, color);
        score.horizontal = getHorizontalScore(board, n, row, col, color);
        score.diagonalDown = getDiagonalDownScore(board, n, row, col, color);
        score.diagonalUp = getDiagonalUpScore(board, n, row, col, color);
        highest = (int) fmax((double)score.vertical, fmax((double)score.horizontal,fmax((double)score.diagonalDown,(double)score.diagonalUp)));
        if (score.vertical == highest && score.vertical > 4) { // safety precaution: minimum score is 5
            int x, y;
            // Looks up
            bool keepScanningUp = true, firstEndUnoccupied = false, secondEndUnoccupied = false;
            for (x = row-1; x >= 0 && keepScanningUp; x--) {
                if (board[x][col] != color) {
                    keepScanningUp = false;
                    if (board[x][col] == 'U')
                        firstEndUnoccupied = true;
                }
            }
            // Looks down
            bool keepScanningDown = true;
            for (y = row+1; y < n && keepScanningDown; y++) {
                if (board[y][col] != color) {
                    keepScanningDown = false;
                    if (board[y][col] == 'U')
                        secondEndUnoccupied = true;
                }
            }
            return firstEndUnoccupied && secondEndUnoccupied; // both ends must be unoccupied for it to be a trap
        }
        else if (score.horizontal == highest && score.horizontal > 4) {
            int i, j;
            // Looks to the left
            bool keepScanningLeft = true, firstEndUnoccupied = false, secondEndUnoccupied = false;
            for (i = col-1; i >= 0 && keepScanningLeft; i--) {
                if (board[row][i] != color) {
                    keepScanningLeft = false;
                    if (board[row][i] == 'U')
                        firstEndUnoccupied = true;
                }
            }
            // Looks to the right
            bool keepScanningRight = true;
            for (j = col+1; j < n && keepScanningRight; j++) {
                if (board[row][j] != color) {
                    keepScanningRight = false;
                    if (board[row][j] == 'U')
                        secondEndUnoccupied = true;
                }
            }
            return firstEndUnoccupied && secondEndUnoccupied;
        }
        else if (score.diagonalDown == highest && score.diagonalDown > 4) {
            // Looks down and to the right
            bool keepScanningDiagonalDownRight = true, firstEndUnoccupied = false, secondEndUnoccupied = false;
            int m, l; // m is row, l is column
            for (m = row+1, l = col+1; m < n && l < n && keepScanningDiagonalDownRight; m++, l++) { // turnery operator tells program how many times to loop
                if (board[m][l] != color) {
                    keepScanningDiagonalDownRight = false;
                    if (board[m][l] == 'U') 
                        firstEndUnoccupied = true;
                }
            }
            bool keepScanningDiagonalUpLeft = true;
            int h, k; // h is row, k is column
            for (h = row-1, k = col-1; h >= 0 && k >= 0 && keepScanningDiagonalUpLeft; h--, k--) {
                if (board[h][k] != color) {
                    keepScanningDiagonalUpLeft = false;
                    if (board[h][k] == 'U')
                        secondEndUnoccupied = true;
                }
            }
            return firstEndUnoccupied && secondEndUnoccupied;
        }
        else if (score.diagonalUp == highest && score.diagonalUp > 4) {
            // Looks down and to the left
            bool keepScanningDiagonalDownLeft = true, firstEndUnoccupied = false, secondEndUnoccupied = false;
            int q, w; // q is row, w is column
            for (q = row+1, w = col-1; q < n && w >= 0 && keepScanningDiagonalDownLeft; q++, w--) { // turnery operator tells program how many times to loop
                if (board[q][w] != color) {
                    keepScanningDiagonalDownLeft = false;
                    if (board[q][w] == 'U')
                        firstEndUnoccupied = true;
                }
            }
            bool keepScanningDiagonalUpRight = true;
            int r, t; // r is row, t is column
            for (r = row-1, t = col+1; r >= 0 && t < n && keepScanningDiagonalUpRight; r--, t++) {
                if (board[r][t] != color) {
                    keepScanningDiagonalUpRight = false;
                    if (board[r][t] == 'U')
                        secondEndUnoccupied = true;
                }
            }
            return firstEndUnoccupied && secondEndUnoccupied;
        }
        else
            return false;
    }
    else
        return false;
}

/*
 * returns true if the square is surrounded by at least one of its own color
 */
bool surroundedByOwnColor(char board[21][21], int n, int row, int col, char color) {
    int counter = 0;
    // r for row, c for column
    int r;
    for (r = row-1; r <= row+1; r++) {
        if (r < 0 || r >= n) // ensures we don't go out of bounds
            continue;
        int c;
        for (c = col-1; c <= col+1; c++) {
            if (c < 0 || c >= n)
                continue;
            if (board[r][c] == color)
                counter++;
        }
    }
    if (counter > 0)
        return true;
    else
        return false;
}

/*
 * Function that adds to the scoreboard to promote production of traps
 */
void generalTrapMaker(char board[21][21], int n, int row, int col, BlockScore scoreBoard[21][21], char color) {
    char opponentColor = (color == 'B') ? 'W' : 'B';
    int counter = 0, incrementAmount = 1, threshold = 3; // counts the multiplicity of possible trap directions
    if (scoreBoard[row][col].vertical > 0)
        counter++;
    if (scoreBoard[row][col].horizontal > 0)
        counter++;
    if (scoreBoard[row][col].diagonalDown > 0)
        counter++;
    if (scoreBoard[row][col].diagonalUp > 0)
        counter++;
    if (counter >= 2) { // if a trap can be made around this block
        if (scoreBoard[row][col].vertical > threshold) {
            int x, y, i;
            // Looks up
            bool keepScanningUp = true;
            for (x = row-1, i = 0; i < 4 && x >= 0 && keepScanningUp; i++, x--) {
                if (board[x][col] == 'R' || board[x][col] == opponentColor)
                    keepScanningUp = false;
                else if (board[x][col] == 'U')
                    scoreBoard[x][col].totalScore += incrementAmount;
            }
            // Looks down
            i = 0;
            bool keepScanningDown = true;
            for (y = row+1, i = 0; i < 4 && y < n && keepScanningDown; i++, y++) {
                if (board[y][col] == 'R' || board[y][col] == opponentColor)
                    keepScanningDown = false;
                else if (board[y][col] == 'U')
                    scoreBoard[y][col].totalScore += incrementAmount;
            }
        }
        if (scoreBoard[row][col].horizontal > threshold) {
            int i, j, x;
            // Looks to the left
            bool keepScanningLeft = true;
            for (i = col-1, x = 0; x < 4 && i >= 0 && keepScanningLeft; x++, i--) {
                if (board[row][i] == 'R' || board[row][i] == opponentColor)
                    keepScanningLeft = false;
                else if (board[row][i] == 'U')
                    scoreBoard[row][i].totalScore += incrementAmount;
            }
            // Looks to the right
            x = 0;
            bool keepScanningRight = true;
            for (j = col+1, x = 0; x < 4 && j < n && keepScanningRight; x++, j++) {
                if (board[row][j] == 'R' || board[row][j] == opponentColor)
                    keepScanningRight = false;
                else if (board[row][j] == 'U')
                    scoreBoard[row][j].totalScore += incrementAmount;
            }
        }
        if (scoreBoard[row][col].diagonalDown > threshold) {
            // Looks down and to the right
            bool keepScanningDiagonalDownRight = true;
            int m, l, i; // m is row, l is column
            for (m = row+1, l = col+1, i = 0; i < 4 && m < n && l < n && keepScanningDiagonalDownRight; i++, m++, l++) { // turnery operator tells program how many times to loop
                if (board[m][l] == 'R' || board[m][l] == opponentColor)
                    keepScanningDiagonalDownRight = false;
                else if (board[m][l] == 'U') 
                    scoreBoard[m][l].totalScore += incrementAmount;
            }
            bool keepScanningDiagonalUpLeft = true;
            i = 0;
            int h, k; // h is row, k is column
            for (h = row-1, k = col-1, i = 0; i < 4 && h >= 0 && k >= 0 && keepScanningDiagonalUpLeft; i++, h--, k--) {
                if (board[h][k] == 'R' || board[h][k] == opponentColor) 
                    keepScanningDiagonalUpLeft = false;
                else if (board[h][k] == 'U')
                    scoreBoard[h][k].totalScore += incrementAmount;
            }
        }
        if (scoreBoard[row][col].diagonalUp > threshold) {
            // Looks down and to the left
            bool keepScanningDiagonalDownLeft = true;
            int q, w, i; // q is row, w is column
            for (q = row+1, w = col-1, i = 0; i < 4 && q < n && w >= 0 && keepScanningDiagonalDownLeft; i++, q++, w--) { // turnery operator tells program how many times to loop
                if (board[q][w] == 'R' || board[q][w] == opponentColor)
                    keepScanningDiagonalDownLeft = false;
                else if (board[q][w] == 'U')
                    scoreBoard[q][w].totalScore += incrementAmount;
            }
            bool keepScanningDiagonalUpRight = true;
            i = 0;
            int r, t; // r is row, t is column
            for (r = row-1, t = col+1, i = 0; i < 4 && r >= 0 && t < n && keepScanningDiagonalUpRight; i++, r--, t++) {
                if (board[r][t] == 'R' || board[r][t] == opponentColor)
                    keepScanningDiagonalUpRight = false;
                else if (board[r][t] == 'U')
                    scoreBoard[r][t].totalScore += incrementAmount;
            }
        }
    }
}

void scoreSquare(char board[21][21], int n, int row, int col, BlockScore scoreBoard[21][21], char color) {
    // initializes to 0
    scoreBoard[row][col].vertical = 0;
    scoreBoard[row][col].horizontal = 0;
    scoreBoard[row][col].diagonalDown = 0;
    scoreBoard[row][col].diagonalUp = 0;
    scoreBoard[row][col].totalScore = 0;
    
    scoreBoard[row][col].vertical += getVerticalScore(board, n, row, col, color);
    scoreBoard[row][col].horizontal += getHorizontalScore(board, n, row, col, color);
    scoreBoard[row][col].diagonalDown += getDiagonalDownScore(board, n, row, col, color);
    scoreBoard[row][col].diagonalUp += getDiagonalUpScore(board, n, row, col, color);
    scoreBoard[row][col].totalScore += scoreBoard[row][col].vertical + scoreBoard[row][col].horizontal + scoreBoard[row][col].diagonalUp + scoreBoard[row][col].diagonalDown;
    if (fiveLongTrap(board, n, row, col, color))
        scoreBoard[row][col].totalScore += 5000;
    if (surroundedByOwnColor(board, n, row, col, color))
        scoreBoard[row][col].totalScore++;
    
    //generalTrapMaker(board, n, row, col, scoreBoard, color);
}

void scoreBoards(char board[21][21], int n, BlockScore compScore[21][21], BlockScore opponentScore[21][21], char compColor, char opponentColor) {
    // Fills 2 score boards with base-decisions
    // i for rows, j for columns
    int i;
    for (i = 0; i < n; i++) {
        int j;
        for (j = 0; j < n; j++) {
            if (board[i][j] == 'U' && board[i][j] != 'B' && board[i][j] != 'W') {
                scoreSquare(board, n, i, j, compScore, compColor);
                scoreSquare(board, n, i, j, opponentScore, opponentColor);
            }
            else {
                compScore[i][j].totalScore = 0;
                opponentScore[i][j].totalScore = 0;
            }
        }
    }
    
    // adds half of the opponents scoreboard to the player's scoreboard
    // promotes blocking of opponent while producing own traps
    if (n > 6)
        secondaryAlterations(n, compScore, opponentScore);
}

int numMoves(char board[21][21], int n) {
    int counter = 0, row;
    for (row = 0; row < n; row++) {
        int col;
        for (col = 0; col < n; col++)
            if (board[row][col] == 'B' || board[row][col] == 'W')
                counter++;
    }
    return counter;
}

void secondaryAlterations(int n, BlockScore firstBoard[21][21], BlockScore secondBoard[21][21]) {
    BlockScore firstBoardTemp[21][21];
    int row;
    for (row = 0; row < n; row++) {
        int col;
        for (col = 0; col < n; col++) {
            // Makes a copy of the original first board
            firstBoardTemp[row][col].totalScore = firstBoard[row][col].totalScore;
        }
    }
    row = 0;
    for (row = 0; row < n; row++) {
        int col;
        for (col = 0; col < n; col++) {
            // Adds half of secondBoard to firstBoard
            firstBoard[row][col].totalScore += (int) (secondBoard[row][col].totalScore/2);
            // Adds half of firstBoard (the original version) to secondBoard
            secondBoard[row][col].totalScore += (int) (firstBoardTemp[row][col].totalScore/2);
        }
    }
}

void findTop3Moves(char board[21][21], int n, BlockScore scoreBoard[21][21], PotentialMove moves[3]) {
    moves[0].score = 0;
    moves[1].score = 0;
    moves[2].score = 0;
    // nested loop to scan through the scoreBoard. i for row, j for column
    int i;
    for (i = 0; i < n; i++) {
        int j;
        for (j = 0; j < n; j++) {
            if (scoreBoard[i][j].totalScore > moves[0].score) {
                moves[0].score = scoreBoard[i][j].totalScore;
                moves[0].row = i;
                moves[0].col = j;
            }
        }
    }
    i = 0;
    for (i = 0; i < n; i++) {
        int j;
        for (j = 0; j < n; j++) {
            if (scoreBoard[i][j].totalScore > moves[1].score && !(i == moves[0].row && j == moves[0].col)) {
                moves[1].score = scoreBoard[i][j].totalScore;
                moves[1].row = i;
                moves[1].col = j;
            }
        }
    }
    i = 0;
    for (i = 0; i < n; i++) {
        int j;
        for (j = 0; j < n; j++) {
            if (scoreBoard[i][j].totalScore > moves[1].score && !(i == moves[0].row && j == moves[0].col) && !(i == moves[1].row && j == moves[1].col)) {
                moves[2].score = scoreBoard[i][j].totalScore;
                moves[2].row = i;
                moves[2].col = j;
            }
        }
    }
}

void evaluateTop3Moves(char board[21][21], int n, BlockScore scoreBoard[21][21], char color, int *finalRow, int *finalCol) {
    char opponentColor = (color == 'B') ? 'W' : 'B';
    int largestScore = 0;
    
    PotentialMove moves[3];
    findTop3Moves(board, n, scoreBoard, moves);
    int i;
    for (i = 0; i < 3; i++) {
        board[moves[i].row][moves[i].col] = color;
        
        // Evaluates again, except the opponent is the "computer"
        BlockScore compScore[21][21], opponentScore[21][21];
        int rowComp, colComp, largestCompScore = -1, rowOpp, colOpp, largestOpponentScore = -1;
        scoreBoards(board, n, compScore, opponentScore, opponentColor, color);
        findWinningScore(board, compScore, n, &rowComp, &colComp, &largestCompScore);
        findWinningScore(board, opponentScore, n, &rowOpp, &colOpp, &largestOpponentScore);
        if (largestOpponentScore >= largestCompScore)
            moves[i].score += largestOpponentScore;
        else
            moves[i].score -= largestCompScore;
        board[moves[i].row][moves[i].col] = 'U';
    }
    
    for (i = 0; i < 3; i++) {
        if (moves[i].score > largestScore) {
            largestScore = moves[i].score;
            *finalRow = moves[i].row;
            *finalCol = moves[i].col;
        }
    }
}

void computerMove(char board[21][21], int n, char compColor) {
    BlockScore compScore[21][21], opponentScore[21][21]; // contains vertical, horizontal, diagonalDown, diagonalUp, totalScore
    char opponentColor = (compColor == 'B') ? 'W' : 'B';
    
    // Declares variables that store the location of the largest score of the comp and the player
    int finalRow, finalCol;
    
    // Fills each scoreboard and determines the block with the highest score
    scoreBoards(board, n, compScore, opponentScore, compColor, opponentColor);
    findWinningScore(board, n, compScore, opponentScore, &finalRow, &finalCol);
    
    // prints scoreboards for debugging purposes
    /* 
    printf("\n");
    int i;
    for (i = 0; i < n; i++) {
        int j;
        for (j = 0; j < n; j++)
            printf("%-2d", compScore[i][j].totalScore);
        printf("\n");
    }
    printf("\n");
    i=0;
    for (i = 0; i < n; i++) {
        int j;
        for (j = 0; j < n; j++)
            printf("%-2d", opponentScore[i][j].totalScore);
        // New line after each row
        printf("\n");
    } */
    
    // Makes its move
    board[finalRow][finalCol] = compColor;
    printf("Computer moves %c at %d %d\n", compColor, finalRow, finalCol);
    //printf("Comp: %d, Opp: %d\n", largestCompScore, largestOpponentScore);
    
    /*
    if (largestOpponentScore >= largestCompScore)
        evaluateTop3Moves(board, n, opponentScore, compColor, &finalRow, &finalCol);
    else
        evaluateTop3Moves(board, n, compScore, compColor, &finalRow, &finalCol);
    
    board[finalRow][finalCol] = compColor;
    printf("Computer moves %c at %d %d\n", compColor, finalRow, finalCol); */
    
    printBoard(board, n);
}