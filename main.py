# Dev Defined Modules
from config import DEBUG, SIZE_RATIO, RESOLUTION

def main():

    still_tracking = True

    print('Setup...')

    video = VideoStream(resolution = RESOLUTION, framerate = FRAMERATE).start_video_stream()
    print('Starting video stream...')
    # Let the video stream warm up
    time.sleep(2)

    print ('Tracking has started...')

    while still_tracking:

        contours = video.get_contours()

        biggest_area = MIN_AREA * SIZE_RATIO

        motion_found = False

        IMAGE = video.get_threshold_image()
        cv2.drawContours(IMAGE, contours, 0, (0, 255, 0), 1)


        for contour in contours:
            found_area = cv2.contourArea(contour)

            if found_area > biggest_area:
                motion_found = True


        video.FramePerSecond()

        if motion_found:
            print ('Motion Found, Total Contours', len(contours))



        if DEBUG == True:

            cv2.imshow('Image', IMAGE)

            # Clean up before program closes
            if cv2.waitKey(1) == ord('q'):
                cv2.destroyAllWindows()
                video.end_stream()

                still_tracking = False





if __name__ == '__main__':
    try:
        main()
    finally:
        print ('Tracking Done.')
