#pragma once
#include <sc2api/sc2_api.h>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <cmath>
#include "constants.h"

using namespace sc2;

struct Location
{
	int x, y;
	Location direction() const;

	Location(int x_, int y_) {
        x = x_;
        y = y_;
    }

	Location() {
        x = -1;
        y = -1;
    }

	Location(Point2DI point) {
        x = point.x;
        y = point.y;
	}

	operator Point2DI() const {
        return Point2DI(x, y);    
	}

	operator Point2D() const {
		return Point2D(x, y);
    }
};

bool operator==(const Location& a, const Location& b) noexcept;
bool operator!=(const Location& a, const Location& b) noexcept;
bool operator<(const Location& a, const Location& b) noexcept;
Location operator+(const Location& a, const Location& b) noexcept;
Location operator-(const Location& a, const Location& b) noexcept;
Location operator-(const Location& a) noexcept;
Location operator*(const int a, const Location& b) noexcept;
Location operator*(const Location& a, const int b) noexcept;
std::ostream& operator<<(std::ostream& os, const Location& a);

const Location NoneLoc {-1, -1};

/* implement hash function so we can put Location into an unordered_set */
namespace std
{
	template <> struct hash<Location>
	{
		typedef Location argument_type;
		typedef std::size_t result_type;
		std::size_t operator()(const Location& id) const noexcept
		{
			return std::hash<int>()(id.x ^ (id.y << 4));
		}
	};
}

class Grid
{
private:
	int width, height;

public:
	//std::unordered_set<Location> walls;

	Grid() {};
	Grid(int width_, int height_/*, std::unordered_set<Location> walls_*/) : width(width_), height(height_)/*, walls(walls_)*/ {};

	int get_width() const {return width;};
	int get_heigth() const {return height;};

	bool in_bounds(const Location& loc) const noexcept { return 0 <= loc.x && loc.x < width && 0 <= loc.y && loc.y < height; };
    bool passable(const Location& loc, Agent *agent) const {
                    return Aux::checkPathable(loc.x, loc.y, agent);
    };
	bool valid_move(const Location& loc, const Location& dir, Agent *agent) const;
	bool forced(const Location& loc, const Location& parent, const Location& travel_dir) const;

	std::vector<Location> neighbours(const Location& current, const std::vector<Location>& dirs, Agent* agent) const;
    std::vector<Location> pruned_neighbours(const Location& current, const Location& parent, Agent* agent) const;
};

using namespace std;

std::vector<Point2DI> fullPath(std::vector<Location> path) noexcept {
    std::vector<Point2DI> pathout;
    pathout.push_back(path[0]);
    for (int i = 1; i < path.size(); i++) {
        for (int j = 0; j < 300; j++) {
            int x_d = 0;
            int y_d = 0;
            if (path[i].x > pathout.back().x) {
                x_d = 1;
            } else if (path[i].x < pathout.back().x) {
                x_d = -1;
			}
            if (path[i].y > pathout.back().y) {
                y_d = 1;
            } else if (path[i].y < pathout.back().y) {
                y_d = -1;
            }
            pathout.push_back(pathout.back() + Point2DI(x_d, y_d));
		}
	}
    return pathout;
}

float fullDist(std::vector<Location> path) noexcept {
    float dist = 0;
    if (path.size() == 0)
        return -1;
    //printf("PS:%d\n", path.size());
    for (int i = 0; i < path.size() - 1; i++) {
        int dx = path[i + 1].x - path[i].x;
        int dy = path[i + 1].y - path[i].y;
        dist += sqrt(dx * dx + dy * dy);
        //printf("[%d,%d %d,%d %.1f]\n", path[i].x, path[i].y, path[i + 1].x, path[i + 1].y, dist);
    }
    return dist;
}

const static std::vector<Location> ALL_DIRS {
	/* East, West, North, South */
	/* NE, NW, SE, SW*/
	{1, 0}, {-1, 0},
	{0, -1}, {0, 1},
	{1, 1}, {-1, 1},
	{1, -1}, {-1, -1}
};

