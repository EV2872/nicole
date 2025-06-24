# Dockerfile for Fedora

# How to run the Docker is found at the readme file section: Install using docker

FROM fedora:42
ENV LANG C.UTF-8

# Install dependencies
RUN dnf update -y && dnf install -y \
    git \
    wget \
    ninja-build \
    ccache \
    cppcheck \
    lcov \
    clang \
    clang-tools-extra \
    gcc-c++ \
    make \
    llvm-devel && \
    dnf clean all

# Install CMake 3.31.6
RUN wget https://github.com/Kitware/CMake/releases/download/v3.31.6/cmake-3.31.6-linux-x86_64.tar.gz && \
    tar -xzvf cmake-3.31.6-linux-x86_64.tar.gz --strip-components=1 -C /usr/local && \
    rm cmake-3.31.6-linux-x86_64.tar.gz

# Clone repo
RUN git clone --branch main https://github.com/EV2872/nicole.git /app
WORKDIR /app

# Give permissions
RUN chmod +x nicole.sh
# compile and execute a program
RUN ./nicole.sh examples/aa.nc -j