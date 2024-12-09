#pragma once
#include <sc2api/sc2_api.h>
#include <map>
#include "map2dFloat.hpp"

using namespace sc2;
using namespace std;

class UnitWrapper {
public:
    Tag self;
    UnitTypeID type;
    Point3D lastPos;
    float radius;
    Unit::Alliance team;
    AvailableAbilities abilities;
    int8_t ignoreFrames;

    //UnitWrapper(Tag self_);

    //UnitWrapper(Tag self_, UnitTypeID id);

    UnitWrapper(const Unit* unit);

    bool equals(UnitWrapper *wrapper);

    inline bool exists(Agent *agent);

    inline const Unit *get(Agent *agent);

    Point2D pos(Agent *agent);
    Point3D pos3D(Agent *agent);

    virtual bool execute(Agent *agent);

    virtual bool executeDamaged(Agent *agent, float health, float shields) {
        return false;
    }

    static void loadAbilities(Agent *agent);

    static void loadAbilitiesEnemy(Agent* agent);

    bool checkAbility(AbilityID ability) {
        for (int i = 0; i < abilities.abilities.size(); i++) {
            if (ability == abilities.abilities[i].ability_id) {
                return true;
            }
        }
        return false;
    }

    bool operator==(const UnitWrapper &u) {
        return u.self == self;
    }

    bool operator<(const UnitWrapper &u) {
        return u.self < self;
    }

    virtual ~UnitWrapper();
};

class UnitWrapperHash {
public:
    // We use predefined hash functions of strings
    // and define our hash function as XOR of the
    // hash values.
    size_t operator()(const UnitWrapper &p) const {
        return p.self;
    }
};

using UnitWrappers = vector<UnitWrapper*>;
using UnitWrapperSet = set<UnitWrapper*>;
using damageval = uint16_t;

struct Damage {
    damageval normal;
    damageval armored;
    damageval light;
    damageval biological;
    damageval mechanical;
    damageval massive;
    damageval psionic;

    void operator+=(const Damage& u) {
        normal += u.normal;
        armored += u.armored;
        light += u.light;
        biological += u.biological;
        mechanical += u.mechanical;
        massive += u.massive;
        psionic += u.psionic;
    }

    Damage operator+(const Damage& u) {
        return Damage{
            (damageval)(normal + u.normal),
            (damageval)(armored + u.armored),
            (damageval)(light + u.light),
            (damageval)(biological + u.biological),
            (damageval)(mechanical + u.mechanical),
            (damageval)(massive + u.massive), 
            (damageval)(psionic + u.psionic) };
    }
};

struct DamageLocation {
    Damage ground;
    
    Damage air;

    void operator+=(const DamageLocation& u) {
        ground += u.ground;
        air += u.air;
    }

    DamageLocation operator+(const DamageLocation& u) {
        return DamageLocation{
            (ground + u.ground),
            (air + u.air) 
        };
    }
};

namespace UnitManager {
    map<UnitTypeID, UnitWrappers> units;
    map<UnitTypeID, UnitWrappers> neutrals;
    map<UnitTypeID, UnitWrappers> enemies;

    constexpr int damageNetPrecision = 5;
    #define blockSize 1.0F/UnitManager::damageNetPrecision

    map2d<DamageLocation>* enemyDamageNet;
    map2d<int8_t>* enemyDamageNetModify;

    //map2d<int16_t>* enemyDamageNetGround;
    //map2d<int16_t>* enemyDamageNetGroundLight;
    //map2d<int16_t>* enemyDamageNetGroundArmored;
    //map2d<int16_t>* enemyDamageNetAir;
    //map2d<int16_t>* enemyDamageNetAirLight;
    //map2d<int16_t>* enemyDamageNetAirArmored;

    #define damageNetEnemy(p) imRef(UnitManager::enemyDamageNet, int(p.x * UnitManager::damageNetPrecision), int(p.y * UnitManager::damageNetPrecision))

