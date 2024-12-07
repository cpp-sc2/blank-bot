// Anshal Jain

#include <sc2api/sc2_api.h>
#include <sc2lib/sc2_lib.h>

#include <iostream>
#include "jps.hpp"
//#include "jps_old.hpp"
#include "macro.hpp"
#include "grid.hpp"
#include "tools.hpp"
#include "constants.h"
#include "unitmanager.hpp"
#include "dist_transform.hpp"
#include "zhangSuen.hpp"
#include "spacialhashgrid.hpp"
#include "BoudaoudSiderTariThinning.hpp"
#include <sc2api/sc2_gametypes.h>
#include <sc2utils/sc2_arg_parser.h>
#define MICRO_TEST 0
#define START_ME Point2D{40,50}
#define START_OP Point2D{60,50}

using namespace sc2;

class Bot : public Agent {
public:
    //std::vector<Location> path;
    std::vector < std::vector<Location>> expansionPaths;
    std::vector<Point2DI> path;

    std::vector<Point3D> expansions;
    std::vector<Point3D> rankedExpansions;
    std::vector<double> expansionDistance;
    std::vector<double> rankedExpansionDistance;
    Point2DI staging_location;
    Point2D rally_point;

    map2d<int8_t>* path_zhang_suen;

    clock_t last_time;

    Point3D P3D(const Point2D& p) {
        return Point3D(p.x, p.y, Observation()->TerrainHeight(p));
    }

    void initializeStartings() {
        GameInfo game_info = Observation()->GetGameInfo();
        if (Observation()->GetStartLocation().x > game_info.width / 2) {
            staging_location.x = Observation()->GetStartLocation().x - 6;
        } else {
            staging_location.x = Observation()->GetStartLocation().x + 6;
        }

        if (Observation()->GetStartLocation().y > game_info.height / 2) {
            staging_location.y = Observation()->GetStartLocation().y - 6;
        } else {
            staging_location.y = Observation()->GetStartLocation().y + 6;
        }
    }

    void initializeExpansions() {
        // staging_location = Point2DI(startLocation.x + ;
        expansions = sc2::search::CalculateExpansionLocations(Observation(), Query());
        for (auto point : expansions) {
            if (point.x == 0 && point.y == 0)
                continue;
            // auto path = generator.findPath(staging_location, (Point2DI)point);
            // expansionDistance.push_back(path.size());
            auto came_from = jps(gridmap, staging_location, {int(point.x),int(point.y)}, Tool::euclidean, this);
            auto pathToExpansion = Tool::reconstruct_path(staging_location, {int(point.x), int(point.y)}, came_from);

            double length = fullDist(pathToExpansion);
            //expansionDistance.push_back(length);

            //for (Location l : pathToExpansion) {
            //    printf("[%d,%d]", l.x, l.y);
            //}
            //printf("{%.1f}\n\n", length);

            if (rankedExpansions.size() == 0) {
                rankedExpansions.push_back(point);
                rankedExpansionDistance.push_back(length);
            }
            for (int i = 0; i < rankedExpansions.size(); i ++) {
                if (rankedExpansionDistance[i] > length) {
                    rankedExpansions.insert(rankedExpansions.begin() + i, point);
                    rankedExpansionDistance.insert(rankedExpansionDistance.begin() + i, length);
                    break;
                }
            }
        }

        //for (int j = 0; j < expansions.size() - 1; j++) {
        //    if (expansions[j].x == 0 && expansions[j].y == 0)
        //        continue;
        //    if (rankedExpansions.size() == 0) {
        //        rankedExpansions.push_back(expansions[j]);  // might be useless now
        //        rankedExpansionDistance.push_back(expansionDistance[j]);
        //        continue;
        //    }
        //    bool inserted = false;
        //    for (int i = 0; i < rankedExpansions.size(); i++) {
        //        if (rankedExpansionDistance[i] > expansionDistance[j]) {
        //            rankedExpansions.insert(rankedExpansions.begin() + i, expansions[j]);  // might be useless now
        //            rankedExpansionDistance.insert(rankedExpansionDistance.begin() + i, expansionDistance[j]);
        //            inserted = true;
        //            break;
        //        }
        //    }
        //    if (!inserted) {
        //        rankedExpansions.push_back(expansions[j]);  // might be useless now
        //        rankedExpansionDistance.push_back(expansionDistance[j]);
        //    }
        //}

        //for (int i = 0; i < rankedExpansions.size(); i++) {
        //    printf("[%.1f,%.1f %.1f]\n", expansions[i].x, expansions[i].y, expansionDistance[i]);
        //}
    }

    void grid() {
        GameInfo game_info = Observation()->GetGameInfo();

        int mapWidth = game_info.width;
        int mapHeight = game_info.height;

        Point2D center = Observation()->GetCameraPos();
        int wS = int(center.x) - 10;
        if (wS < 0)
            wS = 0;
        int hS = int(center.y) - 5;
        if (hS < 0)
            hS = 0;
        int wE = int(center.x) + 11;
        if (wE > mapWidth)
            wE = mapWidth;
        int hE = int(center.y) + 14;
        if (hE > mapHeight)
            hE = mapHeight;

        int fontSize = 8;

        #define BOX_BORDER 0.02

        for (int w = wS; w < wE; w++) {
            for (int h = hS; h < hE; h++) {
                Point2DI point = Point2DI(w, h);
                float boxHeight = 0;
                Color c(255,255,255);

                //for (auto loc : path) {
                //    if (loc.x == w && loc.y == h) {
                //        c = Color(120, 23, 90);
                //        break;
                //    }
                //}

                if (imRef(path_zhang_suen, w, h)) {
                    c = {250, 200, 210};
                }
                else if (imRef(Aux::buildingBlocked, w, h) != 0) {
                    //boxHeight = 1;
                    c = {123,50,10};
                } else if (imRef(Aux::influenceMap, w, h) != 0) {
                    c = {44, 50, 210};
                } else if (imRef(Aux::influenceMapEnemy, w, h) != 0) {
                    c = {
                        210,
                        50, 44
                    };
                }

                if (0 || !(c.r == 255 && c.g == 255 && c.b == 255) || boxHeight != 0) {
                    float height = Observation()->TerrainHeight(P2D(point) + Point2D{0.5F,0.5F});

                    

                    Debug()->DebugBoxOut(Point3D(w + BOX_BORDER, h + BOX_BORDER, height + 0.01),
                                         Point3D(w + 1 - BOX_BORDER, h + 1 - BOX_BORDER, height + boxHeight), c);
                    #if MICRO_TEST
                        Debug()->DebugTextOut(strprintf("%d, %d", w, h),
                                              Point3D(w + BOX_BORDER, h + 0.2 + BOX_BORDER, height + 0.1),
                                              Color(200, 90, 15), 4);
                    #endif
                    /*std::string cs = imRef(display, w, h);
                    float disp = cs.length() * 0.0667 * fontSize / 15;
                    Debug()->DebugTextOut(cs, Point3D(w + 0.5 - disp, h + 0.5, height + 0.1 + displace),
                                          Color(200, 190, 115), fontSize);*/
                }
            }
        }
    }

