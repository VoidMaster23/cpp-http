FROM mcr.microsoft.com/devcontainers/cpp:ubuntu-22.04

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
        vim \
        astyle \
        python3 \
        python3-pip \
        python3-venv \
        python3-setuptools \
        openssl \
        libssl-dev \
        cpplint \
        curl \
        gnupg \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Install mongo-cxx-driver
RUN curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r4.0.0/mongo-cxx-driver-r4.0.0.tar.gz && \
    tar -xzf mongo-cxx-driver-r4.0.0.tar.gz && \
    mkdir -p mongo-cxx-driver-r4.0.0/build && \
    cd mongo-cxx-driver-r4.0.0/build && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=20 && \
    cmake --build . && \
    sudo cmake --build . --target install

WORKDIR /workspace

COPY . .

RUN mkdir -p build

RUN cmake -S . -B build \
      -DOPENSSL_ROOT_DIR=/usr \
      -DOPENSSL_LIBRARIES=/usr/lib/x86_64-linux-gnu && \
    cmake --build build
