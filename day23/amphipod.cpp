//
// Created by Harold on 2021/12/23.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <array>
#include <queue>

#include <unordered_map>

//#define ROOMSIZE 2
#define ROOMSIZE 4

namespace std
{
    template<typename T, size_t N>
    struct hash<array<T, N> >
    {
        typedef array<T, N> argument_type;
        typedef size_t result_type;

        result_type operator()(const argument_type& a) const
        {
            hash<T> hasher;
            result_type h = 0;
            for (result_type i = 0; i < N; ++i)
            {
                h = h * 31 + hasher(a[i]);
            }
            return h;
        }
    };
}

inline void hash_combine(std::size_t& seed) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest&&... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    hash_combine(seed, rest...);
}

enum class Amphipods : char
{
    E = 0,  // Empty

    A,      // Amber
    B,      // Bronze
    C,      // Copper
    D       // Desert
};

namespace std {
  template <> struct hash<Amphipods> {
    char operator() (const Amphipods &t) const { return char(t); }
  };
}

size_t get_energy(Amphipods ap)
{
    switch (ap)
    {
    case Amphipods::A:
        return 1;
    case Amphipods::B:
        return 10;
    case Amphipods::C:
        return 100;
    case Amphipods::D:
        return 1000;
    default:
        throw;
    }
}

// door on hallway
int get_room_pos(Amphipods ap)
{
    switch (ap)
    {
    case Amphipods::A:
        return 2;
    case Amphipods::B:
        return 4;
    case Amphipods::C:
        return 6;
    case Amphipods::D:
        return 8;
    default:
        throw;
    }
}

struct State {
    std::array<Amphipods, 11> hallway = {};
    std::array<Amphipods, ROOMSIZE> room_a;
    std::array<Amphipods, ROOMSIZE> room_b;
    std::array<Amphipods, ROOMSIZE> room_c;
    std::array<Amphipods, ROOMSIZE> room_d;

    bool operator==(State const& other) const
    {
        return hallway == other.hallway && 
               room_a == other.room_a &&
               room_b == other.room_b && 
               room_c == other.room_c &&
               room_d == other.room_d;
    }

    struct Hasher
    {
        size_t operator()(State const& state) const
        {
            std::size_t ret = 0;
            hash_combine(ret, state.hallway, state.room_a, state.room_b, state.room_c, state.room_d);
            return ret;
        }
    };

    bool is_target_room_free(Amphipods from_type, int& free_pos) const
    {
        auto const& target_room = get_target_room(from_type);
        // room occupied
        if (target_room[0] != Amphipods::E)
            return false;
        for (int pos = 0; pos < ROOMSIZE; pos++)
        {
            if (target_room[pos] == Amphipods::E)
                continue;
            // diff type in inner room
            for (int inner_pos = pos; inner_pos < ROOMSIZE; inner_pos++)
                if (target_room[inner_pos] != from_type)
                    return false;
            free_pos = pos - 1;
            return true;
        }
        // most inner room
        free_pos = ROOMSIZE - 1;
        return true;
    }

    bool is_movable_from_room(Amphipods from_type, int& movable_pos) const
    {
        auto const& from_room = get_target_room(from_type);
        for (int pos = 0; pos < ROOMSIZE; pos++)
        {
            // room free, try deeper
            if (from_room[pos] == Amphipods::E) continue;
            // room is occupied by mismatch type
            if (from_room[pos] != from_type)
            {
                movable_pos = pos;
                return true;
            }
            // check inner room
            for (int inner_pos = pos + 1; inner_pos < ROOMSIZE; inner_pos++)
                // find mismatch to move
                if (from_room[inner_pos] != from_type)
                {
                    movable_pos = pos;
                    return true;
                }
            return false;
        }
        return false;
    }

    std::array<Amphipods, ROOMSIZE> const& get_target_room(Amphipods ap) const
    {
        switch (ap)
        {
        case Amphipods::A:
            return room_a;
        case Amphipods::B:
            return room_b;
        case Amphipods::C:
            return room_c;
        case Amphipods::D:
            return room_d;
        default:
            throw;
        }
    }

