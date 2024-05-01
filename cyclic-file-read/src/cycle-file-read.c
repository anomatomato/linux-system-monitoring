#include <stdio.h>
#include "../include/common.h"
#include "../include/stat.h"
#include "../include/net.h"
#include "../include/disk.h"

int btime;

int main(int argc, char* argv[]) {
    btime = get_btime();
    stat(btime);
}
