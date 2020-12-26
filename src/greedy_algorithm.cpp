#include <fstream>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

float get_distance(cv::Point2f point_1, cv::Point2f point_2)
{
    float d_x = point_2.x - point_1.x;
    float d_y = point_2.y - point_1.y;
    float distance = sqrt(d_x * d_x + d_y * d_y);
    return distance;
}

void greedy(const int& number_of_vertices, const std::vector<cv::Point>& points, const int& start, std::vector<int>& tour, float& cost);

int main()
{
    int height = 800;
    int width = 1200;
    int border = 25;
    int step_size = 10;
    int number_of_vertices = 150;

    std::vector<cv::Point> points;
    for (int i = 0; i < number_of_vertices; ++i)
    {
        points.push_back(cv::Point(rand() % ((width - border * 3) / step_size) * step_size + border, rand() % ((height - border * 3) / step_size) * step_size + border));
    }

    int start = 0;
    std::vector<int> greedy_tour;
    float greedy_cost;
    greedy(number_of_vertices, points, start, greedy_tour, greedy_cost);
    cv::Mat greedy_map(height, width, CV_8UC3, cv::Scalar(255, 255, 255));

    for (int i = 0; i < number_of_vertices; ++i)
    {
        cv::circle(greedy_map, points[greedy_tour[i]], 1, cv::Scalar(0, 0, 255), 2, 0);
    }

    for (int i = 0; i < number_of_vertices - 1; ++i)
    {
        cv::line(greedy_map, points[greedy_tour[i]], points[greedy_tour[i + 1]], cv::Scalar(255, 0, 0), 1, 8, 0);
        cv::imshow("Greedy", greedy_map);
        cv::waitKey(100);
    }

    std::string greedy_text = "number_of_vertices = ";
    greedy_text.append(std::to_string(number_of_vertices));
    greedy_text.append("     greedy: ");
    greedy_text.append(std::to_string(greedy_cost));
    cv::putText(greedy_map, greedy_text, cv::Point(border, height - border),
                cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 2, 8);
    cv::imshow("Greedy", greedy_map);
    cv::waitKey();

    return 0;
}

void greedy(const int& number_of_vertices, const std::vector<cv::Point>& points, const int& start, std::vector<int>& tour, float& cost)
{
    std::vector<bool> has_been_visited = std::vector<bool>(number_of_vertices, false);
    tour = std::vector<int>(number_of_vertices, 0);
    tour[0] = start;
    has_been_visited[start] = true;
    int current_vertex = start;
    cost = 0;
    for (int step = 1; step < number_of_vertices; ++step)
    {
        float min_cost = FLT_MAX;
        int nearest_vertex = 0;

        for (int next_vertex = 0; next_vertex < number_of_vertices; ++next_vertex)
        {
            if (min_cost > get_distance(points[current_vertex], points[next_vertex]) && !has_been_visited[next_vertex])
            {
                min_cost = get_distance(points[current_vertex], points[next_vertex]);
                nearest_vertex = next_vertex;
            }
        }
        tour[step] = nearest_vertex;
        has_been_visited[nearest_vertex] = true;
        current_vertex = nearest_vertex;
        cost += min_cost;
    }
    cost += get_distance(points[tour[number_of_vertices - 1]], points[tour[0]]);
}
