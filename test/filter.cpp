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

#include <color.cpp>

using namespace std;
using namespace cv;
using namespace glm;

class Filter {

    public:
        Filter(int radius) {
            _radius = radius;
        }

        Mat median_filter(Mat input_img) {
            cout << "Weighted Median Filter..." << endl;
            int width = input_img.size().width;
            int height = input_img.size().height;

            // loop through each pixel in input image
            for(int i = 0; i < width; ++i) {
                for (int j = 0; j < height; ++j) {
                    cv::Point img_pos = cv::Point(i, j);
                    Color existed_color = input_img.at<Vec3b>(img_pos);

                    if (i > _radius && j > _radius && (i + _radius) < width && (j + _radius) < height /*&& existed_color._luminace == 0*/) {  
                        
                        std::vector<pair<Color, float>> weighted_window;

                        // loop through each pixel in window and add the color and weight to the vector
                        for(int a = i - _radius; a <= i + _radius; ++a) {
                            for (int b = j - _radius; b <= j + _radius; ++b){
                                cv::Point pos = cv::Point(a, b);
                                Color color = input_img.at<Vec3b>(pos);
                                // use distance from pixels as weight
                                float weight;
                                vector<float> weight_default{1, 2, 1, 2, 3, 2, 1, 2, 1};
                                if(_radius != 1) {
                                    float dist = sqrt((a-i)*(a-i) + (b-j)*(b-j));
                                    weight = compute_weight(dist);
                                }                                    

                                int weight_count = 0;
                                pair<Color, float> values = make_pair(color, weight_default[weight_count]);
                                weight_count += 1;
                                weighted_window.push_back(values);
                            }
                        }
                        sort(weighted_window.begin(), weighted_window.end(),
                                                    [](const pair<Color, float>& lhs, const pair<Color, float>& rhs){
                                                        return lhs.first._luminace < rhs.first._luminace;
                                                    });
                        float added_weight = 0;
                        // add_up the weights
                        for(int index = 0; index < weighted_window.size(); ++index) {
                            weighted_window[index].second += added_weight;
                            added_weight = weighted_window[index].second;
                        }
                        // check which index have the weighted median
                        int median_index;
                        for(int index = 0; index < weighted_window.size(); ++index) {
                            float contribution = weighted_window[index].second * 100 / weighted_window[weighted_window.size() - 1].second;
                            if(contribution > 50.00f) {
                                median_index = index;
                                break;
                            }
                        }
                        input_img.at<Vec3b>(img_pos) = weighted_window[median_index].first._rgb;
                    }
                }
            }
            return input_img;
        }

        Mat biCubicInter(Mat input_img) {

            cout << "Bicubic Interpolation..." << endl;
            int width = input_img.size().width;
            int height = input_img.size().height;
            int radius = 3;

            // loop through each pixel in image
            for(int i = 0; i < width; ++i) {
                for(int j = 0; j < height; ++j) {

                    // exclude the rand of the image
                    if(i > radius && j > radius && (i + radius) < width && (j + radius) < height ) {

                        Vec3b win[4][4];
                        int row_count = 0;
                        // loop through each pixel in window
                        for(int a = i - radius; a <= i + radius; a += 2) {
                            int col_count = 0;
                            for(int b = j - radius; b <= j + radius; b += 2) {
                                Vec3b color = input_img.at<Vec3b>(cv::Point(a, b));
                                win[col_count][row_count] = color;
                                col_count += 1;
                            }
                            row_count += 1;
                        }

                        input_img.at<Vec3b>(cv::Point(i, j)) = compute_center_value(win);
                    }
                }
            }
            return input_img;
        }

        Vec3b compute_center_value(Vec3b win[4][4]) {
            Vec3b arr[4];
            arr[0] = cubicInter(win[0]);
            arr[1] = cubicInter(win[1]);
            arr[2] = cubicInter(win[2]);
            arr[3] = cubicInter(win[3]);

            return cubicInter(arr);
        }
        Vec3b cubicInter(Vec3b arr[4]) {
            Vec3b result;
            result[0] = arr[1][0] + 0.5*0.5*(arr[2][0] - arr[0][0] + 0.5*(2.0*arr[0][0] - 5.0*arr[1][0] + 4.0*arr[2][0] - arr[3][0] + 0.5*(3.0*(arr[1][0] - arr[2][0]) + arr[3][0] - arr[0][0])));
            result[1] = arr[1][1] + 0.5*0.5*(arr[2][1] - arr[0][1] + 0.5*(2.0*arr[0][1] - 5.0*arr[1][1] + 4.0*arr[2][1] - arr[3][1] + 0.5*(3.0*(arr[1][1] - arr[2][1]) + arr[3][1] - arr[0][1])));
            result[2] = arr[1][2] + 0.5*0.5*(arr[2][2] - arr[0][2] + 0.5*(2.0*arr[0][2] - 5.0*arr[1][2] + 4.0*arr[2][2] - arr[3][2] + 0.5*(3.0*(arr[1][2] - arr[2][2]) + arr[3][2] - arr[0][2])));
            
            return result;
        }

    private:
        int _radius;

        float compute_weight(float dist) {
            float result = (1 - dist) * 9 /(_radius - 1) + 10;
            return result;
        }

        
};