    std::array<Amphipods, ROOMSIZE>& get_target_room(Amphipods ap)
    {
        switch (ap)
        {
        case Amphipods::A:
            return room_a;
        case Amphipods::B:
            return room_b;
        case Amphipods::C:
            return room_c;
        case Amphipods::D:
            return room_d;
        default:
            throw;
        }
    }
};

bool is_done(State const& state)
{
    for (auto t : state.room_a)
        if (t != Amphipods::A) return false;
    for (auto t : state.room_b)
        if (t != Amphipods::B) return false;
    for (auto t : state.room_c)
        if (t != Amphipods::C) return false;
    for (auto t : state.room_d)
        if (t != Amphipods::D) return false;  
    return true;
}

bool is_stuck(std::array<Amphipods, 11> const& hallway, int from, int to)
{
    if (from < to)
    {
        for (int pos = from + 1; pos <= to; pos++)
            if (hallway[pos] != Amphipods::E)
                return true;
    }
    else
    {
        for (int pos = from - 1; pos >= to; pos--)
            if (hallway[pos] != Amphipods::E)
                return true;
    }
    return false;
}

struct Move
{
    State state;
    size_t cost;

    Move() = default;
    Move(State state_, size_t cost_) : state(std::move(state_)), cost(cost_) { }

    bool operator > (Move const& other) const
    {
        return cost > other.cost;
    }
};

// move with cost
std::vector<Move> PossibleMovesAndCosts(State const& state)
{
    std::vector<Move> moves;

    // hallway -> room
    for (int from = 0; from < 11; from++)
    {
        auto from_type = state.hallway[from];
        if (from_type == Amphipods::E) continue;
        auto to = get_room_pos(from_type);
        // path stuck
        if (is_stuck(state.hallway, from, to)) continue;
        int hallway_path_length = std::abs(from - to);
        int free_pos = -1;
        // not movable
        if (!state.is_target_room_free(from_type, free_pos)) continue;
        // move result
        State done_state = state;
        done_state.hallway[from] = Amphipods::E;
        done_state.get_target_room(from_type)[free_pos] = from_type;
        size_t cost = (hallway_path_length + 1 + free_pos) * get_energy(from_type);
        moves.emplace_back(std::move(done_state), cost);
    }

    // room -> hallway
    for (const auto type : { Amphipods::A, Amphipods::B, Amphipods::C, Amphipods::D })
    {
        int initial_room_pos = -1;
        // not movable
        if (!state.is_movable_from_room(type, initial_room_pos)) continue;
        // move result
        auto const& from_room = state.get_target_room(type);
        auto from = get_room_pos(type);
        for (int to = from; to >= 0; to--)
        {
            // stuck
            if (state.hallway[to] != Amphipods::E) break;
            // can NOT block the room door
            if (to == 2 || to == 4 || to == 6 || to == 8) continue;
            State done_state = state;
            done_state.get_target_room(type)[initial_room_pos] = Amphipods::E;
            done_state.hallway[to] = from_room[initial_room_pos];
            size_t cost = (initial_room_pos + 1 + from - to) * get_energy(from_room[initial_room_pos]);
            moves.emplace_back(std::move(done_state), cost);
        }
        for (int to = from; to < 11; to++)
        {
            if (state.hallway[to] != Amphipods::E) break;
            if (to == 2 || to == 4 || to == 6 || to == 8) continue;
            State done_state = state;
            done_state.get_target_room(type)[initial_room_pos] = Amphipods::E;
            done_state.hallway[to] = from_room[initial_room_pos];
            size_t cost = (initial_room_pos + 1 + to - from) * get_energy(from_room[initial_room_pos]);
            moves.emplace_back(std::move(done_state), cost);
        }
    }

    return moves;
}

