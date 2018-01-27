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

#K = np.array([[  750,     0.  ,  1296],
#              [    0.  ,   750,  1296],
#              [    0.  ,     0.  ,     1.  ]])



# Termination criteria for the chessboard detection algorithm
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 50, 0.001)

# Chessboard parameters (the dimensions depict the number of cells to be detected)
width = 3
length = 3

# Prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
# These will be the coordinated of the chessboard cells intersections in the local referential of the chessboard
objp = np.zeros((width*length,3), np.float32)
objp[:,:2] = np.mgrid[0:width,0:length].T.reshape(-1,2)

# Arrays to store object points and image points from all the images.
objpoints = [] # 3d point in real world space
imgpoints = [] # 2d points in image plane.

# Read test image
img = cv2.imread("chessboard_10.jpg")
gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

# Find the chess board corners
ret, corners = cv2.findChessboardCorners(gray, (width,length),None)

# Did it succeed?
print(ret)

# If found, add object points, image points (after refining them)
if ret == True:
    objpoints.append(objp)
    
    cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
    imgpoints.append(corners)
    
    # Draw and display the corners
    cv2.drawChessboardCorners(img, (width,length), corners,ret)
    cv2.imwrite('detected_chessboard.png',img)
    
    # Get camera calibration parameters
    ret2, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints,imgpoints,gray.shape[::-1],None,None)
    print(mtx)
    print(dist)
    print(rvecs)
    print(tvecs)

cv2.destroyAllWindows()

              
#K = np.array([[  1.59505578e+03,   0.00000000e+00,   3.02300149e+02],
# [  0.00000000e+00,   1.50489287e+03,   2.54612086e+02],
# [  0.00000000e+00,   0.00000000e+00,   1.00000000e+00]])

K = mtx

print(K.shape)

# zero distortion coefficients work well for this image
#D = np.array([0., 0., 0., 0.])
#~ D = np.array([0.1, 0.1, 0.1, 0.1])
#D = np.array([ -2.24802356e+01,   4.98688017e+02,  -2.50368142e-01,   3.63708910e-01])#,-9.45697355e-01

D = dist[:-1]

# use Knew to scale the output
Knew = K.copy()
Knew[(0,1), (0,1)] = 0.4 * Knew[(0,1), (0,1)]
#~ Knew[(0,1), (0,1)] = 0.1 * Knew[(0,1), (0,1)]


Kfin = np.array([[  750,     0.  ,  320],
              [    0.  ,   750,  240    ],
              [    0.  ,     0.  ,     1.  ]])


## Undistort the provided image and 

# Read image
img = cv2.imread('chessboard_10_red.jpg')

# Add borders to broaden field of view
#img = cv2.copyMakeBorder(img,324,324,0,0,cv2.BORDER_DEFAULT)

# Undistort the image
img_undistorted = cv2.fisheye.undistortImage(img, K, D=D, Knew=Knew)

# Save or display the image
cv2.imwrite('chessboard_10_red_undistorted.jpg', img_undistorted)
#~ cv2.imshow('undistorted', img_undistorted)
cv2.waitKey()
