//
// Created by Harold on 2021/12/22.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <memory>
#include <unordered_map>

struct Cube
{
    int lx, ly, lz, hx, hy, hz;
    bool is_on = false;

    Cube() = default;
    Cube(int lx_, int ly_, int lz_, int hx_, int hy_, int hz_)
        : lx(lx_), ly(ly_), lz(lz_), hx(hx_), hy(hy_), hz(hz_) { }

    bool is_inside(Cube const& other) const
    {
        return (lx >= other.lx && ly >= other.ly && lz >= other.lz) &&
               (hx <= other.hx && hy <= other.hz && lz <= other.hz);
    }

    bool is_intersect(Cube const& other) const
    {
        if (lx > other.hx || hx < other.lx) return false;
        if (ly > other.hy || hy < other.ly) return false;
        if (lz > other.hz || hz < other.lz) return false;
        return true; 
    }

    size_t volume() const
    {
        return size_t(hx - lx + 1) * size_t(hy - ly + 1) * size_t(hz - lz + 1);
    }
};

std::ostream& operator<<(std::ostream& os, Cube const& cube)
{
    os << (cube.is_on ? "on " : "off ") << '[' << cube.lx << ", " << cube.ly << ", " << cube.lz << "], [" << cube.hx << ", " << cube.hy << ", " << cube.hz << ']';
    return os;
}

using Cubes = std::vector<Cube>;

std::ostream& operator<<(std::ostream& os, Cubes const& cubes)
{
    for (auto const& cube : cubes)
        os << cube << '\n';
    return os;
}

void read_all(std::ifstream& ifs, Cubes& cubes)
{
    std::string line;
    while(std::getline(ifs, line))
    {
        Cube cube;
        // on or off
        auto space_pos = line.find(' ');
        cube.is_on = line.substr(0, space_pos) == "on" ? true : false;
        auto position = line.substr(space_pos + 1);
        std::sscanf(position.c_str(), "x=%d..%d,y=%d..%d,z=%d..%d", &cube.lx, &cube.hx, &cube.ly, &cube.hy, &cube.lz, &cube.hz);
        cubes.push_back(cube);
    }
}

void substract(Cube const& c0, Cube const& c1, Cubes& res_cubes)
{
    Cube overlap(std::max(c0.lx, c1.lx), std::max(c0.ly, c1.ly), std::max(c0.lz, c1.lz),
                 std::min(c0.hx, c1.hx), std::min(c0.hy, c1.hy), std::min(c0.hz, c1.hz));
    
    // z direction
    if (c0.lz < overlap.lz)  // bottom part
        res_cubes.emplace_back(c0.lx, c0.ly, c0.lz, c0.hx, c0.hy, overlap.lz - 1);
    if (c0.hz > overlap.hz)  // top part
        res_cubes.emplace_back(c0.lx, c0.ly, overlap.hz + 1, c0.hx, c0.hy, c0.hz);
    
    // x direction
    if (c0.lx < overlap.lx)  // front
        res_cubes.emplace_back(c0.lx, c0.ly, overlap.lz, overlap.lx - 1, c0.hy, overlap.hz);
    if (c0.hx > overlap.hx)  // back
        res_cubes.emplace_back(overlap.hx + 1, c0.ly, overlap.lz, c0.hx, c0.hy, overlap.hz);
    
    // y direction
    if (c0.ly < overlap.ly)  // left
        res_cubes.emplace_back(overlap.lx, c0.ly, overlap.lz, overlap.hx, overlap.ly - 1, overlap.hz);
    if (c0.hy > overlap.hy)  // right
        res_cubes.emplace_back(overlap.lx, overlap.hy + 1, overlap.lz, overlap.hx, c0.hy, overlap.hz);
}

size_t PartOne(char const* input)
{
    std::ifstream ifs(input);
    Cubes cubes;
    read_all(ifs, cubes);
    //std::cout << cubes << std::endl;

    Cubes res;
    for (auto const& cube : cubes)
    {
        Cubes tmp;
        for (auto const &c : res)
            if (c.is_intersect(cube))
                substract(c, cube, tmp);
            else
                tmp.push_back(c);
        if (cube.is_on)
            tmp.push_back(cube);
        res = std::move(tmp);
    }
    //std::cout << res << std::endl;
    size_t cnt = 0;
    Cube region(-50, -50, -50, 50, 50, 50);
    for (auto const& c : res)
        if (c.is_inside(region))
            cnt += c.volume();

    return cnt;
}

size_t PartTwo(char const* input)
{
    std::ifstream ifs(input);
    Cubes cubes;
    read_all(ifs, cubes);
    //std::cout << cubes << std::endl;

    Cubes res;
    for (auto const& cube : cubes)
    {
        Cubes tmp;
        for (auto const &c : res)
            if (c.is_intersect(cube))
                substract(c, cube, tmp);
            else
                tmp.push_back(c);
        if (cube.is_on)
            tmp.push_back(cube);
        res = std::move(tmp);
    }
    //std::cout << res << std::endl;
    size_t cnt = 0;
    for (auto const& c : res)
        cnt += c.volume();

    return cnt;
}

int main(int argc, char* argv[])
{
    std::cout << "how many cubes are on?\n"
              << PartOne(argv[1]) << std::endl;
    std::cout << "how many cubes are on?\n"
              << PartTwo(argv[1]) << std::endl;

    return 0;
}