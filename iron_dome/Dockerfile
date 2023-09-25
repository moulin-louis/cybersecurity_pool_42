FROM rust:buster

RUN apt update -y && apt install -y stress

#create working and source folder
RUN mkdir -p /iron_dome/src

# Copy the source code and build files
COPY ./src/* ./iron_dome/src
#Copy the project toml
COPY ./Cargo.toml ./iron_dome/
#add tiny ransonware to test my program
COPY stockholm ./iron_dome
#add cpu burner to test my program
COPY test.py ./iron_dome
#Change workdir to iron_dome
WORKDIR ./iron_dome
#Build project in release mode (with optimization)
RUN cargo build --release
#Generate 1000 file to test entropy change
RUN mkdir -p /root/inception && for i in $(seq 1 10000); do echo "This is content for file $i" > /root/inception/"file_$i.txt"; done
#Run the projet in release mode, in --no-daemon mode to keep the docker alive
CMD cargo run --release -- /root --no-daemon

