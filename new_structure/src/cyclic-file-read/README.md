## Normal run

```bash
./cyclic-file-read-exec
```

- All metrics are collected and send

## with flags

```bash
./cyclic-file-read-exec <flag>
```

### --proc-stat

- CPU-metrics are collected and send

### --proc-net-dev

- Networkmetrics are collected and send

### --proc-diskstats

- Drivemetrics are collected and send

### --hwmon

- Chip-temperatures are collected and send

### --sys

- all Systemlevel-metrics are collected and send
- includes (stat, net-dev, drive, diskstats, hwmon)

### --pid-stat

- Process-stat-metrics are collected and send

### --pid-statm

- Process-statm-metrics are collected and send

### --pid-io

- Process-io-metrics are collected and send
- run with sudo

### --pid

- all Procesmetrics are collected and send
- run with sudo
