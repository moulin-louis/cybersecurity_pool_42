FROM rust:buster

RUN mkdir -p /iron_dome/src

# Copy the source code and build files
COPY ./src/* ./iron_dome/src
COPY ./Cargo.toml ./iron_dome/
WORKDIR ./iron_dome
RUN cargo build --release
CMD cargo run --release -- /root --no-daemon

