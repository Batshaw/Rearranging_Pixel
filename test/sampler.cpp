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
        int _number_sample_pixel;
        int _num_pixel_xAxis;
        int _num_pixel_yAxis;

        // random function using pseudo random generator mt19937
        int rand_gen(int min_rand, int max_rand) {
            std::random_device rand_device;
            std::mt19937 generator(rand_device());
            // std::default_random_engine generator;
            // std::uniform_real_distribution<> dist(min_rand, max_rand);
            std::uniform_int_distribution<> dist(min_rand, max_rand);
            int random_number = dist(generator);
            // cout << "test float random: " << random_number << endl;

            // return floor(random_number);
            return random_number;
        }    

        // help function to generate n-th number of Halton Sequence
        int halton_single_number(int index, int base) {
            
            int result = 0;
            int n0 = index;
            float inverse_base = 1 / base;

            do
            {
                // compute result
                int n1 = std::floor(n0 / base);
                int r = n0 - n1*base;
                result += inverse_base*r;
                
                // prepare for next round
                inverse_base /= base;
                n0 = n1;

            } while (n0 > 0);
            
            return result;
        }    
        
    public:
        Sampler(Mat input_image, int percent_pixels){
            _input_image = input_image;

            int _num_pixel_xAxis = _input_image.size().width;
            int _num_pixel_yAxis = _input_image.size().height;
            _number_sample_pixel = _num_pixel_xAxis * _num_pixel_yAxis * percent_pixels / 100;

        }

        // random sampling method
        vector<pair<int, int>> random_sampling() {

            vector<pair<int, int>> sampled_pxs(_number_sample_pixel);
            int x_pos;
            int y_pos;
            std::pair<int, int> pos;

            for(int i = 0; i < _number_sample_pixel; ++i) {
                vec2 seed_x = {i*123, i*56745};
                vec2 seed_y = {i*2355.234, i*14124.656};

                do
                {
                    x_pos = rand_gen(0, _num_pixel_xAxis);
                    y_pos = rand_gen(0, _num_pixel_yAxis);

                    pos = make_pair(x_pos, y_pos);

                } while (find(sampled_pxs.begin(), sampled_pxs.end(), pos) != sampled_pxs.end());

                cout << "(" << pos.first << ", " << pos.second << ")     ";
                sampled_pxs.push_back(pos);
            }
            return sampled_pxs;
        }

        vector<pair<int, int>> halton_sequence(int base_x, int base_y) {

            vector<pair<int, int>> sampled_pxs(_number_sample_pixel);
            int x_pos;
            int y_pos;
            std::pair<int, int> pos;

            for(int i = 0; i < _number_sample_pixel; ++i) {

                // generate i-th number of Halton Sequence for each axis
                x_pos = halton_single_number(i, base_x);
                y_pos = halton_single_number(i, base_y);

                // make pair and push in the pattern vector
                pos = make_pair(x_pos, y_pos);
                cout << "(" << pos.first << ", " << pos.second << ")     ";
                sampled_pxs.push_back(pos);
            }
            return sampled_pxs;
        }
};