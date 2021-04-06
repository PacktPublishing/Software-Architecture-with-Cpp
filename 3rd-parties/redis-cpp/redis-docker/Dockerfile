FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y redis-server
RUN sed -i 's/^bind 127\.0\.0\.1 \:\:1$/bind 0\.0\.0\.0/g' /etc/redis/redis.conf
RUN sed -i 's/^daemonize yes$/daemonize no/g' /etc/redis/redis.conf

CMD /usr/bin/redis-server /etc/redis/redis.conf
