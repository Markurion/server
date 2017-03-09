/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Stormwind_City
SD%Complete: 100
SDComment: Quest support: 1640, 1447, 4185, 11223 (DB support required for spell 42711)
SDCategory: Stormwind City
EndScriptData */

/* ContentData
npc_archmage_malin
npc_bartleby
npc_dashel_stonefist
npc_lady_katrana_prestor
EndContentData */

#include "scriptPCH.h"
#include <list>

/*######
## npc_archmage_malin
######*/

#define GOSSIP_ITEM_MALIN "Can you send me to Theramore? I have an urgent message for Lady Jaina from Highlord Bolvar."

bool GossipHello_npc_archmage_malin(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(11223) == QUEST_STATUS_COMPLETE && !pPlayer->GetQuestRewardStatus(11223))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MALIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_archmage_malin(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, 42711, true);
    }

    return true;
}

/*######
## npc_bartleby
######*/

enum
{
    FACTION_ENEMY       = 168,
    QUEST_BEAT          = 1640
};

struct npc_bartlebyAI : public ScriptedAI
{
    npc_bartlebyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiNormalFaction = pCreature->getFaction();
        Reset();
    }

    uint32 m_uiNormalFaction;

    void Reset()
    {
        if (m_creature->getFaction() != m_uiNormalFaction)
            m_creature->setFaction(m_uiNormalFaction);
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (m_creature->getVictim())
            return;

        if (m_creature->IsFriendlyTo(pAttacker))
            return;

        AttackStart(pAttacker);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() || ((m_creature->GetHealth() - uiDamage) * 100 / m_creature->GetMaxHealth() < 15))
        {
            uiDamage = 0;

            if (pDoneBy->GetTypeId() == TYPEID_PLAYER)
                ((Player*)pDoneBy)->AreaExploredOrEventHappens(QUEST_BEAT);

            EnterEvadeMode();
        }
    }
};

bool QuestAccept_npc_bartleby(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_BEAT)
    {
        pCreature->setFaction(FACTION_ENEMY);
        ((npc_bartlebyAI*)pCreature->AI())->AttackStart(pPlayer);
    }
    return true;
}

CreatureAI* GetAI_npc_bartleby(Creature* pCreature)
{
    return new npc_bartlebyAI(pCreature);
}

/*######
## npc_dashel_stonefist
######*/

enum
{
    QUEST_MISSING_DIPLO_PT8     = 1447,
    FACTION_HOSTILE             = 168
};

struct npc_dashel_stonefistAI : public ScriptedAI
{
    npc_dashel_stonefistAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiNormalFaction = pCreature->getFaction();
        Reset();
    }

    uint32 m_uiNormalFaction;

    void Reset()
    {
        if (m_creature->getFaction() != m_uiNormalFaction)
            m_creature->setFaction(m_uiNormalFaction);
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (m_creature->getVictim())
            return;

        if (m_creature->IsFriendlyTo(pAttacker))
            return;

        AttackStart(pAttacker);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() || ((m_creature->GetHealth() - uiDamage) * 100 / m_creature->GetMaxHealth() < 15))
        {
            uiDamage = 0;

            if (pDoneBy->GetTypeId() == TYPEID_PLAYER)
                ((Player*)pDoneBy)->AreaExploredOrEventHappens(QUEST_MISSING_DIPLO_PT8);

            EnterEvadeMode();
        }
    }
};

bool QuestAccept_npc_dashel_stonefist(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_MISSING_DIPLO_PT8)
    {
        pCreature->setFaction(FACTION_HOSTILE);
        ((npc_dashel_stonefistAI*)pCreature->AI())->AttackStart(pPlayer);
    }
    return true;
}

CreatureAI* GetAI_npc_dashel_stonefist(Creature* pCreature)
{
    return new npc_dashel_stonefistAI(pCreature);
}

/*######
## npc_lady_katrana_prestor
######*/

#define GOSSIP_ITEM_KAT_1 "Pardon the intrusion, Lady Prestor, but Highlord Bolvar suggested that I seek your advice."
#define GOSSIP_ITEM_KAT_2 "My apologies, Lady Prestor."
#define GOSSIP_ITEM_KAT_3 "Begging your pardon, Lady Prestor. That was not my intent."
#define GOSSIP_ITEM_KAT_4 "Thank you for your time, Lady Prestor."

