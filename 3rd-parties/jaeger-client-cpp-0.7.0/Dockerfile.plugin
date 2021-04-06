FROM debian
RUN apt-get update
RUN apt-get install --yes build-essential curl git
RUN curl -L -O "https://cmake.org/files/v3.11/cmake-3.11.0-Linux-x86_64.sh" && \
    bash cmake-3.11.0-Linux-x86_64.sh --skip-license

ADD . /src/jaeger-cpp-client
WORKDIR /src/jaeger-cpp-client
RUN ./scripts/build-plugin.sh