Amphipods char_2_amphipods(char c)
{
    switch (c)
    {
    case 'A':
        return Amphipods::A;
    case 'B':
        return Amphipods::B;
    case 'C':
        return Amphipods::C;
    case 'D':
        return Amphipods::D;
    case '.':
        return Amphipods::E;
    default:
        throw;
    }
}

char amphipods_2_char(Amphipods ap)
{
    switch (ap)
    {
    case Amphipods::A:
        return 'A';
    case Amphipods::B:
        return 'B';
    case Amphipods::C:
        return 'C';
    case Amphipods::D:
        return 'D';
    case Amphipods::E:
        return 'E';
    default:
        throw;
    }
}

void read_all(std::ifstream& ifs, State& state)
{
    std::string line;
    std::vector<std::string> board;
    while(std::getline(ifs, line))
        board.push_back(line);
#if ROOMSIZE == 2
    state.room_a = { char_2_amphipods(board[2][3]), char_2_amphipods(board[3][3]) };
    state.room_b = { char_2_amphipods(board[2][5]), char_2_amphipods(board[3][5]) };
    state.room_c = { char_2_amphipods(board[2][7]), char_2_amphipods(board[3][7]) };
    state.room_d = { char_2_amphipods(board[2][9]), char_2_amphipods(board[3][9]) };
#elif ROOMSIZE == 4
    state.room_a = { char_2_amphipods(board[2][3]), Amphipods::D, Amphipods::D, char_2_amphipods(board[3][3]) };
    state.room_b = { char_2_amphipods(board[2][5]), Amphipods::C, Amphipods::B, char_2_amphipods(board[3][5]) };
    state.room_c = { char_2_amphipods(board[2][7]), Amphipods::B, Amphipods::A, char_2_amphipods(board[3][7]) };
    state.room_d = { char_2_amphipods(board[2][9]), Amphipods::A, Amphipods::C, char_2_amphipods(board[3][9]) };
#endif
}

std::ostream& operator<<(std::ostream& os, State const& state)
{
    os << "{\n\thallway: ";
    for (auto c : state.hallway)
        os << amphipods_2_char(c);
    os << "\n\troom_a: ";
    os << amphipods_2_char(state.room_a[0]) << amphipods_2_char(state.room_a[1]);
    os << "\n\troom_b: ";
    os << amphipods_2_char(state.room_b[0]) << amphipods_2_char(state.room_b[1]);
    os << "\n\troom_c: ";
    os << amphipods_2_char(state.room_c[0]) << amphipods_2_char(state.room_c[1]);
    os << "\n\troom_d: ";
    os << amphipods_2_char(state.room_d[0]) << amphipods_2_char(state.room_d[1]);
    os << "\n}\n";
    return os;
}

size_t find_least_energy(State const& init_state)
{
    std::unordered_map<State, size_t, State::Hasher> visited;
    std::priority_queue<Move, std::vector<Move>, std::greater<Move>> path;
    path.push({init_state, 0});
    while (!path.empty())
    {
        auto p = path.top();
        path.pop();
        //std::cout << p.cost << std::endl;

        if (is_done(p.state)) return p.cost;

        if (visited.find(p.state) != visited.end())
            if (visited[p.state] < p.cost) continue;

        auto moves = PossibleMovesAndCosts(p.state);
        for (auto const& m : moves)
        {
            size_t new_cost = p.cost + m.cost;
            size_t old_cost = std::numeric_limits<size_t>::max();
            if (visited.find(m.state) != visited.end())
                old_cost = visited[m.state];
            if (old_cost > new_cost)
            {
                visited[m.state] = new_cost;
                path.push({m.state, new_cost});
            }
        }
    }
    return 0;
}

size_t PartOne(char const* input)
{
    std::ifstream ifs(input);
    State state;
    read_all(ifs, state);
    //std::cout << state << std::endl;

    return find_least_energy(state);
}

int main(int argc, char* argv[])
{
    std::cout << "What is the least energy required to organize the amphipods?\n"
              << PartOne(argv[1]) << std::endl;

    return 0;
}