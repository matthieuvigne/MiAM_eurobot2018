# -*- coding: utf-8 -*-
"""
Created on Sat Jan 27 16:23:41 2018

@author: Quentin
"""

import numpy as np
import cv2
import sys

# You should replace these 3 lines with the output in calibration step
DIM=(2592, 1944)
K=np.array([[573.6192050641611, 0.0, 1774.3113274734023], [0.0, 1408.703595417901, 2059.2587981631586], [0.0, 0.0, 1.0]])
D=np.array([[-0.14503449678445798, -0.29975906060455243, -0.1658672096676059, 0.02495240642894691]])

#DIM=(2592, 1944)
#K=np.array([[-0.24349914134905026, -0.0, 1665.086230352619], [0.0, 2874.158382224394, 848.6163239966586], [0.0, 0.0, 1.0]])
#D=np.array([[-1230371.2377231903], [-784519.008633554], [974448340.5979233], [-275460775128.9717]])

def undistort(img_path):
    img = cv2.imread(img_path)
    h,w = img.shape[:2]
    map1, map2 = cv2.fisheye.initUndistortRectifyMap(K, D, np.eye(3), K, DIM, cv2.CV_16SC2)
    undistorted_img = cv2.remap(img, map1, map2, interpolation=cv2.INTER_LINEAR, borderMode=cv2.BORDER_CONSTANT)
#    cv2.imshow("undistorted", undistorted_img)
#    cv2.waitKey(0)
#    cv2.destroyAllWindows()
    cv2.imwrite("output.jpg", undistorted_img)
if __name__ == '__main__':
    for p in sys.argv[1:]:
        undistort(p)
        
undistort("chessboard_9.jpg")