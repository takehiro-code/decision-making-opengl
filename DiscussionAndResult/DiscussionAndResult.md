
# HW Discussion and Results

### HW1

### HW2
**Problem 1**
Overlay 1D gaussian distribution on the histogram for the two datasets latitude and longitude are shown below.
We set the number of bin to **30** and used the entire data samples **14092** points. The data is taken from 2001-01-02 to 2018-11-27 for 18 years.
![](hw2_problem1_latitude.png)
![](hw2_problem1_longitude.png)
For the result for dataset latitude, the histogram is following the gaussian shape and the mean value of latitude is approximately the highest frequency of the histogram and is **37.41 degree**.
Similarly for the longitude dataset, the mean value (approximately highest frequency) is **142.98 degree**.
For the location (latitude, longitude) = (37.41, 142.98) is shown below from online [https://getlatlong.net/](https://getlatlong.net/).
![](hw2_problem1_location.png)
This shows that the location near the north east of tokyo has higher probability of earthquake to occur based on the 18 years of data.

Computing the Bhatacharrya Coefficient, we got **0.915415**. This value is close to 1, indicating that the two datasets of latitude and longitude is similar in shape.

**Problem 2**

Continuing from the problem 1, I defind the 3 variables as:

- Latitude
- Longitude
- Depth



