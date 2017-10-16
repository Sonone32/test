/* Tom Hsieh tom@ohsweetcherry.com | github.com/Sonone32
 * This took way longer than I had expected and the allotted time because I am a bit rusty on ANSI C,
 * and also because this was written 100% via a terminal on Google Cloud with vim and gdb. Hope my
 * persistence means something... And thank you for taking the effort to review my code, have a great
 * day or night. :)
 * Compile with the command "gcc -ansi main.c -lm".
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char *incrementOperators(char *);
char *equivalent(char *, char *, const int);
/* Function for problem one. I wasn't sure if I was supposed to output to stdout
 * or return a char array, so if the user wants a char array simply return the
 * variable parsed or NULL for no solution.
 */
void findArithmetic(char *numbers, const int target) {
    const int length = strlen(numbers);
    unsigned long int i;
    const unsigned long int bound = pow(3, length - 1);
    char operations[length], *parsed;

    for (i = 0; i < length - 1; i++) {
        operations[i] = ' ';
    }
    operations[length - 1] = '\0';

    for (i = 0; i < bound; i++) {
        if (parsed = equivalent(numbers, operations, target)) {
            printf("\"%s\",%d -> \"%s\"\n", numbers, target, parsed);
            free(parsed);
            return;
        }
        incrementOperators(operations);
    }

    printf("no solution\n");
}

int glueTile(char *, char *, char *, int, int);
void printFloor(char *, int);
/* Function for problem two.
 */
void link(char **tiles) {
    int i, rowSize, deg, tileCount;
    char *floor;

    for (i = 0; tiles[i] != 0; i++);
    tileCount = (i + 1) / 3;
	printf("%d tiles\n", tileCount);

    rowSize = (i + 1) / 3 * 2 + 4;
    floor = malloc(rowSize * rowSize);
    for(i = 0; i < rowSize * rowSize; i++) floor[i] = ' ';

    for (i = 0; i < tileCount * 3;) {
       deg = glueTile(floor, tiles[i], tiles[i + 1], i / 3 + 1, rowSize);
       i += 3;

       printf("link %d", i / 3);
       if (deg != 0) printf(" // note this tile had to be rotated %d degrees clockwise", deg);
       printf("\n");

       printFloor(floor, rowSize);
    }
    free(floor);
}

int main() {
    /* Examples for problem 1.
    char ex1[11] = "1231231234";
    char ex2[11] = "3456237490";
    char ex3[11] = "3456237490";
    char op[10] = " *++* +* ";
    findArithmetic(ex1, 11353);
    findArithmetic(ex2, 1185);
    findArithmetic(ex3, 9191);
    */

    char *tiles1[] = {
		"11",     /* 1 */
		"23",
		"",
		"44",     /* 2 */
		"11",
		"",
		"16",     /* 3 */
		"36",
		"",
		"51",     /* 4 */
		"71",
		"",
		"46",     /* 5 */
		"26",
		"",
		"14",     /* 6 */
		"68",
    0};

	char *tiles2[] = {
		"12",   
		"34",
		"",
		"36",   
		"15",
		"",
		"56",   
		"78",
		"",
		"78",  
		"90",
		"",
		"46", 
		"26",
		"",
		"88",  
		"01",
		"",
		"93", 
		"54",
    0};

	char *tiles3[] = {
		"11",     
		"12",
		"",
		"12",     
		"93",
		"",
		"93",    
		"99",
		"",
		"39",   
		"98",
		"",
		"95",
		"86",
		"",
		"57",
		"66",
		"",
		"77",
		"57",
		"",
		"13",  
		"24",
		"",
		"45",  
		"47",
		"",
		"24", 
		"39",
    0};
    /*
	link(tiles1);
	link(tiles2);
	link(tiles3);
    */
    return 0;
}

/*
 * ==========================PROBLEM ONE==========================
 */
struct MathNode {
    char *val;
    struct MathNode *prev;
    struct MathNode *next;
};

char *copyAnswer(char *answer, struct MathNode head) {
    struct MathNode *temp = head.next;
    answer[0] = '\0';

    while (temp) {
        strcat(answer, temp->val);
        temp = temp->next;
    }

    return answer;
}

/* At first I passed head by value so the prev of second node did not
 * refer back to head, which took hours to see. It wasted a lot of time,
 * hence the really late submission. -10.19pm PDT, 10/15/2017
 */
