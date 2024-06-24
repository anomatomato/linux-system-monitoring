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
    && apt-get clean && rm -rf /var/lib/lists/*

# Clone and install paho.mqtt.c
RUN git clone https://github.com/eclipse/paho.mqtt.c.git && \
    cd paho.mqtt.c && \
    mkdir build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local/paho-mqtt .. && \
    make && make install && \
    cd ../.. && rm -rf paho.mqtt.c

# Configure core dumps
RUN echo 'kernel.core_pattern=|/usr/lib/systemd/systemd-coredump %P %u %g %s %t %e' > /etc/sysctl.d/50-coredump.conf
RUN sysctl -p /etc/sysctl.d/50-coredump.conf
RUN ulimit -c unlimited

# Ensure these commands run as root
USER root