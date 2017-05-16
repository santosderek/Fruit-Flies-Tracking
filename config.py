# Created and Edited by Derek Santos

# Size Ratio: 1 Normal, 0 > Ratio > 1 to decrease size by that number; Ratio > 1 to increase size
SIZE_RATIO = 1

# Debugging Settings
DEBUG = False
SHOW_FPS_INDEPENDENTLY = True
STATE_MOTION_FOUND_INDEPENDENTLY = False
SHOW_VIDEO_INDEPENDENTLY = True
# Camera Settings
RESOLUTION = ( int (1280 * SIZE_RATIO), int (720 * SIZE_RATIO) )
#RESOLUTION = ( int (640 * SIZE_RATIO), int (480 * SIZE_RATIO) )
FRAMERATE = 60

# Tracking Settings
# NOTE: THRESHOLD helps sensetivity
MINIMUM_THRESHOLD = 20

# This is the area for Minimum allowed contour
# Once a "sweet spot" is found, just mutliply by a by a ratio to use for higher resolutions
# Units are in Pixel for area of a contour

MIN_AREA = 5 * SIZE_RATIO
