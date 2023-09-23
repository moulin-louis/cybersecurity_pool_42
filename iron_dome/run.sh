#!/bin/fish
docker rm iron_dome
docker build . -t iron_dome
docker run --name iron_dome iron_dome:latest
