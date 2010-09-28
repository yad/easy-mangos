/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Config/Config.h"
#include "../Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotMgr.h"
#include "../WorldPacket.h"
#include "../Chat.h"
#include "../ObjectMgr.h"
#include "../GossipDef.h"
#include "../Chat.h"
#include "../Language.h"
#include "../Guild.h"

class LoginQueryHolder;
class CharacterHandler;

PlayerbotMgr::PlayerbotMgr()
{
}

PlayerbotMgr::~PlayerbotMgr()
{
}

void PlayerbotMgr::UpdateAI(const uint32 p_time) {}

void PlayerbotMgr::HandleMasterIncomingPacket(const WorldPacket& packet)
{
    switch (packet.GetOpcode())
    {
        // if master is logging out, log out all bots
        case CMSG_LOGOUT_REQUEST:
        {
            PlayerbotMgr::RemoveAllBotsFromGroup(m_master);
            return;
        }

        // If master inspects one of his bots, give the master useful info in chat window
        // such as inventory that can be equipped
        case CMSG_INSPECT:
        {
            WorldPacket p(packet);
            p.rpos(0); // reset reader
            uint64 guid;
            p >> guid;
            Player* const bot = GetPlayerBot(guid);
            if (bot) bot->GetPlayerbotAI()->SendNotEquipList(*bot);
            return;
        }

        // handle emotes from the master
        //case CMSG_EMOTE:
        case CMSG_TEXT_EMOTE:
        {
            WorldPacket p(packet);
            p.rpos(0); // reset reader
            uint32 emoteNum;
            p >> emoteNum;

            /* std::ostringstream out;
               out << "emote is: " << emoteNum;
               ChatHandler ch(m_master);
               ch.SendSysMessage(out.str().c_str()); */

            switch (emoteNum)
            {
                case TEXTEMOTE_BOW:
                {
                    // Buff anyone who bows before me. Useful for players not in bot's group
                    // How do I get correct target???
                    return;
                }
                /*
                   case TEXTEMOTE_BONK:
                   {
                    Player* const pPlayer = GetPlayerBot(m_master->GetSelection());
                    if (!pPlayer || !pPlayer->GetPlayerbotAI())
                        return;
                    PlayerbotAI* const pBot = pPlayer->GetPlayerbotAI();

                    ChatHandler ch(m_master);
                    {
                        std::ostringstream out;
                        out << "time(0): " << time(0)
                            << " m_ignoreAIUpdatesUntilTime: " << pBot->m_ignoreAIUpdatesUntilTime;
                        ch.SendSysMessage(out.str().c_str());
                    }
                    {
                        std::ostringstream out;
                        out << "m_TimeDoneEating: " << pBot->m_TimeDoneEating
                            << " m_TimeDoneDrinking: " << pBot->m_TimeDoneDrinking;
                        ch.SendSysMessage(out.str().c_str());
                    }
                    {
                        std::ostringstream out;
                        out << "m_CurrentlyCastingSpellId: " << pBot->m_CurrentlyCastingSpellId;
                        ch.SendSysMessage(out.str().c_str());
                    }
                    {
                        std::ostringstream out;
                        out << "IsBeingTeleported() " << pBot->GetPlayer()->IsBeingTeleported();
                        ch.SendSysMessage(out.str().c_str());
                    }
                    {
                        std::ostringstream out;
                        bool tradeActive = (pBot->GetPlayer()->GetTrader()) ? true : false;
                        out << "tradeActive: " << tradeActive;
                        ch.SendSysMessage(out.str().c_str());
                    }
                    {
                        std::ostringstream out;
                        out << "IsCharmed() " << pBot->getPlayer()->isCharmed();
                        ch.SendSysMessage(out.str().c_str());
                    }
                    return;
                   }
                 */

                case TEXTEMOTE_EAT:
                case TEXTEMOTE_DRINK:
                {
                    if (!m_master->GetGroup())
                        return;

                    for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
                    {
                        Player* const bot = itr->getSource();
                        bot->GetPlayerbotAI()->Feast();
                    }
                    return;
                }

                // emote to attack selected target
                case TEXTEMOTE_POINT:
                {
                    uint64 attackOnGuid = m_master->GetSelection();
                    if (!attackOnGuid) return;

                    Unit* thingToAttack = ObjectAccessor::GetUnit(*m_master, attackOnGuid);
                    if (!thingToAttack) return;

                    Player *bot = 0;
                    if (!m_master->GetGroup())
                        return;

                    for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
                    {
                        bot = itr->getSource();
                        if (!bot->IsFriendlyTo(thingToAttack) && bot->IsWithinLOSInMap(thingToAttack))
                            bot->GetPlayerbotAI()->GetCombatTarget(thingToAttack);
                    }
                    return;
                }

                // emote to stay
                case TEXTEMOTE_STAND:
                {
                    Player* const bot = GetPlayerBot(m_master->GetSelection());
                    if (bot)
                        bot->GetPlayerbotAI()->SetMovementOrder(PlayerbotAI::MOVEMENT_STAY);
                    else
                    {
                        if (!m_master->GetGroup())
                            return;

                        for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
                        {
                            Player* const bot = itr->getSource();
                            bot->GetPlayerbotAI()->SetMovementOrder(PlayerbotAI::MOVEMENT_STAY);
                        }
                    }
                    return;
                }

                // 324 is the followme emote (not defined in enum)
                // if master has bot selected then only bot follows, else all bots follow
                case 324:
                case TEXTEMOTE_WAVE:
                {
                    Player* const bot = GetPlayerBot(m_master->GetSelection());
                    if (bot)
                        bot->GetPlayerbotAI()->SetMovementOrder(PlayerbotAI::MOVEMENT_FOLLOW, m_master);
                    else
                    {
                        if (!m_master->GetGroup())
                            return;

                        for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
                        {
                            Player* const bot = itr->getSource();
                            bot->GetPlayerbotAI()->SetMovementOrder(PlayerbotAI::MOVEMENT_FOLLOW, m_master);
                        }
                    }
                    return;
                }
            }

            return;
        } /* EMOTE ends here */

        case CMSG_GAMEOBJ_USE:
        {
            WorldPacket p(packet);
            p.rpos(0);     // reset reader
            uint64 objGUID;
            p >> objGUID;

            GameObject *obj = m_master->GetMap()->GetGameObject(objGUID);
            if (!obj)
                return;

            if (!m_master->GetGroup())
                return;

            for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* const bot = itr->getSource();

                if (obj->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
                    bot->GetPlayerbotAI()->TurnInQuests(obj);
                 // add other go types here, i.e.:
                // GAMEOBJECT_TYPE_CHEST - loot quest items of chest
            }

            return;
        }
        break;

        // if master talks to an NPC
        case CMSG_GOSSIP_HELLO:
        case CMSG_QUESTGIVER_HELLO:
        {
            WorldPacket p(packet);
            p.rpos(0);    // reset reader
            uint64 npcGUID;
            p >> npcGUID;

            WorldObject* pNpc = m_master->GetMap()->GetWorldObject(npcGUID);
            if (!pNpc)
                return;

            if (!m_master->GetGroup())
                return;                

            // for all master's bots
            for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* const bot = itr->getSource();
                bot->GetPlayerbotAI()->TurnInQuests(pNpc);
            }

            return;
        }

        // if master accepts a quest, bots should also try to accept quest
        case CMSG_QUESTGIVER_ACCEPT_QUEST:
        {
            WorldPacket p(packet);
            p.rpos(0);    // reset reader
            uint64 guid;
            uint32 quest;
            p >> guid >> quest;
            Quest const* qInfo = sObjectMgr.GetQuestTemplate(quest);
            if (qInfo)
            {
                if (!m_master->GetGroup())
                    return;

                for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
                {
                    Player* const bot = itr->getSource();

                    if (bot->GetQuestStatus(quest) == QUEST_STATUS_COMPLETE)
                        bot->GetPlayerbotAI()->TellMaster("J'ai deja fini cette quete.");
                    else if (!bot->CanTakeQuest(qInfo, false))
                    {
                        if (!bot->SatisfyQuestStatus(qInfo, false))
                            bot->GetPlayerbotAI()->TellMaster("Je suis deja sur cette quete.");
                        else
                            bot->GetPlayerbotAI()->TellMaster("Je ne peux pas prendre cette quete.");
                    }
                    else if (!bot->SatisfyQuestLog(false))
                        bot->GetPlayerbotAI()->TellMaster("Mon journal de quete est plein.");
                    else if (!bot->CanAddQuest(qInfo, false))
                        bot->GetPlayerbotAI()->TellMaster("Je ne peux pas prendre cette quete car je dois ramasser des objets pour la terminer et mes sacs sont pleins :(");

                    else
                    {
                        p.rpos(0);         // reset reader
                        bot->GetSession()->HandleQuestgiverAcceptQuestOpcode(p);
                        bot->GetPlayerbotAI()->TellMaster("J'ai pris la quete.");
                    }
                }
            }
            return;
        }
        case CMSG_LOOT_ROLL:
        {

            WorldPacket p(packet);    //WorldPacket packet for CMSG_LOOT_ROLL, (8+4+1)
            uint64 Guid;
            uint32 NumberOfPlayers;
            uint8 rollType;
            p.rpos(0);    //reset packet pointer
            p >> Guid;    //guid of the item rolled
            p >> NumberOfPlayers;    //number of players invited to roll
            p >> rollType;    //need,greed or pass on roll

            if (!m_master->GetGroup())
                return;

            for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                uint32 choice = urand(0, 3);    //returns 0,1,2 or 3

                Player* const bot = itr->getSource();
                if(!bot)
                     return;

                Group* group = bot->GetGroup();
                if (!group)
                    return;

                group->CountRollVote(bot, Guid, NumberOfPlayers, RollVote(choice));

                switch (choice)
                {
                    case ROLL_NEED:
                        bot->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED, 1);
                        break;
                    case ROLL_GREED:
                        bot->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED, 1);
                        break;
                }
            }
            return;
        }
        case CMSG_REPAIR_ITEM:
        {

            WorldPacket p(packet);    // WorldPacket packet for CMSG_REPAIR_ITEM, (8+8+1)

            sLog.outDebug("PlayerbotMgr: CMSG_REPAIR_ITEM");

            uint64 npcGUID, itemGUID;
            uint8 guildBank;

            p.rpos(0);    //reset packet pointer
            p >> npcGUID;
            p >> itemGUID;     // Not used for bot but necessary opcode data retrieval
            p >> guildBank;    // Flagged if guild repair selected

            if (!m_master->GetGroup())
                return;

            for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* const bot = itr->getSource();
                if (!bot)
                    return;

                Group* group = bot->GetGroup();      // check if bot is a member of group
                if (!group)
                    return;

                Creature *unit = bot->GetNPCIfCanInteractWith(npcGUID, UNIT_NPC_FLAG_REPAIR);
                if (!unit)     // Check if NPC can repair bot or not
                {
                    sLog.outDebug("PlayerbotMgr: HandleRepairItemOpcode - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(npcGUID)));
                    return;
                }

                // remove fake death
                if (bot->hasUnitState(UNIT_STAT_DIED))
                    bot->RemoveSpellsCausingAura(SPELL_AURA_FEIGN_DEATH);

                // reputation discount
                float discountMod = bot->GetReputationPriceDiscount(unit);

                uint32 TotalCost = 0;
                if (itemGUID)     // Handle redundant feature (repair individual item) for bot
                {
                    sLog.outDebug("ITEM: Repair single item is not applicable for %s", bot->GetName());
                    continue;
                }
                else      // Handle feature (repair all items) for bot
                {
                    sLog.outDebug("ITEM: Repair all items, npcGUID = %u", GUID_LOPART(npcGUID));

                    TotalCost = bot->DurabilityRepairAll(true, discountMod, guildBank > 0 ? true : false);
                }
                if (guildBank)     // Handle guild repair
                {
                    uint32 GuildId = bot->GetGuildId();
                    if (!GuildId)
                        return;
                    Guild *pGuild = sObjectMgr.GetGuildById(GuildId);
                    if (!pGuild)
                        return;
                    pGuild->LogBankEvent(GUILD_BANK_LOG_REPAIR_MONEY, 0, bot->GetGUIDLow(), TotalCost);
                    pGuild->SendMoneyInfo(bot->GetSession(), bot->GetGUIDLow());
                }

            }
            return;
        }
        case CMSG_SPIRIT_HEALER_ACTIVATE:
        {
            // sLog.outDebug("SpiritHealer is resurrecting the Player %s",m_master->GetName());
            if (!m_master->GetGroup())
                return;

            for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* const bot = itr->getSource();
                Group *grp = bot->GetGroup();
                if (grp)
                    grp->RemoveMember(bot->GetGUID(), 1);
            }
            return;
        }

            /*
               case CMSG_NAME_QUERY:
               case MSG_MOVE_START_FORWARD:
               case MSG_MOVE_STOP:
               case MSG_MOVE_SET_FACING:
               case MSG_MOVE_START_STRAFE_LEFT:
               case MSG_MOVE_START_STRAFE_RIGHT:
               case MSG_MOVE_STOP_STRAFE:
               case MSG_MOVE_START_BACKWARD:
               case MSG_MOVE_HEARTBEAT:
               case CMSG_STANDSTATECHANGE:
               case CMSG_QUERY_TIME:
               case CMSG_CREATURE_QUERY:
               case CMSG_GAMEOBJECT_QUERY:
               case MSG_MOVE_JUMP:
               case MSG_MOVE_FALL_LAND:
                return;

               default:
               {
                const char* oc = LookupOpcodeName(packet.GetOpcode());
                // ChatHandler ch(m_master);
                // ch.SendSysMessage(oc);

                std::ostringstream out;
                out << "masterin: " << oc;
                sLog.outDebug(out.str().c_str());
               }
             */
    }
}

