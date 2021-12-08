//
// Created by Harold on 2021/12/8.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>

struct Signal {
    std::vector<std::string> io;
};

void read_all_signals(std::ifstream& ifs, std::vector<Signal>& signals)
{
    std::string line, input, output;
    while (std::getline(ifs, line))
    {
        std::stringstream ss(line);
        std::getline(ss, input, '|');
        std::getline(ss, output);
        output = output.substr(1);  // eliminate first space

        Signal signal;
        std::stringstream si(input), so(output);
        std::string s;
        while (std::getline(si, s, ' '))
        {
            signal.io.push_back(s);
        }
        while (std::getline(so, s, ' '))
        {
            signal.io.push_back(s);
        }

        signals.push_back(signal);
    }
}

int unique_digits_appear_times(std::vector<Signal> const& signals)
{
    // 1 -> cf      -> 2
    // 4 -> eafb    -> 4
    // 7 -> dab     -> 3
    // 8 -> abcdefg -> 7
    int cnt = 0;
    for (auto const& signal : signals)
    {
        for (auto i = 10; i < 14; i++)
            if (signal.io[i].size() == 2 ||
                signal.io[i].size() == 4 ||
                signal.io[i].size() == 3 ||
                signal.io[i].size() == 7)
                cnt++;
    }
    return cnt;
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    std::vector<Signal> signals;
    read_all_signals(ifs, signals);
    return unique_digits_appear_times(signals);
}

// // find substring regardless of order
// bool find_in_string(std::string const& s, std::string const& str)
// {
//     bool found = false;
//     int pos = 0;

//     auto _find_in_string = [](int pos, std::string s, std::string str)
//     {
//         std::string str_s = str.substr(pos, s.length());
//         int s_pos = 0;

//         while (!s.empty())
//         {
//             std::size_t found = str_s.find(s[0]);
//             if (found != std::string::npos)
//             {
//                 s.erase(0, 1);
//                 str_s.erase(found, 1);
//             }
//             else
//                 return false;
//         }

//         return true;
//     };

//     while (!found && pos < str.length() - s.length() + 1)
//         found = _find_in_string(pos++, s, str);

//     return found;
// }

// char find_char_not_in(std::string const& s, std::string const& str)
// {
//     for (auto c : str)
//         if (s.find(c) == std::string::npos)
//             return c;
//     return ' ';
// }

int count_commons(std::string const& s1, std::string const& s2)
{
    // a-g: 7
    int freq1[7] = { 0 };
    int freq2[7] = { 0 };
    int cnt = 0;

    for (auto i = 0; i < s1.size(); i++)
        freq1[s1[i] - 'a']++;
    for (auto i = 0; i < s2.size(); i++)
        freq2[s2[i] - 'a']++;

    for (auto i = 0; i < 7; i++)
        if (freq1[i] > 0 && freq2[i] > 0)
            cnt++;
    
    return cnt;
}

//  0000
// 1    2
// 1    2
//  3333
// 4    5
// 4    5
//  6666

// num     -> length
// 1       -> 2
// 4       -> 4
// 7       -> 3
// 8       -> 7
// 2, 3, 5 -> 5
// 0, 6, 9 -> 6
int add_all_outputs(std::vector<Signal> const& signals)
{
    int cnt = 0;
    std::vector<int> nums;
    std::vector<std::string> digits(4, "");  // <1, 4, 7, 8>
    // commons in <1, 4, 7, 8>
    // 0 -> <2, 3, 3, 6>
    // 2 -> <1, 2, 2, 5>
    // 3 -> <2, 3, 3, 5>
    // 5 -> <1, 3, 2, 5>
    // 6 -> <1, 3, 2, 6>
    // 9 -> <2, 4, 3, 6>
    for (auto const& signal : signals)
    {
        nums.clear();
        for (auto const& s : signal.io)
            if (s.size() == 2) digits[0] = s;
            else if (s.size() == 4) digits[1] = s;
            else if (s.size() == 3) digits[2] = s;
            else if (s.size() == 7) digits[3] = s;
        for (auto i = 10; i < 14; i++)
            // 1
            if (signal.io[i].size() == 2)
                nums.push_back(1);
            // 4
            else if (signal.io[i].size() == 4)
                nums.push_back(4);
            // 7
            else if (signal.io[i].size() == 3)
                nums.push_back(7);
            // 8
            else if (signal.io[i].size() == 7)
                nums.push_back(8);
            // 2, 3, 5
            else if (signal.io[i].size() == 5)
            {
                // 2
                if (count_commons(digits[0], signal.io[i]) == 1 && count_commons(digits[1], signal.io[i]) == 2 && count_commons(digits[2], signal.io[i]) == 2 && count_commons(digits[3], signal.io[i]) == 5)
                    nums.push_back(2);
                // 3
                if (count_commons(digits[0], signal.io[i]) == 2 && count_commons(digits[1], signal.io[i]) == 3 && count_commons(digits[2], signal.io[i]) == 3 && count_commons(digits[3], signal.io[i]) == 5)
                    nums.push_back(3);
                // 5
                if (count_commons(digits[0], signal.io[i]) == 1 && count_commons(digits[1], signal.io[i]) == 3 && count_commons(digits[2], signal.io[i]) == 2 && count_commons(digits[3], signal.io[i]) == 5)
                    nums.push_back(5);
            }
            // 0, 6, 9
            else if (signal.io[i].size() == 6)
            {
                // 0
                if (count_commons(digits[0], signal.io[i]) == 2 && count_commons(digits[1], signal.io[i]) == 3 && count_commons(digits[2], signal.io[i]) == 3 && count_commons(digits[3], signal.io[i]) == 6)
                    nums.push_back(0);
                // 6
                if (count_commons(digits[0], signal.io[i]) == 1 && count_commons(digits[1], signal.io[i]) == 3 && count_commons(digits[2], signal.io[i]) == 2 && count_commons(digits[3], signal.io[i]) == 6)
                    nums.push_back(6);
                // 9
                if (count_commons(digits[0], signal.io[i]) == 2 && count_commons(digits[1], signal.io[i]) == 4 && count_commons(digits[2], signal.io[i]) == 3 && count_commons(digits[3], signal.io[i]) == 6)
                    nums.push_back(9);
            }
        cnt += nums[0] * 1000 + nums[1] * 100 + nums[2] * 10 + nums[3];    
    }
    return cnt;
}

int PartTwo(char const* input)
{
    std::ifstream ifs(input);
    std::vector<Signal> signals;
    read_all_signals(ifs, signals);
    return add_all_outputs(signals);
}

int main(int argc, char* argv[])
{
    std::cout << "In the output values, how many times do digits 1, 4, 7, or 8 appear?\n"
              << PartOne(argv[1]) << std::endl;

    std::cout << "What do you get if you add up all of the output values?\n"
              << PartTwo(argv[1]) << std::endl;

    return 0;
}