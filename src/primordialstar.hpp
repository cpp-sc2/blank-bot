#pragma once
#include <sc2api/sc2_api.h>
#include <sc2api/sc2_common.h>

#include <map>
#include "constants.h"
#include "debugging.hpp"

using namespace sc2;
using namespace std;

class StarNode {
public:
	int pathNode;

	//distance already travelled
	float g;

	//optimistic distance to travel
	float h;

	StarNode(int pathNode, float g, float h) : pathNode(pathNode), g(g), h(h) {

	}

	//bool operator<(const StarNode& b) const {
	//	return	(g + h) < (b.g + b.h);
	//}

	operator float() {
		return g + h;
	}
};

namespace PrimordialStar {

	enum Cardinal {
		INVALID,
		UP,
		UP_RT,
		RT,
		DN_RT,
		DN,
		DN_LT,
		LT,
		UP_LT
	};

	Point2D cardinalToDirection(Cardinal &cardinal) {
		switch (cardinal) {
			case(UP): {
				return { 0,1 };
			}
			case(UP_RT): {
				return { 1,1 };
			}
			case(RT): {
				return { 1,0 };
			}
			case(DN_RT): {
				return { 1,-1 };
			}
			case(DN): {
				return { 0,-1 };
			}
			case(DN_LT): {
				return { -1,-1 };
			}
			case(LT): {
				return { -1,0 };
			}
			case(UP_LT): {
				return { -1,1 };
			}
			default: {
				return { 0,0 };
			}
		}
	}

	class PathNode {
	private:
		Point2D pos;
	public:

		Cardinal wall;
		int id;
		vector<int> connected;

		PathNode(Point2D pos, Cardinal wall);

		Point2D rawPos() {
			return pos;
		}

		Point2D position(float radius) {
			if (wall == INVALID) {
				return pos;
			}
			return pos - normalize(cardinalToDirection(wall)) * radius;
		}
	};

	vector<PathNode*> basePathNodes;
	map2d<int8_t>* blobGrid;

	PathNode::PathNode(Point2D pos, Cardinal wall) : pos(pos), wall(wall) {
		id = basePathNodes.size();
		basePathNodes.push_back(this);
	}

	//Point2D findNextWall(Point2D start, Point2D direction) {
	//	Point2D dir = normalize(direction);
	//	if (dir.y == 0) {

	//	}
	//	return { 0,0 };
	//}

	//DDA https://en.wikipedia.org/wiki/Digital_differential_analyzer_(graphics_algorithm)
	bool checkLinearPath(Point2D start, Point2D end, Agent* agent) {
		float dx = end.x - start.x;
		float dy = end.y - start.y;
		float step = 0;

		if (abs(dx) >= abs(dy)) {
			step = abs(dx);
		}
		else {
			step = abs(dy);
		}

		dx /= step;
		dy /= step;

		Point2D operating = start;
		for (int i = 0; i < step; i++) {
			if (!Aux::checkPathable(int(operating.x), int(operating.y), agent)) {
				return false;
			}
			operating += {dx, dy};
		}
		return true;
	}

	void calculateConnection(PathNode* p, Agent* agent) {
		for (int i = 0; i < basePathNodes.size(); i++) {
			if (i == p->id || find(p->connected.begin(), p->connected.end(), i) != p->connected.end()) {
				continue;
			}
			PrimordialStar::PathNode* node = PrimordialStar::basePathNodes[i];
			if (checkLinearPath(p->rawPos(), node->rawPos(), agent)) {
				p->connected.push_back(i);
				node->connected.push_back(p->id);
			}
		}
	}

	void generateBlobGrid() {

	}

	constexpr float displacementMinute = 0.1F;

//#define BASIC(i, j) pathables[i + 2][j + 2]
//#define BASIC_UP(i, j) BASIC(i,j+1)
//#define BASIC_UP_RT(i, j) BASIC(i+1,j+1)
//#define BASIC_RT(i, j) BASIC(i+1,j)
//#define BASIC_DN_RT(i, j) BASIC(i+1,j-1)
//#define BASIC_DN(i, j) BASIC(i,j-1)
//#define BASIC_DN_LT(i, j) BASIC(i-1,j-1)
//#define BASIC_LT(i, j) BASIC(i-1,j)
//#define BASIC_UP_LT(i, j) BASIC(i-1,j+1)