bool GossipHello_npc_lady_katrana_prestor(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(4185) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(2693, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_lady_katrana_prestor(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(2694, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(2695, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(2696, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(4185);
            break;
    }
    return true;
}
/*######
## Quest #434 The Attack
######*/

enum QuestTheAttack
{
    QUEST_ITEMS_OF_SOME_CONSEQUENCE = 2746, // prequest for 434
    QUEST_THE_ATTACK                = 434,

    FACTION_ENEMYY                  = 14,
    FACTION_NORMAL_LESCOVAR         = 12,
    FACTION_NORMAL_MARZON           = 84,

    NPC_LORD_GREGOR_LESCOVAR        = 1754,
    NPC_MARZON_THE_SILENT_BLADE     = 1755,
    NPC_STORMWIND_ROYAL_GUARD       = 1756,
    NPC_PRIESTRESS                  = 7779,
    NPC_TYRION                      = 7766,
    NPC_TYRIONS_SPYBOT              = 8856,

    MODEL_TYRIANA                   = 6703,
    MODEL_SPYBOT                    = 1159,

    GOSSIP_TEXT_ID_EVENT_RUNNING    = 2394,

    SAY_SPYBOT                      = -1700000,
    SAY_TYRION_1                    = -1700001,
    SAY_TYRIONA_1                   = -1700002,
    SAY_ROYAL_GUARD_1               = -1700003,
    SAY_TYRIONA_2                   = -1700004,
    SAY_TYRIONA_3                   = -1700005,
    SAY_GREGOR_1                    = -1700006,
    SAY_TYRIONA_4                   = -1700007,
    SAY_GREGOR_2                    = -1700008,
    SAY_ROYAL_GUARD_2               = -1700009,
    SAY_GREGOR_3                    = -1700010,
    SAY_GREGOR_4                    = -1700011,
    SAY_MARZON_1                    = -1700012,
    SAY_GREGOR_5                    = -1700013,
    SAY_MARZON_2                    = -1700014,
    SAY_TYRION_2                    = -1700015,
    SAY_GREGOR_6                    = -1700016,
    SAY_MARZON_3                    = -1700017,

    SPELL_STEALTH                   = 8874, // for Marzon
};


/*######
## npc_tyrion
######*/
struct npc_tyrionAI : public ScriptedAI
{
    npc_tyrionAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_IsEventRunning;

    void GetAIInformation(ChatHandler& reader)
    {
        ScriptedAI::GetAIInformation(reader);
        reader.PSendSysMessage("TYRION: Event running: [%s]", m_IsEventRunning ? "YES" : "NO");
    }

    void Reset() override
    {
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_IsEventRunning = false;
    }

    bool AreCreaturesRequiredForQuestPresent(float fMaxSearchRange = 40.0f)
    {
        // m_guidTyrion Spybot
        if (!GetClosestCreatureWithEntry(m_creature, NPC_TYRIONS_SPYBOT, VISIBLE_RANGE))
            return false;
        return true;
    }

};

CreatureAI* GetAI_npc_tyrion(Creature* pCreature)
{
    return new npc_tyrionAI(pCreature);
}


/*######
## npc_lord_gregor_lescovar
######*/
struct npc_lord_gregor_lescovarAI : public npc_escortAI
{
    npc_lord_gregor_lescovarAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiEventTimer;
    uint8 m_uiEventPhase;
    ObjectGuid m_guidGuard1;
    ObjectGuid m_guidGuard2;
    ObjectGuid m_guidMarzon;
    ObjectGuid m_guidTyrion;
    ObjectGuid m_guidPriestress;

    void Reset()
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        m_uiEventTimer = 0;
        m_uiEventPhase = 0;
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_creature->getFaction() == FACTION_ENEMYY)
            m_creature->setFaction(FACTION_NORMAL_LESCOVAR);

        if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
            if (!pMarzon->isAlive())
            {
                if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
                {
                    pGuard1->ForcedDespawn(0);
                    pGuard1->Respawn();
                }
                if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
                {
                    pGuard2->ForcedDespawn(0);
                    pGuard2->Respawn();
                }
                if (Creature* pPriestress = m_creature->GetMap()->GetCreature(m_guidPriestress))
                    pPriestress->Respawn();

                if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
                {
                    pTyrion->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(pTyrion->AI()))
                        ptyrionAI->Reset();
                }
            }
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() != NPC_MARZON_THE_SILENT_BLADE)
            return;

        if (!m_creature->isAlive())
        {
            if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
            {
                pGuard1->ForcedDespawn(0);
                pGuard1->Respawn();
            }
            if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
            {
                pGuard2->ForcedDespawn(0);
                pGuard2->Respawn();
            }
            if (Creature* pPriestress = m_creature->GetMap()->GetCreature(m_guidPriestress))
                pPriestress->Respawn();

            if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
            {
                pTyrion->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(pTyrion->AI()))
                    ptyrionAI->m_IsEventRunning = false;
            }
        }
    }

    void Aggro(Unit* pAttacker)
    {
        if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
        {
            DoScriptText(SAY_MARZON_3, pMarzon);
            DoScriptText(SAY_GREGOR_6, m_creature);
            pMarzon->AI()->AttackStart(pAttacker);
        }
        m_uiEventPhase = 13;
    }

    void SummonedCreatureDespawn(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() != NPC_MARZON_THE_SILENT_BLADE)
            return;

        if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
        {
            pGuard1->ForcedDespawn(0);
            pGuard1->Respawn();
        }

        if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
        {
            pGuard2->ForcedDespawn(0);
            pGuard2->Respawn();
        }

        if (Creature* pPriestress = m_creature->GetMap()->GetCreature(m_guidPriestress))
            pPriestress->Respawn();

        if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
        {
            pTyrion->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(pTyrion->AI()))
                ptyrionAI->m_IsEventRunning = false;
        }

    }

    void WaypointReached(uint32 uiPoint)
    {
        Player* pPlayer = GetPlayerForEscort();
        if (!pPlayer)
            return;

        switch (uiPoint)
        {
        case 13:
            SetEscortPaused(false);
            if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
              if (pGuard1->isAlive())
                  pGuard1->SetFacingToObject(m_creature);
            if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
              if (pGuard2->isAlive())
                  pGuard2->SetFacingToObject(m_creature);
            DoScriptText(SAY_GREGOR_2, m_creature);
            m_uiEventPhase = 1;
            m_uiEventTimer = 3500;
            SetEscortPaused(true);
            break;
        case 17:
            m_uiEventPhase = 4;
            m_uiEventTimer = 1500;
            SetEscortPaused(true);
            break;
        case 20:
            m_creature->setFaction(FACTION_NORMAL_LESCOVAR);
            if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
            {
                pMarzon->CastSpell(pMarzon, SPELL_STEALTH, true);
                pMarzon->ForcedDespawn(0);
            }
            break;
        case 21:
            if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
            {
                pGuard1->ForcedDespawn(0);
                pGuard1->Respawn();
            }
            if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
            {
                pGuard2->ForcedDespawn(0);
                pGuard2->Respawn();
            }
            if (Creature* pPriestress = m_creature->GetMap()->GetCreature(m_guidPriestress))
                pPriestress->Respawn();

            if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
            {
                pTyrion->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(pTyrion->AI()))
                    ptyrionAI->m_IsEventRunning = false;
            }

            if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                if (!pMarzon->isInCombat())
                    pMarzon->ForcedDespawn(0);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                switch (m_uiEventPhase)
                {
                case 1:
                    if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
                        if (pGuard1->isAlive() && !pGuard1->getVictim())
                        {
                            DoScriptText(SAY_ROYAL_GUARD_2, pGuard1);
                            pGuard1->GetMotionMaster()->MovePoint(0, -8364.07f, 406.775f, 122.274f, MOVE_PATHFINDING);
                        }
                    if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
                        if (pGuard2->isAlive() && !pGuard2->getVictim())
                        {
                            DoScriptText(SAY_ROYAL_GUARD_2, pGuard2);
                            pGuard2->GetMotionMaster()->MovePoint(0, -8353.91f, 415.318f, 122.274f, MOVE_PATHFINDING);
                        }
                    ++m_uiEventPhase;
                    m_uiEventTimer = 3000;
                    break;
                case 2:
                    ++m_uiEventPhase;
                    SetEscortPaused(false);
                    break;
                case 4:
                    if (Creature* pMarzon = m_creature->SummonCreature(NPC_MARZON_THE_SILENT_BLADE, -8407.71f, 482.117f, 123.76f, 4.79f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 1 * MINUTE*IN_MILLISECONDS))
                    {
                        pMarzon->CastSpell(pMarzon, SPELL_STEALTH, true);
                        pMarzon->GetMotionMaster()->MovePoint(0, -8406.00f, 470.00f, 123.76f, MOVE_PATHFINDING);
                        m_guidMarzon = pMarzon->GetObjectGuid();
                    }
                    m_uiEventTimer = 10000;
                    ++m_uiEventPhase;
                    SetEscortPaused(true);
                    break;
                case 5:
                    DoScriptText(SAY_GREGOR_3, m_creature);
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 6:

                    if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                    {
                        pMarzon->RemoveAura(SPELL_STEALTH, EFFECT_INDEX_0);
                        m_creature->SetFacingToObject(pMarzon);
                    }
                    DoScriptText(SAY_GREGOR_4, m_creature);
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 7:
                    if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                        DoScriptText(SAY_MARZON_1, pMarzon);
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 8:
                    DoScriptText(SAY_GREGOR_5, m_creature);
                    m_uiEventTimer = 4000;
                    ++m_uiEventPhase;
                    break;
                case 9:
                    if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                        DoScriptText(SAY_MARZON_2, pMarzon);
                    m_uiEventTimer = 3000;
                    ++m_uiEventPhase;
                    break;
                case 10:
                    if (Player* pPlayer = GetPlayerForEscort())
                        pPlayer->GroupEventHappens(QUEST_THE_ATTACK, m_creature);
                    if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
                        DoScriptText(SAY_TYRION_2, pTyrion);
                    if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                        pMarzon->setFaction(FACTION_ENEMYY);
                    m_creature->setFaction(FACTION_ENEMYY);
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 11:
                    if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                    {
                        if (!m_creature->isInCombat())
                            pMarzon->GetMotionMaster()->MovePoint(0, -8419.00f, 486.26f, 123.75f, MOVE_PATHFINDING);
                        else if (m_creature->getVictim())
                            pMarzon->AI()->AttackStart(m_creature->getVictim());
                    }
                    ++m_uiEventPhase;
                    SetEscortPaused(false);
                    break;
                case 12:
                    if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                      if (pMarzon->isInCombat() && pMarzon->getVictim() && !m_creature->isInCombat())
                          m_creature->AI()->AttackStart(pMarzon->getVictim());
                    break;
                case 13:
                    if (Player* pPlayer = GetPlayerForEscort())
                        if (pPlayer->isDead())
                        {
                            SetEscortPaused(false);
                            m_creature->setFaction(FACTION_NORMAL_LESCOVAR);
                            if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
                                if (!pMarzon->isDead() && pMarzon->getFaction() == FACTION_ENEMYY)
                                    pMarzon->setFaction(FACTION_NORMAL_MARZON);
                        }
                    if (!m_creature->isInCombat())
                        SetEscortPaused(false);
                    break;
                }
            }
            else
                m_uiEventTimer -= uiDiff;
        }

        if (Creature* pMarzon = m_creature->GetMap()->GetCreature(m_guidMarzon))
          if (pMarzon && pMarzon->isDead() && pMarzon->getFaction() == FACTION_ENEMYY)
              pMarzon->setFaction(FACTION_NORMAL_MARZON);

        npc_escortAI::UpdateAI(uiDiff);

        if (m_creature->SelectHostileTarget() || m_creature->getVictim())
            DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_npc_lord_gregor_lescovar(Creature* pCreature)
{
    return new npc_lord_gregor_lescovarAI(pCreature);
}


