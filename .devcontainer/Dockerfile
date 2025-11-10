FROM ubuntu:22.04

RUN apt update && apt install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    liburing-dev \
    pkg-config \
    vim \
    curl \
    gdb \
    && rm -rf /var/lib/apt/lists/* \
    wget

WORKDIR /workspace

CMD ["/bin/bash"]