#include <iostream>
#include <random>
#include <algorithm> 
#include <cmath>

#include <opencv4/opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <glm/glm.hpp>
#include <glm/vec2.hpp>

using namespace std;
using namespace cv;
using namespace glm;

struct Color
{
    Vec3b _rgb;
    float _luminace;

    Color(Vec3b rgb) {
        _rgb = rgb;
        _luminace = 0.2126*rgb[0] + 0.7152*rgb[1] + 0.0722*rgb[2];
    }
};
