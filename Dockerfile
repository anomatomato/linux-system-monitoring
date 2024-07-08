FROM debian:latest

RUN apt-get update \
    && apt-get install -y \
    crossbuild-essential-arm64 \
    crossbuild-essential-armhf \
    dpkg-dev \
    cmake \
    make \
    libsensors4-dev \
    libdbus-1-dev \
    libdbus-1-3 \
    systemd-coredump \
    git \
    acct \
    sudo \
    && apt-get clean && rm -rf /var/lib/lists/*

WORKDIR /app

COPY . .

CMD ["./build.sh"]

# Clone and install paho.mqtt.c
# RUN git clone https://github.com/eclipse/paho.mqtt.c.git && \
#     cd paho.mqtt.c && \
#     mkdir build && cd build && \
#     cmake .. && \
#     make && make install && \
#     cd ../.. && rm -rf paho.mqtt.c