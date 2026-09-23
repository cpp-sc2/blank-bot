// The MIT License (MIT)
//
// Copyright (c) 2021-2026 Alexander Kurbatov

#include "Bot.h"

#include <sc2api/sc2_client.h>
#include <sc2api/sc2_typeenums.h>
#include <sc2api/sc2_unit.h>

#include <iostream>
#include <string>
#include <vector>

void Bot::OnGameStart()
{
    std::cout << "New game started!\n";
}

void Bot::OnGameEnd()
{
    std::cout << "Game over!\n";
}

void Bot::OnBuildingConstructionComplete(const sc2::Unit* building)
{
    std::cout << sc2::UnitTypeToName(building->unit_type) << " (" << building->tag << ") constructed\n";
}

void Bot::OnStep()
{
    std::cout << "OnStep\n";
}

void Bot::OnUnitCreated(const sc2::Unit* unit)
{
    std::cout << sc2::UnitTypeToName(unit->unit_type) << " (" << unit->tag << ") was created\n";
}

void Bot::OnUnitIdle(const sc2::Unit* unit)
{
    std::cout << sc2::UnitTypeToName(unit->unit_type) << " (" << unit->tag << ") is idle\n";
}

void Bot::OnUnitDestroyed(const sc2::Unit* unit)
{
    std::cout << sc2::UnitTypeToName(unit->unit_type) << " (" << unit->tag << ") was destroyed\n";
}

void Bot::OnUpgradeCompleted(sc2::UpgradeID id)
{
    std::cout << sc2::UpgradeIDToName(id) << " completed\n";
}

void Bot::OnError(const std::vector<sc2::ClientError>& client_errors, const std::vector<std::string>& protocol_errors)
{
    for (const auto& err : client_errors) {
        std::cerr << "Encountered client error: " << static_cast<int>(err) << '\n';
    }

    for (const auto& i : protocol_errors) {
        std::cerr << "Encountered protocol error: " << i << '\n';
    }
}
