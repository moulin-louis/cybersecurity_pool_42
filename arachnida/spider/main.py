import multiprocessing
import os
import argparse
import requests
import datetime
from urllib.parse import urljoin
from bs4 import BeautifulSoup


def parse_n_chek_url(base_url):
    r = requests.get(base_url)
    if r.status_code != 200:
        print("Error connection to url: ", r.status_code)
        return 1
    if not str(base_url).endswith("/"):
        base_url += "/"
    return base_url


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
    if not current_url.endswith("/"):
        return tmp + "/" + src
    else:
        return current_url + "/" + src


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


def download_img(url, pathname, list_manager, lock):
    with lock:
        if url in list_manager:
            return
        if not os.path.isdir(pathname):
            os.makedirs(pathname)
        r = requests.get(url, stream=True)
        if r.status_code != 200:
            print("Error connection to " + str(url) + " :" + str(r.status_code))
        list_manager[url] = True
        tmp = url.split("/")
        filename = pathname + "_" + datetime.datetime.now().strftime("%f") + "_" + tmp[len(tmp) - 1]
        print("img saved at ", filename)
        r.raw.decode_content = True
        with open(filename, "wb") as f:
            for chunk in r:
                f.write(chunk)
        r.close()


# def spider(url, recursive_flag, max_depth, save_path, url_set_manager, url_without_slash, lock, current_depth=1):
#     images = get_all_img_url(url, url_without_slash)
#     if not images:
#         return
#     for img_url in images:
#         if is_valid_img(img_url):
#             download_img(img_url, save_path, url_set_manager, lock)
#     if recursive_flag and current_depth + 1 <= max_depth:
#         soup = BeautifulSoup(requests.get(url).content, "html.parser")
#         links = []
#         for link in soup.find_all("a"):
#             href = link.attrs.get("href")
#             if href:
#                 href = urljoin(url, href)
#                 if href != url:
#                     links.append(href)
#         with multiprocessing.Pool(multiprocessing.cpu_count()) as pool:
#             arg_to_call = [
#                 (
#                     link,
#                     recursive_flag,
#                     max_depth,
#                     save_path,
#                     url_set_manager,
#                     url_without_slash,
#                     lock,
#                     current_depth + 1,
#                 )
#                 for link in links
#             ]
#             pool.starmap(spider, arg_to_call)


def gather_links(url, max_depth):
    links_already_seen = set()
    links_to_process = [url]
    result = set()
    result.add(url)

    for depth in range(1, max_depth + 1):
        print("depth: ", 1)
        next_list = []
        for link in links_to_process:
            # print("link seen: ", links_already_seen)
            print("link: ", link)
            if str(link).startswith("javascript:"):
                continue
            if str(link).startswith("mailto:"):
                continue
            if link in links_already_seen:
                continue
            links_already_seen.add(link)
            try:
                soup = BeautifulSoup(requests.get(link).content, "html.parser")
                data = soup.find_all("a")
            except requests.exceptions.RequestException:
                continue
            for redirect in data:
                href = redirect.attrs.get("href")
                href = urljoin(url, href)
                if str(href).startswith("javascript:"):
                    continue
                if str(href).startswith("mailto:"):
                    continue
                if href in links_already_seen:
                    continue
                # print("href: ", href)
                if href != link:
                    result.add(href)
                    next_list.append(href)
            links_to_process = next_list

    result = sorted(result)
    print(result)
    return list(result)


def process_link(argument):
    link, recursive_flag, max_depth, save_path, url_set_manager, url_without_slash, lock = argument
    images = get_all_img_url(link, url_without_slash)
    if not images:
        return
    for img_url in images:
        if is_valid_img(img_url):
            download_img(img_url, save_path, url_set_manager, lock)


if __name__ == "__main__":
    with multiprocessing.Manager() as manager:
        download_lock = manager.Lock()
        photo_downloaded_set = manager.dict()
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
        all_links = gather_links(args.url, args.level)
        print("all link gathered")
        arg_to_call = [
            (link, args.recursive, args.level, args.path, photo_downloaded_set, input_without_slash, download_lock)
            for link in all_links
        ]
        with multiprocessing.Pool(multiprocessing.cpu_count()) as pool:
            pool.map(process_link, arg_to_call)
        print("all image processed")
