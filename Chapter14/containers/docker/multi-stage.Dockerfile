FROM ubuntu:latest AS builder

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y install build-essential gcc cmake libcpprest-dev

ADD . /usr/src

WORKDIR /usr/src

RUN mkdir build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build .

FROM ubuntu:latest

COPY --from=builder /usr/src/build/bin/customer /bin/customer

CMD /bin/customer