/*######
## npc_tyrion_spybot
######*/

struct npc_tyrion_spybotAI : public npc_escortAI
{
    npc_tyrion_spybotAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    ObjectGuid m_guidGuard1;
    ObjectGuid m_guidGuard2;
    ObjectGuid m_guidLordGregor;
    ObjectGuid m_guidTyrion;
    ObjectGuid m_guidMarzon;
    ObjectGuid m_guidPriestress;
    uint8 m_uiGardenGuardsCounter;

    uint32 m_uiEventTimer;
    uint8 m_uiEventPhase;
    float m_fDefaultScaleSize;

    void Reset()
    {

        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        m_fDefaultScaleSize = 0.30f;
        m_uiEventTimer = 0;
        m_uiEventPhase = 0;
        m_uiGardenGuardsCounter = 0;
        m_creature->SetDisplayId(MODEL_SPYBOT);

        if (Creature* tyrion = GetClosestCreatureWithEntry(m_creature, NPC_TYRION, VISIBLE_RANGE))
        {
            if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(tyrion->AI()))
            {
                ptyrionAI->m_IsEventRunning = false;
                tyrion->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            }
        }
    }

    void Aggro(Unit* pAttacker) {}


    bool AreCreaturesRequiredForQuestPresent(float fMaxSearchRange = 40.0f)
    {
        if (m_guidTyrion && m_guidLordGregor && m_guidGuard1 && m_guidGuard2)
            return true;

        // m_guidPriestress
        if (!m_guidPriestress)
            if (Creature* pPriestress = GetClosestCreatureWithEntry(m_creature, NPC_PRIESTRESS, VISIBLE_RANGE))
            {
                if (!pPriestress)
                    return false;
                else if (pPriestress->GetEntry() != NPC_PRIESTRESS)
                    return false;
                else
                    m_guidPriestress = pPriestress->GetObjectGuid();
            }

        // m_guidTyrion
        if (!m_guidTyrion)
            if (Creature* pTyrion = GetClosestCreatureWithEntry(m_creature, NPC_TYRION, VISIBLE_RANGE))
            {
                if (!pTyrion)
                    return false;
                else if (pTyrion->GetEntry() != NPC_TYRION)
                    return false;
                else
                    m_guidTyrion = pTyrion->GetObjectGuid();
            }

        // Lord Gregor
        if (!m_guidLordGregor)
            if (Creature* pGregor = GetClosestCreatureWithEntry(m_creature, NPC_LORD_GREGOR_LESCOVAR, VISIBLE_RANGE))
            {
                if (!pGregor)
                    return false;
                else if (!pGregor->isAlive())
                    pGregor->Respawn();
                m_guidLordGregor = pGregor->GetObjectGuid();
            }
        // Stormwind Royal Guards (Garden)
        if (!m_guidGuard1 || !m_guidGuard2)
        {
            std::list<Creature*> lGardenStormwindRoyalGuards;
            GetCreatureListWithEntryInGrid(lGardenStormwindRoyalGuards, m_creature, NPC_STORMWIND_ROYAL_GUARD, fMaxSearchRange);

            if (lGardenStormwindRoyalGuards.empty())
                return true;

            for (std::list<Creature*>::iterator iter = lGardenStormwindRoyalGuards.begin(); iter != lGardenStormwindRoyalGuards.end(); ++iter)
            {
                Creature* GardenStormwindRoyalGuard = (*iter);
                if (GardenStormwindRoyalGuard->isDead() || !GardenStormwindRoyalGuard->isAlive())
                    GardenStormwindRoyalGuard->Respawn();

                if (m_uiGardenGuardsCounter == 0)
                {
                    m_guidGuard1 = GardenStormwindRoyalGuard->GetObjectGuid();
                    m_uiGardenGuardsCounter++;
                    continue;
                }
                else if (m_uiGardenGuardsCounter == 1)
                {
                    m_guidGuard2 = GardenStormwindRoyalGuard->GetObjectGuid();
                    m_uiGardenGuardsCounter++;
                    continue;
                }
            }
            return true;
        }
        return true;
    }

    void WaypointReached(uint32 uiPoint)
    {
        if (!AreCreaturesRequiredForQuestPresent())
            return;

        switch (uiPoint)
        {
        case 1:
            if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
            {
                pTyrion->SetFacingToObject(m_creature);
                pTyrion->HandleEmote(EMOTE_STATE_USESTANDING);
            }
            m_fDefaultScaleSize = m_creature->GetFloatValue(OBJECT_FIELD_SCALE_X);
            m_creature->SetDisplayId(MODEL_TYRIANA);
            m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.00f);

            SetEscortPaused(true);
            m_uiEventTimer = 5000;
            m_uiEventPhase = 1;
            break;
        case 2:
            if (Creature* pPriestress = m_creature->GetMap()->GetCreature(m_guidPriestress))
                pPriestress->ForcedDespawn(0);
            if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
                DoScriptText(SAY_TYRION_1, pTyrion, GetPlayerForEscort());
            break;
        case 6:
            DoScriptText(SAY_TYRIONA_1, m_creature);
            if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
            {
                if (pGuard1 && pGuard1->isAlive() && !pGuard1->getVictim())
                    pGuard1->SetFacingToObject(m_creature);
            }

            if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
            {
                if (pGuard2 && pGuard2->isAlive() && !pGuard2->getVictim())
                    pGuard2->SetFacingToObject(m_creature);
            }

            m_uiEventPhase = 2;
            m_uiEventTimer = 5000;
            SetEscortPaused(true);
            break;
        case 18:
            DoScriptText(SAY_TYRIONA_3, m_creature);
            if (Creature* gregor = m_creature->GetMap()->GetCreature(m_guidLordGregor))
                gregor->SetFacingToObject(m_creature);
            m_uiEventPhase = 5;
            m_uiEventTimer = 5000;
            SetEscortPaused(true);
            break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {

        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                switch (m_uiEventPhase)
                {
                case 1:
                    if (Creature* pTyrion = m_creature->GetMap()->GetCreature(m_guidTyrion))
                        pTyrion->HandleEmote(EMOTE_ONESHOT_NONE);
                    SetEscortPaused(false);
                    break;
                case 2:
                    if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
                    {
                        DoScriptText(SAY_ROYAL_GUARD_1, pGuard1);
                        m_creature->SetFacingToObject(pGuard1);
                    }
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 3:
                    DoScriptText(SAY_TYRIONA_2, m_creature);
                    if (Creature* pGuard1 = m_creature->GetMap()->GetCreature(m_guidGuard1))
                        pGuard1->HandleEmote(EMOTE_ONESHOT_KNEEL);
                    if (Creature* pGuard2 = m_creature->GetMap()->GetCreature(m_guidGuard2))
                        pGuard2->HandleEmote(EMOTE_ONESHOT_KNEEL);
                    ++m_uiEventPhase; // 4 = nothing (It's OK)
                    SetEscortPaused(false);
                    break;
                case 5:
                    if (Creature* gregor = m_creature->GetMap()->GetCreature(m_guidLordGregor))
                        DoScriptText(SAY_GREGOR_1, gregor);
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 6:
                    DoScriptText(SAY_TYRIONA_4, m_creature);
                    m_uiEventTimer = 5000;
                    ++m_uiEventPhase;
                    break;
                case 7:
                    if (Player* pPlayer = GetPlayerForEscort())
                        if (Creature* gregor = m_creature->GetMap()->GetCreature(m_guidLordGregor))
                            if (npc_lord_gregor_lescovarAI* pGregorEscortAI = dynamic_cast<npc_lord_gregor_lescovarAI*>(gregor->AI()))
                            {
                                pGregorEscortAI->SetMaxPlayerDistance(200.0f);
                                pGregorEscortAI->Start(false, pPlayer->GetGUID());
                                pGregorEscortAI->m_guidGuard1 = m_guidGuard1;
                                pGregorEscortAI->m_guidGuard2 = m_guidGuard2;
                                pGregorEscortAI->m_guidTyrion = m_guidTyrion;
                                pGregorEscortAI->m_guidPriestress = m_guidPriestress;
                            }
                    m_creature->SetDisplayId(MODEL_SPYBOT);
                    m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, m_fDefaultScaleSize);
                    ++m_uiEventPhase;
                    m_uiEventTimer = 1000;
                    break;
                case 8:
                    SetEscortPaused(false);
                    break;
                }
            }
            else m_uiEventTimer -= uiDiff;
        }

        npc_escortAI::UpdateAI(uiDiff);
    }
};

