# Dev Defined Modules
from config import * 
from videostream import VideoStream

# Python Modules
from time import sleep

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
    
    print ('Tracking has started...')
    
    while STILL_TRACKING:
        
        contours = video.get_contours()
        # Allowed Contours will be drawn on top of the threshold image
        #contour_drawn_frame = video.get_threshold_image()
        contour_drawn_frame = video.get_frame()

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
