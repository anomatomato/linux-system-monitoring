#include "pacct_to_line_protocol.h"

double comp_to_double(comp_t comp)
{
    int exponent = (comp >> 13) & 0x7; // Extract the 3-bit exponent
    int mantissa = comp & 0x1FFF;      // Extract the 13-bit mantissa
    return mantissa * (1 << exponent); // Calculate the value
}

double time_comp_to_double(comp_t comp)
{
    long v = (comp & 0x1fff) << (((comp >> 13) & 0x7) * 3);
    return (double)v / sysconf(_SC_CLK_TCK);
}

char* format_time(time_t start_time)
{
    static char formatted_time[64]; // static so that the memory is retained
                                    // after function return
    if (start_time == 0)
    {
        snprintf(formatted_time, sizeof(formatted_time), "0");
    }
    else
    {
        struct tm* timeinfo = localtime(&start_time);
        strftime(formatted_time, sizeof(formatted_time), "%a-%b-%d-%H:%M:%S-%Y",
                 timeinfo);
    }
    return formatted_time;
}

const char* get_flag_string(char ac_flag)
{
    switch (ac_flag)
    {
        case AFORK:
            return "AFORK";
        case ASU:
            return "ASU";
        case ACORE:
            return "ACORE";
        case AXSIG:
            return "AXSIG";
        default:
            return "NO_MATCH";
    }
}

void construct_payload(struct acct_v3* acct_record, char* payload,
                       size_t payload_size)
{
    char* start_time_str = format_time(acct_record->ac_btime);
    double cpu_time      = comp_to_double(acct_record->ac_utime);
    double sys_time      = comp_to_double(acct_record->ac_stime);
    double elapsed_time  = comp_to_double(acct_record->ac_etime);
    double avg_mem       = comp_to_double(acct_record->ac_mem);
    int exit_code        = acct_record->ac_exitcode;
    const char* flag_str = get_flag_string(acct_record->ac_flag);

    snprintf(payload, payload_size,
             "process_accounting,pid=%u,ppid=%u,start_time=%s,cpu_time=%.2f,"
             "system_time=%.2f,elapsed_time=%.2f,average_memory_usage=%.2f,"
             "exit_code=%d,flag=%s process=\"%s\" %lld",
             acct_record->ac_pid, acct_record->ac_ppid, start_time_str,
             cpu_time, sys_time, elapsed_time, avg_mem,
             acct_record->ac_exitcode, flag_str, acct_record->ac_comm,
             get_timestamp());
}