CreatureAI* GetAI_npc_tyrion_spybot(Creature* pCreature)
{
    return new npc_tyrion_spybotAI(pCreature);
}

/*######
## npc_tyrion
######*/

bool GossipHello_npc_tyrion(Player* pPlayer, Creature* pCreature)
{
    bool bIsEventReady = false;

    if (npc_tyrionAI* pTyrion = dynamic_cast<npc_tyrionAI*>(pCreature->AI()))
        bIsEventReady =  (!pTyrion->m_IsEventRunning && pTyrion->AreCreaturesRequiredForQuestPresent());

    // Check if event is possible and also check the status of the quests
    if (!bIsEventReady)
    {
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_EVENT_RUNNING, pCreature->GetObjectGuid());
    }
    else
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

        pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetObjectGuid());
    }

    return true;
}


bool QuestAccept_npc_tyrion(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(pCreature->AI()))
        if (ptyrionAI->m_IsEventRunning)
            return false;

    if (pQuest->GetQuestId() == QUEST_THE_ATTACK)
    {
        if (Creature* TyrionSpyBot = GetClosestCreatureWithEntry(pCreature, NPC_TYRIONS_SPYBOT, VISIBLE_RANGE))
        {
            if (npc_tyrion_spybotAI* pSpybotEscortAI = dynamic_cast<npc_tyrion_spybotAI*>(TyrionSpyBot->AI()))
            {
                DoScriptText(SAY_SPYBOT, TyrionSpyBot);
                pSpybotEscortAI->SetMaxPlayerDistance(200.0f);
                pSpybotEscortAI->Start(false, pPlayer->GetGUID(), nullptr, false, false);
                pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                if (npc_tyrionAI* ptyrionAI = dynamic_cast<npc_tyrionAI*>(pCreature->AI()))
                    ptyrionAI->m_IsEventRunning = true;
            }
        }
    }
    return true;
}

