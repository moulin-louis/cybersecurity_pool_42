import os
import argparse
from urllib.parse import urljoin, urlparse
from datetime import datetime

import requests
from bs4 import BeautifulSoup


def is_valid_img(img_url):
    try:
        return any(
            img_url.endswith(ext) for ext in [".jpg", ".jpeg", ".png", ".gif", ".bmp"]
        )
    except:
        return None


def get_all_img_url(url):
    r = requests.get(url)
    soup = BeautifulSoup(r.text, "html.parser")
    print(soup.prettify())
    images_tag = soup.find_all("img")
    result = []
    if len(images_tag) == 0:
        print("0 tag img found")
    for image in images_tag:
        try:
            print("try get :", image["src"])
            requests.get(image["src"])
        except:
            result.append(url + image["src"])
        else:
            result.append(image)

    return result


def download_img(url, pathname):
    if not os.path.isdir(pathname):
        os.makedirs(pathname)
    response = requests.get(url, stream=True)
    tmp = url.split("/")
    filename = pathname + "/_" + datetime.now().strftime("%f") + "_" + tmp[len(tmp) - 1]
    print("img saved at ", filename)
    with open(filename, "wb") as f:
        for data in response.iter_content(1024):
            f.write(data)


def spider(url, recursize_flag, max_depth, save_path, current_depth=1):
    if int(current_depth) > int(max_depth):
        return
    images = get_all_img_url(url)
    for img_url in images:
        if is_valid_img(img_url):
            download_img(img_url, save_path)
    if recursize_flag:
        soup = BeautifulSoup(requests.get(url).content, "html.parser")
        for link in soup.find_all("a"):
            href = link.attrs.get("href")
            if href:
                href = urljoin(url, href)
                if href != url:
                    spider(
                        href, recursize_flag, max_depth, save_path, current_depth + 1
                    )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Image spider")
    parser.add_argument("url", help="URL to scrape")
    parser.add_argument(
        "-r", "--recursive", action="store_true", help="Recursive image download"
    )
    parser.add_argument(
        "-l", "--level", type=int, default=5, help="Maximum recursive depth level"
    )
    parser.add_argument(
        "-p", "--path", default="./data/", help="Path where images will be saved"
    )

    args = parser.parse_args()
    if not os.path.isdir(args.path):
        os.makedirs(args.path)
    spider(args.url, args.recursive, args.level, args.path)
