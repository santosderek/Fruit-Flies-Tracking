# Size Ratio: 1 Normal, 0 > Ratio > 1 to decrease size by that number; Ratio > 1 to increase size
SIZE_RATIO = 1

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