/*######
## npc_bolvar_fordragon
######*/

enum
{
    SPELL_HIGHLORDS_JUSTICE = 20683,
    SPELL_CLEAVE            = 20684,
    SPELL_SHIELD_WALL       = 29061,
    SPELL_LAY_ON_HANDS      = 17233,
};

struct npc_bolvar_fordragonAI : ScriptedAI
{
    explicit npc_bolvar_fordragonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        npc_bolvar_fordragonAI::Reset();
    }

    uint32 m_uiCleaveTimer;
    uint32 m_uiHighlordsJusticeTimer;
    uint32 m_uiShieldwallTimer;
    bool m_bHasUsedLOH;

    void Reset() override
    {
        m_bHasUsedLOH = false;
        m_uiCleaveTimer = 5000;
        m_uiHighlordsJusticeTimer = 10000;
        m_uiShieldwallTimer = 18000;
    }

    void DamageTaken(Unit* /*pDealer*/, uint32& /*uiDamage*/) override
    {
        if (!m_bHasUsedLOH && m_creature->GetHealthPercent() < 20.0f)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_LAY_ON_HANDS) == CAST_OK)
                m_bHasUsedLOH = true;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    { 
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCleaveTimer < uiDiff)
        {
           if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
			   m_uiCleaveTimer = urand(5000, 8000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        if (m_uiHighlordsJusticeTimer < uiDiff)
        {
            if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 5.0f))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_HIGHLORDS_JUSTICE) == CAST_OK)
                    m_uiHighlordsJusticeTimer = urand(20000, 22000);
            }
        }
        else
            m_uiHighlordsJusticeTimer -= uiDiff;

        if (m_uiShieldwallTimer < uiDiff)
        {
            if (m_creature->GetHealthPercent() < 90.0f)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_SHIELD_WALL) == CAST_OK)
                    m_uiShieldwallTimer = urand (40000, 60000);
            }
            else
                m_uiShieldwallTimer += 5000;
        }
        else m_uiShieldwallTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_bolvar_fordragon(Creature* pCreature)
{
    return new npc_bolvar_fordragonAI(pCreature);
}

