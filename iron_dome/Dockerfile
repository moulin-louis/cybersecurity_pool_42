FROM debian:buster

# Install necessary packages
RUN apt update && apt-get install -y g++ cmake && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Create necessary directories
RUN mkdir -p iron_dome/build

# Copy the source code and build files
COPY ./CMakeLists.txt /iron_dome/CMakeLists.txt
COPY ./src /iron_dome/src
COPY ./inc /iron_dome/inc

RUN cd iron_dome/build && cmake .. && make

CMD ["/iron_dome/build/iron_dome", "/home"]

