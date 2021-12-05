//
// Created by Harold on 2021/12/5.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

struct Board
{
    std::array<std::array<int, 5>, 5> m;
    bool win = false;
};

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

bool read_one_board(std::ifstream& ifs, Board& board)
{
    std::string line;
    std::getline(ifs, line);
    for (int i = 0; i < 5; i++)
    {
        std::getline(ifs, line);
        if (line.empty()) return false;
        std::istringstream ss(line);
        for (int j = 0; j < 5; j++)
            ss >> board.m[i][j];
    }
    return true;
}

void draw_board(Board& board, int num)
{
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            if (board.m[i][j] == num)
                board.m[i][j] = -1;
}

bool is_board_win(Board const& board)
{
    for (int i = 0; i < 5; i++)
    {
        int r_sum = 0, c_sum = 0;
        for (int j = 0; j < 5; j++)
        {
            // row-wise
            r_sum += board.m[i][j];
            // column-wise
            c_sum += board.m[j][i];
        }
        if (r_sum == -5 || c_sum == -5)
                return true;
    }
    return false;
}

std::pair<Board, int> find_board_win(std::vector<Board>& boards, std::vector<int> markers)
{
    for (auto m : markers)
    {
        for (auto& b : boards)
        {
            draw_board(b, m);
            if (is_board_win(b))
                return std::make_pair(b, m);
        }
    }
    return std::make_pair(Board(), -1);
}

std::pair<Board, int> find_board_last_win(std::vector<Board>& boards, std::vector<int> markers)
{
    Board last_board;
    int last_marker;
    for (auto m : markers)
    {
        for (auto& b : boards)
        {
            draw_board(b, m);
            if (is_board_win(b) && !b.win)
            {
                last_board = b;
                last_marker = m;
                b.win = true;
            }
        }
    }
    return std::make_pair(last_board, last_marker);
}

void print_board(Board const& board)
{
    for (int i = 0; i < 5; i++)
    {
        for (auto j = 0; j < 5; j++)
            std::cout << board.m[i][j] << ' ';
        std::cout << '\n';
    }
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    std::vector<int> nums;
    read_all_nums(ifs, nums);
    std::vector<Board> boards;
    Board board;
    while (read_one_board(ifs, board))
        boards.push_back(board);

    auto bm = find_board_win(boards, nums);
    board = bm.first;
    auto marker = bm.second;
    // print_board(board);
    // std::cout << "maker: " << marker << std::endl;
    int sum_unmarked = 0;
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            if (board.m[i][j] != -1)
                sum_unmarked += board.m[i][j];
    return sum_unmarked * marker;
}

int PartTwo(char const* input)
{
    std::ifstream ifs(input);
    std::vector<int> nums;
    read_all_nums(ifs, nums);
    std::vector<Board> boards;
    Board board;
    while (read_one_board(ifs, board))
        boards.push_back(board);

    auto bm = find_board_last_win(boards, nums);
    board = bm.first;
    auto marker = bm.second;
    // print_board(board);
    // std::cout << "maker: " << marker << std::endl;
    int sum_unmarked = 0;
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            if (board.m[i][j] != -1)
                sum_unmarked += board.m[i][j];
    return sum_unmarked * marker;
}

int main(int argc, char* argv[])
{
    std::cout << "What will your final score be if you choose that board?" << '\n'
              << PartOne(argv[1]) << std::endl;

    std::cout << "Once it wins, what would its final score be?" << '\n'
              << PartTwo(argv[1]) << std::endl;
    return 0;
}