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
#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "../ItemPrototype.h"
#include "../World.h"
#include "../SpellMgr.h"
#include "PlayerbotAI.h"
#include "PlayerbotMgr.h"
#include "PlayerbotDeathKnightAI.h"
#include "PlayerbotDruidAI.h"
#include "PlayerbotHunterAI.h"
#include "PlayerbotMageAI.h"
#include "PlayerbotPaladinAI.h"
#include "PlayerbotPriestAI.h"
#include "PlayerbotRogueAI.h"
#include "PlayerbotShamanAI.h"
#include "PlayerbotWarlockAI.h"
#include "PlayerbotWarriorAI.h"
#include "../Player.h"
#include "../ObjectMgr.h"
#include "../Chat.h"
#include "WorldPacket.h"
#include "../Spell.h"
#include "../Unit.h"
#include "../SpellAuras.h"
#include "../SharedDefines.h"
#include "Log.h"
#include "../GossipDef.h"
#include "../ArenaTeam.h"
#include "../MotionMaster.h"
#include "../BattleGroundMgr.h"

float rand_float(float low, float high)
{
    return (rand() / (static_cast<float> (RAND_MAX) + 1.0)) * (high - low) + low;
}

class PlayerbotChatHandler : protected ChatHandler
{
    public:
        explicit PlayerbotChatHandler(Player* pMasterPlayer) : ChatHandler(pMasterPlayer) {}
        void sysmessage(const char *str) { SendSysMessage(str); }
        bool dropQuest(char *str) { return HandleQuestRemoveCommand(str); }
};

PlayerbotAI::PlayerbotAI(PlayerbotMgr* const mgr, Player* const bot) : m_mgr(mgr), m_bot(bot)
{
    m_ignoreAIUpdatesUntilTime = 0;

    m_TimeDoneEating = 0;
    m_TimeDoneDrinking = 0;

    m_CurrentlyCastingSpellId = 0;

    m_targetGuidCommand = 0;

    m_taxiMaster = ObjectGuid();

    m_classAI = 0;
    m_role = 0;
    m_new_role = 1;

    m_enterBg = 0;
    m_leaveBg = 0;

    m_botState = BOTSTATE_NORMAL;
    m_targetCombat = NULL;
    m_followTarget = m_bot;

    SetQuestNeedItems();

    m_bot->GetPosition(orig_x, orig_y, orig_z);
    orig_map = m_bot->GetMapId();

    switch (m_bot->getClass())
    {
        case CLASS_WARRIOR:
            m_classAI = (PlayerbotClassAI*) new PlayerbotWarriorAI(m_bot, this);
            break;
        case CLASS_PALADIN:
            m_classAI = (PlayerbotClassAI*) new PlayerbotPaladinAI(m_bot, this);
            break;
        case CLASS_HUNTER:
            m_classAI = (PlayerbotClassAI*) new PlayerbotHunterAI(m_bot, this);
            break;
        case CLASS_ROGUE:
            m_classAI = (PlayerbotClassAI*) new PlayerbotRogueAI(m_bot, this);
            break;
        case CLASS_PRIEST:
            m_classAI = (PlayerbotClassAI*) new PlayerbotPriestAI(m_bot, this);
            break;
        case CLASS_DEATH_KNIGHT:
            m_classAI = (PlayerbotClassAI*) new PlayerbotDeathKnightAI(m_bot, this);
            break;
        case CLASS_SHAMAN:
            m_classAI = (PlayerbotClassAI*) new PlayerbotShamanAI(m_bot, this);
            break;
        case CLASS_MAGE:
            m_classAI = (PlayerbotClassAI*) new PlayerbotMageAI(m_bot, this);
            break;
        case CLASS_WARLOCK:
            m_classAI = (PlayerbotClassAI*) new PlayerbotWarlockAI(m_bot, this);
            break;
        case CLASS_DRUID:
            m_classAI = (PlayerbotClassAI*) new PlayerbotDruidAI(m_bot, this);
            break;
    }
}

PlayerbotAI::~PlayerbotAI()
{
    delete m_classAI;
}

void PlayerbotAI::ReinitAI()
{
    m_botState = BOTSTATE_NORMAL;
    m_targetCombat = NULL;
    m_followTarget = m_bot;

    if (m_bot->m_movementInfo.HasMovementFlag(MOVEFLAG_FLYING))
        m_bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);

    if (m_bot->isDead())
        m_bot->ResurrectPlayer(100.0f);

    if (m_bot == GetLeader())
    {
        m_bot->GiveLevel(m_bot->GetLevelAtLoading());
        CheckStuff();
        m_bot->TeleportTo(orig_map, orig_x, orig_y, orig_z, 0.0f);
        for (uint8 i = 0; i < MAX_ARENA_SLOT; ++i)
        {
            uint32 a_id = m_bot->GetArenaTeamId(i);
            if (a_id==0)
                continue;

            ArenaTeam *at = sObjectMgr.GetArenaTeamById(a_id);
            if (!at)
                continue;

            if (at->DisbandNoSave(m_bot))
                delete at;
        }
        if (m_bot->GetGroup())
            m_bot->RemoveFromGroup();
    }
}

Player* PlayerbotAI::GetLeader() const
{
    return m_mgr->GetLeader();
}

void PlayerbotAI::SetLeader(Player* pl)
{
    m_mgr->SetLeader(pl);
}

