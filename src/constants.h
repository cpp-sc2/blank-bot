#pragma once

#include <sc2api/sc2_api.h>
#include <sc2api/sc2_interfaces.h>

#include "map2d.hpp"
#include "profiler.hpp"
//#include "jps.hpp"
//#include "grid.hpp"
//#include "tools.hpp"

#define DISP ((size%2 == 0) ? 0.5F : 0.0F)
#define GET(tag) agent->Observation()->GetUnit(tag)

#define M_PI           3.14159265358979323846

#define AP3D(p2) Point3D(p2.x, p2.y, agent->Observation()->TerrainHeight(p2))

using namespace sc2;

//enum Composition { NONE, AIR, GND, BOTH };

//#define Composition Weapon::TargetType
using Composition = Weapon::TargetType;
using Targets = std::map<Tag, Tags>;

struct Cost {
    unsigned int minerals = 0;
    unsigned int vespene = 0;
    unsigned int energy = 0;
    int psi = 0;
};

template <class T>
class dynarray {
public:
    int size;
    T* data;

    dynarray(int size) : size(size) {
        data = new T[size];  // allocate space for image data
    }

    T operator[](int index) {
        return data[index];
    }

    ~dynarray() {
        delete[] data;
    }
};

#define dyn(im, index) (im.data[index])

template <typename... Args>
std::string strprintf(const std::string &format, Args... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;  // Extra space for '\0'
    if (size_s <= 0) {
        throw std::runtime_error("Error during formatting.");
    }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);  // We don't want the '\0' inside
}

Point2D P2D(const Point2DI &p) {
    return Point2D((float)p.x, (float)p.y);
}

Point2D P2D(const Point3D &p) {
    return Point2D(p.x, p.y);
}

Point2DI P2DI(const Point3D &p) {
    return Point2DI((int)p.x, (int)p.y);
}

Point2DI P2DI(const Point2D& p) {
    return Point2DI((int)p.x, (int)p.y);
}

Point2D normalize(const Point2D& p) {
    return p / (std::sqrt(p.x * p.x + p.y * p.y));
 
}

