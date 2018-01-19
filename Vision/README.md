# Vision

Codes avec des tests OpenCV pour la calibration de la caméra, la détection du robot, du robot adverse.


### Contents

**undistort.py**: given a camera matrix K and a distorsion coefficients vector d, undistort the provided image.
**detect_chessboard.py**: given a chessboard in the image, detect the chessboard and use point to point correspondance to calibrate K and d.