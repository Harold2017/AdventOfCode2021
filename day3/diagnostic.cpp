//
// Created by Harold on 2021/12/3.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

int count_code_length(std::ifstream& ifs)
{
    if (ifs)
        ifs.seekg(0, ifs.beg);
    else
        return 0;
    char c;
    int cnt = -1;
    do
    {
        ifs.read(&c, 1);
        cnt++;
    } while (c != '\n');
    ifs.seekg(0, ifs.beg);
    return cnt;
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    auto length = count_code_length(ifs);
    std::string line;
    std::vector<int> cnts(length);
    while (std::getline(ifs, line))
    {
        for (int i = 0; i < length; i++)
            if (line[i] == '0')
                cnts[i]++;
            else if (line[i] == '1')
                cnts[i]--;
    }
    std::string gamma_str(length, '0'), epsilon_str(length, '1');
    for (auto i = 0; i < length; i++)
        if (cnts[i] > 0)
        {
            gamma_str[i] = '0';
            epsilon_str[i] = '1';
        }
        else
        {
            gamma_str[i] = '1';
            epsilon_str[i] = '0';
        }

    int gamma = std::stoi(gamma_str, nullptr, 2);
    int epsilon = std::stoi(epsilon_str, nullptr, 2);
    
    return gamma * epsilon;
}

int PartTwo(char const* input)
{
    std::ifstream ifs(input);
    auto length = count_code_length(ifs);
    std::string line;
    std::vector<std::string> strs;
    while (std::getline(ifs, line))
        strs.push_back(line);

    std::vector<std::string> ogr(strs);
    std::vector<std::string> csr(std::move(strs));
    auto predict_ogr = [](char c, char MCB) { return c == MCB; };
    auto predict_csr = [](char c, char MCB) { return c != MCB; };

    for (int i = 0; i < length; i++)  // at most length turn
    {
        int cnt_ogr = 0;
        int cnt_csr = 0;
        // count for MCB
        for (auto const& s : ogr)
        {
            if (s[i] == '0')  // s has size == length, so no worry about index range
                cnt_ogr++;
            else if (s[i] == '1')
                cnt_ogr--;
        }
        for (auto const& s : csr)
        {
            if (s[i] == '0')
                cnt_csr++;
            else if (s[i] == '1')
                cnt_csr--;
        }

        // update ogr and csr
        if (ogr.size() > 1)
        {
            if (cnt_ogr > 0)  // `0` as MCB
            {
                auto it = std::partition(ogr.begin(), ogr.end(), [=](std::string const& s) { return !predict_ogr(s[i], '0'); });
                if (it != ogr.end())
                    ogr.erase(ogr.begin(), it);
            }
            else  // `1` as MCB
            {
                auto it = std::partition(ogr.begin(), ogr.end(), [=](std::string const& s) { return !predict_ogr(s[i], '1'); });
                if (it != ogr.end())
                    ogr.erase(ogr.begin(), it);
            }
        }
        if (csr.size() > 1)
        {
            if (cnt_csr > 0)  // `0` as MCB
            {
                auto it = std::partition(csr.begin(), csr.end(), [=](std::string const& s) { return predict_csr(s[i], '1'); });
                if (it != csr.end())
                    csr.erase(csr.begin(), it);
            }
            else  // `1` as MCB
            {
                auto it = std::partition(csr.begin(), csr.end(), [=](std::string const& s) { return predict_csr(s[i], '0'); });
                if (it != csr.end())
                    csr.erase(csr.begin(), it);
            }
        }

        if (ogr.size() == 1 && csr.size() == 1)
            break;

        // std::cout << "turn " << i << ":\n";
        // std::cout << "ogr: " << cnt_ogr  <<  "\n";
        // for (auto const& s : ogr) std::cout << s << '\n';
        // std::cout << "csr: " << cnt_csr  <<  "\n";
        // for (auto const& s : csr) std::cout << s << '\n';
    }

    int ogr_num = std::stoi(ogr[0], nullptr, 2);
    int csr_num = std::stoi(csr[0], nullptr, 2);
    
    return ogr_num * csr_num;
}

int main(int argc, char* argv[])
{
    std::cout << "What is the power consumption of the submarine?" << '\n'
              << PartOne(argv[1]) << std::endl;
    std::cout << "What is the life support rating of the submarine?" << '\n'
              << PartTwo(argv[1]) << std::endl;
    return 0;
}