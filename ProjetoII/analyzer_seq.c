#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

// -------- Função utilitária para extrair URL --------
static char* extract_url(char *line) {
    char *p = strchr(line, ' ');
    if (p) return p + 1;
    return line;
}

// --------- Carrega Manifesto ---------
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

// --------- Processamento Sequencial ---------
static void process_seq(HashTable *ht, const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) { perror("access_log"); exit(1); }

    char buf[8192];
    while (fgets(buf, sizeof(buf), fp)) {
        buf[strcspn(buf, "\r\n")] = 0;
        char *url = extract_url(buf);
        CacheNode *n = ht_get(ht, url);
        if (n) n->hit_count++;
    }
    fclose(fp);
}

int main() {
    HashTable *ht = ht_create(2000003);

    load_manifest(ht, "manifest.txt");
    process_seq(ht, "access_log.txt");

    ht_save_results(ht, "results.csv");
    ht_destroy(ht);
}
