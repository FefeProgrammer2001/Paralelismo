#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "hash_table.h"

static char* extract_url(char *line) {
    char *p = strchr(line, ' ');
    if (p) return p + 1;
    return line;
}

static void load_manifest(HashTable *ht, const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) { perror("manifest"); exit(1); }

    char buf[8192];
    while (fgets(buf, sizeof(buf), fp)) {
        buf[strcspn(buf, "\r\n")] = 0;
        if (buf[0] != '\0')
            ht_put(ht, buf);
    }
    fclose(fp);
}

// --------- Versão Paralela com CRITICAL ---------
static void process_par_critical(HashTable *ht, const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) { perror("access_log"); exit(1); }

    // 1) carregar tudo na memória
    char **lines = NULL;
    size_t count = 0;
    char buf[8192];

    while (fgets(buf, sizeof(buf), fp)) {
        lines = realloc(lines, (count+1)*sizeof(char*));
        lines[count] = strdup(buf);
        count++;
    }
    fclose(fp);

    // 2) paralelizar
    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < count; i++) {
        char *line = lines[i];
        line[strcspn(line, "\r\n")] = 0;

        char *url = extract_url(line);
        CacheNode *n = ht_get(ht, url);

        if (n) {
            #pragma omp critical
            n->hit_count++;
        }

        free(line);
    }
    free(lines);
}

int main() {
    HashTable *ht = ht_create(2000003);

    load_manifest(ht, "manifest.txt");
    process_par_critical(ht, "access_log.txt");

    ht_save_results(ht, "results.csv");
    ht_destroy(ht);
}
