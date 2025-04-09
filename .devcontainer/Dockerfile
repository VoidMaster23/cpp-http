FROM mcr.microsoft.com/devcontainers/cpp:1-debian-11

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    vim \
    astyle \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/* 


RUN pip3 install cpplint

WORKDIR /workspace

COPY . .

RUN ls

RUN mkdir -p build

RUN cmake -S . -B build \
    && cmake --build build