    //DamageLocation getDamage(Point2D p) {
    //    int16_t g = imRef(enemyDamageNetGround, int(p.x * damageNetPrecision), int(p.y * damageNetPrecision));
    //    int16_t gl = imRef(enemyDamageNetGroundLight, int(p.x * damageNetPrecision), int(p.y * damageNetPrecision));
    //    int16_t ga = imRef(enemyDamageNetGroundArmored, int(p.x * damageNetPrecision), int(p.y * damageNetPrecision));
    //    int16_t a = imRef(enemyDamageNetAir, int(p.x * damageNetPrecision), int(p.y * damageNetPrecision));
    //    int16_t al = imRef(enemyDamageNetAirLight, int(p.x * damageNetPrecision), int(p.y * damageNetPrecision));
    //    int16_t aa = imRef(enemyDamageNetAirArmored, int(p.x * damageNetPrecision), int(p.y * damageNetPrecision));
    //    return DamageLocation{ g,gl,ga,a,al,aa };
    //}

    //DamageLocation getDamageRaw(int i, int j) {
    //    int16_t g = imRef(enemyDamageNetGround, i, j);
    //    int16_t gl = imRef(enemyDamageNetGroundLight, i, j);
    //    int16_t ga = imRef(enemyDamageNetGroundArmored, i, j);
    //    int16_t a = imRef(enemyDamageNetAir, i, j);
    //    int16_t al = imRef(enemyDamageNetAirLight, i, j);
    //    int16_t aa = imRef(enemyDamageNetAirArmored, i, j);
    //    return DamageLocation{ g,gl,ga,a,al,aa };
    //}

    //void setDamage(Point2D p, DamageLocation damage) {
    //    imRef(enemyDamageNetGround, int(p.x * damageNetPrecision), int(p.y * damageNetPrecision)) = damage.ground;
    //    imRef(enemyDamageNetGroundLight, int(p.x * damageNetPrecision), int(p.y * damageNetPrecision)) = damage.groundlight;
    //    imRef(enemyDamageNetGroundArmored, int(p.x * damageNetPrecision), int(p.y * damageNetPrecision)) = damage.groundarmored;
    //    imRef(enemyDamageNetAir, int(p.x * damageNetPrecision), int(p.y * damageNetPrecision)) = damage.air;
    //    imRef(enemyDamageNetAirLight, int(p.x * damageNetPrecision), int(p.y * damageNetPrecision)) = damage.airlight;
    //    imRef(enemyDamageNetAirArmored, int(p.x * damageNetPrecision), int(p.y * damageNetPrecision)) = damage.airarmored;
    //}

    //void setDamageRaw(int i, int j, DamageLocation damage) {
    //    imRef(enemyDamageNetGround, i, j) = damage.ground;
    //    imRef(enemyDamageNetGroundLight, i, j) = damage.groundlight;
    //    imRef(enemyDamageNetGroundArmored, i, j) = damage.groundarmored;
    //    imRef(enemyDamageNetAir, i, j) = damage.air;
    //    imRef(enemyDamageNetAirLight, i, j) = damage.airlight;
    //    imRef(enemyDamageNetAirArmored, i, j) = damage.airarmored;
    //}

    void setEnemyDamageRadius2(Point2D pos, float radius, DamageLocation damage, Agent* agent) {
        int x = (pos.x - radius) * damageNetPrecision;
        int y = (pos.y - radius) * damageNetPrecision;
        int xmax = (pos.x + radius) * damageNetPrecision + 1;
        int ymax = (pos.y + radius) * damageNetPrecision + 1;
        //printf("%d - %d, %d - %d\n", x, xmax, y, ymax);
        for (int i = x; i <= xmax; i++) {
            for (int j = y; j <= ymax; j++) {
                //agent->Debug()->DebugLineOut(Point3D{ (float)(i) / damageNetPrecision, (float)(j) / damageNetPrecision, 0.0F }, Point3D{ (float)(i) / damageNetPrecision, (float)(j) / damageNetPrecision, 13.0F });
                bool activate = false;
                for (float a = 0; a <= 1; a += 0.25) {
                    for (float b = 0; b <= 1; b += 0.25) {
                        Point2D temp{ i + a, j + b };
                        if (Distance2D(pos * damageNetPrecision, temp) < radius * damageNetPrecision) {
                            activate = true;
                            break;
                        }
                    }
                    if (activate) {
                        break;
                    }
                }
                if (activate) {
                    if (i > 1 && j > 1) {
                        DamageLocation d = imRef(enemyDamageNet, i - 1, j - 1);
                        //printf("pre %.1f,%.1f  %.1f,%.1f  %.1f,%.1f\n", d.ground, d.air, d.groundlight, d.airlight, d.groundarmored, d.airarmored);
                        imRef(enemyDamageNet, i - 1, j - 1) += damage;
                        d = imRef(enemyDamageNet, i - 1, j - 1);
                        //printf("post %.1f,%.1f  %.1f,%.1f  %.1f,%.1f\n", d.ground, d.air, d.groundlight, d.airlight, d.groundarmored, d.airarmored);
                    }
                    //DamageLocation();
                    //DamageLocation d = getDamageRaw(i - 1, j - 1) + damage;
                    //setDamageRaw( i-1, j-1, d);
                }
            }
        }
    }

