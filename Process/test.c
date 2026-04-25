#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>

int main(void) {
    char vetor[] = "Ola mundo";
    pid_t pid;
    for(int i = 0; i < 2; i++) {
        pid = fork();
    }
    if(pid == 0) {
        printf("%s do processo pai\n", vetor);
    } else {
        printf("%s do processo filho de ID: %d\n", vetor, getpid());
    }
    return 0;
}
