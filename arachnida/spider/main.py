import multiprocessing
import os
import argparse
import requests
import datetime
from urllib.parse import urljoin
from bs4 import BeautifulSoup

photo_downloaded = []


def parse_n_chek_url(base_url):
    r = requests.get(base_url)
    if r.status_code != 200:
        print("Error connection to url: ", r.status_code)
        return 1
    if not str(base_url).endswith("/"):
        base_url += "/"
    return base_url


def is_valid_img(img_url):
    # print("img_url = ", img_url)
    return any(img_url.endswith(ext) for ext in [".jpg", ".jpeg", ".png", ".gif", ".bmp"])


def start_slash(src):
    # print("img src:", src)
    if src[0] == "/" and src[1] == "/":
        return "https:" + src
    try:
        requests.get(src)
    except requests.exceptions.RequestException:
        return url_without_slash + src
    else:
        return src


def not_start_slash(src, current_url):
    # print("img src:", src)
    if current_url == url_without_slash:
        tmp = current_url
    else:
        tmp = current_url.rpartition("/")[0]
    if not current_url.endswith("/"):
        return tmp + "/" + src
    else:
        return current_url + "/" + src


def get_all_img_url(current_url):
    # print("cur url: ", current_url)
    try:
        r = requests.get(current_url)
    except requests.exceptions.RequestException:
        # print("Bad url")
        return None
    soup = BeautifulSoup(r.text, "html.parser")
    images_tag = soup.find_all("img")
    result = []
    for image in images_tag:
        try:
            image["src"]
        except KeyError:
            continue
        if str(image["src"]).startswith("/"):
            result.append(start_slash(image["src"]))
        else:
            result.append(not_start_slash(image["src"], current_url))
    # for image in result:
    #     print("image_url: ", image)
    return result


def download_img(url, pathname):
    if photo_downloaded.count(url) != 0:
        return
    if not os.path.isdir(pathname):
        os.makedirs(pathname)
    r = requests.get(url, stream=True)
    if r.status_code != 200:
        print("Error connection to " + str(url) + " :" + str(r.status_code))
    photo_downloaded.append(url)
    print(url + " downloaded")
    tmp = url.split("/")
    filename = pathname + "_" + datetime.datetime.now().strftime("%f") + "_" + tmp[len(tmp) - 1]
    print("img saved at ", filename)
    r.raw.decode_content = True
    with open(filename, "wb") as f:
        for chunk in r:
            f.write(chunk)
    r.close()


def spider(url, recursive_flag, max_depth, save_path, current_depth=1):
    images = get_all_img_url(url)
    if not images:
        return
    for img_url in images:
        if is_valid_img(img_url):
            download_img(img_url, save_path)
    if recursive_flag and current_depth + 1 <= max_depth:
        soup = BeautifulSoup(requests.get(url).content, "html.parser")
        for link in soup.find_all("a"):
            href = link.attrs.get("href")
            if href:
                href = urljoin(url, href)
                if href != url:
                    spider(href, recursive_flag, max_depth, save_path, current_depth + 1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Image spider")
    parser.add_argument("url", help="URL to scrape")
    parser.add_argument("-r", "--recursive", action="store_true", help="Recursive image download")
    parser.add_argument("-l", "--level", type=int, default=5, help="Maximum recursive depth level")
    parser.add_argument("-p", "--path", default="./data/", help="Path where images will be saved")
    args = parser.parse_args()

    if not os.path.isdir(args.path):
        os.makedirs(args.path)
    url_without_slash = args.url
    if str(url_without_slash).endswith("/"):
        url_without_slash.pop()
    spider(args.url, args.recursive, args.level, args.path)
