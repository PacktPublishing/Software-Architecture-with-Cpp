FROM ubuntu:latest

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y install build-essential gcc cmake git googletest python3-pip && \
    pip3 install conan && conan profile new sacpp --detect && \
    conan profile update settings.compiler.libcxx=libstdc++11 sacpp

ADD . /usr/src

WORKDIR /usr/src

RUN mkdir build && \
    cd build && \
    conan install .. --build=missing -s build_type=Release -pr=sacpp && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . && \
    cmake --install .

CMD /usr/local/bin/customer
