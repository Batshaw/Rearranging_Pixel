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
// using namespace glm;

class Interpolater {

    public:
        Interpolater(int size_of_window) {
            _size_of_window = size_of_window;
        }

        // simple splatting with/-out opacity
        Mat splatting_simple(Mat input_img, vector<pair<int, int>> pattern, Mat result, int radius) {

            cout << "Splatting..." << endl;
            Mat overlay;
            input_img.copyTo(overlay); 
            // // Mat result; 
            input_img.copyTo(result);
            
            int overlay_count = 1;
            for (int i = 1; i <= radius; i++){

                cout << endl;
                cout << "overlay " << overlay_count << "  ";
                for(int j = 0; j < pattern.size(); ++j) {

                    Point pos(pattern[j].first, pattern[j].second);
                    Vec4b color = input_img.at<Vec4b>(pos);
                    cv::circle(overlay, pos, i, color, -1, cv::LINE_AA, 0);
                }
                
                double trans_alpha = compute_alpha(i, radius);
                cv::addWeighted(overlay, trans_alpha, result, 1-trans_alpha, 0, result);      

                overlay_count += 1;            
            }

            // test just splatting without opacity
            // for(int j = 0; j < pattern.size(); ++j) {

            //     Point pos(pattern[j].first, pattern[j].second);

            //     if(radius == 0) {
            //         result.at<Vec3b>(pos) = input_img.at<Vec3b>(pos);
            //     }
            //     else
            //     {
            //         Vec3b color = input_img.at<Vec3b>(pos);
            //         cv::circle(result, pos, radius, color, -1, cv::LINE_AA, 0);
            //     }  
            // }
            
            return result;
        }

        // TODO - pixel splatting
        Mat splatting_pixel(Mat input_img, vector<pair<int, int>> pattern, Mat result, int size = 5) {
            cout << "Pixel splatting..." << endl;

            int width = input_img.size().width;
            int height = input_img.size().height;
            input_img.copyTo(result);

            vector<vector<int>> alpha_matrix = alpha_value(size);

            int radius = (size - 1) / 2;
            cout<< "radius: "<< radius<< endl;

            for(int a = 0; a < pattern.size(); a++) {

                Point pos(pattern[a].first, pattern[a].second);
                // color of sampling pixel
                Vec4b color = input_img.at<Vec4b>(pos);

                // counter of the rows and columns in the alpha_matrix
                int count_x = 0;
                int count_y = 0;

                for(int i = pos.x - radius; i <= pos.x + radius; i++) {
                    for(int j = pos.y - radius; j <= pos.y + radius; j++) {
                        
                        if(0 <= i && i < width && 0 <= j && j < height) {
                            // position of each neighboring pixel
                            Point pos_win_element(i, j);

                            int alpha = alpha_matrix[count_x][count_y];

                            // destination of the blending
                            Vec4b dst = result.at<Vec4b>(pos_win_element);

                            // source of the blending
                            Vec4b src = Vec4b(color[0], color[1], color[2], alpha);
                            // result of the blending
                            Vec4b res = color_blending(dst, src);
                            result.at<Vec4b>(pos_win_element) = res;
                        }
                        // next column
                        count_y += 1;
                    }
                    // reset column to 0 and go to next row
                    count_y = 0;
                    count_x += 1;
                }
                // result.at<Vec4b>(pos) = Vec4b(color[0], color[1], color[2], 255);
            }
            return result;
        }

        // Delaunay triangulation
        Mat delaunay_triangulation(Mat input_img, vector<pair<int, int>> pattern) {

            // cout << "Delaunay Triangulation..." << endl;
/*            
            int width = input_img.size().width;
            int height = input_img.size().height;

            // collect points from pattern and save them to a vector of Point2f
            vector<Point2f> points;
            for(int i = 0; i < pattern.size(); ++i) {
                int x_coord = pattern[i].first;
                int y_coord = pattern[i].second;

                if(x_coord < width && y_coord < height) {
                    Point2f point(x_coord, y_coord);
                    points.push_back(point);
                }
            }
            cout << "collect points: " << points.size() << endl;
            cout << "Number of points: " << pattern.size() << endl;
            // define space to partition with Delaunay
            // a rectangle equals to the input image
            Rect rect(0, 0, width, height);

            // create a instance of Subdiv2D
            Subdiv2D subdiv(rect);
            cout << "subdiv created!" << endl;
            // insert point to subdiv
            subdiv.insert(points);
            // for(vector<Point2f>::iterator it = points.begin(); it != points.end(); it++) {
            //     subdiv.insert(*it);
            // }

            cout << "insert points to subdiv" << endl;
            // vector of vec6f to store the triangles
            vector<Vec6f> triangleList;
            cout << "get triangleList..." << endl;
            subdiv.getTriangleList(triangleList);

            cout << "Number of triangles: " << triangleList.size() << endl;

            // draw the triangles to test
            Mat result;
            input_img.copyTo(result);
            vector<Point> triangle_points(3);
            cout << "Draw triangle..." << endl;
            for(int i = 0; i < triangleList.size(); ++i) {
                Vec6f tri = triangleList[i];
                triangle_points[0] = Point(cvRound(tri[0]), cvRound(tri[1]));
                triangle_points[1] = Point(cvRound(tri[2]), cvRound(tri[3]));
                triangle_points[2] = Point(cvRound(tri[4]), cvRound(tri[5]));

                if(rect.contains(triangle_points[0]) 
                    && rect.contains(triangle_points[1])
                    && rect.contains(triangle_points[2])) {

                        Vec3b color1 = input_img.at<Vec3b>(triangle_points[0]);
                        Vec3b color2 = input_img.at<Vec3b>(triangle_points[1]);
                        Vec3b color3 = input_img.at<Vec3b>(triangle_points[2]);
                        Scalar poly_color = (color1 + color2 + color3) / 3.0f;
                        fillConvexPoly(result, triangle_points, poly_color, 8, 0);
                }
            }
*/
            // compute the max. distance between 2 sampled pixels
            // int radius = ceil(compute_max_distance(triangleList));
            // cout << "Radius for Splatting: " << radius << endl;
            Mat result(input_img.size(), CV_8UC4);

            // result = splatting_simple(input_img, pattern, result, 5);
            result = splatting_pixel(input_img, pattern, result, _size_of_window);

            return result;
        }

