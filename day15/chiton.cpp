//
// Created by Harold on 2021/12/15.
//

#include "../day11/mat.h"
#include <queue>

using Cavern = Mat<int>;
using Visited = Mat<bool>;

struct Point
{
    int row, col;
    int dist;

    Point(int r, int c, int d)
        : row(r), col(c), dist(d)
    { }

    bool operator > (Point const& other) const
    {
        return dist > other.dist;
    }
};

// minimum on top to ensure the shortest dist
using Path = std::priority_queue<Point, std::vector<Point>, std::greater<Point>>;

int find_lowest_risks(Cavern const& cavern)
{
    Visited visited;
    visited.rows = cavern.rows;
    visited.cols = cavern.cols;
    visited.data.resize(visited.rows * visited.cols, false);

    // start from left top (0, 0)
    Path path;
    path.push(Point(0, 0, 0));
    visited(0, 0) = true;
    while (!path.empty())
    {
        auto p = path.top();
        path.pop();

        Point np(0, 0, 0);

        // reach end
        if (p.row == cavern.rows - 1 && p.col == cavern.cols - 1)
            return p.dist;
        
        // moving up
        if (p.row - 1 >= 0 && visited(p.row - 1, p.col) == false)
        {
            path.push(Point(p.row - 1, p.col, p.dist + cavern(p.row - 1, p.col)));
            visited(p.row - 1, p.col) = true;
        }

        // moving down
        if (p.row + 1 < cavern.rows && visited(p.row + 1, p.col) == false)
        {
            path.push(Point(p.row + 1, p.col, p.dist + cavern(p.row + 1, p.col)));
            visited(p.row + 1, p.col) = true;
        }

        // moving left
        if (p.col - 1 >= 0 && visited(p.row, p.col - 1) == false)
        {
            path.push(Point(p.row, p.col - 1, p.dist + cavern(p.row, p.col - 1)));
            visited(p.row, p.col - 1) = true;
        }

        // moving right
        if (p.col + 1 < cavern.cols && visited(p.row, p.col + 1) == false)
        {
            path.push(Point(p.row, p.col + 1, p.dist + cavern(p.row, p.col + 1)));
            visited(p.row, p.col + 1) = true;
        }
    }
    return 0;
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    Cavern cavern;
    read_all(ifs, cavern);
    //std::cout << cavern << std::endl;

    return find_lowest_risks(cavern);
}

Cavern expand_cavern(Cavern& cavern, int times)
{
    auto rows = cavern.rows * times;
    auto cols = cavern.cols * times;
    Cavern n_cavern(rows, cols);
    for (auto i = 0; i < rows; i++)
        for (auto j = 0; j < cols; j++)
        {
            n_cavern(i, j) = cavern(i % cavern.rows, j % cavern.cols) + i / cavern.rows + j / cavern.cols;
            while (n_cavern(i, j) > 9)
                n_cavern(i, j) -= 9;
        }            

    return n_cavern;
}

int PartTwo(char const* input)
{
    std::ifstream ifs(input);
    Cavern cavern;
    read_all(ifs, cavern);
    auto n_cavern = expand_cavern(cavern, 5);
    //std::cout << n_cavern << std::endl;

    return find_lowest_risks(n_cavern);
}

int main(int argc, char* argv[])
{
    std::cout << "What is the lowest total risk of any path from the top left to the bottom right?\n"
              << PartOne(argv[1]) << std::endl;

    std::cout << "What is the lowest total risk of any path from the top left to the bottom right?\n"
              << PartTwo(argv[1]) << std::endl;

    return 0;
}