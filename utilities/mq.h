#define MAX_MSG_SIZE 1024
#define MAX_MESSAGES 10
#define NUM_QUEUES 2
#define MESSAGE_QUEUES ((char const*[]){"/inotify_coredump", "/inotify_pacct"})

/* Initialize posix-mq-to-mqtt-bridge */
int init_mq(const char* mq_path);

/* Send message in InfluxDB Line Protocol to posix-mq-to-mqtt-bridge */
int send_to_mq(const char* message, const char* mq_path);

/* Get timestamp in nanoseconds */
long long get_timestamp();