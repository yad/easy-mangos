    /*
    Name    : PlayerbotRogueAI.cpp
    Complete: maybe around 28%
    Author    : Natsukawa
    Version : 0.37
    */
#include "PlayerbotRogueAI.h"
#include "PlayerbotMgr.h"

class PlayerbotAI;
PlayerbotRogueAI::PlayerbotRogueAI(Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(bot, ai)
{
    InitSpells(ai);
}

PlayerbotRogueAI::~PlayerbotRogueAI() {}

void PlayerbotRogueAI::InitSpells(PlayerbotAI* const ai)
{
    SINISTER_STRIKE     = ai->getSpellId("sinister strike"); //COMBAT
    BACKSTAB            = ai->getSpellId("backstab");
    KICK                = ai->getSpellId("kick");
    FEINT               = ai->getSpellId("feint");
    FAN_OF_KNIVES       = ai->getSpellId("fan of knives");
//    DEADLY_POISON       = 20844; //ItemID
    DEADLY_POISON       = 25351; //SpellID
//    CRIPPLING_POISON    = 3775; //ItemID
    CRIPPLING_POISON    = 3408; //SpellID
    MIND_NUMBING_POISON = 5761; //SpellID
    GOUGE               = ai->getSpellId("gouge");
    SPRINT              = ai->getSpellId("sprint");

    SHADOWSTEP          = ai->getSpellId("shadowstep"); //SUBTLETY
    STEALTH             = ai->getSpellId("stealth");
    VANISH              = ai->getSpellId("vanish");
    EVASION             = ai->getSpellId("evasion");
    CLOAK_OF_SHADOWS    = ai->getSpellId("cloak of shadows");
    HEMORRHAGE          = ai->getSpellId("hemorrhage");
    GHOSTLY_STRIKE      = ai->getSpellId("ghostly strike");
    SHADOW_DANCE        = ai->getSpellId("shadow dance");
    BLIND               = ai->getSpellId("blind");
    DISTRACT            = ai->getSpellId("distract");
    PREPARATION         = ai->getSpellId("preparation");
    PREMEDITATION       = ai->getSpellId("premeditation");

    EVISCERATE          = ai->getSpellId("eviscerate"); //ASSASSINATION
    KIDNEY_SHOT         = ai->getSpellId("kidney shot");
    SLICE_DICE          = ai->getSpellId("slice and dice");
    GARROTE             = ai->getSpellId("garrote");
    EXPOSE_ARMOR        = ai->getSpellId("expose armor");
    RUPTURE             = ai->getSpellId("rupture");
    DISMANTLE           = ai->getSpellId("dismantle");
    CHEAP_SHOT          = ai->getSpellId("cheap shot");
    AMBUSH              = ai->getSpellId("ambush");
    MUTILATE            = ai->getSpellId("mutilate");

    RECENTLY_BANDAGED   = 11196; // first aid check
    // racial
    ARCANE_TORRENT          = ai->getSpellId("arcane torrent"); // blood elf
    STONEFORM               = ai->getSpellId("stoneform"); // dwarf
    ESCAPE_ARTIST           = ai->getSpellId("escape artist"); // gnome
    EVERY_MAN_FOR_HIMSELF   = ai->getSpellId("every man for himself"); // human
    SHADOWMELD              = ai->getSpellId("shadowmeld"); // night elf
    BLOOD_FURY              = ai->getSpellId("blood fury"); // orc
    BERSERKING              = ai->getSpellId("berserking"); // troll
    WILL_OF_THE_FORSAKEN    = ai->getSpellId("will of the forsaken"); // undead
}

bool PlayerbotRogueAI::DoFirstCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    Player * m_bot = GetPlayerBot();

    if ( STEALTH>0 && !m_bot->HasAura( STEALTH ) && ai->CastSpell(STEALTH, *m_bot) )
        return false;

    return false;
}

