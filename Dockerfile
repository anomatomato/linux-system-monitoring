FROM debian:latest

RUN apt-get update \
    && apt-get install -y \
    crossbuild-essential-arm64 \
    crossbuild-essential-armhf \
    debhelper \
    dpkg-dev \
    cmake \
    libsensors4-dev \
    systemd-coredump \
    git \
    sudo \
    && apt-get clean && rm -rf /var/lib/lists/*

# Clone and install paho.mqtt.c
# RUN git clone https://github.com/eclipse/paho.mqtt.c.git && \
#     cd paho.mqtt.c && \
#     mkdir build && cd build && \
#     cmake .. && \
#     make && make install && \
#     cd ../.. && rm -rf paho.mqtt.c