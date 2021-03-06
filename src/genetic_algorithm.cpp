#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

struct Tour
{
    std::vector<int> vertex_indexes;
    Tour(std::vector<int> vertex_indexes = std::vector<int>(0))
        : vertex_indexes(vertex_indexes)
    {
    }
};

class Genetic
{
private:
    int number_of_vertices_;
    int population_size_;
    int hybridization_size_;
    int mutation_size_;
    int number_of_generations_;
    int height_;
    int width_;
    int step_size_;
    int border_;
    std::vector<cv::Point> vertices_;
    std::vector<std::vector<float>> cost_matrix_;
    std::vector<Tour> population_;

public:
    Genetic(int number_of_vertices = 0, int population_size = 0, int hybridization_size = 0, int mutation_size = 0, int number_of_generations = 0, int height = 0, int width = 0, int step_size = 0, int border = 0);
    void generate_vertices();
    void read_vertices(const std::string& file_name);
    std::vector<cv::Point> get_vertices();
    void compute_cost_matrix();
    void initialise();
    // void set_population(std::vector<std::vector<int>> population);
    void hybridise(const Tour& tour1, const Tour& tour2, Tour& tour1_, Tour& tour2_, const int& from, const int& to);
    void mutate(const Tour& tour, Tour& tour_, const int& method, int& from, int& to);
    void select(std::vector<float>& current_costs);
    void genetic(Tour& genetic_tour, float& genetic_algorithm_cost);
};

