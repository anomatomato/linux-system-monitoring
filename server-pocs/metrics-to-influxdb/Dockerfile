FROM debian:latest

RUN apt-get update && apt-get upgrade

RUN apt-get install -y gcc systemd-journal-remote systemd-coredump mosquitto-clients libsystemd-dev cmake make libssl-dev git nano build-essential

WORKDIR /home/workspace

COPY sd-journal-to-influxdb /home/workspace/sd-journal-to-influxdb
COPY coredump-to-influxdb /home/workspace/coredump-to-influxdb
COPY entrypoint.sh /home/workspace
# COPY entrypoint.sh /usr/local/bin

RUN chmod +x /home/workspace/entrypoint.sh
RUN cd sd-journal-to-influxdb && \
    mkdir -p build && \
    cd build && \
    cmake .. && \
    make install
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

ENTRYPOINT ["/home/workspace/entrypoint.sh"]