    void displaySpacialHashGrid() {
        GameInfo game_info = Observation()->GetGameInfo();

        int mapWidth = game_info.width;
        int mapHeight = game_info.height;

        Point2D center = Observation()->GetCameraPos();
        int wS = int(center.x) - 10;
        if (wS < 0)
            wS = 0;
        int hS = int(center.y) - 5;
        if (hS < 0)
            hS = 0;
        int wE = int(center.x) + 11;
        if (wE > mapWidth)
            wE = mapWidth;
        int hE = int(center.y) + 14;
        if (hE > mapHeight)
            hE = mapHeight;

        int fontSize = 8;

        #define BOX_BORDER 0.02

        for (int w = wS; w < wE; w++) {
            for (int h = hS; h < hE; h++) {
                Point2DI point = Point2DI(w, h);
                float boxHeight = 0;
                Color c(255, 255, 255);

                // for (auto loc : path) {
                //     if (loc.x == w && loc.y == h) {
                //         c = Color(120, 23, 90);
                //         break;
                //     }
                // }

                if (imRef(SpacialHash::grid, w, h).size() != 0) {
                        c = {20, 200, 210};
                } else if (imRef(SpacialHash::gridEnemy, w, h).size() != 0) {
                        c = {210, 200, 20};
                }

                if (0 || !(c.r == 255 && c.g == 255 && c.b == 255)) {
                    float height = Observation()->TerrainHeight(P2D(point) + Point2D{0.5F, 0.5F});

                    Debug()->DebugBoxOut(Point3D(w + BOX_BORDER, h + BOX_BORDER, height + 0.01),
                                            Point3D(w + 1 - BOX_BORDER, h + 1 - BOX_BORDER, height + boxHeight), c);
                    #if MICRO_TEST
                        Debug()->DebugTextOut(strprintf("%d, %d", w, h),
                                            Point3D(w + BOX_BORDER, h + 0.2 + BOX_BORDER, height + 0.1),
                                            Color(200, 90, 15), 4);
                    #endif
                    /*std::string cs = imRef(display, w, h);
                    float disp = cs.length() * 0.0667 * fontSize / 15;
                    Debug()->DebugTextOut(cs, Point3D(w + 0.5 - disp, h + 0.5, height + 0.1 + displace),
                                            Color(200, 190, 115), fontSize);*/
                }
            }
        }
    }

    void displayEnemyDamageGrid() {
        GameInfo game_info = Observation()->GetGameInfo();

        int mapWidth = game_info.width;
        int mapHeight = game_info.height;

        Point2D center = Observation()->GetCameraPos();
        int wS = int(center.x) - 10;
        if (wS < 0)
            wS = 0;
        int hS = int(center.y) - 5;
        if (hS < 0)
            hS = 0;
        int wE = int(center.x) + 11;
        if (wE > mapWidth)
            wE = mapWidth;
        int hE = int(center.y) + 14;
        if (hE > mapHeight)
            hE = mapHeight;

        int fontSize = 8;

        #define BOX_BORDER_S 0.002

        for (float w = wS; w < wE; w += blockSize) {
            for (float h = hS; h < hE; h += blockSize) {
                Point2D p(w, h);
                float boxHeight = 0;
                Color c(255, 255, 255);

                // for (auto loc : path) {
                //     if (loc.x == w && loc.y == h) {
                //         c = Color(120, 23, 90);
                //         break;
                //     }
                // }

                if (damageNetEnemy(p).ground != 0 || damageNetEnemy(p).air != 0) {
                    uint8_t r = (damageNetEnemy(p).ground * 2);
                    uint8_t b = (damageNetEnemy(p).air * 2);
                    //printf("col %d %d\n", r, b);
                    c = {r, 0, b};
                }

                if (0 || !(c.r == 255 && c.g == 255 && c.b == 255)) {
                    float height = Observation()->TerrainHeight(Point2D{w+0.5F, h+0.5F});

                    Debug()->DebugBoxOut(Point3D(w + BOX_BORDER_S, h + BOX_BORDER_S, height + 0.01),
                        Point3D(w + blockSize - BOX_BORDER_S, h + blockSize - BOX_BORDER_S, height - 0.01), c);
                #if MICRO_TEST
                    Debug()->DebugTextOut(strprintf("%d, %d", w, h),
                        Point3D(w + BOX_BORDER_S, h + 0.2 + BOX_BORDER_S, height + 0.1),
                        Color(200, 90, 15), 4);
                #endif
                    /*std::string cs = imRef(display, w, h);
                    float disp = cs.length() * 0.0667 * fontSize / 15;
                    Debug()->DebugTextOut(cs, Point3D(w + 0.5 - disp, h + 0.5, height + 0.1 + displace),
                                            Color(200, 190, 115), fontSize);*/
                }
            }
        }
    }

    void listUnitWraps() {
        string tot = "UNITS:\n";
        for (auto it = UnitManager::units.begin(); it != UnitManager::units.end(); it++) {
            auto all = it->second;
            string type = UnitTypeToName(it->first);
            tot += ("\n"+ type + ":\n");
            for (auto it2 = all.begin(); it2 != all.end(); it2++) {
                tot += strprintf("%lx\n",(*it2)->self);
            }
        }
        Debug()->DebugTextOut(tot, Point2D(0.01, 0.01), Color(100, 190, 215), 8);
    }

    void listUnitWrapsNeutral() {
        string tot = "UNITS:\n";
        for (auto it = UnitManager::neutrals.begin(); it != UnitManager::neutrals.end(); it++) {
            auto all = it->second;
            string type = UnitTypeToName(it->first);
            tot += ("\n" + type + ":\n");
            for (auto it2 = all.begin(); it2 != all.end(); it2++) {
                tot += strprintf("%lx\n", (*it2)->self);
            }
        }
        Debug()->DebugTextOut(tot, Point2D(0.11, 0.01), Color(100, 190, 215), 8);
    }

