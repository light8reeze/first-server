FROM ubuntu:24.04

ARG BOOST_VERSION=1.83.0
ARG BOOST_DIR=boost_${BOOST_VERSION//./_}
ARG BOOST_ARCHIVE=${BOOST_DIR}.tar.gz
ARG BOOST_PREFIX=/usr/local

RUN apt update && apt install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    pkg-config \
    vim \
    curl \
    gdb \
    gcc g++ \
    wget \
    ca-certificates \
    python3 \
    libicu-dev \
    liburing-dev \
    nodejs npm \
    && rm -rf /var/lib/apt/lists/*

RUN cd /tmp \
    && wget -q https://archives.boost.io/release/${BOOST_VERSION}/source/${BOOST_ARCHIVE} \
    && tar -xf ${BOOST_ARCHIVE} \
    && cd ${BOOST_DIR} \
    && ./bootstrap.sh --prefix=${BOOST_PREFIX} --with-libraries=atomic,filesystem,system \
    && ./b2 -j$(nproc) install \
    && cd /tmp \
    && rm -rf ${BOOST_DIR} ${BOOST_ARCHIVE}

RUN npm install -g @anthropic-ai/claude-code

ENV CMAKE_PREFIX_PATH=${BOOST_PREFIX}:${CMAKE_PREFIX_PATH}

WORKDIR /workspace

CMD ["/bin/bash"]