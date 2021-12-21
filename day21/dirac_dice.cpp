//
// Created by Harold on 2021/12/21.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <memory>
#include <unordered_map>

class Dice {
public:
    virtual int Roll() = 0;
    virtual int GetTurns() = 0;
};

class DeterministicDice : public Dice
{
public:
    int Roll() override
    {
        int ret = 3 * num + 3;
        num += 3;
        cnt += 3;
        return ret;
    }

    int GetTurns() override
    {
        return cnt;
    }

private:
    int num = 1;
    int cnt = 0;
};

class Player
{
public:
    Player() = default;
    Player(int starting_position_, std::shared_ptr<Dice> dice_, int win_score_)
        : pos(starting_position_), dice(dice_), win_score(win_score_) { }

    void play()
    {
        pos = (pos + dice->Roll() % 10 - 1) % 10 + 1;
        score += pos; 
    }

    bool is_win()
    {
        if (score >= win_score)
            return true;
        else
            return false;
    }

    int get_score()
    {
        return score;
    }

    friend std::ostream& operator<<(std::ostream& os, Player const& p);

private:
    int pos = -1;
    int score = 0;
    int win_score = 0;
    std::shared_ptr<Dice> dice = nullptr;
};

std::ostream& operator<<(std::ostream& os, Player const& p)
{
    os << "pos: " << p.pos << ", score: " << p.score;
    return os;
}

std::pair<int, int> read_all(std::ifstream& ifs)
{
    std::string line;
    int sp1 = 0, sp2 = 0;
    std::getline(ifs, line);
    std::sscanf(line.c_str(), "Player 1 starting position: %d", &sp1);
    std::getline(ifs, line);
    std::sscanf(line.c_str(), "Player 2 starting position: %d", &sp2);
    return std::make_pair(sp1, sp2);
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    auto dice = std::make_shared<DeterministicDice>();
    auto poses = read_all(ifs);
    Player p1(poses.first, dice, 1000), p2(poses.second, dice, 1000);
    Player loser;
    while (true)
    {
        //std::cout << p1 << ", " << p2 << '\n';
        p1.play();
        if (p1.is_win())
        {
            loser = p2;
            break;
        }
        p2.play();
        if (p2.is_win())
        {
            loser = p1;
            break;
        }
    }

    return loser.get_score() * dice->GetTurns();
}

// class DiracDice : public Dice
// {
// public:
//     int Roll() override
//     {
//         int ret = roll_set[num];
//         num += 1;
//         if (num > 9) num = 3;
//         cnt += 3;
//         return ret;
//     }

//     int GetTurns() override
//     {
//         return cnt;
//     }

//     // (1, 2, 3) * 3 => 3 ^ 3 = 27
//     // combination  sum  amount
//     // 111        -> 3   (1)
//     // 112        -> 4   (3)
//     // 113, 122   -> 5   (6)
//     // 123, 222   -> 6   (7)
//     // 133, 223   -> 7   (6)
//     // 233        -> 8   (3)
//     // 333        -> 9   (1)
//     static void generate_roll_set()
//     {
//         for (auto i = 0; i <= 3; i++)
//             for (auto j = 0; j <= 3; j++)
//                 for (auto k = 0; k <= 3; k++)
//                     roll_set[i + j + k] += 1;
//     }

// private:
//     int num = 3;
//     int cnt = 0;

//     static std::unordered_map<int, int> roll_set;  // roll number : roll cnt
// };

// std::unordered_map<int, int> DiracDice::roll_set;

struct State
{
    int p1_pos, p1_score;
    int p2_pos, p2_score;

    bool operator==(State const& other) const
    {
        return p1_pos == other.p1_pos && p1_score == other.p1_score && p2_pos == other.p2_pos && p2_score == other.p2_score;
    }

    struct Hasher {
        int operator()(State const& s) const
        {
            return std::hash<int>()(s.p1_pos) ^ std::hash<int>()(s.p1_score) ^ std::hash<int>()(s.p2_pos) ^ std::hash<int>()(s.p2_score);
        }
    };
};

using StateCounter = std::unordered_map<State, size_t, State::Hasher>;

size_t PartTwo(char const* input)
{
    std::ifstream ifs(input);
    auto poses = read_all(ifs);
    std::unordered_map<int, int> roll_set = {
        { 3, 1 },
        { 4, 3 },
        { 5, 6 },
        { 6, 7 },
        { 7, 6 },
        { 8, 3 },
        { 9, 1 },
    };
    StateCounter states = { {{ poses.first, 0, poses.second, 0 }, 1} };
    size_t p1_wins = 0, p2_wins = 0;
    bool p1_turn = true;
    while (!states.empty())
    {
        StateCounter new_states;
        for (auto const& state : states)
            for (auto roll = 3; roll <= 9; roll++)
                if (p1_turn)
                {
                    int p1_pos = (state.first.p1_pos + roll - 1) % 10 + 1;
                    int p1_score = state.first.p1_score + p1_pos;
                    if (p1_score >= 21)  // win
                        p1_wins += state.second * roll_set[roll];  // number of ways to reach this state x number of ways to obtain this roll number
                    else  // otherwise, update new state
                        new_states[{ p1_pos, p1_score, state.first.p2_pos, state.first.p2_score }] += state.second * roll_set[roll];
                }
                else
                {
                    int p2_pos = (state.first.p2_pos + roll - 1) % 10 + 1;
                    int p2_score = state.first.p2_score + p2_pos;
                    if (p2_score >= 21)  // win
                        p2_wins += state.second * roll_set[roll];  // number of ways to reach this state x number of ways to obtain this roll number
                    else  // otherwise, update new state
                        new_states[{ state.first.p1_pos, state.first.p1_score, p2_pos, p2_score }] += state.second * roll_set[roll];
                }
        p1_turn = !p1_turn;
        states = new_states;
    }

    return std::max(p1_wins, p2_wins);
}

int main(int argc, char* argv[])
{

    std::cout << "what do you get if you multiply the score of the losing player by the number of times the die was rolled during the game?\n"
              << PartOne(argv[1]) << std::endl;

    std::cout << "Find the player that wins in more universes; in how many universes does that player win?\n"
              << PartTwo(argv[1]) << std::endl;

    return 0;
}