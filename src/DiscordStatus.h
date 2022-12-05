#pragma once

#include "discord_rpc.h"
#include <time.h>
#include <string>

class DiscordStatus
{
public:

    void Start();

    void Stop();

    DiscordRichPresence& getPresence();

    static DiscordStatus* getInstance();

    void update();

    void setTopText(std::string text);

    void setBottomText(std::string text);

private:
    DiscordStatus();

    DiscordEventHandlers handler;
    DiscordRichPresence  discordPresence;
    time_t               startTime;
};