    void listUnitWrapsEnemies() {
        string tot = "UNITS:\n";
        for (auto it = UnitManager::enemies.begin(); it != UnitManager::enemies.end(); it++) {
            auto all = it->second;
            string type = UnitTypeToName(it->first);
            tot += ("\n" + type + ":\n");
            for (auto it2 = all.begin(); it2 != all.end(); it2++) {
                tot += strprintf("%lx\n", (*it2)->self);
            }
        }
        Debug()->DebugTextOut(tot, Point2D(0.21, 0.01), Color(100, 190, 215), 8);
    }

    void listMacroActions() {
        string tot = "MACRO:\n";
        for (auto it = Macro::actions.begin(); it != Macro::actions.end(); it++) {
            auto all = it->second;
            string type = UnitTypeToName(it->first);
            tot += ("\n" + type + ":\n");
            for (auto it2 = all.begin(); it2 != all.end(); it2++) {
                tot += strprintf("%s %d %.1f,%.1f\n", AbilityTypeToName(it2->ability), it2->index, it2->pos.x, it2->pos.y);
            }
        }
        Debug()->DebugTextOut(tot, Point2D(0.01, 0.11), Color(250, 50, 15), 8);
    }

    void probeLines() {
        auto probes = UnitManager::get(UNIT_TYPEID::PROTOSS_PROBE);
        for (auto it = probes.begin(); it != probes.end(); it++) {
            // printf("Probe %xu Mineral %xu\n", it->first, it->second.minerals);
            Probe* probe = ((Probe*)*it);
            if (Observation()->GetUnit(probe->getTargetTag(this)) != nullptr && Observation()->GetUnit(probe->self) != nullptr)
                Debug()->DebugLineOut(Observation()->GetUnit(probe->getTargetTag(this))->pos + Point3D(0, 0, 1),
                                      Observation()->GetUnit(probe->self)->pos + Point3D(0, 0, 1), Color(20, 90, 215));
            else if (Observation()->GetUnit(probe->self) != nullptr) {
                Debug()->DebugLineOut(Observation()->GetUnit(probe->self)->pos,
                                      Observation()->GetUnit(probe->self)->pos + Point3D(0, 0, 1), Color(20, 90, 215));
            }
        }
    }

    void orderDisplay() {
        Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
        for (const Unit* unit : units) {
            if (unit->orders.size() == 0)
                continue;
            #define LETTER_DISP -0.07F
            string s = "";
            if (unit->orders[0].target_unit_tag != NullTag && unit->orders[0].target_pos.x != 0 && unit->orders[0].target_pos.y != 0){
                s += strprintf("%s [%lx] [%.1f, %.1f]", AbilityTypeToName(unit->orders[0].ability_id),
                                     unit->orders[0].target_unit_tag, unit->orders[0].target_pos.x,
                                     unit->orders[0].target_pos.y);
                
            }else if (unit->orders[0].target_pos.x != 0 && unit->orders[0].target_pos.y != 0) {
                s += strprintf("%s [%.1f, %.1f]", AbilityTypeToName(unit->orders[0].ability_id),
                                      unit->orders[0].target_pos.x, unit->orders[0].target_pos.y);
            } else if (unit->orders[0].target_unit_tag != NullTag) {
                s += strprintf("%s [%lx]", AbilityTypeToName(unit->orders[0].ability_id),
                                     unit->orders[0].target_unit_tag);
            } else {
                s += strprintf("%s", AbilityTypeToName(unit->orders[0].ability_id));
            }

            if (unit->weapon_cooldown != 0) {
                s += strprintf(" %.1fs", unit->weapon_cooldown);
            }
            Debug()->DebugTextOut(s, unit->pos + Point3D{s.size() * LETTER_DISP, 0, 0.5}, Color(100, 210, 55), 8);
        }
    }

    void tagDisplay() {
        Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
        for (const Unit* unit : units) {
            if (unit->orders.size() == 0)
                continue;
            #define LETTER_DISP -0.07F
            string s = strprintf("%lx", unit->tag);
            Debug()->DebugTextOut(s, unit->pos + Point3D{s.size() * LETTER_DISP, 0.3, 0.5}, Color(210, 55, 55), 8);
        }
    }

    void neutralDisplay() {
        for (auto it = UnitManager::neutrals.begin(); it != UnitManager::neutrals.end(); it++) {
            auto all = it->second;
            for (auto it2 = all.begin(); it2 != all.end(); it2++) {
                #define LETTER_DISP -0.07F
                string s = strprintf("%lx", (*it2)->self);
                Debug()->DebugTextOut(s, (*it2)->pos3D(this) + Point3D{s.size() * LETTER_DISP, 0.3, 0.5}, Color(210, 55, 55),
                                      8);
            }
        }
    }

    void buildingDisplay() {
        auto probes = UnitManager::get(UNIT_TYPEID::PROTOSS_PROBE);
        for (auto it = probes.begin(); it != probes.end(); it++) {
            Probe* probe = ((Probe*)*it);
            if (probe->buildings.size() == 0)
                continue;
            Debug()->DebugTextOut(strprintf("%s %d,%d", AbilityTypeToName(probe->buildings[0].build),
                                            probe->buildings[0].pos.x, probe->buildings[0].pos.y),
                                  Observation()->GetUnit(probe->self)->pos + Point3D{0, 0, 0}, Color(100, 30, 55),
                                  8);
        }
    }

    void enemiesDisplay() {
        for (auto it = UnitManager::enemies.begin(); it != UnitManager::enemies.end(); it++) {
            auto all = it->second;
            for (auto it2 = all.begin(); it2 != all.end(); it2++) {
                string tot = strprintf("%s", UnitTypeToName((*it2)->type));
                Debug()->DebugTextOut(tot, (*it2)->pos3D(this), {255, 40, 10}, 8);
                float radius = Aux::structureDiameter((*it2)->type);
                if (radius == 0) {
                    radius = 0.5;
                }
                Debug()->DebugSphereOut((*it2)->pos3D(this), radius, {255, 40, 10});
            }
        }
        
    }

    void expansionsLoc() {
        for (int i = 0; i < rankedExpansions.size(); i++) {
            Point3D p = P3D(rankedExpansions[i]);
            Debug()->DebugSphereOut(p, 12, {253, 216, 53});
            Debug()->DebugTextOut(strprintf("%.1f", rankedExpansionDistance[i]), p + Point3D{0, 0, 0.5});
        }
    }

    void pylonBuildingLoc() {
        for (int i = 0; i < Aux::pylonLocations.size(); i++) {
            Point3D p = P3D(Aux::pylonLocations[i]);
            Debug()->DebugBoxOut(p + Point3D{-1, -1, 0}, p + Point3D{1, 1, 2});
        }
        for (int i = 0; i < Aux::buildingLocations.size(); i++) {
            Point3D p = P3D(Aux::buildingLocations[i]);
            Debug()->DebugBoxOut(p + Point3D{-1.5, -1.5, 0}, p + Point3D{1.5, 1.5, 3});
        }
    }

