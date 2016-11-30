CC=g++
CFLAGS= -lopencv_calib3d -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres  -lopencv_video -lopencv_videoio -lopencv_videostab
OBJECTS= main.o cameraFeed.o errors.o

all: FruitFlies

FruitFlies: $(OBJECTS)
	$(CC) $(OBJECTS) -o FruitFlies

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

errors.o: errors.cpp
	$(CC) $(CFLAGS) errors.cpp

cameraFeed.o: camera_feed.cpp
	$(CC) $(CFLAGS) camera_feed.cpp

clean:
	rm *.o
