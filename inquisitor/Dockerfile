# CLion remote docker environment (How to build docker container, run and stop it)
#
# Build and run:
#   docker build -t clion/remote-alpine:1.0 -f Dockerfile-alpine .
#   docker run -d --cap-add sys_ptrace --privileged -p127.0.0.1:2222:22 clion/remote-alpine:1.0
#   ssh-keygen -f "$HOME/.ssh/known_hosts" -R "[localhost]:2222"
#
# stop:
#   docker stop clion_remote_env
# 
# ssh credentials (test user):
#   root@root

FROM rocker/vscode:latest

CMD [ "tail", "-f" ]