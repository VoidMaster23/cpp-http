FROM mcr.microsoft.com/devcontainers/cpp:ubuntu-22.04

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    vim \
    astyle \
    curl \
    wget \
    gnupg \
    ca-certificates \
    python3 \
    python3-pip \
    python3-venv \
    python3-setuptools \
    openssl \
    libssl-dev \
    cmake \
    build-essential \
    pkg-config \
    libtool \
    autoconf \
    cpplint \
    && rm -rf /var/lib/apt/lists/*

#installing mongocxx driver - connects c++ to mongo
RUN wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r4.0.0/mongo-cxx-driver-r4.0.0.tar.gz && \
    tar -xzf mongo-cxx-driver-r4.0.0.tar.gz && \
    cd mongo-cxx-driver-r4.0.0 && \
    mkdir -p build && cd build && \
    cmake -DCMAKE_CXX_STANDARD=20 -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc) && \
    make install && \
    cd ../.. && \
    rm mongo-cxx-driver-r4.0.0.tar.gz && \
    rm -rf mongo-cxx-driver-r4.0.0

RUN wget https://github.com/nlohmann/json/archive/refs/tags/v3.12.0.tar.gz && \
    tar -xzf v3.12.0.tar.gz && \
    cd json-3.12.0 && \
    mkdir build && cd build && \
    cmake .. && \
    make && \
    make install && \
    cd ../.. && \
    rm -rf json-3.12.0 v3.12.0.tar.gz

WORKDIR /workspace

COPY . .

RUN mkdir -p build

ENV LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"

RUN cmake -S . -B build \ 
    -DOPENSSL_ROOT_DIR=/usr \
    -DOPENSSL_LIBRARIES=/usr/lib/x86_64-linux-gnu && \
    cmake --build build
