#pragma once
#include <sc2api/sc2_api.h>

#include <map>
 
#include "constants.h"
#include "profiler.hpp"
#include "unit.hpp"
#include "spacialhashgrid.hpp"
#include "map2dFloat.hpp"
#include "unitpriority.hpp"

constexpr int BERTH = 1;

enum SquadMode {
    ATTACK,
    RETREAT,
    DEFEND,
    FULL_RETREAT
};

char *SquadModeToString(SquadMode mode) {
    if (mode == ATTACK) {
        return "ATTACK";
    } else if (mode == RETREAT) {
        return "RETREAT";
    } else if (mode == DEFEND) {
        return "DEFEND";
    } else if (mode == FULL_RETREAT) {
        return "FULL_RETREAT";
    }
    return "HUH?";
}

//map2dFloat<float>* enemyDamageNet;

struct DamageNet {
    Point2D pos;
    Weapon weapon;
};



class Squad {
public:    
    Point2D location;

    Point2D intermediateLoc;
    SquadMode mode;
    Composition comp;

    std::map<Tag, char> squadStates;

    float radius;
    int8_t ignoreFrames;

    std::vector<UnitWrapper *> army;

    Squad() {
        army = std::vector<UnitWrapper *>();
        radius = 0;
        comp = Composition::Invalid;
        squadStates = std::map<Tag, char>();
        ignoreFrames = 0;
    }

    bool has(UnitTypeID type) {
        for (UnitWrapper *u : army) {
            if (u->type == type) {
                return true;
            }
        }
        return false;
    }

    bool find(UnitTypeID tag) {
        for (UnitWrapper *u : army) {
            if (u->self == tag) {
                return true;
            }
        }
        return false;
    }

    void alignCenter(Agent *agent) {
        UnitWrapper *closest = nullptr;
        for (auto wrap : army) {
            if (closest == nullptr ||
                (Distance2D(location, wrap->pos(agent)) < Distance2D(location, closest->pos(agent)))) {
                closest = wrap;
            }
            //printf("dp %.1f, dc %.1f\n", Distance2D(location, wrap->pos(agent)),
            //       Distance2D(location, closest->pos(agent)));
        }
        if (closest != nullptr) {
            squadStates[closest->self] = 'n';
            //printf("ASSIGN %Ix TO CENTER\n", closest->self);
        }
    }

    Point2D center(Agent *agent) {
        Point2D center = {0, 0};
        if (army.size() == 0)
            return center;
        int cnt = 0;
        for (int i = 0; i < army.size(); i++) {
            if (army[i]->type == UNIT_TYPEID::PROTOSS_WARPPRISM || squadStates[army[i]->self] == 'o' ||
                squadStates[army[i]->self] == 'm') {
                continue;
            }
            center += army[i]->pos(agent);
            cnt++;
        }
        return (center / cnt);
    }

    bool withinRadius(Agent *agent) {
        float r = armyballRadius();
        Point2D cntr = center(agent);
        for (int i = 0; i < army.size(); i++) {
            if (army[i]->type == UNIT_TYPEID::PROTOSS_WARPPRISM || squadStates[army[i]->self] == 'o' ||
                squadStates[army[i]->self] == 'm') {
                continue;
            }
            if (Distance2D(army[i]->pos(agent), cntr) > r) {
                return false;
            }
        }
        return true;
    }

    //Point2D centerGND(Agent *agent) {
    //    Point2D center = {0, 0};
    //    if (army.size() == 0)
    //        return center;
    //    int cnt = 0;
    //    for (int i = 0; i < army.size(); i++) {
    //        if (army[i]->get(agent)->is_flying || squadStates[army[i]->self] == 'o') {
    //            continue;
    //        }
    //        center += army[i]->pos(agent);
    //        cnt++;
    //    }
    //    return (center / cnt);
    //}

    //bool withinRadiusGND(Agent *agent) {
    //    float r = armyballRadius();
    //    Point2D cntr = centerGND(agent);
    //    for (int i = 0; i < army.size(); i++) {
    //        if (army[i]->type == UNIT_TYPEID::PROTOSS_WARPPRISM || squadStates[army[i]->self] == 'o') {
    //            continue;
    //        }
    //        if (Distance2D(army[i]->pos(agent), cntr) > r) {
    //            return false;
    //        }
    //    }
    //    return true;
    //}
    