namespace Aux {

map2d<int8_t>* pathingMap;
map2d<int8_t> *buildingBlocked;
map2d<int8_t> *influenceMap;
map2d<int8_t> *influenceMapEnemy;

bool init_data = false;
UnitTypes cached_data;

std::map<UnitTypeID, UnitTypeData> statsMap = std::map<UnitTypeID, UnitTypeData>();

std::vector<Point2D> pylonLocations = std::vector<Point2D>();
int pylonPointer = 0;

std::vector<Point2D> buildingLocations = std::vector<Point2D>();
int buildingPointer = 0;

constexpr auto MINERALS_PER_PROBE_PER_SEC = 55.0 / 60;
constexpr auto VESPENE_PER_PROBE_PER_SEC = 61.0 / 60;

constexpr int PYLON_RADIUS = 6;

UnitTypes allData(Agent *agent) {
    if (!init_data) {
        cached_data = agent->Observation()->GetUnitTypeData();
    }
    return cached_data;
}

UnitTypeData getStats(UnitTypeID type, Agent *agent) {
    if (statsMap.find(type) == statsMap.end()) {
        statsMap[type] = allData(agent).at(static_cast<uint32_t>(type));
    }
    return statsMap[type];
}

static void loadPathables(Agent *agent) {
    int mapWidth = agent->Observation()->GetGameInfo().width;
    int mapHeight = agent->Observation()->GetGameInfo().height;
    for (int i = 0; i < mapWidth; i++) {
        for (int j = 0; j < mapHeight; j++) {
            if (!agent->Observation()->IsPathable({ float(i), float(j) })) {
                imRef(pathingMap, i, j) = 127;
            }
        }
    }
}

static bool checkPathable(int x, int y, Agent *agent) {
    if (x < 0 || x >= pathingMap->width() || y < 0 || y >= pathingMap->height()) {
        return false;
    }
    //return agent->Observation()->IsPathable({float(x), float(y)});
    return imRef(pathingMap, x, y) == 0;
}

static bool checkPathable(Point2D p, Agent* agent) {
    return checkPathable(int(p.x), int(p.y), agent);
}

static bool isPylon(const Unit &unit) {
    UnitTypeID type = unit.unit_type;
    return (type == UNIT_TYPEID::PROTOSS_PYLON);
}

static bool isNexus(const Unit &unit) {
    UnitTypeID type = unit.unit_type;
    return (type == UNIT_TYPEID::PROTOSS_NEXUS);
}

static bool isAssimilator(const Unit &unit) {
    UnitTypeID type = unit.unit_type;
    return (type == UNIT_TYPEID::PROTOSS_ASSIMILATOR);
}

static bool isMineral(const Unit &unit) {
    UnitTypeID type = unit.unit_type;
    return (type == UNIT_TYPEID::NEUTRAL_MINERALFIELD || type == UNIT_TYPEID::NEUTRAL_LABMINERALFIELD ||
            type == UNIT_TYPEID::NEUTRAL_MINERALFIELD750 || type == UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750 ||
            type == UNIT_TYPEID::NEUTRAL_MINERALFIELD450);
}

static bool isVespene(const Unit &unit) {
    UnitTypeID type = unit.unit_type;
    return (type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER || type == UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER ||
            type == UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER || type == UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER ||
            type == UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER || type == UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER);
    // return (type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER);
}

static UnitTypeID buildAbilityToUnit(AbilityID build_ability) {
    switch (uint32_t(build_ability)) {
        case (uint32_t(ABILITY_ID::BUILD_ASSIMILATOR)):
            return UNIT_TYPEID::PROTOSS_ASSIMILATOR;
        case (uint32_t(ABILITY_ID::BUILD_CYBERNETICSCORE)):
            return UNIT_TYPEID::PROTOSS_CYBERNETICSCORE;
        case (uint32_t(ABILITY_ID::BUILD_DARKSHRINE)):
            return UNIT_TYPEID::PROTOSS_DARKSHRINE;
        case (uint32_t(ABILITY_ID::BUILD_FLEETBEACON)):
            return UNIT_TYPEID::PROTOSS_FLEETBEACON;
        case (uint32_t(ABILITY_ID::BUILD_FORGE)):
            return UNIT_TYPEID::PROTOSS_FORGE;
        case (uint32_t(ABILITY_ID::BUILD_GATEWAY)):
            return UNIT_TYPEID::PROTOSS_GATEWAY;
        case (uint32_t(ABILITY_ID::BUILD_NEXUS)):
            return UNIT_TYPEID::PROTOSS_NEXUS;
        case (uint32_t(ABILITY_ID::BUILD_PHOTONCANNON)):
            return UNIT_TYPEID::PROTOSS_PHOTONCANNON;
        case (uint32_t(ABILITY_ID::BUILD_PYLON)):
            return UNIT_TYPEID::PROTOSS_PYLON;
        case (uint32_t(ABILITY_ID::BUILD_ROBOTICSBAY)):
            return UNIT_TYPEID::PROTOSS_ROBOTICSBAY;
        case (uint32_t(ABILITY_ID::BUILD_ROBOTICSFACILITY)):
            return UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY;
        case (uint32_t(ABILITY_ID::BUILD_SHIELDBATTERY)):
            return UNIT_TYPEID::PROTOSS_SHIELDBATTERY;
        case (uint32_t(ABILITY_ID::BUILD_STARGATE)):
            return UNIT_TYPEID::PROTOSS_STARGATE;
        case (uint32_t(ABILITY_ID::BUILD_TEMPLARARCHIVE)):
            return UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE;
        case (uint32_t(ABILITY_ID::BUILD_TWILIGHTCOUNCIL)):
            return UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL;
        case (uint32_t(ABILITY_ID::TRAIN_ADEPT)):
            return UNIT_TYPEID::PROTOSS_ADEPT;
        case (uint32_t(ABILITY_ID::TRAIN_ARCHON)):
            return UNIT_TYPEID::PROTOSS_ARCHON;
        case (uint32_t(ABILITY_ID::TRAIN_CARRIER)):
            return UNIT_TYPEID::PROTOSS_CARRIER;
        case (uint32_t(ABILITY_ID::TRAIN_COLOSSUS)):
            return UNIT_TYPEID::PROTOSS_COLOSSUS;
        case (uint32_t(ABILITY_ID::TRAIN_DARKTEMPLAR)):
            return UNIT_TYPEID::PROTOSS_DARKTEMPLAR;
        case (uint32_t(ABILITY_ID::TRAIN_DISRUPTOR)):
            return UNIT_TYPEID::PROTOSS_DISRUPTOR;
        case (uint32_t(ABILITY_ID::TRAIN_HIGHTEMPLAR)):
            return UNIT_TYPEID::PROTOSS_HIGHTEMPLAR;
        case (uint32_t(ABILITY_ID::TRAIN_IMMORTAL)):
            return UNIT_TYPEID::PROTOSS_IMMORTAL;
        case (uint32_t(ABILITY_ID::TRAIN_MOTHERSHIP)):
            return UNIT_TYPEID::PROTOSS_MOTHERSHIP;
        case (uint32_t(ABILITY_ID::TRAIN_MOTHERSHIPCORE)):
            return UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE;
        case (uint32_t(ABILITY_ID::TRAIN_OBSERVER)):
            return UNIT_TYPEID::PROTOSS_OBSERVER;
        case (uint32_t(ABILITY_ID::TRAIN_ORACLE)):
            return UNIT_TYPEID::PROTOSS_ORACLE;
        case (uint32_t(ABILITY_ID::TRAIN_PHOENIX)):
            return UNIT_TYPEID::PROTOSS_PHOENIX;
        case (uint32_t(ABILITY_ID::TRAIN_PROBE)):
            return UNIT_TYPEID::PROTOSS_PROBE;
        case (uint32_t(ABILITY_ID::TRAIN_SENTRY)):
            return UNIT_TYPEID::PROTOSS_SENTRY;
        case (uint32_t(ABILITY_ID::TRAIN_STALKER)):
            return UNIT_TYPEID::PROTOSS_STALKER;
        case (uint32_t(ABILITY_ID::TRAIN_TEMPEST)):
            return UNIT_TYPEID::PROTOSS_TEMPEST;
        case (uint32_t(ABILITY_ID::TRAIN_VOIDRAY)):
            return UNIT_TYPEID::PROTOSS_VOIDRAY;
        case (uint32_t(ABILITY_ID::TRAIN_WARPPRISM)):
            return UNIT_TYPEID::PROTOSS_WARPPRISM;
        case (uint32_t(ABILITY_ID::TRAIN_ZEALOT)):
            return UNIT_TYPEID::PROTOSS_ZEALOT;
        case (uint32_t(ABILITY_ID::TRAINWARP_ZEALOT)):
            return UNIT_TYPEID::PROTOSS_ZEALOT;
        case (uint32_t(ABILITY_ID::TRAINWARP_SENTRY)):
            return UNIT_TYPEID::PROTOSS_SENTRY;
        case (uint32_t(ABILITY_ID::TRAINWARP_STALKER)):
            return UNIT_TYPEID::PROTOSS_STALKER;
        case (uint32_t(ABILITY_ID::TRAINWARP_ADEPT)):
            return UNIT_TYPEID::PROTOSS_ADEPT;
        case (uint32_t(ABILITY_ID::TRAINWARP_DARKTEMPLAR)):
            return UNIT_TYPEID::PROTOSS_DARKTEMPLAR;
        case (uint32_t(ABILITY_ID::TRAINWARP_HIGHTEMPLAR)):
            return UNIT_TYPEID::PROTOSS_HIGHTEMPLAR;

        default:
            return 0;
    }
    return 0;
}

static AbilityID unitToBuildAbility(UnitTypeID unit_type) {
    switch (uint32_t(unit_type)) {
        case (uint32_t(UNIT_TYPEID::PROTOSS_ASSIMILATOR)):
            return ABILITY_ID::BUILD_ASSIMILATOR;
        case (uint32_t(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)):
            return ABILITY_ID::BUILD_CYBERNETICSCORE;
        case (uint32_t(UNIT_TYPEID::PROTOSS_DARKSHRINE)):
            return ABILITY_ID::BUILD_DARKSHRINE;
        case (uint32_t(UNIT_TYPEID::PROTOSS_FLEETBEACON)):
            return ABILITY_ID::BUILD_FLEETBEACON;
        case (uint32_t(UNIT_TYPEID::PROTOSS_FORGE)):
            return ABILITY_ID::BUILD_FORGE;
        case (uint32_t(UNIT_TYPEID::PROTOSS_GATEWAY)):
            return ABILITY_ID::BUILD_GATEWAY;
        case (uint32_t(UNIT_TYPEID::PROTOSS_NEXUS)):
            return ABILITY_ID::BUILD_NEXUS;
        case (uint32_t(UNIT_TYPEID::PROTOSS_PHOTONCANNON)):
            return ABILITY_ID::BUILD_PHOTONCANNON;
        case (uint32_t(UNIT_TYPEID::PROTOSS_PYLON)):
            return ABILITY_ID::BUILD_PYLON;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ROBOTICSBAY)):
            return ABILITY_ID::BUILD_ROBOTICSBAY;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)):
            return ABILITY_ID::BUILD_ROBOTICSFACILITY;
        case (uint32_t(UNIT_TYPEID::PROTOSS_SHIELDBATTERY)):
            return ABILITY_ID::BUILD_SHIELDBATTERY;
        case (uint32_t(UNIT_TYPEID::PROTOSS_STARGATE)):
            return ABILITY_ID::BUILD_STARGATE;
        case (uint32_t(UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE)):
            return ABILITY_ID::BUILD_TEMPLARARCHIVE;
        case (uint32_t(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)):
            return ABILITY_ID::BUILD_TWILIGHTCOUNCIL;
        default:
            return 0;
    }
    return 0;
}

