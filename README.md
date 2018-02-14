# ImageEnhancementsAndGeometricTransformations
Implementation of image resize, rotation, translation, shear, image negatives, log transformation, gamma transformation, contrast stretching, thresholding, intensity slicing, bitplane slicing, image reconstruction from tie points, histogram equalization, adaptive histogram equalization and histogram matching using OpenCV C++ without using inbuilt functions.

Complie and Run
................
g++ -o  a2_paras_kumar__2016csb1047  a2_paras_kumar_2016csb1047.cpp `pkg-config opencv --cflags --libs`

./a2_paras_kumar__2016csb1047

Comments:

1.Running the tie points subprogram requires input in format:
x1 x4 y1 y4
x2 x5 y2 y5
x3 x6 y3 y6 (this line will be 0 0 0 0 in case of non translational correction)

where,
INPUT POINTS	OUTPUT POINTS
(x1, y1)      ->    (x4 ,y4)
(x2, y2)      ->    (x5 ,y5)
(x3, y3)      ->    (x6 ,y6)

Note: *Example Data and image names are provided during run for easy testing.(I have included two images q.jpg and q1.jpg for this test)

2.Rmse is dispalyed for Resizing both bilinear and nearest neighbour, histogram equalization and histogram matching in rmse and psnr.

3.I have used same interpolation function for resize, shear and rotation. And the rmse in case of reisze comes around 8 for bilinear
  and 0 for nearest neighbour.

4.There are options for selecting type of interpolation in each geometric transformation.

5.Adaptive histogram takes long time for HD images, so try with poor quality images only.
