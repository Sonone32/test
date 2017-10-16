#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char *incrementOperators(char *);
char *equivalent(char *, char *, const int);

char *findArithmetic(char *numbers, const int target) {
    const int length = strlen(numbers);
    unsigned long int i;
    const unsigned long int bound = pow(3, length);
    /* Space for terminating \0 included in operations. */
    char operations[length], *parsed;

    for (i = 0; i < length - 1; i++) {
        operations[i] = ' ';
    }
    operations[length - 1] = '\0';

    for (i = 0; i < bound; i++) {
        if (parsed = equivalent(numbers, operations, target)) {
            printf("\"%s\",%d -> \"%s\"\n", numbers, target, parsed);
            return parsed;
        }
        incrementOperators(operations);
    }

    printf("no solution\n");
    return "no solution";
}

int main() {
    char *str = "1231231234";
    findArithmetic(str, 11353);
    return 0;
}

struct MathNode {
    char *val;
    struct MathNode *prev;
    struct MathNode *next;
};

int evalHelper(struct MathNode head, size_t maxLength) {
    struct MathNode *temp, *current = head.next;
    int num;
    char *newNum;

    /* Evaluate all the multiplications. */
    while (current) {
        if (current->val[0] == '*') {
            /* current is a multiplication operator */
            num = atoi(current->prev->val) * atoi(current->next->val);

            free(current->val);
            free(current->prev->val);
            free(current->next->val);
            
            temp = current->prev->prev;
            temp->next = current;
            free(current->prev);
            current->prev = temp;

            temp = current->next->next;
            if (temp) temp->prev = current;
            free(current->next);
            current->next = temp;

            newNum = malloc(maxLength + 1);
            sprintf(newNum, "%d", num);
            current->val = newNum;
        }

        current = current->next;
    }

    /* Evaluate all the pluses. */
    current = head.next;
    while (current) {
        if (current->val[0] == '+') {
            /* current is a plus sign */
            num = atoi(current->prev->val) + atoi(current->next->val);

            free(current->val);
            free(current->prev->val);
            free(current->next->val);
            
            temp = current->prev->prev;
            temp->next = current;
            free(current->prev);
            current->prev = temp;

            temp = current->next->next;
            if (temp) temp->prev = current;
            free(current->next);
            current->next = temp;

            newNum = malloc(maxLength + 1);
            sprintf(newNum, "%d", num);
            current->val = newNum;
        }

        current = current->next;
    }

    return atoi(head.next->val);
}

char *copyAnswer(char *answer, struct MathNode head) {
    struct MathNode *temp = head.next;
    answer[0] = '\0';

    while (temp) {
        strcat(answer, temp->val);
        temp = temp->next;
    }

    return answer;
}

/* Return value needs to be dynamically allocated.
 */
char *equivalent(char *nums, char *ops, const int target) {
    size_t i, opsLength = strlen(ops), numsLength = strlen(nums), tokLength = 1;
    char *temp, *answer = malloc(numsLength * 2), *currentNum =  nums;
    struct MathNode head, *current = &head, *new;

    for (i = 1; i < numsLength; i++) {
        switch(ops[i - 1]) {
            case ' ':
                tokLength += 1;
                break;;
            default:
                /* Make a node for a number and append it to list.*/
                new = malloc(sizeof(struct MathNode));
                new->prev = current;
                current->next = new;
                temp = malloc(sizeof(tokLength + 1));
                strncpy(temp, currentNum, tokLength);
                new->val = temp;
                current = new;

                currentNum += tokLength;
                tokLength = 0;
                
                /* Make a node for operator and append it to list.*/
                new = malloc(sizeof(struct MathNode));
                new->prev = current;
                current->next = new;
                temp = malloc(2);
                if (ops[i - 1] == '+') {
                    strncpy(temp, "+", 1);
                } else {
                    strncpy(temp, "*", 1);
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
    temp = malloc(sizeof(tokLength + 1));
    strncpy(temp, currentNum, tokLength);
    new->val = temp;

    copyAnswer(answer, head);
    printf("ops is :  |%s|\n", ops);
    printf("answer is:|%s|\n", answer);
    if (evalHelper(head, numsLength) == target) {
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
    size_t i = strlen(ops) - 1;

    while (1) {
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