	bool check_UP_RT(int i, int j) {
		bool up = Aux::checkPathable(i, j + 1, nullptr);
		bool up_rt = Aux::checkPathable(i + 1, j + 1, nullptr);
		bool rt = Aux::checkPathable(i + 1, j, nullptr);
		return up && up_rt && rt;
	}

	bool check_DN_RT(int i, int j) {
		bool rt = Aux::checkPathable(i + 1, j, nullptr);
		bool dn_rt = Aux::checkPathable(i + 1, j - 1, nullptr);
		bool dn = Aux::checkPathable(i, j - 1, nullptr);
		return dn && dn_rt && rt;
	}

	bool check_DN_LT(int i, int j) {
		bool dn = Aux::checkPathable(i, j - 1, nullptr);
		bool dn_lt = Aux::checkPathable(i - 1, j - 1, nullptr);
		bool lt = Aux::checkPathable(i - 1, j, nullptr);
		return dn && dn_lt && lt;
	}

	bool check_UP_LT(int i, int j) {
		bool lt = Aux::checkPathable(i - 1, j, nullptr);
		bool up_lt = Aux::checkPathable(i - 1, j + 1, nullptr);
		bool up = Aux::checkPathable(i, j + 1, nullptr);
		return up && up_lt && lt;
	}

	void generatePathNodes(Agent *agent) {
		int mapWidth = agent->Observation()->GetGameInfo().width;
		int mapHeight = agent->Observation()->GetGameInfo().height;
		for (int i = 1; i < mapWidth - 1; i++) {
			for (int j = 1; j < mapHeight - 1; j++) {
				bool center = Aux::checkPathable(i, j, agent);
				if (!center) {
					//bool up =		Aux::checkPathable(i	, j + 1	, agent);
					//bool up_rt =	Aux::checkPathable(i + 1, j + 1	, agent);
					//bool rt =		Aux::checkPathable(i + 1, j		, agent);
					//bool dn_rt =	Aux::checkPathable(i + 1, j - 1	, agent);
					//bool dn =		Aux::checkPathable(i	, j - 1	, agent);
					//bool dn_lt =	Aux::checkPathable(i - 1, j - 1	, agent);
					//bool lt =		Aux::checkPathable(i - 1, j		, agent);
					//bool up_lt =	Aux::checkPathable(i - 1, j + 1	, agent);

					//bool basic_up_rt = up && up_rt && rt;
					//bool basic_up_lt = dn && dn_rt && rt;
					//bool basic_dn_rt = dn && dn_lt && lt;
					//bool basic_dn_lt = up && up_lt && lt;

					//if (basic_up_rt) {
					//	new PathNode(Point2D{ (float)(i + 1 + displacementMinute), (float)(j + 1 + displacementMinute) }, DN_LT);
					//}
					//if (basic_up_lt) {
					//	new PathNode(Point2D{ (float)(i + 1 + displacementMinute), (float)(j - displacementMinute) }, UP_LT);
					//}
					//if (basic_dn_rt) {
					//	new PathNode(Point2D{ (float)(i - displacementMinute), (float)(j - displacementMinute) }, UP_RT);
					//}
					//if (basic_dn_lt) {
					//	new PathNode(Point2D{ (float)(i - displacementMinute), (float)(j + 1 + displacementMinute) }, DN_RT);
					//}


					//bool basic_up_rt = up && up_rt && rt;
					//bool basic_dn_rt = dn && dn_rt && rt;
					//bool basic_dn_lt = dn && dn_lt && lt;
					//bool basic_up_lt = up && up_lt && lt;

					//bool basic_up_rt = up && up_rt && rt;
					//bool basic_dn_rt = dn && dn_rt && rt;
					//bool basic_dn_lt = dn && dn_lt && lt;
					//bool basic_up_lt = up && up_lt && lt;

					if (check_UP_RT(i, j) && ((Aux::checkPathable(i - 1, j + 1, agent) || !check_UP_RT(i - 1, j + 1)) || (Aux::checkPathable(i + 1, j - 1, agent) || !check_UP_RT(i + 1, j - 1)))) {
						new PathNode(Point2D{ (float)(i + 1 + displacementMinute), (float)(j + 1 + displacementMinute) }, DN_LT);
					}
					if (check_DN_RT(i, j) && ((Aux::checkPathable(i - 1, j - 1, agent) || !check_DN_RT(i - 1, j - 1)) || (Aux::checkPathable(i + 1, j + 1, agent) || !check_DN_RT(i + 1, j + 1)))) {
						new PathNode(Point2D{ (float)(i + 1 + displacementMinute), (float)(j - displacementMinute) }, UP_LT);
					}
					if (check_DN_LT(i, j) && ((Aux::checkPathable(i - 1, j + 1, agent) || !check_DN_LT(i - 1, j + 1)) || (Aux::checkPathable(i + 1, j - 1, agent) || !check_DN_LT(i + 1, j - 1)))) {
						new PathNode(Point2D{ (float)(i - displacementMinute), (float)(j - displacementMinute) }, UP_RT);
					}
					if (check_UP_LT(i, j) && ((Aux::checkPathable(i - 1, j - 1, agent) || !check_UP_LT(i - 1, j - 1)) || (Aux::checkPathable(i + 1, j + 1, agent) || !check_UP_LT(i + 1, j + 1)))) {
						new PathNode(Point2D{ (float)(i - displacementMinute), (float)(j + 1 + displacementMinute) }, DN_RT);
					}
				}
			}
		}
		for (int i = 0; i < basePathNodes.size(); i++) {
			calculateConnection(basePathNodes[i], agent);
		}
	}

