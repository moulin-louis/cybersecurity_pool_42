FROM debian:buster
RUN apt update -y && apt upgrade -y && apt install -y curl unzip fish
RUN curl -fsSL https://bun.sh/install | bash
COPY ./index.ts /src/index.ts
COPY ./sample_file /root/infection

#setup permissions and executable
RUN cd src && /root/.bun/bin/bun build --compile ./index.ts --outfile=stockholm && mv stockholm ..
RUN useradd toto
RUN chmod 755 /root
RUN chown -R toto:toto /root/infection


CMD [ "tail", "-f"]
#CMD su -s /bin/bash -c "./stockholm -v" toto
#CMD su -s /bin/bash -c "./stockholm -h" toto
#CMD su -s /bin/bash -c "echo $HOME" toto
#CMD su -s /bin/bash -c "./stockholm" toto
# RUN su -s /bin/bash -c "./stockholm" toto
#RUN su -s /bin/bash -c "./stockholm" toto
#RUN su -s /bin/bash -c "./stockholm -r $KEY" toto
#CMD [ "tail", "-f"]
#CMD su -s /bin/bash -c "./stockholm -s" toto
#CMD su -s /bin/bash -c "./stockholm -s -r $KEY" toto

