//
// Created by Harold on 2021/12/10.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <stack>
#include <map>

void read_all(std::ifstream& ifs, std::vector<std::string>& chars)
{
    std::string line;
    while (std::getline(ifs, line))
        chars.push_back(line);
}

bool is_right(char c)
{
    if (c == ')' || c == ']' || c == '}' || c == '>')
        return true;
    return false;
}

bool is_pair(char left, char right)
{
    if (left == '(' && right == ')')
        return true;
    if (left == '[' && right == ']')
        return true;
    if (left == '{' && right == '}')
        return true;
    if (left == '<' && right == '>')
        return true;
    return false;
}

int get_score(char c)
{
    if (c == ')') return 3;
    if (c == ']') return 57;
    if (c == '}') return 1197;
    if (c == '>') return 25137;
    return 0;
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    std::vector<std::string> chars;
    read_all(ifs, chars);
    std::map<char, int> counter = {
        {')', 0},
        {']', 0},
        {'}', 0},
        {'>', 0}
    };
    for (auto& str : chars)
    {
        std::stack<char> s;
        for (auto c : str)
        {
            if (!is_right(c))
            {
                s.push(c);
                continue;
            }
            char last_left = s.top();
            s.pop();
            if (!is_pair(last_left, c))
            {
                counter[c]++;
                break;
            }
        }
    }

    int score = 0;
    for (auto const& kv : counter)
        score += get_score(kv.first) * kv.second;

    return score;
}

size_t PartTwo(char const* input)
{
    std::ifstream ifs(input);
    std::vector<std::string> chars;
    read_all(ifs, chars);
    std::vector<size_t> scores;
    bool skip_line = false;
    for (auto& str : chars)
    {
        std::stack<char> s;
        // syntax error
        for (auto c : str)
        {
            if (!is_right(c))
            {
                s.push(c);
                continue;
            }
            char last_left = s.top();
            s.pop();
            if (!is_pair(last_left, c))
            {
                skip_line = true;
                break;
            }
        }
        if (skip_line)
        {
            skip_line = false;
            continue;
        }

        // incomplete
        size_t score = 0;
        while (!s.empty())
        {
            char last_left = s.top();
            s.pop();
            if (last_left == '(')
                score = score * 5 + 1;
            else if (last_left == '[')
                score = score * 5 + 2;
            else if (last_left == '{')
                score = score * 5 + 3;
            else if (last_left == '<')
                score = score * 5 + 4;
        }
        scores.push_back(score);
    }

    std::sort(scores.begin(), scores.end());

    return scores[(scores.size() - 1) / 2];
}

int main(int argc, char* argv[])
{
    std::cout << "What is the total syntax error score for those errors?\n"
              << PartOne(argv[1]) << std::endl;

    std::cout << "What is the middle score?\n"
              << PartTwo(argv[1]) << std::endl;
    return 0;
}
