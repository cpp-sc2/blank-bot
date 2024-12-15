#pragma once
#include <sc2api/sc2_api.h>

#include "constants.h"

#define CASE(unit) case (uint32_t(UNIT_TYPEID::unit)):

//UnitTypeId\.([A-Z]+) ?: (-?[0-9]+),
//CASE(TERRAN_$1) {\n\treturn $2;\n}\n

namespace Army {
	int Priority(UnitTypeID self, UnitTypeID opponent) {
        switch (uint32_t(self)) {
            /*
            CASE(PROTOSS_SENTRY) {
                switch (uint32_t(opponent)) {

                    default: {
                        return 0;
                    }
                }
            }
            CASE(PROTOSS_PROBE) {
                switch (uint32_t(opponent)) {

                    default: {
                        return 0;
                    }
                }
            }
            CASE(PROTOSS_HIGHTEMPLAR) {
                switch (uint32_t(opponent)) {

                    default: {
                        return 0;
                    }
                }
            }
            CASE(PROTOSS_DARKTEMPLAR) {
                switch (uint32_t(opponent)) {

                    default: {
                        return 0;
                    }
                }
            }
            */
            CASE(PROTOSS_ADEPT) {
                switch (uint32_t(opponent)) {
                    //Terran
                    CASE(TERRAN_MULE) { return 9; }
                    CASE(TERRAN_SCV) { return 9; }
                    CASE(TERRAN_SIEGETANK) { return 3; }
                    CASE(TERRAN_SIEGETANKSIEGED) { return 5; }
                    CASE(TERRAN_GHOST) { return 10; }
                    CASE(TERRAN_REAPER) { return 8; }
                    CASE(TERRAN_MARAUDER) { return 4; }
                    CASE(TERRAN_MARINE) { return 8; }
                    CASE(TERRAN_CYCLONE) { return 4; }
                    CASE(TERRAN_HELLION) { return 8; }
                    CASE(TERRAN_HELLIONTANK) { return 3; }
                    CASE(TERRAN_THOR) { return 3; }
                    CASE(TERRAN_MEDIVAC) { return -1; }
                    CASE(TERRAN_VIKINGFIGHTER) { return -1; }
                    CASE(TERRAN_VIKINGASSAULT) { return -1; }
                    CASE(TERRAN_LIBERATORAG) { return -1; }
                    CASE(TERRAN_LIBERATOR) { return -1; }
                    CASE(TERRAN_RAVEN) { return -1; }
                    CASE(TERRAN_BATTLECRUISER) { return -1; }
                    CASE(TERRAN_MISSILETURRET) { return 1; }
                    CASE(TERRAN_BUNKER) { return 2; }
                    //Zerg
                    CASE(ZERG_DRONE) { return 9; }
                    CASE(ZERG_ZERGLING) { return 8; }
                    CASE(ZERG_BANELING) { return 10; }
                    CASE(ZERG_ULTRALISK) { return 4; }
                    CASE(ZERG_QUEEN) { return 6; }
                    CASE(ZERG_ROACH) { return 4; }
                    CASE(ZERG_RAVAGER) { return 4; }
                    CASE(ZERG_HYDRALISK) { return 8; }
                    CASE(ZERG_HYDRALISKBURROWED) { return 8; }
                    CASE(ZERG_LURKERMP) { return 3; }
                    CASE(ZERG_LURKERMPBURROWED) { return 3; }
                    CASE(ZERG_INFESTOR) { return 10; }
                    CASE(ZERG_BROODLORD) { return -1; }
                    CASE(ZERG_MUTALISK) { return -1; }
                    CASE(ZERG_CORRUPTOR) { return -1; }
                    CASE(ZERG_LARVA) { return -1; }
                    CASE(ZERG_EGG) { return -1; }
                    CASE(ZERG_LOCUSTMP) { return -1; }
                    //Protoss
                    CASE(PROTOSS_SENTRY) { return 9; }
                    CASE(PROTOSS_PROBE) { return 10; }
                    CASE(PROTOSS_HIGHTEMPLAR) { return 10; }
                    CASE(PROTOSS_DARKTEMPLAR) { return 9; }
                    CASE(PROTOSS_ADEPT) { return 8; }
                    CASE(PROTOSS_ZEALOT) { return 8; }
                    CASE(PROTOSS_STALKER) { return 4; }
                    CASE(PROTOSS_IMMORTAL) { return 2; }
                    CASE(PROTOSS_COLOSSUS) { return 3; }
                    CASE(PROTOSS_WARPPRISM) { return 8; }
                    CASE(PROTOSS_OBSERVER) { return 7; }
                    CASE(PROTOSS_DISRUPTOR) { return 9; }
                    CASE(PROTOSS_PHOENIX) { return 5; }
                    CASE(PROTOSS_VOIDRAY) { return 5; }
                    CASE(PROTOSS_CARRIER) { return 7; }
                    CASE(PROTOSS_TEMPEST) { return 6; }
                    CASE(PROTOSS_MOTHERSHIP) { return 9; }
                    CASE(PROTOSS_ARCHON) { return 4; }
                    CASE(PROTOSS_SHIELDBATTERY) { return 1; }
                    CASE(PROTOSS_PHOTONCANNON) { return 1; }
                    CASE(PROTOSS_PYLON) { return 2; }
                    default: { return 0; }
                }
            }
            /*
            CASE(PROTOSS_ZEALOT) { 
                switch (uint32_t(opponent)) {

                    default: {
                        return 0;
                    }
                }
            }
            */
            CASE(PROTOSS_STALKER) { 
                switch (uint32_t(opponent)) {
                    //Terran
                    CASE(TERRAN_WIDOWMINE) { return 8; }
                    CASE(TERRAN_WIDOWMINEBURROWED) { return 10; }
                    CASE(TERRAN_SIEGETANK) { return 8; }
                    CASE(TERRAN_SIEGETANKSIEGED) { return 10; }
                    CASE(TERRAN_MULE) { return 3; }
                    CASE(TERRAN_SCV) { return 10; }
                    CASE(TERRAN_GHOST) { return 7; }
                    CASE(TERRAN_REAPER) { return 4; }
                    CASE(TERRAN_MARAUDER) { return 4; }
                    CASE(TERRAN_MARINE) { return 3; }
                    CASE(TERRAN_CYCLONE) { return 5; }
                    CASE(TERRAN_HELLION) { return 2; }
                    CASE(TERRAN_HELLIONTANK) { return 3; }
                    CASE(TERRAN_THOR) { return 7; }
                    CASE(TERRAN_MEDIVAC) { return 6; }
                    CASE(TERRAN_VIKINGFIGHTER) { return 5; }
                    CASE(TERRAN_VIKINGASSAULT) { return 5; }
                    CASE(TERRAN_LIBERATORAG) { return 9; }
                    CASE(TERRAN_LIBERATOR) { return 5; }
                    CASE(TERRAN_RAVEN) { return 10; }
                    CASE(TERRAN_BATTLECRUISER) { return 8; }
                    CASE(TERRAN_MISSILETURRET) { return 1; }
                    CASE(TERRAN_BUNKER) { return 2; }
                    //Zerg
                    CASE(ZERG_DRONE) { return 4; }
                    CASE(ZERG_ZERGLING) { return 3; }
                    CASE(ZERG_BANELING) { return 6; }
                    CASE(ZERG_BANELINGCOCOON) { return 6; }
                    CASE(ZERG_ULTRALISK) { return 6; }
                    CASE(ZERG_QUEEN) { return 5; }
                    CASE(ZERG_ROACH) { return 6; }
                    CASE(ZERG_RAVAGER) { return 8; }
                    CASE(ZERG_RAVAGERCOCOON) { return 8; }
                    CASE(ZERG_HYDRALISK) { return 7; }
                    CASE(ZERG_HYDRALISKBURROWED) { return 7; }
                    CASE(ZERG_LURKERMP) { return 9; }
                    CASE(ZERG_LURKERMPEGG) { return 9; }
                    CASE(ZERG_LURKERMPBURROWED) { return 9; }
                    CASE(ZERG_INFESTOR) { return 10; }
                    CASE(ZERG_BROODLORD) { return 10; }
                    CASE(ZERG_BROODLORDCOCOON) { return 10; }
                    CASE(ZERG_MUTALISK) { return 6; }
                    CASE(ZERG_CORRUPTOR) { return 8; }
                    CASE(ZERG_LARVA) { return -1; }
                    CASE(ZERG_EGG) { return -1; }
                    CASE(ZERG_LOCUSTMP) { return -1; }
                    //Protoss
                    CASE(PROTOSS_SENTRY) { return 8; }
                    CASE(PROTOSS_PROBE) { return 4; }
                    CASE(PROTOSS_HIGHTEMPLAR) { return 10; }
                    CASE(PROTOSS_DARKTEMPLAR) { return 9; }
                    CASE(PROTOSS_ADEPT) { return 4; }
                    CASE(PROTOSS_ZEALOT) { return 4; }
                    CASE(PROTOSS_STALKER) { return 5; }
                    CASE(PROTOSS_IMMORTAL) { return 9; }
                    CASE(PROTOSS_COLOSSUS) { return 10; }
                    CASE(PROTOSS_WARPPRISM) { return 8; }
                    CASE(PROTOSS_OBSERVER) { return 7; }
                    CASE(PROTOSS_DISRUPTOR) { return 9; }
                    CASE(PROTOSS_PHOENIX) { return 5; }
                    CASE(PROTOSS_VOIDRAY) { return 5; }
                    CASE(PROTOSS_CARRIER) { return 7; }
                    CASE(PROTOSS_TEMPEST) { return 6; }
                    CASE(PROTOSS_MOTHERSHIP) { return 9; }
                    CASE(PROTOSS_ARCHON) { return 6; }
                    CASE(PROTOSS_SHIELDBATTERY) { return 1; }
                    CASE(PROTOSS_PHOTONCANNON) { return 1; }
                    CASE(PROTOSS_PYLON) { return 2; }
                    default: { return 0; }
                }
            }
            /*
            CASE(PROTOSS_IMMORTAL) { 
                switch (uint32_t(opponent)) {

                    default: {
                        return 0;
                    }
                }
            }
            */
            CASE(PROTOSS_COLOSSUS) { 
                switch (uint32_t(opponent)) {
                    //Terran
                    CASE(TERRAN_MULE) { return 9; }
                    CASE(TERRAN_SCV) { return 7; }
                    CASE(TERRAN_SIEGETANK) { return 3; }
                    CASE(TERRAN_SIEGETANKSIEGED) { return 5; } //sieged tanks are much higher priority than unsieged
                    CASE(TERRAN_GHOST) { return 8; }
                    CASE(TERRAN_REAPER) { return 6; }
                    CASE(TERRAN_MARAUDER) { return 4; }
                    CASE(TERRAN_MARINE) { return 10; }
                    CASE(TERRAN_CYCLONE) { return 4; }
                    CASE(TERRAN_HELLION) { return 8; }
                    CASE(TERRAN_HELLIONTANK) { return 3; }
                    CASE(TERRAN_THOR) { return 3; }
                    CASE(TERRAN_MEDIVAC) { return -1; }
                    CASE(TERRAN_VIKINGFIGHTER) { return -1; }
                    CASE(TERRAN_VIKINGASSAULT) { return -1; }
                    CASE(TERRAN_LIBERATORAG) { return -1; }
                    CASE(TERRAN_LIBERATOR) { return -1; }
                    CASE(TERRAN_RAVEN) { return -1; }
                    CASE(TERRAN_BATTLECRUISER) { return -1; }
                    CASE(TERRAN_MISSILETURRET) { return 1; }
                    CASE(TERRAN_BUNKER) { return 2; }
                    //Zerg
                    CASE(ZERG_DRONE) { return 7; }
                    CASE(ZERG_ZERGLING) { return 10; }
                    CASE(ZERG_BANELING) { return 9; }
                    CASE(ZERG_ULTRALISK) { return 4; }
                    CASE(ZERG_QUEEN) { return 6; }
                    CASE(ZERG_ROACH) { return 4; }
                    CASE(ZERG_RAVAGER) { return 4; }
                    CASE(ZERG_HYDRALISK) { return 9; }
                    CASE(ZERG_HYDRALISKBURROWED) { return 8; }
                    CASE(ZERG_LURKERMP) { return 3; }
                    CASE(ZERG_LURKERMPBURROWED) { return 3; }
                    CASE(ZERG_INFESTOR) { return 6; }
                    CASE(ZERG_BROODLORD) { return -1; }
                    CASE(ZERG_MUTALISK) { return -1; }
                    CASE(ZERG_CORRUPTOR) { return -1; }
                    CASE(ZERG_LARVA) { return -1; }
                    CASE(ZERG_EGG) { return -1; }
                    CASE(ZERG_LOCUSTMP) { return -1; }
                    //Protoss
                    CASE(PROTOSS_SENTRY) { return 9; }
                    CASE(PROTOSS_PROBE) { return 7; }
                    CASE(PROTOSS_HIGHTEMPLAR) { return 10; }
                    CASE(PROTOSS_DARKTEMPLAR) { return 9; }
                    CASE(PROTOSS_ADEPT) { return 8; }
                    CASE(PROTOSS_ZEALOT) { return 8; }
                    CASE(PROTOSS_STALKER) { return 4; }
                    CASE(PROTOSS_IMMORTAL) { return 2; }
                    CASE(PROTOSS_COLOSSUS) { return 3; }
                    CASE(PROTOSS_WARPPRISM) { return 8; }
                    CASE(PROTOSS_OBSERVER) { return 7; }
                    CASE(PROTOSS_DISRUPTOR) { return 9; }
                    CASE(PROTOSS_PHOENIX) { return 5; }
                    CASE(PROTOSS_VOIDRAY) { return 5; }
                    CASE(PROTOSS_CARRIER) { return 7; }
                    CASE(PROTOSS_TEMPEST) { return 6; }
                    CASE(PROTOSS_MOTHERSHIP) { return 9; }
                    CASE(PROTOSS_ARCHON) { return 4; }
                    CASE(PROTOSS_SHIELDBATTERY) { return 1; }
                    CASE(PROTOSS_PHOTONCANNON) { return 1; }
                    CASE(PROTOSS_PYLON) { return 2; }
                    default: { return 0; }
                }
            }
            /*
            CASE(PROTOSS_ARCHON) { 
                switch (uint32_t(opponent)) {

                    default: {
                        return 0;
                    }
                }
            }
            CASE(PROTOSS_SHIELDBATTERY) { 
                switch (uint32_t(opponent)) {

                    default: {
                        return 0;
                    }
                }
            }
            CASE(PROTOSS_PHOTONCANNON) { 
                switch (uint32_t(opponent)) {

                    default: {
                        return 0;
                    }
                }
            }
            CASE(PROTOSS_PYLON) { 
                switch (uint32_t(opponent)) {

                    default: {
                        return 0;
                    }
                }
            }
            CASE(PROTOSS_FLEETBEACON) { 
                switch (uint32_t(opponent)) {

                    default: {
                        return 0;
                    }
                }
            }
            */
            default: {
                switch (uint32_t(opponent)) {
                    //Terran
                    CASE(TERRAN_WIDOWMINE) { return 8; }
                    CASE(TERRAN_WIDOWMINEBURROWED) { return 10; }
                    CASE(TERRAN_SIEGETANK) { return 8; }
                    CASE(TERRAN_SIEGETANKSIEGED) { return 10; }
                    CASE(TERRAN_MULE) { return 3; }
                    CASE(TERRAN_SCV) { return 3; }
                    CASE(TERRAN_GHOST) { return 7; }
                    CASE(TERRAN_REAPER) { return 4; }
                    CASE(TERRAN_MARAUDER) { return 4; }
                    CASE(TERRAN_MARINE) { return 3; }
                    CASE(TERRAN_CYCLONE) { return 5; }
                    CASE(TERRAN_HELLION) { return 2; }
                    CASE(TERRAN_HELLIONTANK) { return 3; }
                    CASE(TERRAN_THOR) { return 7; }
                    CASE(TERRAN_MEDIVAC) { return 6; }
                    CASE(TERRAN_VIKINGFIGHTER) { return 5; }
                    CASE(TERRAN_VIKINGASSAULT) { return 5; }
                    CASE(TERRAN_LIBERATORAG) { return 7; }
                    CASE(TERRAN_LIBERATOR) { return 5; }
                    CASE(TERRAN_RAVEN) { return 7; }
                    CASE(TERRAN_BATTLECRUISER) { return 8; }
                    CASE(TERRAN_MISSILETURRET) { return 1; }
                    CASE(TERRAN_BUNKER) { return 2; }
                    //Zerg
                    CASE(ZERG_DRONE) { return 4; }
                    CASE(ZERG_ZERGLING) { return 3; }
                    CASE(ZERG_BANELING) { return 6; }
                    CASE(ZERG_BANELINGCOCOON) { return 6; }
                    CASE(ZERG_ULTRALISK) { return 6; }
                    CASE(ZERG_QUEEN) { return 5; }
                    CASE(ZERG_ROACH) { return 6; }
                    CASE(ZERG_RAVAGER) { return 8; }
                    CASE(ZERG_RAVAGERCOCOON) { return 8; }
                    CASE(ZERG_HYDRALISK) { return 7; }
                    CASE(ZERG_HYDRALISKBURROWED) { return 7; }
                    CASE(ZERG_LURKERMP) { return 9; }
                    CASE(ZERG_LURKERMPEGG) { return 9; }
                    CASE(ZERG_LURKERMPBURROWED) { return 9; }
                    CASE(ZERG_INFESTOR) { return 10; }
                    CASE(ZERG_BROODLORD) { return 10; }
                    CASE(ZERG_BROODLORDCOCOON) { return 10; }
                    CASE(ZERG_MUTALISK) { return 6; }
                    CASE(ZERG_CORRUPTOR) { return 8; }
                    CASE(ZERG_OVERLORD) { return 2; }
                    CASE(ZERG_OVERSEER) { return 1; }
                    CASE(ZERG_VIPER) { return 3; }
                    CASE(ZERG_LARVA) { return -1; }
                    CASE(ZERG_EGG) { return -1; }
                    CASE(ZERG_LOCUSTMP) { return -1; }
                    //Protoss
                    CASE(PROTOSS_SENTRY) { return 8; }
                    CASE(PROTOSS_PROBE) { return 4; }
                    CASE(PROTOSS_HIGHTEMPLAR) { return 10; }
                    CASE(PROTOSS_DARKTEMPLAR) { return 9; }
                    CASE(PROTOSS_ADEPT) { return 4; }
                    CASE(PROTOSS_ZEALOT) { return 4; }
                    CASE(PROTOSS_STALKER) { return 5; }
                    CASE(PROTOSS_IMMORTAL) { return 8; }
                    CASE(PROTOSS_COLOSSUS) { return 10; }
                    CASE(PROTOSS_WARPPRISM) { return 8; }
                    CASE(PROTOSS_OBSERVER) { return 7; }
                    CASE(PROTOSS_DISRUPTOR) { return 9; }
                    CASE(PROTOSS_PHOENIX) { return 5; }
                    CASE(PROTOSS_VOIDRAY) { return 5; }
                    CASE(PROTOSS_CARRIER) { return 7; }
                    CASE(PROTOSS_TEMPEST) { return 6; }
                    CASE(PROTOSS_MOTHERSHIP) { return 9; }
                    CASE(PROTOSS_ARCHON) { return 6; }
                    CASE(PROTOSS_SHIELDBATTERY) { return 1; }
                    CASE(PROTOSS_PHOTONCANNON) { return 1; }
                    CASE(PROTOSS_PYLON) { return 2; }
                    default: { return 0; }
                }
            }
        }
        return 0;
	}