bool GossipHello_npc_bolvar_fordragon(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    pPlayer->SEND_GOSSIP_MENU(865, pCreature->GetGUID());

    return true;
}

struct SpawnLocation
{
    float m_fX, m_fY, m_fZ;
};

// NOTE: Stormwind Keep is explicitly not supposed to receive the buff
static const SpawnLocation aRallyGeneratorLocs[7] =
{
    { -8800.6f, 643.17f, 94.29f },   // Trade District
    { -8643.5f, 758.10f, 97.28f },   // Cathedral Square
    { -8523.0f, 848.78f, 106.8f },   // Cathedral of Light
    { -8951.7f, 865.88f, 104.9f },   // Mage District
    { -8424.0f, 619.94f, 95.55f },   // Blacksmith District
    { -8705.6f, 418.51f, 99.35f },   // Old Town
    { -8734.6f, 1043.2f, 92.15f }    // The Park
};

enum
{
    SPELL_RALLYING_CRY_DRAGONSLAYER = 22888,
    NPC_RALLY_CRY_GENERATOR_ALLY    = 21002,

    MAX_RALLY_GENERATORS            = 7
};

/*######
## npc_major_mattingly
######*/

enum
{
    QUEST_CELEBRATING_GOOD_TIMES        = 7496,

    YELL_ONY_ALLY_REWARD_1              = -143941,
    YELL_ONY_ALLY_REWARD_2              = -143942,