static UpgradeID researchAbilityToUpgrade(AbilityID build_ability) {
    switch (uint32_t(build_ability)) {
        case (uint32_t(ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES)):
            return UPGRADE_ID::ADEPTKILLBOUNCE;
        case(uint32_t(ABILITY_ID::RESEARCH_BLINK)):
            return UPGRADE_ID::BLINKTECH;
        case(uint32_t(ABILITY_ID::RESEARCH_CHARGE)):
            return UPGRADE_ID::CHARGE;
        case(uint32_t(ABILITY_ID::RESEARCH_EXTENDEDTHERMALLANCE)):
            return UPGRADE_ID::EXTENDEDTHERMALLANCE;
        case(uint32_t(ABILITY_ID::RESEARCH_GRAVITICBOOSTER)):
            return UPGRADE_ID::OBSERVERGRAVITICBOOSTER;
        case(uint32_t(ABILITY_ID::RESEARCH_GRAVITICDRIVE)):
            return UPGRADE_ID::GRAVITICDRIVE;
        case (uint32_t(ABILITY_ID::RESEARCH_PROTOSSAIRARMOR)):
            return UPGRADE_ID::INVALID;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL1)):
            return UPGRADE_ID::PROTOSSAIRARMORSLEVEL1;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL2)):
            return UPGRADE_ID::PROTOSSAIRARMORSLEVEL2;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL3)):
            return UPGRADE_ID::PROTOSSAIRARMORSLEVEL3;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONS)):
            return UPGRADE_ID::INVALID;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL1)):
            return UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL2)):
            return UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL3)):
            return UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSGROUNDARMOR)):
            return UPGRADE_ID::INVALID;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL1)):
            return UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL2)):
            return UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL3)):
            return UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS)):
            return UPGRADE_ID::INVALID;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL1)):
            return UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL2)):
            return UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL3)):
            return UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSSHIELDS)):
            return UPGRADE_ID::INVALID;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL1)):
            return UPGRADE_ID::PROTOSSSHIELDSLEVEL1;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL2)):
            return UPGRADE_ID::PROTOSSSHIELDSLEVEL2;
        case(uint32_t(ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL3)):
            return UPGRADE_ID::PROTOSSSHIELDSLEVEL3;
        // case(uint32_t(ABILITY_ID::RESEARCH_PSIONICAMPLIFIERS)):
        //     return UPGRADE_ID::PSIONICAMPLIFIERS;
        case(uint32_t(ABILITY_ID::RESEARCH_PSISTORM)):
            return UPGRADE_ID::PSISTORMTECH;
        case(uint32_t(ABILITY_ID::RESEARCH_SHADOWSTRIKE)):
            return UPGRADE_ID::DARKTEMPLARBLINKUPGRADE;
        case(uint32_t(ABILITY_ID::RESEARCH_TEMPESTRANGEUPGRADE)):
            return UPGRADE_ID::TEMPESTRANGEUPGRADE;
        // case(uint32_t(ABILITY_ID::RESEARCH_TEMPESTRESEARCHGROUNDATTACKUPGRADE)):
        //     return UPGRADE_ID::TEMPESTGROUNDATTACKUPGRADE;  
        // case(uint32_t(ABILITY_ID::RESEARCH_VOIDRAYSPEEDUPGRADE)):
        //     return UPGRADE_ID::VOIDRAYSPEEDUPGRADE;
        case(uint32_t(ABILITY_ID::RESEARCH_WARPGATE)):
            return UPGRADE_ID::WARPGATERESEARCH;
    }
    return 0;
}

