# Python Modules
import io
from time import sleep, time

# 3rd Party Modules
import cv2

from picamera.array import PiRGBArray
from picamera import PiCamera
from threading import Thread

# Dev developed modules
from config import *

class VideoStream:
    def __init__(self, resolution = (1280, 780), framerate = 30):
        # Camera Settings
        self.camera = PiCamera()
        self.camera.resolution = resolution
        self.camera.framerate = framerate

        # Initializing Frame Buffer
        self.rawFrame = PiRGBArray(self.camera, size = resolution)
        self.stream = self.camera.capture_continuous(self.rawFrame,
                                                     format = 'bgr',
                                                     use_video_port = True)
        # Condition To Close Stream
        self.is_closed = False

        # Time of last recorded image
        self.time_since_last_update = time()
        self.number_of_frames = 0

        # Stored Images
        self.base_frame = None
        self.current_frame = None
        self.threshold_image = None

        # Stored Contours
        self.contours = None
        self.hierarchy = None

        # Stored Threads
        self.contour_thread = None

    def start_detection(self):
        # Creates a thread that consistantly grabs a frame from camera
        

        """ By setting daemon to True, the program will automatically close the
            thread when I exit the program
        """
        video_thread = Thread(target = self.refresh_frame, args = ())
        video_thread.daemon = True
        video_thread.start()

        sleep(5)
        
        self.contour_thread = Thread( target = self.refresh_contours, args = ())
        self.contour_thread.daemon = True
        self.contour_thread.start()
        
        return self

    def refresh_frame(self):
        for frame in self.stream:


            # This makes the base_image every 'last' image

            """ If base_frame is non-existant we will get the first frame from the
                camera and then make it the base.

                Else, base_frame will be the last image, while the current image is
                the 'real time' frame
            """
            if self.base_frame == None:
                self.current_frame = frame.array
                self.base_frame = self.current_frame.copy()
            else:
                self.base_frame = self.current_frame.copy()
                self.current_frame = frame.array
                
            self.rawFrame.truncate(0)

            # When stream is closing, initiate cleansing
            if self.is_closed:

                self.stream.close()
                self.rawFrame.close()
                self.camera.close()
                return


    

    def get_contours(self):
        return self.contours


    # Returns an Absolute Value Frame
    def refresh_contours(self):
        
        # While the VideoStream class is not closed 
        while not self.is_closed:
            try:
                current_gray = cv2.cvtColor (self.current_frame.copy(), cv2.COLOR_BGR2GRAY)
                base_gray = cv2.cvtColor (self.base_frame.copy(), cv2.COLOR_BGR2GRAY)

                # NOTE: I might need to make base_gray and currentgray , base_gray.copy() and current_gray.copy()
                abs_image = cv2.absdiff(base_gray, current_gray)
                abs_image = cv2.blur (abs_image, (10, 10))

                _, self.threshold_image = cv2.threshold (abs_image, MINIMUM_THRESHOLD, 255, cv2.THRESH_BINARY)
                
                self.threshold_image, self.contours, self.hierarchy = cv2.findContours(self.threshold_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

                self.FramesPerSecond()
                
            except Exception as e:
                print (e.what())
    # Returns ammount of frames occured every second
    def FramesPerSecond(self):

        time_elapsed = float ( time() - self.time_since_last_update )

        if time_elapsed >= 1:
            frames_per_second = float (self.number_of_frames / time_elapsed)

            self.time_since_last_update = time()

            print ('Frames Per Second: %d' % (frames_per_second))

            self.number_of_frames = 0

        else:
            self.number_of_frames += 1

    # Grab the latest frame recieved
    def get_frame(self):
        return self.current_frame.copy()

    def get_threshold_image(self):
        return self.threshold_image.copy()

    # Closes the stream cleanly
    def end_stream(self):
        self.is_closed = True
