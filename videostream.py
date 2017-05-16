from picamera.array import PiRGBArray
from picamera import PiCamera, BufferIO
from threading import Thread
from time import sleep, time
import cv2
import io
import numpy as np

import config
import motor

class FlyContourTracker:
    def __init__(self):
        # Initialize common variables
        self.resolution = (1280, 720)

        # Camera Setup
        self.camera = PiCamera()
        self.camera.resolution = self.resolution
        self.camera.framerate = 20
        self.camera.hflip = False
        self.camera.vflip = False

        # Let Camera Initialize
        #self.camera.start_preview()
        sleep(2)

        # Stream Setup
        self.capture = PiRGBArray(self.camera, size = self.resolution)
        #self.stream = self.camera.capture_continuous(self.capture, format = 'bgr', use_video_port = False, burst=True)

        self.byteCapture = io.BytesIO()

        # Create Normal Frame Thread
        #self.normal_frame_thread = Thread(target = self.get_normal_frame, args = ())
        #self.normal_frame_thread.daemon = True

        # Create Contour Thread
        self.contour_frame_thread = Thread(target = self.contour_detection, args = ())
        self.contour_frame_thread.daemon = True

        # Create Show Frame Thread
        self.show_frame_thread = Thread(target = self.show_frame, args = ())
        self.show_frame_thread.daemon = True

        # Create Check Motion Thread
        self.check_motion_thread = Thread(target = self.check_motion, args = (0,1280))
        self.check_motion_thread.daemon = True


        # When set to true, will close the program
        self.closed = False

        # Stored Frames
        self.normal_frame = None
        self.base_frame = None

        # Contours
        self.contours = None
        self.hierarchy = None

        # FPS Trackers
        self.normal_fps = 0
        self.contour_fps = 0


    def run_contour_detection(self):
        # Start Normal Frame Thread
        #self.normal_frame_thread.start()

        # Start contour Fame Thread
        self.contour_frame_thread.start()

        # Start check motion in a different thread
        self.check_motion_thread.start()

        # When in debug mode, it will show the developer each contour frame
        if config.DEBUG or config.SHOW_VIDEO_INDEPENDENTLY:
            self.show_frame_thread.start()

        return self

    def capturing_video(self):
        columns, rows = self.resolution
        #self.normal_frame = np.empty((rows, columns, 3), dtype=np.uint8)

        fps = 0
        normal_delay = time()

        while not self.closed:

            # Returns a generator
            yield self.byteCapture
            self.byteCapture.seek(0)
            self.normal_frame = np.fromstring(self.byteCapture.getvalue(), dtype=np.uint8).reshape(rows,columns,3)
            self.byteCapture.seek(0)
            self.byteCapture.truncate(0)

            if time() - normal_delay >= 1:
                self.normal_fps += 1
                normal_delay = time()
                if config.DEBUG or config.SHOW_FPS_INDEPENDENTLY:
                    print('Normal FPS:', self.normal_fps)
                self.normal_fps = 0
            else:
                self.normal_fps += 1






    def normal_frame_loop(self):
        #normal_delay = time()

        #for frame in self.camera.capture_continuous(self.byteCapture, format = 'bgr', use_video_port = False, burst=True):
        #for frame in self.camera.capture_continuous(self.capture, format = 'bgr', use_video_port = False, burst=True):
        #while not self.closed:
        try:
            # if closed
            #if self.closed:
            #    return

            #columns, rows = self.resolution

            #self.normal_frame = frame.array
            #self.normal_frame = np.fromstring(frame.getvalue(), dtype=np.uint8).reshape(rows, columns, 3)


            self.camera.capture_sequence(self.capturing_video(), 'bgr', use_video_port = True)


        finally:

        #    # FPS Counter
        #    if config.DEBUG or config.SHOW_FPS_INDEPENDENTLY:
        #        if time() - normal_delay >= 1:
        #            self.normal_fps += 1
        #            normal_delay = time()
        #            print('Normal FPS:', self.normal_fps)
        #            self.normal_fps = 0
        #        else:
        #            self.normal_fps += 1

        #    #self.byteCapture.truncate(0)
        #    #self.byteCapture.seek(0)

        #    self.capture.seek(0)
        #    self.capture.truncate(0)
            pass





    def contour_detection(self):

        # Initialize FPS conouter variables
        contour_delay = time()
        while not self.closed:

            try:
                if self.normal_frame == None:
                    continue

                if self.base_frame == None:
                    self.base_frame = self.normal_frame.copy()
                    base_gray = cv2.cvtColor(self.base_frame.copy(), cv2.COLOR_BGR2GRAY)

                # Convert to Gray
                normal_gray = cv2.cvtColor(self.normal_frame.copy(), cv2.COLOR_BGR2GRAY)

                # Get absolute difference of previous frame to current frame
                difference_frame = cv2.absdiff(normal_gray.copy(), base_gray)
                difference_frame = cv2.blur(difference_frame, (10, 10))

                # Apply threshold to extract contours
                _, threshold_image = cv2.threshold(difference_frame, config.MINIMUM_THRESHOLD, 255, cv2.THRESH_BINARY)

                # Locate contours
                threshold_image, self.contours, self.hierarchy = cv2.findContours(threshold_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
                #print('contours checked')
                #print(len(self.contours))

                base_gray = normal_gray.copy()

                # Contour FPS Counter
                if config.DEBUG or config.SHOW_FPS_INDEPENDENTLY:
                    if time() - contour_delay >= 1:
                        # Updates the class to current time
                        contour_delay = time()

                        # Shows FPS data
                        print ('Contour FPS:', self.contour_fps)

                        # Sets fps back to zero
                        self.contour_fps = 0

                    else:
                        self.contour_fps += 1

            except Exception as e:
                print ('ERROR - Contour Detection:', e)




    def show_frame(self):

        while not self.closed:
            try:
                if (self.normal_frame == None):
                    continue

                # Show contour Frame
                if self.contours != None:
                    cv2.imshow('Image', self.get_contour_frame())

                # If 'q' is pressed then it will quit the program.
                if cv2.waitKey(1) == ord('q'):
                    cv2.destroyAllWindows()
                    self.closed = True


            except Exception as e:
                print ('ERROR - Show Frame:', e)

    def get_contour_frame(self):

        contour_frame = cv2.drawContours(self.normal_frame.copy(), self.contours, -1, (128, 255, 0), 3)

        if contour_frame != None:
            return contour_frame
        else:
            return self.normal_frame.copy()


    def check_motion(self, leftbounds, rightbounds):
        #NOTE: The plan for this function is to be threaded with each thread being a different section of the frame

        #TODO: need to a bool or something to communicate between each of the x many threads
        #TODO: just a thought but this could be made as if self.motor_alive == True: dont track, else start motor after 15 seconds

        # If there has not been any motion for at least MIN_TIME
        # then we shake the viles to see if the flies are drunk
        # This will be in it's own thread

        time_at_last_update = time()
        duration_since_last_update = 0
        step_motor = motor.Motor()

        # old_contours prevents the program from processing the same contours over and overagain
        old_contours = None
        while not self.closed:
            try:


                if self.contours != None and len(self.contours) != 0:
                    same_contours = False
                    if len(old_contours) == range(len(self.contours)):
                        for i in len(old_contours):
                            if self.contours[i] == old_contours[i]:
                                same_contours = True

                        if same_contours:
                            continue


                    for contour in self.contours:
                        if cv2.contourArea(contour) <= config.MIN_AREA:
                            continue

                        x, y, w, h = cv2.boundingRect(contour)

                        if leftbounds < x and x < rightbounds:
                            #total_allowed_contours += 1
                            time_at_last_update = time()
                            if config.DEBUG or config.STATE_MOTION_FOUND_INDEPENDENTLY:
                                print ('MOTION FOUND', time())


                # Calculating duration since last update
                duration_since_last_update = time() - time_at_last_update

                # If divisable by 15 seconds
                if int(duration_since_last_update) % 15 == 0 and duration_since_last_update >= 10:
                    if config.DEBUG:
                        print('It has been {} seconds'.format(duration_since_last_update))

                    # Step function automatically disables the tracking
                    step_motor.step()

                #print('It has been {} seconds'.format(duration_since_last_update))

                #TODO: Need to add a start and end to each "session"
                if self.contours != None:
                    old_contours = self.contours.copy()

            except Exception as e:
                print ('ERROR Check motion:', e)

    def get_summary(self):
        return 'Normal Frame Processing FPS: {}, Contour Processing FPS: {}'.format(self.normal_fps, self.contour_fps)
    def is_alive(self):
        return not self.closed
