# -*- coding: utf-8 -*-
"""
Created on Sat Jan 27 17:07:41 2018

@author: Quentin
"""

import numpy as np
import matplotlib.pyplot as plt
import cv2

im = plt.imread("chessboard_9.jpg")

pts = np.array([\
    [1752, 568],\
    [1765, 642],\
    [1774, 723],\
    [1779, 812],\
    [1780, 907],\
    [1777, 1004],\
    [1773, 1105], # col\
    [1678, 538],\
    [1688, 615],\
    [1696, 699],\
    [1700, 793],\
    [1700, 894],\
    [1697, 997],\
    [1692, 1102], # col\
    [1593, 510],\
    [1600, 587],\
    [1608, 675],\
    [1610, 771],\
    [1609, 878],\
    [1604, 987],\
    [1598, 1100], # col\
    [1502, 483],\
    [1502, 561],\
    [1505, 650],\
    [1505, 752],\
    [1500, 862],\
    [1498, 978],\
    [1492, 1096]
   ])

ptslist = pts.reshape((pts.shape[0], 1, pts.shape[1])).astype(np.float32)

plt.imshow(im)
plt.scatter(x=pts[:, 0], y = pts[:, 1])
plt.show()

objpoints = [] # 3d point in real world space
imgpoints = []

# Chessboard parameters (the dimensions depict the number of cells to be detected)
width = 4
length = 7
CHECKERBOARD = (4, 7)

# Prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
# These will be the coordinated of the chessboard cells intersections in the local referential of the chessboard
objp = np.zeros((1, CHECKERBOARD[0]*CHECKERBOARD[1], 3), np.float32)
objp[0,:,:2] = np.mgrid[0:CHECKERBOARD[0], 0:CHECKERBOARD[1]].T.reshape(-1, 2)


objpoints.append(objp)

cv2.drawChessboardCorners(im, (width,length), ptslist ,True)
cv2.imwrite('detected_chessboard.png',im)

print(objpoints)
imgpoints.append(ptslist)

gray = cv2.cvtColor(im,cv2.COLOR_BGR2GRAY)
K = np.zeros((3, 3))
D = np.zeros((4, 1))
N_OK = len(objpoints)
rvecs = [np.zeros((1, 1, 3), dtype=np.float64) for i in range(N_OK)]
tvecs = [np.zeros((1, 1, 3), dtype=np.float64) for i in range(N_OK)]
calibration_flags = cv2.fisheye.CALIB_RECOMPUTE_EXTRINSIC+cv2.fisheye.CALIB_FIX_SKEW
rms, _, _, _, _ = \
    cv2.fisheye.calibrate(
        objpoints,
        imgpoints,
        gray.shape[::-1],
        K,
        D,
        rvecs,
        tvecs,
        calibration_flags,
        (cv2.TERM_CRITERIA_EPS+cv2.TERM_CRITERIA_MAX_ITER, 30, 1e-6)
    )

print(K)
print(D)
#print(rvecs)
#print(tvecs)


print("DIM=" + str(im.shape[:2][::-1]))
print("K=np.array(" + str(K.tolist()) + ")")
print("D=np.array(" + str(D.tolist()) + ")")