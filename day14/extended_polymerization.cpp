//
// Created by Harold on 2021/12/14.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>

using Template = std::list<char>;
using Rules = std::unordered_map<std::string, char>;

using ElementCounter = std::unordered_map<char, size_t>;
using PairCounter = std::unordered_map<std::string, size_t>;

void read_template(std::ifstream& ifs, Template& tmp)
{
    std::string line;
    std::getline(ifs, line);
    for (auto c : line) tmp.push_back(c);
}

void read_rules(std::ifstream& ifs, Rules& rules)
{
    std::string line;
    std::getline(ifs, line);
    while (std::getline(ifs, line))
    {
        auto pos = line.find(" -> ");
        rules[line.substr(0, pos)] = line[pos + 4];
    }
}

void print_template(Template const& tmp)
{
    for (auto c : tmp) std::cout << c;
    std::cout << '\n';
}

void print_rules(Rules const& rules)
{
    for (auto const& kv : rules)
        std::cout << kv.first << " -> " << kv.second << '\n';
}

void OneStep(Template& tmp, Rules& rules)
{
    for (auto it = tmp.begin(); it != tmp.end(); it++)
    {
        std::string s(2, ' ');
        s[0] = *it;
        it++;
        s[1] = *it;
        if (rules.find(s) != rules.end())
            it = tmp.insert(it, rules[s]);
        else
            it--;
    }
}

int PartOne(char const* input, int steps)
{
    std::ifstream ifs(input);
    Template tmp;
    read_template(ifs, tmp);
    //print_template(tmp);

    Rules rules;
    read_rules(ifs, rules);
    //print_rules(rules);

    for (auto i = 0; i < steps; i++)
    {
        OneStep(tmp, rules);
        //print_template(tmp);
    }

    std::vector<int> counter(26);  // a-z
    for (auto c : tmp)
        counter[c - 'A']++;
    
    std::sort(counter.begin(), counter.end(), std::greater<int>());

    return counter[0] - *(std::find(counter.begin(), counter.end(), 0) - 1);
}

// AB -> ACB : counts(AC, n + 1) += counts(AB, n); counts(CB, n + 1) += counts(AB, n);
void OneStep2(Rules& rules, ElementCounter& element_counter, PairCounter& pair_counter)
{
    PairCounter new_pair_counter;
    for (auto const& kv : pair_counter)
        if (rules.find(kv.first) != rules.end())
        {
            char a = kv.first[0];
            char b = kv.first[1];
            char c = rules[kv.first];

            new_pair_counter[std::string({a, c})] += kv.second;  // ac(n + 1) += ab(n)
            new_pair_counter[std::string({c, b})] += kv.second;  // cb(n + 1) += ab(n)

            element_counter[c] += kv.second;  // ab -> c
        }
        else
            new_pair_counter[kv.first] = kv.second;
    pair_counter = new_pair_counter;
}

void print_element_counter(ElementCounter const& element_counter)
{
    for (auto const& kv : element_counter)
        std::cout << kv.first << ": " << kv.second << '\n';
    std::cout << '\n';
}

void print_pair_counter(PairCounter const& pair_counter)
{
    for (auto const& kv : pair_counter)
        std::cout << kv.first << ": " << kv.second << '\n';
    std::cout << '\n';
}

size_t PartTwo(char const* input, int steps)
{
    std::ifstream ifs(input);
    std::string tmp;
    std::getline(ifs, tmp);

    Rules rules;
    read_rules(ifs, rules);

    // initialize conters

    // element counter
    ElementCounter element_counter;
    for (auto c : tmp)
        element_counter[c]++;
    //print_element_counter(element_counter);

    // pair counter
    PairCounter pair_counter;
    for (auto i = 0; i < tmp.size() - 1; i++)
    {
        std::string s(2, ' ');
        s[0] = tmp[i];
        s[1] = tmp[i + 1];
        pair_counter[std::string({ tmp[i], tmp[i + 1] })]++;
    }
    //print_pair_counter(pair_counter);

    for (auto i = 0; i < steps; i++)
    {
        OneStep2(rules, element_counter, pair_counter);
        //print_element_counter(element_counter);
        //print_pair_counter(pair_counter);
    }

    auto minmax = std::minmax_element(element_counter.begin(), element_counter.end(), [](ElementCounter::value_type const& p0, ElementCounter::value_type const& p1){ return p0.second < p1.second; });

    return minmax.second->second - minmax.first->second;
}

int main(int argc, char* argv[])
{
    std::cout << "What do you get if you take the quantity of the most common element and subtract the quantity of the least common element?\n"
              << PartOne(argv[1], 10) << std::endl;

    std::cout << "What do you get if you take the quantity of the most common element and subtract the quantity of the least common element?\n"
              << PartTwo(argv[1], 40) << std::endl;

    return 0;
}