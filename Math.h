#ifndef MATH_H
#define MATH_H

#include <opencv2/core/core.hpp>

using cv::Point;
using cv::Point2f;

namespace Math
{
    int distance(const Point p1, const Point p2);
    float calculAngleDegre(const Point2f p1, const Point2f p2);
}


#endif // MATH_H
