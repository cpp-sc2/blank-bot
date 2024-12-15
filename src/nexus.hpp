#pragma once
#include <sc2api/sc2_api.h>

#include <map>

#include "constants.h"
#include "unit.hpp"

class Nexus : public UnitWrapper {
private:
    UnitTypeID type1;
    Tag vespene1;
    bool taken1;

    UnitTypeID type2;
    Tag vespene2;
    bool taken2;
public:
    Nexus(const Unit* unit) : UnitWrapper(unit), type1(UNIT_TYPEID::INVALID), vespene1(NullTag), taken1(false), type2(UNIT_TYPEID::INVALID), vespene2(NullTag), taken2(false) {

    }
    
    void initVesp(Agent* agent) {
        //UnitWrappers vespenes = UnitManager::getVespene();
        //for (UnitWrapper* vesp : vespenes) {
        //    if (Distance2D(vesp->pos(agent), pos(agent)) < 12) {
        //        if (type1 == UNIT_TYPEID::INVALID) {
        //            type1 = vesp->type;
        //            vespene1 = vesp->self;
        //            ((Vespene*)vesp)->taken = 1;
        //        }
        //        else if (type2 == UNIT_TYPEID::INVALID) {
        //            type2 = vesp->type;
        //            vespene2 = vesp->self;
        //            ((Vespene*)vesp)->taken = 1;
        //            //break; //ADD THIS WHEN I KNOW IT WORKS
        //        }
        //        else {
        //            printf("THIRD VESPENE FOUND NEAR NEXUS, HUH??\n");
        //        }
        //    }
        //}
    }
};