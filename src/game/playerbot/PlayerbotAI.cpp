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
    m_CurrentlyCastingSpellId = 0;

    m_TimeDoneEating = 0;
    m_TimeDoneDrinking = 0;

    m_taxiMaster = ObjectGuid();
    m_latestVictim = NULL;

    m_classAI = 0;
    m_role = 0;
    m_new_role = 1;

    m_enterBg = 0;
    m_leaveBg = 0;

    m_botState = BOTSTATE_NORMAL;

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

    if (m_bot->m_movementInfo.HasMovementFlag(MOVEFLAG_FLYING))
        m_bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);

    if (m_bot->isDead())
        m_bot->ResurrectPlayer(100.0f);

    if (m_bot == GetLeader())
    {
        InitBotStatsForLevel(m_bot->GetLevelAtLoading());
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
    if (!GetLeader() || !GetLeader()->IsInWorld() || !GetLeader()->GetSession() || GetLeader()->GetSession()->PlayerLogout())
        return;

    switch (packet.GetOpcode())
    {
        case SMSG_DUEL_WINNER:
        {
            m_bot->HandleEmoteCommand(EMOTE_ONESHOT_APPLAUD);
            return;
        }
        case SMSG_DUEL_COMPLETE:
        {
            m_bot->GetMotionMaster()->Clear();
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
            ObjectGuid casterGuid;
            p >> casterGuid.ReadAsPacked();
            if (casterGuid != m_bot->GetObjectGuid())
                return;
            m_ignoreAIUpdatesUntilTime = time(0);
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
            ObjectGuid guid;
            p >> guid.ReadAsPacked();
            if (guid != m_bot->GetObjectGuid())
                return;
            m_bot->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
            SetFollowTarget(GetLeader(), true);
            return;
        }

        case SMSG_MOVE_UNSET_CAN_FLY:
        {
            WorldPacket p(packet);
            ObjectGuid guid;
            p >> guid.ReadAsPacked();
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
                Player* newLeader = FindGroupRandomRealPlayer();
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
                m_bot->GetMotionMaster()->Clear();
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

        case SMSG_SPELL_START:
        {
            WorldPacket p(packet);
            ObjectGuid castItemGuid;
            p >> castItemGuid.ReadAsPacked();
            ObjectGuid casterGuid;
            p >> casterGuid.ReadAsPacked();
            uint8 castCount;
            p >> castCount;
            uint32 spellId;
            p >> spellId;
            uint32 castFlags;
            p >> castFlags;
            uint32 msTime;
            p >> msTime;

            const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
            if (!pSpellInfo)
                return;

            if (casterGuid == m_bot->GetObjectGuid())
            {
                if (pSpellInfo->AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_SEATED)
                    return;
            }
            else if(casterGuid == GetLeader()->GetObjectGuid())
            {
                if (IsPositiveSpell(pSpellInfo))
                    return;

                Unit* pTarget = ObjectAccessor::GetUnit(*m_bot, GetLeader()->GetSelectionGuid());
                if (!pTarget)
                    return;

                m_latestVictim = pTarget;
                Player* mainTank = FindGroupMainTank();

                if (mainTank)
                {
                    if (mainTank==m_bot)
                    {
                        if (m_bot->getRole()==DruidFeralCombat) //TODO create proper first_combat_action for AI
                            Cast(5487);
                        MoveTo(m_latestVictim);
                    }
                    else
                        MoveTo(m_latestVictim, m_bot->GetAngle(m_latestVictim)+rand_float(-0.25f, 0.25f), GetDist(), 0.0f);
                }
                else
                {
                    if (GetCombatType()==BOTCOMBAT_CAC)
                        MoveTo(m_latestVictim);
                    else
                        MoveTo(m_latestVictim, m_bot->GetAngle(m_latestVictim)+rand_float(-0.25f, 0.25f), GetDist(), 0.0f);
                }
            }
            return;
        }

        case SMSG_SPELL_GO:
        {
            WorldPacket p(packet);
            ObjectGuid castItemGuid;
            p >> castItemGuid.ReadAsPacked();
            ObjectGuid casterGuid;
            p >> casterGuid.ReadAsPacked();
            if (casterGuid != m_bot->GetObjectGuid())
                return;

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

                m_CurrentlyCastingSpellId = 0;
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

Unit* PlayerbotAI::GetNewCombatTarget(bool no_dist_check)
{
    Unit* newCombatTarget = NULL;

    //TODO thread system
    if (m_botState != BOTSTATE_COMBAT)
    {
        SetState(BOTSTATE_COMBAT);
        SetQuestNeedItems();
        m_victimList.clear();
        m_lootCreature = 0;
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
                if (!IsValidEnemy((*itr)->GetOwner() ? (*itr)->GetOwner() : (*itr), no_dist_check))
                    continue;

                if ((*itr)->GetTypeId()==TYPEID_PLAYER)
                    return (*itr);

                if ((*itr)->GetOwner())
                {
                    if ((*itr)->GetOwner()->GetTypeId()==TYPEID_PLAYER)
                        return (*itr)->GetOwner();
                    else
                        return (*itr)->GetOwner();
                }

                return (*itr);
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
                        if (!IsValidEnemy((*itr)->GetOwner() ? (*itr)->GetOwner() : (*itr), no_dist_check))
                            continue;

                        if ((*itr)->GetTypeId()==TYPEID_PLAYER)
                            return (*itr);

                        if ((*itr)->GetOwner())
                        {
                            if ((*itr)->GetOwner()->GetTypeId()==TYPEID_PLAYER)
                                return (*itr)->GetOwner();
                            else
                                return (*itr)->GetOwner();
                        }

                        return (*itr);
                    }
                }
                ref = ref->next();
            }
        }
        if (m_bot->GetPet() && !m_bot->GetPet()->getAttackers().empty())
        {
            for(Unit::AttackerSet::const_iterator itr = m_bot->GetPet()->getAttackers().begin(); itr != m_bot->GetPet()->getAttackers().end(); ++itr)
            {
                if (!IsValidEnemy((*itr)->GetOwner() ? (*itr)->GetOwner() : (*itr), no_dist_check))
                    continue;

                if ((*itr)->GetTypeId()==TYPEID_PLAYER)
                    return (*itr);

                if ((*itr)->GetOwner())
                {
                    if ((*itr)->GetOwner()->GetTypeId()==TYPEID_PLAYER)
                        return (*itr)->GetOwner();
                    else
                        return (*itr)->GetOwner();
                }

                return (*itr);
            }
        }
        if (m_bot->GetGroup())
        {
            GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
            while (ref)
            {
                if (ref->getSource()->GetPet() && !ref->getSource()->GetPet()->getAttackers().empty())
                {
                    for(Unit::AttackerSet::const_iterator itr = ref->getSource()->GetPet()->getAttackers().begin(); itr != ref->getSource()->GetPet()->getAttackers().end(); ++itr)
                    {
                        if (!IsValidEnemy((*itr)->GetOwner() ? (*itr)->GetOwner() : (*itr), no_dist_check))
                            continue;

                        if ((*itr)->GetTypeId()==TYPEID_PLAYER)
                            return (*itr);

                        if ((*itr)->GetOwner())
                        {
                            if ((*itr)->GetOwner()->GetTypeId()==TYPEID_PLAYER)
                                return (*itr)->GetOwner();
                            else
                                return (*itr)->GetOwner();
                        }

                        return (*itr);
                    }
                }
                ref = ref->next();
            }
        }
    }
    //TODO ajouter de quoi rendre les bots agressifs
    return NULL;
}

