FROM debian:latest

RUN dpkg --add-architecture arm64
RUN dpkg --add-architecture armhf

RUN apt-get update \
    && apt-get install -y \
    crossbuild-essential-arm64 \
    crossbuild-essential-armhf \
    dpkg-dev \
    cmake \
    make \
    libsensors4-dev:amd64 libsensors4-dev:arm64 libsensors4-dev:armhf \
    libdbus-1-dev:amd64 libdbus-1-dev:arm64 libdbus-1-dev:armhf \
    systemd-coredump \
    acct \
    sudo \
    && apt-get clean && rm -rf /var/lib/lists/*

WORKDIR /app

COPY . .

# CMD ["./build.sh", "--all"]