Location Location::direction() const
{
	return Location{x>0 ? 1 : (x<0 ? -1 : 0), y>0 ? 1 : (y<0 ? -1 : 0)};
}

bool operator==(const Location& a, const Location& b) noexcept
{
	return a.x == b.x && a.y == b.y;
}

bool operator!=(const Location& a, const Location& b) noexcept
{
	return a.x != b.x || a.y != b.y;
}

bool operator<(const Location& a, const Location& b) noexcept
{
	return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}

Location operator+(const Location& a, const Location& b) noexcept
{
	return {a.x + b.x, a.y + b.y};
}

Location operator-(const Location& a, const Location& b) noexcept
{
	return {a.x - b.x, a.y - b.y};
}

Location operator-(const Location& a) noexcept
{
	return {-a.x, -a.y};
}

Location operator*(const int a, const Location& b) noexcept
{
	return {a * b.x, a * b.y};
}

Location operator*(const Location& a, const int b) noexcept
{
	return {b * a.x, b * a.y};
}

std::ostream& operator<<(std::ostream& os, const Location& a)
{
	return os << "<Location {" << a.x << ", " << a.y << "}>";
}

bool Grid::forced(const Location& loc, const Location& parent, const Location& travel_dir) const{
	const Location dir {(loc - parent).direction()};
	// Diagonal move into diagonal check
	if(travel_dir.x != 0 && travel_dir.y != 0){
		if((dir.x == travel_dir.x && dir.y == -travel_dir.y) || 
	   		(dir.x == -travel_dir.x && dir.y == travel_dir.y)){
			return true;
		}
	}
	// Horizontal or vertical move into diagonal check
	else if(dir.x != 0 && dir.y != 0){
		return true;
	}
	return false;
}

bool Grid::valid_move(const Location& loc, const Location& dir, Agent* agent) const {
	const auto next_loc = loc + dir;
	if(dir.x != 0 && dir.y != 0){
		return in_bounds(next_loc) && passable(next_loc, agent) &&
                       (passable(loc + Location{dir.x, 0}, agent) || passable(loc + Location{0, dir.y}, agent));
	}
        return in_bounds(next_loc) && passable(next_loc, agent);
};

vector<Location> Grid::neighbours(const Location& current, const std::vector<Location>& dirs, Agent* agent) const {
	vector<Location> results;
	for(auto& dir : dirs){
		if (valid_move(current, dir, agent)) {
			results.push_back(current + dir);
		}
	}
	return results;
}

vector<Location> Grid::pruned_neighbours(const Location& current, const Location& parent, Agent *agent) const
{
	if(parent == NoneLoc){
		return neighbours(current, ALL_DIRS, agent);
	}
	vector<Location> current_neighbours;
	const auto dir {(current - parent).direction()};
	// Diagonal neighbour
	if(dir.x != 0 && dir.y != 0){
		const Location dir_x {dir.x, 0};
		const Location dir_y {0, dir.y};

		// Add natural neighbours
        current_neighbours = neighbours(current, {dir, dir_x, dir_y}, agent);

		// Add forced neighbours
		for(const auto& candidate_dir : {dir_x, dir_y}){
			const auto previous = current - dir;
            if (!valid_move(previous, candidate_dir, agent) &&
                            valid_move(previous, 2 * candidate_dir, agent)) {
 				current_neighbours.push_back(previous + 2 * candidate_dir);
			}
		}
	}
	// Horizontal or vertical neighbour
	else
	{
		// Add natural neighbours
        current_neighbours = neighbours(current, {dir}, agent);

		// Add forced neighbours
		const Location inverted_dir {dir.y, dir.x};
        if (!valid_move(current, inverted_dir, agent) && valid_move(current, inverted_dir + dir, agent)) {
			current_neighbours.push_back(current + inverted_dir + dir);
		}
        if (!valid_move(current, -inverted_dir, agent) && valid_move(current, -inverted_dir + dir, agent)) {
			current_neighbours.push_back(current - inverted_dir + dir);
		}
	}
	return current_neighbours;
}

Grid gridmap;