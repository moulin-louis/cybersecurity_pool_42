docker rm inquisitor
docker build -t inquisitor .
docker run --name inquisitor -p127.0.0.1:8787:22 inquisitor:latest