//
// Created by Harold on 2021/12/7.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>

void read_all_nums(std::ifstream& ifs, std::vector<int>& nums)
{
    if (ifs)
        ifs.seekg(0, ifs.beg);
    else
        return;
    std::string line, num;
    std::getline(ifs, line);
    std::stringstream ss(line);
    while (std::getline(ss, num, ',')) nums.push_back(std::stoi(num));
}

int find_least_fuel_position(std::vector<int> const& positions)
{
    auto min_max = std::minmax_element(positions.begin(), positions.end());
    auto min_pos = *min_max.first;
    auto max_pos = *min_max.second;

    int min_fuel = std::numeric_limits<int>::max();
    for (int i = min_pos; i <= max_pos; i++)
    {
        int fuels = 0;
        for (auto p : positions)
            fuels += std::abs(p - i);
        if (fuels < min_fuel)
            min_fuel = fuels;
    }
    
    return min_fuel;
}

int find_least_fuel_position2(std::vector<int> const& positions)
{
    auto min_max = std::minmax_element(positions.begin(), positions.end());
    auto min_pos = *min_max.first;
    auto max_pos = *min_max.second;

    int min_fuel = std::numeric_limits<int>::max();
    for (int i = min_pos; i <= max_pos; i++)
    {
        int fuels = 0;
        for (auto p : positions)
        {
            int steps = std::abs(p - i);
            int fuel = 0;
            for (int i = 1; i <= steps; i++) fuel += i;
            fuels += fuel;
        }
        if (fuels < min_fuel)
            min_fuel = fuels;
    }
    
    return min_fuel;
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    std::vector<int> positions;
    read_all_nums(ifs, positions);
    return find_least_fuel_position(positions);
}

int PartTwo(char const* input)
{
    std::ifstream ifs(input);
    std::vector<int> positions;
    read_all_nums(ifs, positions);
    return find_least_fuel_position2(positions);
}

int main(int argc, char* argv[])
{
    std::cout << "How much fuel must they spend to align to that position?\n"
              << PartOne(argv[1]) << std::endl;
    std::cout << "How much fuel must they spend to align to that position?\n"
              << PartTwo(argv[1]) << std::endl;

    return 0;
}