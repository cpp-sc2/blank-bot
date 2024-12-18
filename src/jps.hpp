#include <queue>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sc2api/sc2_api.h>

#include "grid.hpp"
#include "tools.hpp"


using namespace std;
using namespace sc2;

typedef pair<double, Location> PQElement;
typedef priority_queue<PQElement, vector<PQElement>, greater<PQElement>> PQLoc;

typedef double(heuristic_fn)(const Location&, const Location&);

Location jump(const Grid& grid, const Location initial, const Location dir, const Location goal, Agent *agent) {
    auto new_loc{initial + dir};
    if (!grid.valid_move(initial, dir, agent)) {
        return NoneLoc;
    }
    if (new_loc == goal) {
        return new_loc;
    }
    for (const auto next : grid.pruned_neighbours(new_loc, initial, agent)) {
        if (grid.forced(next, new_loc, dir)) {
            return new_loc;
        }
    }
    if (dir.x != 0 && dir.y != 0) {
        for (const auto& new_dir : {Location{dir.x, 0}, Location{0, dir.y}}) {
            auto jump_point{jump(grid, new_loc, new_dir, goal, agent)};
            if (jump_point != NoneLoc) {
                return new_loc;
            }
        }
    }
    return jump(grid, new_loc, dir, goal, agent);
}

vector<Location> successors(const Grid& grid, const Location& current, const Location& parent, const Location& goal, Agent *agent) {
    vector<Location> successors;
    auto neighbours{grid.pruned_neighbours(current, parent, agent)};
    for (const auto& n : neighbours) {
        auto jump_point{jump(grid, current, (n - current).direction(), goal, agent)};
        if (jump_point != NoneLoc) {
            successors.push_back(jump_point);
        }
    }
    return successors;
}

unordered_map<Location, Location> jps(const Grid& grid, const Point2DI& start_, const Point2DI& goal_,
                                      heuristic_fn heuristic, Agent* agent, float range = 0, Point2DI* end = nullptr) {
    PQLoc open_set;
    unordered_map<Location, Location> came_from{};
    unordered_map<Location, double> cost_so_far{};

    Location start(start_);
    Location goal(goal_);

    open_set.emplace(0, start);
    came_from[start] = start;
    cost_so_far[start] = 0;
    Location parent{NoneLoc};

    while (!open_set.empty()) {
        const auto current{open_set.top().second};
        if (current == goal || (range != 0 && Distance2D(current, goal) < range)) {
            if(end != nullptr)
                *end = current;
            break;
        }

        open_set.pop();
        if (current != start) {
            parent = came_from[current];
        }

        for (const auto& next : successors(grid, current, parent, goal, agent)) {
            const auto new_cost = cost_so_far[current] + heuristic(current, next);
            auto existing_cost = std::numeric_limits<double>::max();
            if (cost_so_far.count(next)) {
                existing_cost = cost_so_far.at(next);
            }
            if (cost_so_far.find(next) == cost_so_far.end() || new_cost < existing_cost) {
                cost_so_far[next] = new_cost;
                came_from[next] = current;
                open_set.emplace(new_cost + heuristic(next, goal), next);
            }
        }
    }
    return came_from;
}

//int main() {
//    unordered_set<Location> walls{{5, 0}, {5, 1}, {2, 2}, {5, 2}, {2, 3}, {5, 3}, {2, 4}, {5, 4},
//                                  {2, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {2, 6}, {2, 7}};
//    Grid map{10, 10, walls};
//
//    Location start{1, 1};
//    Location goal{6, 2};
//
//    auto came_from = jps(map, start, goal, Tool::euclidean);
//    auto path = Tool::reconstruct_path(start, goal, came_from);
//    Tool::draw_grid(map, {}, {}, path, came_from, start, goal);
//}

// int main()
// {
// 	unordered_set<Location> walls {
// 		{5, 0}, {5, 1}, {2, 2}, {5, 2},
// 		{2, 3}, {5, 3}, {2, 4}, {5, 4}, {2, 5}, {4, 5},
// 		{5, 5}, {6, 5}, {7, 5}, {2, 6}, {2, 7}};
// 	Grid map {10, 10, walls};

// 	Location start {1, 1};
// 	Location goal {6, 2};

// 	auto came_from = jps(map, start, goal, Tool::euclidean);
// 	auto path = Tool::reconstruct_path(start, goal, came_from);
// 	Tool::draw_grid(map, {}, {}, path, came_from, start, goal);
// }