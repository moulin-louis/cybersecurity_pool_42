# Stockhom Encryption Tool

**⚠️ WARNING: This project is for educational purposes only. Do not use for malicious intent. Always run in a safe environment like a virtual machine or Docker. The author is not responsible for any illegal use or data loss**

## Description

A demonstration of ransomware behavior similar to WannaCry, developed for the Linux platform. It encrypts files within a `infection` directory in the user's $HOME directory, and provides a decryption feature using the correct key.

## Prerequisites

- Bun
- TypeScript

## Features

- Encrypts files with specific extensions in the `/inception` directory within the user's home.
- Allows decryption if the correct key is provided.
- Supports various command-line flags for different functionalities.

## Usage
### To run the program
 - bun build --compile ./index.ts --outfile=stockholm
 - ./stockholm [OPTIONS]

## Options

- `-v`, `--version`: Display the version of the program.
- `-h`, `--help`: Display help information.
- `-s`, `--silent`: Run the program without producing any output.
- `-r`, `--reverse [KEY]`: Decrypt files. This requires the correct decryption key.

## Important Notes
- Only files within the infection folder in the user's HOME directory will be affected.
- Files will be renamed with a .ft extension after encryption.
- Ensure to keep the encryption key safe for decryption