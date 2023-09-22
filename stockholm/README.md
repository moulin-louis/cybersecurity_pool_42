Inception Encryption Tool
WARNING: This tool is designed to encrypt files in a directory. Please use responsibly and always ensure you have backups before running encryption tools.

Description
The Inception Encryption Tool is a command-line program written in TypeScript that can encrypt or decrypt files in a specified directory (default is $HOME/inception). The tool targets specific file extensions, as defined in the source code.

Features
Encrypts files with specific extensions in the /inception directory within the user's home.
Allows decryption if the correct key is provided.
Supports various command-line flags for different functionalities.
Usage
bash
Copy code
# To run the program
$ bun build --compile ./index.ts --outfile=stockholm
$ ./stockholm [OPTIONS]
Options
-v, --version: Display the version of the program.
-h, --help: Display help information.
-s, --silent: Run the program without producing any output.
-r, --reverse [KEY]: Decrypt files. This requires the correct decryption key.
Dependencies
crypto: For encryption and decryption functionalities.
fs: To read and modify files and directories.
Warning
Be extremely careful when running this tool, especially in production environments. Always ensure that you have backups of important files. The tool will encrypt targeted files, making them inaccessible without the decryption key.

Disclaimer
This tool is provided for educational purposes. The author is not responsible for any misuse or data loss. Always review and test code before running it in a critical environment.