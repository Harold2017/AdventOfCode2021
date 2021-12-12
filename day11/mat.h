//
// Created by Harold on 2021/12/12.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>

template<typename T>
class Mat
{
public:
    using value_type = typename std::vector<T>::value_type;
    using reference = typename std::vector<T>::reference;
    using size_type = typename std::vector<T>::size_type;

public:
    struct Point
    {
        int x, y;

        Point(int i, int j) : x(i), y(j) { }

        bool operator==(Point const& other) const
        {
            return x == other.x && y == other.y;
        }
    };
    struct PointHasher
    {
        int operator()(Point const& point) const
        {
            return std::hash<int>()(point.x) ^ std::hash<int>()(point.y);
        }
    };

public:
    Mat() = default;

    Mat(size_type const r, size_type const c)
        : rows(c), cols(c)
    {
        data.resize(r * c);
    }

    reference operator()(size_type const i, size_type const j) noexcept
    {
        return data[i * cols + j];
    }

    T operator()(size_type const i, size_type const j) const noexcept
    {
        return data[i * cols + j];
    }

    bool is_local_minimum(int const i, int const j) const
    {
        std::vector<Point> neighbors;
        neighbors.reserve(8);
        find_neighbors(i, j, neighbors);
        for (auto n : neighbors)
            if (this->operator()(n.x, n.y) <= this->operator()(i, j))
                return false;

        return true;
    }

    bool is_local_maximum(int const i, int const j) const
    {
        std::vector<Point> neighbors;
        neighbors.reserve(8);
        find_neighbors(i, j, neighbors);
        for (auto n : neighbors)
            if (this->operator()(n.x, n.y) >= this->operator()(i, j))
                return false;

        return true;
    }

    bool valid_pos(int const i, int const j) const
    {
        return (0 <= i && i < rows) && (0 <= j && j < cols);
    }
    void find_neighbors(int const i, int const j, std::vector<Point>& neighbors) const
    {
        // eight directions
        if (valid_pos(i - 1, j))
            neighbors.push_back(Point(i - 1, j));
        if (valid_pos(i + 1, j))
            neighbors.push_back(Point(i + 1, j));
        if (valid_pos(i, j - 1))
            neighbors.push_back(Point(i, j - 1));
        if (valid_pos(i, j + 1))
            neighbors.push_back(Point(i, j + 1));
        if (valid_pos(i - 1, j - 1))
            neighbors.push_back(Point(i - 1, j - 1));
        if (valid_pos(i - 1, j + 1))
            neighbors.push_back(Point(i - 1, j + 1));
        if (valid_pos(i + 1, j - 1))
            neighbors.push_back(Point(i + 1, j - 1));
        if (valid_pos(i + 1, j + 1))
            neighbors.push_back(Point(i + 1, j + 1));
    }

public:
    size_type rows, cols;
    std::vector<T> data;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, Mat<T> const& mat)
{
    for (auto i = 0; i < mat.rows; i++)
    {
        for (auto j = 0; j < mat.cols; j++)
            os << mat(i, j) << ' ';
        os << '\n';
    }
        
    return os;
}

void read_all(std::ifstream& ifs, Mat<int>& mat)
{
    std::string line;
    char c;
    int rows = 0, cols = 0;
    bool first = true;
    while (std::getline(ifs, line))
    {
        for (auto c : line)
        {
            mat.data.push_back(c - '0');
            if (first)
            {
                cols++;
            }
        }
        first = false;
        rows++;
    }
    mat.rows = rows;
    mat.cols = cols;
    mat.data.resize(mat.rows * mat.cols, 0);
}