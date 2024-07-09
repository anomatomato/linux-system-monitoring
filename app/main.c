#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>


void fill_memory(){
        int i = 0;
        int *array[100];
        while (i < 100) {
                int *memory = malloc(sizeof(int));
                array[i] = memory;
                i++;
        }
        for (int i = 0; i < 100; i++) {
                free(array[i]);
        }
}

void calc_pi() {
        long int n = 1000000000, i;
        double sum=0.0, term, pi;

        /* Applying Leibniz Formula */
        for (i=0; i < n; i++) {
                term = pow(-1, i) / (2*i+1);
                sum += term;
        }
        pi = 4 * sum;
        printf("%f\n", pi);
}

int main() {
        int j;
        //while (j < 20) {
                //pid_t p = fork();
                //if(p < 0) {
                        //perror("fork fail");
                        //break;
                //}
                //else if (p == 0) {
                        //printf("Hello from Child!\n");
                        //fill_memory();
                        //continue;
                //}
                //else {
                        //printf("Hello from Parent!\n");
                        //calc_pi();
                        //wait();
                        //break;
                //}
        //}

        for (j = 0; j < 20; j++) {
                pid_t p = fork();
                if(p < 0) {
                        perror("fork fail");
                        break;
                }
                else if (p == 0) {
                        printf("Hello from Child!\n");
                        fill_memory();
                        _exit(0);
                }
                else {
                        printf("Hello from Parent!\n");
                        calc_pi();
                        wait(NULL);
                        break;
                }
        }
}
