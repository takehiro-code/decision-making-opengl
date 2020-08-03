
# HW Discussion and Results

### HW1

### HW2
**Problem 1**
Overlay 1D gaussian distribution on the histogram for the two datasets latitude and longitude are shown below.
We set the number of bin to **30** and used the entire data samples **14092** points. The data is taken from 2001-01-02 to 2018-11-27 for 18 years.
![ScreenShot](hw2_problem1_latitude.PNG)
![ScreenShot](hw2_problem1_longitude.PNG)
For the result for dataset latitude, the histogram is following the gaussian shape and the mean value of latitude is approximately the highest frequency of the histogram and is **37.41 degree**.
Similarly for the longitude dataset, the mean value (approximately highest frequency) is **142.98 degree**.
For the location (latitude, longitude) = (37.41, 142.98) is shown below from online [https://getlatlong.net/](https://getlatlong.net/).
![ScreenShot](hw2_problem1_location.PNG)
This shows that the location near the north east of tokyo has higher probability of earthquake to occur based on the 18 years of data.

Computing the Bhatacharrya Coefficient, we got **0.915415**. This value is close to 1, indicating that the two datasets of latitude and longitude is similar in shape.

In case, I will also show the result of 3rd variable as following.
![ScreenShot](hw2_problem1_depth.PNG)


**Problem 2**

Continuing from the problem 1, I defind the 3 variables as:

- Latitude (variable 1)
- Longitude (variable 2)
- Depth (variable 3)

Input dataset is attached in the same project directory OpenGLVS.
The scatter matrix from the 3 variables are shown below. Note that my program can input any number of variables, so 
it can be n by n scatter matrix.
![ScreenShot](hw2_problem2_scatterMatrix.PNG)
As you can see that the scatter plot between the data 1 (latitude) and data 2 (longitude) are showng some strength
on the straight line with a positive direction. This is also shown in the correlation value
between data 1 and data 2, showing high value of correlation.

On the other hand, as for the case for the data 1/data 2 (latitude/longitude) and data 3 (depth) are showing very weak
strength of straightline as also can be shown in the low correlation value.

This concludes that latitude and longitude of earthquake that occurs closely related (which is obvious since
 both information represents the geographical information), but the there seems no relation between the 
geographical informatoion (latitude or longitude) and the depth.


**Problem 3**
Using the Least-Squares Regression Lines method (1st order), the regression is layed on the scatter matrix as shown below.
![ScreenShot](hw2_problem3_scatterMatrix_Regression.PNG)

Looking at the slope of plot 1-2, plot 1-3, and plot 2-3, the slope value in plot 1-2 (latitude vs longitude) shows
stronger linearity since the value is closer to 1 while slope values in plot 1-3 is very low (approximiately flat).
plot 2-3 (longitude vs depth) shows slope of -1.27, which could be caused by the very different scale of each variable
 that causes the unstable slope value. The scale of depth is [0, 683.359] while the scale of longitude is [124.29, 158.82].
 The scale of depth (y axis) is much larger than the longitude (x axis), so the slope fluctuate a lot in the vertical direction, 
causing the slope to be deviate from 0.