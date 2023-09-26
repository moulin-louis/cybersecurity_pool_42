import multiprocessing
import os
import signal
import argparse
import requests
import datetime
from urllib.parse import urljoin, urlparse
from bs4 import BeautifulSoup


photo_download = {}


def is_valid_img(img_r):
    valid_mime_types = ["image/jpg", "image/jpeg", "image/png", "image/bmp", "image/gif"]
    content_type = img_r.headers.get('content-type')
    if content_type in valid_mime_types:
        return (True)
    return (False)


def get_all_img_src(images, url):
    img_src = []
    for image in images:
            src = image.get('src')
            img_src.append(requests.compat.urljoin(url, src))
    return (img_src)



def download_img(url, pathname):
    if url in photo_download:
        return
    img_head = requests.head(url)
    if not is_valid_img(img_head):
        return
    r = requests.get(url)
    if r.status_code != 200:
        print("Error connection to " + str(url) + " :" + str(r.status_code))
    photo_download[url] = True
    img_content = r.content
    img_name = os.path.basename(urlparse(url).path)
    img_path = os.path.join(pathname, img_name)
    print("img saved at ", img_path)
    r.raw.decode_content = True
    with open(img_path, "wb") as f:
        f.write(img_content)
    r.close()


def recurse_search(url, path, depth_level, soup):
    links = soup.find_all('a')
    for link in links:
        href = link.get('href')
        if href:
            if href.startswith("/"):
                href = urljoin(url, href)
            if not href.startswith("#"):
                if urlparse(href).netloc == urlparse(url).netloc:
                    spider(href, path, depth_level -1)


def spider(url, save_path, depth_level):
    if depth_level == 0:
        return
    response = requests.get(url)
    soup= BeautifulSoup(response.text, 'html.parser')
    images = soup.find_all('img')
    img_src = get_all_img_src(images, url)
    for image in img_src:
        download_img(image, save_path)
    recurse_search(url, path, depth_level, soup)


def sig_handler(arg_1, arg_2):
    print('SIGINT received, aborting the program')
    exit(1)


if __name__ == "__main__":
    signal.signal(signal.SIGINT, sig_handler)
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
    depth_level = args.level;
    spider(args.url, str(path), depth_level)