    int armyOnPoint(Agent *agent) {
        Point2D cntr = center(agent);
        float radius = armyballRadius();
        int cnt = 0;
        for (auto wrap : army) {
            if (Distance2D(cntr, wrap->pos(agent)) < armyballRadius()) {
                cnt++;
            }
        }
        return cnt;
    }

    Composition composition(Agent *agent) {
        if (comp != Composition::Invalid) {
            return comp;
        }
        bool air = false, gnd = false;
        for (int i = 0; i < army.size(); i++) {
            if (comp != Composition::Any) {
                singleUnitComp(army[i], agent);
            } else {
                break;
            }
        }
        return comp;
    }

    void singleUnitComp(UnitWrapper *unit, Agent *agent) {
        if (unit->get(agent)->is_flying) {
            if (comp == Composition::Invalid) {
                comp = Composition::Air;
                return;
            } else if (comp == Composition::Ground) {
                comp = Composition::Any;
                return;
            }
        } else {
            if (comp == Composition::Invalid) {
                comp = Composition::Ground;
                return;
            } else if (comp == Composition::Air) {
                comp = Composition::Any;
                return;
            }
        }
    }

    //float priorityAttack(UnitTypeID self_type, Weapon weapon, const Unit *opponent, Agent *agent) {  // HIGHER IS MORE DESIRABLE TO ATTACK
    //    //auto *padad = new Profiler("pA_o");
    //    //UnitTypes allData = Aux::allData(agent);
    //    //UnitTypeData myStats = allData.at(static_cast<uint32_t>(self_type));
    //    //UnitTypeData enemyStats = allData.at(static_cast<uint32_t>(opponent->unit_type));
    //    UnitTypeData myStats = Aux::getStats(self_type, agent);
    //    UnitTypeData enemyStats = Aux::getStats(opponent->unit_type, agent);
    //    Weapon strongestWeapon;
    //    float mostDmag = 0;
    //    //delete padad;
    //    //auto *pe = new Profiler("pA_eW");
    //    for (int i = 0; i < enemyStats.weapons.size(); i++) {
    //        if (Aux::hitsUnit(composition(agent), enemyStats.weapons[i].type)) {
    //            float damageE = enemyStats.weapons[i].damage_;
    //            for (int w = 0; w < enemyStats.weapons[i].damage_bonus.size(); w++) {
    //                for (int a = 0; a < myStats.attributes.size(); a++) {
    //                    if (enemyStats.weapons[i].damage_bonus[w].attribute == myStats.attributes[a]) {
    //                        damageE += enemyStats.weapons[i].damage_bonus[w].bonus;
    //                    }
    //                }
    //            }
    //            if ((mostDmag / strongestWeapon.speed) < (damageE / enemyStats.weapons[i].speed)) {
    //                strongestWeapon = enemyStats.weapons[i];
    //                mostDmag = damageE;
    //            }
    //        }
    //    }
    //    //delete pe;
    //    //auto pe2ads = Profiler("pA_mD");
    //    float damage = weapon.damage_;
    //    for (int w = 0; w < weapon.damage_bonus.size(); w++) {
    //        for (int a = 0; a < enemyStats.attributes.size(); a++) {
    //            if (weapon.damage_bonus[w].attribute == enemyStats.attributes[a]) {
    //                damage += weapon.damage_bonus[w].bonus;
    //            }
    //        }
    //    }
    //    float prioriT = (damage / weapon.speed) * (strongestWeapon.damage_ / strongestWeapon.speed) /
    //                    (opponent->shield + opponent->health);
    //    return prioriT;
    //}

    //float priorityAvoid(UnitTypeID self_type, UnitTypeID opponent_type, Weapon weapon,
    //                    Agent *agent) {  // HIGHER IS MORE DESIRABLE TO AVOID
    //    //UnitTypes allData = agent->Observation()->GetUnitTypeData();
    //    //UnitTypeData myStats = allData.at(static_cast<uint32_t>(self_type));
    //    //UnitTypeData enemyStats = allData.at(static_cast<uint32_t>(opponent->unit_type));
    //    return 1;
    //}