static Cost buildAbilityToCost(AbilityID build_ability, Agent *agent) {
    sc2::UnitTypeData unit_stats =
        agent->Observation()->GetUnitTypeData().at(static_cast<uint32_t>(buildAbilityToUnit(build_ability)));
    return {unit_stats.mineral_cost, unit_stats.vespene_cost, 0, int(unit_stats.food_required)};
}

static Cost UpgradeToCost(AbilityID research_ability, Agent *agent) {
    UpgradeData upgrade_stats =
        agent->Observation()->GetUpgradeData().at(static_cast<uint32_t>(researchAbilityToUpgrade(research_ability)));
    return {upgrade_stats.mineral_cost, upgrade_stats.vespene_cost, 0};
}

static Cost abilityToCost(AbilityID ability, Agent *agent) {
    if (buildAbilityToUnit(ability) != 0) {
        return buildAbilityToCost(ability, agent);
    } else if (researchAbilityToUpgrade(ability)) {
        return UpgradeToCost(ability, agent);
    }
    return {0, 0, 0, 0};
}

static int structureDiameter(UnitTypeID type) {
    switch (uint32_t(type)) {
        case (uint32_t(UNIT_TYPEID::PROTOSS_PYLON)):
            return 2;
        case (uint32_t(UNIT_TYPEID::PROTOSS_GATEWAY)):
            return 3;
        case (uint32_t(UNIT_TYPEID::PROTOSS_NEXUS)):
            return 5;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ASSIMILATOR)):
            return 3;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)):
            return 3;
        case (uint32_t(UNIT_TYPEID::PROTOSS_WARPGATE)):
            return 3;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ASSIMILATORRICH)):
            return 3;
        case (uint32_t(UNIT_TYPEID::PROTOSS_FORGE)):
            return 3;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ROBOTICSBAY)):
            return 3;
        case (uint32_t(UNIT_TYPEID::PROTOSS_SHIELDBATTERY)):
            return 2;
        case (uint32_t(UNIT_TYPEID::PROTOSS_DARKSHRINE)):
            return 2;
        case (uint32_t(UNIT_TYPEID::PROTOSS_PHOTONCANNON)):
            return 2;
        case (uint32_t(UNIT_TYPEID::PROTOSS_STARGATE)):
            return 3;
        case (uint32_t(UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE)):
            return 3;
        case (uint32_t(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)):
            return 3;
        case (uint32_t(UNIT_TYPEID::NEUTRAL_DESTRUCTIBLECITYDEBRIS6X6)):
            return 6;
        case (uint32_t(UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRIS6X6)):
            return 6;
        case (uint32_t(UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEROCK6X6)):
            return 6;
        case (uint32_t(UNIT_TYPEID::NEUTRAL_UNBUILDABLEBRICKSDESTRUCTIBLE)):
            return 2;
        case (uint32_t(UNIT_TYPEID::NEUTRAL_UNBUILDABLEPLATESDESTRUCTIBLE)):
            return 2;
        case (uint32_t(UNIT_TYPEID::UNBUILDABLEROCKSDESTRUCTIBLE)):
            return 2;
        default:
            return 0;
    }

    /*PROTOSS_ASSIMILATOR = 61,
    PROTOSS_ASSIMILATORRICH = 1994, PROTOSS_CYBERNETICSCORE = 72,
    PROTOSS_DARKSHRINE = 69,
    PROTOSS_FLEETBEACON = 64, PROTOSS_FORGE = 63, PROTOSS_GATEWAY = 62,
    PROTOSS_NEXUS = 59, PROTOSS_PHOTONCANNON = 66,
    PROTOSS_PYLON = 60, PROTOSS_ROBOTICSBAY = 70, PROTOSS_ROBOTICSFACILITY = 71,
    PROTOSS_SHIELDBATTERY = 1910, PROTOSS_STARGATE = 67,
    PROTOSS_TEMPLARARCHIVE = 68, PROTOSS_TWILIGHTCOUNCIL = 65,
    PROTOSS_WARPGATE = 133,*/
}

