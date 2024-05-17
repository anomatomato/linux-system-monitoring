#include "MQTTClient.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/acct.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define ADDRESS "tcp://sep-cm0-server.ibr.cs.tu-bs.de:1883/"
#define CLIENTID "test_acct_log"
#define TOPIC "linux-monitoring/logs/sd_journal"
#define QOS 1
#define TIMEOUT 10000L

long long get_timestamp()
{
    return (long long)time(NULL) * 1000000000;
}

int main()
{
    FILE* fp =
        fopen("/var/log/pacct/diff.txt", "r"); // replace with your file path
    if (fp == NULL)
    {
        perror("Failed to open file");
        return 1;
    }

    struct acct_v3 account;
    while (fread(&account, sizeof(struct acct_v3), 1, fp) == 1)
    {
        // Print or process the metrics here
        // printf("ac_flag:%c\n", account.ac_flag);
        // printf("ac_version:%c\n", account.ac_version);
        // printf("ac_tty:%u\n", account.ac_tty);
        // printf("ac_exitcode:%u\n", account.ac_exitcode);
        // printf("ac_uid:%u\n", account.ac_uid);
        // printf("ac_gid:%u\n", account.ac_gid);
        // printf("ac_pid:%u\n", account.ac_pid);
        // printf("ac_ppid:%u\n", account.ac_ppid);
        // printf("ac_btime:%u\n", account.ac_btime);
        // printf("ac_etime:%f\n", account.ac_etime);
        // printf("ac_utime:%u\n", account.ac_utime);
        // printf("ac_stime:%u\n", account.ac_stime);
        // printf("ac_mem:%u\n", account.ac_mem);
        // printf("ac_io:%u\n", account.ac_io);
        // printf("ac_rw:%u\n", account.ac_rw);
        // printf("ac_minflt:%u\n", account.ac_minflt);
        // printf("ac_majflt:%u\n", account.ac_majflt);
        // printf("ac_swaps:%u\n", account.ac_swaps);
        // printf("ac_comm:%s\n", account.ac_comm);
        // printf("ac_etime:%u\n", account.ac_etime);
        // printf("ac_exitcode:%u\n", account.ac_exitcode);
        // printf("ac_flag:%c\n", account.ac_flag);
        // printf("ac_version:%c\n", account.ac_version);
    }

    if (ferror(fp))
    {
        perror("Failed to read file");
    }

    fclose(fp);
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg           = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE,
                      NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession      = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("Connected to MQTT broker at %s\n", ADDRESS);
    char message[1024];
    snprintf(message, 1024,
             "pacct,raspi=pi-85cd "
             "ac_flag=%c,ac_version=%c,ac_tty=%u,ac_exitcode=%u,ac_uid=%u,"
             "ac_gid=%u,ac_pid=%u,ac_ppid=%u,ac_btime=%u,ac_etime=%f,ac_"
             "utime=%u,ac_stime=%u,ac_mem=%u,ac_io=%u,ac_rw=%u,ac_minflt=%u"
             "u,ac_majflt=%u,ac_swaps=%u,ac_comm=\"%s\" %lld",
             account.ac_flag, account.ac_version, account.ac_tty,
             account.ac_exitcode, account.ac_uid, account.ac_gid,
             account.ac_pid, account.ac_ppid, account.ac_btime,
             account.ac_etime, account.ac_utime, account.ac_stime,
             account.ac_mem, account.ac_io, account.ac_rw, account.ac_minflt,
             account.ac_majflt, account.ac_swaps,account.ac_comm, get_timestamp());
    printf("Message: %s\n", message);
    pubmsg.payload    = message;
    pubmsg.payloadlen = strlen(message);
    pubmsg.qos        = QOS;
    pubmsg.retained   = 0;
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    printf("Waiting for publication of message with token %d\n", token);

    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with token %d delivered\n", token);

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}