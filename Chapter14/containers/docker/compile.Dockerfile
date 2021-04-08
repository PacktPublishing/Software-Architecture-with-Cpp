FROM ubuntu:bionic

RUN apt-get update && apt-get -y install build-essential gcc cmake

ADD . /usr/src

WORKDIR /usr/src

RUN mkdir build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . && \
    cmake --install .

CMD /usr/local/bin/customer