    private:
        
        // compute the transparency value
        // using cos(x)⁴ to compute the transparency. x_range [0, pi/2]
        //                                            y_range [1, 0] 
        float compute_alpha(int current_r, int radius) {
            // cout<< "max_radius: "<< radius<< endl;
            // angle in degree
            double angle = ((double)current_r - 1) / ((double)radius - 1) * 90.00;
            // cout<< "angle in degree: "<< angle<< endl;
            // turn the angle into radian
            angle = angle * CV_PI / 180;
            // cout<< "angle in radian: "<< angle<< endl;
            float result = pow(cos(angle), 4);
            cout << "alpha: " << result << "   curr_radius: " << current_r << endl;
            return result;
        }

        // function to pre-create a alpha matrix with input radius
        std::vector<std::vector<int>> alpha_value(int size) {
            // create new alpha matrix
            std::vector<std::vector<int>> alpha_matrix;

            // preparing the step and evaluation value
            double step_x = 1.0 / (size - 1.0);
            double step_y = 1.0 / (size - 1.0);
            std::cout << "step_x: " << step_x << std::endl;

            double eval_x = -1.0 / 2.0;
            std::cout << "eval_x: " << eval_x << std::endl;

            double eval_y = -1.0 / 2.0;
            double cos_value = 0.0;
            int alpha = 0;

            for(int i = 0; i < size; i++) {
                // row of the alpha matrix
                std::vector<int> row;

                for(int j = 0; j < size; j++) {
                // evaluation distance to center pixel
                double dis = std::sqrt(std::pow(eval_x, 2) + std::pow(eval_y, 2));

                if(dis <= 0.5) {
                    cos_value = std::pow(cos(dis * CV_PI / 2.0), 4);
                } else
                {
                    cos_value = 0;
                }
                alpha = (int)(cos_value * 255.0);
                row.push_back(alpha);
                eval_y += step_y;
                }
                alpha_matrix.push_back(row);
                // reset eval_y and raise eval_x
                eval_y = -1.0 / 2.0;
                eval_x += step_x;
            }

            return alpha_matrix;
        }

        // function to perform the color_blending
        // REFERENCE: https://en.wikipedia.org/wiki/Alpha_compositing
        Vec4b color_blending(Vec4b dst, Vec4b src) {
            Vec4b res;
            // match alpha of srouce to range 0.0 - 1.0
            float src_alpha = (float)src[3] / 255.0f;
            // RGB of the source and destination is multiplied with their alpha value (sum of them is equal 1.0)
            res[0] = src[0] * src_alpha + dst[0] * (1.0f - src_alpha);
            res[1] = src[1] * src_alpha + dst[1] * (1.0f - src_alpha);
            res[2] = src[2] * src_alpha + dst[2] * (1.0f - src_alpha);
            // output of the alpha value
            res[3] = src[3] + dst[3] * (1.0f - src_alpha);

            return res;
        }

        // compute max distance between 2 sampled pixels
        float compute_max_distance(vector<Vec6f> triangleList) {
            int result;
            vector<float> max_dists;

            vector<Point> triangle(3);

            for(int i = 0; i < triangleList.size(); ++i) {

                Vec6f tri = triangleList[i];
                // triangle is a vector of 3 points
                triangle[0] = Point((int)tri[0], (int)tri[1]);
                triangle[1] = Point((int)tri[2], (int)tri[3]);
                triangle[2] = Point((int)tri[4], (int)tri[5]);

                vector<float> dist(3);
                dist[0] = distance(triangle[0], triangle[1]);
                dist[1] = distance(triangle[0], triangle[2]);
                dist[2] = distance(triangle[2], triangle[1]);
                float max_in_triangle = max_float_vector(dist);

                max_dists.push_back(max_in_triangle);
            }
            result = max_float_vector(max_dists);

            return result;
        }
        float distance(Point point_1, Point point_2) {
            float x_1 = point_1.x;
            float y_1 = point_1.y;
            float x_2 = point_2.x;
            float y_2 = point_2.y;
            float result = std::sqrt(pow(x_1 - x_2, 2) + pow(y_1 - y_2, 2));

            return result;
        }
        float max_float_vector(vector<float> vec) {
            int max = 0;
            for(int i = 0; i < vec.size(); ++i) {
                if(vec[i] > max) {
                    max = vec[i];
                }
            }
            return max;
        }

        int _size_of_window;
};