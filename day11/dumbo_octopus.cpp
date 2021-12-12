//
// Created by Harold on 2021/12/12.
//

#include "mat.h"

using Point = Mat<int>::Point;
using PointMap = std::unordered_map<Point, bool, Mat<int>::PointHasher>;

int flashes_of_one_step(Mat<int>& mat)
{
    // each increases by 1
    for (auto i = 0; i < mat.rows; i++)
        for (auto j = 0; j < mat.cols; j++)
            mat(i, j) += 1;

    // > 9 adjacent increases by 1
    auto new_flash = [](Mat<int>& mat, PointMap& flashed_points)
    {
        auto new_flash_impl = [](Mat<int>& mat, PointMap& flashed_points, auto& new_flash_impl_ref)
        {
            std::vector<Point> flashing_points;
            for (auto i = 0; i < mat.rows; i++)
                for (auto j = 0; j < mat.cols; j++)
                    // energy > 9 and not in flashed
                    if (mat(i, j) > 9 && flashed_points.find(Point(i, j)) == flashed_points.end())
                        flashing_points.emplace_back(i, j);
            if (flashing_points.empty()) 
                return;
            for (auto const& p : flashing_points)
            {
                std::vector<Point> neighbors;
                mat.find_neighbors(p.x, p.y, neighbors);
                for (auto const& n : neighbors)
                    mat(n.x, n.y) += 1;
                // update flashed
                flashed_points[p] = true;
            }
            return new_flash_impl_ref(mat, flashed_points, new_flash_impl_ref);
        };
        return new_flash_impl(mat, flashed_points, new_flash_impl);
    };

    PointMap flashed_points;
    new_flash(mat, flashed_points);

    int flashes = 0;
    // >9 -> 0
    for (auto i = 0; i < mat.rows; i++)
        for (auto j = 0; j < mat.cols; j++)
        {
            if (mat(i, j) > 9)
            {
                mat(i, j) = 0;
                flashes++;
            }
        }
    return flashes;
}

int PartOne(char const* input, int steps)
{
    std::ifstream ifs(input);
    Mat<int> mat;
    read_all(ifs, mat);
    int sum = 0;
    for (auto i = 0; i < steps; i++)
    {
        //std::cout << mat << '\n' << std::endl;
        sum += flashes_of_one_step(mat);
    }
    return sum;
}

int PartTwo(char const* input)
{
    std::ifstream ifs(input);
    Mat<int> mat;
    read_all(ifs, mat);
    int cnt = 0;
    int num = mat.rows * mat.cols;
    while (true)
    {
        cnt++;
        if (flashes_of_one_step(mat) == num)
            break;
    }
    return cnt;
}

int main(int argc, char* argv[])
{
    std::cout << "How many total flashes are there after 100 steps?\n"
              << PartOne(argv[1], 100) << std::endl;

    std::cout << "What is the first step during which all octopuses flash?\n"
              << PartTwo(argv[1]) << std::endl;

    return 0;
}