void PlayerbotAI::HandleBotOutgoingPacket(const WorldPacket& packet)
{
    switch (packet.GetOpcode())
    {
        case SMSG_DUEL_WINNER:
        {
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_APPLAUD);
            return;
        }
        case SMSG_DUEL_COMPLETE:
        {
            m_bot->GetMotionMaster()->Clear(true);
            return;
        }
        case SMSG_DUEL_OUTOFBOUNDS:
        {
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_CHICKEN);
            return;
        }
        case SMSG_DUEL_REQUESTED:
        {
            WorldPacket p(packet);
            ObjectGuid flagGuid;
            p >> flagGuid;
            ObjectGuid playerGuid;
            p >> playerGuid;

            WorldPacket* const packet = new WorldPacket(CMSG_DUEL_ACCEPTED, 8);
            *packet << flagGuid;
            m_bot->GetSession()->QueuePacket(packet);

            m_ignoreAIUpdatesUntilTime = time(0) + 3;
            return;
        }

        case SMSG_SPELL_FAILURE:
        {
            WorldPacket p(packet);
            uint8 castCount;
            uint32 spellId;

            ObjectGuid casterGuid = p.readPackGUID();
            if (casterGuid != m_bot->GetObjectGuid())
                return;

            p >> castCount >> spellId;
            if (m_CurrentlyCastingSpellId == spellId)
            {
                m_ignoreAIUpdatesUntilTime = time(0);
                m_CurrentlyCastingSpellId = 0;
            }
            return;
        }

        case SMSG_FORCE_RUN_SPEED_CHANGE:
        {
            CheckMount();
            return;
        }

        case SMSG_MOVE_SET_CAN_FLY:
        {
            WorldPacket p(packet);
            ObjectGuid guid = p.readPackGUID();
            if (guid != m_bot->GetObjectGuid())
                return;
            m_bot->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
            SetFollowTarget(GetLeader(), true);
            return;
        }

        case SMSG_MOVE_UNSET_CAN_FLY:
        {
            WorldPacket p(packet);
            ObjectGuid guid = p.readPackGUID();
            if (guid != m_bot->GetObjectGuid())
                return;
            m_bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
            SetFollowTarget(GetLeader(), true);
            return;
        }

        case SMSG_GROUP_SET_LEADER:
        {
            WorldPacket p(packet);
            std::string name;
            p >> name;
            if (m_bot->GetGroup() && name == m_bot->GetName())
            {
                Player* newLeader = FindNewGroupLeader();
                if (newLeader)
                {
                    SetLeader(newLeader);
                    m_bot->GetGroup()->ChangeLeader(newLeader->GetObjectGuid());
                }
                else if (m_bot->GetGroup()->IsMember(GetLeader()->GetObjectGuid()))
                {
                    m_bot->GetGroup()->ChangeLeader(newLeader->GetObjectGuid());
                }
            }
            return;
        }

        case SMSG_GROUP_INVITE:
        {
            if (m_bot->GetGroupInvite())
            {
                const Group* const grp = m_bot->GetGroupInvite();
                if (!grp)
                    return;

                Player* const inviter = sObjectMgr.GetPlayer(grp->GetLeaderGuid());
                if (!inviter)
                {
                    m_bot->SetGroupInvite(NULL);
                    return;
                }
                WorldPacket p;

                if (inviter->GetBattleGround())
                {
                    ChatHandler ch(inviter);
                    ch.PSendSysMessage("Impossible d'inviter un bot depuis un champ de bataille");
                    m_bot->GetSession()->HandleGroupDeclineOpcode(p);
                    m_bot->SetGroupInvite(NULL);
                    return;
                }

                if (inviter->getLevel() < 55 && m_bot->getClass() == CLASS_DEATH_KNIGHT)
                {
                    ChatHandler ch(inviter);
                    ch.PSendSysMessage("Impossible d'inviter un Chevalier de la Mort avant le niveau 55");
                    m_bot->GetSession()->HandleGroupDeclineOpcode(p);
                    m_bot->SetGroupInvite(NULL);
                    return;
                }

                m_bot->GetSession()->HandleGroupAcceptOpcode(p);
                m_bot->GetMotionMaster()->Clear(true);
                if (!inviter->IsBot())
                {
                    m_bot->SetDungeonDifficulty(inviter->GetDungeonDifficulty());
                    SetFollowTarget(inviter);
                }
                m_ignoreAIUpdatesUntilTime = time(0);
            }
            return;
        }

        case SMSG_PETITION_SHOW_SIGNATURES:
        {
            WorldPacket p(packet);
            ObjectGuid petitionguid;
            p >> petitionguid;

            WorldPacket packet = WorldPacket(CMSG_PETITION_SIGN, 8+1);
            packet << petitionguid;
            packet << uint8(1);
            m_bot->GetSession()->HandlePetitionSignOpcode(packet);
            return;
        }

        case SMSG_ARENA_TEAM_INVITE:
        {
            WorldPacket pk;
            m_bot->GetSession()->HandleArenaTeamAcceptOpcode(pk);
            return;
        }

        case SMSG_BATTLEFIELD_STATUS:
        {
            WorldPacket p(packet);
            if (p.size() == 4+8)
                return;

            uint32 QueueSlot;
            uint8 type;
            uint8 unk;
            uint32 bgTypeId_;
            uint16 unk0;
            uint8 unk1;
            uint8 unk2;
            uint32 ClientInstanceID;
            uint8 Rated;
            uint32 StatusID;

            p >> QueueSlot >> type >> unk >> bgTypeId_ >> unk0 >> unk1 >> unk2 >> ClientInstanceID >> Rated >> StatusID;

            if (StatusID == STATUS_WAIT_JOIN)
            {
                BattleGroundTypeId bgTypeId = BattleGroundTypeId(bgTypeId_);
                BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(bgTypeId, ArenaType(type));
                WorldPacket packet = WorldPacket(CMSG_BATTLEFIELD_PORT,1+1+4+2+1);
                packet << uint8(type);
                packet << uint8(unk);
                packet << uint32(bgTypeId_);
                packet << uint16(unk0);
                packet << uint8(m_bot->IsInvitedForBattleGroundQueueType(bgQueueTypeId) ? 1 : 0);
                m_bot->GetSession()->HandleBattleFieldPortOpcode(packet);
                m_bot->GetPlayerbotAI()->SetEnterBGTime(60);
            }
            return;
        }
        case MSG_PVP_LOG_DATA:
        {
            BattleGround *bg = m_bot->GetBattleGround();
            if (bg && bg->GetStatus() == STATUS_WAIT_LEAVE)
            {
                m_leaveBg = time(0) + 10;
                m_ignoreAIUpdatesUntilTime = time(0) + 10;
            }
            return;
        }

        /*case SMSG_TRADE_STATUS:
        {
            if (m_bot->GetTrader() == NULL)
                break;

            WorldPacket p(packet);
            uint32 status;
            p >> status;
            p.resize(4);

            if (status == TRADE_STATUS_TRADE_ACCEPT)
                m_bot->GetSession()->HandleAcceptTradeOpcode(p);

            else if (status == TRADE_STATUS_BEGIN_TRADE)
            {
                m_bot->GetSession()->HandleBeginTradeOpcode(p);
                for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
                {
                    const Item* const pItem = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                    if (pItem && pItem->CanBeTraded())
                    {
                        const ItemPrototype* const pItemProto = pItem->GetProto();

                        std::string itemName = pItemProto->Name1;
                        ItemLocalization(itemName, pItemProto->ItemId);


                            << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
                        if (pItem->GetCount() > 1)

                    }
                }

                for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
                {
                    const Bag* const pBag = (Bag*) m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
                    if (pBag)
                        for (uint8 slot = 0; slot < pBag->GetBagSize(); ++slot)
                        {
                            const Item* const pItem = m_bot->GetItemByPos(bag, slot);
                            if (pItem && pItem->CanBeTraded())
                            {
                                const ItemPrototype* const pItemProto = pItem->GetProto();

                                std::string itemName = pItemProto->Name1;
                                ItemLocalization(itemName, pItemProto->ItemId);
                                    << ":0:0:0:0:0:0:0" << "|h[" << itemName
                                    << "]|h|r";
                                if (pItem->GetCount() > 1)

                            }
                        }
                }

                uint32 copper = m_bot->GetMoney();
                uint32 gold = uint32(copper / 10000);
                copper -= (gold * 10000);
                uint32 silver = uint32(copper / 100);
                copper -= (silver * 100);

                whisper << "Je possède actuellement |cff00ff00" << gold
                        << "|r|cfffffc00g|r|cff00ff00" << silver
                        << "|r|cffcdcdcds|r|cff00ff00" << copper
                        << "|r|cffffd333c|r" << " et je peux échanger les objets suivants :";

                ChatHandler ch(m_bot->GetTrader());
                ch.SendSysMessage(out.str().c_str());
            }
            return;
        }*/

        case SMSG_SPELL_START:
        {
            if (GetLeader()==m_bot)
                return;

            WorldPacket p(packet);
            ObjectGuid castItemGuid = p.readPackGUID();
            ObjectGuid casterGuid = p.readPackGUID();
            if(casterGuid == GetLeader()->GetObjectGuid())
            {
                Unit* pTarget = ObjectAccessor::GetUnit(*m_bot, GetLeader()->GetSelectionGuid());
                if (pTarget && pTarget->IsHostileTo(m_bot))
                {
                    ChangeCombatTarget(pTarget);
                    Spell* const pSpell = GetCurrentSpell();
                    if ((!GetLeader()->IsMounted()) && (m_bot->IsMounted()))
                        UnMount();

                    if (!pSpell || !pSpell->IsChannelActive())
                        DoCombatManeuver();
                    else
                        SetIgnoreUpdateTime(1);
                }
            }
            return;
        }

        case SMSG_SPELL_GO:
        {
            WorldPacket p(packet);
            ObjectGuid castItemGuid = p.readPackGUID();
            ObjectGuid casterGuid = p.readPackGUID();
            if (casterGuid == m_bot->GetObjectGuid())
            {
                uint32 spellId;
                p >> spellId;
                uint16 castFlags;
                p >> castFlags;
                uint32 msTime;
                p >> msTime;
                uint8 numHit;
                p >> numHit;

                if (m_CurrentlyCastingSpellId == spellId)
                {
                    Spell* const pSpell = m_bot->FindCurrentSpellBySpellId(spellId);
                    if (!pSpell)
                        return;

                    if (pSpell->IsChannelActive() || pSpell->IsAutoRepeat())
                        m_ignoreAIUpdatesUntilTime = time(0) + (GetSpellDuration(pSpell->m_spellInfo) / 1000) + 1;
                    else if (pSpell->IsAutoRepeat())
                        m_ignoreAIUpdatesUntilTime = time(0) + 6;
                    else
                    {
                        m_ignoreAIUpdatesUntilTime = time(0) + 1;
                        m_CurrentlyCastingSpellId = 0;
                    }
                }
            }
            return;
        }

        case SMSG_RESURRECT_REQUEST:
        {
            if (!m_bot->isAlive())
            {
                WorldPacket p(packet);
                ObjectGuid guid;
                p >> guid;

                WorldPacket* const packet = new WorldPacket(CMSG_RESURRECT_RESPONSE, 8+1);
                *packet << guid;
                *packet << uint8(1);
                m_bot->GetSession()->QueuePacket(packet);

                SetState(BOTSTATE_NORMAL);
                SetIgnoreUpdateTime(0);
            }
            return;
        }
        /*case SMSG_MONSTER_MOVE:
        case SMSG_UPDATE_WORLD_STATE:
        case SMSG_COMPRESSED_UPDATE_OBJECT:
        case MSG_MOVE_SET_FACING:
        case MSG_MOVE_STOP:
        case MSG_MOVE_HEARTBEAT:
        case MSG_MOVE_STOP_STRAFE:
        case MSG_MOVE_START_STRAFE_LEFT:
        case SMSG_UPDATE_OBJECT:
        case MSG_MOVE_START_FORWARD:
        case MSG_MOVE_START_STRAFE_RIGHT:
        case SMSG_DESTROY_OBJECT:
        case MSG_MOVE_START_BACKWARD:
        case SMSG_AURA_UPDATE_ALL:
        case MSG_MOVE_FALL_LAND:
        case MSG_MOVE_JUMP:
        case CMSG_NAME_QUERY:
        case CMSG_STANDSTATECHANGE:
        case CMSG_QUERY_TIME:
        case CMSG_CREATURE_QUERY:
        case CMSG_GAMEOBJECT_QUERY:
        case SMSG_TIME_SYNC_REQ:
        case CMSG_TIME_SYNC_RESP:
        case CMSG_WORLD_STATE_UI_TIMER_UPDATE:
        case SMSG_WORLD_STATE_UI_TIMER_UPDATE:
        case SMSG_EMOTE:
        case SMSG_POWER_UPDATE:
        case SMSG_AURA_UPDATE:
        case SMSG_PERIODICAURALOG:
            break;
        default:
        {
            const char* oc = LookupOpcodeName(packet.GetOpcode());
            sLog.outString(oc);
        }*/
    }
}

