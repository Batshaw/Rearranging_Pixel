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
            std::uniform_real_distribution<> dist(min_rand, max_rand);
            // std::uniform_int_distribution<> dist(min_rand, max_rand);
            float random_number = dist(generator);
            // cout << "test float random: " << random_number << endl;

            return floor(random_number);
            // return random_number;
        }    

        // help function to generate n-th number of Halton Sequence
        float halton_single_number(int index, int base) {
            
            float result = 0;
            float f = 1;

            while (index > 0)
            {
                // compute result
                f /= base;
                result += f*(index % base);
                index /= base;
            }
            
            return result;
        }  
        
    public:
        Sampler(Mat input_image, int percent_pixels){
            _input_image = input_image;

            _num_pixel_xAxis = _input_image.size().width;
            _num_pixel_yAxis = _input_image.size().height;
            _number_sample_pixel = _num_pixel_xAxis * _num_pixel_yAxis * percent_pixels / 100;
        }

        // random sampling method
        vector<pair<int, int>> random_sampling() {

            vector<pair<int, int>> sampled_pxs(_number_sample_pixel);
            int x_pos = 0;
            int y_pos = 0;
            std::pair<int, int> pos;

            for(int i = 0; i < _number_sample_pixel; ++i) {

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
                x_pos = halton_single_number(i+1, base_x)*_num_pixel_xAxis;
                y_pos = halton_single_number(i+1, base_y)*_num_pixel_yAxis;
                pos = make_pair(x_pos, y_pos);
                
                // push in the pattern vector
                cout << "(" << pos.first << ", " << pos.second << ")  ";
                sampled_pxs.push_back(pos);
            }
            return sampled_pxs;
        }

        vector<pair<int, int>> correlated_multi_jitterred() {

            int x_pos;
            int y_pos;
            std::pair<int, int> pos;

            // find nearest square number to the number of samples
            int n = std::pow(std::ceil(std::sqrt(_number_sample_pixel)), 2);
            cout << "new number of samples: " << n << endl;
            // use its square root as size of the cells
            // size of the cells
            int s = std::sqrt(n);
            cout << "size of cells: " << s << endl;
            // vector to store the sampling pattern, with size is n
            vector<pair<int, int>> sampled_pxs(n);

            for(int j = 0; j < s; ++j) {
                for(int i = 0; i < s; ++i) {
                    x_pos = (int)((i + (j + drand48()) / s) / s * _num_pixel_xAxis);
                    y_pos = (int)((j + (i + drand48()) / s) / s * _num_pixel_yAxis);
                    pos = make_pair(x_pos, y_pos);

                    cout << "(" << pos.first << ", " << pos.second << ")  ";
                    sampled_pxs.push_back(pos);
                }
            }

            return sampled_pxs;
        }
};