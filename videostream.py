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

        # Create Tracking Threads
        self.thread_list = []
        amount_of_threads = 4

        start = 0
        width = self.resolution[0] / amount_of_threads
        end = 0 + width


        for count in range(amount_of_threads):

            # For every thread bellow last thread
            if count < amount_of_threads - 1:
                self.thread_list.append(Thread(target = self.contour_detection, args = ('Vile_{}'.format(count + 1), start, end)))

            # If last thread
            else:
                self.thread_list.append(Thread(target = self.contour_detection, args = ('Vile_{}'.format(count + 1), start, self.resolution[0])))

            # Increment the left and right bounds
            start += width
            end += width


        # Shows the amount of Threads
        print ('THREAD LIST:', len(self.thread_list))

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

        # Start check motion in different threads
        # This for loop with start all the threads in the list
        for thread in self.thread_list:
            thread.start()

        # If Debugging, show the frames that are collected
        if config.DEBUG or config.SHOW_VIDEO_INDEPENDENTLY:
            self.show_video_thread.start()

        return self

    def increment_tick_rate(self, current_tick, time_buffer):
        if time() - time_buffer >= 1:
            # Resets the tickrate and timebuffer, and returns the old tick rate
            # Returns 0, time(), and old_tick_rate
            return 0, time(), current_tick + 1
        else:
            # Returns current_tick + 1, old time buffer, No old tick rate
            return current_tick + 1, time_buffer, None


    def capturing_video(self):
        normal_delay = time()

        while not self.closed:
            # Returns a generator
            try:
                yield self.byteCapture
                self.byteCapture.seek(0)
                self.normal_frame = np.fromstring(self.byteCapture.getvalue(), dtype=np.uint8).reshape(self.resolution[1], self.resolution[0], 3)
                self.byteCapture.seek(0)
                self.byteCapture.truncate(0)
            except Exception as e:
                print ('ERROR - capturing_video:', e)
            finally:
                self.normal_fps, normal_delay, old_tick = self.increment_tick_rate(self.normal_fps, normal_delay)

                if config.DEBUG or config.SHOW_FPS_INDEPENDENTLY or config.SHOW_NORMAL_FPS_INDEPENDENTLY:
                    if not old_tick is None:
                        print('Normal Frame Ticks:', old_tick)


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
                self.normal_fps, normal_delay, old_tick = self.increment_tick_rate(self.normal_fps, normal_delay)

                if config.DEBUG or config.SHOW_FPS_INDEPENDENTLY or config.SHOW_NORMAL_FPS_INDEPENDENTLY:
                    if not old_tick is None:
                        print('Normal Frame Ticks:', old_tick)


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
                self.contour_fps, contour_delay, old_tick = self.increment_tick_rate(self.contour_fps, contour_delay)

                if config.DEBUG or config.SHOW_FPS_INDEPENDENTLY or config.SHOW_N2G_FPS_INDEPENDENTLY:
                    if not old_tick is None:
                        print ('Normal to Gray Ticks:', old_tick)


    def show_frame(self):

        while not self.closed:
            try:
                # If 'q' is pressed then it will quit the program.
                if not self.vile_1_frame is None:
                    cv2.imshow('Vile_1', self.vile_1_frame)

                if not self.vile_2_frame is None:
                    cv2.imshow('Vile_2', self.vile_2_frame)

                if not self.vile_3_frame is None:
                    cv2.imshow('Vile_3', self.vile_3_frame)

                if not self.vile_4_frame is None:
                    cv2.imshow('Vile_4', self.vile_4_frame)

                if cv2.waitKey(1) == ord('q'):
                    cv2.destroyAllWindows()
                    self.closed = True


            except Exception as e:
                print (name, 'ERROR - Show Frame:', e)


    def contour_detection(self, name, leftbounds, rightbounds):

        # Initialize variables for duration
        time_at_last_update = time()
        checking_buffer_print_delay = time()
        base_gray = None
        current_frame  = None
        #contours = None
        allowed_contours = []
        tick_rate = 0
        tick_rate_for_frame = 0
        # old_contours prevents the program from processing the same contours over and overagain
        while not self.closed:
            try:
                # Ruling out any unnessesary runs

                # If Camera hasn't started yet, skip processing
                if self.normal_gray is None:
                    continue

                # Delay contour tracking if motor is shaking flies
                while self.step_motor.is_shaking:
                    #print (name, ' is waiting...')
                    sleep(1)

                # Segment the normal gray frame using left and right bounds
                current_frame = self.normal_gray[0:self.resolution[1], leftbounds:leftbounds + (rightbounds - leftbounds)]

                # If there is no base frame, set current as the base, and skip processing
                if base_gray is None:
                    base_gray = current_frame.copy()
                    continue

                # If the base frame and current are both the same, skip processing
                # ... no need to process the same frame
                if (base_gray == current_frame).all():
                    continue

                # Start Processing

                # Get the difference frame and blur it
                difference_frame = cv2.absdiff(current_frame, base_gray)
                blured_frame = cv2.blur(difference_frame, (10, 10))

                # Process the blured frame under a threshold, and get the contours
                _, threshold_image = cv2.threshold(blured_frame, config.MINIMUM_THRESHOLD, 255, cv2.THRESH_BINARY)
                _, contours, hierarchy = cv2.findContours(threshold_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

                if contours != None and len(contours) != 0:

                    # Set motion found to false, and start gathering allowed contours
                    motion_found = False
                    count_contour = 0

                    # Process each contour until motion found
                    for contour in contours:
                        contour_area = cv2.contourArea(contour)
                        # if not allowed
                        if contour_area < config.MIN_AREA or motion_found:
                            continue
                        # if allowed
                        elif contour_area >= config.MIN_AREA:
                            # If motion was found already then we can disregard the other contours
                            motion_found = True
                            # Update the time last motion was found
                            time_at_last_update = time()
                            # Include this contour into the list of contours that were found
                            allowed_contours.append(count_contour)
                            # Debugging:
                            if config.DEBUG or config.STATE_MOTION_FOUND_INDEPENDENTLY:
                                print (name, 'Motion Found:', time())

                            if config.DEBUG or config.SHOW_CONTOUR_AREA:
                                print (name, ', AREA:', contour_area)

                        count_contour += 1

                # Calculating duration since last update
                duration_since_last_update = time() - time_at_last_update

                # If divisable by 20 seconds
                if int(duration_since_last_update) % 20 == 0 and duration_since_last_update >= 10:
                    if config.DEBUG or True:
                        print(name, ': It has been {} seconds'.format(int(duration_since_last_update)))

                    # Step function automatically delays the tracking
                    # If step motor is already shaking then wait a while;
                    if self.step_motor.is_shaking:
                        while (self.step_motor.is_shaking):
                            sleep(1)
                    else:
                        print (name, ' has started motor...')
                        self.step_motor.step()
                        print (name, ' has ended motor')


                base_gray = current_frame.copy()

                # Update contour frame
                if config.DEBUG or config.SHOW_VIDEO_INDEPENDENTLY:

                    if name == 'Vile_1':
                        if len(allowed_contours) > 0:
                            for pos in allowed_contours:
                                self.vile_1_frame = cv2.drawContours(current_frame.copy(), contours, pos, (128,255,0), 3)
                        else:
                            self.vile_1_frame = current_frame.copy()

                        cv2.putText(self.vile_1_frame, 'Ticks Per Second: %s' % tick_rate_for_frame, (10, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, 255 )
                    elif name == 'Vile_2':
                        if len(allowed_contours) > 0:
                            for pos in allowed_contours:
                                self.vile_2_frame = cv2.drawContours(current_frame.copy(), contours, pos, (128,255,0), 3)
                        else:
                            self.vile_2_frame = current_frame.copy()

                        cv2.putText(self.vile_2_frame, 'Ticks Per Second: %s' % tick_rate_for_frame, (10, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, 255 )

                    elif name == 'Vile_3':
                        if len(allowed_contours) > 0:
                            for pos in allowed_contours:
                                self.vile_3_frame = cv2.drawContours(current_frame.copy(), contours, pos, (128,255,0), 3)
                        else:
                            self.vile_3_frame = current_frame.copy()

                        cv2.putText(self.vile_3_frame, 'Ticks Per Second: %s' % tick_rate_for_frame, (10, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, 255 )

                    elif name == 'Vile_4':
                        if len(allowed_contours) > 0:
                            for pos in allowed_contours:
                                self.vile_4_frame = cv2.drawContours(current_frame.copy(), contours, pos, (128,255,0), 3)
                        else:
                            self.vile_4_frame = current_frame.copy()

                        cv2.putText(self.vile_4_frame, 'Ticks Per Second: %s' % tick_rate_for_frame, (10, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, 255 )

                # Clear the list of contours to show
                allowed_contours.clear()


            except Exception as e:
                print (name, 'ERROR Contour Detection:', e)
            finally:
                tick_rate, checking_buffer_print_delay, old_tick_rate = self.increment_tick_rate(tick_rate, checking_buffer_print_delay)

                if not old_tick_rate is None:
                    tick_rate_for_frame = old_tick_rate

                if config.DEBUG or config.SHOW_FPS_INDEPENDENTLY or config.SHOW_CONTOUR_FPS_INDEPENDENTLY:
                    if not old_tick_rate is None:
                        print(name, ': Contour Tracking Ticks:', old_tick_rate)
