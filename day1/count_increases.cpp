//
// Created by Harold on 2021/12/2.
//

#include <fstream>
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
    std::ifstream ifs(argv[1]);
    std::string line;
    int last = std::numeric_limits<int>::max();
    int cnt = 0;
    while (std::getline(ifs, line))
    {
        int n = std::stoi(line);
        if (n > last) cnt++;
        last = n;
    }
    
    std::cout << "How many measurements are larger than the previous measurement?" << '\n'
              << cnt << std::endl;
    return 0;
}