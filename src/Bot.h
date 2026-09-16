// The MIT License (MIT)
//
// Copyright (c) 2021-2026 Alexander Kurbatov

#pragma once

#include <sc2api/sc2_agent.h>
#include <sc2api/sc2_client.h>

#include <string>
#include <vector>

// The main bot class.
struct Bot final : sc2::Agent {
    Bot() = default;

    auto OnGameStart() -> void final;

    auto OnGameEnd() -> void final;

    auto OnStep() -> void final;

    auto OnBuildingConstructionComplete(const sc2::Unit* building) -> void final;

    auto OnUnitCreated(const sc2::Unit* unit) -> void final;

    auto OnUnitIdle(const sc2::Unit* unit) -> void final;

    auto OnUnitDestroyed(const sc2::Unit* unit) -> void final;

    auto OnUpgradeCompleted(sc2::UpgradeID id) -> void final;

    auto OnError(const std::vector<sc2::ClientError>& client_errors,
                 const std::vector<std::string>& protocol_errors = {}) -> void final;
};
