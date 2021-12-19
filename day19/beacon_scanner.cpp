//
// Created by Harold on 2021/12/19.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <numeric>

struct Point
{
    int x = 0, y = 0, z = 0;

    Point() = default;
    Point(int x_, int y_, int z_) : x(x_), y(y_), z(z_) { }

    int operator[](int i) const
    {
        if (i == 0) return x;
        if (i == 1) return y;
        if (i == 2) return z;

        return -1;
    }

    bool operator==(Point const& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    struct Hasher
    {
        int operator()(Point const& point) const
        {
            return std::hash<int>()(point.x) ^ std::hash<int>()(point.y) ^ std::hash<int>()(point.z);
        }
    };
};

std::ostream& operator<<(std::ostream& os, Point const& b)
{
    os << "(" << b.x << ", " << b.y << ", " << b.z << ")";
    return os;
}

using Beacon = Point;
using BeaconSet = std::unordered_set<Beacon, Beacon::Hasher>;

struct PermPair
{
    int scanner_no, direction;

    bool operator==(PermPair const& other) const
    {
            return scanner_no == other.scanner_no && direction == other.direction;
    }

    struct Hasher
    {
        int operator()(PermPair const& pp) const
        {
            return std::hash<int>()(pp.scanner_no) ^ std::hash<int>()(pp.direction);
        }
    };
};

using PermPairBeaconsMap = std::unordered_map<PermPair, std::vector<Beacon>, PermPair::Hasher>;
using Counter = std::unordered_map<Point, int, Point::Hasher>;

void read_all(std::ifstream& ifs, std::vector<std::vector<Beacon>>& beacons)
{
    std::string line;
    std::string scanner_prefix = "---";
    std::vector<Beacon> bs;
    while (std::getline(ifs, line))
    {
        if (line.empty())
        {
            beacons.push_back(bs);
            bs.clear();
            continue;
        }
        if (line.substr(0, scanner_prefix.size()) == scanner_prefix)
            continue;
        Beacon b;
        std::sscanf(line.c_str(), "%d,%d,%d", &b.x, &b.y, &b.z);
        bs.push_back(b);
    }
    beacons.push_back(bs);
}

void print_beacons(std::vector<std::vector<Beacon>> const& beacons)
{
    for (auto const& bs : beacons)
    {
        for (auto const& b : bs)
            std::cout << b << '\n';
        std::cout << '\n';
    }
}

void print_beacons(BeaconSet const& bs)
{
    for (auto const& b : bs)
        std::cout << b << '\n';
}

// [+-x, +-y, +-z] -> forward: 6, up: 4, right: 2 -> 6 x 4 x 2 = 48 possible permutations
//  x  y  z
//  x  y -z
//  x -y  z
//  x -y -z
// -x  y  z
// -x  y -z
// -x -y  z
// -x -y -z
// ... (x z y)
// ... (y x z, y z x)
// ... (z x y, z y x)
//
// (x y z) (x z y) (y x z) (y z x) (z x y) (z y x) -> 6 * 8
Beacon Rotate(Beacon const& b, int dir)
{
    const std::vector<Point> permutations = {
        { 0, 1, 2 },  // x y z
        { 0, 2, 1 },  // x z y
        { 1, 0, 2 },  // y x z
        { 1, 2, 0 },  // y z x
        { 2, 0, 1 },  // z x y
        { 2, 1, 0 }   // z y x
    };

    Beacon nb;
    for (auto i = 0; i < 6; i++)
        if (dir / 8 == i)
        {
            nb.x = b[permutations[i][0]];
            nb.y = b[permutations[i][1]];
            nb.z = b[permutations[i][2]];
        }
    if (dir % 2 == 1)
        nb.x *= -1;
    if ((dir / 2) % 2 == 1)
        nb.y *= -1;
    if ((dir / 4) % 2 == 1)
        nb.z *= -1;
    return nb;
}

// TODO: too slow.... any more efficient way?
BeaconSet find_all_beacons(std::vector<std::vector<Beacon>> const& beacons, std::vector<Point>& scanner_pos)
{
    auto scanners = beacons.size();
    
    std::vector<int> tmp(scanners - 1);
    std::iota(tmp.begin(), tmp.end(), 1);

    // start from scanner 0
    scanner_pos.push_back(Point(0, 0, 0));
    BeaconSet res(beacons[0].begin(), beacons[0].end());
    std::unordered_set<int> fail_set(tmp.begin(), tmp.end());
    PermPairBeaconsMap ppbsm;
    for (auto i = 0; i < scanners; i++)
        for (auto d = 0; d < 48; d++)
            for (auto const& b : beacons[i])
                ppbsm[PermPair{ i, d }].push_back(Rotate(b, d));
    
    while (!fail_set.empty())
    {
        int found_scanner_no = 0;
        for (auto scanner_no : fail_set)
        {
            if (found_scanner_no != 0)
                continue;
            
            for (auto d = 0; d < 48; d++)
            {
                auto b_scan = ppbsm[PermPair{ scanner_no, d }];
                Counter counter;
                for (auto i = 0; i < beacons[scanner_no].size(); i++)
                    for (auto const& rb : res)
                    {
                        // assume b in fail set is the same beacon with rb in res
                        int dx = rb[0] - b_scan[i][0];
                        int dy = rb[1] - b_scan[i][1];
                        int dz = rb[2] - b_scan[i][2];
                        counter[Point(dx, dy, dz)] += 1;
                    }
                for (auto const& kv : counter)
                    // at least 12 same beacons
                    if (kv.second >= 12)
                    {
                        auto p = kv.first;
                        for (auto const& bb : b_scan)
                            res.insert(Beacon(bb.x + p.x, bb.y + p.y, bb.z + p.z));
                        found_scanner_no = scanner_no;
                        scanner_pos.push_back(p);
                    }
            }
        }
        fail_set.erase(fail_set.find(found_scanner_no));
    }
    return res;
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    std::vector<std::vector<Beacon>> beacons;
    read_all(ifs, beacons);
    //print_beacons(beacons);

    std::vector<Point> scanner_pos;
    auto bs = find_all_beacons(beacons, scanner_pos);
    //print_beacons(bs);

    return bs.size();
}

int PartTwo(char const* input)
{
    std::ifstream ifs(input);
    std::vector<std::vector<Beacon>> beacons;
    read_all(ifs, beacons);

    std::vector<Point> scanner_pos;
    find_all_beacons(beacons, scanner_pos);
    
    int max_dist = 0;
    for (auto const& p : scanner_pos)
        for (auto const& q : scanner_pos)
            max_dist = std::max(max_dist, std::abs(p.x - q.x) + std::abs(p.y - q.y) + std::abs(p.z - q.z));

    return max_dist;
}

int main(int argc, char* argv[])
{
    std::cout << "How many beacons are there?\n"
              << PartOne(argv[1]) << std::endl;

    std::cout << "What is the largest Manhattan distance between any two scanners?\n"
              << PartTwo(argv[1]) << std::endl;


    return 0;
}