    float armyballRadius() {
        if (radius != 0)
            return radius;
        else {
            //printf("A:%d SQRT%.1f\n", army.size(), std::sqrt(army.size()));
            return std::sqrt(army.size())*2;
        }
    }

    bool execute(Agent *agent) {
        Point2D centr = center(agent);
        if (centr.x != centr.x || centr.y != centr.y) {
            alignCenter(agent);
        }

        for (auto wrap : army) {
            if (squadStates[wrap->self] == 'o' && Distance2D(wrap->pos(agent), center(agent)) < armyballRadius()) {
                squadStates[wrap->self] = 'n';
            }
        }

        //auto squadEx = Profiler("sE");
        if (mode == ATTACK) {
            if (ignoreFrames > 0) {
                ignoreFrames--;
                return false;
            }
            /*
            //Tags rmy = Tags();
            //for (auto wrap : army) {
            //    rmy.push_back(wrap->self);
            //}
            //agent->Actions()->UnitCommand(rmy, ABILITY_ID::ATTACK, location);
            //ignoreFrames = 10;
            
            UnitTypes allData = agent->Observation()->GetUnitTypeData();
            //Units enemies = agent->Observation()->GetUnits(Unit::Alliance::Enemy);
            //std::vector<DamageNet> enemiesnet;
            //for (int e = 0; e < enemies.size(); e++) {
            //    UnitTypeData enemy_stats = allData.at(static_cast<uint32_t>(enemies[e]->unit_type));
            //    if (enemy_stats.weapons.size() == 0) {
            //        continue;
            //    }
            //    for (int w = 0; w < enemy_stats.weapons.size(); w++) {
            //        enemiesnet.push_back({enemies[e]->pos, enemy_stats.weapons[w]});
            //    }
            //}
            Tags move;
            Tags join;
            //Tags all;
            //Tags atk;
            //Tag attackTarget;
            Targets attacking;
            Targets avoiding;
            map<Tag, Point2D> avoidingPos;
            int numArmyEngaged = 0;
            int numArmyEndangered = 0;
            UnitWrappers relevantEnemies;

            int numArmyInArea = 0;

            for (auto wrap : army) {
                //auto preaf = Profiler(strprintf("sE - perUnit %s", UnitTypeToName(wrap->type)));
                //auto preaf = Profiler("sE_pU");
                const Unit *wrapGet = wrap->get(agent);
                //if (squadStates.find(wrap->self) == squadStates.end()) {
                //    squadStates[wrap->self] = 'o';
                //}
                if (squadStates[wrap->self] == 'm') {
                    //join.push_back(wrap->self);
                    continue;
                }
                numArmyInArea += 1;

                if (squadStates[wrap->self] == 'n' && (wrapGet->shield / wrapGet->shield_max) < 0.05) {
                    squadStates[wrap->self] = 'r';
                } else if (squadStates[wrap->self] == 'r' && (wrapGet->shield/wrapGet->shield_max) > 0.95) {
                    squadStates[wrap->self] = 'n';
                }

                int berthRadius = BERTH;
                if (0 && squadStates[wrap->self] == 'r') {
                    berthRadius += 4;
                }

                //all.push_back(wrap->self);
                if (0) {
                    if (squadStates[wrap->self] == 'o') {
                        join.push_back(wrap->self);
                    } else {
                        move.push_back(wrap->self);
                    }
                } else {

                    UnitTypeData unit_stats = Aux::getStats(wrap->type, agent);  // allData.at(static_cast<uint32_t>(wrap->type));
                    UnitWrappers inRange = UnitWrappers();
                    std::vector<float> inRangePriority = std::vector<float>();
                    UnitWrappers dangerous = UnitWrappers();
                    std::vector<float> dangerousPriority = std::vector<float>();
                    int engaged = false;
                    //auto profileStuff = new Profiler("checkEnemies");
                    UnitWrappers enemi = SpacialHash::findInRadiusEnemy(wrap->pos(agent), 14, agent);
                    for (auto it2 = enemi.begin(); it2 != enemi.end(); it2++) {
                        if ((*it2)->pos(agent) == Point2D{0, 0}) {
                            continue;
                        }
                        const Unit *enemy = (*it2)->get(agent);
                        // auto parg = Profiler(strprintf("sE - perEnemy %s",
                        // UnitTypeToName(enemies[e]->unit_type)));
                        // auto parg = Profiler("sE_pE");
                        UnitTypeData enemy_stats = Aux::getStats(
                            (*it2)->type, agent);  // allData.at(static_cast<uint32_t>(enemies[e]->unit_type));
                        for (int i = 0; i < enemy_stats.weapons.size(); i++) {
                            // auto pre = Profiler("sE_pEW");
                            bool withinRange = ((Distance2D(wrapGet->pos, (*it2)->pos(agent)) - wrapGet->radius -
                                                 (*it2)->radius) < enemy_stats.weapons[i].range + berthRadius);
                            if (withinRange && Aux::hitsUnit(wrapGet, enemy_stats.weapons[i].type)) {
                                // inRange.push_back(enemies[e]);
                                bool inserted = false;
                                float priority = priorityAvoid(wrap->type, (*it2)->type, enemy_stats.weapons[i], agent);
                                if (dangerous.size() == 0) {
                                    inserted = true;
                                    dangerous.push_back(*it2);
                                    dangerousPriority.push_back(priority);
                                    // printf("%s %d\n", AbilityTypeToName(currentAction.ability),
                                    // currentAction.index);
                                }
                                for (int d = 0; d < dangerous.size(); d++) {
                                    if (dangerousPriority[d] < priority) {
                                        dangerous.insert(dangerous.begin() + d, *it2);
                                        dangerousPriority.insert(dangerousPriority.begin() + d, priority);
                                        inserted = true;
                                        break;
                                    }
                                }
                                if (inserted == false) {
                                    dangerous.push_back(*it2);
                                    dangerousPriority.push_back(priority);
                                }
                                break;
                            }
                        }
                        if (wrapGet->weapon_cooldown == 0 && enemy != nullptr) {
                            // auto pdd = Profiler("sE_aMW");
                            for (int i = 0; i < unit_stats.weapons.size(); i++) {
                                // auto pdd = Profiler("sE_pMW");
                                bool withinRange = ((Distance2D(wrapGet->pos, (*it2)->pos(agent)) - wrapGet->radius -
                                                     (*it2)->radius) < unit_stats.weapons[i].range);
                                if (withinRange && Aux::hitsUnit(enemy, unit_stats.weapons[i].type)) {
                                    // inRange.push_back(enemies[e]);
                                    // auto *pdd = new Profiler("sE_pMW_I");
                                    bool inserted = false;
                                    engaged = true;
                                    float priority = priorityAttack(wrap->type, unit_stats.weapons[i], enemy, agent);
                                    // delete pdd;
                                    // auto *pddads32 = new Profiler("sE_pMW_I1");
                                    if (inRange.size() == 0) {
                                        inserted = true;
                                        inRange.push_back(*it2);
                                        inRangePriority.push_back(priority);
                                        // printf("%s %d\n", AbilityTypeToName(currentAction.ability),
                                        // currentAction.index);
                                    }
                                    // delete pddads32;
                                    // auto *pddads33 = new Profiler("sE_pMW_I2");
                                    for (int d = 0; d < inRange.size(); d++) {
                                        if (inRangePriority[d] < priority) {
                                            inRange.insert(inRange.begin() + d, *it2);
                                            inRangePriority.insert(inRangePriority.begin() + d, priority);
                                            inserted = true;
                                            break;
                                        }
                                    }
                                    // delete pddads33;
                                    // auto *pddad4s = new Profiler("sE_pMW_I3");
                                    if (inserted == false) {
                                        inRange.push_back(*it2);
                                        inRangePriority.push_back(priority);
                                    }

                                    if (std::find(relevantEnemies.begin(), relevantEnemies.end(), *it2) ==
                                        relevantEnemies.end()) {
                                        relevantEnemies.push_back(*it2);
                                    }

                                    numArmyEngaged++;
                                    break;
                                }
                            }
                        }
                    }
                    //delete profileStuff;
                    if (engaged) {
                        numArmyEngaged++;
                    }
                    if (0 && (squadStates[wrap->self] == 'r' || !withinRadius(agent))) {
                        if (dangerous.size() != 0) {
                            if (avoiding.find(dangerous.front()->self) == avoiding.end()) {
                                avoiding[dangerous.front()->self] = Tags();
                                avoidingPos[dangerous.front()->self] = dangerous.front()->pos(agent);
                            }
                            avoiding[dangerous.front()->self].push_back(wrap->self);
                        } else if (inRange.size() != 0) {
                            if (attacking.find(inRange.front()->self) == attacking.end()) {
                                attacking[inRange.front()->self] = Tags();
                            }
                            attacking[inRange.front()->self].push_back(wrap->self);
                        } else {
                            if (squadStates[wrap->self] == 'o') {
                                join.push_back(wrap->self);
                            } else {
                                move.push_back(wrap->self);
                            }
                        }
                    } else {
                        if (inRange.size() != 0) {
                            if (attacking.find(inRange.front()->self) == attacking.end()) {
                                attacking[inRange.front()->self] = Tags();
                            }
                            attacking[inRange.front()->self].push_back(wrap->self);
                        } else if (dangerous.size() != 0) {
                            if (avoiding.find(dangerous.front()->self) == avoiding.end()) {
                                avoiding[dangerous.front()->self] = Tags();
                                avoidingPos[dangerous.front()->self] = dangerous.front()->pos(agent);
                            }
                            avoiding[dangerous.front()->self].push_back(wrap->self);
                        } else {
                            if (squadStates[wrap->self] == 'o') {
                                join.push_back(wrap->self);
                            } else {
                                move.push_back(wrap->self);
                            }
                        }
                    }
                    //for (int i = 0; i < enemiesnet.size(); i++) {
                    //    if (Aux::hitsUnit(wrap->type, enemiesnet[i].weapon.type)) {
                    //        
                    //    }
                    //}
                }
            }

            intermediateLoc = {0, 0};
            if (relevantEnemies.size() != 0) {
                for (auto unitWrap : relevantEnemies) {
                    intermediateLoc += unitWrap->pos(agent);
                    //printf("[%s][%.1f,%.1f]", UnitTypeToName(unitWrap->type), intermediateLoc.x, intermediateLoc.y);
                }
                intermediateLoc /= relevantEnemies.size();
            }
            //printf("[%.1f,%.1f]", intermediateLoc.x, intermediateLoc.y);

            if (numArmyEngaged < army.size() * 0.9 && intermediateLoc != Point2D{0,0}) {
                agent->Actions()->UnitCommand(move, ABILITY_ID::ATTACK, intermediateLoc);
            } else {
                agent->Actions()->UnitCommand(move, ABILITY_ID::ATTACK, location);
            }
            for (auto it = avoiding.begin(); it != avoiding.end(); it++) {
                Point3D avg = {0, 0, 0};
                for (int i = 0; i < it->second.size(); i++) {
                    avg += agent->Observation()->GetUnit(it->second[i])->pos;
                }
                avg /= (it->second.size());
                Point2D dir = normalize(avoidingPos[it->first] - avg) * 1;
                agent->Actions()->UnitCommand(it->second, ABILITY_ID::MOVE_MOVE, avg - dir);
                dir *= -1;
                agent->Debug()->DebugLineOut(avg + Point3D{0, 0, 1}, Aux::addKeepZ(avg, dir) + Point3D{0, 0, 1});
                //printf("AVOID %.1f,%.1f\n", -dir.x, -dir.y);
                
            }
            for (auto it = attacking.begin(); it != attacking.end(); it++) {
                agent->Actions()->UnitCommand(it->second, ABILITY_ID::ATTACK, it->first);
                
            }
            agent->Actions()->UnitCommand(join, ABILITY_ID::ATTACK, center(agent));
            */
            ignoreFrames = 3;
            return false;
        } else if (mode == RETREAT) {
            return false;
        } else if (mode == DEFEND) {
            return false;
        } else if (mode == FULL_RETREAT) {
            return false;
        }
        return false;
    }

