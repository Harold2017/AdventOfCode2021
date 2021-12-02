//
// Created by Harold on 2021/12/2.
//

#include <fstream>
#include <string>
#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
    std::ifstream ifs(argv[1]);
    std::string line;
    std::vector<int> nums;
    while (std::getline(ifs, line)) nums.push_back(std::stoi(line));

    std::vector<int> sum3s;
    
    for (int i = 0; i < nums.size() - 2; i++) sum3s.push_back(nums[i] + nums[i + 1] + nums[i + 2]);

    int last = std::numeric_limits<int>::max();
    int cnt = 0;
    for (auto s : sum3s)
    {
        if (s > last) cnt++;
        last = s;
    }
    
    std::cout << "How many measurements are larger than the previous measurement?" << '\n'
              << cnt << std::endl;
    return 0;
}