void PlayerbotMgr::HandleMasterOutgoingPacket(const WorldPacket& packet)
{
    /*
       switch (packet.GetOpcode())
       {
        // maybe our bots should only start looting after the master loots?
        //case SMSG_LOOT_RELEASE_RESPONSE: {}
        case SMSG_NAME_QUERY_RESPONSE:
        case SMSG_MONSTER_MOVE:
        case SMSG_COMPRESSED_UPDATE_OBJECT:
        case SMSG_DESTROY_OBJECT:
        case SMSG_UPDATE_OBJECT:
        case SMSG_STANDSTATE_UPDATE:
        case MSG_MOVE_HEARTBEAT:
        case SMSG_QUERY_TIME_RESPONSE:
        case SMSG_AURA_UPDATE_ALL:
        case SMSG_CREATURE_QUERY_RESPONSE:
        case SMSG_GAMEOBJECT_QUERY_RESPONSE:
            return;
        default:
        {
            const char* oc = LookupOpcodeName(packet.GetOpcode());

            std::ostringstream out;
            out << "masterout: " << oc;
            sLog.outDebug(out.str().c_str());
        }
       }
     */
}

void PlayerbotMgr::Stay()
{
    /*if (!player->GetGroup())
        return;

    for (GroupReference *itr = player->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* const bot = itr->getSource();
        bot->GetMotionMaster()->Clear();
    }*/
}


