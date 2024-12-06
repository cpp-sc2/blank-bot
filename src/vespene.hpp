#pragma once
#include <sc2api/sc2_api.h>

#include <map>

#include "constants.h"
#include "unit.hpp"

 class Vespene : public UnitWrapper {
 private:
     
 public:
     int taken;
     Vespene(const Unit* unit) : UnitWrapper(unit), taken(0) {

     }
 };