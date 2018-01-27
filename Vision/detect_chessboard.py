###
# Given an image in which a chessboard is present, compute K and d. This is meant to be
# used as camera calibration.
# First, define chessboard cell intersection as points in the local referential.
# Then, use OpenCV function to detect these points (chessboard)
# Finally, use OpenCV function to compute K, d corresponding to this transformation.
#
# 14chanwa
# 14rodubo
# Jan, 7 2018
###


import numpy as np
import cv2
import glob

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
img = cv2.imread("chessboard_10_red.jpg")
gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

# Find the chess board corners
ret, corners = cv2.findChessboardCorners(gray, (width,length),None)

# Did it succeed?
print(ret)
print(corners)

# If found, add object points, image points (after refining them)
if ret == True:
    objpoints.append(objp)
    
    cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
    imgpoints.append(corners)
    
    print(imgpoints)
    print(imgpoints[0].shape)
    print(type(imgpoints[0]))
    print(type(imgpoints[0][0]))
    
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
