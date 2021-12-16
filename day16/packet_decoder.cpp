//
// Created by Harold on 2021/12/16.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

static std::unordered_map<char, std::string> hex_map = {
    { '0', "0000"},
    { '1', "0001"},
    { '2', "0010"},
    { '3', "0011"},
    { '4', "0100"},
    { '5', "0101"},
    { '6', "0110"},
    { '7', "0111"},
    { '8', "1000"},
    { '9', "1001"},
    { 'A', "1010"},
    { 'B', "1011"},
    { 'C', "1100"},
    { 'D', "1101"},
    { 'E', "1110"},
    { 'F', "1111"},
};

void read_all(std::ifstream& ifs, std::string& bs)
{
    char c;
    while (ifs >> c)
    {
        if (c == '\n') break;
        bs.append(hex_map[c]);
    }
}

int str2bin(std::string const& bs, int& pos, int length)
{
    auto str = bs.substr(pos, length);
    pos += length;
    int num = std::stoi(str, nullptr, 2);
    //std::cout << str << ", " << num << '\n';
    return num;
}

void count_literal(std::string const &bs, int &pos)
{
    bool is_last = true;
    while (true)
    {
        if (pos >= bs.size() - 5)
            break;
        is_last = bs[pos++] == '0' ? true : false;
        pos += 4;
        if (is_last)
            break;
    }
    return;
}

void ParseOnePacket(std::string const &bs, int &pos, int &sum)
{
    if (pos >= bs.size() - 6) return;

    int version_num = str2bin(bs, pos, 3);
    int type_id = str2bin(bs, pos, 3);

    sum += version_num;

    if (type_id == 4) // literal value
    {
        //std::cout << "literal" << '\n';
        count_literal(bs, pos);
    }
    else // operator
    {
        //std::cout << "operator" << '\n';

        //if (pos >= bs.size() - 1) return;

        int length_id = bs[pos++] == '0' ? 0 : 1;

        if (length_id == 0) // 15 bits
        {
            //std::cout << '0' << '\n';

            //if (pos >= bs.size() - 15) return;

            int length_of_subpackets = str2bin(bs, pos, 15);
            int end_pos = pos + length_of_subpackets;
            while (pos < end_pos && end_pos < bs.size())
                ParseOnePacket(bs, pos, sum);
        }
        else // 11 bits
        {
            //std::cout << '1' << '\n';

            //if (pos >= bs.size() - 11) return;

            int num_of_subpackets = str2bin(bs, pos, 11);
            for (auto i = 0; i < num_of_subpackets; i++)
                ParseOnePacket(bs, pos, sum);
        }
    }
}

struct Packet
{
    std::vector<Packet> subpackets;
    int version_num = 0;
    int type_id = 0;
    unsigned long long literal = 0;

    int total_version()
    {
        int sum = version_num;
        for (auto& sp : subpackets)
            sum += sp.total_version();
        return sum;
    }

    unsigned long long evaluation()
    {
        unsigned long long res = 0;
        switch (type_id)
        {
        case 0:
            res = 0;
            for (auto& sp : subpackets)
                res += sp.evaluation();
            break;
        case 1:
            res = 1;
            for (auto& sp : subpackets)
                res *= sp.evaluation();
            break;
        case 2:
            res = -1;
            for (auto& sp : subpackets)
                res = std::min(res, sp.evaluation());
            break;
        case 3:
            res = 0;
            for (auto& sp : subpackets)
                res = std::max(res, sp.evaluation());
            break;
        case 4:
            res = literal;
            break;
        case 5:
            res = (subpackets[0].evaluation() > subpackets[1].evaluation());
            break;
        case 6:
            res = (subpackets[0].evaluation() < subpackets[1].evaluation());
            break;
        case 7:
            res = (subpackets[0].evaluation() == subpackets[1].evaluation());
            break;
        
        default:
            break;
        }

        return res;
    }
};

Packet ParsePacket(std::string const &bs, int &pos)
{
    Packet packet;

    if (pos >= bs.size() - 6) return packet;

    packet.version_num = str2bin(bs, pos, 3);
    packet.type_id = str2bin(bs, pos, 3);

    if (packet.type_id == 4) // literal value
    {
        std::string value = "";
        bool is_last = true;
        while (true)
        {
            is_last = bs[pos++] == '0' ? true : false;
            value.append(bs.substr(pos, 4));
            pos += 4;
            if (is_last)
                break;
        }
        //std::cout << value << std::endl;
        if (!value.empty())
            packet.literal = std::stoull(value, nullptr, 2);  // notice: out of int range error
    }
    else // operator
    {
        int length_id = bs[pos++] == '0' ? 0 : 1;

        if (length_id == 0) // 15 bits
        {
            int length_of_subpackets = str2bin(bs, pos, 15);
            int end_pos = pos + length_of_subpackets;
            while (pos < end_pos && end_pos < bs.size())
            {
                auto sp = ParsePacket(bs, pos);
                packet.subpackets.push_back(sp);
            }
        }
        else // 11 bits
        {
            int num_of_subpackets = str2bin(bs, pos, 11);
            for (auto i = 0; i < num_of_subpackets; i++)
            {
                auto sp = ParsePacket(bs, pos);
                packet.subpackets.push_back(sp);
            }
        }
    }

    //std::cout << packet.literal << std::endl;

    return packet;
}

int PartOne(char const* input)
{
    std::ifstream ifs(input);
    std::string bs;
    read_all(ifs, bs);
    //std::cout << bs << std::endl;
    int pos = 0;
    int sum = 0;
    ParseOnePacket(bs, pos, sum);

    return sum;
}

unsigned long long PartTwo(char const* input)
{
    std::ifstream ifs(input);
    std::string bs;
    read_all(ifs, bs);
    int pos = 0;
    auto root_packet = ParsePacket(bs, pos);
    //std::cout << root_packet.total_version() << std::endl;

    return root_packet.evaluation();
}

int main(int argc, char* argv[])
{
    std::cout << "what do you get if you add up the version numbers in all packets?\n"
              << PartOne(argv[1]) << std::endl;

    std::cout << "What do you get if you evaluate the expression represented by your hexadecimal-encoded BITS transmission?\n"
              << PartTwo(argv[1]) << std::endl;

    return 0;
}