    GO_ONYXIAS_HEAD_ALLY                = 179558,

};

struct npc_major_mattinglyAI : public ScriptedAI
{
    npc_major_mattinglyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
    uint32 m_uiTick;
    uint32 m_uiDialogueTimer;
    bool m_bRallyingCryEvent;
    ObjectGuid m_playerGuid;

    void Reset()
    {
        m_uiTick = 0;
        m_uiDialogueTimer = 2000;
        m_bRallyingCryEvent = false;
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void StartRallyEvent(ObjectGuid playerGuid)
    {
        m_playerGuid = playerGuid;
        m_bRallyingCryEvent = true;
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_bRallyingCryEvent)
        {
            if (m_uiDialogueTimer <= uiDiff)
            {
                switch (m_uiTick)
                {
                    case 0:
                        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                        {
                            m_creature->HandleEmote(EMOTE_ONESHOT_SHOUT);
                            m_creature->MonsterYellToZone(YELL_ONY_ALLY_REWARD_1, 0, pPlayer);
                        }
                        m_uiDialogueTimer = 10000;
                        break;
                    case 1:
                        m_creature->HandleEmote(EMOTE_ONESHOT_SHOUT);
                        m_creature->MonsterYellToZone(YELL_ONY_ALLY_REWARD_2);
                        if (GameObject* pGo = m_creature->FindNearestGameObject(GO_ONYXIAS_HEAD_ALLY, 150.0f))
                        {
                            if (!pGo->isSpawned())
                            {
                                pGo->SetRespawnTime(7200);
                                pGo->Refresh();
                            }
                        }
                        m_uiDialogueTimer = 7000;
                        break;
                    case 2:
                        m_creature->CastSpell(m_creature, SPELL_RALLYING_CRY_DRAGONSLAYER, true);
                        for (uint8 i = 0; i < MAX_RALLY_GENERATORS; ++i)
                        {
                            if (Creature* pGenerator = m_creature->SummonCreature(NPC_RALLY_CRY_GENERATOR_ALLY, aRallyGeneratorLocs[i].m_fX, aRallyGeneratorLocs[i].m_fY, aRallyGeneratorLocs[i].m_fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 1000))
                                pGenerator->CastSpell(pGenerator, SPELL_RALLYING_CRY_DRAGONSLAYER, true);
                        }
                        Reset();
                        return;
                }
                m_uiTick++;
            }
            else m_uiDialogueTimer -= uiDiff;
        } 

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_major_mattingly(Creature* pCreature)
{
    return new npc_major_mattinglyAI(pCreature);
}

bool QuestRewarded_npc_major_mattingly(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_CELEBRATING_GOOD_TIMES)
    {
        if (npc_major_mattinglyAI* pMattingly = dynamic_cast<npc_major_mattinglyAI*>(pCreature->AI()))
            pMattingly->StartRallyEvent(pPlayer->GetObjectGuid());
    }
    return true;
}

/*######
## npc_field_marshal_afrasiabi
######*/

enum
{
    QUEST_LORD_OF_BLACKROCK_ALLY          = 7782,

    YELL_NEF_REWARD_1_MALE_ALLY          = -147211,
    YELL_NEF_REWARD_2_MALE_ALLY          = -147212,
    YELL_NEF_REWARD_1_FEMALE_ALLY        = -147213,
    YELL_NEF_REWARD_2_FEMALE_ALLY        = -147214,