Unit* PlayerbotAI::GetNewCombatTarget()
{
    //TODO ajouter gestion de l'aggro
    Unit* newCombatTarget = NULL;

    if (m_botState != BOTSTATE_COMBAT)
    {
        SetState(BOTSTATE_COMBAT);
        SetQuestNeedItems();
        m_lootCreature.clear();
        m_lootCurrent = 0;
        m_targetCombat = NULL;
        if (m_bot->HasAura(25990))
            m_bot->RemoveAurasDueToSpell(25990);
    }

    //TODO BG WP
    if (m_bot->GetBattleGround())
    {
        return FindEnemy();
    }
    else
    {
        if (!m_bot->getAttackers().empty())
        {
            for(Unit::AttackerSet::const_iterator itr = m_bot->getAttackers().begin(); itr != m_bot->getAttackers().end(); ++itr)
            {
                newCombatTarget = (*itr)->GetOwner();
                if (!newCombatTarget || !newCombatTarget->isAlive())
                    newCombatTarget = (*itr);
                if (newCombatTarget && newCombatTarget->isAlive())
                    return newCombatTarget;
            }
        }
        if (m_bot->GetGroup())
        {
            GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
            while (ref)
            {
                if (!ref->getSource()->getAttackers().empty())
                {
                    for(Unit::AttackerSet::const_iterator itr = ref->getSource()->getAttackers().begin(); itr != ref->getSource()->getAttackers().end(); ++itr)
                    {
                        newCombatTarget = (*itr)->GetOwner();
                        if (!newCombatTarget || !newCombatTarget->isAlive())
                            newCombatTarget = (*itr);
                        if (newCombatTarget && newCombatTarget->isAlive())
                            return newCombatTarget;
                    }
                }

                if (ref->getSource()->getVictim())
                {
                    newCombatTarget = ref->getSource()->getVictim()->GetOwner();
                    if (!newCombatTarget || !newCombatTarget->isAlive())
                            newCombatTarget = ref->getSource()->getVictim();
                    if (newCombatTarget && newCombatTarget->isAlive())
                        return newCombatTarget;
                }

                ref = ref->next();
            }
        }
        if (m_bot->GetPet() && !m_bot->GetPet()->getAttackers().empty())
        {
            for(Unit::AttackerSet::const_iterator itr = m_bot->GetPet()->getAttackers().begin(); itr != m_bot->GetPet()->getAttackers().end(); ++itr)
            {
                newCombatTarget = (*itr)->GetOwner();
                if (!newCombatTarget || !newCombatTarget->isAlive())
                    newCombatTarget = (*itr);
                if (newCombatTarget && newCombatTarget->isAlive())
                    return newCombatTarget;
            }
        }
        if (m_bot->GetGroup())
        {
            GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
            while (ref)
            {
                if (!ref->getSource()->getAttackers().empty())
                {
                    for(Unit::AttackerSet::const_iterator itr = ref->getSource()->getAttackers().begin(); itr != ref->getSource()->getAttackers().end(); ++itr)
                    {
                        newCombatTarget = (*itr)->GetOwner();
                        if (!newCombatTarget || !newCombatTarget->isAlive())
                            newCombatTarget = (*itr);
                        if (newCombatTarget && newCombatTarget->isAlive())
                            return newCombatTarget;
                    }
                }

                if (ref->getSource()->getVictim())
                {
                    newCombatTarget = ref->getSource()->getVictim()->GetOwner();
                    if (!newCombatTarget || !newCombatTarget->isAlive())
                            newCombatTarget = ref->getSource()->getVictim();
                    if (newCombatTarget && newCombatTarget->isAlive())
                        return newCombatTarget;
                }
                ref = ref->next();
            }
        }
    }
    //TODO ajouter de quoi rendre les bots agressifs
    return NULL;
}

