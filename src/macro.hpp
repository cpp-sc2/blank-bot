#pragma once
#include <sc2api/sc2_api.h>

#include <iostream>
#include <unordered_set>
#include <vector>

#include "probe.hpp"
#include "vespene.hpp"
#include "armyunit.hpp"
//#include "jps.hpp"
//#include "grid.hpp"
#include "tools.hpp"
//#include "constants.h"

using namespace sc2;

#define ACTION_CHECK_DT 10
#define PROBE_CHECK_DT 30
#define PROBE_CHECK_DACTION 3

struct MacroAction {
    static int globalIndex;
    UnitTypeID unit_type;
    AbilityID ability;
    Point2D pos;
    int index;
    uint32_t lastChecked;

    float dist_cache;
    Tag unit_cache;

    MacroAction(UnitTypeID unit_type_, AbilityID ability_, Point2D pos_)
        : unit_type(unit_type_),
          ability(ability_),
          pos(pos_),
          index(globalIndex),
          lastChecked(0),
          dist_cache(-1),
          unit_cache(NullTag) {
        globalIndex ++;
    }

    MacroAction(UnitTypeID unit_type_, AbilityID ability_, Point2D pos_, int index)
        : unit_type(unit_type_),
          ability(ability_),
          pos(pos_),
          index(index),
          lastChecked(0),
          dist_cache(-1),
          unit_cache(NullTag) {
        globalIndex++;
    }

    Cost cost(Agent *agent) {
        return Aux::abilityToCost(ability, agent);
    }

    operator Building() const {
        return Building{ability, pos};
    }
};

namespace Macro {
    map<UnitTypeID, vector<MacroAction>> actions;
    int lastChecked = 0;
    string diagnostics = "";

    void addAction(UnitTypeID unit_type_, AbilityID ability_, Point2D pos_ = Point2D{0,0}) {
        if (actions.find(unit_type_) == actions.end()) {
            actions[unit_type_] = vector<MacroAction>();
        }
        actions[unit_type_].push_back(MacroAction(unit_type_, ability_, pos_));
    }

    void addActionTop(UnitTypeID unit_type_, AbilityID ability_, Point2D pos_, int index_) {
        if (actions.find(unit_type_) == actions.end()) {
            actions[unit_type_] = vector<MacroAction>();
        }
        actions[unit_type_].insert(actions[unit_type_].begin(), MacroAction(unit_type_, ability_, pos_, index_));
    }

    void addProbe() {
        addAction(UNIT_TYPEID::PROTOSS_NEXUS, ABILITY_ID::TRAIN_PROBE, {0, 0});
    }

    void addBuilding(AbilityID ability_, Point2D pos_) {
        addAction(UNIT_TYPEID::PROTOSS_PROBE, ability_, pos_);
    }

    void addBuildingTop(AbilityID ability_, Point2D pos_, int index_) {
        addActionTop(UNIT_TYPEID::PROTOSS_PROBE, ability_, pos_, index_);
    }

