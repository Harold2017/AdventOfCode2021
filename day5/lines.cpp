//
// Created by Harold on 2021/12/5.
//

#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

using Board = std::vector<std::vector<int>>;

struct Point
{
    int x, y;
};

struct Line
{
    Point s, e;
};

std::ostream& operator<<(std::ostream& os, Line const& line)
{
    os << "<(" << line.s.x << ", " << line.s.y << ") (" << line.e.x << ", " << line.e.y << ")>";
    return os;
}

int read_in_all_lines(std::ifstream& ifs, std::vector<Line>& lines)
{
    std::string str;
    Line line;
    int x0, x1, y0, y1;
    int N = 0;
    while (std::getline(ifs, str))
    {
        std::sscanf(str.c_str(), "%d,%d -> %d,%d", &x0, &y0, &x1, &y1);
        line.s.x = x0;
        line.s.y = y0;
        line.e.x = x1;
        line.e.y = y1;
        N = N < x0 ? x0 : N;
        N = N < x1 ? x1 : N;
        N = N < y0 ? y0 : N;
        N = N < y1 ? y1 : N;
        lines.push_back(line);
    }
    return N + 1;
}

Board genreate_board(std::vector<Line> const& lines, int N, bool diagonal = false)
{
    Board b(N, std::vector<int>(N, 0));  // N x N
    int minX, maxX, minY, maxY;
    for (auto const& line : lines)
    {
        // diagonal line
        if (line.s.x != line.e.x && line.s.y != line.e.y)
        {
            if (!diagonal) continue;
            // bottom left to upper right
            if (line.s.x < line.e.x && line.s.y < line.e.y)
            {
                for (int i = line.s.x, j = line.s.y; i <= line.e.x, j <= line.e.y; i++, j++)
                    b[j][i] += 1;
            }
            // upper left to bottom right
            else if (line.s.x < line.e.x && line.s.y > line.e.y)
            {
                for (int i = line.s.x, j = line.s.y; i <= line.e.x, j >= line.e.y; i++, j--)
                    b[j][i] += 1;
            }
            // bottom right to upper left
            else if (line.s.x > line.e.x && line.s.y < line.e.y)
            {
                for (int i = line.e.x, j = line.e.y; i <= line.s.x, j >= line.s.y; i++, j--)
                    b[j][i] += 1;
            }
            // upper right to bottom left
            else if (line.s.x > line.e.x && line.s.y > line.e.y)
            {
                for (int i = line.e.x, j = line.e.y; i <= line.s.x, j <= line.s.y; i++, j++)
                    b[j][i] += 1;
            }
        }
        // 0 length line
        else if (line.s.x == line.e.x && line.s.y == line.e.y)
        {
            // col, row
            b[line.s.y][line.s.x] += 1;
        }
        // horizontal line
        else if (line.s.x == line.e.x)
        {
            if (line.s.y < line.e.y)
            {
                minY = line.s.y;
                maxY = line.e.y;
            }
            else
            {
                minY = line.e.y;
                maxY = line.s.y;
            }
            for (int i = minY; i <= maxY; i++)
                b[i][line.s.x] += 1;
        }
        // vertical line
        else if (line.s.y == line.e.y)
        {
            if (line.s.x < line.e.x)
            {
                minX = line.s.x;
                maxX = line.e.x;
            }
            else
            {
                minX = line.e.x;
                maxX = line.s.x;
            }
            for (int i = minX; i <= maxX; i++)
                b[line.s.y][i] += 1;
        }
    }

    return b;
}

void print_board(Board const& board)
{
    for (int i = 0; i < board[0].size(); i++)
    {
        for (int j = 0; j < board.size(); j++)
            std::cout << board[i][j] << ' ';
        std::cout << '\n';
    }
}

int count_overlap(Board const& board)
{
    int cnt = 0;
    for (int i = 0; i < board[0].size(); i++)
        for (int j = 0; j < board.size(); j++)
            if (board[i][j] > 1)
                cnt++;

    return cnt;
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    std::vector<Line> lines;
    auto N = read_in_all_lines(ifs, lines);
    auto board = genreate_board(lines, N);
    //print_board(board);
    return count_overlap(board);
}

int PartTwo(char const* input)
{
    std::ifstream ifs(input);
    std::vector<Line> lines;
    auto N = read_in_all_lines(ifs, lines);
    auto board = genreate_board(lines, N, true);
    //print_board(board);
    return count_overlap(board);
    return 0;
}

int main(int argc, char* argv[])
{
    std::cout << "At how many points do at least two lines overlap?" << '\n'
              << PartOne(argv[1]) << std::endl;
    std::cout << "At how many points do at least two lines overlap?" << '\n'
              << PartTwo(argv[1]) << std::endl;
    return 0;
}