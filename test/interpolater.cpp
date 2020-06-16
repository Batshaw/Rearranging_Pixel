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

    public:
        Interpolater() {

        }

        // simple splatting with/-out opacity
        Mat splatting_simple(Mat input_img, vector<pair<int, int>> pattern, int radius, double alpha = 0.1) {

            cout << "Splatting..." << endl;
            Mat overlay;
            input_img.copyTo(overlay); 
            Mat result; 
            input_img.copyTo(result);
            int overlay_count = 1;
            // for (int i = 1; i <= radius; i += 2){

            //     cout << endl;
            //     cout << "overlay " << overlay_count << "  ";
            //     for(int j = 0; j < pattern.size(); ++j) {

            //         Point pos(pattern[j].first, pattern[j].second);
            //         Vec3b color = input_img.at<Vec3b>(pos);
            //         cv::circle(overlay, pos, i, color, -1, cv::LINE_AA, 0);
            //     }
                
            //     double trans_alpha = compute_alpha(i, radius, alpha);
            //     cv::addWeighted(overlay, trans_alpha, result, 1-trans_alpha, 0, result);      

            //     overlay_count += 1;            
            // }

            // test just splatting without opacity
            for(int j = 0; j < pattern.size(); ++j) {

                Point pos(pattern[j].first, pattern[j].second);
                Vec3b color = input_img.at<Vec3b>(pos);
                cv::circle(result, pos, radius, color, -1, cv::LINE_AA, 0);
            }
            
            return result;
        }

        // Delaunay triangulation
        Mat delaunay_triangulation(Mat input_img, vector<pair<int, int>> pattern) {

            cout << "Delaunay Triangulation..." << endl;
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
            // vector<Point> triangle_points(3);
            // cout << "Draw triangle..." << endl;
            // for(int i = 0; i < triangleList.size(); ++i) {
            //     Vec6f tri = triangleList[i];
            //     triangle_points[0] = Point((int)tri[0], (int)tri[1]);
            //     triangle_points[1] = Point((int)tri[2], (int)tri[3]);
            //     triangle_points[2] = Point((int)tri[4], (int)tri[5]);

            //     if(rect.contains(triangle_points[0]) 
            //         && rect.contains(triangle_points[1])
            //         && rect.contains(triangle_points[2])) {
                        
            //             line(input_img, triangle_points[0], triangle_points[1], Scalar(255, 0, 0), 1, LINE_AA, 0);
            //             line(input_img, triangle_points[0], triangle_points[2], Scalar(255, 0, 0), 1, LINE_AA, 0);
            //             line(input_img, triangle_points[2], triangle_points[1], Scalar(255, 0, 0), 1, LINE_AA, 0);
            //     }
            // }

            // compute the max. distance between 2 sampled pixels
            int radius = ceil(compute_max_distance(triangleList));
            cout << "Radius for Splatting: " << radius << endl;

            Mat result;
            result = splatting_simple(input_img, pattern, radius);

            return result;
        }

    private:
        
        // compute the transparency value
        float compute_alpha(int current_r, int radius, double alpha) {
            float result = (current_r - 1) * (alpha - 1) / (radius - 1) + 1;
            result = (int)(result * 100 + 0.5f);
            result /= 100.0f;
            cout << "alpha: " << result << "   radius: " << current_r << endl;
            return result;
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
};