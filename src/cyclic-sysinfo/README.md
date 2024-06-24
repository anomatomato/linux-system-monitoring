# Cyclic Sysinfo

## Setup

### 1. Connect to IBR Server

1. Ensure you are in the `stats-recording/cyclic-sysinfo` directory on your machine.
2. Replace `yNUMMER` with your specific y-number.
3. Use the following command to connect to the IBR server:

```sh
ssh -J yNUMMER@x1.ibr.cs.tu-bs.de yNUMMER@sep-cm0-server
```

### 2. Compile the Code
1. Once connected to the server, compile your C code using the gcc compiler with the following command:

```sh
gcc -o cyclic_sysinfo cyclic_sysinfo.c -lrt -lpthread
```

This command does the following:

- **o cyclic_sysinfo**: Specifies the name of the output executable file (cyclic_sysinfo).
- **cyclic_sysinfo.c**: Specifies the source file to compile.
- **lrt**: Links the real-time library.
- **lpthread**: Links the POSIX thread library (if you are using threads).

### 3. Run the Program
- To run the compiled program, use:

```sh
./cyclic_sysinfo
```

## Verifying Output
- Ensure the posix-mq-to-mqtt-bridge is running and configured to read from the same message queue (/my_queue).
- Run the program and check if the posix-mq-to-mqtt-bridge is receiving and processing the messages correctly.


### Example Output
- When you run the program, you should see output similar to the following, indicating system statistics in the InfluxDB Line Protocol format:

```sh
Stats: sysinfo,host=my_host uptime=75653,total_ram=2058362880,free_ram=711045120,process_count=409 1715827352000000000
Stats: sysinfo,host=my_host uptime=75658,total_ram=2058362880,free_ram=711045120,process_count=409 1715827357000000000
Stats: sysinfo,host=my_host uptime=75663,total_ram=2058362880,free_ram=711045120,process_count=409 1715827362000000000
Stats: sysinfo,host=my_host uptime=75668,total_ram=2058362880,free_ram=711045120,process_count=409 1715827367000000000
Stats: sysinfo,host=my_host uptime=75673,total_ram=2058362880,free_ram=711045120,process_count=409 1715827372000000000
```

- This output verifies that the program is successfully gathering system statistics and sending them to the message queue in the correct format.