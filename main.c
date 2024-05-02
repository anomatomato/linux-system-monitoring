#include "MQTTClient.h"
#include <fcntl.h> /* For O_* constants */
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> /* For mode constants */
#include <unistd.h>

#define MQ_PATH "/my_queue"
#define MAX_MSG_SIZE 50
#define ADDRESS "tcp://localhost:1883/"
#define CLIENTID "ExampleClientPub"
#define TIMEOUT 10000L

int main()
{   
    // here should be called all needed functions 
    return 0;
}
