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
        
    public:
        Sampler(int percent_pixels){
            _percent_pixels = percent_pixels;
        }

        // random sampling method
        vector<pair<int, int>> random_sampling(Mat input_image) {
            set_variable(input_image);

            vector<pair<int, int>> sampled_pxs(_number_sample_pixel, make_pair(0, 0));
            // int x_pos = 0;
            // int y_pos = 0;
            std::pair<int, int> pos;

            int width = input_image.size().width;
            int height = input_image.size().height;

            vector<pair<int, int>> not_sampled_pxs;
            for(int i = 0; i < width; ++i) {
                for(int j = 0; j < height; ++j) {
                    not_sampled_pxs.push_back(pair<int, int>(i, j));
                }
            }


            for(int i = 0; i < _number_sample_pixel; i++) {
                // do
                // {
                //     // x_pos = rand_gen(0, _num_pixel_xAxis);
                //     // y_pos = rand_gen(0, _num_pixel_yAxis);
                //     x_pos = drand48() * _num_pixel_xAxis;
                //     y_pos = drand48() * _num_pixel_yAxis;

                //     pos = make_pair(x_pos, y_pos);

                // } while (find(sampled_pxs.begin(), sampled_pxs.end(), pos) != sampled_pxs.end());
                // int n = rand_gen(0, not_sampled_pxs.size());
                int n = rand() % not_sampled_pxs.size();
                pos.first = not_sampled_pxs[n].first;
                pos.second = not_sampled_pxs[n].second;
                // iter_swap(not_sampled_pxs.begin() + n, not_sampled_pxs.end());
                not_sampled_pxs[n] = not_sampled_pxs.back();
                not_sampled_pxs.pop_back();
                // sampled_pxs.push_back(pos);
                sampled_pxs[i] = pos;
            }
            return sampled_pxs;
        }

        vector<pair<int, int>> halton_sequence(Mat input_image, int base_x, int base_y) {
            set_variable(input_image);

            vector<pair<int, int>> sampled_pxs(_number_sample_pixel, make_pair(0, 0));
            int x_pos;
            int y_pos;
            std::pair<int, int> pos;
            cout << "number of sample pixel: " << _number_sample_pixel << endl;

            for(int i = 0; i < _number_sample_pixel; ++i) {

                // generate i-th number of Halton Sequence for each axis
                x_pos = halton_single_number(i+1, base_x)*_num_pixel_xAxis;
                y_pos = halton_single_number(i+1, base_y)*_num_pixel_yAxis;
                pos = make_pair(x_pos, y_pos);
                
                // push in the pattern vector
                // cout << "(" << pos.first << ", " << pos.second << ")  ";
                sampled_pxs[i] = pos;
            }
            cout << "number of points by halton: " << sampled_pxs.size() << endl;
            return sampled_pxs;
        }

        vector<pair<int, int>> correlated_multi_jitterred(Mat input_image) {
            set_variable(input_image);

            // find nearest square number to the number of samples
            int n = std::pow(std::ceil(std::sqrt(_number_sample_pixel)), 2);
            cout << "new number of samples: " << n << endl;
            // use its square root as size of the cells

            // size of the cells
            int s = std::sqrt(n);
            cout << "size of cells: " << s << endl;

            // vector to store the sampling pattern, with size is n
            vector<pair<int, int>> sampled_pxs(s*s, make_pair(0, 0));

            for(int j = 0; j < s; ++j) {
                for(int i = 0; i < s; ++i) {

                    // placing the sampling position in "canonical" arrangement
                    sampled_pxs[j * s + i].first = (int)((i + (j + drand48()) / s) / s * _num_pixel_xAxis);
                    sampled_pxs[j * s + i].second = (int)((j + (i + drand48()) / s) / s * _num_pixel_yAxis);
                }
            }

            // shuffle the arrangement
            // 1. X-coordinates in each column of the cells are shuffled
            for(int j = 0; j < s; ++j) {

                int temp = j + drand48() * (s - j);

                for(int i = 0; i < s; ++i) {
                    std::swap(sampled_pxs[j * s + i].first,
                              sampled_pxs[temp * s + i].first);
                }
            }

            // 2. Y-coordinates in each row of the cells are shuffled
            for(int i = 0; i < s; ++i) {

                int temp = i + drand48() * (s - i);

                for(int j = 0; j < s; ++j) {
                    std::swap(sampled_pxs[j * s + i].second,
                              sampled_pxs[j * s + temp].second);
                }
            }
            
/*
            int m = static_cast<int>(std::sqrt(_number_sample_pixel * 1.0f));
            int n = (_number_sample_pixel + m - 1) / m;
            // vector to store the sampling pattern, with size is n
            vector<pair<int, int>> sampled_pxs(m*n, make_pair(0, 0));
            int s = 1;
            int p = 5;
            for(int j = 0; j < m; ++j) {
                for(int i = 0; i < n; ++i) {
                    s = permute(s, _number_sample_pixel, p * 0x51633e2d);
                    int sx = permute(s % m, m, p * 0x68bc21eb);
                    int sy = permute(s / m, n, p * 0x02e5be93);
                    float jx = randfloat(s, p * 0x967a889b);
                    float jy = randfloat(s, p * 0x368cc8b7);
                    sampled_pxs[j * m + i].first = (sx + (sy + jx) / n) / m * _num_pixel_xAxis;
                    sampled_pxs[j * m + i].second = (s + jy) / _number_sample_pixel * _num_pixel_yAxis;
                }
            }
*/
            // print_sampling_pattern(sampled_pxs);

            return sampled_pxs;
        }
