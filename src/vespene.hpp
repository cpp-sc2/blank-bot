#pragma once
#include <sc2api/sc2_api.h>

#include <map>

#include "constants.h"
#include "unit.hpp"

 class Vespene : public UnitWrapper {
 private:
     
 public:
     int taken; //1 is macro taken, 2 is built;
     Vespene(const Unit* unit) : UnitWrapper(unit), taken(0) {

     }
 };