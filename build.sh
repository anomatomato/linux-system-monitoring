#!/bin/bash

# Create directory for deb packages
mkdir -p debs

build_for_arch() {
    arch=$1
    build_dir="build-$arch"

    echo "Building for $arch"
    mkdir -p "$build_dir"
    cd $build_dir
    rm -rf CMakeCache.txt CMakeFiles >/dev/null 2>&1

    if [[ "$arch" == "amd64" || "$arch" == "x86_64" ]]; then
        cmake .. || return 1
    elif [ "$arch" == "arm64" ]; then
        cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/arm64-toolchain.cmake .. \
            -DCMAKE_FIND_ROOT_PATH="/usr/aarch64-linux-gnu" || return 1
    elif [ "$arch" == "armhf" ]; then
        cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/armhf-toolchain.cmake .. \
            -DCMAKE_FIND_ROOT_PATH="/usr/arm-linux-gnueabihf" || return 1
    else
        echo "Unsupported architecture: $arch"
        return 1
    fi

    make || return 1
    cpack
    cd ..
}

build_all() {
    build_for_arch amd64
    build_for_arch arm64
    build_for_arch armhf
}

all=false

# Parse command line arguments
OPTS=$(getopt -o a --long all -- "$@")
if [ $? != 0 ]; then
    echo "Usage: $0 [-a|--all]"
    exit 1
fi

eval set -- "$OPTS"

# Extract options
while true; do
    case "$1" in
    -a | --all)
        all=true
        shift
        ;;
    --)
        shift
        break
        ;;
    *) break ;;
    esac
done

# Main logic
if [ "$all" = true ]; then
    # Build for all architectures
    build_all
    echo "All .deb packages have been created in the 'debs' directory."
else
    # Build for the current architecture
    current_arch=$(dpkg --print-architecture)
    build_for_arch "$current_arch"
    echo "Package for $current_arch has been created in the 'debs' directory."
fi