void PlayerbotAI::DoCombatManeuver()
{
    if (m_bot->GetBattleGround())
    {
        if (!m_targetCombat || m_targetCombat->isDead() || !m_targetCombat->IsInWorld()
            || !m_bot->IsInMap(m_targetCombat) || !((Player*)m_targetCombat)->GetBattleGround())
            ChangeCombatTarget(GetNewCombatTarget());
    }
    else
    {
        if (!m_targetCombat || m_targetCombat->isDead() || !m_targetCombat->IsInWorld()
            || !m_bot->IsWithinDistInMap(m_targetCombat, 50.0f) || !m_bot->IsHostileTo(m_targetCombat))
            ChangeCombatTarget(GetNewCombatTarget());
    }

    if (m_targetCombat)
    {
        if (m_bot->GetHealthPercent() < MIN_HP_PERCENT_BEFORE_FLEEING)
        {
            m_bot->GetMotionMaster()->MoveFleeing(m_targetCombat);
            //Add some stuff here to heal himself
            return;
        }
        SetInFront(m_targetCombat);
        if (GetCombatType()==BOTCOMBAT_CAC)
        {
            if (m_bot->Attack(m_targetCombat,true))
            {
                m_bot->GetMotionMaster()->MoveChase(m_targetCombat);
            }
            if (m_bot->IsWithinDistInMap(m_targetCombat, ATTACK_DISTANCE))
            {
                if (m_bot->isAttackReady())
                {
                    m_bot->AttackerStateUpdate(m_targetCombat);
                    m_bot->resetAttackTimer();
                }
                GetClassAI()->DoCombatManeuver(m_targetCombat, true);
            }
            else
            {
                GetClassAI()->DoFastBuffOnOneself();
            }
        }
        else
        {
            if (m_bot->Attack(m_targetCombat,false))
            {
                m_bot->GetMotionMaster()->MoveChase(m_targetCombat, 30.0f);
            }
            GetClassAI()->DoCombatManeuver(m_targetCombat, m_bot->IsWithinDistInMap(m_targetCombat, ATTACK_DISTANCE));
        }        
    }
}