    GO_NEFARIANS_HEAD_ALLY               = 179882,
};

struct npc_field_marshal_afrasiabiAI : public ScriptedAI
{
    npc_field_marshal_afrasiabiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiTick;
    uint32 m_uiDialogueTimer;
    bool m_bRallyingCryEvent;
    ObjectGuid m_playerGuid;

    void Reset()
    {
        m_uiTick = 0;
        m_uiDialogueTimer = 2000;
        m_bRallyingCryEvent = false;
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void StartRallyEvent(ObjectGuid playerGuid)
    {
        m_playerGuid = playerGuid;
        m_bRallyingCryEvent = true;
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_bRallyingCryEvent)
        {
            if (m_uiDialogueTimer <= uiDiff)
            {
                switch (m_uiTick)
                {
                    case 0:
                        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                        {
                            m_creature->HandleEmote(EMOTE_ONESHOT_SHOUT);
                            m_creature->MonsterYellToZone(pPlayer->getGender() ? YELL_NEF_REWARD_1_FEMALE_ALLY : YELL_NEF_REWARD_1_MALE_ALLY, 0, pPlayer);
                        }
                        m_uiDialogueTimer = 10000;
                        break;
                    case 1:
                        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                        {
                            m_creature->HandleEmote(EMOTE_ONESHOT_SHOUT);
                            m_creature->MonsterYellToZone(pPlayer->getGender() ? YELL_NEF_REWARD_2_FEMALE_ALLY : YELL_NEF_REWARD_2_MALE_ALLY);
                        }
                        if (GameObject* pGo = m_creature->FindNearestGameObject(GO_NEFARIANS_HEAD_ALLY, 150.0f))
                        {
                            if (!pGo->isSpawned())
                            {
                                pGo->SetRespawnTime(7200);
                                pGo->Refresh();
                            }
                        }
                        m_uiDialogueTimer = 7000;
                        break;
                    case 2:
                        m_creature->CastSpell(m_creature, SPELL_RALLYING_CRY_DRAGONSLAYER, true);
                        for (uint8 i = 0; i < MAX_RALLY_GENERATORS; ++i)
                        {
                            if (Creature* pGenerator = m_creature->SummonCreature(NPC_RALLY_CRY_GENERATOR_ALLY, aRallyGeneratorLocs[i].m_fX, aRallyGeneratorLocs[i].m_fY, aRallyGeneratorLocs[i].m_fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 1000))
                                pGenerator->CastSpell(pGenerator, SPELL_RALLYING_CRY_DRAGONSLAYER, true);
                        }
                        Reset();
                        return;
                }
                m_uiTick++;
            }
            else m_uiDialogueTimer -= uiDiff;
        } 

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_field_marshal_afrasiabi(Creature* pCreature)
{
    return new npc_field_marshal_afrasiabiAI(pCreature);
}

bool QuestRewarded_npc_field_marshal_afrasiabi(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_LORD_OF_BLACKROCK_ALLY)
    {
        if (npc_field_marshal_afrasiabiAI* pAfrasiabi = dynamic_cast<npc_field_marshal_afrasiabiAI*>(pCreature->AI()))
            pAfrasiabi->StartRallyEvent(pPlayer->GetObjectGuid());
    }
    return true;
}

void AddSC_stormwind_city()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_archmage_malin";
    newscript->pGossipHello = &GossipHello_npc_archmage_malin;
    newscript->pGossipSelect = &GossipSelect_npc_archmage_malin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_bartleby";
    newscript->GetAI = &GetAI_npc_bartleby;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_bartleby;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dashel_stonefist";
    newscript->GetAI = &GetAI_npc_dashel_stonefist;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_dashel_stonefist;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lady_katrana_prestor";
    newscript->pGossipHello = &GossipHello_npc_lady_katrana_prestor;
    newscript->pGossipSelect = &GossipSelect_npc_lady_katrana_prestor;
    newscript->RegisterSelf();

    // The Attack quest
    newscript = new Script;
    newscript->Name = "npc_lord_gregor_lescovar";
    newscript->GetAI = &GetAI_npc_lord_gregor_lescovar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tyrion_spybot";
    newscript->GetAI = &GetAI_npc_tyrion_spybot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tyrion";
    newscript->GetAI = &GetAI_npc_tyrion;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_tyrion;
    newscript->pGossipHello = &GossipHello_npc_tyrion;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_bolvar_fordragon";
    newscript->GetAI = &GetAI_npc_bolvar_fordragon;
    newscript->pGossipHello   = &GossipHello_npc_bolvar_fordragon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_major_mattingly";
    newscript->GetAI = &GetAI_npc_major_mattingly;
    newscript->pQuestRewardedNPC = &QuestRewarded_npc_major_mattingly;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_field_marshal_afrasiabi";
    newscript->GetAI = &GetAI_npc_field_marshal_afrasiabi;
    newscript->pQuestRewardedNPC = &QuestRewarded_npc_field_marshal_afrasiabi;
    newscript->RegisterSelf();
}