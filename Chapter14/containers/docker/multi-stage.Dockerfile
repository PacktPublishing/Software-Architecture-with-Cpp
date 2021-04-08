FROM ubuntu:latest AS builder

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y install build-essential gcc cmake git googletest python3-pip && \
    pip3 install conan && conan profile new hosacpp --detect && \
    conan profile update settings.compiler.libcxx=libstdc++11 hosacpp

ADD . /usr/src

WORKDIR /usr/src

RUN mkdir build && \
    cd build && \
    conan install .. --build=missing -s build_type=Release -pr=hosacpp && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build .

FROM ubuntu:latest

COPY --from=builder /usr/src/build/bin/customer /bin/customer

CMD /bin/customer
