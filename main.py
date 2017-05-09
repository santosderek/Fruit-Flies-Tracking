# Created and Edited by Derek Santos

# Dev Defined Modules
from config import * 
from videostream import VideoStream

# Python Modules
from time import sleep, time

# 3rd Party Modules
import cv2

STILL_TRACKING = True

def check_windows(frame):
    global STILL_TRACKING
    
    cv2.imshow('Image', frame)

    # If key == 'q' then close all windows and stop main loop
    if cv2.waitKey(1) == ord('q'):
        cv2.destroyAllWindows()
        STILL_TRACKING = False

def main():

    global STILL_TRACKING
    
    print ('Starting video stream and contour motion detection...')
    # Starting two individual threads: one for grabbing normal frames, and a second to detect contours 
    video = VideoStream(resolution = RESOLUTION, framerate = FRAMERATE).start_detection()

    # Time since there was detection, declaration
    time_of_last_motion = None
    print_time_delay = time()
    
    print ('Tracking has started...')
    
    while STILL_TRACKING:
        
        contours = video.get_contours()

        # Contours will be drawn on top of contour_drawn_frame
        contour_drawn_frame = video.get_frame()

        # When time_of_last_motion has been updated, and print_time_delay has been delayed for more than a second
        if time_of_last_motion != None and time() - print_time_delay >= 1:
            print ('Time duration since last update:', int (time() - time_of_last_motion))

            # When print time delay is greater than a second, reset it
            print_time_delay = time()

        # If there are no contours, skip the whole process
        if contours == None or len(contours) == 0:
            check_windows(contour_drawn_frame)
            
            if not STILL_TRACKING:
                video.end_stream()
                
            continue
        
        #print (len(contours))
        motion_found = False
        
        # Allowed contours will be a list of position within the contours list that are considered 'worthy' to track
        allowed_contours = []
        
        for pos, contour in enumerate(contours):
            
            if cv2.contourArea(contour) > MIN_AREA:
                motion_found = True
                allowed_contours.append(pos)
                
        if motion_found:
            time_of_last_motion = time()
            
        for position in allowed_contours:
            # Remember that the frame is in Grayscale so the only colors displayed would be black or gray
            contour_drawn_frame = cv2.drawContours(contour_drawn_frame, contours[position], -1, (128, 255, 0), 3)

        if DEBUG == True:

            check_windows(contour_drawn_frame)
            
            if not STILL_TRACKING:
                video.end_stream()

            

if __name__ == '__main__':
    
    try:
        main()
    finally:
        print ('Tracking Done.')
