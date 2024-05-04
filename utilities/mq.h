#define MAX_MSG_SIZE 150
#define NUM_QUEUES 10
#define MESSAGE_QUEUES ((char const*[]){"inotify_coredump"})

int send_to_mq(const char* message);