static bool requiresPylon(AbilityID build_ability) {
    switch (uint32_t(build_ability)) {
        case (uint32_t(ABILITY_ID::BUILD_ASSIMILATOR)):  //FALSE
            return false;
        case (uint32_t(ABILITY_ID::BUILD_CYBERNETICSCORE)):
            return true;
        case (uint32_t(ABILITY_ID::BUILD_DARKSHRINE)):
            return true;
        case (uint32_t(ABILITY_ID::BUILD_FLEETBEACON)):
            return true;
        case (uint32_t(ABILITY_ID::BUILD_FORGE)):
            return true;
        case (uint32_t(ABILITY_ID::BUILD_GATEWAY)):
            return true;
        case (uint32_t(ABILITY_ID::BUILD_NEXUS)):  // FALSE
            return false;
        case (uint32_t(ABILITY_ID::BUILD_PHOTONCANNON)):
            return true;
        case (uint32_t(ABILITY_ID::BUILD_PYLON)):  // FALSE
            return false;
        case (uint32_t(ABILITY_ID::BUILD_ROBOTICSBAY)):
            return true;
        case (uint32_t(ABILITY_ID::BUILD_ROBOTICSFACILITY)):
            return true;
        case (uint32_t(ABILITY_ID::BUILD_SHIELDBATTERY)):
            return true;
        case (uint32_t(ABILITY_ID::BUILD_STARGATE)):
            return true;
        case (uint32_t(ABILITY_ID::BUILD_TEMPLARARCHIVE)):
            return true;
        case (uint32_t(ABILITY_ID::BUILD_TWILIGHTCOUNCIL)):
            return true;
        case (uint32_t(ABILITY_ID::TRAINWARP_ADEPT)):
            return true;
        case (uint32_t(ABILITY_ID::TRAINWARP_DARKTEMPLAR)):
            return true;
        case (uint32_t(ABILITY_ID::TRAINWARP_HIGHTEMPLAR)):
            return true;
        case (uint32_t(ABILITY_ID::TRAINWARP_SENTRY)):
            return true;
        case (uint32_t(ABILITY_ID::TRAINWARP_STALKER)):
            return true;
        case (uint32_t(ABILITY_ID::TRAINWARP_ZEALOT)):
            return true;
        default:
            return false;
    }
    return 0;
}

