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
