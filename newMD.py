import io
import time
import cv2

from picamera.array import PiRGBArray
from picamera import PiCamera
from threading import Thread


""" Size Ratio: 1 Normal, 0 > Ratio > 1 to decrease size by that number; Ratio > 1 to increase size """
SIZE_RATIO = 1

RESOLUTION = (640, 480)
FRAMERATE = 60

DEBUG = True


MINIMUM_THRESHOLD = 25



MIN_AREA = 720




class VideoStream:
    def __init__(self, resolution = (1280, 780), framerate = 30):
        self.camera = PiCamera()
        self.camera.resolution = resolution
        self.camera.framerate = framerate
        self.rawFrame = PiRGBArray(self.camera, size = resolution)
        self.stream = self.camera.capture_continuous(self.rawFrame,
                                                     format = 'bgr',
                                                     use_video_port = True)
        self.current_frame = None
        self.is_closed = False
        self.time_since_last_update = time.time()
        self.number_of_frames = 0

    def start_video_stream(self):

        thread = Thread(target = self.refresh_frame, args = ())
        thread.daemon = True
        thread.start()
        return self

    def refresh_frame(self):
        for frame in self.stream:
            
            self.current_frame = frame.array
            self.rawFrame.truncate(0)

            if self.is_closed:
                
                self.stream.close()
                self.rawFrame.close() 
                self.camera.close()
                return
        
        
    def get_frame(self):
        return self.current_frame

    def end_stream(self):
        self.is_closed = True

    def FramePerSecond(self):

        time_elapsed = float ( time.time() - self.time_since_last_update )
        
        if time_elapsed >= 1:
            frames_per_second = float (self.number_of_frames / time_elapsed)
            
            self.time_since_last_update = time.time()

            print ('Frames Per Second: %d' % (frames_per_second))
            #print ('ACTUAL FRAMES: ', self.number_of_frames)
            
            self.number_of_frames = 0
            
        else:
            self.number_of_frames += 1
            
        """if self.number_of_frames >= 100:
            time_elapsed = float ( time.time() - self.time_since_last_update )
            frames_per_second = float ( self.number_of_frames / time_elapsed )
            self.number_of_frames = 0
            self.time_since_last_update = time.time()

            print ('Frames Per Second: %d' %(frames_per_second))
        else:
            self.number_of_frames += 1
            """
   

def main():

    still_tracking = True
    
    print('Setup...')
    
    #video = VideoStream(resolution = (320, 240), framerate = 60).start_video_stream()
    video = VideoStream(resolution = RESOLUTION, framerate = FRAMERATE).start_video_stream()

    time.sleep(2)

    print('Video stream started...')

    current_image = video.get_frame()

    #######current_image = cv2.resize(current_image, (0,0), fx = SIZE_RATIO, fy = SIZE_RATIO)    
    
    base_image = current_image
    print ('Image Size: (Height, Width)', current_image.shape )
    
    base_image_gray = cv2.cvtColor(base_image, cv2.COLOR_BGR2GRAY)

    print ('Tracking has started...')
    
    while still_tracking:
        current_image = video.get_frame()

        


        ######current_image = cv2.resize(current_image, (0,0), fx = SIZE_RATIO, fy = SIZE_RATIO)



        
        current_image_gray = cv2.cvtColor(current_image, cv2.COLOR_BGR2GRAY)
        
        abs_image = cv2.absdiff(base_image_gray.copy(), current_image_gray.copy())
        abs_image = cv2.blur(abs_image, (10, 10))

        _, threshold_image = cv2.threshold(abs_image, MINIMUM_THRESHOLD, 255, cv2.THRESH_BINARY)
      

        threshold_image, contours, hierarchy = cv2.findContours(threshold_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
             
        total_contours = len(contours)

        base_image_gray = current_image_gray.copy()

        biggest_area = MIN_AREA * SIZE_RATIO

        
        motion_found = False
        
        for contour in contours:
            found_area = cv2.contourArea(contour)

            if found_area > biggest_area:
                motion_found = True


        video.FramePerSecond()
        
        if motion_found:
            print ('Motion Found')
            print ('Total Contours', total_contours) 
        
        if DEBUG == True:
            #cv2.imshow('DEBUG', abs_image)
            cv2.imshow('Current_Image_Gray', current_image_gray)

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
        