static int theorySupply(Agent *agent) {
    Units nexi = agent->Observation()->GetUnits(Unit::Alliance::Self, Aux::isNexus);
    Units pylons = agent->Observation()->GetUnits(Unit::Alliance::Self, Aux::isPylon);
    return int(nexi.size() * 15) + (pylons.size() * 8);
}

bool addPlacement(Point2D p, int size) {
    int x = p.x - (size / 2) + DISP;
    int y = p.y - (size / 2) + DISP;
    for (int i = x; i < x + size; i++) {
        for (int j = y; j < y + size; j++) {
            // printf("{%d,%d}", i, j);
            imRef(buildingBlocked, i, j) = 1;
        }
    }
    return true;
}

bool addPlacement(Point2D p, UnitTypeID unit_type) {
    return addPlacement(p, structureDiameter(unit_type));
}

bool removePlacement(Point2D p, int size) {
    int x = p.x - (size / 2) + DISP;
    int y = p.y - (size / 2) + DISP;
    for (int i = x; i < x + size; i++) {
        for (int j = y; j < y + size; j++) {
            imRef(buildingBlocked, i, j) = 0;
        }
    }
    return true;
}

bool removePlacement(Point2D p, UnitTypeID unit_type) {
    return removePlacement(p, structureDiameter(unit_type));
}

bool checkPlacement(Point2D p, int size) {
    int x = p.x - (size / 2) + DISP;
    int y = p.y - (size / 2) + DISP;
    for (int i = x; i < x + size; i++) {
        for (int j = y; j < y + size; j++) {
            //Point2D check(i, j);
            if (imRef(buildingBlocked, i, j) > 0) {
                return false;
            }
        }
    }
    return true;
}

bool checkPlacementFull(Point2D p, int size, Agent *agent) {
    int x = p.x - (size / 2) + DISP;
    int y = p.y - (size / 2) + DISP;
    for (int i = x; i < x + size; i++) {
        for (int j = y; j < y + size; j++) {
            // Point2D check(i, j);
            //printf("CHECK: %d,%d BB%d IP%d\n", i, j, imRef(buildingBlocked, i, j),
            //       !agent->Observation()->IsPlacable(Point2D{(float)i + 0.5F, (float)j + 0.5F}));
            if (imRef(buildingBlocked, i, j) > 0 ||
                !agent->Observation()->IsPlacable(Point2D{(float)i + 0.5F, (float)j + 0.5F})) {
                return false;
            }
        }
    }
    return true;
}

bool isFlyingTargetType(UnitTypeID unit_type) {
    switch (uint32_t(unit_type)) {
        case (uint32_t(UNIT_TYPEID::PROTOSS_ASSIMILATOR)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_DARKSHRINE)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_FLEETBEACON)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_FORGE)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_GATEWAY)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_NEXUS)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_PHOTONCANNON)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_PYLON)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ROBOTICSBAY)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_SHIELDBATTERY)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_STARGATE)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ADEPT)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ARCHON)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_CARRIER)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_COLOSSUS)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_DARKTEMPLAR)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_DISRUPTOR)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_IMMORTAL)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_MOTHERSHIP)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_OBSERVER)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ORACLE)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_PHOENIX)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_PROBE)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_SENTRY)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_STALKER)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_TEMPEST)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_VOIDRAY)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_WARPPRISM)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ZEALOT)):
            return false;
        default:
            printf("ISFLYINGTARGET ERROR\n");
            return false;
    }
}

