#include "DiscordStatus.h"
#include <cstring>

DiscordStatus* DiscordStatus::getInstance()
{
    static DiscordStatus* discordStatus = new DiscordStatus;
    return discordStatus;
}

DiscordRichPresence& DiscordStatus::getPresence()
{
    return discordPresence;
}

DiscordStatus::DiscordStatus()
{

}

void DiscordStatus::Start()
{
    startTime = time(nullptr);
    memset(&handler, 0, sizeof(handler));
    Discord_Initialize("950115981169147904",
                       &handler,
                       1,
                       nullptr);

    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.startTimestamp = startTime;
    discordPresence.largeImageKey = "icon";
    Discord_UpdatePresence(&discordPresence);
}

void DiscordStatus::Stop()
{
    Discord_ClearPresence();
}

void DiscordStatus::update()
{
    Discord_UpdatePresence(&discordPresence);
}

void DiscordStatus::setTopText(std::string text)
{
    const char* c_text = text.c_str();
    discordPresence.details = c_text;
}

void DiscordStatus::setBottomText(std::string text)
{
    const char* c_text = text.c_str();
    discordPresence.state = c_text;
}