    void setEnemyDamageRadius(Point2D pos, float radius, DamageLocation damage, Agent* agent) {
        //printf("5ize:%d\n", sizeof(DamageLocation));
        enemyDamageNetModify->clear();
        int x = (pos.x - radius) * damageNetPrecision;
        int y = (pos.y - radius) * damageNetPrecision;
        int xmax = (pos.x + radius) * damageNetPrecision;
        int ymax = (pos.y + radius) * damageNetPrecision;
        //printf("%d - %d, %d - %d\n", x, xmax, y, ymax);
        imRef(enemyDamageNetModify, int(pos.x * damageNetPrecision), int(pos.y * damageNetPrecision)) = 1;
        for (int i = x; i <= xmax; i++) {
            for (int j = y; j <= ymax; j++) {
                //agent->Debug()->DebugLineOut(Point3D{ (float)(i) / damageNetPrecision, (float)(j) / damageNetPrecision, 0.0F }, Point3D{ (float)(i) / damageNetPrecision, (float)(j) / damageNetPrecision, 13.0F });
                float f = Distance2D(pos * damageNetPrecision, Point2D{ (float)i,(float)j });
                if (i > 1 && i < enemyDamageNetModify->width() && j > 1 && j < enemyDamageNetModify->height() && f < radius * damageNetPrecision) {
                    imRef(enemyDamageNetModify, i, j) = 1;
                    imRef(enemyDamageNetModify, i, j - 1) = 1;
                    imRef(enemyDamageNetModify, i - 1, j) = 1;
                    imRef(enemyDamageNetModify, i - 1, j - 1) = 1;
                }
            }
        }

        for (int i = x-1; i <= xmax+1; i++) {
            for (int j = y-1; j <= ymax+1; j++) {
                //agent->Debug()->DebugLineOut(Point3D{ (float)(i) / damageNetPrecision, (float)(j) / damageNetPrecision, 0.0F }, Point3D{ (float)(i) / damageNetPrecision, (float)(j) / damageNetPrecision, 13.0F });
                if (i > 1 && i < enemyDamageNetModify->width() && j > 1 && j < enemyDamageNetModify->height() && imRef(enemyDamageNetModify, i, j)) {
                    imRef(enemyDamageNet, i, j) += damage;
                }
            }
        }
    }

    bool checkExist(UnitTypeID id) {
        return units.find(id) != units.end();
    }

    UnitWrappers get(UnitTypeID type) {
        if (checkExist(type)) {
            return units[type];
        }
        return UnitWrappers();
    }

    UnitWrapper* find(UnitTypeID type, Tag tag) {
        UnitWrappers v = get(type);
        for (int i = 0; i < v.size(); i++) {
            if (v[i]->self == tag) {
                return v[i];
            }
        }
        return nullptr;
    }

    bool checkExistNeutral(UnitTypeID id) {
        return neutrals.find(id) != neutrals.end();
    }

    UnitWrappers getNeutral(UnitTypeID type) {
        if (checkExistNeutral(type)) {
            return neutrals[type];
        }
        return UnitWrappers();
    }

    UnitWrappers getVespene() {
        UnitWrappers v1 = getNeutral(UNIT_TYPEID::NEUTRAL_VESPENEGEYSER);
        UnitWrappers v2 = getNeutral(UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER);
        UnitWrappers v3 = getNeutral(UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER);
        UnitWrappers v4 = getNeutral(UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER);
        UnitWrappers v5 = getNeutral(UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER);
        UnitWrappers v6 = getNeutral(UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER);
        v1.insert(v1.end(), v2.begin(), v2.end());
        v1.insert(v1.end(), v3.begin(), v3.end());
        v1.insert(v1.end(), v4.begin(), v4.end());
        v1.insert(v1.end(), v5.begin(), v5.end());
        v1.insert(v1.end(), v6.begin(), v6.end());
        return v1;
    }

