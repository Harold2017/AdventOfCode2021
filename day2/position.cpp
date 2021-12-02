//
// Created by Harold on 2021/12/2.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    std::string line;
    int horizontal = 0;
    int depth = 0;
    std::string direction;
    int value = 0;
    while (std::getline(ifs, line))
    {
        std::istringstream ss(line);
        ss >> direction;
        ss >> value;
        if (direction == "forward") horizontal += value;
        else if (direction == "up") depth -= value;
        else if (direction == "down") depth += value;
        else std::cout << "wrong direction" << std::endl;
    }
    return horizontal * depth;
}

int PartTwo(char const* input)
{
    std::ifstream ifs(input);
    std::string line;
    int horizontal = 0;
    int depth = 0;
    int aim = 0;
    std::string direction;
    int value = 0;
    while (std::getline(ifs, line))
    {
        std::istringstream ss(line);
        ss >> direction;
        ss >> value;
        if (direction == "forward")
        {
            horizontal += value;
            depth += aim * value;
        }
        else if (direction == "up") aim -= value;
        else if (direction == "down") aim += value;
        else std::cout << "wrong direction" << std::endl;
    }
    return horizontal * depth;
}

int main(int argc, char* argv[])
{
    std::cout << "What do you get if you multiply your final horizontal position by your final depth?" << '\n'
              << PartTwo(argv[1]) << std::endl;
    return 0;
}