void PlayerbotRogueAI::DoNextCombatManeuver(Unit *pTarget)
{
    if ( !pTarget )
        return;

    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            if (SINISTER_STRIKE > 0)
                ai->CastSpell(SINISTER_STRIKE);
            return;
    }

    ai->SetInFront( pTarget );
    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();
    float fTargetDist = m_bot->GetDistance( pTarget );

    // decide what to do:
    if ( pVictim==m_bot && CLOAK_OF_SHADOWS>0 && pVictim->HasAura(SPELL_AURA_PERIODIC_DAMAGE) && !m_bot->HasAura(CLOAK_OF_SHADOWS,EFFECT_INDEX_0) && ai->CastSpell(CLOAK_OF_SHADOWS) )
        return;
    else if ( m_bot->HasAura( STEALTH ) )
        SpellSequence = RogueStealth;
    else if ( pTarget->IsNonMeleeSpellCasted(true) )
        SpellSequence = RogueSpellPreventing;
    else if ( pVictim==m_bot && ai->GetHealthPercent()<40 )
        SpellSequence = RogueThreat;
    else
        SpellSequence = RogueCombat;

    // we fight in melee, target is not in range, skip the next part!
    if ( fTargetDist > ATTACK_DISTANCE )
        return;

    switch (SpellSequence)
    {
        case RogueStealth:
            if ( PREMEDITATION>0 && ai->CastSpell(PREMEDITATION,*pTarget) )
            {
            }
            else if ( AMBUSH>0 && ai->GetEnergyAmount()>=60 && ai->CastSpell(AMBUSH,*pTarget) )
            {
            }
            else if ( CHEAP_SHOT>0 && !pTarget->HasAura(CHEAP_SHOT,EFFECT_INDEX_0) && ai->GetEnergyAmount()>=60 && ai->CastSpell(CHEAP_SHOT,*pTarget) )
            {
            }
            else if ( GARROTE>0 && ai->GetEnergyAmount()>=50 && ai->CastSpell(GARROTE,*pTarget) )
            {
            }
            else
            {
            }
            break;
        case RogueThreat:
            if ( GOUGE>0 && ai->GetEnergyAmount()>=45 && !pTarget->HasAura(GOUGE,EFFECT_INDEX_0) && ai->CastSpell(GOUGE,*pTarget) )
            {
            }
            else if ( EVASION>0 && ai->GetHealthPercent()<=35 && !m_bot->HasAura(EVASION,EFFECT_INDEX_0) && ai->CastSpell(EVASION) )
            {
            }
            else if ( BLIND>0 && ai->GetHealthPercent()<=30 && !pTarget->HasAura(BLIND,EFFECT_INDEX_0) && ai->GetEnergyAmount()>=30 && ai->CastSpell(BLIND,*pTarget) )
            {
            }
            else if ( FEINT>0 && ai->GetHealthPercent()<=25 && ai->GetEnergyAmount()>=20 && ai->CastSpell(FEINT) )
            {
            }
            else if ( VANISH>0 && ai->GetHealthPercent()<=20 && !m_bot->HasAura(FEINT,EFFECT_INDEX_0) && ai->CastSpell(VANISH) )
            {
            }
            else if ( PREPARATION>0 && ai->CastSpell(PREPARATION) )
            {
            }
            else if ( m_bot->getRace() == RACE_NIGHTELF && ai->GetHealthPercent()<=15 && !m_bot->HasAura( SHADOWMELD, EFFECT_INDEX_0 ) && ai->CastSpell( SHADOWMELD, *m_bot ) )
            {
            }
            else
            {
            }
            break;
        case RogueSpellPreventing:
            if ( KIDNEY_SHOT>0 && ai->GetEnergyAmount()>=25 && m_bot->GetComboPoints()>=2 && ai->CastSpell(KIDNEY_SHOT,*pTarget) )
            {
            }
            else if ( KICK>0 && ai->GetEnergyAmount()>=25  && ai->CastSpell(KICK,*pTarget) )
            {
            }
            else
            {
            }
            break;
        case RogueCombat:
        default:
            if ( m_bot->GetComboPoints()<=4 )
            {
                if ( SHADOW_DANCE>0 && !m_bot->HasAura(SHADOW_DANCE,EFFECT_INDEX_0) && ai->CastSpell(SHADOW_DANCE,*m_bot) )
                {
                }
                else if ( CHEAP_SHOT>0 && m_bot->HasAura(SHADOW_DANCE,EFFECT_INDEX_0) && !pTarget->HasAura(CHEAP_SHOT,EFFECT_INDEX_0) && ai->GetEnergyAmount()>=60 && ai->CastSpell(CHEAP_SHOT,*pTarget) )
                {
                }
                else if ( AMBUSH>0 && m_bot->HasAura(SHADOW_DANCE,EFFECT_INDEX_0) && ai->GetEnergyAmount()>=60 && ai->CastSpell(AMBUSH,*pTarget) )
                {
                }
                else if ( GARROTE>0 && m_bot->HasAura(SHADOW_DANCE,EFFECT_INDEX_0) && ai->GetEnergyAmount()>=50 && ai->CastSpell(GARROTE,*pTarget) )
                {
                }
                else if ( BACKSTAB>0 && pTarget->isInBackInMap(m_bot,1) && ai->GetEnergyAmount()>=60 && ai->CastSpell(BACKSTAB,*pTarget) )
                {
                }
                else if ( MUTILATE>0 && ai->GetEnergyAmount()>=60 && ai->CastSpell(MUTILATE,*pTarget) )
                {
                }
                else if ( SINISTER_STRIKE>0 && ai->GetEnergyAmount()>=45 && ai->CastSpell(SINISTER_STRIKE,*pTarget) )
                {
                }
                else if ( GHOSTLY_STRIKE>0 && ai->GetEnergyAmount()>=40 && ai->CastSpell(GHOSTLY_STRIKE,*pTarget) )
                {
                }
                else if ( HEMORRHAGE>0 && ai->GetEnergyAmount()>=35 && ai->CastSpell(HEMORRHAGE,*pTarget) )
                {
                }
                else if ( DISMANTLE>0 && !pTarget->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_DISARMED) && ai->GetEnergyAmount()>=25 && ai->CastSpell(DISMANTLE,*pTarget) )
                {
                }
                else if ( SHADOWSTEP>0 && ai->GetEnergyAmount()>=10 && ai->CastSpell(SHADOWSTEP,*pTarget) )
                {
                }
                else if ( m_bot->getRace() == RACE_BLOODELF && !pTarget->HasAura( ARCANE_TORRENT,EFFECT_INDEX_0 ) && ai->CastSpell( ARCANE_TORRENT,*pTarget ) )
                {
                }
                else if ( m_bot->getRace() == RACE_HUMAN && (m_bot->hasUnitState( UNIT_STAT_STUNNED ) || m_bot->HasAuraType( SPELL_AURA_MOD_FEAR ) || m_bot->HasAuraType( SPELL_AURA_MOD_DECREASE_SPEED ) || m_bot->HasAuraType( SPELL_AURA_MOD_CHARM )) && ai->CastSpell( EVERY_MAN_FOR_HIMSELF, *m_bot ) )
                {
                }
                else if ( m_bot->getRace() == RACE_UNDEAD_PLAYER && (m_bot->HasAuraType( SPELL_AURA_MOD_FEAR ) || m_bot->HasAuraType( SPELL_AURA_MOD_CHARM )) && ai->CastSpell( WILL_OF_THE_FORSAKEN, *m_bot ) )
                {
                }
                else if ( m_bot->getRace() == RACE_DWARF && m_bot->HasAuraState( AURA_STATE_DEADLY_POISON ) && ai->CastSpell( STONEFORM, *m_bot ) )
                {
                }
                else if ( m_bot->getRace() == RACE_GNOME && (m_bot->hasUnitState( UNIT_STAT_STUNNED ) || m_bot->HasAuraType( SPELL_AURA_MOD_DECREASE_SPEED )) && ai->CastSpell( ESCAPE_ARTIST, *m_bot ) )
                {
                }
                else if ( m_bot->getRace() == RACE_ORC && !m_bot->HasAura( BLOOD_FURY, EFFECT_INDEX_0 ) && ai->CastSpell( BLOOD_FURY, *m_bot ) )
                {
                }
                else if ( m_bot->getRace() == RACE_TROLL && !m_bot->HasAura( BERSERKING, EFFECT_INDEX_0 ) && ai->CastSpell( BERSERKING, *m_bot ) )
                {
                }
                else
                {
                }
            }
            else
            {
                if ( EVISCERATE>0 && pTarget->getClass()==CLASS_ROGUE && ai->GetEnergyAmount()>=35 && ai->CastSpell(EVISCERATE,*pTarget) )
                {
                }
                else if ( EVISCERATE>0 && pTarget->getClass()==CLASS_DRUID && ai->GetEnergyAmount()>=35 && ai->CastSpell(EVISCERATE,*pTarget) )
                {
                }
                else if ( KIDNEY_SHOT>0 && pTarget->getClass()==CLASS_SHAMAN && ai->GetEnergyAmount()>=25 && ai->CastSpell(KIDNEY_SHOT,*pTarget) )
                {
                }
                else if (SLICE_DICE>0 && pTarget->getClass()==CLASS_WARLOCK && ai->GetEnergyAmount()>=25 && ai->CastSpell(SLICE_DICE, *pTarget) )
                {
                }
                else if (SLICE_DICE>0 && pTarget->getClass()==CLASS_HUNTER && ai->GetEnergyAmount()>=25 && ai->CastSpell(SLICE_DICE, *pTarget) )
                {
                }
                else if (EXPOSE_ARMOR>0 && pTarget->getClass()==CLASS_WARRIOR && !pTarget->HasAura(EXPOSE_ARMOR, EFFECT_INDEX_0) && ai->GetEnergyAmount()>=25 && ai->CastSpell(EXPOSE_ARMOR, *pTarget) )
                {
                }
                else if (EXPOSE_ARMOR>0 && pTarget->getClass()==CLASS_PALADIN && !pTarget->HasAura(EXPOSE_ARMOR, EFFECT_INDEX_0) && ai->GetEnergyAmount()>=25 && ai->CastSpell(EXPOSE_ARMOR, *pTarget) )
                {
                }
                else if (EXPOSE_ARMOR>0 && pTarget->getClass()==CLASS_DEATH_KNIGHT && !pTarget->HasAura(EXPOSE_ARMOR, EFFECT_INDEX_0) && ai->GetEnergyAmount()>=25 && ai->CastSpell(EXPOSE_ARMOR, *pTarget) )
                {
                }
                else if (RUPTURE>0 && pTarget->getClass()==CLASS_MAGE && ai->GetEnergyAmount()>=25 && ai->CastSpell(RUPTURE, *pTarget) )
                {
                }
                else if (RUPTURE>0 && pTarget->getClass()==CLASS_PRIEST && ai->GetEnergyAmount()>=25 && ai->CastSpell(RUPTURE, *pTarget) )
                {
                }
                else if ( EVISCERATE>0 && ai->GetEnergyAmount()>=35 && ai->CastSpell(EVISCERATE, *pTarget) )
                {
                }
                else
                {
                }
            }
            break;
    }
}

// end DoNextCombatManeuver

bool PlayerbotRogueAI::DoNonCombatActions()
{
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return false;

    PlayerbotAI* ai = GetAI();
    if (!ai)
        return false;

    Player* pMaster = ai->GetMaster();
    if (!pMaster)
        return false;

    // remove stealth
    if ( m_bot->HasAura( STEALTH ) )
        m_bot->RemoveSpellsCausingAura( SPELL_AURA_MOD_STEALTH );

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = ai->FindFood();
    Item* fItem = ai->FindBandage();

    if ( pItem
        && (ai->GetHealthPercent() < 30) )
    {
        ai->TellMaster("J'ai besoin de manger un peu...");
        ai->UseItem(*pItem);
        ai->SetIgnoreUpdateTime(30);
        return true;
    }
    else if ( !pItem
        && fItem
        && (!m_bot->HasAura(RECENTLY_BANDAGED, EFFECT_INDEX_0))
        && (ai->GetHealthPercent() < 70) )
    {
        ai->TellMaster("J'ai besoin de me faire un bandage...");
        ai->UseItem(*fItem);
        ai->SetIgnoreUpdateTime(8);
        return true;
    }

    return false;
}

void PlayerbotRogueAI::BuffPlayer(Player* target) {}