bool PlayerbotAI::IsValidEnemy(Unit* combatTarget, bool no_dist_check)
{
    if (!combatTarget)
        return false;

    if (!combatTarget->IsInWorld())
        return false;

    if (!combatTarget->isAlive())
        return false;

    if (!m_bot->IsInMap(combatTarget))
        return false;
    
    if (!no_dist_check && !m_bot->IsWithinDistInMap(combatTarget, MAX_DIST_COMBAT_TARGET))
        return false;

    if (!combatTarget->isTargetableForAttack())
        return false;
    
    if (combatTarget->IsInWater())
        return false;

    if (!m_bot->IsWithinLOSInMap(combatTarget))
        return false;

    return true;
}

void PlayerbotAI::DoCombatManeuver(Unit* forcedTarget)
{
    UnMount();

    Unit* combatTarget = m_bot->getVictim();
    if (!forcedTarget)
    {
        if (m_bot->GetBattleGround())
        {
            if (!combatTarget || !combatTarget->IsInWorld()
                || !combatTarget->isAlive() || !m_bot->IsInMap(combatTarget)
                || !((Player*)combatTarget)->GetBattleGround())
            {
                combatTarget = GetNewCombatTarget();
                AttackStart(combatTarget);
            }
        }
        else
        {
            if (!IsValidEnemy(combatTarget))
            {
                combatTarget = GetNewCombatTarget();
                AttackStart(combatTarget);
            }
        }
    }
    else if (forcedTarget->IsInWorld() && forcedTarget->isAlive())
    {
        if (m_bot->getVictim())
        {
            m_bot->CombatStop(true);
            m_bot->SetSelectionGuid(ObjectGuid());
        }
        combatTarget = forcedTarget;
        AttackStart(forcedTarget);
    }
    else
    {
        m_latestVictim = NULL;
    }

    if (!combatTarget)
    {
        combatTarget = GetNewCombatTarget(true);
        AttackStart(combatTarget);
    }

    if (!combatTarget)
    {
        m_bot->CombatStop(true);
        SetFollowTarget(GetLeader(), true);
        return;
    }

    SetInFront(combatTarget);

    if (GetCombatType()==BOTCOMBAT_CAC)
    {
        if (combatTarget->getVictim() == m_bot && m_bot->IsWithinDistInMap(combatTarget, ATTACK_DISTANCE))
        {
            if (m_bot->getClass() == CLASS_ROGUE)
            {
                if (!GetClassAI()->DoEvadeAction())
                    GetClassAI()->DoProtectSelfAction();
            }
            else
                GetClassAI()->DoProtectSelfAction();
        }

        if (m_bot->IsWithinDistInMap(combatTarget, MIN_DIST_COMBAT_CAC_TARGET))
        {
            //Last Attack() has failed so :
            if (!m_bot->getVictim())
            {
                //If new succeeded you can remove old reference
                if (m_bot->Attack(combatTarget, true))
                    m_latestVictim = NULL;
                else
                    MoveTo(combatTarget);
                //If new has fail too, move to near target !
            }
            if (!GetClassAI()->DoCombatManeuver(combatTarget, true))
            {
                if (m_bot->isAttackReady())
                {
                    m_bot->AttackerStateUpdate(combatTarget);
                    m_bot->resetAttackTimer();
                }
            }
        }
        else
        {
            m_latestVictim = m_bot->getVictim() ? m_bot->getVictim() : combatTarget;
            m_bot->AttackStop();
            m_bot->SendAttackSwingCancelAttack();
            m_bot->ClearInCombat();
            MoveTo(m_latestVictim);
            GetClassAI()->DoFastBuffOnOneself();
        }
    }
    else
    {
        if (m_bot->IsWithinDistInMap(combatTarget, ATTACK_DISTANCE))
        {
            if (combatTarget->getVictim() != m_bot)
            {
                m_bot->AttackStop();
                m_bot->SendAttackSwingCancelAttack();
                m_bot->ClearInCombat();
                float x, y, z;
                m_bot->GetClosePoint(x, y, z, combatTarget->GetObjectBoundingRadius(), (MAX_DIST_COMBAT_RANGED_TARGET - 15.0f), m_bot->GetOrientation(), combatTarget);
                MoveTo(x, y, z);
            }
            else
            {
                if (!GetClassAI()->DoEvadeAction())
                    GetClassAI()->DoProtectSelfAction();
            }
        }
        
        if (HasArrived())
        {
            if (!m_bot->getVictim())
                m_bot->Attack(combatTarget, false);
            if (!GetClassAI()->DoCombatManeuver(combatTarget, m_bot->IsWithinDistInMap(combatTarget, ATTACK_DISTANCE)))
            {
                if (m_bot->isAttackReady(RANGED_ATTACK))
                {
                    Item* item = m_bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
                    if (item)
                    {
                        if (m_bot->HasSpell(3018))
                            CastSpell(3018, combatTarget);
                        if (m_bot->HasSpell(5019))
                            CastSpell(5019, combatTarget);
                        m_bot->InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
                        m_bot->resetAttackTimer(RANGED_ATTACK);
                    }
                }
            }
        }
    }
}

