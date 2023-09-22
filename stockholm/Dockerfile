FROM debian:buster
RUN apt update -y && apt upgrade -y && apt install -y curl unzip
RUN curl -fsSL https://bun.sh/install | bash
COPY ./index.ts index.ts
COPY ./sample_file /root/inception
CMD [ "/root/.bun/bin/bun", "run", "index.ts" ]