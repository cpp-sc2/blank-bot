#pragma once
#include <sc2api/sc2_api.h>
#include <map>
#include "unit.hpp"
#include "constants.h"

map<Tag, int8_t> probeTargetting;

Point2D getBuildingLocation(Agent *agent) {
    if (Aux::buildingPointer >= Aux::buildingLocations.size()) {
        GameInfo game_info = agent->Observation()->GetGameInfo();
        auto pylons = UnitManager::get(UNIT_TYPEID::PROTOSS_PYLON);
        Point2D diffuse[] = {{2.5, 0.5}, {-0.5, 2.5}, {-2.5, -0.5}, {0.5, -2.5}};
        bool found = false;
        for (int i = 0; i < UnitManager::get(UNIT_TYPEID::PROTOSS_PYLON).size(); i++) {
            for (int d = 0; d < 4; d++) {
                Point2D p = pylons[i]->pos(agent) + diffuse[d];
                if (Aux::checkPlacementFull(p, 3, agent)) {
                    Aux::buildingLocations.push_back(p);
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        if (!found) {
            for (int i = 0; i < 500; i++) {
                float theta = ((float)std::rand()) * 2 * 3.1415926 / RAND_MAX;
                float radius = ((float)std::rand()) * Aux::PYLON_RADIUS / RAND_MAX;

                auto pylons = UnitManager::get(UNIT_TYPEID::PROTOSS_PYLON);

                float x = std::cos(theta) * radius;
                float y = std::sin(theta) * radius;

                Point2D p = pylons[std::rand() % pylons.size()]->pos(agent) + Point2D{x, y};

                if (Aux::checkPlacementFull(p, 3, agent)) {
                    Aux::buildingLocations.push_back(p);
                    break;
                } else if (i == 499) {
                    return {-1, -1};
                }
            }
        }
    }
    Aux::addPlacement(Aux::buildingLocations[Aux::buildingPointer], 3);
    return Aux::buildingLocations[Aux::buildingPointer++];
}

Point2D getPylonLocation(Agent *agent) {
    if (Aux::pylonPointer >= Aux::pylonLocations.size()) {
        GameInfo game_info = agent->Observation()->GetGameInfo();
        auto pylons = UnitManager::get(UNIT_TYPEID::PROTOSS_PYLON);
        Point2D diffuse[] = { {6, -7}, {-7, -6}, {-6, 7}, {7, 6} };
        bool found = false;
        for (int i = 0; i < UnitManager::get(UNIT_TYPEID::PROTOSS_PYLON).size(); i++) {
            for (int d = 0; d < 4; d++) {
                Point2D p = pylons[i]->pos(agent) + diffuse[d];
                if (Aux::checkPlacementFull(p, 2, agent)) {
                    Aux::pylonLocations.push_back(p);
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        if (!found) {
            for (int i = 0; i < 5000; i++) {
                float x = ((float)std::rand()) * game_info.width / RAND_MAX;
                float y = ((float)std::rand()) * game_info.height / RAND_MAX;

                Point2D p{ x, y };

                if ((i > 1000 || (imRef(Aux::influenceMap, (int)x, (int)y) != 0)) &&
                    Aux::checkPlacementFull(p, 2, agent)) {
                    Aux::pylonLocations.push_back(p);
                    break;
                }
                else if (i == 4999) {
                    return p;
                }
            }
        }
    }
    Aux::addPlacement(Aux::pylonLocations[Aux::pylonPointer], 2);
    return Aux::pylonLocations[Aux::pylonPointer++];
}

class Probe : public UnitWrapper {
private:
    Tag target;

public:
    vector<Building> buildings;

    //Probe(Tag self_) : UnitWrapper(self_, UNIT_TYPEID::PROTOSS_PROBE) {
    //    target = 0;
    //}

    Probe(const Unit* unit) : UnitWrapper(unit) {
        target = 0;
    }

    bool addBuilding(Building building) {
        buildings.push_back(building);
        return true;
    }

    Tag getTargetTag(Agent *agent) {
        if (agent->Observation()->GetUnit(target) == nullptr) {
            target = NullTag;
        }
        if (target == NullTag) {
            Units minerals = agent->Observation()->GetUnits(Unit::Alliance::Neutral, Aux::isMineral);
            Units assimilators = agent->Observation()->GetUnits(Unit::Alliance::Self, Aux::isAssimilator);
            minerals.insert(minerals.end(), assimilators.begin(), assimilators.end());

            const Unit *nearest = nullptr;
            for (const Unit *targ : minerals) {
                if (targ->display_type != Unit::DisplayType::Visible) {
                    continue;
                }
                if (probeTargetting.find(targ->tag) == probeTargetting.end()) {
                    probeTargetting[targ->tag] = 0;
                }
                int limit = 0;
                if (Aux::isMineral(*targ)) {
                    limit = 2;
                } else if (Aux::isAssimilator(*targ)) {
                    limit = 3;
                } else {
                    printf("ERROR\n");
                    continue;
                }
                if (probeTargetting[targ->tag] >= limit)
                    continue;
                if (nearest == nullptr || Distance2D(nearest->pos, targ->pos) >
                                            Distance2D(agent->Observation()->GetUnit(self)->pos, targ->pos)) {
                    nearest = targ;
                }
            }
            
            if (nearest == nullptr) {
                return NullTag;
            }
            target = nearest->tag;
            probeTargetting[target] += 1;
        }
        return target;
    }

    bool execute(Agent *agent) {
        if (agent->Observation()->GetUnit(self) == nullptr) {
            return false;
        }
        if (buildings.size() == 0) {
            if (ignoreFrames == 0) {
                ignoreFrames--;
            } else {
                if (get(agent)->orders.size() == 0) {
                    agent->Actions()->UnitCommand(self, ABILITY_ID::HARVEST_GATHER, getTargetTag(agent));
                    if (checkAbility(ABILITY_ID::HARVEST_RETURN)) {
                        agent->Actions()->UnitCommand(self, ABILITY_ID::HARVEST_RETURN);
                    }
                } else {
                    if (get(agent)->orders[0].ability_id == ABILITY_ID::HARVEST_GATHER &&
                        get(agent)->orders[0].target_unit_tag != getTargetTag(agent)) {

                        //if (get(agent)->orders.size() != 0)
                        //    printf("%Ix compare %Ix\n", get(agent)->orders[0].target_unit_tag, target);
                        Tag harvest = getTargetTag(agent);
                        if (harvest == NullTag) {
                            printf("NOT FOUND ANY MINERALS");
                            ignoreFrames = 100;
                        } else {
                            agent->Actions()->UnitCommand(self, ABILITY_ID::HARVEST_GATHER, harvest);
                        }

                    }
                }
                //if (checkAbility(ABILITY_ID::HARVEST_RETURN) && get(agent)->orders.size() == 0 ||
                //    get(agent)->orders[0].ability_id == ABILITY_ID::HARVEST_RETURN) {
                //    agent->Actions()->UnitCommand(self, ABILITY_ID::HARVEST_RETURN);
                //} else if (get(agent)->orders.size() == 0 ||
                //           (get(agent)->orders[0].ability_id == ABILITY_ID::HARVEST_GATHER &&
                //            get(agent)->orders[0].target_unit_tag != getTargetTag(agent))) {
                //    if (get(agent)->orders.size() != 0)
                //        printf("%Ix compare %Ix\n", get(agent)->orders[0].target_unit_tag, target);
                //    Tag harvest = getTargetTag(agent);
                //    if (harvest == NullTag) {
                //        ignoreFrames = 100;
                //    } else {
                //        agent->Actions()->UnitCommand(self, ABILITY_ID::HARVEST_GATHER, getTargetTag(agent));
                //    }
                //}
            }
        } else {
            Building top = buildings[0];
            if (Distance2D(agent->Observation()->GetUnit(self)->pos, top.pos) < 2) {
                if (Aux::requiresPylon(top.build)) {
                    auto pylons = UnitManager::get(UNIT_TYPEID::PROTOSS_PYLON);
                    bool foundPylon = false;
                    for (int i = 0; i < pylons.size(); i++) {
                        const Unit *pylon = agent->Observation()->GetUnit(pylons[i]->self);
                        if (Distance2D(pylon->pos, top.pos) < Aux::PYLON_RADIUS) {
                            if (pylon->build_progress == 1.0) {
                                foundPylon = true;
                                break;
                            }
                        }
                    }
                    if (!foundPylon) {
                        return false;
                    }
                }
                Cost c = Aux::buildAbilityToCost(top.build, agent);
                if (c.minerals > agent->Observation()->GetMinerals() || c.vespene > agent->Observation()->GetVespene())
                    return false;
                if (top.build == ABILITY_ID::BUILD_ASSIMILATOR) {
                    auto vespene = UnitManager::getVespene();
                    for (int i = 0; i < vespene.size(); i++) {
                        //if (agent->Observation()->GetUnit(vespene[i]->self) == nullptr) {
                        //    continue;
                        //}
                        printf("TRY: %.1f,%.1f %.1f,%.1f\n", vespene[i]->pos(agent).x, vespene[i]->pos(agent).y,
                               pos(agent).x, pos(agent).y);
                        if (Distance2D(vespene[i]->pos(agent), pos(agent)) < 2) {
                            printf("%Ix %s %Ix\n", self, AbilityTypeToName(top.build), vespene[i]->self);
                            agent->Actions()->UnitCommand(self, top.build, vespene[i]->self);
                            break;
                        }
                    }
                    //agent->Actions()->UnitCommand(self, ABILITY_ID::HARVEST_RETURN);
                } else {
                    if (agent->Query()->Placement(top.build, top.pos)) {
                        printf("CAN PLACE %s %.1f,%.1f\n", AbilityTypeToName(top.build), top.pos.x, top.pos.y);
                        agent->Actions()->UnitCommand(self, top.build, top.pos);
                    } else {
                        return false;
                    }
                }
                buildings.erase(buildings.begin());
            } else {
                //agent->Actions()->UnitCommand(self, ABILITY_ID::MOVE_MOVE, top.pos);
                const Unit *prob = get(agent);
                if (prob->orders.size() == 0 || prob->orders.front().target_pos != top.pos) {
                    agent->Actions()->UnitCommand(self, ABILITY_ID::MOVE_MOVE, top.pos);
                }
            }
        }
        return true;
    }

    static void loadAbilities(Agent *agent) {
        Units u;
        vector<UnitWrapper *> probes = UnitManager::get(UNIT_TYPEID::PROTOSS_PROBE);
        //for (int i = 0; i < probes.size(); i++) {
        //    const Unit *unit = agent->Observation()->GetUnit(probes[i]->self);
        //    if (unit != nullptr) {
        //        u.push_back(unit);
        //    } else {
        //        probes.erase(probes.begin() + i);
        //        i --;
        //    }
        //}
        vector<AvailableAbilities> allAb = agent->Query()->GetAbilitiesForUnits(u);
        for (int i = 0; i < allAb.size(); i++) {
            if (probes[i]->self == allAb[i].unit_tag) {
                ((Probe *)probes[i])->abilities = allAb[i];
            } else {
                printf("ABILITY ASSIGNMENT ERROR\n");
            }
        }
    }
};