from difflib import Differ
import zipfile
import os
import bs4
import requests


def find_first_pos(line):
    x = 0
    white_space = "\t\n\r\t\f "
    for letter in line:
        if white_space.find(letter) != -1:
            x = x + 1
            continue
        return x + 1


def diff_html(html1, html2, result):
    if html1 == html2:
        # print('LOG: No data found this time')
        return
    # print('LOG: Data found!')
    differ = Differ()
    deltas = list(differ.compare(html1, html2))
    for line in deltas:
        if line.startswith('+') and not line[1:].isspace():
            pos = find_first_pos(line[1:])
            parsed_line = line[pos:len(line) - 1]
            if parsed_line.startswith('<'):
                parsed_line = bs4.BeautifulSoup(parsed_line, 'html.parser').get_text()
            if len(parsed_line) != 0:
                print(f"[{parsed_line}]")
                result.append(parsed_line)


def write_result(archive_file, result):
    with open("result.txt", 'w') as f:
        for line in result:
            f.write(line + '\n')
    if not archive_file.endswith('.zip'):
        zip_file = archive_file + '.zip'
    else:
        zip_file = archive_file
    with zipfile.ZipFile(zip_file, 'w') as zipf:
        zipf.write("result.txt")
    os.unlink("result.txt")


def perform_requests(url, payload, method):
    if method == "GET":
        return requests.get(url + payload)
    return requests.post(url, data=payload)
