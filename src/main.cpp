// The MIT License (MIT)
//
// Copyright (c) 2021 Alexander Kurbatov

#include "Bot.h"

#include <sc2api/sc2_coordinator.h>
#include <sc2api/sc2_gametypes.h>

#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Provide either name of the map file or path to it!" << std::endl;
        return 1;
    }

    sc2::Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    // NOTE: Uncomment to start the game in full screen mode.
    // coordinator.SetFullScreen(true);

    // NOTE: Uncomment to play at normal speed.
    // coordinator.SetRealtime(true);

    // NOTE: Uncomment to increase speed of steps processing.
    // Disables ability to control your bot during game.
    // Recommended for competitions.
    // coordinator.SetRawAffectsSelection(true);

    Bot bot;
    coordinator.SetParticipants(
        {
            CreateParticipant(sc2::Race::Random, &bot, "BlankBot"),
            CreateComputer(
                sc2::Race::Random,
                sc2::Difficulty::CheatInsane,
                sc2::AIBuild::Rush,
                "CheatInsane"
                )
        });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(argv[1]);

    while (coordinator.Update())
    {}

    return 0;
}