// Playerbot mod: logs out a Playerbot.
void PlayerbotMgr::LogoutPlayerBot(uint64 guid)
{
    Player* bot = GetPlayerBot(guid);
    if (bot)
    {
        WorldSession * botWorldSessionPtr = bot->GetSession();
        botWorldSessionPtr->LogoutPlayer(true); // this will delete the bot Player object and PlayerbotAI object
        delete botWorldSessionPtr;  // finally delete the bot's WorldSession
    }
}

// Playerbot mod: Gets a player bot Player object for this WorldSession master
Player* PlayerbotMgr::GetPlayerBot(uint64 playerGuid) const
{
    HashMapHolder < Player > ::MapType& m = sObjectAccessor.GetPlayers();
    for (HashMapHolder < Player > ::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        Player* const bot = itr->second;
        if (bot && bot->IsBot() && (bot->GetGUID() == playerGuid))
            return bot;
    }
    return NULL;
}

void PlayerbotMgr::OnBotLogin(Player * const bot)
{
    PlayerbotAI* ai = bot->GetPlayerbotAI();
    if (!ai)
    {
        PlayerbotAI* ai = new PlayerbotAI(this, bot);
        if (ai)
            bot->SetPlayerbotAI(ai);
    }
    ai = bot->GetPlayerbotAI();

    if (bot->GetGroup())
        bot->RemoveFromGroup();

    ChatHandler ch(bot);
    bot->RemoveMyEquipement();
    bot->GiveLevel(bot->GetLevelAtLoading());
    ch.HandleGMStartUpCommand("");
    bot->SetHealth(bot->GetMaxHealth());
    bot->SetPower(bot->getPowerType(), bot->GetMaxPower(bot->getPowerType()));
}