int evalHelper(struct MathNode *head, size_t maxLength) {
    struct MathNode *temp, *current = head->next;
    int num;
    char *newNum;

    /* Evaluate all the multiplications. */
    while (current) {
        if (current->val[0] == '*') {
            /* current is a multiplication operator */
            num = atoi(current->prev->val) * atoi(current->next->val);

            free(current->val);
            
            temp = current->prev->prev;
            temp->next = current;
            free(current->prev->val);
            free(current->prev);
            current->prev = temp;

            temp = current->next->next;
            if (temp) temp->prev = current;
            free(current->next->val);
            free(current->next);
            current->next = temp;

            newNum = malloc(maxLength + 1);
            sprintf(newNum, "%d", num);
            current->val = newNum;
        }

        current = current->next;
    }

    /* Evaluate all the pluses. */
    current = head->next;
    while (current) {
        if (current->val[0] == '+') {
            /* current is a plus sign */
            num = atoi(current->prev->val) + atoi(current->next->val);

            free(current->val);
            
            temp = current->prev->prev;
            temp->next = current;
            free(current->prev->val);
            free(current->prev);
            current->prev = temp;

            temp = current->next->next;
            if (temp) temp->prev = current;
            free(current->next->val);
            free(current->next);
            current->next = temp;

            newNum = malloc(maxLength + 1);
            sprintf(newNum, "%d", num);
            current->val = newNum;
        }
    
        current = current->next;
    }

    return atoi(head->next->val);
}

/* Return value needs to be dynamically allocated.
 */
char *equivalent(char *nums, char *ops, const int target) {
    size_t i, numsLength = strlen(nums), tokLength = 1;
    char *temp, *answer = malloc(numsLength * 2), *currentNum =  nums;
    struct MathNode head, *current = &head, *new;
    int ans;

    for (i = 1; i < numsLength; i++) {
        switch(ops[i - 1]) {
            case ' ':
                tokLength += 1;
                continue;
            default:
                /* Make a node for a number and append it to list.*/
                new = malloc(sizeof(struct MathNode));
                new->prev = current;
                current->next = new;
                temp = malloc(sizeof(tokLength + 1));
                memcpy(temp, currentNum, tokLength);
                temp[tokLength] = '\0';
                new->val = temp;
                current = new;

                currentNum += tokLength;
                tokLength = 1;
                
                /* Make a node for operator and append it to list.*/
                new = malloc(sizeof(struct MathNode));
                new->prev = current;
                current->next = new;
                temp = malloc(2);
                if (ops[i - 1] == '+') {
                    memcpy(temp, "+\0", 2);
                } else {
                    memcpy(temp, "*\0", 2);
                }
                new->val = temp;
                current = new;
        }
    }

    /* Push the tail of numbers to the list as well. */
    new = malloc(sizeof(struct MathNode));
    new->prev = current;
    current->next = new;
    new->next = NULL;
    temp = malloc(sizeof(numsLength + 1));
    strcpy(temp, currentNum);
    new->val = temp;

    copyAnswer(answer, head);
    ans = evalHelper(&head, numsLength);
    if (ans == target) {
        /* concat nodes to answer and then free list */
        free(head.next->val);
        free(head.next);
        return answer;
    } else {
        free(answer);
        free(head.next->val);
        free(head.next);
        return NULL;
    }
}

/* Expects ops to be the same in length before and after the call.
 * No extra error checking is done.
 */
char *incrementOperators(char *ops) {
    int i = strlen(ops) - 1;

    while (i >= 0) {
        switch(ops[i]) {
            case ' ':
                ops[i] = '+';
                return ops;
            case '+':
                ops[i] = '*';
                return ops;
            case '*':
                ops[i] = ' ';
                i -= 1;
        }
    }
}
/*
 * ==========================END PROBLEM ONE==========================
 */

/*
 * ==========================PROBLEM TWO==========================
 */
void rotateBy90(char block[4]) {
    char temp = block[3];
    int i;

    for (i = 3; i > 0; i--) block[i] = block[i-1];
    block[0] = temp;
}

