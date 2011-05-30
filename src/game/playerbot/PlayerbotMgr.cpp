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
#include "WorldPacket.h"
#include "../Chat.h"
#include "../ObjectMgr.h"
#include "../GuildMgr.h"
#include "../GossipDef.h"
#include "../Chat.h"
#include "../Language.h"
#include "../Guild.h"
#include "../ArenaTeam.h"

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
        // handle emotes from the master
        //case CMSG_EMOTE:
        case CMSG_TEXT_EMOTE:
        {
            WorldPacket p(packet);
            p.rpos(0); // reset reader
            uint32 emoteNum;
            p >> emoteNum;
            switch (emoteNum)
            {
                case TEXTEMOTE_BOW:
                {
                    // Buff anyone who bows before me. Useful for players not in bot's group
                    // How do I get correct target???
                    //Player* const pPlayer = GetPlayerBot(m_master->GetSelection());
                    //if (pPlayer->GetPlayerbotAI()->GetClassAI())
                    //    pPlayer->GetPlayerbotAI()->GetClassAI()->BuffPlayer(pPlayer);
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

                            << " m_ignoreAIUpdatesUntilTime: " << pBot->m_ignoreAIUpdatesUntilTime;
                        ch.SendSysMessage(out.str().c_str());
                    }
                    {

                            << " m_TimeDoneDrinking: " << pBot->m_TimeDoneDrinking;
                        ch.SendSysMessage(out.str().c_str());
                    }
                    {

                        ch.SendSysMessage(out.str().c_str());
                    }
                    {

                        ch.SendSysMessage(out.str().c_str());
                    }
                    {

                        bool tradeActive = (pBot->GetPlayer()->GetTrader()) ? true : false;

                        ch.SendSysMessage(out.str().c_str());
                    }
                    {

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
                    ObjectGuid attackOnGuid = m_master->GetSelectionGuid();
                    if (attackOnGuid.IsEmpty())
                        return;

                    Unit* thingToAttack = ObjectAccessor::GetUnit(*m_master, attackOnGuid);
                    if (!thingToAttack) return;

                    Player *bot = 0;
                    if (!m_master->GetGroup())
                        return;

                    for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
                    {
                        bot = itr->getSource();
                        if (!bot->IsFriendlyTo(thingToAttack) && bot->IsWithinLOSInMap(thingToAttack))
                            bot->GetPlayerbotAI()->SetCombatTarget(thingToAttack);
                    }
                    return;
                }

                // emote to stay
                case TEXTEMOTE_STAND:
                {
                    Player* const bot = GetPlayerBot(m_master->GetSelectionGuid().GetRawValue());
                    if (bot)
                        bot->GetPlayerbotAI()->SetFollowTarget(m_master);
                    else
                    {
                        if (!m_master->GetGroup())
                            return;

                        for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
                        {
                            Player* const bot = itr->getSource();
                            bot->GetPlayerbotAI()->SetFollowTarget(m_master);
                        }
                    }
                    return;
                }

                // 324 is the followme emote (not defined in enum)
                // if master has bot selected then only bot follows, else all bots follow
                case 324:
                case TEXTEMOTE_WAVE:
                {
                    Player* const bot = GetPlayerBot(m_master->GetSelectionGuid().GetRawValue());
                    if (bot)
                        bot->GetPlayerbotAI()->SetFollowTarget(m_master);
                    else
                    {
                        if (!m_master->GetGroup())
                            return;

                        for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
                        {
                            Player* const bot = itr->getSource();
                            bot->GetPlayerbotAI()->SetFollowTarget(m_master);
                        }
                    }
                    return;
                }
            }

            return;
        } /* EMOTE ends here */

        case CMSG_GAMEOBJ_USE: // not sure if we still need this one
        case CMSG_GAMEOBJ_REPORT_USE:
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
                        continue;

                    if (!bot->CanTakeQuest(qInfo, false))
                        continue;

                    if (!bot->SatisfyQuestLog(false))
                        continue;

                    if (!bot->CanAddQuest(qInfo, false))
                        continue;

                    p.rpos(0);         // reset reader
                    bot->GetSession()->HandleQuestgiverAcceptQuestOpcode(p);
                }
            }
            return;
        }

        case CMSG_AREATRIGGER:
        {
            WorldPacket p(packet);

            for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* const bot = itr->getSource();

                p.rpos(0);         // reset reader
                bot->GetSession()->HandleAreaTriggerOpcode(p);
            }
            return;
        }

        case CMSG_QUESTGIVER_COMPLETE_QUEST:
        {
            WorldPacket p(packet);
            p.rpos(0);    // reset reader
            uint32 quest;
            ObjectGuid npcGUID;
            p >> npcGUID >> quest;

            WorldObject* pNpc = m_master->GetMap()->GetWorldObject(npcGUID);
            if (!pNpc)
                return;

            // for all master's bots
            for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* const bot = itr->getSource();
                bot->GetPlayerbotAI()->TurnInQuests(pNpc);
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

            ObjectGuid npcGUID;
            uint64 itemGUID;
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
                    return;

                // remove fake death
                if (bot->hasUnitState(UNIT_STAT_DIED))
                    bot->RemoveSpellsCausingAura(SPELL_AURA_FEIGN_DEATH);

                // reputation discount
                float discountMod = bot->GetReputationPriceDiscount(unit);

                uint32 TotalCost = 0;
                if (!itemGUID)     // Handle redundant feature (repair individual item) for bot
                    TotalCost = bot->DurabilityRepairAll(true, discountMod, guildBank > 0 ? true : false);

                if (guildBank)     // Handle guild repair
                {
                    uint32 GuildId = bot->GetGuildId();
                    if (!GuildId)
                        return;
                    Guild* pGuild = sGuildMgr.GetGuildById(GuildId);
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
            if (!m_master->GetGroup())
                return;

            for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* const bot = itr->getSource();
                Group *grp = bot->GetGroup();
                if (grp)
                    grp->RemoveMember(bot->GetObjectGuid(), 1);
            }
            return;
        }
    }
}

