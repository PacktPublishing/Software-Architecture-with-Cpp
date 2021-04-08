FROM ubuntu:bionic

RUN apt-get update && apt-get -y install build-essential gcc

CMD /usr/bin/gcc