void PlayerbotAI::ChangeCombatTarget(Unit* targetCombat)
{
    m_targetCombat = targetCombat;
    if (m_targetCombat)
    {
        GetClassAI()->ReinitCycles();

        m_bot->SetSelectionGuid((m_targetCombat->GetObjectGuid()));
        SetFollowTarget(m_targetCombat);

        if (m_targetCombat->GetTypeId()==TYPEID_UNIT)
            m_lootCreature.push_back((Creature*)m_targetCombat);
    }
    else
    {
        m_bot->CombatStop();
        m_bot->SetSelectionGuid(ObjectGuid());
        SetFollowTarget(GetLeader());
        SetInFront(GetLeader());
    }
}

bool PlayerbotAI::IsInCombat()
{
    if (m_bot->duel)
        return true;

    if (m_bot->GetBattleGround())
        return true;

    if (m_targetCombat)
        return true;

    if (!m_bot->getAttackers().empty()
        || (m_bot->GetPet() && !m_bot->GetPet()->getAttackers().empty()))
        return true;

    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            if (!ref->getSource()->getAttackers().empty())
                return true;

            if (ref->getSource()->getVictim() && ref->getSource()->getVictim()->isAlive())
                return true;

            if (ref->getSource()->GetPet() && !ref->getSource()->GetPet()->getAttackers().empty())
                return true;

            if (ref->getSource()->GetPet() && ref->getSource()->GetPet()->getVictim() && ref->getSource()->GetPet()->getVictim()->isAlive())
                return true;

            ref = ref->next();
        }
    }
    return false;
}

void PlayerbotAI::SetFollowTarget(Unit * followTarget, bool forced)
{
    if (!followTarget)
    {
        m_bot->GetMotionMaster()->Clear(true);
        return;
    }

    if (followTarget->GetTypeId() == TYPEID_PLAYER)
    {
        Player* target = ((Player*)followTarget);
        if (target->GetCorpse() && !FollowCheckTeleport(target->GetCorpse()))
            return;
        if (!FollowCheckTeleport(followTarget))
            return;
    }

    if (!forced && m_followTarget==followTarget)
        return;

    m_followTarget = followTarget;

    if (m_bot == GetLeader())
    {
        if (!m_bot->GetBattleGround())
        {
            if (GetCombatType()==BOTCOMBAT_CAC)
                MoveTo(rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)+M_PI_F); //CAC
            else
                MoveTo(rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)); //RANGED
        }
        else
        {
            if (m_bot->IsWithinDistInMap(m_followTarget, orig_x, orig_y, orig_z, 3.0f))
            {
                SetInFront(m_followTarget);
            }
            else
            {
                MoveTo(rand_float(0, M_PI_F));
            }
        }
    }
    else
    {
        if (!m_bot->GetBattleGround())
        {
            if (m_bot->IsWithinDistInMap(m_followTarget, 100.0f))
            {
                if (m_followTarget == GetLeader())
                {
                    if (GetCombatType()==BOTCOMBAT_CAC)
                        MoveTo(rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)+M_PI_F); //CAC
                    else
                        MoveTo(rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)); //RANGED
                    SetInFront(m_followTarget);
                }
                else
                {
                    if (GetCombatType()==BOTCOMBAT_CAC)
                    {
                        MoveTo(rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)+M_PI_F); //CAC
                    }
                    else
                    {
                        Spell* sp = GetCurrentSpell();
                        if (!sp)
                        {
                            MoveTo(rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)); //RANGED
                        }
                        else if (sp->IsRangedSpell())
                        {
                            SpellRangeEntry const *TempRange = GetSpellRangeStore()->LookupEntry(sp->m_spellInfo->rangeIndex);
                            if (!TempRange)
                            {
                                MoveTo(rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)); //RANGED
                            }
                            else
                            {
                                if (!m_bot->IsInRange(m_followTarget,TempRange->minRange,TempRange->maxRange))
                                {
                                    MoveTo(rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)+M_PI_F, TempRange->maxRange); //RANGED
                                }
                            }
                        }
                        else
                        {
                            MoveTo(rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)); //RANGED
                        }
                    }
                }
            }
        }
        else
        {
            if (m_bot->IsWithinDistInMap(m_followTarget, 3.0f))
            {
                SetInFront(m_followTarget);
            }
            else if (m_bot->IsInMap(GetLeader()))
            {
                MoveTo(rand_float(0, M_PI_F));
            }
        }
    }
}

