/* Tic Tac Toe with Lenny
 * Copyright (c) 2009,2010 Henry Brausen
 * This software is licensed under the BSD license: http://opensource.org/licenses/bsd-license.php
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define CONTINUE 0
#define WIN_X 1
#define WIN_O 2
#define DRAW 3

// Lenny will give the player friendly comments in-game
char *comments[] = {     "Good move!\n",
            "Wow, didn't see that one.\n",
            "Hey, you're good at this!\n",
            "Nice choice.\n",
            "This going to be a close game!\n",
			"You really have a talent for this.\n",
			"What a great move!\n",
			"You should do this for a living!\n",
			"You continue to surprise me.\n",
			"How did you get so good at this game?\n",
			"I'm so lucky to have an opponent as skilled as you!\n",
			"Interesting move! I wonder where it will lead . . .\n",
			"It's great to have a pal like you to play against.\n",
			"An excellent player! A most noble player!\n"};

// Please be nice to Lenny
char *nowImMad[] = {    "Invalid Input.\n",
                        "Again, that's invalid.\n",
                        "I'm losing my patience with you . . .\n",
                        "OK, that's it, I'm just going to play by myself!\n" };

// Structure for a node in the tree (representing a discreet possibility)
typedef struct node_s {
    int board[3][3];	// The gameboard
    int type;			// Is this an end-game situation, or a continuing game?
    int numBranches;    // How many possible games continue from here
    struct node_s *branches[9];	// Links to nodes representing continuing games
} node_t;

// Check if player inPlayer has won
int checkWin(node_t* inNode, const char inPlayer)
{
    int i;
    for(i = 0; i < 3; ++i) {
        if((inNode->board[0][i] == inPlayer)&&(inNode->board[1][i] == inPlayer)&&(inNode->board[2][i] == inPlayer)) {
            return 1;
        }
        if((inNode->board[i][0] == inPlayer)&&(inNode->board[i][1] == inPlayer)&&(inNode->board[i][2] == inPlayer)) {
            return 1;
        }
    }
    if((inNode->board[0][0] == inPlayer)&&(inNode->board[1][1] == inPlayer)&&(inNode->board[2][2] == inPlayer)) {
        return 1;
    }
    if((inNode->board[2][0] == inPlayer)&&(inNode->board[1][1] == inPlayer)&&(inNode->board[0][2] == inPlayer)) {
        return 1;
    }
    return 0;
}

// Check for a draw
int checkDraw(node_t* inNode)
{
    int i;
    int j;
    int count = 0;
    for(i = 0; i < 3; ++i) {
        for(j = 0; j < 3; ++j) {
            if(inNode->board[i][j] != ' ') {
                ++count;
            }
        }
    }
    if(count == 9) {
        return 1;
    }
    return 0;
}

// Recursively build the tree of nodes (possible game scenarios)
int buildSubtree(node_t *parent)
{
    if(checkWin(parent, 'x')) {
        parent->type = WIN_X;
        return 0;
    }
   
    if(checkWin(parent, 'o')) {
        parent->type = WIN_O;
        return 0;
    }
   
    if(checkDraw(parent)) {
        parent->type = DRAW;
        return 0;
    }
   
    parent->type = CONTINUE;
   
    int currentBranch = 0;
    int numX = 0;
    int numO = 0;
    char toPlace = ' ';
    int i;
    int j;
    int k;
    int m;
    node_t *child = NULL;
   
    for(i = 0; i < 3; ++i) {
        for(j = 0; j < 3; ++j) {
            if(parent->board[i][j] == 'x') {
                ++numX;
            } else if(parent->board[i][j] == 'o') {
                ++numO;
            }
        }
    }
   
    if(numX == numO) {
        toPlace = 'x';
    } else {
        toPlace = 'o';
    }
   
    for(i = 0; i < 3; ++i) {
        for(j = 0; j < 3; ++j) {
            if(parent->board[i][j] == ' ') {
                child = (node_t *)malloc(sizeof(node_t));
                for(k = 0; k < 9; ++k) {
                    child->branches[k] = NULL;
                }
                for(k = 0; k < 3; ++k) {
                    for(m = 0; m < 3; ++m) {
                        child->board[k][m] = parent->board[k][m];
                    }
                }
                child->board[i][j] = toPlace;
                child->numBranches = buildSubtree(child);
                parent->branches[currentBranch] = child;
                ++currentBranch;
            }
        }
    }
    return currentBranch;
}

// Deallocate memory for the tree
void freeTree(node_t *currentNode)
{
    if(currentNode->numBranches <= 0) {
        free(currentNode);
        return;
    } else {
        int i;
        for(i = 0; i < currentNode->numBranches; ++i) {
            freeTree(currentNode->branches[i]);
        }
        free(currentNode);
        return;
    }
}

// Print out a tree node (in short, display the game board)
void printNode(node_t *inNode)
{
    int i;
    int j;
    printf("Col   0 1 2\n");
    printf("     +-+-+-+\n");
    for(i = 0; i < 3; ++i) {
        printf("%s %d|",(i == 1)?("Row"):("   ") , i);
        for(j = 0; j < 3; ++j) {
            printf("%c|", inNode->board[i][j]);
        }
        printf("\n");
    }
    printf("     +-+-+-+\n");
}

// Find the value of a given move using the minimax algorithm
int minmax(bool player, node_t *currentNode)
{
    if(currentNode->type == WIN_O) {
        return -1;
    } else if(currentNode->type == WIN_X) {
		return 1;
	} else if(currentNode->type == DRAW) {
		return 0;
	}
    int m = minmax(!player, currentNode->branches[0]);
	int temp;
    int i;
    for(i = 1; i < currentNode->numBranches; ++i) {
        temp = minmax(!player, currentNode->branches[i]);
		if(!player) {
			if(temp < m) m = temp;
		} else {
			if(temp > m) m = temp;
		}

    }
    return m;
}

// Choose the best next move, and return a pointer to that move in the tree
node_t *bestBranch(node_t *currentNode)
{   
    int minCount = minmax(true, currentNode->branches[0]);
    int tempCount = 0;
    int bestIndex = 0;
    int i;
    for(i = 1; i < currentNode->numBranches; ++i) {
        tempCount = minmax(true, currentNode->branches[i]);
        if(tempCount < minCount) {
            minCount = tempCount;
            bestIndex = i;
        }
    }
    return currentNode->branches[bestIndex];
}

// Check if two boards are equal
int compareBoards(int b1[3][3], int b2[3][3])
{
    int i;
    int j;
    int same = 1;
    for(i = 0; i < 3; ++i) {
        for(j = 0; j < 3; ++j) {
            if(b1[i][j] != b2[i][j]) {
                same = 0;
            }
        }
    }
    return same;
}

// Creepy typing effect!
void type(const char* str, int d)
{
	int lim = strlen(str);
	for(int i = 0; i < lim; ++i) {
		_sleep(d);
		putch(str[i]);
	}
	return;
}

// Main program
int main(int argc, char** argv)
{
    type("Hi, I'm Lenny v2.0! Let's play Tic Tac Toe.\n", 40);
    type("Please wait as I do some calculations . . .\n", 40);
	type(". . .\n", 200);
	type("\nActually, I'll tell you what: ", 60);
	_sleep(200);
	type("\nOnce you beat me, there may or may not be cake.\nWhat do you say?\n", 40);
   
    // Allocate memory for root node of tree
    node_t *root = (node_t *)malloc(sizeof(node_t));
   
    // Pointer to current position in the tree
    node_t *next;
   
    // Initialize root node (clear it)
    int i;
    int j;
    for(i = 0; i < 3; ++i) {
        for(j = 0; j < 3; ++j) {
            root->board[i][j] = ' ';
        }
    }
   
    // User input variables and related variables
    int inRow = 0;
    int inCol = 0;
    int tempBoard[3][3];

    int invalidCount = 0;
   
    // Build our game tree
    root->numBranches = buildSubtree(root);
   
    type("\nThere, all finished. Let's play.\n", 60);

    // Position in comments array :)
    int commentCount;
   
    // Play again?
    char userConfirm = ' ';
    int isDone = 0;
   
    int inputCheck = 0;

	// Lenny's a good sport!
	commentCount = 0;

    while(!isDone) {
        next = root;
        invalidCount = 0;
       
        // Print the current (blank) board
		putch('\n');
        printNode(next);
       
        // Main game loop.
        while(!checkWin(next, 'x') && !checkWin(next, 'o') && !checkDraw(next)) {
            for(i = 0; i < 3; ++i) {
                for(j = 0; j < 3; ++j) {
                    tempBoard[i][j] = next->board[i][j];
                }
            }
            if(invalidCount < 3) {
            while(1) {
                fflush(stdin);
                printf("Enter row #: ");
                if(!scanf_s("%d", &inRow)) {
                    fflush(stdin);
                    printf("%s", nowImMad[invalidCount++]);
                    if(invalidCount > 3) break;
                    continue;
                }
                fflush(stdin);
                printf("Enter col #: ");
                if(!scanf_s("%d", &inCol)) {
                    fflush(stdin);
                    printf("%s", nowImMad[invalidCount++]);
                    if(invalidCount > 3) break;
                    continue;
                }
                if(!((inRow >= 0)&&(inRow <= 2)&&(inCol >=0)&&(inCol <= 2))) {
                    printf("Oops! That space doesn't exist! Please try again.\n");
                    inRow = 0;
                    continue;
                }
                if(tempBoard[inRow][inCol] != ' ') {
                    printf("Oops! That space is already occupied! Please try again.\n");
                    continue;
                }
                break;
            }
            }
			putch('\n');
            if(invalidCount >= 4) {
                _sleep(1000);
            }
            if(invalidCount == 4) {
                invalidCount = 10;
                next = bestBranch(next);
                continue;
            } else {
                tempBoard[inRow][inCol] = 'x';
            }
           
            for(i = 0; i < next->numBranches; ++i) {
                if(compareBoards(tempBoard, next->branches[i]->board)) {
                    next = next->branches[i];
                }
            }
            if(commentCount > 12) { commentCount = 0; }
            type(comments[commentCount++], 30);
			_sleep(100);
			putch('\n');
           
            if(checkWin(next, 'x')) {
                printNode(next);
                type("\nYou win . . .\n", 80);
				type("Umm . . . ow, my feelings. No cake for you, friend.\n", 60);
                break;
            }
           
            if(checkDraw(next)) {
                printNode(next);
                type("\nTie game!\n", 80);
                break;
            }
           
            next = bestBranch(next);
            printNode(next);
           
            if(checkWin(next, 'o')) {
                type("\nI win! :)\n", 80);
                break;
            }
        }
        type("\nThat was fun! Want to play again? (y/n) :", 30);
        while((userConfirm = getchar()) != EOF && userConfirm == '\n');
        if(toupper(userConfirm) == 'Y') {
            type("Cool! Let's get started! You go first.\n", 30);
            continue;
        } else {
            type("\nOkay then. Goodbye!\n", 30);
            break;
        }
    }
    freeTree(root);
    return 0;
}
