//
// Created by Harold on 2021/12/13.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using Edge = std::string[2];
using ConnectionMap = std::unordered_map<std::string, std::vector<std::string>>;
using VisitedMap = std::unordered_map<std::string, int>;
using PathSet = std::unordered_set<std::string>;

void read_all(std::ifstream& ifs, ConnectionMap& cm, VisitedMap& visited)
{
    std::string line, source, target;
    while (std::getline(ifs, line))
    {
        std::stringstream ss(line);
        std::getline(ss, source, '-');
        std::getline(ss, target);
        cm[source].push_back(target);
        cm[target].push_back(source);
        visited.insert({ source, 1 });
        visited.insert({ target, 1 });
    }
}

bool is_small_cave(std::string const& s)
{
    return s != "start" && s != "end" && std::islower(s[0]);
}

void find_path(ConnectionMap const& cm, std::string const& node, std::string subpath, VisitedMap& visited, PathSet& paths)
{
    // not allow visit small cave twice
    if (is_small_cave(node))
    {
        if (visited[node] == 2)
            return;
        visited[node]++;
    }

    subpath += node + " ";

    // end path
    if (node == "end")
    {
        paths.insert(subpath);
        return;
    }

    // search for next
    auto it = cm.find(node);
    if (it == cm.end())
        return;
    for (auto const& n : it->second)
    {
        // skip path back to start
        if (n == "start") continue;

        find_path(cm, n, subpath, visited, paths);
    }

    // reset visited
    if (is_small_cave(node))
        visited[node]--;
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    ConnectionMap cm;
    VisitedMap visited;
    read_all(ifs, cm, visited);

    // for (auto const& kv : cm)
    // {
    //     std::cout << kv.first << ": [";
    //     for (auto const& s : kv.second)
    //         std::cout << s << ", ";
    //     std::cout << "]\n";
    // }

    PathSet paths;
    find_path(cm, "start", "", visited, paths);

    // for (auto const& p : paths)
    //     std::cout << p << '\n';

    return paths.size();
}

int PartTwo(char const* input)
{
    std::ifstream ifs(input);
    ConnectionMap cm;
    VisitedMap visited;
    read_all(ifs, cm, visited);

    PathSet paths;
    for (auto const& kv : visited)
        if (is_small_cave(kv.first))
        {
            auto visited_copy = visited;
            // one single small cave can be visited twice
            visited_copy[kv.first] = 0;
            find_path(cm, "start", "", visited_copy, paths);
        }

    // for (auto const& p : paths)
    //     std::cout << p << '\n';

    return paths.size();
}

int main(int argc, char* argv[])
{
    std::cout << "How many paths through this cave system are there that visit small caves at most once?\n"
              << PartOne(argv[1]) << std::endl;

    std::cout << "How many paths through this cave system are there?\n"
              << PartTwo(argv[1]) << std::endl;

    return 0;
}