void PlayerbotAI::MoveTo(float angle, float minDist, float maxDist, float x, float y, float z)
{
    float max = maxDist;
    if (m_followTarget)
    {
        if (m_followTarget->IsInWorld())
        {
            if (m_followTarget == GetLeader())
            {
                Group *gr = m_bot->GetGroup();
                max = gr ? (float)gr->GetMembersCount() : 1.0f;
                if (max > 30.0f)
                    max = 30.0f;

                m_bot->GetMotionMaster()->Clear(true);
                m_bot->GetMotionMaster()->MoveFollow(m_followTarget, rand_float(minDist, max), angle);
            }
            else
            {
                if (m_bot->IsHostileTo(m_followTarget))
                {
                    m_bot->GetMotionMaster()->Clear(true);
                    m_bot->GetMotionMaster()->MoveChase(m_followTarget);
                }
                else
                {
                    m_bot->GetMotionMaster()->Clear(true);
                    m_bot->GetMotionMaster()->MoveFollow(m_followTarget, rand_float(minDist, max), angle);
                }
            }
            SetInFront(m_followTarget);
        }
    }
    else
    {
        m_bot->GetMotionMaster()->Clear(true);
        m_bot->GetMotionMaster()->MovePoint(0, x, y, z);
    }
}

void PlayerbotAI::MoveInLineOfSight(Unit *u)
{
    if (m_bot->getVictim())
        return;

    if (!m_bot->GetCharmInfo() || !m_bot->GetCharmInfo()->HasReactState(REACT_AGGRESSIVE))
        return;

    if (u->isTargetableForAttack() && m_bot->IsHostileTo( u ))
    {
        if(m_bot->IsWithinDistInMap(u, ATTACK_DISTANCE) && m_bot->GetDistanceZ(u) <= CREATURE_Z_ATTACK_RANGE)
        {
            if(m_bot->IsWithinLOSInMap(u))
            {
                if (m_bot->Attack(m_targetCombat,true))
                {
                    m_bot->GetMotionMaster()->MoveChase(m_targetCombat);
                }
            }
        }
    }
}

bool PlayerbotAI::FindPOI()
{
    Unit* target = m_bot->SelectRandomFriendlyTargetBetween(0, 10.0f, MAX_RANGE_MOVEMENT);

    if (!m_followTarget)
    {
        if (target)
        {
            m_followTarget = target;
            return true;
        }
        return false;
    }

    if (m_bot->IsWithinDistInMap(m_followTarget, 5.0f))
    {
        if (target)
        {
            if (target != m_followTarget)
            {
                m_followTarget = target;
                return true;
            }
            return false;
        }
        else
        {
            m_followTarget = NULL;
            return false;
        }
    }
    return true;
}

Unit* PlayerbotAI::FindEnemy()
{
    if (m_bot->duel)
        return m_bot->duel->opponent;

    if (m_bot->GetBattleGround())
    {
        BattleGroundPlayerMap players = m_bot->GetBattleGround()->GetPlayers();

        for(BattleGroundPlayerMap::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player *plr = ObjectAccessor::FindPlayer(itr->first);
            if (!plr || !plr->IsInWorld() || !plr->isAlive())
                continue;

            if (plr->HasAura(32727) || plr->HasAura(32728) || plr->HasAura(44521))
                return NULL;
        }

        for(BattleGroundPlayerMap::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player *plr = ObjectAccessor::FindPlayer(itr->first);
            if (!plr || !plr->IsInWorld() || !plr->isAlive())
                continue;

            if (plr->GetBGTeam() == m_bot->GetBGTeam())
                continue;

            return plr;
        }
    }
    else
    {
        Unit* target = m_bot->SelectRandomUnfriendlyTargetWithBgCheck(0, MAX_RANGE_MOVEMENT);
        if (target)
        {
            int difflvl = ((int)target->getLevel()) - ((int)m_bot->getLevel());
            if (3 >= difflvl && difflvl >= 0)
                return target;
        }
    }
    return NULL;
}

bool PlayerbotAI::SetInFront(const Unit* obj)
{
    if (m_bot->HasInArc((float)M_PI/16.0f, obj))
        return false;

    m_bot->SetInFront(obj);
    float ori = m_bot->GetAngle(obj);
    float x, y, z;
    x = m_bot->m_movementInfo.GetPos()->x;
    y = m_bot->m_movementInfo.GetPos()->y;
    z = m_bot->m_movementInfo.GetPos()->z;
    m_bot->m_movementInfo.ChangePosition(x,y,z,ori);
    m_bot->SendHeartBeat(false);
    return true;
}