void PlayerbotMgr::RemoveAllBotsFromGroup(Player* player)
{
    sLog.outDebug("PlayerbotMgr::RemoveAllBotsInGroup(...)");
    bool removed = false;
    do
    {
        removed = false;
        Player* bot = NULL;

        if (player->GetGroup())
        {
            GroupReference *ref = player->GetGroup()->GetFirstMember();
            while (ref)
            {
                bot = ref->getSource();

                if (!bot || !bot->IsBot() || bot == player)
                {
                    ref = ref->next();
                    continue;
                }

                sLog.outDebug("Player name : %s, removed", bot->GetName());
                bot->RemoveFromGroup();
                bot->GetPlayerbotMgr()->LogoutPlayerBot(bot->GetGUID());
                removed = true;
                break;
            }
        }
    }while(removed);
    PlayerbotMgr::AddAllBots();
}     

/*void Creature::LoadBotMenu(Player *pPlayer)
{

    if (pPlayer->GetPlayerbotAI()) return;
    uint64 guid = pPlayer->GetGUID();
    uint32 accountId = sObjectMgr.GetPlayerAccountIdByGUID(guid);
    QueryResult *result = CharacterDatabase.PQuery("SELECT guid, name FROM characters WHERE account='%d'", accountId);
    do
    {
        Field *fields = result->Fetch();
        uint64 guidlo = fields[0].GetUInt64();
        std::string name = fields[1].GetString();
        std::string word = "";

        if ((guid == 0) || (guid == guidlo))
        {
            //not found or himself
        }
        else
        {
            // if(sConfig.GetBoolDefault("PlayerbotAI.DisableBots", false)) return;
            // create the manager if it doesn't already exist
            if (!pPlayer->GetPlayerbotMgr())
                pPlayer->SetPlayerbotMgr(new PlayerbotMgr(pPlayer));
            if (pPlayer->GetPlayerbotMgr()->GetPlayerBot(guidlo) == NULL) // add (if not already in game)
            {
                word += "Recruit ";
                word += name;
                word += " as a Bot.";
                pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem((uint8) 9, word, guidlo, GOSSIP_OPTION_BOT, word, false);
            }
            else if (pPlayer->GetPlayerbotMgr()->GetPlayerBot(guidlo) != NULL) // remove (if in game)
            {
                word += "Dismiss ";
                word += name;
                word += " from duty.";
                pPlayer->PlayerTalkClass->GetGossipMenu().AddMenuItem((uint8) 0, word, guidlo, GOSSIP_OPTION_BOT, word, false);
            }
        }
    }
    while (result->NextRow());
    delete result;
}

void Player::chompAndTrim(std::string& str)
{
    while (str.length() > 0)
    {
        char lc = str[str.length() - 1];
        if (lc == '\r' || lc == '\n' || lc == ' ' || lc == '"' || lc == '\'')
            str = str.substr(0, str.length() - 1);
        else
            break;
        while (str.length() > 0)
        {
            char lc = str[0];
            if (lc == ' ' || lc == '"' || lc == '\'')
                str = str.substr(1, str.length() - 1);
            else
                break;
        }
    }
}

bool Player::getNextQuestId(const std::string& pString, unsigned int& pStartPos, unsigned int& pId)
{
    bool result = false;
    unsigned int i;
    for (i = pStartPos; i < pString.size(); ++i)
    {
        if (pString[i] == ',')
            break;
    }
    if (i > pStartPos)
    {
        std::string idString = pString.substr(pStartPos, i - pStartPos);
        pStartPos = i + 1;
        chompAndTrim(idString);
        pId = atoi(idString.c_str());
        result = true;
    }
    return(result);
}

bool Player::requiredQuests(const char* pQuestIdString)
{
    if (pQuestIdString != NULL)
    {
        unsigned int pos = 0;
        unsigned int id;
        std::string confString(pQuestIdString);
        chompAndTrim(confString);
        while (getNextQuestId(confString, pos, id))
        {
            QuestStatus status = GetQuestStatus(id);
            if (status == QUEST_STATUS_COMPLETE)
                return true;
        }
    }
    return false;
}

bool ChatHandler::HandlePlayerbotCommand(char* args)
{
    if (!(m_session->GetSecurity() > SEC_PLAYER))
        if (botConfig.GetBoolDefault("PlayerbotAI.DisableBots", false))
        {
            PSendSysMessage("|cffff0000Playerbot system is currently disabled!");
            SetSentErrorMessage(true);
            return false;
        }

    if (!m_session)
    {
        PSendSysMessage("|cffff0000You may only add bots from an active session");
        SetSentErrorMessage(true);
        return false;
    }

    if (!*args)
    {
        PSendSysMessage("|cffff0000usage: add PLAYERNAME  or  remove PLAYERNAME");
        SetSentErrorMessage(true);
        return false;
    }

    char *cmd = strtok ((char*) args, " ");
    char *charname = strtok (NULL, " ");
    if (!cmd || !charname)
    {
        PSendSysMessage("|cffff0000usage: add PLAYERNAME  or  remove PLAYERNAME");
        SetSentErrorMessage(true);
        return false;
    }

    std::string cmdStr = cmd;
    std::string charnameStr = charname;

    if (!normalizePlayerName(charnameStr))
        return false;

    uint64 guid = sObjectMgr.GetPlayerGUIDByName(charnameStr.c_str());
    if (guid == 0 || (guid == m_session->GetPlayer()->GetGUID()))
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 accountId = sObjectMgr.GetPlayerAccountIdByGUID(guid);
    if (accountId != m_session->GetAccountId())
    {
        PSendSysMessage("|cffff0000You may only add bots from the same account.");
        SetSentErrorMessage(true);
        return false;
    }

    // create the playerbot manager if it doesn't already exist
    PlayerbotMgr* mgr = m_session->GetPlayer()->GetPlayerbotMgr();
    if (!mgr)
    {
        mgr = new PlayerbotMgr(m_session->GetPlayer());
        m_session->GetPlayer()->SetPlayerbotMgr(mgr);
    }

    QueryResult *resultchar = CharacterDatabase.PQuery("SELECT COUNT(*) FROM characters WHERE online = '1' AND account = '%u'", m_session->GetAccountId());
    if (resultchar)
    {
        Field *fields = resultchar->Fetch();
        int acctcharcount = fields[0].GetUInt32();
        int maxnum = botConfig.GetIntDefault("PlayerbotAI.MaxNumBots", 9);
        if (!(m_session->GetSecurity() > SEC_PLAYER))
            if (acctcharcount > maxnum && (cmdStr == "add" || cmdStr == "login"))
            {
                PSendSysMessage("|cffff0000You cannot summon anymore bots.(Current Max: |cffffffff%u)", maxnum);
                SetSentErrorMessage(true);
                delete resultchar;
                return false;
            }
    }
    delete resultchar;

    QueryResult *resultlvl = CharacterDatabase.PQuery("SELECT level,name FROM characters WHERE guid = '%lu'", guid);
    if (resultlvl)
    {
        Field *fields = resultlvl->Fetch();
        int charlvl = fields[0].GetUInt32();
        int maxlvl = botConfig.GetIntDefault("PlayerbotAI.RestrictBotLevel", 80);
        if (!(m_session->GetSecurity() > SEC_PLAYER))
            if (charlvl > maxlvl)
            {
                PSendSysMessage("|cffff0000You cannot summon |cffffffff[%s]|cffff0000, it's level is too high.(Current Max:lvl |cffffffff%u)", fields[1].GetString(), maxlvl);
                SetSentErrorMessage(true);
                delete resultlvl;
                return false;
            }
    }
    delete resultlvl;
    // end of gmconfig patch
    if (cmdStr == "add" || cmdStr == "login")
    {
        if (mgr->GetPlayerBot(guid))
        {
            PSendSysMessage("Bot already exists in world.");
            SetSentErrorMessage(true);
            return false;
        }
        CharacterDatabase.DirectPExecute("UPDATE characters SET online = 1 WHERE guid = '%lu'", guid);
        mgr->AddPlayerBot(guid);
        PSendSysMessage("Bot added successfully.");
    }
    else if (cmdStr == "remove" || cmdStr == "logout")
    {
        if (!mgr->GetPlayerBot(guid))
        {
            PSendSysMessage("|cffff0000Bot can not be removed because bot does not exist in world.");
            SetSentErrorMessage(true);
            return false;
        }
        CharacterDatabase.DirectPExecute("UPDATE characters SET online = 0 WHERE guid = '%lu'", guid);
        mgr->LogoutPlayerBot(guid);
        PSendSysMessage("Bot removed successfully.");
    }
    else if (cmdStr == "co" || cmdStr == "combatorder")
    {
        Unit *target = NULL;
        char *orderChar = strtok(NULL, " ");
        if (!orderChar)
        {
            PSendSysMessage("|cffff0000Syntax error:|cffffffff .bot co <botName> <order=reset|tank|assist|heal|protect> [targetPlayer]");
            SetSentErrorMessage(true);
            return false;
        }
        std::string orderStr = orderChar;
        if (orderStr == "protect" || orderStr == "assist")
        {
            char *targetChar = strtok(NULL, " ");
            uint64 targetGUID = m_session->GetPlayer()->GetSelection();
            if (!targetChar && !targetGUID)
            {
                PSendSysMessage("|cffff0000Combat orders protect and assist expect a target either by selection or by giving target player in command string!");
                SetSentErrorMessage(true);
                return false;
            }
            if (targetChar)
            {
                std::string targetStr = targetChar;
                targetGUID = sObjectMgr.GetPlayerGUIDByName(targetStr.c_str());
            }
            target = ObjectAccessor::GetUnit(*m_session->GetPlayer(), targetGUID);
            if (!target)
            {
                PSendSysMessage("|cffff0000Invalid target for combat order protect or assist!");
                SetSentErrorMessage(true);
                return false;
            }
        }
        if (mgr->GetPlayerBot(guid) == NULL)
        {
            PSendSysMessage("|cffff0000Bot can not receive combat order because bot does not exist in world.");
            SetSentErrorMessage(true);
            return false;
        }
        mgr->GetPlayerBot(guid)->GetPlayerbotAI()->SetCombatOrderByStr(orderStr, target);
    }
    return true;
}*/