    bool attack(Point2D location_) {
        mode = ATTACK;
        location = location_;
        return false;
    }

    bool retreat(Point2D location_) {
        mode = RETREAT;
        location = location_;
        return false;
    }

    bool defend(Point2D location_) {
        mode = DEFEND;
        location = location_;
        return false;
    }

    bool fullRetreat(Point2D location_) {
        mode = FULL_RETREAT;
        location = location_;
        return false;
    }
};

#define ENEMY_SQUAD_RADIUS 5

class EnemySquad {
public:
    UnitWrappers army;
    Point2D center;
    std::vector<UnitTypeID> unitComp;

    EnemySquad() : army(), center(){
        
    }

    void add(UnitWrapper *unitWrap, Agent *agent) {
        army.push_back(unitWrap);
        Point2D pos = unitWrap->pos(agent);
        if (army.size() == 1) {
            center = pos;
        } else {
            center += (center * army.size() + pos) / (army.size() + 1);
        }

        if (std::find(unitComp.begin(), unitComp.end(), unitWrap->type) == unitComp.end()) {
            unitComp.push_back(unitWrap->type);
        }
    }
};

using EnemySquads = std::vector<EnemySquad>;

std::vector<Squad> squads = std::vector<Squad>();

class ArmyUnit : public UnitWrapper {
private:
    
