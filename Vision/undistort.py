###
# Given a camera matrix K and distorsion coefficients D, undistort the provided image.
# 14chanwa
# 14rodubo
#
# Jan, 7 2018
###

import cv2
import numpy as np

assert float(cv2.__version__.rsplit('.', 1)[0]) >= 3, 'OpenCV version 3 or newer required.'


## Define K and d, camera matrix and distorsion coefficients

#~ K = np.array([[  689.21,     0.  ,  1295.56],
              #~ [    0.  ,   690.48,   942.17],
              #~ [    0.  ,     0.  ,     1.  ]])

K = np.array([[  750,     0.  ,  1296],
              [    0.  ,   750,  1296],
              [    0.  ,     0.  ,     1.  ]])

# zero distortion coefficients work well for this image
D = np.array([0., 0., 0., 0.])
#~ D = np.array([0.1, 0.1, 0.1, 0.1])

# use Knew to scale the output
Knew = K.copy()
Knew[(0,1), (0,1)] = 0.4 * Knew[(0,1), (0,1)]
#~ Knew[(0,1), (0,1)] = 0.1 * Knew[(0,1), (0,1)]


## Undistort the provided image and 

# Read image
img = cv2.imread('fisheye_sample.jpg')

# Add borders to broaden field of view
img = cv2.copyMakeBorder(img,324,324,0,0,cv2.BORDER_DEFAULT)

# Undistort the image
img_undistorted = cv2.fisheye.undistortImage(img, K, D=D, Knew=Knew)

# Save or display the image
cv2.imwrite('fisheye_sample_undistorted.jpg', img_undistorted)
#~ cv2.imshow('undistorted', img_undistorted)
cv2.waitKey()
