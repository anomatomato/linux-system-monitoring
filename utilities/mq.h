#define MQ_PATH "/my_queue"
#define MAX_MSG_SIZE 150

/* Send message in InfluxDB Line Protocol to posix-mq-to-mqtt-bridge */
int send_to_mq(const char* message);

/* Get timestamp in nanoseconds */
long long get_timestamp();