    Composition unitTypeTargetComposition(UnitTypeID op) {
        switch (uint32_t(op)) {
            //Terran
            CASE(TERRAN_WIDOWMINE) { return Composition::Ground; }
            CASE(TERRAN_WIDOWMINEBURROWED) { return Composition::Ground; }
            CASE(TERRAN_SIEGETANK) { return Composition::Ground; }
            CASE(TERRAN_SIEGETANKSIEGED) { return Composition::Ground; }
            CASE(TERRAN_MULE) { return Composition::Ground; }
            CASE(TERRAN_SCV) { return Composition::Ground; }
            CASE(TERRAN_GHOST) { return Composition::Ground; }
            CASE(TERRAN_REAPER) { return Composition::Ground; }
            CASE(TERRAN_MARAUDER) { return Composition::Ground; }
            CASE(TERRAN_MARINE) { return Composition::Ground; }
            CASE(TERRAN_CYCLONE) { return Composition::Ground; }
            CASE(TERRAN_HELLION) { return Composition::Ground; }
            CASE(TERRAN_HELLIONTANK) { return Composition::Ground; }
            CASE(TERRAN_THOR) { return Composition::Ground; }
            CASE(TERRAN_MEDIVAC) { return Composition::Air; }
            CASE(TERRAN_VIKINGFIGHTER) { return Composition::Air; }
            CASE(TERRAN_VIKINGASSAULT) { return Composition::Ground; }
            CASE(TERRAN_LIBERATORAG) { return Composition::Air; }
            CASE(TERRAN_LIBERATOR) { return Composition::Air; }
            CASE(TERRAN_RAVEN) { return Composition::Air; }
            CASE(TERRAN_BATTLECRUISER) { return Composition::Air; }
            CASE(TERRAN_MISSILETURRET) { return Composition::Ground; }
            CASE(TERRAN_BUNKER) { return Composition::Ground; }
            //Zerg
            CASE(ZERG_DRONE) { return Composition::Ground; }
            CASE(ZERG_ZERGLING) { return Composition::Ground; }
            CASE(ZERG_BANELING) { return Composition::Ground; }
            CASE(ZERG_BANELINGCOCOON) { return Composition::Ground; }
            CASE(ZERG_ULTRALISK) { return Composition::Ground; }
            CASE(ZERG_QUEEN) { return Composition::Ground; }
            CASE(ZERG_ROACH) { return Composition::Ground; }
            CASE(ZERG_RAVAGER) { return Composition::Ground; }
            CASE(ZERG_RAVAGERCOCOON) { return Composition::Ground; }
            CASE(ZERG_HYDRALISK) { return Composition::Ground; }
            CASE(ZERG_HYDRALISKBURROWED) { return Composition::Ground; }
            CASE(ZERG_LURKERMP) { return Composition::Ground; }
            CASE(ZERG_LURKERMPEGG) { return Composition::Ground; }
            CASE(ZERG_LURKERMPBURROWED) { return Composition::Ground; }
            CASE(ZERG_INFESTOR) { return Composition::Ground; }
            CASE(ZERG_BROODLORD) { return Composition::Air; }
            CASE(ZERG_BROODLORDCOCOON) { return Composition::Air; }
            CASE(ZERG_MUTALISK) { return Composition::Air; }
            CASE(ZERG_CORRUPTOR) { return Composition::Air; }
            CASE(ZERG_OVERLORD) { return Composition::Air; }
            CASE(ZERG_OVERSEER) { return Composition::Air; }
            CASE(ZERG_VIPER) { return Composition::Air; }
            CASE(ZERG_LARVA) { return Composition::Ground; }
            CASE(ZERG_EGG) { return Composition::Ground; }
            CASE(ZERG_LOCUSTMP) { return Composition::Ground; }
            //Protoss
            CASE(PROTOSS_SENTRY) { return Composition::Ground; }
            CASE(PROTOSS_PROBE) { return Composition::Ground; }
            CASE(PROTOSS_HIGHTEMPLAR) { return Composition::Ground; }
            CASE(PROTOSS_DARKTEMPLAR) { return Composition::Ground; }
            CASE(PROTOSS_ADEPT) { return Composition::Ground; }
            CASE(PROTOSS_ZEALOT) { return Composition::Ground; }
            CASE(PROTOSS_STALKER) { return Composition::Ground; }
            CASE(PROTOSS_IMMORTAL) { return Composition::Ground; }
            CASE(PROTOSS_COLOSSUS) { return Composition::Any; }
            CASE(PROTOSS_WARPPRISM) { return Composition::Air; }
            CASE(PROTOSS_OBSERVER) { return Composition::Air; }
            CASE(PROTOSS_DISRUPTOR) { return Composition::Ground; }
            CASE(PROTOSS_PHOENIX) { return Composition::Air; }
            CASE(PROTOSS_VOIDRAY) { return Composition::Air; }
            CASE(PROTOSS_CARRIER) { return Composition::Air; }
            CASE(PROTOSS_TEMPEST) { return Composition::Air; }
            CASE(PROTOSS_MOTHERSHIP) { return Composition::Air; }
            CASE(PROTOSS_ARCHON) { return Composition::Ground; }
            CASE(PROTOSS_SHIELDBATTERY) { return Composition::Ground; }
            CASE(PROTOSS_PHOTONCANNON) { return Composition::Ground; }
            CASE(PROTOSS_PYLON) { return Composition::Ground; }
            default: { 
                printf("HITCHECK %s\n", UnitTypeToName(op));
                return Composition::Invalid; 
            }
        }
    }

    bool hitsUnit(Composition weapon, Composition target) {
        if (weapon == Composition::Invalid || target == Composition::Invalid) {
            printf("HITCHECK ARMY ERROR %d %d\n", weapon, target);
            return false;
        }
        if (weapon == Composition::Any || target == Composition::Any) {
            return true;
        }
        if ((weapon == Composition::Air && target == Composition::Air) ||
            (weapon == Composition::Ground && target == Composition::Ground)) {
            return true;
        }
        return false;
    }
}