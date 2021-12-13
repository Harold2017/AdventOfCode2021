//
// Created by Harold on 2021/12/13.
//

#include "../day11/mat.h"

using Paper = Mat<char>;
using Point = Paper::Point;
using Fold = std::pair<char, int>;
using Folds = std::vector<Fold>;

void read_paper(std::ifstream& ifs, Paper& paper)
{
    std::string line;
    std::vector<Point> points;
    int x = 0, y = 0;
    int xmax = 0, ymax = 0;
    while (std::getline(ifs, line))
    {
        if (line.empty())
            break;
        std::sscanf(line.c_str(), "%d,%d", &x, &y);
        points.emplace_back(x, y);
        if (x > xmax) xmax = x;
        if (y > ymax) ymax = y;
    }

    ymax += 1;
    xmax += 1;

    paper.rows = ymax;
    paper.cols = xmax;
    paper.data.resize(xmax * ymax, '.');
    for (auto const& p : points)
        paper(p.y, p.x) = '#';
}

void read_fold(std::ifstream& ifs, Folds& fs)
{
    std::string line;
    while (std::getline(ifs, line))
    {
        auto pos = line.find('=');
        fs.emplace_back(line[pos - 1], std::stoi(line.substr(pos + 1)));
    }
}

void print_paper(Paper const& paper)
{
    std::cout << paper << std::endl;
}

void print_folds(Folds const& fs)
{
    for (auto const& kv : fs)
        std::cout << kv.first << '=' << kv.second << '\n';
}

void fold(Paper& paper, Fold fold)
{
    int anchor = fold.second;
    if (fold.first == 'x')
    {
        Paper new_paper;
        new_paper.rows = paper.rows;
        new_paper.cols = anchor;
        new_paper.data.resize(paper.rows * anchor, '.');
        for (int i = 0; i < paper.rows; i++)
            for (int j = anchor * 2; j > anchor; j--)
                if (paper(i, j) == '#' || paper(i, anchor * 2 - j) == '#')
                    new_paper(i, anchor * 2 - j) = '#';
        paper = new_paper;
    }
    else if (fold.first == 'y')
    {
        for (int i = anchor * 2; i > anchor; i--)
            for (int j = 0; j < paper.cols; j++)
                if (paper(i, j) == '#')
                    paper(anchor * 2 - i, j) = '#';
        paper.data.resize(anchor * paper.cols);
        paper.rows = anchor;
    }
}

int count_visible_dots(Paper const& paper)
{
    int cnt = 0;
    for (auto d : paper.data)
        if (d == '#')
            cnt++;
    return cnt;
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    Paper paper;
    read_paper(ifs, paper);
    //print_paper(paper);

    Folds fs;
    read_fold(ifs, fs);
    //print_folds(fs);

    // for (auto const& f : fs)
    //     fold(paper, f);
    //print_paper(paper);
    fold(paper, fs[0]);

    return count_visible_dots(paper);
}

void PartTwo(char const* input)
{
    std::ifstream ifs(input);
    Paper paper;
    read_paper(ifs, paper);

    Folds fs;
    read_fold(ifs, fs);

    for (auto const& f : fs)
        fold(paper, f);
    print_paper(paper);
}

int main(int argc, char* argv[])
{
    std::cout << "How many dots are visible after completing just the first fold instruction on your transparent paper?\n"
              << PartOne(argv[1]) << std::endl;

    std::cout << "What code do you use to activate the infrared thermal imaging camera system?\n";
    PartTwo(argv[1]);

    return 0;
}