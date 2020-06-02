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
        

        // compute the transparency value
        float compute_alpha(int current_r, int radius, double alpha) {
            float result = (current_r - 1) * (alpha - 1) / (radius - 1) + 1;
            result = (int)(result * 100 + 0.5f);
            result /= 100.0f;
            cout << "alpha: " << result << "   radius: " << current_r << endl;
            return result;
        }

    public:
        Interpolater() {

        }

        // simple splatting
        Mat splatting_simple(Mat input_img, vector<pair<int, int>> pattern, int radius, double alpha) {
            // Mat overlay;
            // input_img.copyTo(overlay);

            // for(int i = 0; i < pattern.size(); ++i) {

            //     Point pos(pattern[i].first, pattern[i].second);
            //     Vec3b color = input_img.at<Vec3b>(pos);
            //     cv::circle(overlay, pos, 1, color, -1, cv::LINE_AA, 0);
            // }
            // alpha = 1.0;
            // cv::addWeighted(overlay, alpha, input_img, 1-alpha, 0, input_img);

            // for(int i = 0; i < pattern.size(); ++i) {

            //     Point pos(pattern[i].first, pattern[i].second);
            //     Vec3b color = input_img.at<Vec3b>(pos);
            //     cv::circle(overlay, pos, 10, color, -1, cv::LINE_AA, 0);
            // }
            // alpha = 0.5;
            // cv::addWeighted(overlay, alpha, input_img, 1-alpha, 0, input_img);

            cout << "Splatting..." << endl;
            Mat overlay;
            input_img.copyTo(overlay); 
            Mat result; 
            input_img.copyTo(result);
            int overlay_count = 1;
            for (int i = 1; i <= radius; i += 2){

                cout << endl;
                cout << "overlay " << overlay_count << "  ";
                for(int j = 0; j < pattern.size(); ++j) {

                    Point pos(pattern[j].first, pattern[j].second);
                    Vec3b color = input_img.at<Vec3b>(pos);
                    cv::circle(overlay, pos, i, color, -1, cv::LINE_AA, 0);
                }
                // Point pos(500, 500);
                // Vec3b color = Vec3b(255, 0, 0);
                // cv::circle(overlay, pos, i, color, -1, cv::LINE_AA, 0);
                double trans_alpha = compute_alpha(i, radius, alpha);
                cv::addWeighted(overlay, trans_alpha, result, 1-trans_alpha, 0, result);      

                overlay_count += 1;            
            }
            return result;
        }
};