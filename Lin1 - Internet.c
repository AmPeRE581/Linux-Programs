#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INTERVAL 1 // intervallo di tempo in secondi

typedef struct {
    char interface[16];
    unsigned long long rx_bytes;
    unsigned long long tx_bytes;
} NetworkStat;

void get_network_stats(NetworkStat *stats, int *num_interfaces) {
    FILE *file = fopen("/proc/net/dev", "r");
    if (file == NULL) {
        perror("Errore apertura /proc/net/dev");
        exit(1);
    }

    char line[256];
    int i = 0;

    // Salta le prime due righe
    fgets(line, sizeof(line), file);
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, " %15[^:]: %llu 0 0 0 0 0 0 0 %llu",
               stats[i].interface,
               &stats[i].rx_bytes,
               &stats[i].tx_bytes);
        i++;
    }

    *num_interfaces = i;
    fclose(file);
}

void print_network_stats(const NetworkStat *stats, int num_interfaces) {
    for (int i = 0; i < num_interfaces; i++) {
        printf("%s: RX = %llu bytes, TX = %llu bytes\n",
               stats[i].interface,
               stats[i].rx_bytes,
               stats[i].tx_bytes);
    }
}

int main() {
    NetworkStat prev_stats[10];
    NetworkStat curr_stats[10];
    int num_interfaces;

    // Inizializza le statistiche precedenti
    get_network_stats(prev_stats, &num_interfaces);

    while (1) {
        sleep(INTERVAL);

        // Ottiene le statistiche correnti
        get_network_stats(curr_stats, &num_interfaces);

        // Calcola la differenza e stampa i risultati
        printf("Traffico di rete (ultimo %d secondi):\n", INTERVAL);
        for (int i = 0; i < num_interfaces; i++) {
            unsigned long long rx_diff = curr_stats[i].rx_bytes - prev_stats[i].rx_bytes;
            unsigned long long tx_diff = curr_stats[i].tx_bytes - prev_stats[i].tx_bytes;

            printf("%s: RX = %llu bytes, TX = %llu bytes\n",
                   curr_stats[i].interface, rx_diff, tx_diff);

            // Aggiorna le statistiche precedenti
            prev_stats[i] = curr_stats[i];
        }
        printf("\n");
    }

    return 0;
}
