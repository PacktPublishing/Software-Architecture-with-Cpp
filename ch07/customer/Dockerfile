FROM ubuntu:latest AS builder
ENV DEBIAN_FRONTEND=noninteractive
COPY . /app
RUN apt-get update && apt install cmake g++ git python3-pip googletest -y && pip3 install conan
RUN conan profile new default --detect && \
    conan profile update settings.compiler.libcxx=libstdc++11 default && \
    conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan || true
RUN mkdir /app/build && cd /app/build && \
    conan install .. --build=missing -pr=default && cmake .. && cmake --build . && cpack

FROM ubuntu:latest AS runtime
COPY --from=builder /app/build/packages/ /app
RUN apt-get update && apt -y --no-install-recommends install /app/*.deb && \
    apt-get autoremove -y && apt-get clean && rm -rf /app/
ENTRYPOINT ["/usr/bin/customer"]
EXPOSE 8080