bool isGroundTargetType(UnitTypeID unit_type) {
    switch (uint32_t(unit_type)) {
        case (uint32_t(UNIT_TYPEID::PROTOSS_ASSIMILATOR)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_DARKSHRINE)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_FLEETBEACON)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_FORGE)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_GATEWAY)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_NEXUS)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_PHOTONCANNON)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_PYLON)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ROBOTICSBAY)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_SHIELDBATTERY)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_STARGATE)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ADEPT)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ARCHON)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_CARRIER)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_COLOSSUS)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_DARKTEMPLAR)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_DISRUPTOR)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_IMMORTAL)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_MOTHERSHIP)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_OBSERVER)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ORACLE)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_PHOENIX)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_PROBE)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_SENTRY)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_STALKER)):
            return true;
        case (uint32_t(UNIT_TYPEID::PROTOSS_TEMPEST)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_VOIDRAY)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_WARPPRISM)):
            return false;
        case (uint32_t(UNIT_TYPEID::PROTOSS_ZEALOT)):
            return true;
        default:
            printf("ISGROUNDTARGET ERROR\n");
            return false;
    }
}

bool isFlyingTarget(const Unit* unit) {
    return unit->unit_type == UNIT_TYPEID::PROTOSS_COLOSSUS || unit->is_flying;
}
    
bool isGroundTarget(const Unit *unit) {
    return !unit->is_flying;
}

Composition unitTypeAsTarget(UnitTypeID unit_type) {
    if (isFlyingTargetType(unit_type)) {
        if (isGroundTargetType(unit_type)) {
            Composition::Any;
        } else {
            Composition::Air;
        }
    } else {
        if (isGroundTargetType(unit_type)) {
            Composition::Ground;
        } else {
            Composition::Invalid;
        }
    }
    return Composition::Invalid;
}

Composition unitAsTarget(const Unit *unit) {
    if (isFlyingTarget(unit)) {
        if (isGroundTarget(unit)) {
            return Composition::Any;
        } else {
            return Composition::Air;
        }
    } else {
        if (isGroundTarget(unit)) {
            return Composition::Ground;
        } else {
            return Composition::Invalid;
        }
    }
    return Composition::Invalid;
}

//bool hitsUnit(const Unit *target, Composition weapon) {
//    Composition unit_comp = unitAsTarget(target);
//    if (weapon == Composition::Invalid || unit_comp == Composition::Invalid) {
//        printf("HITCHECK ERROR %d %d\n", weapon, unit_comp);
//        return false;
//    }
//    if (weapon == Composition::Any || unit_comp == Composition::Any) {
//        return true;
//    }
//    if ((weapon == Composition::Air && unit_comp == Composition::Air) ||
//        (weapon == Composition::Ground && unit_comp == Composition::Ground)) {
//        return true;
//    }
//    return false;
//}

//bool hitsUnit(Composition army, Composition weapon) {
//    if (weapon == Composition::Invalid || army == Composition::Invalid) {
//        printf("HITCHECK ARMY ERROR %d %d\n", weapon, army);
//        return false;
//    }
//    if (weapon == Composition::Any || army == Composition::Any) {
//        return true;
//    }
//    if ((weapon == Composition::Air && army == Composition::Air) ||
//        (weapon == Composition::Ground && army == Composition::Ground)) {
//        return true;
//    }
//    return false;
//}

Point3D addKeepZ(Point3D &a, Point2D &b) {
    return {a.x + b.x, a.y + b.y, a.z};
}

}  // namespace Aux

struct Building {
    AbilityID build;
    Point2D pos;

    Cost cost(Agent *agent) {
        return Aux::buildAbilityToCost(build, agent);
    }
};

constexpr float timeSpeed = 1.4;
constexpr float fps = 16 * timeSpeed;