void PlayerbotMgr::HandleMasterOutgoingPacket(const WorldPacket& packet)
{
}

void PlayerbotMgr::Stay()
{
}

void PlayerbotMgr::LogoutPlayerBot(ObjectGuid guid)
{
    Player* bot = GetPlayerBot(guid);
    if (bot)
    {
        for (uint8 i = 0; i < MAX_ARENA_SLOT; ++i)
        {
            uint32 a_id = bot->GetArenaTeamId(i);
            if (a_id==0)
                continue;

            ArenaTeam *at = sObjectMgr.GetArenaTeamById(a_id);
            if (!at)
                continue;

            if (at->DisbandNoSave(bot))
                delete at;
        }

        if (bot->GetGroup())
            bot->RemoveFromGroup();

        WorldSession * botWorldSessionPtr = bot->GetSession();
        botWorldSessionPtr->LogoutPlayer(true); // this will delete the bot Player object and PlayerbotAI object
        delete botWorldSessionPtr;  // finally delete the bot's WorldSession
    }
}

Player* PlayerbotMgr::GetPlayerBot(ObjectGuid playerGuid) const
{
    HashMapHolder < Player > ::MapType& m = sObjectAccessor.GetPlayers();
    for (HashMapHolder < Player > ::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        Player* const bot = itr->second;
        if (bot && bot->GetObjectGuid() == playerGuid)
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

    for (uint8 i = 0; i < MAX_ARENA_SLOT; ++i)
    {
        uint32 a_id = bot->GetArenaTeamId(i);
        if (a_id==0)
            continue;

        ArenaTeam *at = sObjectMgr.GetArenaTeamById(a_id);
        if (!at)
           continue;

        at->DelMember(bot->GetObjectGuid());
    }

    if (bot->GetGroup())
        bot->RemoveFromGroup();

    bot->GiveLevel(bot->GetLevelAtLoading());
    bot->GMStartup();
    bot->SetHealth(bot->GetMaxHealth());
    bot->SetPower(bot->getPowerType(), bot->GetMaxPower(bot->getPowerType()));
}
