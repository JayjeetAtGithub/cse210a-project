FROM ubuntu:21.04

ARG DEBIAN_FRONTEND=noninteractive

ADD . /java/

RUN apt update && \
    apt install -y python3 \
               python3-pip \
               python3-venv \
               python3-numpy \
               libsnappy-dev \
               libutf8proc-dev \
               libre2-dev \
               libthrift-dev \
               default-jre \
               default-jdk \
               maven && \
    apt clean && \
    cd /java/ && \
    mvn clean install
