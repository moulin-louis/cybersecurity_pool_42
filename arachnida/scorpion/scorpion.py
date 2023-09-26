import sys
from PIL import Image, ExifTags, ImageSequence

def extract_jpeg_png(path):
    try:
        img = Image.open(path)
        img_exif = img.getexif()
        if img_exif is None:
            return
        if not len(img_exif.items()):
            return
        print('Metadata for ', path)
        for key, val in img_exif.items():
            if key in ExifTags.TAGS:
                print(f'{ExifTags.TAGS[key]}:{val}')
        print('')
    except:
        print("Cant open: ", path)

def extract_gif(path):
    try:
        img = Image.open(path)
    except:
        print("Cant open: ", path)
        return
    width, height = img.size
    mode = img.mode
    format_ = img.format
    frames = [frame.copy() for frame in ImageSequence.Iterator(img)]
    print('Metadata for ', path)
    print(f"Width: {width}")
    print(f"Height: {height}")
    print(f"Color Mode: {mode}")
    print(f"Format: {format_}")
    print(f"Number of Frames: {len(frames)}")
    print('')


def extract_bmp(path):
    try:
        img = Image.open(path)
    except:
        print("Cant open: ", path)
        return
    width, height = img.size
    mode = img.mode
    format_ = img.format
    print(f"Width: {width}")
    print(f"Height: {height}")
    print(f"Color Mode: {mode}")
    print(f"Format: {format_}")
    for key in img.info:
        print(key + ': ', img.info[key])
    print('')

for pos in range(len(sys.argv)):
    file = sys.argv[pos];
    if pos == 0:
        continue
    if str(file).endswith('.jpeg'):
        extract_jpeg_png(file)
        continue
    if str(file).endswith('.jpg'):
        extract_jpeg_png(file)
        continue
    if str(file).endswith('.png'):
        extract_jpeg_png(file)
        continue
    if str(file).endswith('.gif'):
        extract_gif(file)
        continue
    if str(file).endswith('.bmp'):
        extract_bmp(file)
        continue
    else:
        print("Wrong extension for ", file)