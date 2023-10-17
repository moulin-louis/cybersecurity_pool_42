# SQL Injection Testing Tool
This project provides a tool for detecting SQL injection vulnerabilities in web applications and a vulnerable environment to test against.

## Setup
- ### Prerequisites
- Docker
- Python 3.x

## Usage
- ### Setting Up the Testing Environment:
- ./run_docker.sh
- This will set up a Flask web application vulnerable to SQL injection, connected to either a MySQL or SQLite database (based on the DB_ENGINE environment variable).

- ## Running the SQL Injection Tool:
- python vaccine.py [OPTIONS] URL
- -o: Specify an archive file for storage.
- -X: Specify request type (GET/POST). Default is GET.
## Tool Features
- Detects SQL injection vulnerabilities using error-based and union-based techniques.
- Supports both MySQL and SQLite databases.
- Extracts database metadata: database names, table names, column names, and full data dumps.
## Warning
- This tool is for educational and testing purposes only. Always obtain proper authorization before testing any web application.