void PlayerbotAI::AttackStart(Unit *pWho)
{
    if (!pWho)
        return;

    GetClassAI()->ReinitCycles();
    m_bot->SetSelectionGuid(pWho->GetObjectGuid());
    m_victimList.push_back(pWho);

    if (GetCombatType()==BOTCOMBAT_CAC)
        MoveTo(pWho);
    else
        MoveTo(pWho, 3.0f*M_PI_F/2.0f, MAX_DIST_COMBAT_RANGED_TARGET, 0.0f);
}

bool PlayerbotAI::IsInCombat()
{
    if (m_latestVictim)
        return true;

    if (m_bot->duel)
        return true;

    if (m_bot->GetBattleGround())
        return true;

    if (m_bot->getVictim())
        return true;

    if (!m_bot->getAttackers().empty()
        || (m_bot->GetPet() && !m_bot->GetPet()->getAttackers().empty()))
        return true;

    if (m_bot->GetGroup())
    {
        GroupReference *ref = m_bot->GetGroup()->GetFirstMember();
        while (ref)
        {
            if (!m_bot->IsWithinDistInMap(ref->getSource()->GetOwner() ? ref->getSource()->GetOwner() : ref->getSource(), MAX_DIST_COMBAT_TARGET))
            {
                ref = ref->next();
                continue;
            }

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
        m_bot->GetMotionMaster()->Clear();
        return;
    }

    if (!forced && m_bot->GetMotionMaster()->GetDestinationTarget()==followTarget)
        return;

    if (m_bot->GetMotionMaster()->GetDestinationTarget()==followTarget)
    {
        Player* grpFollowTarget = FindGroupRandomRealPlayer();
        if (grpFollowTarget)
            followTarget = grpFollowTarget;
    }

    if (followTarget->GetTypeId() == TYPEID_PLAYER)
    {
        Player* target = ((Player*)followTarget);
        if (target->GetCorpse() && !FollowCheckTeleport(target->GetCorpse()))
            return;
        if (!FollowCheckTeleport(followTarget))
            return;
    }

    if (m_bot == GetLeader())
    {
        if (!m_bot->GetBattleGround())
        {
            MoveTo(followTarget);
        }
        else
        {
            if (m_bot->IsWithinDistInMap(followTarget, orig_x, orig_y, orig_z, MIN_DIST_COMBAT_CAC_TARGET))
                SetInFront(followTarget);
            else
                MoveTo(followTarget);
        }
    }
    else
    {
        if (!m_bot->GetBattleGround())
        {
            if (m_bot->IsWithinDistInMap(followTarget, MAX_DIST_BETWEEN_BOT_LEADER))
            {
                if (followTarget == GetLeader())
                {
                    if (GetCombatType()==BOTCOMBAT_CAC)
                        MoveTo(followTarget, rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)+M_PI_F); //CAC
                    else
                        MoveTo(followTarget, rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)); //RANGED
                    SetInFront(followTarget);
                }
                else
                {
                    if (GetCombatType()==BOTCOMBAT_CAC)
                    {
                        MoveTo(followTarget, rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)+M_PI_F); //CAC
                    }
                    else
                    {
                        Spell* sp = GetCurrentSpell();
                        if (!sp)
                        {
                            MoveTo(followTarget, rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)); //RANGED
                        }
                        else if (sp->IsRangedSpell())
                        {
                            SpellRangeEntry const *TempRange = GetSpellRangeStore()->LookupEntry(sp->m_spellInfo->rangeIndex);
                            if (!TempRange)
                            {
                                MoveTo(followTarget, rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)); //RANGED
                            }
                            else
                            {
                                if (!m_bot->IsInRange(followTarget,TempRange->minRange,TempRange->maxRange))
                                {
                                    MoveTo(followTarget, rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)+M_PI_F, TempRange->maxRange); //RANGED
                                }
                            }
                        }
                        else
                        {
                            MoveTo(followTarget, rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)); //RANGED
                        }
                    }
                }
            }
        }
        else
        {
            if (m_bot->IsWithinDistInMap(followTarget, MIN_DIST_COMBAT_CAC_TARGET))
                SetInFront(followTarget);
            else if (m_bot->IsInMap(GetLeader()))
                MoveTo(followTarget, rand_float(0, M_PI_F));
        }
    }
}