    void execute(Agent *agent) {
        DebugText(agent,diagnostics, Point2D(0.03, 0.01), Color(100, 190, 215), 8);
        uint32_t gt = agent->Observation()->GetGameLoop();
        if (gt < lastChecked + ACTION_CHECK_DT) {
            return;
        }
        lastChecked = gt;
        Profiler macroProfiler("macroExecute");
        macroProfiler.disable();
        vector<MacroAction> topActions = vector<MacroAction>();
        for (auto it = actions.begin(); it != actions.end(); it++) {
            //vector<MacroAction> acts = it->second;
            if (it->second.size() == 0 /* || gt < lastChecked + ACTION_CHECK_DT*/) {
                continue;
            }
            MacroAction currentAction = it->second.front();
            //printf("%s %d\n", AbilityTypeToName(currentAction.ability), currentAction.index);
            if (currentAction.unit_type == UNIT_TYPEID::PROTOSS_GATEWAY &&
                agent->Observation()->GetWarpGateCount() > 0) {
                it->second.front().unit_type = UNIT_TYPEID::PROTOSS_WARPGATE;
                //it->second.front().pos = {-1, -1};
                if (currentAction.ability == ABILITY_ID::TRAIN_ZEALOT) {
                    it->second.front().ability = ABILITY_ID::TRAINWARP_ZEALOT;
                } else if (currentAction.ability == ABILITY_ID::TRAIN_STALKER) {
                    it->second.front().ability = ABILITY_ID::TRAINWARP_STALKER;
                } else if (currentAction.ability == ABILITY_ID::TRAIN_ADEPT) {
                    it->second.front().ability = ABILITY_ID::TRAINWARP_ADEPT;
                } else if (currentAction.ability == ABILITY_ID::TRAIN_SENTRY) {
                    it->second.front().ability = ABILITY_ID::TRAINWARP_SENTRY;
                } else if (currentAction.ability == ABILITY_ID::TRAIN_DARKTEMPLAR) {
                    it->second.front().ability = ABILITY_ID::TRAINWARP_DARKTEMPLAR;
                } else if (currentAction.ability == ABILITY_ID::TRAIN_HIGHTEMPLAR) {
                    it->second.front().ability = ABILITY_ID::TRAINWARP_HIGHTEMPLAR;
                } else {
                    printf("DELETED WARPGATE ACTION\n");
                    //it->second.erase(it->second.begin());
                    continue;
                }
            }
            currentAction = it->second.front();
            //lastChecked = gt;
            bool inserted = false;
            if (topActions.size() == 0) {
                inserted = true;
                topActions.push_back(currentAction);
                //printf("%s %d\n", AbilityTypeToName(currentAction.ability), currentAction.index);
            }
            for (auto it2 = topActions.begin(); it2 != topActions.end(); it2++) {
                if (it2->index > currentAction.index) {
                    topActions.insert(it2, currentAction);
                    inserted = true;
                    break;
                }
            }
            if (inserted == false) {
                topActions.push_back(currentAction);
            }
        }
        macroProfiler.midLog("TopSetup");
        diagnostics = "";
        for (int iAction = 0; iAction < topActions.size(); iAction++) {
            macroProfiler.subScope();
            MacroAction topAct = topActions[iAction];
            Units units = agent->Observation()->GetUnits(sc2::Unit::Alliance::Self, 
                [topAct](const Unit &unit) -> bool { 
                //return (unit.unit_type == topAct.unit_type) &&
                //       ((unit.unit_type == UNIT_TYPEID::PROTOSS_PROBE) || unit.orders.size() == 0);
                return (unit.unit_type == topAct.unit_type) && unit.build_progress == 1.0 && 
                    ((unit.unit_type == UNIT_TYPEID::PROTOSS_PROBE &&
                    (unit.orders.size() == 0 || unit.orders.front().ability_id == ABILITY_ID::HARVEST_RETURN)) ||
                    unit.orders.size() == 0);
            });

            diagnostics += strprintf("%s %s: ", UnitTypeToName(topAct.unit_type), AbilityTypeToName(topAct.ability));   
            Point2D diag = Point2D(0.01, 0.01 + 0.02 * iAction);

            //UnitTypes allData = agent->Observation()->GetUnitTypeData();

            UnitTypeData unit_stats = Aux::getStats(topAct.unit_type, agent); //allData.at(static_cast<uint32_t>(topAct.unit_type));

            auto pylons = UnitManager::get(UNIT_TYPEID::PROTOSS_PYLON);
            
            //printf("R:%d C:%d U:%d\n", unit_stats)

            UnitTypeID prerequisite = UNIT_TYPEID::INVALID;

            macroProfiler.midLog("RandomSetup");

            if (Aux::buildAbilityToUnit(topAct.ability) != UNIT_TYPEID::INVALID) {
                //UnitTypeData ability_stats = allData.at(static_cast<uint32_t>());
                UnitTypeData ability_stats = Aux::getStats(Aux::buildAbilityToUnit(topAct.ability), agent);

                prerequisite = ability_stats.tech_requirement;

                int foodCap = agent->Observation()->GetFoodCap();
                int foodUsed = agent->Observation()->GetFoodUsed();

                if (ability_stats.food_required > foodCap - foodUsed) {
                    bool cont = false;
                    if (actions[UNIT_TYPEID::PROTOSS_PROBE].size() != 0 && actions [UNIT_TYPEID::PROTOSS_PROBE].front().ability == ABILITY_ID::BUILD_PYLON) {
                        actions[UNIT_TYPEID::PROTOSS_PROBE].front().index = 0;
                        diagnostics += "PYLON IN TRANSIT\n\n";
                        macroProfiler.midLog("PYLON IN TRANSIT");
                        continue;
                    }
                    for (int i = 0; i < pylons.size(); i++) {
                        if (agent->Observation()->GetUnit(pylons[i]->self)->build_progress != 1.0) {
                            cont = true;
                            break;
                        }
                    }
                    if (cont) {
                        diagnostics += "PYLON BUILDING\n\n";
                        macroProfiler.midLog("PYLON BUILDING");
                        continue;
                    }

                    addBuildingTop(ABILITY_ID::BUILD_PYLON, Point2D{-1, -1}, 0);
                    diagnostics += "PYLON REQUESTED\n\n";
                    macroProfiler.midLog("PYLON REQUESTED");
                    break;
                }
            }

            macroProfiler.midLog("CheckSupply");

            if (units.size() == 0) {
                diagnostics += "NO FREE UNITS\n\n";
                macroProfiler.midLog("NO FREE UNITS");
                continue;
            }

            macroProfiler.midLog("CheckAvailableUnits");

            if (topAct.pos == Point2D{-1, -1}) {
                if (topAct.unit_type == UNIT_TYPEID::PROTOSS_PROBE) {
                    if (topAct.ability == ABILITY_ID::BUILD_PYLON) {
                        Point2D p = getPylonLocation(agent);
                        if (p == Point2D{-1, -1}) {
                            diagnostics += "INVALID POSITION\n\n";
                            macroProfiler.midLog("INVALID POSITION");
                            continue;
                        }
                        topAct.pos = p;
                        actions[topAct.unit_type].front().pos = p;
                    } else if (topAct.ability == ABILITY_ID::BUILD_ASSIMILATOR) {
                        UnitWrappers vespenes = UnitManager::getVespene();
                        std::vector<float> dist;
                        for (int v = 0; v < vespenes.size(); v ++) {
                            
                            for (auto n : UnitManager::get(UNIT_TYPEID::PROTOSS_NEXUS)) {
                                float d = Distance2D(n->pos(agent), vespenes[v]->pos(agent));
                                if (d < 12 && ((Vespene*)(vespenes[v]))->taken == 0) {
                                    ((Vespene*)(vespenes[v]))->taken = 1;
                                    topAct.pos = ((Vespene*)(vespenes[v]))->pos(agent);
                                    actions[topAct.unit_type].front().pos = topAct.pos;
                                    break;
                                }
                            }
                            if (topAct.pos != Point2D{ -1, -1 }) {
                                break;
                            }

                            //if (((Vespene *)(vespenes[i]))->taken == 1) {
                            //    /*vespenes.erase(vespenes.begin() + i);
                            //    v--;
                            //    continue;*/
                            //    ((Vespene*)(vespenes[i]))->taken = 2;
                            //    topAct.pos = ((Vespene*)(vespenes[i]))->pos(agent);
                            //    actions[topAct.unit_type].front().pos = topAct.pos;
                            //}
                            //float nexdist = -1;
                            //for (auto n : UnitManager::get(UNIT_TYPEID::PROTOSS_NEXUS)) {
                            //    float d = Distance2D(n->pos(agent), vespenes[i]->pos(agent));
                            //    if (nexdist == -1 || d < nexdist) {
                            //        nexdist = d;
                            //    }
                            //}
                            //dist.push_back(nexdist);
                        }
                        if (topAct.pos == Point2D{ -1, -1 }) {
                            diagnostics += "NO VESPENE SLOT\n\n";
                            macroProfiler.midLog("NO VESPENE SLOT");
                            continue;
                        }
                    } else if (topAct.ability == ABILITY_ID::BUILD_NEXUS) {

                    } else {
                        if (Aux::requiresPylon(topAct.ability)) {
                            if (UnitManager::get(UNIT_TYPEID::PROTOSS_PYLON).size() == 0) {
                                diagnostics += "NO PYLONS EXIST 1\n\n";
                                macroProfiler.midLog("NO PYLONS EXIST 1");
                                continue;
                            }
                        }
                        Point2D p = getBuildingLocation(agent);
                        if (p == Point2D{-1, -1}) {
                            diagnostics += "INVALID POSITION\n\n";
                            macroProfiler.midLog("INVALID POSITION");
                            continue;
                        }
                        topAct.pos = p;
                        actions[topAct.unit_type].front().pos = p;
                    }
                } else {
                    diagnostics += strprintf("NO LOCATION\n\n");
                    macroProfiler.midLog("NO LOCATION");
                    continue;
                }

            }

            macroProfiler.midLog("GenerateBuildingLocation");

            if (topAct.unit_type == UNIT_TYPEID::PROTOSS_WARPGATE) {
                auto sources = agent->Observation()->GetPowerSources();
                int index = -1;
                for (int i = 0; i < sources.size(); i++) {
                    if (index == -1 || Distance2D(sources[i].position, topAct.pos) <
                                           Distance2D(sources[index].position, topAct.pos)) {
                        index = i;
                    }
                }
                if (index == -1) {
                    diagnostics += strprintf("NO PYLONS???\n\n");
                    macroProfiler.midLog("NO PYLONS???");
                    continue;
                }
                for (int ao = 0; ao < 200; ao++) {
                    float theta = ((float)std::rand()) * 2 * 3.1415926 / RAND_MAX;
                    float radius = ((float)std::rand()) * sources[index].radius / RAND_MAX;

                    float x = std::cos(theta) * radius;
                    float y = std::sin(theta) * radius;

                    Point2D p = sources[index].position + Point2D{x, y};

                    if (Aux::checkPlacementFull(p, 2, agent)) {
                        topAct.pos = p;
                        break;
                    }
                }
                if (topAct.pos == Point2D{-1, -1}) {
                    diagnostics += strprintf("NO WARP LOCATION FOUND\n\n");
                    macroProfiler.midLog("NO WARP LOCATION FOUND");
                    continue;
                }
            }

            macroProfiler.midLog("GenerateWarpgateLocation");

            Cost c = Aux::abilityToCost(topAct.ability, agent);
            int theoreticalMinerals = agent->Observation()->GetMinerals();
            int theoreticalVespene = agent->Observation()->GetVespene();

            auto probes = UnitManager::get(UNIT_TYPEID::PROTOSS_PROBE);
            for (int i = 0; i < probes.size(); i ++) {
                vector<Building> buildings = ((Probe *)probes[i])->buildings;
                for (int b = 0; b < buildings.size(); b++) {
                    Cost g = buildings[b].cost(agent);
                    theoreticalMinerals -= g.minerals;
                    theoreticalVespene -= g.vespene;
                }
            }

            macroProfiler.midLog("CalculateBuildingDebt");

            //if (theoreticalMinerals < c.minerals - 30 && theoreticalVespene < c.vespene - 30) {
            //    break;
            //}

            const Unit *actionUnit = nullptr;
            

            //UnitTypeID prerequisite = unit_stats.tech_requirement;

            if (topAct.unit_type == UNIT_TYPEID::PROTOSS_PROBE && topAct.ability != ABILITY_ID::MOVE_MOVE &&
                topAct.ability != ABILITY_ID::MOVE_MOVEPATROL) {
                macroProfiler.subScope();

                if (prerequisite != UNIT_TYPEID::INVALID && UnitManager::get(prerequisite).size() == 0) {
                    addBuildingTop(Aux::unitToBuildAbility(prerequisite), Point2D{-1, -1}, topAct.index);
                    diagnostics += strprintf("PREREQUISITE REQUIRED: %s\n\n", UnitTypeToName(prerequisite));
                    macroProfiler.midLog("PREREQUISITE REQUIRED");
                    continue;
                }

                Units viablePylons = Units();

                if (Aux::requiresPylon(topAct.ability)) {
                    if (UnitManager::get(UNIT_TYPEID::PROTOSS_PYLON).size() == 0) {
                        diagnostics += "NO PYLONS EXIST 2\n\n";
                        macroProfiler.midLog("NO PYLONS EXIST 2");
                        continue;
                    }
                    //auto pylons = UnitManager::get(UNIT_TYPEID::PROTOSS_PYLON);
                    bool foundPylon = false;
                    for (int i = 0; i < pylons.size(); i++) {
                        const Unit *pylon = agent->Observation()->GetUnit(pylons[i]->self);
                        if (Distance2D(pylon->pos, topAct.pos) < Aux::PYLON_RADIUS) {
                            viablePylons.push_back(pylon);
                            foundPylon = true;
                            if (pylon->build_progress == 1.0) {
                                break;
                            }
                        }
                    }
                    if (!foundPylon) {
                        diagnostics += "NO PYLON IN VICINITY\n\n";
                        macroProfiler.midLog("NO PYLON IN VICINITY");
                        continue;
                    }
                }
                macroProfiler.midLog("PylonCheck1");

                //float dt = 0;
                float mindist = actions[UNIT_TYPEID::PROTOSS_PROBE].front().dist_cache;
                actionUnit = agent->Observation()->GetUnit(actions[UNIT_TYPEID::PROTOSS_PROBE].front().unit_cache);

                const Unit *uni = units[std::rand() % units.size()];

                Point2DI start = P2D(uni->pos);
                Point2DI goal = topAct.pos;

                float dist = 0;
                if (topAct.ability == ABILITY_ID::BUILD_ASSIMILATOR) {
                    dist = Distance2D(P2D(start), P2D(goal));
                } else {
                    auto came_from = jps(gridmap, start, goal, Tool::euclidean, agent);
                    vector<Location> pat = Tool::reconstruct_path(start, goal, came_from);
                    dist = fullDist(pat);
                }

                if (mindist == -1 || dist < mindist || actionUnit == nullptr) {
                    mindist = dist;
                    actionUnit = uni;
                    actions[UNIT_TYPEID::PROTOSS_PROBE].front().dist_cache = dist;
                    actions[UNIT_TYPEID::PROTOSS_PROBE].front().unit_cache = uni->tag;
                }

                float dt = (mindist - 2) / (unit_stats.movement_speed * timeSpeed);

                if (mindist == -1)
                    dt = 0;

                //float mindist = -1;

                //for (const Unit *uni : units) {
                //    Point2DI start = uni->pos;
                //    Point2DI goal = topAct.pos;

                //    float dist = 0;
                //    if (topAct.ability == ABILITY_ID::BUILD_ASSIMILATOR) {
                //        dist = Distance2D(P2D(start), P2D(goal));
                //    } else {
                //        auto came_from = jps(gridmap, start, goal, Tool::euclidean, agent);
                //        vector<Location> pat = Tool::reconstruct_path(start, goal, came_from);
                //        dist = fullDist(pat);
                //    }

                //    // printf("%.2f %.2f\n", agent->Query()->PathingDistance(uni, topAct.pos), dist);

                //    if (mindist == -1 || dist < mindist) {
                //        mindist = dist;
                //        actionUnit = uni;
                //    }
                //}

                //// float dt = agent->Query()->PathingDistance(actionUnit, topAct.pos) / (unit_stats.movement_speed *
                //// timeSpeed);
                //dt = (mindist - 2) / (unit_stats.movement_speed * timeSpeed);

                //if (mindist == -1)
                //    dt = 0;
                
                macroProfiler.midLog("ProbeDistanceCheck");
                
                if (Aux::requiresPylon(topAct.ability) && viablePylons.back()->build_progress != 1.0) {
                    //UnitTypeData pylon_stats = allData.at(static_cast<uint32_t>(UNIT_TYPEID::PROTOSS_PYLON));
                    UnitTypeData pylon_stats = Aux::getStats(UNIT_TYPEID::PROTOSS_PYLON, agent);

                    bool found = false;
                    for (int i = 0; i < viablePylons.size(); i++) {
                        if (((1.0 - viablePylons[i]->build_progress) * pylon_stats.build_time / fps) < dt) {
                            found = true;
                            break;
                        }
                    }
                    if (found == false) {
                        diagnostics += "NO ACTIVE PYLON IN VICINITY\n\n";
                        macroProfiler.midLog("NO ACTIVE PYLON IN VICINITY");
                        continue;
                    }
                }

                macroProfiler.midLog("PylonCheck2");

                if (prerequisite != UNIT_TYPEID::INVALID) {
                    //UnitTypeData prereq_stats = allData.at(static_cast<uint32_t>(prerequisite));
                    UnitTypeData prereq_stats = Aux::getStats(prerequisite, agent);

                    auto prereqs = UnitManager::get(prerequisite);
                    bool found = false;
                    for (int i = 0; i < prereqs.size(); i++) {
                        const Unit *prereq = agent->Observation()->GetUnit(prereqs[i]->self);
                        if (prereq->build_progress == 1.0 || ((1.0 - prereq->build_progress) * prereq_stats.build_time / fps) < dt) {
                            found = true;
                            break;
                        }
                    }
                    if (found == false) {
                        diagnostics += "PREQUISITE NOT READY\n\n";
                        macroProfiler.midLog("PREQUISITE NOT READY");
                        continue;
                    }
                }

                macroProfiler.midLog("PrereqCheck");

                //printf("%.2f %.2f\n", agent->Query()->PathingDistance(actionUnit, topAct.pos), mindist);

                int numMineralMiners = 0, numVespeneMiners = 0;
                for (int i = 0; i < probes.size(); i ++) {
                    const Unit* target = agent->Observation()->GetUnit(((Probe *)probes[i])->getTargetTag(agent));
                    if (target == nullptr)
                        continue;
                    if (Aux::isMineral(*target)) {
                        numMineralMiners++;
                    } else if (Aux::isAssimilator(*target)) {
                        numVespeneMiners++;
                    }
                }

                theoreticalMinerals += (dt * Aux::MINERALS_PER_PROBE_PER_SEC * numMineralMiners);
                theoreticalVespene += (dt * Aux::VESPENE_PER_PROBE_PER_SEC * numVespeneMiners);
                macroProfiler.midLog("TheoryResources");
            } else {
                macroProfiler.subScope();

                if (theoreticalMinerals < int(c.minerals) || theoreticalVespene < int(c.vespene)) {
                    diagnostics += "NOT ENOUGH RESOURCES\n\n";
                    macroProfiler.midLog("NOT ENOUGH RESOURCES");
                    break;
                }

                macroProfiler.midLog("CheckResources");

                auto abil = agent->Query()->GetAbilitiesForUnits(units);
                for (int i = 0; i < units.size(); i++) {
                    for (int a = 0; a < abil[i].abilities.size(); a ++) {
                        if (abil[i].abilities[a].ability_id == topAct.ability) {
                            actionUnit = units[i];
                        }
                    }
                }

                macroProfiler.midLog("CheckAbilities");

                if (actionUnit == nullptr) {
                    diagnostics += "NO UNIT WITH RELEVANT ABILITY";
                    //diagnostics += strprintf(" %d/%d %d/%d\n\n", theoreticalMinerals, int(c.minerals),
                    //                         theoreticalVespene, int(c.vespene));
                    macroProfiler.midLog("NO UNIT WITH RELEVANT ABILITY");
                    continue;
                }

                if (prerequisite != UNIT_TYPEID::INVALID) {
                    //UnitTypeData prereq_stats = allData.at(static_cast<uint32_t>(prerequisite));
                    UnitTypeData prereq_stats = Aux::getStats(prerequisite, agent);

                    auto prereqs = UnitManager::get(prerequisite);
                    bool found = false;
                    for (int i = 0; i < prereqs.size(); i++) {
                        const Unit *prereq = agent->Observation()->GetUnit(prereqs[i]->self);
                        if (prereq->build_progress == 1.0) {
                            found = true;
                            break;
                        }
                    }
                    if (found == false) {
                        diagnostics += "PREQUISITE NOT READY\n\n";
                        macroProfiler.midLog("PREQUISITE NOT READY");
                        continue;
                    }
                }
                macroProfiler.midLog("PrereqCheck");
            }

            //printf("M:%d V:%d | %s %llx %s M:%d/%d V:%d/%d S:%d/%d\n", agent->Observation()->GetMinerals(),
            //       agent->Observation()->GetVespene(),
            //       UnitTypeToName(topAct.unit_type), actionUnit->tag,
            //       AbilityTypeToName(topAct.ability), theoreticalMinerals,
            //       int(c.minerals), theoreticalVespene, int(c.vespene), agent->Observation()->GetFoodUsed(), c.psi);
            macroProfiler.subScope();
            if (theoreticalMinerals >= int(c.minerals) && theoreticalVespene >= int(c.vespene)) {
                if (topAct.pos != Point2D{0, 0}) {
                    if (topAct.pos == Point2D{-1, -1}) {
                        diagnostics += "POS NOT DEFINED EARLIER\n\n";
                        macroProfiler.midLog("POS NOT DEFINED EARLIER");
                        continue;
                    } else {
                        if (topAct.unit_type == UNIT_TYPEID::PROTOSS_PROBE) {
                            for (UnitWrapper *probe : probes) {
                                if (probe->self == actionUnit->tag) {
                                    ((Probe *)probe)->addBuilding(topAct);
                                    break;
                                }
                            }
                        } else {
                            agent->Actions()->UnitCommand(actionUnit, topAct.ability, topAct.pos);
                        }
                    }
                } else {
                    agent->Actions()->UnitCommand(actionUnit, topAct.ability);
                }
                if (topAct.unit_type == UNIT_TYPEID::PROTOSS_WARPGATE) {
                    actions[UNIT_TYPEID::PROTOSS_GATEWAY].erase(actions[UNIT_TYPEID::PROTOSS_GATEWAY].begin());
                } else {
                    actions[topAct.unit_type].erase(actions[topAct.unit_type].begin());
                }
                
                diagnostics += "SUCCESS\n\n";
                macroProfiler.midLog("SUCCESS");
                break;
            }
            diagnostics += "NOT ENOUGH RESOURCES\n\n";
            macroProfiler.midLog("NOT ENOUGH RESOURCES");
            break;
        }
    }
}

int MacroAction::globalIndex = 0;