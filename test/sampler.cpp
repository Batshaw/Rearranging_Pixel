#include <iostream>
#include <random>
#include <algorithm> 

#include <opencv4/opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <glm/glm.hpp>
#include <glm/vec2.hpp>

using namespace std;
using namespace cv;
using namespace glm;

class Sampler {

    private:
        Mat _input_image;
        int _percent_pixels;

        // random function using pseudo random generator mt19937
        int rand_gen(int min_rand, int max_rand) {
            std::random_device rand_device;
            std::mt19937 generator(rand_device());
            // std::default_random_engine generator;
            std::uniform_real_distribution<> dist(min_rand, max_rand);
            // std::uniform_int_distribution<> dist(min_rand, max_rand);
            float random_number = dist(generator);
            // cout << "test float random: " << random_number << endl;

            return floor(random_number);
        }        
        
    public:
        Sampler(Mat input_image, int percent_pixels){
            _input_image = input_image;
            _percent_pixels = percent_pixels;
        }

        // random sampling method
        vector<pair<int, int>> random_sampling() {
            
            int num_pixel_xAxis = _input_image.size().width;
            int num_pixel_yAxis = _input_image.size().height;
            int number_sample_pixel = num_pixel_xAxis * num_pixel_yAxis * _percent_pixels / 100;

            vector<pair<int, int>> sampled_pxs;

            int x_pos = 0;
            int y_pos = 0;
            std::pair<int, int> pos;

            for(int i = 0; i < number_sample_pixel; ++i) {
                vec2 seed_x = {i*123, i*56745};
                vec2 seed_y = {i*2355.234, i*14124.656};

                do
                {
                    x_pos = rand_gen(0, num_pixel_xAxis);
                    y_pos = rand_gen(0, num_pixel_yAxis);

                    pos = make_pair(x_pos, y_pos);

                } while (find(sampled_pxs.begin(), sampled_pxs.end(), pos) != sampled_pxs.end());

                cout << "(" << pos.first << ", " << pos.second << ")     ";
                sampled_pxs.push_back(pos);
            }
            return sampled_pxs;
        }
};