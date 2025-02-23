FROM debian:latest

RUN apt-get update
RUN apt-get upgrade
RUN apt-get install systemd-journal-remote -y
RUN mkdir -p /var/log/journal/remote/
ENTRYPOINT [ \
    "/lib/systemd/systemd-journal-remote", \
    "--listen-http=19532", \
    "--output=/var/log/journal/remote/" \
]