    UnitTypeData stats;
    bool is_flying;
    int8_t targetFrames = 0;

public:
    UnitWrapper* targetWrap;
    Squad* squad;

    ArmyUnit(const Unit* unit) : UnitWrapper(unit) {
        targetWrap = nullptr;
        if (squads.size() == 0) {
            squads.emplace_back();
        }
        squads[0].army.push_back(this);
        squad = &squads[0];
        is_flying = unit->is_flying; 
        squad->squadStates[self] = 'o';
    }

    UnitTypeData getStats(Agent *agent) {
        if (stats.unit_type_id == UNIT_TYPEID::INVALID) {
            stats = Aux::getStats(type,agent);
        }
        return stats;
    }

    bool withSquad(Agent* agent) {
        return Distance2D(pos(agent), squad->center(agent)) < squad->armyballRadius();
    }

    float priorityAttack(Weapon w, UnitWrapper* op, Agent *agent) {  // HIGHER IS MORE DESIRABLE TO ATTACK
        if (Army::hitsUnit(w.type, Army::unitTypeTargetComposition(op->type))) {
            return Army::Priority(type, op->type); //include health
        }
        return -1;
    }

    float priorityAvoid(UnitTypeID self_type, UnitTypeID opponent_type, Weapon weapon,
        Agent* agent) {  // HIGHER IS MORE DESIRABLE TO AVOID
        //UnitTypes allData = agent->Observation()->GetUnitTypeData();
        //UnitTypeData myStats = allData.at(static_cast<uint32_t>(self_type));
        //UnitTypeData enemyStats = allData.at(static_cast<uint32_t>(opponent->unit_type));
        return 1;
    }