    UnitWrappers getMinerals() {
        UnitWrappers v1 = getNeutral(UNIT_TYPEID::NEUTRAL_MINERALFIELD);
        UnitWrappers v2 = getNeutral(UNIT_TYPEID::NEUTRAL_LABMINERALFIELD);
        UnitWrappers v3 = getNeutral(UNIT_TYPEID::NEUTRAL_MINERALFIELD750);
        UnitWrappers v4 = getNeutral(UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750);
        UnitWrappers v5 = getNeutral(UNIT_TYPEID::NEUTRAL_MINERALFIELD450);
        v1.insert(v1.end(), v2.begin(), v2.end());
        v1.insert(v1.end(), v3.begin(), v3.end());
        v1.insert(v1.end(), v4.begin(), v4.end());
        v1.insert(v1.end(), v5.begin(), v5.end());
        return v1;
    }

    UnitWrapper *findNeutral(UnitTypeID type, Tag tag) {
        UnitWrappers v = getNeutral(type);
        for (int i = 0; i < v.size(); i++) {
            if (v[i]->self == tag) {
                return v[i];
            }
        }
        return nullptr;
    }

    bool checkExistEnemy(UnitTypeID id) {
        return enemies.find(id) != enemies.end();
    }

    UnitWrappers getEnemy(UnitTypeID type) {
        if (checkExistEnemy(type)) {
            return enemies[type];
        }
        return UnitWrappers();
    }

    UnitWrapper *findEnemy(UnitTypeID type, Tag tag) {
        UnitWrappers v = getEnemy(type);
        for (int i = 0; i < v.size(); i++) {
            if (v[i]->self == tag) {
                return v[i];
            }
        }
        return nullptr;
    }
}  // namespace UnitManager


//UnitWrapper::UnitWrapper(Tag self_) : self(self_), type(UNIT_TYPEID::INVALID), lastPos{0, 0} {
//    printf("WHY ARE YOU USING THE DEFAULT UNITWRAPPER CONSTRUCTOR\n");
//}

//UnitWrapper::UnitWrapper(Tag self_, UnitTypeID type_)
//    : self(self_), type(type_), radius(0){
//    if (!UnitManager::checkExist(type)) {
//            UnitManager::units[type] = UnitWrappers();
//    }
//    UnitManager::units[type].push_back(this);
//}

UnitWrapper::UnitWrapper(const Unit *unit) : self(unit->tag), type(unit->unit_type), lastPos{0, 0, 0}, radius(unit->radius), team(unit->alliance) {
    if (unit->alliance == Unit::Alliance::Self) {
        if (!UnitManager::checkExist(type)) {
            UnitManager::units[type] = UnitWrappers();
        }
        UnitManager::units[type].push_back(this);
        //if (std::find(UnitManager::units[type].begin(), UnitManager::units[type].end(), this) == UnitManager::units[type].end()) {
        //    UnitManager::units[type].push_back(this);
        //}
        //else {
        //    printf("DuplicateUnit\n");
        //}
    }else if (unit->alliance == Unit::Alliance::Neutral) {
        if (!UnitManager::checkExistNeutral(type)) {
            UnitManager::neutrals[type] = UnitWrappers();
        }
        UnitManager::neutrals[type].push_back(this);
        //if (std::find(UnitManager::neutrals[type].begin(), UnitManager::neutrals[type].end(), this) == UnitManager::neutrals[type].end()) {
        //    UnitManager::neutrals[type].push_back(this);
        //}
        //else {
        //    printf("DuplicateNeutral\n");
        //}
    }else if (unit->alliance == Unit::Alliance::Enemy) {
        if (!UnitManager::checkExistEnemy(type)) {
            UnitManager::enemies[type] = UnitWrappers();
        }
        //UnitManager::enemies[type].push_back(this);
        bool found = false;
        for (UnitWrapper* wrap : UnitManager::enemies[type]) {
            if (unit->tag == wrap->self) {
                found = true;
                break;
            }
        }
        if (found) {
            printf("DuplicateEnemy\n");
        } else {
            UnitManager::enemies[type].push_back(this);
        }
        //if (std::find(UnitManager::enemies[type].begin(), UnitManager::enemies[type].end(), this) == UnitManager::enemies[type].end()) {
        //    UnitManager::enemies[type].push_back(this);
        //}
        //else {
        //    printf("DuplicateEnemy\n");
        //}
    }
    ignoreFrames = 0;
    abilities = AvailableAbilities();
}