    void manageArmy() {
        EnemySquads danger = checkDangerAtHome();
        if (danger.size() == 0) {
            #if MICRO_TEST
                squads[0].attack(START_OP);
            #else
                if ((squads[0].army.size() > 9 && squads[0].withinRadius(this)) || squads[0].army.size() > 11) {
                    squads[0].attack(Observation()->GetGameInfo().enemy_start_locations[0]);
                } else {
                    squads[0].attack(rally_point);
                }
            #endif
        } else {
            for (int i = 0; i < danger.size(); i++) {
                if (danger[i].unitComp.size() == 1 && (danger[i].unitComp[0] == UNIT_TYPEID::PROTOSS_PROBE ||
                                                       danger[i].unitComp[0] == UNIT_TYPEID::ZERG_DRONE ||
                                                       danger[i].unitComp[0] == UNIT_TYPEID::TERRAN_SCV)) {
                    UnitWrappers probes = UnitManager::get(UNIT_TYPEID::PROTOSS_PROBE);
                    UnitWrapper* closest = nullptr;
                    for (UnitWrapper* probeWrap : probes) {
                        if (closest == nullptr || Distance2D(probeWrap->pos(this), danger[i].center) <
                                                      Distance2D(closest->pos(this), danger[i].center)) {
                            closest = probeWrap;
                        }
                    }
                    Actions()->UnitCommand(closest->self, ABILITY_ID::ATTACK, danger[i].center);
                } else {
                    squads[0].attack(danger[0].center);
                }
            }
        }
    }

    EnemySquads checkDangerAtHome() {
        //UnitWrappers danger = UnitWrappers();
        EnemySquads danger = EnemySquads();
        for (auto it = UnitManager::enemies.begin(); it != UnitManager::enemies.end(); it++) {
            auto all = it->second;
            for (auto it2 = all.begin(); it2 != all.end(); it2++) {
                Point2D pos = (*it2)->pos(this);
                if (pos == Point2D{0, 0}) {
                    continue;
                }
                if (imRef(Aux::influenceMap, int(pos.x), int(pos.y)) != 0) {
                    //danger.push_back((*it2));
                    bool added = false;
                    for (int i = 0; i < danger.size(); i++) {
                        if (Distance2D(danger[i].center, pos) < ENEMY_SQUAD_RADIUS) {
                            danger[i].add(*it2, this);
                            added = true;
                            break;
                        }
                    }
                    if (added == false) {
                        danger.emplace_back();
                        danger.back().add(*it2, this);
                    }
                }
            }
        }
        return danger;
    }

