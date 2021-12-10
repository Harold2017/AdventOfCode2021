//
// Created by Harold on 2021/12/10.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <unordered_map>

template<typename T>
class Mat
{
public:
    using value_type = typename std::vector<T>::value_type;
    using reference = typename std::vector<T>::reference;
    using size_type = typename std::vector<T>::size_type;

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

    bool is_local_minimum(size_type const i, size_type const j) const
    {
        std::vector<std::pair<size_type, size_type>> neighbors;
        neighbors.reserve(4);
        find_neighbors(i, j, neighbors);
        for (auto n : neighbors)
            if (this->operator()(n.first, n.second) <= this->operator()(i, j))
                return false;

        return true;
    }

    bool valid_pos(size_type const i, size_type const j) const
    {
        return (0 <= i && i < rows) && (0 <= j && j < cols);
    }
    void find_neighbors(size_type const i, size_type const j, std::vector<std::pair<size_type, size_type>>& neighbors) const
    {
        // four directions
        if (valid_pos(i - 1, j)) neighbors.push_back(std::make_pair(i - 1, j));
        if (valid_pos(i + 1, j)) neighbors.push_back(std::make_pair(i + 1, j));
        if (valid_pos(i, j - 1)) neighbors.push_back(std::make_pair(i, j - 1));
        if (valid_pos(i, j + 1)) neighbors.push_back(std::make_pair(i, j + 1));
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

int sum_of_risk(Mat<int> const& mat)
{
    int sum = 0;
    for (auto i = 0; i < mat.rows; i++)
        for (auto j = 0; j < mat.cols; j++)
            if (mat.is_local_minimum(i, j))
                sum += mat(i, j) + 1;
    return sum;
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    Mat<int> mat;
    read_all(ifs, mat);
    
    return sum_of_risk(mat);
}

struct Pos
{
    size_t x, y;

    bool operator==(Pos const& other) const
    {
        return x == other.x && y == other.y;
    }
};

struct PosHasher
{
    size_t operator()(Pos const& pos) const
    {
        return std::hash<size_t>()(pos.x) ^ std::hash<size_t>()(pos.y);
    }
};

// Basin map
using Basin = std::unordered_map<Pos, bool, PosHasher>;

void gen_basin(Mat<int> const& mat, Basin& basin, Pos pos)
{
    // already in or reach basin edge (9)
    if (basin.find(pos) != basin.end() || mat(pos.x, pos.y) == 9)
        return;
    basin[pos] = true;
    // search neighbors
    std::vector<std::pair<Mat<int>::size_type, Mat<int>::size_type>> neighbors;
    neighbors.reserve(4);
    mat.find_neighbors(pos.x, pos.y, neighbors);
    for (auto n : neighbors)
        if (mat(pos.x, pos.y) <= mat(n.first, n.second))  // search fan-out direction
            gen_basin(mat, basin, Pos{ n.first, n.second });
}

size_t product_of_three_largest_basins(Mat<int> const& mat)
{
    std::vector<int> basin_sizes;
    for (size_t i = 0; i < mat.rows; i++)
        for (size_t j = 0; j < mat.cols; j++)
            if (mat.is_local_minimum(i, j))
            {
                Basin basin;
                gen_basin(mat, basin, Pos{ i, j });
                // for (auto it : basin) std::cout << it.first.x << ", " << it.first.y << '\n';
                // std::cout << '\n';
                basin_sizes.push_back(basin.size());
            }
    std::sort(basin_sizes.begin(), basin_sizes.end(), std::greater<>());
    return basin_sizes[0] * basin_sizes[1] * basin_sizes[2];
}

size_t PartTwo(char const* input)
{
    std::ifstream ifs(input);
    Mat<int> mat;
    read_all(ifs, mat);
    return product_of_three_largest_basins(mat);
}

int main(int argc, char* argv[])
{
    std::cout << "What is the sum of the risk levels of all low points on your heightmap?\n"
              << PartOne(argv[1]) << std::endl;
    std::cout << "What do you get if you multiply together the sizes of the three largest basins?\n"
              << PartTwo(argv[1]) << std::endl;
}