    virtual bool execute(Agent *agent) {
        if (squad->mode == ATTACK) {
            return executeAttack(agent);
        } else if (squad->mode == RETREAT) {
            return executeRetreat(agent);
        } else if (squad->mode == DEFEND) {
            return executeDefend(agent);
        } else if (squad->mode == FULL_RETREAT) {
            return executeFullRetreat(agent);
        }
        return false;
    }

    virtual bool executeAttack(Agent *agent) {
        constexpr float extraRadius = 2.0F;
        if (ignoreFrames > 0) {
            if (targetWrap != nullptr && UnitManager::findEnemy(targetWrap->type, targetWrap->self) == nullptr) {
                targetFrames = 0;
                targetWrap = nullptr;
                ignoreFrames = 0;
            }
            else {
                ignoreFrames--;
            }
        }
        Point2D position = pos(agent);
        if (targetFrames > 0) {
            targetFrames--;
        }else{
            targetWrap = nullptr;
            UnitWrappers potentialTargets = UnitWrappers();
            std::vector<float> potentialPriority = std::vector<float>();
            for (Weapon w : getStats(agent).weapons) {
                
                for (UnitWrapper* enemy : SpacialHash::findInRadiusEnemy(position, w.range + radius + extraRadius, agent)) {
                    float weaponRadius = w.range + radius + enemy->radius;
                    float enemyRadius = Distance2D(position, enemy->pos(agent));
                    if (Army::hitsUnit(w.type, Army::unitTypeTargetComposition(enemy->type))) {
                        bool inserted = false;
                        float priority = priorityAttack(w, enemy, agent);
                        if (enemyRadius > weaponRadius) {
                            priority += (weaponRadius - enemyRadius) * 0.1F;
                        }
                        if (potentialTargets.size() == 0) {
                            inserted = true;
                            potentialTargets.push_back(enemy);
                            potentialPriority.push_back(priority);
                        }
                        for (int d = 0; d < potentialTargets.size(); d++) {
                            if (potentialPriority[d] < priority) {
                                potentialTargets.insert(potentialTargets.begin() + d, enemy);
                                potentialPriority.insert(potentialPriority.begin() + d, priority);
                                inserted = true;
                                break;
                            }
                        }
                        if (inserted == false) {
                            potentialTargets.push_back(enemy);
                            potentialPriority.push_back(priority);
                        }
                        break;
                    }
                }
            }
            if (potentialTargets.size() != 0) {
                targetWrap = potentialTargets.front();
                targetFrames = 10;
            }
        }
        if (ignoreFrames == 0) {
            if (targetWrap == nullptr) {
                if (!withSquad(agent)) {
                    agent->Actions()->UnitCommand(self, ABILITY_ID::ATTACK, squad->center(agent));
                } else {
                    agent->Actions()->UnitCommand(self, ABILITY_ID::ATTACK, squad->location);
                }
            }
            else {
                float dTtoEnemy = Distance2D(pos(agent), targetWrap->pos(agent)) / getStats(agent).movement_speed;
                if (dTtoEnemy >= get(agent)->weapon_cooldown) {
                    agent->Actions()->UnitCommand(self, ABILITY_ID::ATTACK, targetWrap->self);
                }
                else {
                    
                    //Point2D best = UnitManager::findMinimumDamage(this, 4, agent);
                    Point2D direction = UnitManager::weightedVector(this, 1, agent);
                    //printf("%.1f,%.1f min\n", (best - position).x, (best - position).y);
                    //printf("%.1f,%.1f dir\n", direction.x, direction.y);
                    //agent->Actions()->UnitCommand(self, ABILITY_ID::MOVE_MOVE, best);
                    //agent->Debug()->DebugLineOut(Point3D{best.x,best.y,0}, Point3D{best.x,best.y, 53.0F});
                    Point2D location = position - direction * 2;
                    agent->Actions()->UnitCommand(self, ABILITY_ID::MOVE_MOVE, location);
                    agent->Debug()->DebugLineOut(Point3D{ location.x,location.y,0 }, Point3D{ location.x,location.y, 53.0F });
                    //for (int i = 0; i < 16; i++) {
                    //    float x = cos(2.0 * M_PI * i / 16.0);
                    //    float y = sin(2.0 * M_PI * i / 16.0);
                    //    if()
                    //}
                }
            }
            ignoreFrames = 10;
        }
        return false;
    }

