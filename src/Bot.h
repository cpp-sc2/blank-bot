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

    void OnGameStart() final;

    void OnGameEnd() final;

    void OnStep() final;

    void OnBuildingConstructionComplete(const sc2::Unit* building) final;

    void OnUnitCreated(const sc2::Unit* unit) final;

    void OnUnitIdle(const sc2::Unit* unit) final;

    void OnUnitDestroyed(const sc2::Unit* unit) final;

    void OnUpgradeCompleted(sc2::UpgradeID id) final;

    void OnError(const std::vector<sc2::ClientError>& client_errors,
                 const std::vector<std::string>& protocol_errors = {}) final;
};
