#include <iostream>
#include <random>
#include <algorithm>                        // find, for the vector

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
// #include <glm/vec4.hpp>

// #include <CL/cl.h>
// #include <CL/opencl.h>

#include <opencv4/opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <sampler.cpp>
#include <interpolater.cpp>
#include <filter.cpp>

using namespace cv;
using namespace std;
using namespace glm;

                                            // TODO - Resample the downsampled image
// random function to produce random position of pixels
vec2 rand_help_vector = {12.9898, 4.1414};
float rand(vec2 n) {
    float result = (0.5 * (1 + sin(dot(n, rand_help_vector)))) * 43758.5453;
    return result - floor(result);
}

string full_file_path(string fileName) {
    string full_file_path = "../imgs/";
    full_file_path.append(fileName);
    return full_file_path;
}

int main(int argc, char* argv[]) {

    // Take inputs from command line as options for the application
    string fileName = "test_01.jpg";    // default img
    int pixelPercent = 10;              // default percent of the pixel that need tobe resampled
    string sampler_name = "";
    int base_x = 2;
    int base_y = 3;

    if(argc > 2 ) {
        fileName = argv[1];
        sampler_name = argv[2];
        if(sampler_name == "halton") {
            base_x = stoi(argv[3]);
            base_y = stoi(argv[4]);
            pixelPercent = stoi(argv[5]);
        }
        if(sampler_name == "random") {
            pixelPercent = stoi(argv[3]);
        }
        if(sampler_name == "corjitt") {
            pixelPercent = stoi(argv[3]);
        }
    }


    string fullFilePath = full_file_path(fileName);
    cout << "Name of the opened file: " << fullFilePath << endl;

    // read image
    Mat input_img = imread(fullFilePath, IMREAD_COLOR);
    // cout << "size: " << input_img.size() << endl;
    // cout << "input image: " << input_img.at<Vec3b>(500, 500) << endl;

    // downsample the input image for testing
    Mat downsample_img;
    cv::resize(input_img, downsample_img, Size(), 1.0, 1.0, INTER_CUBIC);
    // cout << "size: " << downsample_img.size() << endl;
    // vector to store information of the pixel's position
    vector<pair<int, int>> sampling_pattern;


                                                // TEST - resample pixels

    //init output image and vector to store which position has already be sampled
    cv::Mat resampled_img = Mat::zeros(downsample_img.size(), CV_8UC3);

    // create new sampler and call random sampling method
    Sampler rand_sampler(pixelPercent);
    if(sampler_name == "random") {
        sampling_pattern = rand_sampler.random_sampling(downsample_img);
    }
    if(sampler_name == "halton") {
        sampling_pattern = rand_sampler.halton_sequence(downsample_img, base_x, base_y);
    }
    if(sampler_name == "corjitt") {
        sampling_pattern = rand_sampler.correlated_multi_jitterred(downsample_img);
    }
    cout << "size of the pattern: " << sampling_pattern.size() << endl;
    
    

    // use sampling pattern to resample pixel from input image to output image
    for(int i = 0; i < sampling_pattern.size(); i++) {
        cv::Point pos = cv::Point(sampling_pattern[i].first, sampling_pattern[i].second);
        // check again the position before resampling
        if(pos.x >= 0 && pos.x <= downsample_img.size().width && pos.y >= 0 && pos.y <= downsample_img.size().height) {
            resampled_img.at<Vec3b>(pos) = downsample_img.at<Vec3b>(pos);
        }
    }

    // interpolation
    Interpolater interpolater;
    // resampled_img = interpolater.splatting_simple(resampled_img, sampling_pattern, 5, 0.1);
    resampled_img = interpolater.delaunay_triangulation(resampled_img, sampling_pattern);

    // Filter filter(1);
    // resampled_img = filter.biCubicInter(resampled_img);
    // resampled_img = filter.median_filter(resampled_img);

    string output_name = "../imgs/outputs/" + fileName;
    // if(sampler_name == "random") {
    //     // imwrite("../imgs/outputs/random_pixels_out.jpg", resampled_img);
    // }
    // if(sampler_name == "halton") {
    //     // imwrite("../imgs/outputs/halton_sequence_out.png", resampled_img);
    // }
    // if(sampler_name == "corjitt") {
    //     // imwrite("../imgs/outputs/correlated_jittered_out.png", resampled_img);
    // }
    imwrite("../imgs/outputs/test_imgs/3_corjitt_splat_10.jpg", resampled_img);
}