void PlayerbotAI::UpdateAI(const uint32 p_time)
{
    time_t currentTime = time(0);
    if (currentTime < m_ignoreAIUpdatesUntilTime)
        return;
    m_ignoreAIUpdatesUntilTime = currentTime + 1;

    //Usefull comment to test bots !
    /*if (!m_bot->GetGroup())
        return;*/

    //ALWAYS RESPECT THIS ORDER !!!!
    if (!CheckLeader() || !CheckTeleport())
        return;

    if (m_bot==GetLeader())
    {
        if (m_bot->GetZoneId()==876)
           return;
    }
    else
    {
        if (!CheckGroup())
            return;
    }

    if (m_bot->GetTrader())
        return;

    CheckBG();

    if (!m_bot->isAlive())
    {
        if (m_botState != BOTSTATE_DEAD && m_botState != BOTSTATE_DEADRELEASED)
        {
            m_lootCreature.clear();
            m_lootCurrent = 0;
            m_bot->SetSelectionGuid(ObjectGuid());
            m_bot->GetMotionMaster()->Clear(true);
            SetState(BOTSTATE_DEAD);
        }
        else if (m_botState == BOTSTATE_DEAD)
        {
            if (m_bot != GetLeader() && !GetLeader()->GetMap()->IsBattleGroundOrArena())
            {
                if (m_bot->GetCorpse())
                {
                    SetState(BOTSTATE_DEADRELEASED);
                    return;
                }
                m_bot->SetBotDeathTimer();
                m_bot->BuildPlayerRepop();
                SetState(BOTSTATE_DEADRELEASED);

                WorldLocation loc;
                Corpse *corpse = m_bot->GetCorpse();
                corpse->GetPosition(loc);
                m_bot->TeleportTo(loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z, m_bot->GetOrientation());
                return;
            }
            else
            {
                if (m_bot->GetMap()->IsBattleArena())
                    return;

                if (m_bot->GetCorpse())
                {
                    SetState(BOTSTATE_DEADRELEASED);
                    return;
                }
                m_bot->SetBotDeathTimer();
                m_bot->BuildPlayerRepop();
                SetState(BOTSTATE_DEADRELEASED);

                if (m_bot->GetMap()->IsBattleGround())
                    return;

                WorldLocation loc;
                Corpse *corpse = m_bot->GetCorpse();
                corpse->GetPosition(loc);
                m_bot->TeleportTo(loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z, m_bot->GetOrientation());
            }
        }
        else if (m_botState == BOTSTATE_DEADRELEASED)
        {
            Corpse *corpse = m_bot->GetCorpse();
            if (!corpse)
                return;

            FollowCheckTeleport(corpse);

            if (corpse->GetGhostTime() + m_bot->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP) > time(0))
            {
                m_ignoreAIUpdatesUntilTime = corpse->GetGhostTime() + m_bot->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP);
                return;
            }

            m_ignoreAIUpdatesUntilTime = time(0) + 6;
            PlayerbotChatHandler ch(GetLeader());

            m_bot->ResurrectPlayer(0.5f);
            m_bot->SpawnCorpseBones();

            if (m_bot->getAttackers().empty())
            {
                uint32 lvl = m_bot->getLevel() +1;
                if (lvl < 81 && m_bot == GetLeader())
                {
                    m_bot->GiveLevel(lvl);
                    GetClassAI()->InitSpells(m_bot->GetPlayerbotAI());
                }
                m_bot->GMStartup();
                m_bot->SetHealth(m_bot->GetMaxHealth());
                m_bot->SetPower(m_bot->getPowerType(), m_bot->GetMaxPower(m_bot->getPowerType()));
            }

            SetState(BOTSTATE_NORMAL);
        }
    }
    else
    {
        Spell* const pSpell = GetCurrentSpell();
        if (pSpell && !(pSpell->IsChannelActive() || pSpell->IsAutoRepeat()))
            InterruptCurrentCastingSpell();

        //Never remove it otherwise bots are glued
        if (!m_followTarget || (!pSpell && m_bot->GetMotionMaster()->GetCurrentMovementGeneratorType()==IDLE_MOTION_TYPE))
            SetFollowTarget(GetLeader(), true);

        if (!IsInCombat())
        {
            if (m_targetCombat)
            {
                m_targetCombat = NULL;
                SetFollowTarget(GetLeader(), true);
                m_bot->AttackStop();
                m_bot->SetSelectionGuid(ObjectGuid());
                for (uint8 i = 0; i < CURRENT_MAX_SPELL; ++i)
                    m_bot->InterruptSpell(CurrentSpellTypes(i), true, false);
            }

            if (GetLeader()!=m_bot)
            {
                if (m_bot->getClass()==CLASS_DEATH_KNIGHT 
                    && GetLeader()->getLevel()<55)
                {
                    SetLeader(m_bot);
                    ReinitAI();
                    return;
                }
            }
            else
            {
                if (FindPOI())
                {
                    m_ignoreAIUpdatesUntilTime = time(0) + urand(5, 10);
                    m_bot->GetMotionMaster()->Clear(true);
                    MoveTo(rand_float(0, M_PI_F), 1.0f, 3.0f, orig_x, orig_y, orig_z);
                }
                else
                {
                    MoveTo(0.0f, 1.0f, 3.0f, orig_x, orig_y, orig_z);
                }
            }

            if (m_botState == BOTSTATE_COMBAT)
            {
                SetState(BOTSTATE_LOOTING);
                SetIgnoreUpdateTime();
            }
            else if (m_botState == BOTSTATE_LOOTING)
            {
                if (m_bot->getClass() == CLASS_HUNTER)
                       m_bot->HandleEmote(1);

                CastAura(25990, m_bot); // Regen hp + mana : Spell Aura Gruccu Food
                DoLoot();
            }
            else
            {
                CheckRoles();
                if(CheckLevel())
                    CheckStuff();
                GetClassAI()->DoNonCombatActions();
                CheckMount();
            }
        }
        else
        {
            if ((!GetLeader()->IsMounted()) && (m_bot->IsMounted()))
                UnMount();

            if (!pSpell || !pSpell->IsChannelActive())
                DoCombatManeuver();
            else
                SetIgnoreUpdateTime(1);
        }
    }
}