void PlayerbotAI::MoveTo(Unit* target, float angle, float minDist, float maxDist)
{
    if (!target || !target->IsInWorld())
        return;

    float dist = maxDist;

    if (target==GetLeader())
    {
        Group *gr = m_bot->GetGroup();
        dist = gr ? (float)gr->GetMembersCount() : 1.0f;
        dist > MAX_DIST_POS_IN_GROUP ? MAX_DIST_POS_IN_GROUP : dist;
     }
     m_bot->GetMotionMaster()->SetDestinationTarget(target, dist == 0.0f ? minDist : rand_float(minDist, dist), angle);
     SetInFront(target);
}

void PlayerbotAI::MoveTo(float x, float y, float z)
{
    m_bot->UpdateGroundPositionZ(x,y,z);
    m_bot->GetMotionMaster()->SetDestinationXYZ(x, y, z);
}

bool PlayerbotAI::HasArrived()
{
    return m_bot->GetMotionMaster()->HasArrived();
}

void PlayerbotAI::MoveInLineOfSight(Unit *u)
{
    /*if (m_bot->getVictim())
        return;

    if (u->isTargetableForAttack())
    {
        if (m_bot->IsWithinLOSInMap(u))
        {
            AttackStart(u);
        }
    }*/
}

Unit* PlayerbotAI::FindPOI()
{
    Unit* destTarget = m_bot->GetMotionMaster()->GetDestinationTarget();
    if (!destTarget)
    {
        Unit* target = m_bot->SelectRandomFriendlyTargetBetween(0, 10.0f, MAX_RANGE_MOVEMENT);
        if (target)
            return target;
        return NULL;
    }
    else if (m_bot->IsWithinDistInMap(destTarget, 5.0f))
    {
        Unit* target = m_bot->SelectRandomFriendlyTargetBetween(0, 10.0f, MAX_RANGE_MOVEMENT);
        if (target && target != destTarget)
            return target;
        return NULL;
    }
    return destTarget;
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
    m_bot->SendHeartBeat();
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

    if (!m_bot->GetMotionMaster()->GetDestinationTarget()
        || m_bot->GetMotionMaster()->GetCurrentMovementGeneratorType()!=FOLLOW_MOTION_TYPE)
    {
        m_bot->GetMotionMaster()->MoveFollow(GetLeader(), 0.0f, 0.0f);
        SetFollowTarget(GetLeader(), true);
    }

    if (m_bot->GetTrader())
        return;

    CheckBG();

    if (!m_bot->isAlive())
    {
        if (m_botState != BOTSTATE_DEAD && m_botState != BOTSTATE_DEADRELEASED)
        {
            m_victimList.clear();
            m_lootCreature = 0;
            m_bot->SetSelectionGuid(ObjectGuid());
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
                uint32 lvl = m_bot->getLevel();
                if (m_bot == GetLeader())
                    lvl = (lvl+1) > DEFAULT_MAX_LEVEL ? DEFAULT_MAX_LEVEL : (lvl+1);
                InitBotStatsForLevel(lvl);
            }

            SetState(BOTSTATE_NORMAL);
        }
    }
    else
    {
        m_bot->GetIndispensableItems();

        /*sLog.outString("%s => %s", m_bot->GetName(), m_followTarget ? m_followTarget->GetName() : "NULL");
        sLog.outString("%s !> %s", m_bot->GetName(), m_bot->getVictim() ? m_bot->getVictim()->GetName() : "NULL");
        sLog.outString("Movement : %u", m_bot->GetMotionMaster()->GetCurrentMovementGeneratorType());
        sLog.outString("(100)=%u", m_bot->IsWithinDistInMap(GetLeader(), MAX_DIST_BETWEEN_BOT_LEADER) ? 1 : 0);
        sLog.outString("===============================================");*/

        if (!IsInCombat())
        {
            if (!m_bot->IsWithinDistInMap(GetLeader(), MAX_DIST_BETWEEN_BOT_LEADER) ||
                m_bot->GetMotionMaster()->GetDestinationTarget()!=GetLeader())
                SetFollowTarget(GetLeader(), true);

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
                if (HasArrived())
                {
                    Unit* destTarget = FindPOI();
                    if (destTarget)
                    {
                        m_ignoreAIUpdatesUntilTime = time(0) + urand(5, 10);
                        MoveTo(destTarget, rand_float(0, M_PI_F));
                    }
                    else
                    {
                        MoveTo(orig_x, orig_y, orig_z);
                    }
                }
            }

            CheckRoles();
            if (GetLeader()->getLevel() != m_bot->getLevel())
                InitBotStatsForLevel(GetLeader()->getLevel());
            GetClassAI()->DoNonCombatActions();
            CheckMount();

            switch (m_botState)
            {
                case BOTSTATE_COMBAT:
                {
                    SetState(BOTSTATE_LOOTING);
                    SetIgnoreUpdateTime();
                    break;
                }
                case BOTSTATE_LOOTING:
                {
                    m_bot->CombatStop(true);
                    WorldPacket data( SMSG_ATTACKSTOP, (4+16) );
                    data << m_bot->GetPackGUID();
                    data << uint64(0);
                    data << uint32(0);
                    m_bot->SendMessageToSet(&data, true);
                    m_bot->SetSelectionGuid(ObjectGuid());
                    InterruptCurrentCastingSpell();
                    CastAura(25990, m_bot); // Regen hp + mana : Spell Aura Gruccu Food
                    DoLoot();
                    break;
                }
                default:
                    break;
            }
        }
        else
        {
            UnMount();
            if (!GetCurrentSpell() && HasArrived())
                DoCombatManeuver(m_latestVictim);
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

    if (!m_bot->IsWithinDistInMap(obj, MAX_DIST_BETWEEN_BOT_LEADER, true) && GetLeader()->isAlive() && !GetLeader()->IsTaxiFlying())
    {
        m_bot->CombatStop(true);
        m_bot->SetSelectionGuid(ObjectGuid());

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
        Group *gr = m_bot->GetGroup();
        float dist = gr ? (float)gr->GetMembersCount() : 1.0f;
        dist > MAX_DIST_POS_IN_GROUP ? MAX_DIST_POS_IN_GROUP : dist;
        float angle = GetCombatType()==BOTCOMBAT_CAC ? rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f)+M_PI_F : rand_float(M_PI_F/2.0f, 3.0f*M_PI_F/2.0f);
        GetLeader()->GetNearPoint(GetLeader(), x,y,z, m_bot->GetObjectBoundingRadius(), dist, angle);
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