	vector<Point2D> getPath(Point2D start, Point2D end, float radius, Agent* agent) {
		PathNode* startNode = new PathNode(start, INVALID);
		PathNode* operatingNode = startNode;
		PathNode* endNode = new PathNode(end, INVALID);

		calculateConnection(startNode, agent);
		calculateConnection(endNode, agent);

		bool* visited = new bool[basePathNodes.size()];
		//visited.reserve(basePathNodes.size());
		//for (int i = 0; i < basePathNodes.size(); i++) {
		//	visited[i] = false;
		//}
		memset(visited, 0, basePathNodes.size() * sizeof(bool));

		vector<Point2D> points;
		map<int, int> backpath;

		if (startNode->connected.size() == 0 || endNode->connected.size() == 0) {

		}
		else {
			std::priority_queue<StarNode, vector<StarNode>, greater<float>> starNodes;
			starNodes.push(StarNode(operatingNode->id, 0, Distance2D(start, end)));
			visited[operatingNode->id] = true;
			bool found = false;
			for (int cycles = 0; cycles < 10000; cycles++) {
				if (starNodes.size() == 0) {
					break;
				}
				StarNode star = starNodes.top();
				starNodes.pop();
				operatingNode = basePathNodes[star.pathNode];
				Point2D currentPos = operatingNode->position(radius);
				for (int i = 0; i < operatingNode->connected.size(); i++) {
					int subNodeID = operatingNode->connected[i];
					if (visited[subNodeID]) {
						continue;
					}
					Point2D nextPos = basePathNodes[subNodeID]->position(radius);
					backpath[subNodeID] = operatingNode->id;
					starNodes.push(StarNode(subNodeID, star.g + Distance2D(currentPos, nextPos), Distance2D(nextPos, end)));
					visited[subNodeID] = true;

					//DebugText(agent, strprintf("%.1f,%.1f", star.g + Distance2D(currentPos, nextPos), Distance2D(nextPos, end)), AP3D(nextPos));

					if (operatingNode->connected[i] == endNode->id) {
						found = true;
						break;
					}
				}
				if (found) {
					break;
				}
			}

			operatingNode = endNode;
			points.push_back(basePathNodes[operatingNode->id]->rawPos());
			for (int i = 0; i < starNodes.size(); i++) {
				operatingNode = basePathNodes[backpath[operatingNode->id]];
				points.insert(points.begin(), basePathNodes[operatingNode->id]->position(radius));
				if (operatingNode->id == startNode->id) {
					break;
				}
			}
		}
		delete[] visited;
		basePathNodes.pop_back();
		basePathNodes.pop_back();
		
		//printf("SOMETHING WENT WRONG\n");
		return points;
	}
}