#!/usr/bin/env bats

bats_require_minimum_version 1.5.0

setup() {
    load 'test_helper/common-setup'
    _common_setup
    stub hostname "echo 'testhost'"
    stub mosquitto_pub "echo 'Message published'"
}

#teardown() {
#unstub hostname
#unstub coredumpctl
#unstub mosquitto_pub
#}

@test "extract_coredump_metrics test" {
    local input="Tue 2024-01-01 11:57:42 CEST   1234  1000  1000 SIGSEGV /var/lib/systemd/coredump/coredump  /usr/bin/test"
    run -0 extract_coredump_metrics "$input"

    local delimiter=$'\x1F'
    local expected="testhost${delimiter}1234${delimiter}SIGSEGV${delimiter}Tue 2024-01-01 11:57:42 CEST${delimiter}/usr/bin/test"
    [ "$output" == "$expected" ]
}
@test "coredump_to_line_protocol test" {
    local hostname="testhost"
    local pid="1234"
    local sig="SIGSEGV"
    local time="Tue 2024-01-01 11:57:42 CEST"
    local exec="/usr/bin/test"
    local delimiter=$'\x1F'
    run -0 coredump_to_line_protocol "$hostname$delimiter$pid$delimiter$sig$delimiter$time$delimiter$exec"
    [ "$output" == "coredump_metrics,hostname=$hostname pid=$pid,signal=\"$sig\",time=\"$time\",executable=\"$exec\"" ]
}
@test "publish_to_mqtt test" {
    local message="measurement,hostname=testhost temp=13 123456789"
    run -0 publish_to_mqtt "$message"
    [ "$output" == "Message published" ]
}
