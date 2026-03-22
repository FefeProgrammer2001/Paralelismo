#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int id;
    int burst_time;
    int waiting_time;
    int turnaround_time;
} Processo;

void ordenarPorBurst(Processo proc[], int n) {
    for(int i = 0; i < n - 1; i++) {
        int menor = i;
        for(int j = i + 1; j < n; j++) {
            if(proc[j].burst_time < proc[menor].burst_time)
                menor = j;
        }
        if(menor != i) {
            Processo temp = proc[i];
            proc[i] = proc[menor];
            proc[menor] = temp;
        }
    }
}

void calcularTempo(Processo proc[], int n) {
    proc[0].waiting_time = 0;
    for(int i = 1; i < n; i++)
        proc[i].waiting_time = proc[i-1].waiting_time + proc[i-1].burst_time;

    for(int i = 0; i < n; i++)
        proc[i].turnaround_time = proc[i].waiting_time + proc[i].burst_time;
}

void exibirResultados(Processo proc[], int n) {
    float total_wt = 0, total_tat = 0;

    printf("\n╔══════════╦══════════════╦══════════════╦═══════════════════╗\n");
    printf("║ Processo ║ Burst Time   ║ Waiting Time ║ Turnaround Time   ║\n");
    printf("╠══════════╬══════════════╬══════════════╬═══════════════════╣\n");

    for(int i = 0; i < n; i++) {
        printf("║  P%-7d║  %-12d║  %-12d║  %-17d║\n",
                proc[i].id,
                proc[i].burst_time,
                proc[i].waiting_time,
                proc[i].turnaround_time);
        total_wt += proc[i].waiting_time;
        total_tat += proc[i].turnaround_time;
    }

    printf("╚══════════╩══════════════╩══════════════╩═══════════════════╝\n");
    printf("\n  Tempo médio de espera     (AWT): %.2f\n", total_wt  / n);
    printf("  Tempo médio de retorno    (ATAT): %.2f\n", total_tat / n);
}

void exibirGantt(Processo proc[], int n) {
    printf("\n  Diagrama de Gantt:\n  |");
    for(int i = 0; i < n; i++)
        printf(" P%-2d |", proc[i].id);

    printf("\n  0");
    int tempo = 0;
    for(int i = 0; i < n; i++) {
        tempo += proc[i].burst_time;
        printf("    %2d", tempo);
    }
    printf("\n");
}

int main() {
    int n;

    srand((unsigned int)time(NULL));

    printf("╔══════════════════════════════════════╗\n");
    printf("║   Simulador SJF - Shortest Job First  ║\n");
    printf("╚══════════════════════════════════════╝\n\n");

    printf("  Informe a quantidade de processos: ");
    scanf("%d", &n);

    if(n <= 0) {
        printf("  Erro: quantidade de processos deve ser maior que zero\n");
        return 1;
    }

    Processo proc[n];

    printf("\n  Processos gerados:\n");
    for(int i = 0; i < n; i++) {
        proc[i].id = i + 1;
        proc[i].burst_time = (rand() % 20) + 1;
        proc[i].waiting_time = 0;
        proc[i].turnaround_time = 0;
        printf("  P%d -> Burst Time: %d\n", proc[i].id, proc[i].burst_time);
    }

    ordenarPorBurst(proc, n);
    calcularTempo(proc, n);

    printf("\n  Ordem de execucao (SJF):\n");
    exibirResultados(proc, n);
    exibirGantt(proc, n);

    return 0;
}