/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "PlayerbotAIConfig.h"
#include "Config.h"
#include "Playerbots.h"
#include "PlayerbotFactory.h"
#include "RandomItemMgr.h"
#include "RandomPlayerbotFactory.h"
#include "Talentspec.h"
#include "PlayerbotDungeonSuggestionMgr.h"

#include <iostream>

template <class T>
void LoadList(std::string const value, T& list)
{
    std::vector<std::string> ids = split(value, ',');
    for (std::vector<std::string>::iterator i = ids.begin(); i != ids.end(); i++)
    {
        uint32 id = atoi((*i).c_str());
        if (!id)
            continue;

        list.push_back(id);
    }
}

template <class T>
void LoadListString(std::string const value, T& list)
{
    std::vector<std::string> strings = split(value, ',');
    for (std::vector<std::string>::iterator i = strings.begin(); i != strings.end(); i++)
    {
        std::string const string = *i;
        if (string.empty())
            continue;

        list.push_back(string);
    }
}

bool PlayerbotAIConfig::Initialize()
{
    LOG_INFO("server.loading", "Initializing AI Playerbots by ike3, based on the original Playerbots by blueboy");

    enabled = sConfigMgr->GetOption<bool>("AiPlayerbot.Enabled", true);
    if (!enabled)
    {
        LOG_INFO("server.loading", "AI Playerbots is Disabled in aiplayerbot.conf");
        return false;
    }

    globalCoolDown = sConfigMgr->GetOption<int32>("AiPlayerbot.GlobalCooldown", 1500);
    maxWaitForMove = sConfigMgr->GetOption<int32>("AiPlayerbot.MaxWaitForMove", 5000);
    expireActionTime = sConfigMgr->GetOption<int32>("AiPlayerbot.ExpireActionTime", 5000);
    dispelAuraDuration = sConfigMgr->GetOption<int32>("AiPlayerbot.DispelAuraDuration", 7000);
    reactDelay = sConfigMgr->GetOption<int32>("AiPlayerbot.ReactDelay", 500);
    passiveDelay = sConfigMgr->GetOption<int32>("AiPlayerbot.PassiveDelay", 10000);
    repeatDelay = sConfigMgr->GetOption<int32>("AiPlayerbot.RepeatDelay", 5000);
    errorDelay = sConfigMgr->GetOption<int32>("AiPlayerbot.ErrorDelay", 5000);
    rpgDelay = sConfigMgr->GetOption<int32>("AiPlayerbot.RpgDelay", 10000);
    sitDelay = sConfigMgr->GetOption<int32>("AiPlayerbot.SitDelay", 30000);
    returnDelay = sConfigMgr->GetOption<int32>("AiPlayerbot.ReturnDelay", 7000);
    lootDelay = sConfigMgr->GetOption<int32>("AiPlayerbot.LootDelay", 1000);

    farDistance = sConfigMgr->GetOption<float>("AiPlayerbot.FarDistance", 20.0f);
    sightDistance = sConfigMgr->GetOption<float>("AiPlayerbot.SightDistance", 75.0f);
    spellDistance = sConfigMgr->GetOption<float>("AiPlayerbot.SpellDistance", 22.0f);
    shootDistance = sConfigMgr->GetOption<float>("AiPlayerbot.ShootDistance", 25.0f);
    healDistance = sConfigMgr->GetOption<float>("AiPlayerbot.HealDistance", 20.0f);
    lootDistance = sConfigMgr->GetOption<float>("AiPlayerbot.LootDistance", 15.0f);
    fleeDistance = sConfigMgr->GetOption<float>("AiPlayerbot.FleeDistance", 7.5f);
    aggroDistance = sConfigMgr->GetOption<float>("AiPlayerbot.AggroDistance", 22.0f);
    tooCloseDistance = sConfigMgr->GetOption<float>("AiPlayerbot.TooCloseDistance", 5.0f);
    meleeDistance = sConfigMgr->GetOption<float>("AiPlayerbot.MeleeDistance", 1.5f);
    followDistance = sConfigMgr->GetOption<float>("AiPlayerbot.FollowDistance", 1.5f);
    whisperDistance = sConfigMgr->GetOption<float>("AiPlayerbot.WhisperDistance", 6000.0f);
    contactDistance = sConfigMgr->GetOption<float>("AiPlayerbot.ContactDistance", 0.5f);
    aoeRadius = sConfigMgr->GetOption<float>("AiPlayerbot.AoeRadius", 5.0f);
    rpgDistance = sConfigMgr->GetOption<float>("AiPlayerbot.RpgDistance", 200.0f);
    grindDistance = sConfigMgr->GetOption<float>("AiPlayerbot.GrindDistance", 75.0f);
    reactDistance = sConfigMgr->GetOption<float>("AiPlayerbot.ReactDistance", 150.0f);

    criticalHealth = sConfigMgr->GetOption<int32>("AiPlayerbot.CriticalHealth", 20);
    lowHealth = sConfigMgr->GetOption<int32>("AiPlayerbot.LowHealth", 45);
    mediumHealth = sConfigMgr->GetOption<int32>("AiPlayerbot.MediumHealth", 65);
    almostFullHealth = sConfigMgr->GetOption<int32>("AiPlayerbot.AlmostFullHealth", 85);
    lowMana = sConfigMgr->GetOption<int32>("AiPlayerbot.LowMana", 15);
    mediumMana = sConfigMgr->GetOption<int32>("AiPlayerbot.MediumMana", 40);

    randomGearLoweringChance = sConfigMgr->GetOption<float>("AiPlayerbot.RandomGearLoweringChance", 0.15f);
    randomBotMaxLevelChance = sConfigMgr->GetOption<float>("AiPlayerbot.RandomBotMaxLevelChance", 0.15f);
    randomBotRpgChance = sConfigMgr->GetOption<float>("AiPlayerbot.RandomBotRpgChance", 0.20f);

    iterationsPerTick = sConfigMgr->GetOption<int32>("AiPlayerbot.IterationsPerTick", 100);

    allowGuildBots = sConfigMgr->GetOption<bool>("AiPlayerbot.AllowGuildBots", true);

    randomBotMapsAsString = sConfigMgr->GetOption<std::string>("AiPlayerbot.RandomBotMaps", "0,1,530,571");
    LoadList<std::vector<uint32>>(randomBotMapsAsString, randomBotMaps);
    LoadList<std::vector<uint32>>(sConfigMgr->GetOption<std::string>("AiPlayerbot.RandomBotQuestItems", "6948,5175,5176,5177,5178,16309,12382,13704,11000"), randomBotQuestItems);
    LoadList<std::vector<uint32>>(sConfigMgr->GetOption<std::string>("AiPlayerbot.RandomBotSpellIds", "54197"), randomBotSpellIds);
    LoadList<std::vector<uint32>>(sConfigMgr->GetOption<std::string>("AiPlayerbot.PvpProhibitedZoneIds", "2255,656,2361,2362,2363,976,35,2268,3425,392,541,1446,3828,3712,3738,3565,3539,3623,4152,3988,4658,4284,4418,4436,4275,4323"), pvpProhibitedZoneIds);
    LoadList<std::vector<uint32>>(sConfigMgr->GetOption<std::string>("AiPlayerbot.RandomBotQuestIds", "7848,3802,5505,6502,7761"), randomBotQuestIds);

    botAutologin = sConfigMgr->GetOption<bool>("AiPlayerbot.BotAutologin", false);
    randomBotAutologin = sConfigMgr->GetOption<bool>("AiPlayerbot.RandomBotAutologin", true);
    minRandomBots = sConfigMgr->GetOption<int32>("AiPlayerbot.MinRandomBots", 50);
    maxRandomBots = sConfigMgr->GetOption<int32>("AiPlayerbot.MaxRandomBots", 200);
    randomBotUpdateInterval = sConfigMgr->GetOption<int32>("AiPlayerbot.RandomBotUpdateInterval", MINUTE);
    randomBotCountChangeMinInterval = sConfigMgr->GetOption<int32>("AiPlayerbot.RandomBotCountChangeMinInterval", 30 * MINUTE);
    randomBotCountChangeMaxInterval = sConfigMgr->GetOption<int32>("AiPlayerbot.RandomBotCountChangeMaxInterval", 2 * HOUR);
    minRandomBotInWorldTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MinRandomBotInWorldTime", 2 * HOUR);
    maxRandomBotInWorldTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MaxRandomBotInWorldTime", 12 * HOUR);
    minRandomBotRandomizeTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MinRandomBotRandomizeTime", 2 * HOUR);
    maxRandomBotRandomizeTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MaxRandomRandomizeTime", 14 * 24 * HOUR);
    minRandomBotChangeStrategyTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MinRandomBotChangeStrategyTime", 30 * MINUTE);
    maxRandomBotChangeStrategyTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MaxRandomBotChangeStrategyTime", 2 * HOUR);
    minRandomBotReviveTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MinRandomBotReviveTime", MINUTE);
    maxRandomBotReviveTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MaxRandomBotReviveTime", 5 * MINUTE);
    randomBotTeleportDistance = sConfigMgr->GetOption<int32>("AiPlayerbot.RandomBotTeleportDistance", 100);
    randomBotsPerInterval = sConfigMgr->GetOption<int32>("AiPlayerbot.RandomBotsPerInterval", MINUTE);
    minRandomBotsPriceChangeInterval = sConfigMgr->GetOption<int32>("AiPlayerbot.MinRandomBotsPriceChangeInterval", 2 * HOUR);
    maxRandomBotsPriceChangeInterval = sConfigMgr->GetOption<int32>("AiPlayerbot.MaxRandomBotsPriceChangeInterval", 48 * HOUR);
    randomBotJoinLfg = sConfigMgr->GetOption<bool>("AiPlayerbot.RandomBotJoinLfg", true);
    randomBotSuggestDungeons = sConfigMgr->GetOption<bool>("AiPlayerbot.RandomBotSuggestDungeons", true);
    suggestDungeonsInLowerCaseRandomly = sConfigMgr->GetOption<bool>("AiPlayerbot.SuggestDungeonsInLowerCaseRandomly", false);
    randomBotJoinBG = sConfigMgr->GetOption<bool>("AiPlayerbot.RandomBotJoinBG", true);
    logInGroupOnly = sConfigMgr->GetOption<bool>("AiPlayerbot.LogInGroupOnly", true);
    logValuesPerTick = sConfigMgr->GetOption<bool>("AiPlayerbot.LogValuesPerTick", false);
    fleeingEnabled = sConfigMgr->GetOption<bool>("AiPlayerbot.FleeingEnabled", true);
    summonAtInnkeepersEnabled = sConfigMgr->GetOption<bool>("AiPlayerbot.SummonAtInnkeepersEnabled", true);
    randomBotMinLevel = sConfigMgr->GetOption<int32>("AiPlayerbot.RandomBotMinLevel", 1);
    randomBotMaxLevel = sConfigMgr->GetOption<int32>("AiPlayerbot.RandomBotMaxLevel", 80);
    randomBotLoginAtStartup = sConfigMgr->GetOption<bool>("AiPlayerbot.RandomBotLoginAtStartup", true);
    randomBotTeleLevel = sConfigMgr->GetOption<int32>("AiPlayerbot.RandomBotTeleLevel", 5);
    openGoSpell = sConfigMgr->GetOption<int32>("AiPlayerbot.OpenGoSpell", 6477);

    randomChangeMultiplier = sConfigMgr->GetOption<float>("AiPlayerbot.RandomChangeMultiplier", 1.0);

    randomBotCombatStrategies = sConfigMgr->GetOption<std::string>("AiPlayerbot.RandomBotCombatStrategies", "-threat,+custom::say");
    randomBotNonCombatStrategies = sConfigMgr->GetOption<std::string>("AiPlayerbot.RandomBotNonCombatStrategies", "+custom::say");
    combatStrategies = sConfigMgr->GetOption<std::string>("AiPlayerbot.CombatStrategies", "+custom::say");
    nonCombatStrategies = sConfigMgr->GetOption<std::string>("AiPlayerbot.NonCombatStrategies", "+custom::say,+return");

    commandPrefix = sConfigMgr->GetOption<std::string>("AiPlayerbot.CommandPrefix", "");
    commandSeparator = sConfigMgr->GetOption<std::string>("AiPlayerbot.CommandSeparator", "\\\\");

    commandServerPort = sConfigMgr->GetOption<int32>("AiPlayerbot.CommandServerPort", 8888);
    perfMonEnabled = sConfigMgr->GetOption<bool>("AiPlayerbot.PerfMonEnabled", false);

    LOG_INFO("server.loading", "---------------------------------------");
    LOG_INFO("server.loading", "          Loading TalentSpecs          ");
    LOG_INFO("server.loading", "---------------------------------------");

    for (uint32 cls = 1; cls < MAX_CLASSES; ++cls)
    {
        classSpecs[cls] = ClassSpecs(1 << (cls - 1));

        for (uint32 spec = 0; spec < MAX_LEVEL; ++spec)
        {
            std::ostringstream os;
            os << "AiPlayerbot.PremadeSpecName." << cls << "." << spec;

            std::string const specName = sConfigMgr->GetOption<std::string>(os.str().c_str(), "", false);
            if (!specName.empty())
            {
                std::ostringstream os;
                os << "AiPlayerbot.PremadeSpecProb." << cls << "." << spec;
                uint32 probability = sConfigMgr->GetOption<int32>(os.str().c_str(), 100, false);

                TalentPath talentPath(spec, specName, probability);

                for (uint32 level = 10; level <= 80; level++)
                {
                    std::ostringstream os;
                    os << "AiPlayerbot.PremadeSpecLink." << cls << "." << spec << "." << level;

                    std::string specLink = sConfigMgr->GetOption<std::string>(os.str().c_str(), "", false);
                    specLink = specLink.substr(0, specLink.find("#", 0));;
                    specLink = specLink.substr(0, specLink.find(" ", 0));;

                    if (!specLink.empty())
                    {
                        std::ostringstream out;

                        // Ignore bad specs.
                        if (!classSpecs[cls].baseSpec.CheckTalentLink(specLink, &out))
                        {
                            LOG_ERROR("playerbots", "Error with premade spec link: {}", specLink.c_str());
                            LOG_ERROR("playerbots", "{}", out.str().c_str());
                            continue;
                        }

                        TalentSpec linkSpec(&classSpecs[cls].baseSpec, specLink);

                        if (!linkSpec.CheckTalents(level, &out))
                        {
                            LOG_ERROR("playerbots", "Error with premade spec: {}", specLink.c_str());
                            LOG_ERROR("playerbots", "{}", out.str().c_str());
                            continue;
                        }


                        talentPath.talentSpec.push_back(linkSpec);
                    }
                }

                // Only add paths that have atleast 1 spec.
                if (talentPath.talentSpec.size() > 0)
                    classSpecs[cls].talentPath.push_back(talentPath);
            }
        }
    }

    botCheats.clear();
    LoadListString<std::vector<std::string>>(sConfigMgr->GetOption<std::string>("AiPlayerbot.BotCheats", "taxi"), botCheats);

    botCheatMask = 0;

    if (std::find(botCheats.begin(), botCheats.end(), "taxi") != botCheats.end())
        botCheatMask |= (uint32)BotCheatMask::taxi;
    if (std::find(botCheats.begin(), botCheats.end(), "gold") != botCheats.end())
        botCheatMask |= (uint32)BotCheatMask::gold;
    if (std::find(botCheats.begin(), botCheats.end(), "health") != botCheats.end())
        botCheatMask |= (uint32)BotCheatMask::health;
    if (std::find(botCheats.begin(), botCheats.end(), "mana") != botCheats.end())
        botCheatMask |= (uint32)BotCheatMask::mana;
    if (std::find(botCheats.begin(), botCheats.end(), "power") != botCheats.end())
        botCheatMask |= (uint32)BotCheatMask::power;

    LoadListString<std::vector<std::string>>(sConfigMgr->GetOption<std::string>("AiPlayerbot.AllowedLogFiles", ""), allowedLogFiles);

    worldBuffs.clear();

    for (uint32 factionId = 0; factionId < 3; factionId++)
    {
        for (uint32 classId = 0; classId < MAX_CLASSES; classId++)
        {
            for (uint32 minLevel = 0; minLevel < MAX_LEVEL; minLevel++)
            {
                for (uint32 maxLevel = 0; maxLevel < MAX_LEVEL; maxLevel++)
                {
                    loadWorldBuf(factionId, classId, minLevel, maxLevel);
                }
            }
        }
    }

    randomBotAccountPrefix = sConfigMgr->GetOption<std::string>("AiPlayerbot.RandomBotAccountPrefix", "rndbot");
    randomBotAccountCount = sConfigMgr->GetOption<int32>("AiPlayerbot.RandomBotAccountCount", 200);
    deleteRandomBotAccounts = sConfigMgr->GetOption<bool>("AiPlayerbot.DeleteRandomBotAccounts", false);
    randomBotGuildCount = sConfigMgr->GetOption<int32>("AiPlayerbot.RandomBotGuildCount", 20);
    deleteRandomBotGuilds = sConfigMgr->GetOption<bool>("AiPlayerbot.DeleteRandomBotGuilds", false);

    guildTaskEnabled = sConfigMgr->GetOption<bool>("AiPlayerbot.EnableGuildTasks", true);
    minGuildTaskChangeTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MinGuildTaskChangeTime", 3 * 24 * 3600);
    maxGuildTaskChangeTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MaxGuildTaskChangeTime", 4 * 24 * 3600);
    minGuildTaskAdvertisementTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MinGuildTaskAdvertisementTime", 300);
    maxGuildTaskAdvertisementTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MaxGuildTaskAdvertisementTime", 12 * 3600);
    minGuildTaskRewardTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MinGuildTaskRewardTime", 300);
    maxGuildTaskRewardTime = sConfigMgr->GetOption<int32>("AiPlayerbot.MaxGuildTaskRewardTime", 3600);
    guildTaskAdvertCleanupTime = sConfigMgr->GetOption<int32>("AiPlayerbot.GuildTaskAdvertCleanupTime", 300);
    guildTaskKillTaskDistance = sConfigMgr->GetOption<int32>("AiPlayerbot.GuildTaskKillTaskDistance", 2000);
    targetPosRecalcDistance = sConfigMgr->GetOption<float>("AiPlayerbot.TargetPosRecalcDistance", 0.1f);

    // cosmetics (by lidocain)
    randomBotShowCloak = sConfigMgr->GetOption<bool>("AiPlayerbot.RandomBotShowCloak", true);
    randomBotShowHelmet = sConfigMgr->GetOption<bool>("AiPlayerbot.RandomBotShowHelmet", true);

    // SPP switches
    enableGreet = sConfigMgr->GetOption<bool>("AiPlayerbot.EnableGreet", true);
    disableRandomLevels = sConfigMgr->GetOption<bool>("AiPlayerbot.DisableRandomLevels", false);
    randomBotRandomPassword = sConfigMgr->GetOption<bool>("AiPlayerbot.RandomBotRandomPassword", true);
    playerbotsXPrate = sConfigMgr->GetOption<int32>("AiPlayerbot.KillXPRate", 1);
    botActiveAlone = sConfigMgr->GetOption<int32>("AiPlayerbot.BotActiveAlone", 10);
    randombotsWalkingRPG = sConfigMgr->GetOption<bool>("AiPlayerbot.RandombotsWalkingRPG", false);
    randombotsWalkingRPGInDoors = sConfigMgr->GetOption<bool>("AiPlayerbot.RandombotsWalkingRPG.InDoors", false);
    minEnchantingBotLevel = sConfigMgr->GetOption<int32>("AiPlayerbot.MinEnchantingBotLevel", 60);
    randombotStartingLevel = sConfigMgr->GetOption<int32>("AiPlayerbot.RandombotStartingLevel", 5);
    gearscorecheck = sConfigMgr->GetOption<bool>("AiPlayerbot.GearScoreCheck", false);
    randomBotPreQuests = sConfigMgr->GetOption<bool>("AiPlayerbot.PreQuests", true);

    // SPP automation
    autoPickReward = sConfigMgr->GetOption<std::string>("AiPlayerbot.AutoPickReward", "yes");
    autoEquipUpgradeLoot = sConfigMgr->GetOption<bool>("AiPlayerbot.AutoEquipUpgradeLoot", true);
    syncQuestWithPlayer = sConfigMgr->GetOption<bool>("AiPlayerbot.SyncQuestWithPlayer", false);
    syncQuestForPlayer = sConfigMgr->GetOption<bool>("AiPlayerbot.SyncQuestForPlayer", false);
    autoTrainSpells = sConfigMgr->GetOption<std::string>("AiPlayerbot.AutoTrainSpells", "yes");
    autoPickTalents = sConfigMgr->GetOption<std::string>("AiPlayerbot.AutoPickTalents", "full");
    autoLearnTrainerSpells = sConfigMgr->GetOption<bool>("AiPlayerbot.AutoLearnTrainerSpells", false);
    autoLearnQuestSpells = sConfigMgr->GetOption<bool>("AiPlayerbot.AutoLearnQuestSpells", false);
    autoDoQuests = sConfigMgr->GetOption<bool>("AiPlayerbot.AutoDoQuests", false);
    syncLevelWithPlayers = sConfigMgr->GetOption<bool>("AiPlayerbot.SyncLevelWithPlayers", false);
    freeFood = sConfigMgr->GetOption<bool>("AiPlayerbot.FreeFood", true);
    randomBotSayWithoutMaster = sConfigMgr->GetOption<bool>("AiPlayerbot.RandomBotSayWithoutMaster", false);
    randomBotGroupNearby = sConfigMgr->GetOption<bool>("AiPlayerbot.RandomBotGroupNearby", true);

    // arena
    randomBotArenaTeamCount = sConfigMgr->GetOption<int32>("AiPlayerbot.RandomBotArenaTeamCount", 20);
    deleteRandomBotArenaTeams = sConfigMgr->GetOption<bool>("AiPlayerbot.DeleteRandomBotArenaTeams", false);

    selfBotLevel = sConfigMgr->GetOption<int32>("AiPlayerbot.SelfBotLevel", 1);

    RandomPlayerbotFactory::CreateRandomBots();
    PlayerbotFactory::Init();
    sRandomItemMgr->Init();
    sRandomItemMgr->InitAfterAhBot();
    sPlayerbotTextMgr->LoadBotTexts();
    sPlayerbotTextMgr->LoadBotTextChance();

    if (!sPlayerbotAIConfig->autoDoQuests)
    {
        LOG_INFO("server.loading", "Loading Quest Detail Data...");
        sTravelMgr->LoadQuestTravelTable();
    }

    if (sPlayerbotAIConfig->randomBotJoinBG)
        sRandomPlayerbotMgr->LoadBattleMastersCache();

    if (sPlayerbotAIConfig->randomBotSuggestDungeons)
    {
        sPlayerbotDungeonSuggestionMgr->LoadDungeonSuggestions();
    }

    LOG_INFO("server.loading", "---------------------------------------");
    LOG_INFO("server.loading", "        AI Playerbots initialized       ");
    LOG_INFO("server.loading", "---------------------------------------");

    return true;
}

