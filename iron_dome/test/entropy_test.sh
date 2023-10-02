#!/bin/bash
while true
  do
    echo "Bonjour" > file_test #init file with a given entropy
    sleep 1 # wait a little
    echo "fsdjklfdskj3k2j4lj23kl4j23\\'\'\mv,cm,vmxcn" > file_test #change entropy for file, should trigger entropy warning
    echo "Bonjour" > file_test #restore initial entropy, should also trigger entropy warining
    rm file_test #check if handle file remove nicely
  done