/*
        unsigned permute(unsigned i, unsigned l, unsigned p) {
            unsigned w = l -1;
            w |= w >> 1;
            w |= w >> 2;
            w |= w >> 4;
            w |= w >> 8;
            w |= w >> 16;
            do
            {
                i ^= p;
                i *= 0xe170893d;

                i ^= p >> 16;
                i ^= (i & w) >> 4;
                i ^= p >> 8;
                i *= 0x0929eb3f;

                i ^= p >> 23;
                i ^= (i & w) >> 1;
                i *= 1 | p >> 27;
                i *= 0x6935fa69;
                i ^= (i & w) >> 11;
                i *= 0x74dcb303;
                i ^= (i & w) >> 2;
                i *= 0x9e501cc3;
                i ^= (i & w) >> 2;
                i *= 0xc860a3df;

                i &= w;
                i ^= i;
            } while (i >= l);

            return (i + p) % l;
        }
        float randfloat(unsigned i, unsigned p) {
            i ^= p;
            i ^= i >> 17;
            i ^= i >> 10;
            i *= 0xb36534e5;
            i ^= i >> 12;
            i ^= i >> 21;
            i *= 0x93fc4795;
            i ^= 0xdf6e307f;
            i ^= i >> 17;
            i *= 1 | p >> 18;

            return i * (1.0f / 4294967808.0f);
        }
*/ 

    private:

        // set internal variable
        void set_variable(Mat input_image) {
            _num_pixel_xAxis = input_image.size().width;
            _num_pixel_yAxis = input_image.size().height;
            _number_sample_pixel = _num_pixel_xAxis * _num_pixel_yAxis * _percent_pixels / 100;
        }

        // print the sampling pattern
        void print_sampling_pattern(vector<pair<int, int>> sampled_pxs) {
            for(int i = 0; i < sampled_pxs.size(); ++i) {
                cout << "(" << sampled_pxs[i].first << ", " << sampled_pxs[i].second << ")   ";
            }
            cout << endl;
        }

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

            
        int _percent_pixels;
        int _number_sample_pixel;
        int _num_pixel_xAxis;
        int _num_pixel_yAxis;
};