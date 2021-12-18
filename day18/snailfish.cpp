//
// Created by Harold on 2021/12/18.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <variant>
#include <memory>

struct Number
{
    std::variant<int, std::unique_ptr<Number>> left, right;
    Number* parent = nullptr;

    void Print() const
    {
        std::cout << '[';
        if (left.index() == 0)
            std::cout << std::get<0>(left);
        else
            std::get<1>(left)->Print();
        std::cout << ',';
        if (right.index() == 0)
            std::cout << std::get<0>(right);
        else
            std::get<1>(right)->Print();
        std::cout << ']';
    }

    void Reduce()
    {
        while (true)
        {
            if (ShouldExplode(1))
                continue;
            if (ShouldSplit())
                continue;
            return;
        }
    }

    int Magnitude() const
    {
        int res = 0;
        if (left.index() == 0)
            res += 3 * std::get<0>(left);
        else
            res += 3 * std::get<1>(left)->Magnitude();
        if (right.index() == 0)
            res += 2 * std::get<0>(right);
        else
            res += 2 * std::get<1>(right)->Magnitude();
        return res;
    }

private:
    bool ShouldExplode(int depth)
    {
        // explode when depth == 4
        if (depth == 4)
        {
            // explode left
            if (left.index() == 1)
            {
                std::get<1>(left)->explode();
                left = 0;
                return true;
            }
            // explode right
            if (right.index() == 1)
            {
                std::get<1>(right)->explode();
                right = 0;
                return true;
            }
        }

        // check left
        if (left.index() == 1 && std::get<1>(left)->ShouldExplode(depth + 1))
            return true;

        // check right
        if (right.index() == 1 && std::get<1>(right)->ShouldExplode(depth + 1))
            return true;

        return false;    
    }

    bool ShouldSplit()
    {
        // split left
        if (left.index() == 0 && std::get<0>(left) >= 10)
        {
            left = split(std::get<0>(left));
            return true;
        }
        if (left.index() == 1 && std::get<1>(left)->ShouldSplit())
            return true;

        // split right
        if (right.index() == 0 && std::get<0>(right) >= 10)
        {
            right = split(std::get<0>(right));
            return true;
        }
        if (right.index() == 1 && std::get<1>(right)->ShouldSplit())
            return true;

        return false;
    }

    void explode() const
    {
        // add left int to first int on left
        int* li = find_left();
        if (li) *li += std::get<0>(left);
        // add right int to first int on right
        int* ri = find_right();
        if (ri) *ri += std::get<0>(right);
    }

    int* find_left() const
    {
        if (parent == nullptr)
            return nullptr;
        // current node is left and it's not int, so search up
        if (parent->left.index() == 1 && std::get<1>(parent->left).get() == this)
            return parent->find_left();
        // current node is right and its left sibling is int
        if (parent->left.index() == 0)
            return &std::get<0>(parent->left);
        // current node is right, get right int of its left sibling
        return std::get<1>(parent->left)->right_int();
    }

    int* find_right() const
    {
        if (parent == nullptr)
            return nullptr;
        // current node is right and it's not int, so search up
        if (parent->right.index() == 1 && std::get<1>(parent->right).get() == this)
            return parent->find_right();
        // current node is left and its right sibling is int
        if (parent->right.index() == 0)
            return &std::get<0>(parent->right);
        // current node is left, get left int of its right sibling
        return std::get<1>(parent->right)->left_int();
    }

    int* left_int()
    {
        if (left.index() == 0) return &std::get<0>(left);
        return std::get<1>(left)->left_int();
    }

    int* right_int()
    {
        if (right.index() == 0) return &std::get<0>(right);
        return std::get<1>(right)->right_int();
    }

    std::unique_ptr<Number> split(int value)
    {
        auto num = std::make_unique<Number>();
        num->parent = this;
        num->left = static_cast<int>(std::floor(value / 2.0));
        num->right = static_cast<int>(std::ceil(value / 2.0));
        return num;
    }
};

bool A2I(std::string const& str, int* num) {
    char *end;
    *num = std::strtol(str.c_str(), &end, 10);
    return *end == '\0';
}

// build tree from outer to inner
std::unique_ptr<Number> ReadNumber(std::string const& str, Number* parent = nullptr)
{
    auto num = std::make_unique<Number>();
    num->parent = parent;

    int depth = 0;
    int comma = -1;
    // find first pair: depth == 1
    for (auto i = 0; i < str.size(); i++)
    {
        if (str[i] == '[')
            depth++;
        else if (str[i] == ']')
            depth--;
        else if (str[i] == ',' && depth == 1)
            comma = i;
    }
    auto left = str.substr(1, comma - 1);
    auto right = str.substr(comma + 1, str.size() - 1 - comma - 1);

    // recursively
    int tmp = 0;
    if (A2I(left, &tmp))
        num->left = tmp;
    else
        num->left = ReadNumber(left, num.get());
    if (A2I(right, &tmp))
        num->right = tmp;
    else
        num->right = ReadNumber(right, num.get());
    
    return num;
}

std::unique_ptr<Number> Sum(std::unique_ptr<Number> left, std::unique_ptr<Number> right)
{
    auto num = std::make_unique<Number>();
    left->parent = num.get();
    right->parent = num.get();
    num->left = std::move(left);
    num->right = std::move(right);
    return num;
}

void read_all(std::ifstream& ifs, std::vector<std::string>& numbers_str)
{
    std::string line;
    while (std::getline(ifs, line))
        numbers_str.push_back(line);
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    std::vector<std::string> numbers_str;
    read_all(ifs, numbers_str);
    auto root = ReadNumber(numbers_str[0]);
    // root->Print();
    // std::cout << std::endl;
    for (auto i = 1; i != numbers_str.size(); ++i)
    {
        root = Sum(std::move(root), ReadNumber(numbers_str[i]));
        root->Reduce();
        // root->Print();
        // std::cout << std::endl;
    }

    return root->Magnitude();
}

int PartTwo(char const* input)
{
    std::ifstream ifs(input);
    std::vector<std::string> numbers_str;
    read_all(ifs, numbers_str);
    int max = 0;
    // permutation
    for (auto i = 0; i < numbers_str.size(); i++)
        for (auto j = 0; j < numbers_str.size(); j++)
        {
            // skip A A
            if (i == j)
                continue;
            // A B
            auto res = Sum(ReadNumber(numbers_str[i]), ReadNumber(numbers_str[j]));
            res->Reduce();
            max = std::max(max, res->Magnitude());

            // B A
            res = Sum(ReadNumber(numbers_str[j]), ReadNumber(numbers_str[i]));
            res->Reduce();
            max = std::max(max, res->Magnitude());
        }

    return max;
}

int main(int argc, char* argv[])
{
    std::cout << "What is the magnitude of the final sum?\n"
              << PartOne(argv[1]) << std::endl;

    std::cout << "What is the largest magnitude of any sum of two different snailfish numbers from the homework assignment?\n"
              << PartTwo(argv[1]) << std::endl;

    return 0;
}