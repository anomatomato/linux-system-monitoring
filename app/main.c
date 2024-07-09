#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct Mem {
    int *waste;
    struct Mem *next;
} Mem_t;

void fill_memory(){
        Mem_t *head = NULL;
        for (int i = 0; i < 100; i++) {
                Mem_t *node = (Mem_t *) malloc(sizeof(Mem_t));
                node->waste = (int*) malloc(sizeof(int));

                if (head == NULL)
                        head = node;
                else {
                        Mem_t *current = head;
                        while (current->next != NULL) {
                                current = current->next;
                        }
                        current->next = node;
                }

        }
        Mem_t *current;

        while (head != NULL) {
                current = head;
                head = head->next;
                free(current->waste);
                free(current);
        }
}

void calc_pi() {
        long int n = 100000000, i;
        double sum=0.0, term, pi;

        /* Applying Leibniz Formula */
        for (i=0; i < n; i++) {
                term = pow(-1, i) / (2*i+1);
                sum += term;
        }
        pi = 4 * sum;
        printf("%f\n", pi);
}

void *forking() {
        int j = 0;
        while (j < 20) {
                pid_t p = fork();
                if(p < 0) {
                        perror("fork fail");
                        break;
                }
                else if (p == 0) {
                        printf("Hello from Child!\n");
                        fill_memory();
                        j++;
                        _exit(0);
                }
                else {
                        printf("Hello from Parent!\n");
                        calc_pi();
                        wait(NULL);
                        j++;
                }
        }
        exit(0);
}

int main() {
        int g = 0;
        while (g < 20) {
                forking();
                g++;
        }
}
