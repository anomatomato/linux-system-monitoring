#include "sd_journal_to_influxdb.h"

int main(int argc, char *argv[]) {
        if(journal_to_influxdb(argc, argv) != 0) {
                fprintf(stderr, "Error sending journals to influxdbs.\n");
                return 1;
        }
        return 0;
}
