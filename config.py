# Created and Edited by Derek Santos

# Size Ratio: 1 Normal, 0 > Ratio > 1 to decrease size by that number; Ratio > 1 to increase size
SIZE_RATIO = 1/2

# Debugging Settings
DEBUG = True

# Camera Settings

# TODO: I want to list all the sizes here in a comment just for myself.
# Example would be 640, 480

RESOLUTION = ( int (640 * SIZE_RATIO), int (480 * SIZE_RATIO) )
FRAMERATE = 60

# Tracking Settings
# NOTE: THRESHOLD helps sensetivity
MINIMUM_THRESHOLD = 20

# This is the area for Minimum allowed contour
# Once a "sweet spot" is found, just mutliply by a by a ratio to use for higher resolutions
# Units are in Pixel for area of a contour

MIN_AREA = 100 * SIZE_RATIO