Point2D UnitWrapper::pos(Agent *agent) {
    const Unit *unit = agent->Observation()->GetUnit(self);
    if (unit != nullptr) {
        lastPos = unit->pos;
    } else if (agent->Observation()->GetVisibility(lastPos) == Visibility::Visible) {
        lastPos = {0, 0, 0};
    }
    return lastPos;
}

Point3D UnitWrapper::pos3D(Agent *agent) {
    const Unit *unit = agent->Observation()->GetUnit(self);
    if (unit != nullptr) {
        lastPos = unit->pos;
    } else if (agent->Observation()->GetVisibility(lastPos) == Visibility::Visible) {
        lastPos = {0, 0, 0};
    }
    return lastPos;
}

inline bool UnitWrapper::exists(Agent *agent) {
    return agent->Observation()->GetUnit(self) == nullptr;
}

inline const Unit* UnitWrapper::get(Agent *agent) {
    return agent->Observation()->GetUnit(self);
}

bool UnitWrapper::equals(UnitWrapper *wrapper) {
    return (wrapper->self == self) && (wrapper->type == type);
}

void UnitWrapper::loadAbilities(Agent *agent) {
    Units all = agent->Observation()->GetUnits(Unit::Alliance::Self);
    //Units u;
    //vector<UnitWrapper *> probes = UnitManager::get(UNIT_TYPEID::PROTOSS_PROBE);
    // for (int i = 0; i < probes.size(); i++) {
    //     const Unit *unit = agent->Observation()->GetUnit(probes[i]->self);
    //     if (unit != nullptr) {
    //         u.push_back(unit);
    //     } else {
    //         probes.erase(probes.begin() + i);
    //         i --;
    //     }
    // }
    //vector<AvailableAbilities> allAb = agent->Query()->GetAbilitiesForUnits(u);
    //for (int i = 0; i < allAb.size(); i++) {
    //    if (probes[i]->self == allAb[i].unit_tag) {
    //        ((Probe *)probes[i])->abilities = allAb[i];
    //    } else {
    //        printf("ABILITY ASSIGNMENT ERROR\n");
    //    }
    //}
    vector<AvailableAbilities> allAb = agent->Query()->GetAbilitiesForUnits(all);
    for (AvailableAbilities abil : allAb) {
        if (abil.unit_tag != NullTag) {
            UnitWrapper* u = UnitManager::find(abil.unit_type_id, abil.unit_tag);
            if (u == nullptr) {
                continue;
            }
            u->abilities = abil;
        }
            
    }
}

void UnitWrapper::loadAbilitiesEnemy(Agent* agent) {
    Units all = agent->Observation()->GetUnits(Unit::Alliance::Enemy);
    vector<AvailableAbilities> allAb = agent->Query()->GetAbilitiesForUnits(all);
    for (AvailableAbilities abil : allAb) {
        if (abil.unit_tag != NullTag) {
            UnitWrapper* u = UnitManager::findEnemy(abil.unit_type_id, abil.unit_tag);
            if (u == nullptr) {
                continue;
            }
            u->abilities = abil;
            if (abil.abilities.size() != 0) {
                printf("ABIL:%d %s\n", abil.abilities.size(), UnitTypeToName(abil.unit_tag));
                for (auto a : abil.abilities) {
                    printf("%s\n", AbilityTypeToName(a.ability_id));
                }
            }
        }

    }
}

UnitWrapper::~UnitWrapper() {
    if (team == Unit::Alliance::Self) {
        for (auto it = UnitManager::units[type].begin(); it != UnitManager::units[type].end(); it++) {
            if ((*it)->self == self) {
                UnitManager::units[type].erase(it);
                break;
            }
        }
    } else if (team == Unit::Alliance::Neutral) {
        for (auto it = UnitManager::neutrals[type].begin(); it != UnitManager::neutrals[type].end(); it++) {
            if ((*it)->self == self) {
                UnitManager::neutrals[type].erase(it);
                break;
            }
        }
    } else if (team == Unit::Alliance::Enemy) {
        for (auto it = UnitManager::enemies[type].begin(); it != UnitManager::enemies[type].end(); it++) {
            if ((*it)->self == self) {
                UnitManager::enemies[type].erase(it);
                break;
            }
        }
    } else {
        printf("WHAT THE FUCK TEAM IS THIS UNIT ON\n");
    }

}

bool UnitWrapper::execute(Agent *agent) {
    return false;
}