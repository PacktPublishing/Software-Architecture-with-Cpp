FROM ubuntu:bionic AS builder

RUN apt-get update && apt-get -y install build-essentials gcc cmake

ADD . /usr/src

WORKDIR /usr/src

RUN mkdir build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build .

FROM ubuntu:bionic

COPY --from=builder /usr/src/build/bin/customer /bin/customer

CMD /bin/customer