void PlayerbotAI::CheckRoles()
{
    switch (m_bot->getClass())
    {
        case CLASS_WARRIOR:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = WarriorArms;              break;
                        case 1:        m_new_role = WarriorProtection;        break;
                        default:       m_new_role = WarriorFury;              break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotWarriorAI(m_bot, this);
            }
            break;
        case CLASS_PALADIN:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = PaladinCombat;            break;
                        case 1:        m_new_role = PaladinHoly;              break;
                        default:       m_new_role = PaladinProtection;        break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotPaladinAI(m_bot, this);
            }
            break;
        case CLASS_HUNTER:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = HunterBeastMastery;       break;
                        case 1:        m_new_role = HunterSurvival;           break;
                        default:       m_new_role = HunterMarksmanship;       break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotHunterAI(m_bot, this);
            }
            break;
        case CLASS_ROGUE:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = RogueCombat;              break;
                        case 1:        m_new_role = RogueAssassination;       break;
                        default:       m_new_role = RogueSubtlety;            break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotRogueAI(m_bot, this);
            }
            break;
        case CLASS_PRIEST:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = PriestDiscipline;         break;
                        case 1:        m_new_role = PriestHoly;               break;
                        default:       m_new_role = PriestShadow;             break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotPriestAI(m_bot, this);
            }
            break;
        case CLASS_DEATH_KNIGHT:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = DeathKnightBlood;         break;
                        case 1:        m_new_role = DeathKnightFrost;         break;
                        default:       m_new_role = DeathKnightUnholy;        break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotDeathKnightAI(m_bot, this);
            }
            break;
        case CLASS_SHAMAN:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = ShamanElementalCombat;    break;
                        case 1:        m_new_role = ShamanRestoration;        break;
                        default:       m_new_role = ShamanEnhancement;        break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotShamanAI(m_bot, this);
            }
            break;
        case CLASS_MAGE:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = MageFire;                 break;
                        case 1:        m_new_role = MageArcane;               break;
                        default:       m_new_role = MageFrost;                break;
                    }
                }
                if (m_new_role!=MageFire && m_bot->getLevel() < 10)
                {
                    m_role = MageFire;
                    m_new_role = MageFire;
                }
                else if((m_new_role==MageArcane) && m_bot->getLevel() < 60)
                {
                    if (urand(0,1)==0)
                    {
                        m_role = MageFire;
                        m_new_role = MageFire;
                    }
                    else
                    {
                        m_role = MageFrost;
                        m_new_role = MageFrost;
                    }
                }
                else
                {
                    m_role = m_new_role;
                }
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotMageAI(m_bot, this);
            }
            break;
        case CLASS_WARLOCK:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = WarlockDestruction;       break;
                        case 1:        m_new_role = WarlockCurses;            break;
                        default:       m_new_role = WarlockSummoning;         break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotWarlockAI(m_bot, this);
            }
            break;
        case CLASS_DRUID:
            if (m_role != m_new_role)
            {
                if (m_new_role == 1)
                {
                    switch (urand(0, 2))
                    {
                        case 0:        m_new_role = DruidFeralCombat;         break;
                        case 1:        m_new_role = DruidRestoration;         break;
                        default:       m_new_role = DruidBalance;             break;
                    }
                }
                m_role = m_new_role;
                delete m_classAI;
                m_classAI = (PlayerbotClassAI*) new PlayerbotDruidAI(m_bot, this);
            }
            break;
    }
}

bool PlayerbotAI::FollowCheckTeleport(WorldObject *obj)
{
    if (m_bot->GetMap() && m_bot->GetMap()->IsBattleGroundOrArena())
        return true;

    if (!m_bot->IsWithinDistInMap(obj, 100, true) && GetLeader()->isAlive() && !GetLeader()->IsTaxiFlying())
    {
        m_targetCombat = NULL;

        if (m_bot == GetLeader())
            return false;

        Map* pMap = GetLeader()->GetMap();

        if (pMap->IsBattleGroundOrArena())
        {
            if (m_bot->GetBattleGroundId() && GetLeader()->GetBattleGroundId() != m_bot->GetBattleGroundId())
                return false;

            m_bot->SetBattleGroundId(GetLeader()->GetBattleGroundId(), GetLeader()->GetBattleGroundTypeId());
            if (!m_bot->GetMap()->IsBattleGroundOrArena())
                m_bot->SetBattleGroundEntryPoint();
        }
        else if (pMap->IsDungeon())
        {
            Map* cMap = m_bot->GetMap();
            if (cMap->Instanceable() && cMap->GetInstanceId() != pMap->GetInstanceId())
                return false;

            if (!GetLeader()->GetGroup() || !m_bot->GetGroup() ||
                (m_bot->GetGroup()->GetLeaderGuid() != GetLeader()->GetObjectGuid()) ||
                (GetLeader()->GetGroup()->GetLeaderGuid() != GetLeader()->GetObjectGuid()))
                return false;
        }

        if (m_bot->IsTaxiFlying())
        {
            m_bot->GetMotionMaster()->MovementExpired();
            m_bot->m_taxi.ClearTaxiDestinations();
        }
        else
            m_bot->SaveRecallPosition();

        float x,y,z;
        GetLeader()->GetClosePoint(x, y, z, m_bot->GetObjectBoundingRadius());
        m_bot->TeleportTo(GetLeader()->GetMapId(),x,y,z,m_bot->GetOrientation());
    }
    return true;
}

BotCombatType PlayerbotAI::GetCombatType()
{
    switch (m_bot->getRole())
    {
        case MageFire:
        case MageArcane:
        case MageFrost:
        case PriestDiscipline:
        case PriestHoly:
        case PriestShadow:
        case ShamanElementalCombat:
        case ShamanRestoration:
        case DruidRestoration:
        case DruidBalance:
        case WarlockDestruction:
        case WarlockCurses:
        case WarlockSummoning:
        case HunterBeastMastery:
        case HunterSurvival:
        case HunterMarksmanship:
        case PaladinHoly:
            return BOTCOMBAT_RANGED;
        case WarriorArms:
        case WarriorProtection:
        case WarriorFury:
        case RogueCombat:
        case RogueAssassination:
        case RogueSubtlety:
        case DruidFeralCombat:
            return BOTCOMBAT_CAC;
        case ShamanEnhancement:
        case PaladinCombat:
        case PaladinProtection:
        case DeathKnightBlood:
        case DeathKnightFrost:
        case DeathKnightUnholy:
        {
            Spell *sp = GetCurrentSpell();
            if (sp && sp->IsRangedSpell())
                return BOTCOMBAT_RANGED;
            else
                return BOTCOMBAT_CAC;
        }
        default:
            return BOTCOMBAT_CAC;
    }
}
