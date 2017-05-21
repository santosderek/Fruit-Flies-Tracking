from videostream import *

def main():
    video = FlyContourTracker().run_contour_detection()

    # To make the program get a normal frame faster,
    # we use the main thread to get the normal frame
    # and process the contours in another thread

    # This function serves as the 'mainloop' for the normal frames
    video.normal_frame_loop()

    #while not video.closed:
    #    pass




if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print('ERROR:', e)
