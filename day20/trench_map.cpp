//
// Created by Harold on 2021/12/20.
//

#include "../day11/mat.h"
#include <unordered_set>

using Image = Mat<char>;
using Point = Image::Point;
using PointHasher = Image::PointHasher;

std::ostream& operator<<(std::ostream& os, Point const& p)
{
    std::cout << '(' << p.x << ", " << p.y << ")";
    return os;
}

void read_algo(std::ifstream& ifs, std::string& algo)
{
    std::getline(ifs, algo);
}

void read_img(std::ifstream& ifs, Image& img)
{
    std::string line;
    std::getline(ifs, line);  // empty line
    char c;
    int rows = 0, cols = 0;
    bool first = true;
    while (std::getline(ifs, line))
    {
        for (auto c : line)
        {
            img.data.push_back(c);
            if (first)
                cols++;
        }
            
        first = false;
        rows++;
    }
    img.cols = cols;
    img.rows = rows;
}

Image apply_algo(Image const& img, std::string const& algo, char padding)
{
    // with padding
    // -1 rows 1
    // -1 cols 1
    Image res;
    res.rows = img.rows + 2;
    res.cols = img.cols + 2;
    res.data.resize(res.rows * res.cols, '.');
    //\note take care here, need convert img.rows (size_t) to int first!!! otherwise, i will be convert to size_t!!!
    for (int i = -1; i < int(img.rows) + 1; i++)
    {
        for (int j = -1; j < int(img.cols) + 1; j++)
        {
            std::string val; val.reserve(9);
            char pixel = '#';
            for (auto ii = i - 1; ii <= i + 1; ii++)
                for (auto jj = j - 1; jj <= j + 1; jj++)
                {
                    if (img.valid_pos(ii, jj))
                        pixel = img(ii, jj);
                    else
                        pixel = padding;
                    val.push_back(pixel == '#' ? '1' : '0');
                }
            unsigned idx = std::strtol(val.c_str(), nullptr, 2);
            res(i + 1, j + 1) = algo[idx];
        }
    }
    return res;
}

int count_light(Image const& img)
{
    int cnt = 0;
    for (auto c : img.data)
        if (c == '#')
            cnt++;
    return cnt;
}

int PartOne(char const* input, int times)
{
    std::ifstream ifs(input);
    std::string algo;
    read_algo(ifs, algo);
    //std::cout << algo << std::endl;
    Image img;
    read_img(ifs, img);
    //std::cout << img << std::endl;

    char padding = '.';
    Image res = apply_algo(img, algo, padding);
    //std::cout << res << std::endl;
    for (auto i = 1; i < times; i++)
    {
        if (padding == '.')
            padding = algo.front();
        else
            padding = algo.back();
        res = apply_algo(res, algo, padding);
        //std::cout << res << std::endl;
    }

    return count_light(res);
}

int main(int argc, char* argv[])
{
    std::cout << "How many pixels are lit in the resulting image?\n"
              << PartOne(argv[1], 2) << std::endl;
    std::cout << "How many pixels are lit in the resulting image?\n"
              << PartOne(argv[1], 50) << std::endl;

    return 0;
}