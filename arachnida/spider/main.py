import multiprocessing
import os
import argparse
import requests
import datetime
from urllib.parse import urljoin
from bs4 import BeautifulSoup


def is_valid_img(img_url):
    return any(img_url.endswith(ext) for ext in [".jpg", ".jpeg", ".png", ".gif", ".bmp"])


def start_slash(src, url_without_slash):
    if src[0] == "/" and src[1] == "/":
        return "https:" + src
    try:
        requests.get(src)
    except requests.exceptions.RequestException:
        return url_without_slash + src
    else:
        return src


def not_start_slash(src, current_url, url_without_slash):
    if current_url == url_without_slash:
        tmp = current_url
    else:
        tmp = current_url.rpartition("/")[0]
    try:
        requests.get(src)
    except requests.exceptions.RequestException:
        if not current_url.endswith("/"):
            return tmp + "/" + src
        else:
            return current_url + "/" + src
    else:
        return src


def get_all_img_url(current_url, url_without_slash):
    try:
        r = requests.get(current_url)
    except requests.exceptions.RequestException:
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
            result.append(start_slash(image["src"], url_without_slash))
        else:
            result.append(not_start_slash(image["src"], current_url, url_without_slash))
    return result


def download_img(url, pathname, photo_down_set):
    if url in photo_down_set:
        return
    if not os.path.isdir(pathname):
        os.makedirs(pathname)
    try:
        r = requests.get(url, stream=True)
    except requests.exceptions.RequestException:
        return
    if r.status_code != 200:
        print("Error connection to " + str(url) + " :" + str(r.status_code))
    photo_down_set[url] = True
    tmp = url.split("/")
    filename = pathname + tmp[len(tmp) - 1]
    print("img saved at ", filename)
    r.raw.decode_content = True
    with open(filename, "wb") as f:
        for chunk in r:
            f.write(chunk)
    r.close()


def spider(url, recursive_flag, max_depth, save_path, url_without_slash, photo_down_set, current_depth=1):
    images = get_all_img_url(url, url_without_slash)
    if not images:
        return
    for img_url in images:
        if is_valid_img(img_url):
            download_img(img_url, save_path, photo_down_set)
    if recursive_flag and current_depth + 1 <= max_depth:
        try:
            soup = BeautifulSoup(requests.get(url).content, "html.parser")
        except requests.exceptions.RequestException:
            return
        for link in soup.find_all("a"):
            href = link.attrs.get("href")
            if href:
                href = urljoin(url, href)
                if href != url:
                    spider(
                        href, recursive_flag, max_depth, save_path, url_without_slash, photo_down_set, current_depth + 1
                    )


if __name__ == "__main__":
    try:
        with multiprocessing.Manager() as manager:
            download_lock = manager.Lock()
            photo_downloaded_set = {}
            parser = argparse.ArgumentParser(description="Image spider")
            parser.add_argument("url", help="URL to scrape")
            parser.add_argument("-r", "--recursive", action="store_true", help="Recursive image download")
            parser.add_argument("-l", "--level", type=int, default=5, help="Maximum recursive depth level")
            parser.add_argument("-p", "--path", default="./data/", help="Path where images will be saved")
            args = parser.parse_args()

            if not os.path.isdir(args.path):
                os.makedirs(args.path)
            input_without_slash = args.url
            if str(input_without_slash).endswith("/"):
                input_without_slash = args.url.rstrip("/")
            path = args.path
            if not str(path).endswith("/"):
                path += "/"
            spider(args.url, args.recursive, args.level, str(path), input_without_slash, photo_downloaded_set)
    except KeyboardInterrupt:
        print("Signal received")
