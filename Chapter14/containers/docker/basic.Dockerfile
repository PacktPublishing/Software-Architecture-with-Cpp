FROM ubuntu:bionic

RUN apt-get update && apt-get -y install build-essentials gcc

CMD /usr/bin/gcc