/* Floor should be shifted when it comes in here. */
_Bool layBlock(char *floor, char *block, int rowSize, int numBlocks) {
    int i, j, matches, offset, bound = numBlocks * 2 + 2;
    _Bool matched = 0;

    for (i = 0; i < bound; i++) {
        for (j = 0; j < bound; j++) {
            matches = 0;
            if ((floor[i * rowSize + j] < 48)
                && (floor[i * rowSize + j + 1] < 48)
                && (floor[(i + 1) * rowSize + j] < 48)
                && (floor[(i + 1) * rowSize + j + 1] < 48)) {
                /* The block is in an available empty slot. */

                if (i >= 2) {
                    /* Only check matches at the top if it's feasible. */
                    if (block[0] == floor[(i - 1) * rowSize + j]) {
                        /* Top left corner to top-left. */
                        matches++;
                    } else if (floor[(i - 1) * rowSize + j] >= 48) {
                        /* Mismatch */
                        continue;
                    }

                    if (block[1] == floor[(i - 1) * rowSize + j + 1]) {
                        /* Top right corner to top-right. */
                        matches++;
                    } else if (floor[(i - 1) * rowSize + j + 1] >= 48) {
                        /* Mismatch */
                        continue;
                    }
                }

                if (j >= 2) {
                    /* Only check matches to the left if it's feasible. */
                    if (block[0] == floor[i * rowSize + j - 1]) {
                        /* Top left corner to left-top. */
                        matches++;
                    } else if (floor[i * rowSize + j - 1] >= 48) {
                        /* Mismatch */
                        continue;
                    }

                    if (block[3] == floor[(i + 1) * rowSize + j - 1]) {
                        /* Bottom left corner to left-bottom. */
                        matches++;
                    } else if(floor[(i + 1) * rowSize + j - 1] >= 48) {
                        /* Mismatch */
                        continue;
                    }
                }

                if (block[1] == floor[i * rowSize + j + 2]) {
                    /* Top right corner to right-top. */
                    matches++;
                } else if (floor[i * rowSize + j + 2] >= 48) {
                    /* Mismatch */
                    continue;
                }

                if (block[2] == floor[(i + 1) * rowSize + j + 2]) {
                    /* Bottom right corner to right-bottom. */
                    matches++;
                } else if(floor[(i + 1) * rowSize + j + 2] >= 48) {
                    /* Mismatch */
                    continue;
                }

                if (block[3] == floor[(i + 2) * rowSize + j]) {
                    /* Bottom left corner to bottom-left. */
                    matches++;
                } else if (floor[(i + 2) * rowSize + j] >= 48) {
                    /* Mismatch */
                    continue;
                }

                if (block[2] == floor[(i + 2) * rowSize + j + 1]) {
                    /* Bottom right corner to bottom-right. */
                    matches++;
                } else if(floor[(i + 2) * rowSize + j + 1] >= 48) {
                    /* Mismatch */
                    continue;
                }
                
                if (matches >= 2) {
                    /* Block can sit here legally. */
                    matched = 1;
                    break;
                }
            }
        }
        if (matched) break;
    }

    if (matched) {
        /* Lay the block down and shift floor back. */
        floor[i * rowSize + j] = block[0];
        floor[i * rowSize + j + 1] = block[1];
        floor[(i + 1) * rowSize + j + 1] = block[2];
        floor[(i + 1) * rowSize + j] = block[3];

        offset = 0;
        if (i == 1) offset += rowSize;
        if (i >= 2) offset += 2 * rowSize;

        if (j == 1) offset += 1;
        if (j >= 2) offset += 2;
        
        for(i = 0; i + offset < rowSize * rowSize - 1; i++) floor[i] = floor[i + offset];
        return 1; 
    };
    return 0;
}

void shiftFloor(char *floor, int rowSize, int numBlocks) {
    int i, j;

    /* Shift toward bottom-right by 2 char */
    for (i = numBlocks * 2; i >= 0; i--) {
        for (j = numBlocks * 2; j >= 0 ; j--) {
            floor[rowSize * (i + 2) + j + 2] = floor[rowSize * i + j];
            floor[rowSize * i + j] = ' ';
        }
    }
}

int glueTile(char *floor, char top[2], char bot[2], int numBlocks, int rowSize) {
    int i = 0;
    char block[4] = {top[0], top[1], bot[1], bot[0]};
    /* Block looks like:
     *      01
     *      32
     */

    if (numBlocks == 1) {
        memcpy(floor, top, 2);
        memcpy(floor + rowSize, bot, 2);
        return 0;
    }

    shiftFloor(floor, rowSize, numBlocks);
    do {
        if (layBlock(floor, block, rowSize, numBlocks)) break;
        rotateBy90(block);
        i++;
    } while (i < 4);

    return 90 * i;
}

void printFloor(char *floor, int rowSize) {
    int i, j;
    _Bool flag;
    char buffer[rowSize];

    for (i = 0; i < rowSize; i++) {
        flag = 0;
        for (j = 0; j < rowSize; j++) {
            buffer[j] = floor[j + i * rowSize];
            if (buffer[j] >= 48) flag = 1;
        }

        if (flag) {
            buffer[rowSize - 1] = '\0';
            printf("%s\n", buffer);
            buffer[0] = '\0';
        } else {
            break;
        }
    }

    printf("\n");
}

/*
 * ==========================END PROBLEM ONE==========================
 */
