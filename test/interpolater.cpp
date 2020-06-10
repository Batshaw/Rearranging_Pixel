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
                sort(dist.begin(), dist.end());

                max_dists.push_back(dist.back());
            }

            sort(max_dists.begin(), max_dists.end());
            result = max_dists.back();

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

    public:
        Interpolater() {

        }

        // simple splatting
        Mat splatting_simple(Mat input_img, vector<pair<int, int>> pattern, int radius, double alpha = 0.1) {

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
                
                double trans_alpha = compute_alpha(i, radius, alpha);
                cv::addWeighted(overlay, trans_alpha, result, 1-trans_alpha, 0, result);      

                overlay_count += 1;            
            }

            // test just splatting
            // for(int j = 0; j < pattern.size(); ++j) {

            //     Point pos(pattern[j].first, pattern[j].second);
            //     Vec3b color = input_img.at<Vec3b>(pos);
            //     cv::circle(result, pos, radius, color, -1, cv::LINE_AA, 0);
            // }
            
            return result;
        }

        // Delaunay triangulation
        Mat delaunay_triangulation(Mat input_img, vector<pair<int, int>> pattern) {

            // collect points from pattern and save them to a vector of Point2f
            vector<Point2f> points;
            for(int i = 0; i < pattern.size(); ++i) {
                Point2f point(pattern[i].first, pattern[i].second);
                points.push_back(point);
            }

            // define space to partition with Delaunay
            // a rectangle equals to the input image
            Rect rect(0, 0, input_img.size().width, input_img.size().height);

            // create a instance of Subdiv2D
            Subdiv2D subdiv(rect);

            // insert point to subdiv
            subdiv.insert(points);

            // vector of vec6f to store the triangles
            vector<Vec6f> triangleList;
            
            subdiv.getTriangleList(triangleList);

            cout << "Number of points: " << pattern.size() << endl;
            cout << "Number of points: " << points.size() << endl;
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
            // result = splatting_simple(input_img, pattern, 20);

            return result;
        }
};