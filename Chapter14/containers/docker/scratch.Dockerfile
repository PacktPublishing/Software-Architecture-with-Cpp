FROM scratch

COPY customer /bin/customer
VOLUME /tmp
CMD ["/bin/customer"]
