#include <iostream>
#include <vector>

#include <opencv4/opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <glm/glm.hpp>
#include <glm/vec2.hpp>

using namespace std;
using namespace cv;
using namespace glm;

class Interpolater {
    private:


    public:
        Interpolater() {

        }

        // simple splatting
        Mat splatting_simple(Mat input_img, vector<pair<int, int>> pattern, int radius) {

            for(int i = 0; i < pattern.size(); ++i) {

                Point pos(pattern[i].first, pattern[i].second);
                Vec3b color = input_img.at<Vec3b>(pos);
                cv::circle(input_img, pos, radius, color, -1, cv::LINE_AA, 0);
            }
            return input_img;
        }
};