    //! Called when a game is started or restarted.
    virtual void OnGameStart() final {
        //profilerPrint = false;
        profilerThreshold = 10;
        last_time = clock();
        int mapWidth = Observation()->GetGameInfo().width;
        int mapHeight = Observation()->GetGameInfo().height;
        SpacialHash::grid = new map2d<UnitWrappers>(mapWidth, mapHeight, true);
        SpacialHash::gridEnemy = new map2d<UnitWrappers>(mapWidth, mapHeight, true);
        Aux::buildingBlocked = new map2d<int8_t>(mapWidth, mapHeight, true);
        Aux::influenceMap = new map2d<int8_t>(mapWidth, mapHeight, true);
        Aux::influenceMapEnemy = new map2d<int8_t>(mapWidth, mapHeight, true);
        path_zhang_suen = new map2d<int8_t>(mapWidth, mapHeight, true);

        UnitManager::enemyDamageNet = new map2d<DamageLocation>(mapWidth * UnitManager::damageNetPrecision, mapHeight * UnitManager::damageNetPrecision, true);

        //UnitManager::enemyDamageNetGround = new map2d<int>(mapWidth * UnitManager::damageNetPrecision, mapHeight * UnitManager::damageNetPrecision, true);
        //UnitManager::enemyDamageNetGroundLight = new map2d<int>(mapWidth * UnitManager::damageNetPrecision, mapHeight * UnitManager::damageNetPrecision, true);
        //UnitManager::enemyDamageNetGroundArmored = new map2d<int>(mapWidth * UnitManager::damageNetPrecision, mapHeight * UnitManager::damageNetPrecision, true);
        //UnitManager::enemyDamageNetAir = new map2d<int>(mapWidth * UnitManager::damageNetPrecision, mapHeight * UnitManager::damageNetPrecision, true);
        //UnitManager::enemyDamageNetAirLight = new map2d<int>(mapWidth * UnitManager::damageNetPrecision, mapHeight * UnitManager::damageNetPrecision, true);
        //UnitManager::enemyDamageNetAirArmored = new map2d<int>(mapWidth * UnitManager::damageNetPrecision, mapHeight * UnitManager::damageNetPrecision, true);

        SpacialHash::initGrid(this);
        SpacialHash::initGridEnemy(this);

        for (int i = 0; i < path_zhang_suen->width(); i++) {
            for (int j = 0; j < path_zhang_suen->height(); j++) {
                imRef(path_zhang_suen, i, j) = Observation()->IsPathable(Point2D{i+0.5F,j+0.5F});
            }
        }

        Units units = Observation()->GetUnits(sc2::Unit::Alliance::Neutral);

        for (const Unit* unit : units) {
            if (Aux::isMineral(*unit)) {
                imRef(path_zhang_suen, int(unit->pos.x + 0.5F), int(unit->pos.y)) = 1;
                imRef(path_zhang_suen, int(unit->pos.x - 0.5F), int(unit->pos.y)) = 1;
                imRef(Aux::buildingBlocked, int(unit->pos.x + 0.5F), int(unit->pos.y)) = 1;
                imRef(Aux::buildingBlocked, int(unit->pos.x - 0.5F), int(unit->pos.y)) = 1;
            } else if (Aux::isVespene(*unit)) {
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        imRef(path_zhang_suen, int(unit->pos.x + i), int(unit->pos.y + j)) = 1;
                        imRef(Aux::buildingBlocked, int(unit->pos.x + i), int(unit->pos.y + j)) = 1;
                    }
                }
            } else {
                //printf("NEUTRAL:%s\n", UnitTypeToName(unit->unit_type));
                Aux::addPlacement(unit->pos, unit->unit_type);
            }
        }

        #if MICRO_TEST == 0
            for (int i = -4; i <= 4; i++) {
                for (int j = -4; j <= 4; j++) {
                    imRef(path_zhang_suen, int(Observation()->GetStartLocation().x + i),
                          int(Observation()->GetStartLocation().y + j)) = 1;
                }
            }
        #endif

        zhangSuenThinning(path_zhang_suen, this);
        //thinning_BST(path_zhang_suen, this);

        gridmap = Grid{Observation()->GetGameInfo().width, Observation()->GetGameInfo().height};

        //const ObservationInterface* observe = Observation();
        //PathFinder pf(observe->GetGameInfo().width, observe->GetGameInfo().height);
        //cout << pf.findPath(observe->GetGameInfo().start_locations[0], observe->GetGameInfo().enemy_start_locations[0], this) << endl;
        //unordered_set<Location> walls{{5, 0}, {5, 1}, {2, 2}, {5, 2}, {2, 3}, {5, 3}, {2, 4}, {5, 4},
        //                              {2, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {2, 6}, {2, 7}};

        //Point2DI start{142, 45};
        //Point2DI goal{130, 60};

        //auto came_from = jps(gridmap, start, goal, Tool::euclidean, this);
        //auto pat = Tool::reconstruct_path(start, goal, came_from);
        //path = fullPath(pat);
        /*for (auto loc : path) {
            printf("%d,%d\n", loc.x, loc.y);
        }*/
        //Tool::draw_grid(this, map, {}, {}, path, came_from, start, goal);
        initializeStartings();
        initializeExpansions();

        Point2D middle{Observation()->GetGameInfo().width / 2.0F, Observation()->GetGameInfo().height / 2.0F};

        vector<Point2DI> possiblePoints;

        #if MICRO_TEST == 0
        for (int i = -12; i <= 12; i++) {
            for (int j = -12; j <= 12; j++) {
                float d = Distance2D(Point2D{i + 0.5F, j + 0.5F}, {0,0});
                if (imRef(path_zhang_suen, int(rankedExpansions[0].x) + i, int(rankedExpansions[0].y) + j) == 1 &&
                    d > 10 && d < 12) {
                    possiblePoints.push_back({i + (int)rankedExpansions[0].x, j + (int)rankedExpansions[0].y});
                }
            }
        }
        #endif

        float min = -1;

        for (int i = 0; i < possiblePoints.size(); i++) {
            auto came_from =
                jps(gridmap, middle,
                                 {int(possiblePoints[i].x), int(possiblePoints[i].y)}, Tool::euclidean, this);
            auto pathToExpansion = Tool::reconstruct_path(Location(middle),
                                       {int(possiblePoints[i].x), int(possiblePoints[i].y)}, came_from);

            double length = fullDist(pathToExpansion);
            if (min == -1 || min > length) {
                min = length;
                rally_point = P2D(possiblePoints[i]) + Point2D{0.5F, 0.5F};
            }
        }

        squads.emplace_back();
        #if MICRO_TEST
            squads[0].attack(START_OP);
        #else
            squads[0].attack(rally_point);
        #endif
        

        //Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_STALKER, middle, 1, 6);
        //Debug()->DebugCreateUnit(UNIT_TYPEID::ZERG_ZERGLING, Observation()->GetGameInfo().enemy_start_locations[0], 2, 16);
        //Debug()->DebugCreateUnit(UNIT_TYPEID::PROTOSS_ADEPT, Observation()->GetGameInfo().enemy_start_locations[0], 2, 2);
    }

    //! Called when a game has ended.
    virtual void OnGameEnd() {

    }

    //! Called when a Unit has been created by the player.
    //!< \param unit The created unit.
    virtual void OnUnitCreated(const Unit* unit) {
        if (unit->tag == NullTag) {
            return;
        }
        if (unit->unit_type == UNIT_TYPEID::PROTOSS_PROBE) {
            Probe *u = new Probe(unit);
            u->execute(this);
        } else if (!unit->is_building) {
            if (unit->unit_type == UNIT_TYPEID::PROTOSS_STALKER) {
                new Stalker(unit);
            } else if (unit->unit_type == UNIT_TYPEID::PROTOSS_OBSERVER) {
                new ObserverEye(unit);
            } else if (unit->unit_type == UNIT_TYPEID::PROTOSS_ZEALOT) {
                new Zealot(unit);
            }
            else {
                new ArmyUnit(unit);
            }
        } else{
            //UnitWrapper* u = new UnitWrapper(unit->tag, unit->unit_type);
            //u->execute(this);
            UnitWrapper* u = new UnitWrapper(unit);
            u->execute(this);
        }
        //else
        if (unit->is_building) {
            Aux::addPlacement(unit->pos, unit->unit_type);
            UnitTypes allData = Observation()->GetUnitTypeData();
            UnitTypeData unit_stats = allData.at(static_cast<uint32_t>(unit->unit_type));
            GameInfo game_info = Observation()->GetGameInfo();

            for (int i = std::max(0, int(unit->pos.x - unit_stats.sight_range) - 2);
                 i < std::min(game_info.width, int(unit->pos.x + unit_stats.sight_range) + 2); i++) {
                for (int j = std::max(0, int(unit->pos.y - unit_stats.sight_range) - 2);
                     j < std::min(game_info.height, int(unit->pos.y + unit_stats.sight_range) + 2); j++) {
                    if (Distance2D(Point2D{i + 0.5F, j + 0.5F}, unit->pos) < unit_stats.sight_range) {
                        imRef(Aux::influenceMap, i, j) += 1;
                    }
                }
            }
        }
        //else {
        //    ArmyUnit* u = new ArmyUnit(unit);
        //}
    }

    //! Called whenever one of the player's units has been destroyed.
    //!< \param unit The destroyed unit.
    virtual void OnUnitDestroyed(const Unit* unit) {
        if (unit->alliance == Unit::Alliance::Self) {
            UnitWrapper* u = UnitManager::find(unit->unit_type, unit->tag);
            delete u;
            if (unit->is_building) {
                Aux::removePlacement(unit->pos, unit->unit_type);
                UnitTypes allData = Observation()->GetUnitTypeData();
                UnitTypeData unit_stats = allData.at(static_cast<uint32_t>(unit->unit_type));
                GameInfo game_info = Observation()->GetGameInfo();

                for (int i = std::max(0, int(unit->pos.x - unit_stats.sight_range) - 2);
                     i < std::min(game_info.width, int(unit->pos.x + unit_stats.sight_range) + 2); i++) {
                    for (int j = std::max(0, int(unit->pos.y - unit_stats.sight_range) - 2);
                         j < std::min(game_info.height, int(unit->pos.y + unit_stats.sight_range) + 2); j++) {
                        if (Distance2D(Point2D{i + 0.5F, j + 0.5F}, unit->pos) < unit_stats.sight_range) {
                            imRef(Aux::influenceMap, i, j) -= 1;
                        }
                    }
                }
            }
        } else if(unit->alliance == Unit::Alliance::Enemy) {
            UnitWrapper* u = UnitManager::findEnemy(unit->unit_type, unit->tag);
            delete u;
            printf("HOSTILE %s %Ix DEAD\n", UnitTypeToName(unit->unit_type), unit->tag);
            if (unit->is_building) {
                Aux::removePlacement(unit->pos, unit->unit_type);
                UnitTypes allData = Observation()->GetUnitTypeData();
                UnitTypeData unit_stats = allData.at(static_cast<uint32_t>(unit->unit_type));
                GameInfo game_info = Observation()->GetGameInfo();

                for (int i = std::max(0, int(unit->pos.x - unit_stats.sight_range) - 2);
                     i < std::min(game_info.width, int(unit->pos.x + unit_stats.sight_range) + 2); i++) {
                    for (int j = std::max(0, int(unit->pos.y - unit_stats.sight_range) - 2);
                         j < std::min(game_info.height, int(unit->pos.y + unit_stats.sight_range) + 2); j++) {
                        if (Distance2D(Point2D{i + 0.5F, j + 0.5F}, unit->pos) < unit_stats.sight_range) {
                            imRef(Aux::influenceMapEnemy, i, j) -= 1;
                        }
                    }
                }
            }
        }
        
    }

    //! Called when a unit becomes idle, this will only occur as an event so will only be called when the unit becomes
    //! idle and not a second time. Being idle is defined by having orders in the previous step and not currently having
    //! orders or if it did not exist in the previous step and now does, a unit being created, for instance, will call
    //! both OnUnitCreated and OnUnitIdle if it does not have a rally set.
    //!< \param unit The idle unit.
    virtual void OnUnitIdle(const Unit* unit) {
        //UnitManager::find(unit->unit_type, unit->tag)->execute(this);
    }

    //! In non realtime games this function gets called after each step as indicated by step size.
    //! In realtime this function gets called as often as possible after request/responses are received from the game
    //! gathering observation state.
    virtual void OnStep() final {
        Profiler onStepProfiler("onStep");
        //onStepProfiler.disable();

        Macro::execute(this);

        onStepProfiler.midLog("MacroExecute");

        //Probe::loadAbilities(this);
        UnitWrapper::loadAbilities(this);

        onStepProfiler.midLog("LoadAbilities");

        auto probes = UnitManager::get(UNIT_TYPEID::PROTOSS_PROBE);
        for (auto it = probes.begin(); it != probes.end(); it++) {
            Probe* probe = ((Probe*)*it);
            probe->execute(this);
        }

        onStepProfiler.midLog("ProbeExecute");

        SpacialHash::updateGrid(this);

        onStepProfiler.midLog("SpacialHashUpdate");

        SpacialHash::updateGridEnemy(this);

        onStepProfiler.midLog("SpacialHashUpdateEnemy");

        manageArmy();

        onStepProfiler.midLog("ManageArmy");

        string s = "";
        for (int i = 0; i < squads.size(); i ++) {
            squads[i].execute(this);
            s += strprintf("SQUAD%d %s %.1f,%.1f int[%.1f,%.1f]:\n", i, SquadModeToString(squads[i].mode),
                           squads[i].location.x, squads[i].location.y, squads[i].intermediateLoc.x,
                           squads[i].intermediateLoc.y);
            Point2D c = squads[i].center(this);
            Point2D cg = squads[i].center(this);
            s += strprintf("C:%.1f,%.1f Within?:%d S:%d\n", c.x, c.y, squads[i].withinRadius(this), squads[i].army.size());
            for (int a = 0; a < squads[i].army.size(); a++) {
                squads[i].army[a]->execute(this);
                s += strprintf("%s %.1fs %c\n", UnitTypeToName(squads[i].army[a]->type),
                               squads[i].army[a]->get(this)->weapon_cooldown,
                               squads[i].squadStates[squads[i].army[a]->self]);
                Debug()->DebugTextOut(strprintf("%c", squads[i].squadStates[squads[i].army[a]->self]),
                                      squads[i].army[a]->pos(this) + Point3D{s.size() * LETTER_DISP, 0.3, 0.5},
                                      Color(210, 55, 55), 8);
            }
            s += '\n';
            Debug()->DebugSphereOut(P3D(squads[i].center(this)), squads[i].armyballRadius());
            Debug()->DebugSphereOut(P3D(squads[i].intermediateLoc), 0.5, {30,230, 210});
        }
        Debug()->DebugTextOut(s, Point2D(0.71, 0.11), Color(1, 42, 212), 8);

        onStepProfiler.midLog("SquadExecute");
        #if MICRO_TEST == 0
            if (Observation()->GetGameLoop() == 2) {
                //HOME BASE MINERALS
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();

                //HOME BASE VESPENE 1
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();

                //HOME BASE VESPENE 2
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();

                //NATURAL MINERALS
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();//
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();//
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();//
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();//

                //NATURAL GAS 1
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();

                //NATURAL GAS 2
                Macro::addProbe();
                Macro::addProbe();
                Macro::addProbe();

                //Macro::addBuilding(ABILITY_ID::BUILD_PYLON, P2D(staging_location));
                //Macro::addBuilding(ABILITY_ID::BUILD_GATEWAY, P2D(staging_location) - Point2D{-2.5, 0.5});
                //Macro::addBuilding(ABILITY_ID::BUILD_ASSIMILATOR,
                //                   Observation()->GetUnit(UnitManager::getVespene()[0]->self)->pos);
                //Macro::addBuilding(ABILITY_ID::BUILD_NEXUS, P2D(rankedExpansions[0]));
                //Macro::addBuilding(ABILITY_ID::BUILD_CYBERNETICSCORE, P2D(staging_location) - Point2D{2.5, -0.5});
                //Macro::addBuilding(ABILITY_ID::BUILD_ASSIMILATOR,
                //                   Observation()->GetUnit(UnitManager::getVespene()[1]->self)->pos);
                //Macro::addBuilding(ABILITY_ID::BUILD_PYLON, P2D(staging_location) - Point2D{0.5, -2.5});
                //Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_STALKER);
                //Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_STALKER);
                //Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_STALKER);
                //Macro::addBuilding(ABILITY_ID::BUILD_STARGATE, P2D(staging_location) - Point2D{-0.5, 3});

                Macro::addBuilding(ABILITY_ID::BUILD_PYLON, P2D(staging_location));

                Macro::addBuilding(ABILITY_ID::BUILD_GATEWAY, {-1,-1});

                Macro::addBuilding(ABILITY_ID::BUILD_ASSIMILATOR,
                                   Observation()->GetUnit(UnitManager::getVespene()[0]->self)->pos);

                Macro::addBuilding(ABILITY_ID::BUILD_CYBERNETICSCORE, {-1, -1});

                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_STALKER);

                Macro::addAction(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, ABILITY_ID::RESEARCH_WARPGATE);
                Macro::addAction(UNIT_TYPEID::PROTOSS_ROBOTICSBAY, ABILITY_ID::RESEARCH_EXTENDEDTHERMALLANCE);

                Macro::addBuilding(ABILITY_ID::BUILD_NEXUS, P2D(rankedExpansions[0]));

                Macro::addBuilding(ABILITY_ID::BUILD_ASSIMILATOR,
                                   Observation()->GetUnit(UnitManager::getVespene()[1]->self)->pos);

                Macro::addBuilding(ABILITY_ID::BUILD_PYLON, {-1, -1});
                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_STALKER);
                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_STALKER);

                Macro::addBuilding(ABILITY_ID::BUILD_ROBOTICSFACILITY, {-1, -1});
                Macro::addAction(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, ABILITY_ID::TRAIN_OBSERVER);
                Macro::addAction(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, ABILITY_ID::TRAIN_IMMORTAL);

                Macro::addBuilding(ABILITY_ID::BUILD_GATEWAY, {-1, -1});
                Macro::addBuilding(ABILITY_ID::BUILD_GATEWAY, {-1, -1});
                Macro::addBuilding(ABILITY_ID::BUILD_TWILIGHTCOUNCIL, {-1, -1});

                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_STALKER);
                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_STALKER);
                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_STALKER);

            } else if (Observation()->GetGameLoop() == int(3.00 * 60 * 22.4)) {

                Macro::addAction(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, ABILITY_ID::RESEARCH_BLINK);

                Macro::addBuilding(ABILITY_ID::BUILD_ROBOTICSBAY, {-1, -1});

                Macro::addAction(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, ABILITY_ID::TRAIN_COLOSSUS);
                Macro::addAction(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, ABILITY_ID::TRAIN_COLOSSUS);
                Macro::addAction(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, ABILITY_ID::TRAIN_IMMORTAL);

                //Macro::addBuilding(ABILITY_ID::BUILD_ASSIMILATOR,
                //                   Observation()->GetUnit(UnitManager::getVespene()[2]->self)->pos);
                //Macro::addBuilding(ABILITY_ID::BUILD_ASSIMILATOR,
                //                   Observation()->GetUnit(UnitManager::getVespene()[3]->self)->pos);
                Macro::addBuilding(ABILITY_ID::BUILD_GATEWAY, {-1, -1});
                Macro::addBuilding(ABILITY_ID::BUILD_GATEWAY, {-1, -1});
                Macro::addBuilding(ABILITY_ID::BUILD_GATEWAY, {-1, -1});
                Macro::addBuilding(ABILITY_ID::BUILD_GATEWAY, {-1, -1});
                Macro::addBuilding(ABILITY_ID::BUILD_GATEWAY, {-1, -1});
                Macro::addBuilding(ABILITY_ID::BUILD_GATEWAY, {-1, -1});
                Macro::addBuilding(ABILITY_ID::BUILD_PYLON, {-1, -1});
                Macro::addBuilding(ABILITY_ID::BUILD_PYLON, {-1, -1});
                Macro::addBuilding(ABILITY_ID::BUILD_PYLON, {-1, -1});
                Macro::addBuilding(ABILITY_ID::BUILD_PYLON, {-1, -1});
                Macro::addBuilding(ABILITY_ID::BUILD_PYLON, {-1, -1});

                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_ZEALOT);
                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_ZEALOT);
                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_ZEALOT);
                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_ZEALOT);
                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_ZEALOT);
                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_ZEALOT);
                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_ZEALOT);
                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_ZEALOT);
                Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_ZEALOT);
            
                Macro::addAction(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, ABILITY_ID::TRAIN_WARPPRISM);
            }
        #endif

        onStepProfiler.midLog("BuildOrderSetup");

        int spareMinerals = Observation()->GetMinerals();
        int spareVespene = Observation()->GetVespene();

        for (auto it = Macro::actions.begin(); it != Macro::actions.end(); it++) {
            auto all = it->second;
            for (auto it2 = all.begin(); it2 != all.end(); it2++) {
                Cost c = it2->cost(this);
                spareMinerals -= c.minerals;
                spareVespene -= c.vespene;
            }
        }
        if (spareMinerals > 150 && spareVespene > 75) {
            //if (squads[0].has(UNIT_TYPEID::PROTOSS_WARPPRISM)) {
            //    Macro::addAction(UNIT_TYPEID::PROTOSS_WARPPRISM, ABILITY_ID::MORPH_WARPPRISMPHASINGMODE);
            //}
            Macro::addAction(UNIT_TYPEID::PROTOSS_GATEWAY, ABILITY_ID::TRAIN_STALKER, squads[0].center(this));
        }

        onStepProfiler.midLog("StalkerCreation");

        clock_t new_time = clock();
        int dt = (new_time - last_time);
        last_time = new_time;
        Debug()->DebugTextOut(strprintf("%d", dt), Point2D(0.10, 0.10), Color(100, 190, 215), 16);

        onStepProfiler.midLog("DT");

        for (int i = 0; i < UnitManager::enemyDamageNet->width(); i++) {
            for (int j = 0; j < UnitManager::enemyDamageNet->height(); j++) {
                imRef(UnitManager::enemyDamageNet, i, j) = { 0.0,0.0,0.0,0.0,0.0,0.0 };
            }
        }

        //for (int i = 0; i < UnitManager::enemyDamageNetGround->width(); i++) {
        //    for (int j = 0; j < UnitManager::enemyDamageNetGround->height(); j++) {
        //        imRef(UnitManager::enemyDamageNetGround, i, j) = 0;
        //        imRef(UnitManager::enemyDamageNetGroundLight, i, j) = 0;
        //        imRef(UnitManager::enemyDamageNetGroundArmored, i, j) = 0;
        //        imRef(UnitManager::enemyDamageNetAir, i, j) = 0;
        //        imRef(UnitManager::enemyDamageNetAirLight, i, j) = 0;
        //        imRef(UnitManager::enemyDamageNetAirArmored, i, j) = 0;
        //    }
        //}

        //UnitManager::enemyDamageNetGround->init(0);
        //UnitManager::enemyDamageNetGroundLight->init(0);
        //UnitManager::enemyDamageNetGroundArmored->init(0);
        //UnitManager::enemyDamageNetAir->init(0);
        //UnitManager::enemyDamageNetAirLight->init(0);
        //UnitManager::enemyDamageNetAirArmored->init(0);

        for (auto it = UnitManager::enemies.begin(); it != UnitManager::enemies.end(); it++) {
            for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
                std::vector<Weapon> weapons = Aux::getStats((*it2)->type, this).weapons;
                //TODO:
                // add abilities, oracle, baneling
                // add psi storm (more damage the more time it has left, prioritzed more since its constant dmag)
                // add helion line, lurker line
                for (Weapon w : weapons) {
                    DamageLocation d = { 0,0,0,0,0,0 };
                    int bonusarmor = 0;
                    int bonuslight = 0;
                    for (DamageBonus bonus : w.damage_bonus) {
                        if (bonus.attribute == Attribute::Light) {
                            bonuslight += bonus.bonus;
                        }
                        else if (bonus.attribute == Attribute::Armored) {
                            bonusarmor += bonus.bonus;
                        }
                        else {
                            printf("ATTRIBUTE NOT PROGRAMMED %d\n", bonus.attribute);
                        }
                    }
                    
                    if (w.type == Weapon::TargetType::Air || w.type == Weapon::TargetType::Any) {
                        d.air = w.damage_ / w.speed;
                        d.airarmored = bonusarmor / w.speed;
                        d.airlight = bonuslight / w.speed;
                    }

                    if (w.type == Weapon::TargetType::Ground || w.type == Weapon::TargetType::Any) {
                        d.ground = w.damage_ / w.speed;
                        d.groundarmored = bonusarmor / w.speed;
                        d.groundlight = bonuslight / w.speed;
                    }

                    UnitManager::setEnemyDamageRadius((*it2)->pos(this), (*it2)->radius + w.range, d, this);
                    //printf("%s %Ix set %.1f,%.1f  %.1f,%.1f  %.1f,%.1f\n", UnitTypeToName((*it2)->type), (*it2)->self, d.ground, d.air, d.groundlight, d.airlight, d.groundarmored, d.airarmored);
                }
            }
        }
        //for (const Unit* u : Observation()->GetUnits(Unit::Alliance::Self)) {
        //    UnitManager::setEnemyDamageRadius(u->pos, u->radius, this);
        //    //printf("HEIGHT %.1f\n", u->pos.z);
        //}

        onStepProfiler.midLog("DamageGridUpdate");

        //grid();

        //displaySpacialHashGrid();

        displayEnemyDamageGrid();

        //pylonBuildingLoc();
        //listUnitWraps();
        //listUnitWrapsNeutral();
        //listUnitWrapsEnemies();

        expansionsLoc();

        listMacroActions();

        probeLines();

        orderDisplay();

        //tagDisplay();
        
        neutralDisplay();

        buildingDisplay();

        enemiesDisplay();

        onStepProfiler.midLog("Debug");

        Debug()->SendDebug();
    }

    //!  Called when a neutral unit is created. For example, mineral fields observed for the first time
    //!< \param unit The observed unit.
    virtual void OnNeutralUnitCreated(const Unit* unit) {
        if (Aux::isVespene(*unit)) {
            Vespene* u = new Vespene(unit);
            u->execute(this);
        } else {
            UnitWrapper* u = new UnitWrapper(unit);
            u->execute(this);
        }
    }

    //! Called when an upgrade is finished, warp gate, ground weapons, baneling speed, etc.
    //!< \param upgrade The completed upgrade.
    virtual void OnUpgradeCompleted(UpgradeID upgradeID) {
    }

    //! Called when the unit in the previous step had a build progress less than 1.0 but is greater than or equal to 1.0
    //! in
    // !the current step.
    //!< \param unit The constructed unit.
    virtual void OnBuildingConstructionComplete(const Unit* unit) {
    }

    //! Called when the unit in the current observation has lower health or shields than in the previous observation.
    //!< \param unit The damaged unit.
    //!< \param health The change in health (damage is positive)
    //!< \param shields The change in shields (damage is positive)
    virtual void OnUnitDamaged(const Unit* unit, float health, float shields) {
        UnitWrapper* wrap = UnitManager::find(unit->unit_type, unit->tag);
        if (unit->alliance == Unit::Alliance::Self && wrap != nullptr) {
            wrap->executeDamaged(this, health, shields);
        }
    }

    //! Called when a nydus is placed.
    virtual void OnNydusDetected() {
    }

    //! Called when a nuclear launch is detected.
    virtual void OnNuclearLaunchDetected() {
    }

    //! Called when an enemy unit enters vision from out of fog of war.
    //!< \param unit The unit entering vision.
    virtual void OnUnitEnterVision(const Unit* unit) {
        UnitWrapper* u = new UnitWrapper(unit);
        u->execute(this);
        Aux::addPlacement(unit->pos, unit->unit_type);

        if (unit->is_building) {
            Aux::addPlacement(unit->pos, unit->unit_type);
            UnitTypes allData = Observation()->GetUnitTypeData();
            UnitTypeData unit_stats = allData.at(static_cast<uint32_t>(unit->unit_type));
            GameInfo game_info = Observation()->GetGameInfo();

            for (int i = std::max(0, int(unit->pos.x - unit_stats.sight_range) - 2);
                 i < std::min(game_info.width, int(unit->pos.x + unit_stats.sight_range) + 2); i++) {
                for (int j = std::max(0, int(unit->pos.y - unit_stats.sight_range) - 2);
                     j < std::min(game_info.height, int(unit->pos.y + unit_stats.sight_range) + 2); j++) {
                    if (Distance2D(Point2D{i + 0.5F, j + 0.5F}, unit->pos) < unit_stats.sight_range) {
                        imRef(Aux::influenceMapEnemy, i, j) += 1;
                    }
                }
            }
        }
    }
};


//fully optimize

//add no build zone
//fix building placements with lockout
//remove double actions from army units

//add correctly timed kiting
//stay on border of your own

//weapon net, 8x precision

//tag on units that defines whether they are being individual controlled or group-controlled

//minute micro

//predictive theoretical health stutter step when no blink

//make sure all units are engaging in fight
//add ramp analysis

//add check for no possible units that can execute an action at all

//have nexus stop checking when it has two vespenes
//fix probe build-check

//fix assimilator condition

//add generator to global variables

//if near supply cap, save for pylon

//build list

//army list

//per-unit micro

//squadrons
