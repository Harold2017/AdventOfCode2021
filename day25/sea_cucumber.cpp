//
// Created by Harold on 2021/12/25.
//

#include "../day11/mat.h"

using SearFloor = Mat<char>;
using Point = SearFloor::Point;
using PointHasher = SearFloor::PointHasher;

constexpr char EAST  = '>';
constexpr char SOUTH = 'v';
constexpr char EMPTY = '.';

const Point INVALID_POS(-1, -1);

void scan_seafloor(std::ifstream& ifs, SearFloor& seafloor)
{
    std::string line;
    char c;
    int rows = 0, cols = 0;
    bool first = true;
    while (std::getline(ifs, line))
    {
        for (auto c : line)
        {
            seafloor.data.push_back(c);
            if (first)
            {
                cols++;
            }
        }
        first = false;
        rows++;
    }
    seafloor.rows = rows;
    seafloor.cols = cols;
    seafloor.data.resize(seafloor.rows * seafloor.cols, 0);
}

int move(SearFloor& seafloor, char direction)
{
    // find all directional points
    std::vector<Point> points;
    for (int y = 0; y < seafloor.rows; y++)
        for (int x = 0; x < seafloor.cols; x++)
            if (seafloor(y, x) == direction)
                points.emplace_back(x, y);

    // movable: point -> new_point
    std::vector<std::pair<Point, Point>> movables;
    for (auto && point : points)
    {
        Point new_point = direction == EAST ? 
                                            Point((point.x + 1) % seafloor.cols, point.y) : 
                                            Point(point.x, (point.y + 1) % seafloor.rows);
        if (seafloor(new_point.y, new_point.x) == EMPTY)
            movables.emplace_back(std::make_pair(point, new_point));
    }

    // move at the same time
    for (auto && move_pair : movables)
    {
        seafloor(move_pair.first.y, move_pair.first.x) = EMPTY;
        seafloor(move_pair.second.y, move_pair.second.x) = direction;
    }
    return movables.size();
}

bool one_step(SearFloor& seafloor)
{
    int cnt_move = 0;
    cnt_move += move(seafloor, EAST);
    cnt_move += move(seafloor, SOUTH);
    return cnt_move > 0;
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    SearFloor seafloor;
    scan_seafloor(ifs, seafloor);
    //std::cout << seafloor << '\n';
    int cnt_step = 1;
    while (one_step(seafloor))
        cnt_step++;
    //std::cout << seafloor << '\n';
    return cnt_step;
}


int main(int argc, char* argv[])
{
    std::cout << "What is the first step on which no sea cucumbers move?\n"
              << PartOne(argv[1]) << std::endl;

    return 0;
}