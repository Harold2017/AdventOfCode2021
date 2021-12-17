//
// Created by Harold on 2021/12/17.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

struct Point
{
    int x, y;

    Point(int x_, int y_) : x(x_), y(y_) { }
};

std::ostream& operator<<(std::ostream& os, Point const& point)
{
    os << "x = " << point.x << ", y = " << point.y << "";
    return os;
}

struct Area
{
    int x0, x1, y0, y1;

    bool isPointIn(Point const& point) const
    {
        if (point.x >= x0 && point.x <= x1 && point.y >= y0 && point.y <= y1)
            return true;
        return false;
    }

    bool isPointMiss(Point const& point) const
    {
        if (point.x > x1 || point.y < y0)
            return true;
        return false;
    }
};

std::ostream& operator<<(std::ostream& os, Area const& area)
{
    os << "[ (" << area.x0 << ", " << area.x1 << ") (" << area.y0 << ", " << area.y1 << ") ]";
    return os;
}

struct Velocity : public Point
{
    Velocity(int x, int y) : Point(x, y) { }
    void OneStep()
    {
        if (x > 0)
            x--;
        else if (x < 0)
            x++;
        y--;
    }
};

void read_all(std::ifstream& ifs, Area& area)
{
    std::string line;
    std::getline(ifs, line);
    std::sscanf(line.c_str(), "target area: x=%d..%d, y=%d..%d", &area.x0, &area.x1, &area.y0, &area.y1);
}

int isShot(Velocity& v, Area const& area)
{
    // start position : (0, 0)
    Point p(0, 0);
    int max_y = 0;
    while (!area.isPointMiss(p))
    {
        p.x += v.x;
        p.y += v.y;
        v.OneStep();
        max_y = std::max(max_y, p.y);
        if (area.isPointIn(p))
            return max_y;
    }
    return -1;
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    Area area;
    read_all(ifs, area);
    //std::cout << area << std::endl;
    std::vector<int> ys;
    for (auto i = 0; i < area.x1 + 1; i++)
        for (auto j = area.y0; j < -area.y0; j++)
        {
            Velocity v(i, j);
            auto y = isShot(v, area);
            if (y >= 0)
                ys.push_back(y);
        }
    return *std::max_element(ys.begin(), ys.end());
}

int PartTwo(char const* input)
{
    std::ifstream ifs(input);
    Area area;
    read_all(ifs, area);
    std::vector<int> ys;
    for (auto i = 0; i < area.x1 + 1; i++)
        for (auto j = area.y0; j < -area.y0; j++)
        {
            Velocity v(i, j);
            auto y = isShot(v, area);
            if (y >= 0)
                ys.push_back(y);
        }
    return ys.size();
}

int main(int argc, char* argv[])
{
    std::cout << "What is the highest y position it reaches on this trajectory?\n"
              << PartOne(argv[1]) << std::endl;

    std::cout << "How many distinct initial velocity values cause the probe to be within the target area after any step?\n"
              << PartTwo(argv[1]) << std::endl;

    return 0;
}