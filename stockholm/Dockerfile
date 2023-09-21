FROM debian:buster
RUN apt update -y && apt upgrade -y && apt install -y curl unzip
RUN curl -fsSL https://bun.sh/install | bash
COPY ./index.ts index.ts
CMD [ "/root/.bun/bin/bun", "run", "index.ts" ]