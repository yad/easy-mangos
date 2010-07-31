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
#include "Player.h"
#include "PlayerbotAI.h"
#include "PlayerbotMgr.h"
#include "WorldPacket.h"
#include "ObjectMgr.h"
#include "GossipDef.h"
#include "Chat.h"
#include "Language.h"
#include "Guild.h"

class LoginQueryHolder;
class CharacterHandler;

PlayerbotMgr::PlayerbotMgr()
{
    m_confDebugWhisper = sConfig.GetBoolDefault("PlayerbotAI.DebugWhisper", false);
}

PlayerbotMgr::~PlayerbotMgr()
{
}

void PlayerbotMgr::UpdateAI(const uint32 p_time)
{
}

void PlayerbotMgr::HandleMasterIncomingPacket(const WorldPacket& packet)
{
    switch (packet.GetOpcode())
    {
        case CMSG_LOGOUT_REQUEST:
        {
            RealPlayerLogout(m_master);
            return;
        }
        case CMSG_INSPECT:
        {
            WorldPacket p(packet);
            p.rpos(0);
            uint64 guid;
            p >> guid;
            Player* const bot = GetPlayerBot(guid);
            if (bot)bot->GetPlayerbotAI()->SendNotEquipList(*bot);
                return;
        }
        case CMSG_EMOTE:
        case CMSG_TEXT_EMOTE:
        {
            WorldPacket p(packet);
            p.rpos(0);
            uint32 emoteNum;
            p >> emoteNum;

            switch (emoteNum)
            {
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
                case TEXTEMOTE_POINT:
                {
                    uint64 attackOnGuid = m_master->GetSelection();
                    if (!attackOnGuid)return;

                    Unit* thingToAttack = ObjectAccessor::GetUnit(*m_master, attackOnGuid);
                    if (!thingToAttack)return;

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
        }
        case CMSG_GAMEOBJ_USE:
        {
            WorldPacket p(packet);
            p.rpos(0);
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

            }

            return;
        }
        case CMSG_GOSSIP_HELLO:
        case CMSG_QUESTGIVER_HELLO:
        {
            WorldPacket p(packet);
            p.rpos(0);
            uint64 npcGUID;
            p >> npcGUID;

            WorldObject* pNpc = m_master->GetMap()->GetWorldObject(npcGUID);
            if (!pNpc)
                return;

            if (!m_master->GetGroup())
                return;

            for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* const bot = itr->getSource();
                bot->GetPlayerbotAI()->TurnInQuests(pNpc);
            }

            return;
        }
        case CMSG_QUESTGIVER_ACCEPT_QUEST:
        {
            WorldPacket p(packet);
            p.rpos(0);
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
                        p.rpos(0);
                        bot->GetSession()->HandleQuestgiverAcceptQuestOpcode(p);
                        bot->GetPlayerbotAI()->TellMaster("J'ai pris la quete.");
                    }
                }
            }
            return;
        }
        case CMSG_LOOT_ROLL:
        {
            WorldPacket p(packet);
            uint64 Guid;
            uint32 NumberOfPlayers;
            uint8 rollType;
            p.rpos(0);
            p >> Guid;
            p >> NumberOfPlayers;
            p >> rollType;

            if (!m_master->GetGroup())
                return;

            for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                uint32 choice = urand(0,3);

                Player* const bot = itr->getSource();
                if(!bot)
                     return;

                Group* group = bot->GetGroup();
                if(!group)
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
            WorldPacket p(packet);
            uint64 npcGUID, itemGUID;
            uint8 guildBank;
            p.rpos(0);
            p >> npcGUID;
            p >> itemGUID;
            p >> guildBank;

            if (!m_master->GetGroup())
                return;

            for (GroupReference *itr = m_master->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* const bot = itr->getSource();
                if (!bot)
                     return;

                Group* group = bot->GetGroup();
                if (!group)
                     return;

                Creature *unit = bot->GetNPCIfCanInteractWith(npcGUID, UNIT_NPC_FLAG_REPAIR);
                if (!unit)
                    return;

                if (bot->hasUnitState(UNIT_STAT_DIED))
                     bot->RemoveSpellsCausingAura(SPELL_AURA_FEIGN_DEATH);

                float discountMod = bot->GetReputationPriceDiscount(unit);

                uint32 TotalCost = 0;
                if (itemGUID)
                    continue;
                else
                    TotalCost = bot->DurabilityRepairAll(true, discountMod, guildBank > 0 ?true:false);

                if (guildBank)
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
    }
}

void PlayerbotMgr::Stay(Player * const player)
{
    if (!player->GetGroup())
        return;

    for (GroupReference *itr = player->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* const bot = itr->getSource();
        bot->GetMotionMaster()->Clear();
    }
}

void PlayerbotMgr::LogoutPlayerBot(uint64 guid)
{
    Player* bot = GetPlayerBot(guid);
    if (bot)
    {
        WorldSession * botWorldSessionPtr = bot->GetSession();
        botWorldSessionPtr->LogoutPlayer(true);
        delete botWorldSessionPtr;
    }
}

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

void PlayerbotMgr::OnBotLogin(Player* bot)
{
    PlayerbotAI* ai = bot->GetPlayerbotAI();
    if (!ai)
    {
        PlayerbotAI* ai = new PlayerbotAI(this, bot);
        if (ai)
            bot->SetPlayerbotAI(ai);
    }
    ai = bot->GetPlayerbotAI();
    ai->SetIgnoreSpell(5);

    if (bot->GetGroup())
        bot->RemoveFromGroup();

    ChatHandler ch(bot);
    bot->PurgeMyBags();
    bot->GiveLevel(bot->GetLevelAtLoading());
    ch.HandleGMStartUpCommand("");
    bot->SetHealth(bot->GetMaxHealth());
    bot->SetPower(bot->getPowerType(), bot->GetMaxPower(bot->getPowerType()));
}

void PlayerbotMgr::RealPlayerLogout(Player * const player)
{
    Player* bot = NULL;
    HashMapHolder < Player > ::MapType& m = sObjectAccessor.GetPlayers();
    for (HashMapHolder < Player > ::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
    {
        bot = itr->second;
        if (!bot || !bot->IsBot())
            continue;

        if (bot->GetPlayerbotMgr()->GetMaster() && (bot->GetPlayerbotMgr()->GetMaster() == player))
        {
            if (bot->GetGroup())
               bot->RemoveFromGroup();
            bot->GetPlayerbotMgr()->LogoutPlayerBot(bot->GetGUID());
            AddAllBots(sConfig.GetIntDefault( "PlayerbotAI.MaxBots", 100 ));
        }
    }
}
