import sys
import cv2
sys.path.append("./build")
from pycartoon import cppcartoonize

def picture_cartoonize(inputfile, outputfile):
    image = cv2.imread(inputfile)

    if image is None:
        print('invalid image input')
        sys.exit(0)

    height, width, channels = image.shape

    cimg = cppcartoonize(image, 1024, height*1024/width)

    if outputfile == None:
        cv2.imshow('pycartoon', cimg)
        cv2.waitKey(0)
    else:
        cv2.imwrite(outputfile, cimg)

def video_cartoonize(inputfile, outputfile):
    cap = cv2.VideoCapture(inputfile)
    fps = cap.get(cv2.CAP_PROP_FPS)

    (width, height) = (int(cap.get(cv2.CAP_PROP_FRAME_WIDTH)), int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT)))

    frameno = 0
    while True:
        # get a frame
        ret, frame = cap.read()

        if ret == False:
            break

        print frameno

        height, width, channels = frame.shape

        cimg = cppcartoonize(frame, 1024, height*1024/width)

        cv2.imshow('pycartoon', cimg)
        cv2.waitKey(1)

        frameno += 1

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('cartoonize example.jpg')
        exit(0)

    outputfile = None

    if len(sys.argv) >= 3:
        outputfile = sys.argv[2]

    if sys.argv[1].endswith(".jpg") or sys.argv[1].endswith(".jpeg") or sys.argv[1].endswith(".png"):
        picture_cartoonize(sys.argv[1], outputfile)
    elif sys.argv[1].endswith(".mp4"):
        video_cartoonize(sys.argv[1], outputfile)