import sys
from PIL import Image, ExifTags
from PIL.ExifTags import TAGS


def display_exif_info(file):
    exif_data = file.getexif()
    if exif_data is None:
        print('No exif data')
        return
    if len(exif_data) == 0:
        print('No exif data')
        return
    for tag_id in exif_data:
        tag = TAGS.get(tag_id, tag_id)
        data = exif_data.get(tag_id)
        if isinstance(data, bytes):
            try:
                data = data.decode()
            except:
                data = "Non readable data"
        print(f"{tag} {data}")

def display_basic_info(file):
    width, height = file.size
    mode = file.mode
    format_ = file.format
    print(f"Width: {width}")
    print(f"Height: {height}")
    print(f"Color Mode: {mode}")
    print(f"Format: {format_}")
    # for key in file.info:
    #     print(key + ': ', file.info[key])

def scorpion(path):
    print(path + ":")
    try:
        file = Image.open(path)
    except:
        print('Cant open: ', path)
        return
    display_basic_info(file)
    display_exif_info(file)
    print('')
    

for pos in range(len(sys.argv)):
    if pos == 0:
        continue
    scorpion(sys.argv[pos])