bool PlayerbotAIConfig::IsInRandomAccountList(uint32 id)
{
    return find(randomBotAccounts.begin(), randomBotAccounts.end(), id) != randomBotAccounts.end();
}

bool PlayerbotAIConfig::IsInRandomQuestItemList(uint32 id)
{
    return find(randomBotQuestItems.begin(), randomBotQuestItems.end(), id) != randomBotQuestItems.end();
}

bool PlayerbotAIConfig::IsInPvpProhibitedZone(uint32 id)
{
    return find(pvpProhibitedZoneIds.begin(), pvpProhibitedZoneIds.end(), id) != pvpProhibitedZoneIds.end();
}

std::string const PlayerbotAIConfig::GetTimestampStr()
{
    time_t t = time(nullptr);
    tm* aTm = localtime(&t);
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    char buf[20];
    snprintf(buf, 20, "%04d-%02d-%02d %02d-%02d-%02d", aTm->tm_year + 1900, aTm->tm_mon + 1, aTm->tm_mday, aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
    return std::string(buf);
}

bool PlayerbotAIConfig::openLog(std::string const fileName, char const* mode)
{
    if (!hasLog(fileName))
        return false;

    auto logFileIt = logFiles.find(fileName);
    if (logFileIt == logFiles.end())
    {
        logFiles.insert(std::make_pair(fileName, std::make_pair(nullptr, false)));
        logFileIt = logFiles.find(fileName);
    }

    FILE* file = logFileIt->second.first;
    bool fileOpen = logFileIt->second.second;

    if (fileOpen) //close log file
        fclose(file);

    std::string m_logsDir = sConfigMgr->GetOption<std::string>("LogsDir", "", false);
    if (!m_logsDir.empty())
    {
        if ((m_logsDir.at(m_logsDir.length() - 1) != '/') && (m_logsDir.at(m_logsDir.length() - 1) != '\\'))
            m_logsDir.append("/");
    }


    file = fopen((m_logsDir + fileName).c_str(), mode);
    fileOpen = true;

    logFileIt->second.first = file;
    logFileIt->second.second = fileOpen;

    return true;
}

void PlayerbotAIConfig::log(std::string const fileName, char const* str, ...)
{
    if (!str)
        return;

    std::lock_guard<std::mutex> guard(m_logMtx);

    if (!isLogOpen(fileName) && !openLog(fileName, "a"))
        return;

    FILE* file = logFiles.find(fileName)->second.first;

    va_list ap;
    va_start(ap, str);
    vfprintf(file, str, ap);
    fprintf(file, "\n");
    va_end(ap);
    fflush(file);

    fflush(stdout);
}

void PlayerbotAIConfig::loadWorldBuf(uint32 factionId1, uint32 classId1, uint32 minLevel1, uint32 maxLevel1)
{
    std::vector<uint32> buffs;

    std::ostringstream os;
    os << "AiPlayerbot.WorldBuff." << factionId1 << "." << classId1 << "." << minLevel1 << "." << maxLevel1;

    LoadList<std::vector<uint32>>(sConfigMgr->GetOption<std::string>(os.str().c_str(), "", false), buffs);

    for (auto buff : buffs)
    {
        worldBuff wb = { buff, factionId1, classId1, minLevel1, maxLevel1 };
        worldBuffs.push_back(wb);
    }

    if (maxLevel1 == 0)
    {
        std::ostringstream os;
        os << "AiPlayerbot.WorldBuff." << factionId1 << "." << classId1 << "." << minLevel1;

        LoadList<std::vector<uint32>>(sConfigMgr->GetOption<std::string>(os.str().c_str(), "", false), buffs);

        for (auto buff : buffs)
        {
            worldBuff wb = { buff, factionId1, classId1, minLevel1, maxLevel1 };
            worldBuffs.push_back(wb);
        }
    }

    if (maxLevel1 == 0 && minLevel1 == 0)
    {
        std::ostringstream os;
        os << "AiPlayerbot.WorldBuff." << factionId1 << "." << factionId1 << "." << classId1;

        LoadList<std::vector<uint32>>(sConfigMgr->GetOption<std::string>(os.str().c_str(), "", false), buffs);

        for (auto buff : buffs)
        {
            worldBuff wb = { buff, factionId1, classId1, minLevel1, maxLevel1 };
            worldBuffs.push_back(wb);
        }
    }

    if (classId1 == 0 && maxLevel1 == 0 && minLevel1 == 0)
    {
        std::ostringstream os;
        os << "AiPlayerbot.WorldBuff." << factionId1;

        LoadList<std::vector<uint32>>(sConfigMgr->GetOption<std::string>(os.str().c_str(), "", false), buffs);

        for (auto buff : buffs)
        {
            worldBuff wb = { buff, factionId1, classId1, minLevel1, maxLevel1 };
            worldBuffs.push_back(wb);
        }
    }

    if (factionId1 == 0 && classId1 == 0 && maxLevel1 == 0 && minLevel1 == 0)
    {
        std::ostringstream os;
        os << "AiPlayerbot.WorldBuff";

        LoadList<std::vector<uint32>>(sConfigMgr->GetOption<std::string>(os.str().c_str(), "", false), buffs);

        for (auto buff : buffs)
        {
            worldBuff wb = { buff, factionId1, classId1, minLevel1, maxLevel1 };
            worldBuffs.push_back(wb);
        }
    }
}
