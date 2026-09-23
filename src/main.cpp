// The MIT License (MIT)
//
// Copyright (c) 2021-2026 Alexander Kurbatov

#include <sc2api/sc2_coordinator.h>
#include <sc2api/sc2_gametypes.h>

#include <exception>
#include <iostream>

#include "Bot.h"

#ifdef BUILD_FOR_LADDER
#include <sc2utils/sc2_arg_parser.h>

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <system_error>

namespace
{

constexpr int32_t ResponseTimeoutMS = 10000;

struct Options {
    int32_t GamePort = 0;
    int32_t StartPort = 0;
    std::string ServerAddress;
    std::string OpponentId;
};

bool ParseInt(std::string_view value, int32_t& dst)
{
    const char* const first = value.data();
    const char* const last = first + value.size();
    const auto [ptr, ec] = std::from_chars(first, last, dst);

    return ec == std::errc{} && ptr == last;
}

Options ParseArguments(int argc, char** argv)
{
    Options options;

    sc2::ArgParser arg_parser(argv[0]);
    arg_parser.AddOptions({
        {
            .abbreviation_ = "-g",
            .fullname_ = "--GamePort",
            .description_ = "Port of client to connect to",
            .required_ = false,
        },
        {
            .abbreviation_ = "-o",
            .fullname_ = "--StartPort",
            .description_ = "Starting server port",
            .required_ = false,
        },
        {
            .abbreviation_ = "-l",
            .fullname_ = "--LadderServer",
            .description_ = "Ladder server address",
            .required_ = false,
        },
        {
            .abbreviation_ = "-x",
            .fullname_ = "--OpponentId",
            .description_ = "PlayerId of opponent",
            .required_ = false,
        },
    });

    arg_parser.Parse(argc, argv);

    std::string GamePortStr;
    if (arg_parser.Get("GamePort", GamePortStr) && !ParseInt(GamePortStr, options.GamePort)) {
        std::cerr << "Invalid game port: " << GamePortStr << '\n';
    }

    std::string StartPortStr;
    if (arg_parser.Get("StartPort", StartPortStr) && !ParseInt(StartPortStr, options.StartPort)) {
        std::cerr << "Invalid start port: " << StartPortStr << '\n';
    }

    std::string OpponentId;
    if (arg_parser.Get("OpponentId", OpponentId)) {
        options.OpponentId = OpponentId;
    }

    arg_parser.Get("LadderServer", options.ServerAddress);

    return options;
}

} // namespace

int main(int argc, char* argv[])
try {
    auto options = ParseArguments(argc, argv);

    sc2::Coordinator coordinator;
    Bot bot;

    const size_t num_agents = 2;
    coordinator.SetParticipants({CreateParticipant(sc2::Race::Random, &bot, "BlankBot")});

    std::cout << "Connecting to port " << options.GamePort << '\n';
    coordinator.Connect(options.GamePort);
    coordinator.SetupPorts(num_agents, options.StartPort, false);

    // NB (alkurbatov): Increase speed of steps processing by disabling ability
    // to control your bot during game.
    // Recommended for competitions.
    coordinator.SetRawAffectsSelection(true);

    coordinator.JoinGame();
    coordinator.SetTimeoutMS(ResponseTimeoutMS);
    std::cout << "Successfully joined game\n";

    while (coordinator.Update()) {}

    return 0;
} catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << '\n';
    return 1;
} catch (...) {
    std::cerr << "Fatal error: unknown exception\n";
    return 1;
}

#else

int main(int argc, char* argv[])
try {
    if (argc < 2) {
        std::cerr << "Provide either name of the map file or path to it!\n";
        return 1;
    }

    sc2::Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    // NOTE: Uncomment to start the game in full screen mode.
    // coordinator.SetFullScreen(true);

    // NOTE: Uncomment to play at normal speed.
    // coordinator.SetRealtime(true);

    Bot bot;
    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Random, &bot, "BlankBot"),
        CreateComputer(sc2::Race::Random, sc2::Difficulty::CheatInsane, sc2::AIBuild::Rush, "CheatInsane"),
    });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(argv[1]);

    while (coordinator.Update()) {}

    return 0;
} catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << '\n';
    return 1;
} catch (...) {
    std::cerr << "Fatal error: unknown exception\n";
    return 1;
}

#endif
