FROM debian:buster
RUN apt update && apt upgrade -y
RUN apt install -y apt-transport-https ca-certificates wget \
    gpg openssh-server
RUN mkdir -p /data/site && mkdir -p /var/run/sshd
RUN useradd -m loumouli && echo "loumouli:1234" | chpasswd


COPY ./ressource/tor.list /etc/apt/sources.list.d/tor.list
COPY ./ressource/script.sh /tmp/script.sh
COPY ./ressource/mandatory/nginx.conf /etc/nginx/
COPY ./ressource/mandatory/index.html /data/site
COPY ./ressource/mandatory/torrc /etc/tor/torrc
COPY ./ressource/start_services.sh /start_services.sh
COPY ./ressource/mandatory/sshd_config /etc/ssh/sshd_config

RUN chmod +x /start_services.sh && chmod +x /tmp/script.sh \
        && /tmp/script.sh
RUN apt update && apt install -y nginx \
    tor --option=Dpkg::Options::=--force-confdef

CMD ["./start_services.sh"]