    virtual bool executeRetreat(Agent *agent) {
        Units enemies = agent->Observation()->GetUnits(Unit::Alliance::Enemy);
        return false;
    }

    virtual bool executeDefend(Agent *agent) {
        Units enemies = agent->Observation()->GetUnits(Unit::Alliance::Enemy);
        float dist = Distance2D(squad->location, pos(agent));
        //printf("%s %.1f\n", UnitTypeToName(type), dist);
        if ((get(agent)->orders.size() == 0 || get(agent)->orders[0].target_pos != squad->location) &&
            dist > squad->armyballRadius()) {
            agent->Actions()->UnitCommand(self, ABILITY_ID::ATTACK, squad->location);
        }
        return false;
    }

    virtual bool executeFullRetreat(Agent *agent) {
        Units enemies = agent->Observation()->GetUnits(Unit::Alliance::Enemy);
        return false;
    }

    virtual bool executeDamaged(Agent *agent, float health, float shields) {
        Units enemies = agent->Observation()->GetUnits(Unit::Alliance::Enemy);
        return false;
    }

    virtual ~ArmyUnit() {
        squad->comp = Composition::Invalid;
        for (auto it = squad->army.begin(); it != squad->army.end(); it++) {
            // printf("%lx %lx", )
            if ((*it)->self == self) {
                squad->army.erase(it);
                break;
            }
        }
    }
};

