//
// Created by Harold on 2021/12/24.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <array>
#include <unordered_map>

// read in the expression consists of w x y z
// evaluate with all 14bits number
// check whether the number meet the MONAD requirement
//     - no 0 in any digit (1 <= w <= 9)
//     - final z == 0 if valid else invalid
// find the largest number

// need symbolic expression evaluation...?
// after investigating the input, found some simple rules
//     - 14 groups with 18 instructions per group (18 instructions in the same order)
//     - only z is passed through groups: w as digit input, x, y are all local to each group
//     - every group: w = input, x = z % 26 + b, z = z / a, if (x != w) z = z * 26 + w + c else z = z
//         - a is 1 or 26, 7 b > 0 and 7 b < 0
//         - a at line (4 + 18 * x), b at line (5 + 18 * x), c at line (15 + 18 * x)
//     - for valid model, z finally should be 0
// so need find valid a, b, c permutations

/*
inp w
mul x 0  -> x = 0
add x z  -> x = z
mod x 26 -> x %= 26
div z 1  -> z /= 1   -> z /= 1
add x 15 -> x += 15  -> x = z % 26 + 15

eql x w  -> x = x == w ? 1 : 0 \
                                -> x = x == w ? 0 : 1 \
eql x 0  -> x = x == 0 ? 1 : 0 /                       -> if (x == w) x = 0, z = z else x = 1, z = z * 26 + w + 9
                                                      /                               |
mul y 0  -> y *= 0                                   /                 if (x != w) z = z * 26 + w + 9
add y 25 -> y += 25                                 /
mul y x  -> y *= x                         ---------
add y 1  -> y += 1                        /
mul z y  -> z *= y  -> z *= (x * 25 + 1) /

mul y 0  -> y *= 0
add y w  -> y += w
add y 9  -> y += 9
mul y x  -> y *= x
add z y  -> z += y  -> z += (w + 9) * x
*/

constexpr int DIGITS = 14;

// digit are tested from most to least significant
// 13579246899999: 1 -> 3 -> 5 -> ... -> 9
// so the power is in reverse
constexpr std::array<int64_t, DIGITS> POW10 = []() constexpr noexcept {
    std::array<int64_t, DIGITS> pow10{};
    pow10[DIGITS - 1] = 1;
    for (auto i = DIGITS - 2; i >= 0; i--)
        pow10[i] = pow10[i + 1] * 10;
    return pow10;
}();

std::vector<std::string> read_all(std::ifstream& ifs)
{
    std::vector<std::string> instructions;
    std::string line;
    while (std::getline(ifs, line))
    {
        instructions.emplace_back(line);
    }
    return instructions;
}

struct ABC
{
    int a = 0, b = 0, c = 0;

    ABC() = default;
    ABC(int a_, int b_, int c_) : a(a_), b(b_), c(c_) { }

    void parse_a(std::string const& line) &
    {
        std::sscanf(line.c_str(), "div z %d", &a);
    }

    void parse_b(std::string const& line) &
    {
        std::sscanf(line.c_str(), "add x %d", &b);
    }

    void parse_c(std::string const& line) &
    {
        std::sscanf(line.c_str(), "add y %d", &c);
    }
};

std::ostream& operator<<(std::ostream& os, ABC const& abc)
{
    os << abc.a << ", " << abc.b << ", " << abc.c;
    return os;
}

std::array<ABC, DIGITS> parse_abc(std::vector<std::string> const& instructions)
{
    std::array<ABC, DIGITS> abc;
    for (auto i = 0; i < DIGITS; i++)
    {
        auto ia = 4  + i * 18;
        auto ib = 5  + i * 18;
        auto ic = 15 + i * 18;
        ABC abc_;
        abc_.parse_a(instructions[ia]);
        abc_.parse_b(instructions[ib]);
        abc_.parse_c(instructions[ic]);
        abc[i] = std::move(abc_);
    }
    return abc;
}

struct MONAD
{
    std::array<ABC, DIGITS> parameters;
    // memorization: digit_no -> z : temp_model_num
    std::array<std::unordered_map<int, int64_t>, DIGITS> temp_model_numbers;

    MONAD(std::array<ABC, DIGITS> parameters_) : parameters(std::move(parameters_)) { }

    // DFS
    int64_t compute(int digit_no, int z, bool is_max = true)
    {
        // check validation after computed all digits
        if (digit_no == DIGITS)
            if (z == 0)
                return 0;
            else 
                return -1;
        
        // already computed
        if (temp_model_numbers[digit_no].find(z) != temp_model_numbers[digit_no].end())
            return temp_model_numbers[digit_no][z];
        // set default = -1;
        temp_model_numbers[digit_no][z] = -1;
        // compute model number (1 <= w <= 9), DFS
        // to obtain the largest model number: w from 9 to 1
        if (is_max)
            for (int w = 9; w >= 1; w--)
            {
                auto abc = parameters[digit_no];
                // x = z % 26 + b, z = z / a, if (x != w) z = z * 26 + w + c else z = z
                int x = z % 26 + abc.b;
                int n_z = z / abc.a;
                if (x != w)
                    n_z = n_z * 26 + w + abc.c;
                
                int64_t tmp = compute(digit_no + 1, n_z);
                // invalid
                if (tmp == -1) continue;
                tmp += w * POW10[digit_no];
                temp_model_numbers[digit_no][z] = tmp;
                return tmp;
            }
        else
        // to obtain the smallest model number: w from 1 to 9
            for (int w = 1; w <= 9; w++)
            {
                auto abc = parameters[digit_no];
                // x = z % 26 + b, z = z / a, if (x != w) z = z * 26 + w + c else z = z
                int x = z % 26 + abc.b;
                int n_z = z / abc.a;
                if (x != w)
                    n_z = n_z * 26 + w + abc.c;
                
                int64_t tmp = compute(digit_no + 1, n_z, false);
                // invalid
                if (tmp == -1) continue;
                tmp += w * POW10[digit_no];
                temp_model_numbers[digit_no][z] = tmp;
                return tmp;
            }
        return temp_model_numbers[digit_no][z];
    }
};

int64_t PartOne(char const* input)
{
    std::ifstream ifs(input);
    auto instructions = read_all(ifs);
    auto abc = parse_abc(instructions);
    // for (auto const& p : abc)
    //     std::cout << p << '\n';
    // std::cout << std::endl;

    MONAD monad(abc);
    // init_z = 0
    return monad.compute(0, 0);
}

int64_t PartTwo(char const* input)
{
    std::ifstream ifs(input);
    auto instructions = read_all(ifs);
    auto abc = parse_abc(instructions);

    MONAD monad(abc);
    // init_z = 0
    return monad.compute(0, 0, false);
}


int main(int argc, char* argv[])
{
    std::cout << "What is the largest model number accepted by MONAD?\n"
              << PartOne(argv[1]) << std::endl;
    std::cout << "What is the smallest  model number accepted by MONAD?\n"
              << PartTwo(argv[1]) << std::endl;

    return 0;
}