//BUILDINGS
constexpr int COST_NEXUS[2] = {400, 0};
constexpr int COST_ASSIMILATOR[2] = {75, 0};
constexpr int COST_PYLON[2] = {100, 0};
constexpr int COST_GATEWAY[2] = {150, 0};
constexpr int COST_FORGE[2] = {150, 0};
constexpr int COST_CYBERNETICS_CORE[2] = {150, 0};
constexpr int COST_PHOTON_CANNON[2] = {150, 0};
constexpr int COST_SHIELD_BATTERY[2] = {100, 0};

constexpr int COST_TWILIGHT_COUNCIL[2] = {150, 100};
constexpr int COST_STARGATE[2] = {150, 150};
constexpr int COST_ROBOTICS_FACILITY[2] = {150, 100};

constexpr int COST_TEMPLAR_ARCHIVES[2] = {150, 200};
constexpr int COST_DARK_SHRINE[2] = {150, 150};
constexpr int COST_FLEET_BEACON[2] = {300, 200};
constexpr int COST_ROBOTICS_BAY[2] = {150, 150};

//UNITS
constexpr int COST_PROBE[2] = {50, 0};
constexpr int COST_ZEALOT[2] = {100, 0};
constexpr int COST_SENTRY[2] = {50, 100};
constexpr int COST_STALKER[2] = {125, 50};
constexpr int COST_ADEPT[2] = {100, 25};

constexpr int COST_PHEONIX[2] = {150, 100};
constexpr int COST_ORACLE[2] = {150, 150};
constexpr int COST_VOID_RAY[2] = {250, 150};
constexpr int COST_TEMPEST[2] = {250, 175};
constexpr int COST_CARRIER[2] = {350, 250};

constexpr int COST_MOTHERSHIP[2] = {300, 300};

constexpr int COST_OBSERVER[2] = {25, 75};
constexpr int COST_WARP_PRISM[2] = {250, 0};
constexpr int COST_IMMORTAL[2] = {275, 100};
constexpr int COST_COLOSSUS[2] = {300, 200};
constexpr int COST_DISRUPTOR[2] = {150, 150};

constexpr int COST_HIGH_TEMPLAR[2] = {50, 150};
constexpr int COST_DARK_TEMPLAR[2] = {125, 125};

//UPGRADES
constexpr int COST_UPGRADE_AIR_WEAPON_1[2] = {100, 100};
constexpr int COST_UPGRADE_AIR_WEAPON_2[2] = {175, 175};
constexpr int COST_UPGRADE_AIR_WEAPON_3[2] = {250, 250};

constexpr int COST_UPGRADE_AIR_ARMOR_1[2] = {100, 100};
constexpr int COST_UPGRADE_AIR_ARMOR_2[2] = {175, 175};
constexpr int COST_UPGRADE_AIR_ARMOR_3[2] = {250, 250};

constexpr int COST_UPGRADE_WARP_GATE[2] = {50, 50};

constexpr int COST_UPGRADE_GROUND_WEAPON_1[2] = {100, 100};
constexpr int COST_UPGRADE_GROUND_WEAPON_2[2] = {150, 150};
constexpr int COST_UPGRADE_GROUND_WEAPON_3[2] = {200, 200};

constexpr int COST_UPGRADE_GROUND_ARMOR_1[2] = {100, 100};
constexpr int COST_UPGRADE_GROUND_ARMOR_2[2] = {150, 150};
constexpr int COST_UPGRADE_GROUND_ARMOR_3[2] = {200, 200};

constexpr int COST_UPGRADE_SHIELDS_1[2] = {150, 150};
constexpr int COST_UPGRADE_SHIELDS_2[2] = {200, 200};
constexpr int COST_UPGRADE_SHIELDS_3[2] = {250, 250};

constexpr int COST_UPGRADE_ZEALOT_CHARGE[2] = {100, 100};
constexpr int COST_UPGRADE_STALKER_BLINK[2] = {150, 150};
constexpr int COST_UPGRADE_ADEPT_GLAIVES[2] = {100, 100};

constexpr int COST_UPGRADE_HT_STORM[2] = {200, 200};
constexpr int COST_UPGRADE_DT_BLINK[2] = {100, 100};

constexpr int COST_UPGRADE_PHEONIX_RANGE[2] = {150, 150};
constexpr int COST_UPGRADE_VOID_RAY_SPEED[2] = {100, 100};
constexpr int COST_UPGRADE_TEMPEST_DAMAGE[2] = {150, 150};

constexpr int COST_UPGRADE_OBSERVER_SPEED[2] = {100, 100};
constexpr int COST_UPGRADE_WARP_PRISM_SPEED[2] = {100, 100};
constexpr int COST_UPGRADE_COLOSSUS_RANGE[2] = {150, 150};
