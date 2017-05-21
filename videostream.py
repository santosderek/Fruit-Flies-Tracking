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
    def __init__(self, res = (1280, 720), framerate = 60):
        # Initialize common variables
        self.resolution = res

        # Camera Setup
        self.camera = PiCamera()
        self.camera.resolution = self.resolution
        self.camera.framerate = framerate

        self.camera.hflip = False
        self.camera.vflip = False

        # Let Camera Initialize
        sleep(2)

        # Stored Frames
        self.normal_frame = None
        self.normal_gray = None
        self.vile_1_frame = None
        self.vile_2_frame = None
        self.vile_3_frame = None
        self.vile_4_frame = None

        # Stream Setup
        self.capture = PiRGBArray(self.camera, size = self.resolution)
        self.byteCapture = io.BytesIO()

        # Create Contour Thread
        self.normal_to_gray_thread = Thread(target = self.normal_to_gray_constant_conversion, args = ())
        self.normal_to_gray_thread.daemon = True

        # Create Check Motion Thread
        self.check_motion_thread_vile_1 = Thread(target = self.contour_detection, args = ('Vile_1', 0, 640))
        self.check_motion_thread_vile_1.daemon = True

        # Create Check Motion Thread for vile 2
        self.check_motion_thread_vile_2 = Thread(target = self.contour_detection, args = ('Vile_2', 1280/2, 1280))
        self.check_motion_thread_vile_2.daemon = True

        # Create Show Video for vile 2
        self.show_video_thread = Thread(target = self.show_frame, args = ())
        self.show_video_thread.daemon = True

        # When set to true, will close the program
        self.closed = False



        # Contours
        self.contours = None
        self.hierarchy = None

        # FPS Trackers
        self.normal_fps = 0
        self.contour_fps = 0
        self.checking_fps = 0

        #Initialize step motor
        self.step_motor = motor.Motor()



    def run_contour_detection(self):

        # Start contour Fame Thread
        self.normal_to_gray_thread.start()

        # Start check motion in a different thread
        self.check_motion_thread_vile_1.start()
        self.check_motion_thread_vile_2.start()
        if config.DEBUG or config.SHOW_VIDEO_INDEPENDENTLY:
            self.show_video_thread.start()

        return self

    def capturing_video(self):
        columns, rows = self.resolution
        fps = 0
        normal_delay = time()

        while not self.closed:
            # Returns a generator
            try:
                yield self.byteCapture
                self.byteCapture.seek(0)
                self.normal_frame = np.fromstring(self.byteCapture.getvalue(), dtype=np.uint8).reshape(rows,columns,3)
                self.byteCapture.seek(0)
                self.byteCapture.truncate(0)
            except Exception as e:
                print ('ERROR - capturing_video:', e)
            finally:
                if time() - normal_delay >= 1:
                    self.normal_fps += 1
                    normal_delay = time()
                    if config.DEBUG or config.SHOW_FPS_INDEPENDENTLY or config.SHOW_NORMAL_FPS_INDEPENDENTLY:
                        print('Normal Frame Ticks:', self.normal_fps)
                    self.normal_fps = 0
                else:
                    self.normal_fps += 1

    def normal_frame_loop(self):
        self.camera.capture_sequence(self.capturing_video(), 'bgr', use_video_port = True)

    def normal_frame_usb_camera(self):
        capture_device = cv2.VideoCapture(0)
        normal_delay = time()

        while not self.closed:
            try:
                ret, self.normal_frame = capture_device.read()
            except Exception as e:
                print ('ERROR - normal_frame_usb_camera:', e)
            finally:
                if time() - normal_delay >= 1:
                    self.normal_fps += 1
                    normal_delay = time()
                    if config.DEBUG or config.SHOW_FPS_INDEPENDENTLY or config.SHOW_NORMAL_FPS_INDEPENDENTLY:
                        print('Normal Frame Ticks:', self.normal_fps)
                    self.normal_fps = 0
                else:
                    self.normal_fps += 1


    def normal_to_gray_constant_conversion(self):
        # Initialize FPS conouter variables
        contour_delay = time()
        old_normal_frame = None
        while not self.closed:

            try:
                if self.normal_frame is None:
                    continue

                if old_normal_frame is None:
                    old_normal_frame = self.normal_frame.copy()
                    continue

                if (old_normal_frame == self.normal_frame).all():
                    continue

                # Convert to Gray
                self.normal_gray = cv2.cvtColor(self.normal_frame, cv2.COLOR_BGR2GRAY)
                old_normal_frame = self.normal_frame.copy()


            except Exception as e:
                print ('ERROR - Contour Detection:', e)

            finally:
                # Contour FPS Counter
                if config.DEBUG or config.SHOW_FPS_INDEPENDENTLY or config.SHOW_N2G_FPS_INDEPENDENTLY:
                    if time() - contour_delay >= 1:
                        # Updates the class to current time
                        contour_delay = time()

                        # Shows FPS data
                        print ('Normal to Gray Ticks:', self.contour_fps)

                        # Sets fps back to zero
                        self.contour_fps = 0

                    else:
                        self.contour_fps += 1

    def show_frame(self):

        while not self.closed:
            try:
                # If 'q' is pressed then it will quit the program.
                if self.vile_1_frame is None:
                    continue
                if self.vile_2_frame is None:
                    continue
                #if self.vile_3_frame is None:
                #    continue
                #if self.vile_4_frame is None:
                #    continue
                cv2.imshow('Vile_1', self.vile_1_frame)
                cv2.imshow('Vile_2', self.vile_2_frame)
                #cv2.imshow('Vile_3', self.vile_3_frame)
                #cv2.imshow('Vile_4', self.vile_4_frame)

                if cv2.waitKey(1) == ord('q'):
                    cv2.destroyAllWindows()
                    self.closed = True


            except Exception as e:
                print (name, 'ERROR - Show Frame:', e)


    def contour_detection(self, name, leftbounds, rightbounds):

        # Initialize variables for duration
        time_at_last_update = time()
        checking_buffer = time()
        base_gray = None
        current_frame  = None
        contours = None

        # old_contours prevents the program from processing the same contours over and overagain
        while not self.closed:
            try:
                # Ruling out any unnessesary runs

                if self.normal_gray is None:
                    continue

                current_frame = self.normal_gray[0:self.resolution[1], leftbounds:leftbounds + (rightbounds - leftbounds)]

                #print('x:{},y:{},w:{},h:{}'.format(leftbounds, 0, rightbounds - leftbounds, self.resolution[1]))
                #print (current_frame.shape)

                if base_gray is None:
                    base_gray = current_frame.copy()
                    continue
                if (base_gray == current_frame).all():
                    continue

                difference_frame = cv2.absdiff(current_frame, base_gray)
                blured_frame = cv2.blur(difference_frame, (5, 5))

                _, threshold_image = cv2.threshold(blured_frame, config.MINIMUM_THRESHOLD, 255, cv2.THRESH_BINARY)
                _, contours, hierarchy = cv2.findContours(threshold_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

                if contours != None and len(contours) != 0:

                    motion_found = False
                    for contour in contours:
                        # If contour is not within the range desired, skip the contour
                        # AND if there already has been motion found then skip the rest
                        if cv2.contourArea(contour) <= config.MIN_AREA and motion_found:
                            continue

                        # Get the dimension of the bounding rectangle
                        x, y, w, h = cv2.boundingRect(contour)

                        # Gets the middle x corrdinate of the rectangle
                        middle_of_contour = x + (w/2)

                        # If motion found:
                        if leftbounds < middle_of_contour and middle_of_contour < rightbounds:
                            time_at_last_update = time()
                            motion_found = True
                            if config.DEBUG or config.STATE_MOTION_FOUND_INDEPENDENTLY:
                                print (name, 'Motion Found:', time())

                # Calculating duration since last update
                duration_since_last_update = time() - time_at_last_update

                # If divisable by 20 seconds
                if int(duration_since_last_update) % 20 == 0 and duration_since_last_update >= 10:
                    if config.DEBUG:
                        print(name, ': It has been {} seconds'.format(duration_since_last_update))

                    # Step function automatically delays the tracking
                    # If step motor is already shaking then wait a while;
                    if self.step_motor.is_shaking:
                        while (self.step_motor.is_shaking):
                            sleep(1)
                    else:
                        self.step_motor.step()


                base_gray = current_frame.copy()

                if config.DEBUG or config.SHOW_VIDEO_INDEPENDENTLY:

                    if name == 'Vile_1':
                        self.vile_1_frame = cv2.drawContours(current_frame.copy(), contours, -1, (128,255,0), 3)
                    if name == 'Vile_2':
                        self.vile_2_frame = cv2.drawContours(current_frame.copy(), contours, -1, (128,255,0), 3)
                    if name == 'Vile_3':
                        self.vile_3_frame = cv2.drawContours(current_frame.copy(), contours, -1, (128,255,0), 3)
                    if name == 'Vile_4':
                        self.vile_4_frame = cv2.drawContours(current_frame.copy(), contours, -1, (128,255,0), 3)



            except Exception as e:
                print (name, 'ERROR Contour Detection:', e)
            finally:

                if time() - checking_buffer >= 1:
                    self.checking_fps += 1
                    checking_buffer = time()
                    if config.DEBUG or config.SHOW_FPS_INDEPENDENTLY or config.SHOW_CONTOUR_FPS_INDEPENDENTLY:
                        print(name, ': Contour Tracking Ticks:', self.checking_fps)
                    self.checking_fps = 0
                else:
                    self.checking_fps += 1