int main()
{
    int height = 800;
    int width = 1200;
    int border = 25;
    int step_size = 10;
    int number_of_vertices = 10;
    int population_size = 500;
    int hybridization_size = 150;
    int mutation_size = 20;
    int number_of_generations = 2000;
    Genetic genetic(number_of_vertices, population_size, hybridization_size, mutation_size, number_of_generations, height, width, step_size, border);
    // number_of_vertices = 100;

    // genetic.generate_vertices();
    genetic.read_vertices("vertices.txt");
    std::vector<cv::Point> vertices;
    vertices = genetic.get_vertices();

    genetic.compute_cost_matrix();

    Tour genetic_tour;
    float genetic_algorithm_cost;
    genetic.genetic(genetic_tour, genetic_algorithm_cost);

    cv::Mat genetic_map(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
    for (int i = 0; i < number_of_vertices; ++i)
    {
        cv::circle(genetic_map, vertices[i], 1, cv::Scalar(0, 0, 255), 2, 0);
    }
    for (int i = 0; i < number_of_vertices - 1; ++i)
    {
        cv::line(genetic_map, vertices[genetic_tour.vertex_indexes[i]], vertices[genetic_tour.vertex_indexes[i + 1]], cv::Scalar(255, 0, 0), 1, 8, 0);
    }
    std::string genetic_text = "GA: ";
    genetic_text.append(std::to_string(genetic_algorithm_cost));
    cv::putText(genetic_map, genetic_text, cv::Point(border, height - border),
                cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 2, 8);
    cv::imshow("genetic_map", genetic_map);
    cv::waitKey();

    return 0;
}

Genetic::Genetic(int number_of_vertices, int population_size, int hybridization_size, int mutation_size, int number_of_generations, int height, int width, int step_size, int border)
    : number_of_vertices_(number_of_vertices), population_size_(population_size), hybridization_size_(hybridization_size), mutation_size_(mutation_size), number_of_generations_(number_of_generations),
      height_(height), width_(width), step_size_(step_size), border_(border)
{
    vertices_ = std::vector<cv::Point>(number_of_vertices_);
}

void Genetic::genetic(Tour& genetic_tour, float& genetic_algorithm_cost)
{
    initialise();
    for (int generation = 0; generation < number_of_generations_; ++generation)
    {
        for (int i = 0; i < hybridization_size_; ++i)
        {
            int first_individual_index = rand() % population_size_;
            int second_individual_index = rand() % population_size_;
            int from = rand() % number_of_vertices_;
            int to = rand() % number_of_vertices_;
            Tour tour1_, tour2_;
            hybridise(population_[first_individual_index],
                      population_[second_individual_index],
                      tour1_,
                      tour2_, from, to);
            population_[population_size_ + 2 * i] = tour1_;
            population_[population_size_ + 2 * i + 1] = tour2_;
        }

        for (int i = 0; i < mutation_size_; ++i)
        {
            int individual_index = rand() % population_size_;
            int method = rand() % 3;
            int from = rand() % number_of_vertices_;
            int to = rand() % number_of_vertices_;

            Tour tour_;
            mutate(population_[individual_index],
                   tour_, method,
                   from, to);
            population_[population_size_ + 2 * hybridization_size_ + i] = tour_;
        }

        std::vector<float> current_costs;
        select(current_costs);
        genetic_algorithm_cost = current_costs[0];
    }

    for (int i = 0; i < number_of_vertices_; ++i)
    {
        genetic_tour.vertex_indexes.push_back(population_[0].vertex_indexes[i]);
    }
}

void Genetic::generate_vertices()
{
    for (int i = 0; i < number_of_vertices_; ++i)
    {
        int x = rand() % ((width_ - border_ * 3) / step_size_) * step_size_ + border_;
        int y = rand() % ((height_ - border_ * 3) / step_size_) * step_size_ + border_;
        vertices_[i].x = x;
        vertices_[i].y = y;
    }
}

void Genetic::read_vertices(const std::string& file_name)
{
    std::ifstream input_file_stream;
    input_file_stream.open(file_name);
    std::vector<int> number_seq;
    if (input_file_stream.is_open())
    {
        int n;
        while (input_file_stream >> n)
        {
            number_seq.push_back(n);
        }
    }
    else
    {
        std::cout << "Find not found.\n";
    }
    input_file_stream.close();
    std::cout << number_seq.size() << std::endl;
    for (size_t i = 0; i < number_seq.size(); ++i)
    {
        std::cout << number_seq[i] << std::endl;
    }
    for (int i = 0; i < number_of_vertices_; ++i)
    {
        vertices_[i].x = number_seq[i * 2];
        vertices_[i].y = number_seq[i * 2 + 1];
    }
    std::cout << "here\n";
}

std::vector<cv::Point> Genetic::get_vertices()
{
    return vertices_;
}

void Genetic::compute_cost_matrix()
{
    cost_matrix_ = std::vector<std::vector<float>>(number_of_vertices_);
    for (int i = 0; i < number_of_vertices_; ++i)
    {
        cost_matrix_[i] = std::vector<float>(number_of_vertices_);
        for (int j = 0; j < number_of_vertices_; ++j)
        {
            if (i == j)
            {
                cost_matrix_[i][j] = FLT_MAX;
            }
            else
            {
                int dx = vertices_[i].x - vertices_[j].x;
                int dy = vertices_[i].y - vertices_[j].y;
                cost_matrix_[i][j] = sqrt(dx * dx + dy * dy);
            }
        }
    }
}

void Genetic::initialise()
{
    population_ = std::vector<Tour>(population_size_ + hybridization_size_ * 2 + mutation_size_);
    for (int i = 0; i < population_size_ + hybridization_size_ * 2 + mutation_size_; ++i)
    {
        population_[i] = std::vector<int>(number_of_vertices_);
        std::vector<bool> has_been_visited = std::vector<bool>(number_of_vertices_, false);
        for (int j = 0; j < number_of_vertices_; ++j)
        {
            int shifting_vertex = rand() % (number_of_vertices_ - j);

            int current_vertex = 0;
            while (has_been_visited[current_vertex] != false)
            {
                ++current_vertex;
            }

            while (shifting_vertex != 0)
            {
                ++current_vertex;
                if (has_been_visited[current_vertex] == false)
                {
                    shifting_vertex--;
                }
            }

            population_[i].vertex_indexes[j] = current_vertex;
            has_been_visited[current_vertex] = true;
        }
    }
}

void Genetic::hybridise(const Tour& tour1, const Tour& tour2, Tour& tour1_, Tour& tour2_, const int& from, const int& to)
{
    tour1_ = Tour(std::vector<int>(number_of_vertices_));
    tour2_ = Tour(std::vector<int>(number_of_vertices_));
    std::vector<int> has_been_visited = std::vector<int>(number_of_vertices_, false);
    std::vector<int> check_tour_1 = std::vector<int>(number_of_vertices_, 0);
    std::vector<int> check_tour_2 = std::vector<int>(number_of_vertices_, 0);

    if (from < to)
    {
        for (int i = from; i <= to; ++i)
        {
            has_been_visited[i] = true;
        }
    }
    else
    {
        for (int i = to; i <= from; ++i)
        {
            has_been_visited[i] = true;
        }
    }

    for (int i = 0; i < number_of_vertices_; ++i)
    {
        if (has_been_visited[i] == true)
        {
            tour1_.vertex_indexes[i] = tour2.vertex_indexes[i];
            tour2_.vertex_indexes[i] = tour1.vertex_indexes[i];
            for (int j = 0; j < number_of_vertices_; ++j)
            {
                if (tour1.vertex_indexes[j] == tour2.vertex_indexes[i])
                {
                    check_tour_1[j] = 1;
                }
                if (tour2.vertex_indexes[j] == tour1.vertex_indexes[i])
                {
                    check_tour_2[j] = 1;
                }
            }
        }
    }

    int current_index_1 = 0;
    int current_index_2 = 0;
    for (int i = 0; i < number_of_vertices_; ++i)
    {
        while (check_tour_1[current_index_1] == 1)
        {
            ++current_index_1;
        }
        while (check_tour_2[current_index_2] == 1)
        {
            ++current_index_2;
        }
        if (has_been_visited[i] == false)
        {
            tour1_.vertex_indexes[i] = tour1.vertex_indexes[current_index_1];
            tour2_.vertex_indexes[i] = tour2.vertex_indexes[current_index_2];
            ++current_index_1;
            ++current_index_2;
        }
    }
}

void Genetic::mutate(const Tour& tour, Tour& tour_, const int& method, int& from, int& to)
{
    tour_ = Tour(std::vector<int>(number_of_vertices_));

    if (from > to)
    {
        int temp = to;
        to = from;
        from = temp;
    }

    if (method == 0)
    {
        for (int i = 0; i < number_of_vertices_; ++i)
        {
            tour_.vertex_indexes[i] = tour.vertex_indexes[i];
        }
        tour_.vertex_indexes[from] = tour.vertex_indexes[to];
        tour_.vertex_indexes[to] = tour.vertex_indexes[from];
    }
    else if (method == 1)
    {
        for (int i = 0; i < from; ++i)
        {
            tour_.vertex_indexes[i] = tour.vertex_indexes[i];
        }
        for (int i = from; i <= to; ++i)
        {
            tour_.vertex_indexes[i] = tour.vertex_indexes[from + to - i];
        }
        for (int i = to + 1; i < number_of_vertices_; ++i)
        {
            tour_.vertex_indexes[i] = tour.vertex_indexes[i];
        }
    }
    else
    {
        for (int i = 0; i < from; ++i)
        {
            tour_.vertex_indexes[i] = tour.vertex_indexes[i];
        }
        for (int i = from; i < from + (number_of_vertices_ - (to + 1)); ++i)
        {
            tour_.vertex_indexes[i] = tour.vertex_indexes[i + to - from + 1];
        }
        for (int i = from + (number_of_vertices_ - (to + 1)); i < number_of_vertices_; ++i)
        {
            tour_.vertex_indexes[i] = tour.vertex_indexes[i - (number_of_vertices_ - (to + 1))];
        }
    }
}

void Genetic::select(std::vector<float>& current_costs)
{
    int current_population_size = population_size_ + 2 * hybridization_size_ + mutation_size_;
    current_costs = std::vector<float>(current_population_size);
    for (int i = 0; i < current_population_size; ++i)
    {
        float current_cost = 0;
        for (int j = 0; j < number_of_vertices_; ++j)
        {
            current_cost += cost_matrix_[population_[i].vertex_indexes[j]][population_[i].vertex_indexes[(j + 1) % number_of_vertices_]];
        }
        current_costs[i] = current_cost;
    }

    // std::sort(std::begin(current_costs), std::end(current_costs));

    for (int i = 0; i < current_population_size; ++i)
    {

        for (int j = i + 1; j < current_population_size; ++j)
        {
            if (current_costs[i] > current_costs[j])
            {
                float temporary_cost = current_costs[i];
                current_costs[i] = current_costs[j];
                current_costs[j] = temporary_cost;
                for (int index = 0; index < number_of_vertices_; ++index)
                {
                    int tmp = population_[i].vertex_indexes[index];
                    population_[i].vertex_indexes[index] = population_[j].vertex_indexes[index];
                    population_[j].vertex_indexes[index] = tmp;
                }
            }
        }
    }
}
