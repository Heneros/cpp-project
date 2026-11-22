


FROM alpine:3.19 AS builder


RUN apk add --no-cache \
    build-base \
    cmake \
    make \
    linux-headers \
    libstdc++ \
    openssl-dev \
    spdlog-dev \
    git \
    gdb \
    valgrind \
    htop \
    vim \
    bash \
    netcat-openbsd \
    curl \
    wget

WORKDIR /workspace
COPY . .

RUN chmod +x build.sh 

EXPOSE 65123

CMD ["/bin/sh", "-c", "while true; do sleep 1000; done"]