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

auto Bot::OnGameStart() -> void
{
    std::cout << "New game started!\n";
}

auto Bot::OnGameEnd() -> void
{
    std::cout << "Game over!\n";
}

auto Bot::OnBuildingConstructionComplete(const sc2::Unit* building) -> void
{
    std::cout << sc2::UnitTypeToName(building->unit_type) << " (" << building->tag << ") constructed\n";
}

auto Bot::OnStep() -> void
{
    std::cout << "OnStep\n";
}

auto Bot::OnUnitCreated(const sc2::Unit* unit) -> void
{
    std::cout << sc2::UnitTypeToName(unit->unit_type) << " (" << unit->tag << ") was created\n";
}

auto Bot::OnUnitIdle(const sc2::Unit* unit) -> void
{
    std::cout << sc2::UnitTypeToName(unit->unit_type) << " (" << unit->tag << ") is idle\n";
}

auto Bot::OnUnitDestroyed(const sc2::Unit* unit) -> void
{
    std::cout << sc2::UnitTypeToName(unit->unit_type) << " (" << unit->tag << ") was destroyed\n";
}

auto Bot::OnUpgradeCompleted(sc2::UpgradeID id) -> void
{
    std::cout << sc2::UpgradeIDToName(id) << " completed\n";
}

auto Bot::OnError(const std::vector<sc2::ClientError>& client_errors, const std::vector<std::string>& protocol_errors)
    -> void
{
    for (const auto& err : client_errors) {
        std::cerr << "Encountered client error: " << static_cast<int>(err) << '\n';
    }

    for (const auto& i : protocol_errors) {
        std::cerr << "Encountered protocol error: " << i << '\n';
    }
}
