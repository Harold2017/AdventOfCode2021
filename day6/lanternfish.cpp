//
// Created by Harold on 2021/12/6.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>

//#define DEBUG_PRINT

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

void simulate(std::vector<int>& nums, int days)
{
#ifdef DEBUG_PRINT
    auto D = days;
#endif
    while (days > 0)
    {
        auto N = nums.size();
        for (int i = 0; i < N; i++)
        {
            if (nums[i] > 0) nums[i]--;
            else
            {
                nums[i] = 6;
                nums.push_back(8);
            }
        }
        days--;

#ifdef DEBUG_PRINT
        std::cout << "After " << D - days << " days: ";
        for (auto n : nums) std::cout << n << ", ";
        std::cout << '\n';
#endif
    }
}

size_t simulate2(std::vector<int>& nums, int days)
{
    // fish state: 0-8
    std::vector<size_t> states(9, 0);
    // init states
    for (auto n : nums) states[n]++;
    // grow
    while (days > 0)
    {
        // new states
        std::vector<size_t> n_states(9, 0);
        // loop on 9 states every day
        for (int i = 0; i < 9; i++)
            if (i == 0)  // spawn new fish
            {
                n_states[6] = states[0];  // each 0 -> 6
                n_states[8] = states[0];  // and generate one 8
            }
            else  // fish state shift
                n_states[i - 1] += states[i];  // state shift one
        // update states
        states.swap(n_states);
        days--;
    }
    return std::accumulate(states.begin(), states.end(), size_t(0));
}

int PartOne(char const* input, int days)
{
    std::ifstream ifs(input);
    std::vector<int> nums;
    read_all_nums(ifs, nums);
    simulate(nums, days);
    return nums.size();
}

size_t PartTwo(char const* input, int days)
{
    std::ifstream ifs(input);
    std::vector<int> nums;
    read_all_nums(ifs, nums);
    return simulate2(nums, days);
}

int main(int argc, char* argv[])
{
    std::cout << "How many lanternfish would there be after 80 days?\n"
              << PartOne(argv[1], 80) << std::endl;
    std::cout << "How many lanternfish would there be after 256 days?\n"
              << PartTwo(argv[1], 256) << std::endl;

    return 0;
}