class Stalker : public ArmyUnit {
private:
public:
    Stalker(const Unit *unit) : ArmyUnit(unit) {
    }

    virtual bool executeDamaged(Agent *agent, float health, float shields) {
        if (shields < 0.05) {
            // Units enemies = agent->Observation()->GetUnits(Unit::Alliance::Enemy);
            std::vector<DamageNet> enemiesnet;
            Point2D displace;
            Point2D stutterDisplace;
            int numStutterAvg = 0;
            for (auto it = UnitManager::enemies.begin(); it != UnitManager::enemies.end(); it++) {
                auto all = it->second;
                for (auto it2 = all.begin(); it2 != all.end(); it2++) {
                    UnitTypeData enemy_stats = Aux::getStats((*it2)->type, agent);
                    if (enemy_stats.weapons.size() == 0) {
                        continue;
                    }
                    float distance = Distance2D((*it2)->pos(agent), pos(agent));
                    bool withinEnemyRadius = false;
                    float r1r2 = (*it2)->radius + get(agent)->radius;
                    for (int i = 0; i < enemy_stats.weapons.size(); i++) {
                        float r = enemy_stats.weapons[i].range + r1r2;
                        if ((distance < r) &&
                            (enemy_stats.weapons[i].type == Weapon::TargetType::Any ||
                             (enemy_stats.weapons[i].type == Weapon::TargetType::Ground && !get(agent)->is_flying) ||
                             (enemy_stats.weapons[i].type == Weapon::TargetType::Air && get(agent)->is_flying))) {
                            // enemy_radius = enemy_stats.weapons[i].range;
                            withinEnemyRadius = true;
                            displace += normalize(pos(agent) - (*it2)->pos(agent)) * (r - distance);
                        }
                    }
                    if (displace == Point2D{0, 0}) {
                        continue;
                    }
                }
            }
            if (std::sqrt(displace.x * displace.x + displace.y * displace.y) < 2) {
                displace = normalize(displace) * 2;
            }
            Point3D upos = pos3D(agent);
            Point3D blinkPos{upos.x + displace.x, upos.y + displace.y, upos.z};
            agent->Debug()->DebugLineOut(upos, blinkPos, {24, 123, 250});
            agent->Actions()->UnitCommand(self, ABILITY_ID::EFFECT_BLINK_STALKER, blinkPos);
        }
        return false;
    }
};

class ObserverEye : public ArmyUnit {
private:

public:
    ObserverEye(const Unit *unit) : ArmyUnit(unit) {
        squad->squadStates[self] = 'm';
    }

    virtual bool execute(Agent *agent) {
        if (ignoreFrames > 0) {
            ignoreFrames--;
            return false;
        }
        agent->Actions()->UnitCommand(self, ABILITY_ID::MOVE_MOVE, squad->center(agent));
        ignoreFrames = 50;
    }
};

class Zealot : public ArmyUnit {
private:

public:
    Zealot(const Unit* unit) : ArmyUnit(unit) {
        squad->squadStates[self] = 'm';
    }

    virtual bool execute(Agent* agent) {
        if (ignoreFrames > 0) {
            ignoreFrames--;
            return false;
        }
        if (squad->intermediateLoc != Point2D{ 0,0 }) {
            agent->Actions()->UnitCommand(self, ABILITY_ID::ATTACK, squad->intermediateLoc);
        }
        else {
            agent->Actions()->UnitCommand(self, ABILITY_ID::ATTACK, squad->location);
        }
        ignoreFrames = 3;
    }
};

class WarpPrism : public ArmyUnit {
private:
public:
    WarpPrism(const Unit *unit) : ArmyUnit(unit) {
    }

    // virtual bool execute(Agent *agent) {
    //
    // }
};

class Adept : public ArmyUnit {
private:
public:
    Adept(const Unit *unit) : ArmyUnit(unit) {
    }

    // virtual bool execute(Agent *agent) {
    //
    // }
};

class Immortal : public ArmyUnit {
private:
public:
    Immortal(const Unit *unit) : ArmyUnit(unit) {
    }

    // virtual bool execute(Agent *agent) {
    //
    // }
};