/*
* Copyright (C) 2010 - 2014 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#include "LuaEngine.h"
#include "HookMgr.h"
#include "WorldSocket.h"

void HookMgr::OnWorldUpdate(uint32 diff)
{
    sEluna.m_EventMgr.Update(diff);
    if (!sEluna.ServerEventBindings.BeginCall(WORLD_EVENT_ON_UPDATE))
        return;
    sEluna.Push(sEluna.L, diff);
    sEluna.ServerEventBindings.ExecuteCall();
    sEluna.ServerEventBindings.EndCall();
}

void HookMgr::OnLootItem(Player* pPlayer, Item* pItem, uint32 count, uint64 guid)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_LOOT_ITEM))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pItem);
    sEluna.Push(sEluna.L, count);
    sEluna.Push(sEluna.L, guid);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnLootMoney(Player* pPlayer, uint32 amount)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_LOOT_MONEY))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, amount);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnFirstLogin(Player* pPlayer)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_FIRST_LOGIN))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnRepop(Player* pPlayer)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_REPOP))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnResurrect(Player* pPlayer)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_RESURRECT))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnEquip(Player* pPlayer, Item* pItem, uint8 bag, uint8 slot)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_EQUIP))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pItem);
    sEluna.Push(sEluna.L, bag);
    sEluna.Push(sEluna.L, slot);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

InventoryResult HookMgr::OnCanUseItem(const Player* pPlayer, uint32 itemEntry)
{
    InventoryResult result = EQUIP_ERR_OK;
    if (sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_CAN_USE_ITEM))
    {
        sEluna.Push(sEluna.L, pPlayer);
        sEluna.Push(sEluna.L, itemEntry);
        sEluna.PlayerEventBindings.ExecuteCall();
        for (int i = 1; i <= lua_gettop(sEluna.L); ++i)
        {
            if (lua_isnoneornil(sEluna.L, i))
                continue;
            uint32 res = lua_tounsigned(sEluna.L, i);
            if (res != EQUIP_ERR_OK)
                result = (InventoryResult)res;
        }
        sEluna.PlayerEventBindings.EndCall();
    }
    return result;
}

void HookMgr::HandleGossipSelectOption(Player* pPlayer, Item* item, uint32 sender, uint32 action, std::string code)
{
    int bind = sEluna.ItemGossipBindings.GetBind(item->GetEntry(), GOSSIP_EVENT_ON_SELECT);
    if (bind)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, GOSSIP_EVENT_ON_SELECT);
        sEluna.Push(sEluna.L, pPlayer);
        sEluna.Push(sEluna.L, item);
        sEluna.Push(sEluna.L, sender);
        sEluna.Push(sEluna.L, action);
        if (code.empty())
            lua_pushnil(sEluna.L);
        else
            sEluna.Push(sEluna.L, code);
        sEluna.ExecuteCall(6, 0);
    }
}

void HookMgr::HandleGossipSelectOption(Player* pPlayer, uint32 menuId, uint32 sender, uint32 action, std::string code)
{
    int bind = sEluna.playerGossipBindings.GetBind(menuId, GOSSIP_EVENT_ON_SELECT);
    if (bind)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, GOSSIP_EVENT_ON_SELECT);
        sEluna.Push(sEluna.L, pPlayer); // receiver
        sEluna.Push(sEluna.L, pPlayer); // sender, just not to mess up the amount of args.
        sEluna.Push(sEluna.L, sender);
        sEluna.Push(sEluna.L, action);
        if (code.empty())
            lua_pushnil(sEluna.L);
        else
            sEluna.Push(sEluna.L, code);
        sEluna.Push(sEluna.L, menuId);
        sEluna.ExecuteCall(7, 0);
    }
}

void HookMgr::OnEngineRestart()
{
    if (!sEluna.ServerEventBindings.BeginCall(ELUNA_EVENT_ON_RESTART))
        return;
    sEluna.ServerEventBindings.ExecuteCall();
    sEluna.ServerEventBindings.EndCall();
}

// item
bool HookMgr::OnDummyEffect(Unit* pCaster, uint32 spellId, SpellEffIndex effIndex, Item* pTarget)
{
    int bind = sEluna.ItemEventBindings.GetBind(pTarget->GetEntry(), ITEM_EVENT_ON_DUMMY_EFFECT);
    if (!bind)
        return false;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, ITEM_EVENT_ON_DUMMY_EFFECT);
    sEluna.Push(sEluna.L, pCaster);
    sEluna.Push(sEluna.L, spellId);
    sEluna.Push(sEluna.L, effIndex);
    sEluna.Push(sEluna.L, pTarget);
    sEluna.ExecuteCall(5, 0);
    return true;
}

bool HookMgr::OnQuestAccept(Player* pPlayer, Item* pItem, Quest const* pQuest)
{
    int bind = sEluna.ItemEventBindings.GetBind(pItem->GetEntry(), ITEM_EVENT_ON_QUEST_ACCEPT);
    if (!bind)
        return false;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, ITEM_EVENT_ON_QUEST_ACCEPT);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pItem);
    sEluna.Push(sEluna.L, pQuest);
    sEluna.ExecuteCall(4, 0);
    return true;
}

bool HookMgr::OnUse(Player* pPlayer, Item* pItem, SpellCastTargets const& targets)
{
    int bind1 = sEluna.ItemGossipBindings.GetBind(pItem->GetEntry(), GOSSIP_EVENT_ON_HELLO);
    int bind2 = sEluna.ItemEventBindings.GetBind(pItem->GetEntry(), ITEM_EVENT_ON_USE);
    if (!bind1 && !bind2)
        return false;
    if (bind1)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        sEluna.BeginCall(bind1);
        sEluna.Push(sEluna.L, GOSSIP_EVENT_ON_HELLO);
        sEluna.Push(sEluna.L, pPlayer);
        sEluna.Push(sEluna.L, pItem);
        sEluna.ExecuteCall(3, 0);
    }
    if (bind2)
    {
        sEluna.BeginCall(bind2);
        sEluna.Push(sEluna.L, ITEM_EVENT_ON_USE);
        sEluna.Push(sEluna.L, pPlayer);
        sEluna.Push(sEluna.L, pItem);
        if (GameObject* target = targets.GetGOTarget())
            sEluna.Push(sEluna.L, target);
        else if (Item* target = targets.GetItemTarget())
            sEluna.Push(sEluna.L, target);
        else if (Unit* target = targets.GetUnitTarget())
            sEluna.Push(sEluna.L, target);
        else
            sEluna.Push(sEluna.L);
        sEluna.ExecuteCall(4, 0);
    }
    // pPlayer->SendEquipError((InventoryResult)83, pItem, NULL);
    return false;
}

bool HookMgr::OnExpire(Player* pPlayer, ItemTemplate const* pProto)
{
    int bind = sEluna.ItemEventBindings.GetBind(pProto->ItemId, ITEM_EVENT_ON_EXPIRE);
    if (!bind)
        return false;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, ITEM_EVENT_ON_EXPIRE);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pProto->ItemId);
    sEluna.ExecuteCall(3, 0);
    return true;
}
// creature
bool HookMgr::OnDummyEffect(Unit* pCaster, uint32 spellId, SpellEffIndex effIndex, Creature* pTarget)
{
    int bind = sEluna.CreatureEventBindings.GetBind(pTarget->GetEntry(), CREATURE_EVENT_ON_DUMMY_EFFECT);
    if (!bind)
        return false;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, CREATURE_EVENT_ON_DUMMY_EFFECT);
    sEluna.Push(sEluna.L, pCaster);
    sEluna.Push(sEluna.L, spellId);
    sEluna.Push(sEluna.L, effIndex);
    sEluna.Push(sEluna.L, pTarget);
    sEluna.ExecuteCall(5, 0);
    return true;
}

bool HookMgr::OnGossipHello(Player* pPlayer, Creature* pCreature)
{
    int bind = sEluna.CreatureGossipBindings.GetBind(pCreature->GetEntry(), GOSSIP_EVENT_ON_HELLO);
    if (!bind)
        return false;
    pPlayer->PlayerTalkClass->ClearMenus();
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GOSSIP_EVENT_ON_HELLO);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pCreature);
    sEluna.ExecuteCall(3, 0);
    return true;
}

bool HookMgr::OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    int bind = sEluna.CreatureGossipBindings.GetBind(pCreature->GetEntry(), GOSSIP_EVENT_ON_SELECT);
    if (!bind)
        return false;
    pPlayer->PlayerTalkClass->ClearMenus();
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GOSSIP_EVENT_ON_SELECT);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pCreature);
    sEluna.Push(sEluna.L, sender);
    sEluna.Push(sEluna.L, action);
    sEluna.ExecuteCall(5, 0);
    return true;
}

bool HookMgr::OnGossipSelectCode(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action, const char* code)
{
    int bind = sEluna.CreatureGossipBindings.GetBind(pCreature->GetEntry(), GOSSIP_EVENT_ON_SELECT);
    if (!bind)
        return false;
    pPlayer->PlayerTalkClass->ClearMenus();
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GOSSIP_EVENT_ON_SELECT);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pCreature);
    sEluna.Push(sEluna.L, sender);
    sEluna.Push(sEluna.L, action);
    sEluna.Push(sEluna.L, code);
    sEluna.ExecuteCall(6, 0);
    return true;
}

bool HookMgr::OnQuestAccept(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    int bind = sEluna.CreatureEventBindings.GetBind(pCreature->GetEntry(), CREATURE_EVENT_ON_QUEST_ACCEPT);
    if (!bind)
        return false;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, CREATURE_EVENT_ON_QUEST_ACCEPT);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pCreature);
    sEluna.Push(sEluna.L, pQuest);
    sEluna.ExecuteCall(4, 0);
    return true;
}

bool HookMgr::OnQuestSelect(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    int bind = sEluna.CreatureEventBindings.GetBind(pCreature->GetEntry(), CREATURE_EVENT_ON_QUEST_SELECT);
    if (!bind)
        return false;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, CREATURE_EVENT_ON_QUEST_SELECT);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pCreature);
    sEluna.Push(sEluna.L, pQuest);
    sEluna.ExecuteCall(4, 0);
    return true;
}

bool HookMgr::OnQuestComplete(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    int bind = sEluna.CreatureEventBindings.GetBind(pCreature->GetEntry(), CREATURE_EVENT_ON_QUEST_COMPLETE);
    if (!bind)
        return false;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, CREATURE_EVENT_ON_QUEST_COMPLETE);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pCreature);
    sEluna.Push(sEluna.L, pQuest);
    sEluna.ExecuteCall(4, 0);
    return true;
}

bool HookMgr::OnQuestReward(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    int bind = sEluna.CreatureEventBindings.GetBind(pCreature->GetEntry(), CREATURE_EVENT_ON_QUEST_REWARD);
    if (!bind)
        return false;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, CREATURE_EVENT_ON_QUEST_REWARD);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pCreature);
    sEluna.Push(sEluna.L, pQuest);
    sEluna.ExecuteCall(4, 0);
    return true;
}

uint32 HookMgr::GetDialogStatus(Player* pPlayer, Creature* pCreature)
{
    int bind = sEluna.CreatureEventBindings.GetBind(pCreature->GetEntry(), CREATURE_EVENT_ON_DIALOG_STATUS);
    if (!bind)
        return 0;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, CREATURE_EVENT_ON_DIALOG_STATUS);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pCreature);
    sEluna.ExecuteCall(3, 0);
    return DIALOG_STATUS_SCRIPTED_NO_STATUS;
}
// gameobject
bool HookMgr::OnDummyEffect(Unit* pCaster, uint32 spellId, SpellEffIndex effIndex, GameObject* pTarget)
{
    int bind = sEluna.GameObjectEventBindings.GetBind(pTarget->GetEntry(), GAMEOBJECT_EVENT_ON_DUMMY_EFFECT);
    if (!bind)
        return false;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GAMEOBJECT_EVENT_ON_DUMMY_EFFECT);
    sEluna.Push(sEluna.L, pCaster);
    sEluna.Push(sEluna.L, spellId);
    sEluna.Push(sEluna.L, effIndex);
    sEluna.Push(sEluna.L, pTarget);
    sEluna.ExecuteCall(5, 0);
    return true;
}

bool HookMgr::OnGossipHello(Player* pPlayer, GameObject* pGameObject)
{
    int bind = sEluna.GameObjectGossipBindings.GetBind(pGameObject->GetEntry(), GOSSIP_EVENT_ON_HELLO);
    if (!bind)
        return false;
    pPlayer->PlayerTalkClass->ClearMenus();
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GOSSIP_EVENT_ON_HELLO);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pGameObject);
    sEluna.ExecuteCall(3, 0);
    return true;
}

bool HookMgr::OnGossipSelect(Player* pPlayer, GameObject* pGameObject, uint32 sender, uint32 action)
{
    int bind = sEluna.GameObjectGossipBindings.GetBind(pGameObject->GetEntry(), GOSSIP_EVENT_ON_SELECT);
    if (!bind)
        return false;
    pPlayer->PlayerTalkClass->ClearMenus();
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GOSSIP_EVENT_ON_SELECT);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pGameObject);
    sEluna.Push(sEluna.L, sender);
    sEluna.Push(sEluna.L, action);
    sEluna.ExecuteCall(5, 0);
    return true;
}

bool HookMgr::OnGossipSelectCode(Player* pPlayer, GameObject* pGameObject, uint32 sender, uint32 action, const char* code)
{
    int bind = sEluna.GameObjectGossipBindings.GetBind(pGameObject->GetEntry(), GOSSIP_EVENT_ON_SELECT);
    if (!bind)
        return false;
    pPlayer->PlayerTalkClass->ClearMenus();
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GOSSIP_EVENT_ON_SELECT);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pGameObject);
    sEluna.Push(sEluna.L, sender);
    sEluna.Push(sEluna.L, action);
    sEluna.Push(sEluna.L, code);
    sEluna.ExecuteCall(6, 0);
    return true;
}

bool HookMgr::OnQuestAccept(Player* pPlayer, GameObject* pGameObject, Quest const* pQuest)
{
    int bind = sEluna.GameObjectEventBindings.GetBind(pGameObject->GetEntry(), GAMEOBJECT_EVENT_ON_QUEST_ACCEPT);
    if (!bind)
        return false;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GAMEOBJECT_EVENT_ON_QUEST_ACCEPT);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pGameObject);
    sEluna.Push(sEluna.L, pQuest);
    sEluna.ExecuteCall(4, 0);
    return true;
}

bool HookMgr::OnQuestComplete(Player* pPlayer, GameObject* pGameObject, Quest const* pQuest)
{
    int bind = sEluna.CreatureEventBindings.GetBind(pGameObject->GetEntry(), GAMEOBJECT_EVENT_ON_QUEST_COMPLETE);
    if (!bind)
        return false;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GAMEOBJECT_EVENT_ON_QUEST_COMPLETE);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pGameObject);
    sEluna.Push(sEluna.L, pQuest);
    sEluna.ExecuteCall(4, 0);
    return true;
}

bool HookMgr::OnQuestReward(Player* pPlayer, GameObject* pGameObject, Quest const* pQuest)
{
    int bind = sEluna.GameObjectEventBindings.GetBind(pGameObject->GetEntry(), GAMEOBJECT_EVENT_ON_QUEST_REWARD);
    if (!bind)
        return false;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GAMEOBJECT_EVENT_ON_QUEST_REWARD);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pGameObject);
    sEluna.Push(sEluna.L, pQuest);
    sEluna.ExecuteCall(4, 0);
    return true;
}

uint32 HookMgr::GetDialogStatus(Player* pPlayer, GameObject* pGameObject)
{
    int bind = sEluna.GameObjectEventBindings.GetBind(pGameObject->GetEntry(), GAMEOBJECT_EVENT_ON_DIALOG_STATUS);
    if (!bind)
        return 0;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GAMEOBJECT_EVENT_ON_DIALOG_STATUS);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pGameObject);
    sEluna.ExecuteCall(3, 0);
    return DIALOG_STATUS_SCRIPTED_NO_STATUS; // DIALOG_STATUS_UNDEFINED
}

void HookMgr::OnDestroyed(GameObject* pGameObject, Player* pPlayer)
{
    int bind = sEluna.GameObjectEventBindings.GetBind(pGameObject->GetEntry(), GAMEOBJECT_EVENT_ON_DESTROYED);
    if (!bind)
        return;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GAMEOBJECT_EVENT_ON_DESTROYED);
    sEluna.Push(sEluna.L, pGameObject);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.ExecuteCall(3, 0);
}

void HookMgr::OnDamaged(GameObject* pGameObject, Player* pPlayer)
{
    int bind = sEluna.GameObjectEventBindings.GetBind(pGameObject->GetEntry(), GAMEOBJECT_EVENT_ON_DAMAGED);
    if (!bind)
        return;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GAMEOBJECT_EVENT_ON_DAMAGED);
    sEluna.Push(sEluna.L, pGameObject);
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.ExecuteCall(3, 0);
}

void HookMgr::OnLootStateChanged(GameObject* pGameObject, uint32 state, Unit* pUnit)
{
    int bind = sEluna.GameObjectEventBindings.GetBind(pGameObject->GetEntry(), GAMEOBJECT_EVENT_ON_LOOT_STATE_CHANGE);
    if (!bind)
        return;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GAMEOBJECT_EVENT_ON_LOOT_STATE_CHANGE);
    sEluna.Push(sEluna.L, pGameObject);
    sEluna.Push(sEluna.L, state);
    sEluna.Push(sEluna.L, pUnit);
    sEluna.ExecuteCall(4, 0);
}

void HookMgr::OnGameObjectStateChanged(GameObject* pGameObject, uint32 state)
{
    int bind = sEluna.GameObjectEventBindings.GetBind(pGameObject->GetEntry(), GAMEOBJECT_EVENT_ON_GO_STATE_CHANGED);
    if (!bind)
        return;
    sEluna.BeginCall(bind);
    sEluna.Push(sEluna.L, GAMEOBJECT_EVENT_ON_GO_STATE_CHANGED);
    sEluna.Push(sEluna.L, pGameObject);
    sEluna.Push(sEluna.L, state);
    sEluna.ExecuteCall(3, 0);
}
// Player
void HookMgr::OnPlayerEnterCombat(Player* pPlayer, Unit* pEnemy)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_ENTER_COMBAT))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pEnemy);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnPlayerLeaveCombat(Player* pPlayer)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_LEAVE_COMBAT))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnPVPKill(Player* pKiller, Player* pKilled)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_KILL_PLAYER))
        return;
    sEluna.Push(sEluna.L, pKiller);
    sEluna.Push(sEluna.L, pKilled);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnCreatureKill(Player* pKiller, Creature* pKilled)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_KILL_CREATURE))
        return;
    sEluna.Push(sEluna.L, pKiller);
    sEluna.Push(sEluna.L, pKilled);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnPlayerKilledByCreature(Creature* pKiller, Player* pKilled)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_KILLED_BY_CREATURE))
        return;
    sEluna.Push(sEluna.L, pKiller);
    sEluna.Push(sEluna.L, pKilled);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnLevelChanged(Player* pPlayer, uint8 oldLevel)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_LEVEL_CHANGE))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, oldLevel);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnFreeTalentPointsChanged(Player* pPlayer, uint32 newPoints)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_TALENTS_CHANGE))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, newPoints);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnTalentsReset(Player* pPlayer, bool noCost)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_TALENTS_RESET))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, noCost);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnMoneyChanged(Player* pPlayer, int32& amount)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_MONEY_CHANGE))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, amount);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnGiveXP(Player* pPlayer, uint32& amount, Unit* pVictim)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_GIVE_XP))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, amount);
    sEluna.Push(sEluna.L, pVictim);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnReputationChange(Player* pPlayer, uint32 factionID, int32& standing, bool incremental)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_REPUTATION_CHANGE))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, factionID);
    sEluna.Push(sEluna.L, standing);
    sEluna.Push(sEluna.L, incremental);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnDuelRequest(Player* pTarget, Player* pChallenger)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_DUEL_REQUEST))
        return;
    sEluna.Push(sEluna.L, pTarget);
    sEluna.Push(sEluna.L, pChallenger);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnDuelStart(Player* pStarter, Player* pChallenger)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_DUEL_START))
        return;
    sEluna.Push(sEluna.L, pStarter);
    sEluna.Push(sEluna.L, pChallenger);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnDuelEnd(Player* pWinner, Player* pLoser, DuelCompleteType type)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_DUEL_END))
        return;
    sEluna.Push(sEluna.L, pWinner);
    sEluna.Push(sEluna.L, pLoser);
    sEluna.Push(sEluna.L, type);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnChat(Player* pPlayer, uint32 type, uint32 lang, std::string& msg, Player* pReceiver)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_WHISPER))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, msg);
    sEluna.Push(sEluna.L, type);
    sEluna.Push(sEluna.L, lang);
    sEluna.Push(sEluna.L, pReceiver);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnEmote(Player* pPlayer, uint32 emote)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_EMOTE))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, emote);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnTextEmote(Player* pPlayer, uint32 textEmote, uint32 emoteNum, uint64 guid)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_TEXT_EMOTE))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, textEmote);
    sEluna.Push(sEluna.L, emoteNum);
    sEluna.Push(sEluna.L, guid);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnSpellCast(Player* pPlayer, Spell* pSpell, bool skipCheck)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_SPELL_CAST))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pSpell);
    sEluna.Push(sEluna.L, skipCheck);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnLogin(Player* pPlayer)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_LOGIN))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnLogout(Player* pPlayer)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_LOGOUT))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnCreate(Player* pPlayer)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_CHARACTER_CREATE))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnDelete(uint32 guidlow)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_CHARACTER_DELETE))
        return;
    sEluna.Push(sEluna.L, guidlow);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnSave(Player* pPlayer)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_SAVE))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnBindToInstance(Player* pPlayer, Difficulty difficulty, uint32 mapid, bool permanent)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_BIND_TO_INSTANCE))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, difficulty);
    sEluna.Push(sEluna.L, mapid);
    sEluna.Push(sEluna.L, permanent);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnUpdateZone(Player* pPlayer, uint32 newZone, uint32 newArea)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_UPDATE_ZONE))
        return;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, newZone);
    sEluna.Push(sEluna.L, newArea);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

void HookMgr::OnMapChanged(Player* player)
{
    if (!sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_MAP_CHANGE))
        return;
    sEluna.Push(sEluna.L, player);
    sEluna.PlayerEventBindings.ExecuteCall();
    sEluna.PlayerEventBindings.EndCall();
}

bool HookMgr::OnChat(Player* pPlayer, uint32 type, uint32 lang, std::string& msg)
{
    bool result = true;
    if (sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_CHAT))
    {
        sEluna.Push(sEluna.L, pPlayer);
        sEluna.Push(sEluna.L, msg);
        sEluna.Push(sEluna.L, type);
        sEluna.Push(sEluna.L, lang);
        sEluna.PlayerEventBindings.ExecuteCall();
        for (int i = 1; i <= lua_gettop(sEluna.L); ++i)
        {
            if (lua_isnoneornil(sEluna.L, i))
                continue;
            if (!lua_toboolean(sEluna.L, i))
                result = false;
        }
        sEluna.PlayerEventBindings.EndCall();
    }
    return result;
}

bool HookMgr::OnChat(Player* pPlayer, uint32 type, uint32 lang, std::string& msg, Group* pGroup)
{
    bool result = true;
    if (sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_GROUP_CHAT))
    {
        sEluna.Push(sEluna.L, pPlayer);
        sEluna.Push(sEluna.L, msg);
        sEluna.Push(sEluna.L, type);
        sEluna.Push(sEluna.L, lang);
        sEluna.Push(sEluna.L, pGroup);
        sEluna.PlayerEventBindings.ExecuteCall();
        for (int i = 1; i <= lua_gettop(sEluna.L); ++i)
        {
            if (lua_isnoneornil(sEluna.L, i))
                continue;
            if (!lua_toboolean(sEluna.L, i))
                result = false;
        }
        sEluna.PlayerEventBindings.EndCall();
    }
    return result;
}

bool HookMgr::OnChat(Player* pPlayer, uint32 type, uint32 lang, std::string& msg, Guild* pGuild)
{
    bool result = true;
    if (sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_GUILD_CHAT))
    {
        sEluna.Push(sEluna.L, pPlayer);
        sEluna.Push(sEluna.L, msg);
        sEluna.Push(sEluna.L, type);
        sEluna.Push(sEluna.L, lang);
        sEluna.Push(sEluna.L, pGuild);
        sEluna.PlayerEventBindings.ExecuteCall();
        for (int i = 1; i <= lua_gettop(sEluna.L); ++i)
        {
            if (lua_isnoneornil(sEluna.L, i))
                continue;
            if (!lua_toboolean(sEluna.L, i))
                result = false;
        }
        sEluna.PlayerEventBindings.EndCall();
    }
    return result;
}

bool HookMgr::OnChat(Player* pPlayer, uint32 type, uint32 lang, std::string& msg, Channel* pChannel)
{
    bool result = true;
    if (sEluna.PlayerEventBindings.BeginCall(PLAYER_EVENT_ON_CHANNEL_CHAT))
    {
        sEluna.Push(sEluna.L, pPlayer);
        sEluna.Push(sEluna.L, msg);
        sEluna.Push(sEluna.L, type);
        sEluna.Push(sEluna.L, lang);
        sEluna.Push(sEluna.L, pChannel->GetChannelId());
        sEluna.PlayerEventBindings.ExecuteCall();
        for (int i = 1; i <= lua_gettop(sEluna.L); ++i)
        {
            if (lua_isnoneornil(sEluna.L, i))
                continue;
            if (!lua_toboolean(sEluna.L, i))
                result = false;
        }
        sEluna.PlayerEventBindings.EndCall();
    }
    return result;
}

// Vehicle
void HookMgr::OnInstall(Vehicle* vehicle)
{
    if (!sEluna.VehicleEventBindings.BeginCall(VEHICLE_EVENT_ON_INSTALL))
        return;
    sEluna.Push(sEluna.L, vehicle);
    sEluna.VehicleEventBindings.ExecuteCall();
    sEluna.VehicleEventBindings.EndCall();
}

void HookMgr::OnUninstall(Vehicle* vehicle)
{
    if (!sEluna.VehicleEventBindings.BeginCall(VEHICLE_EVENT_ON_UNINSTALL))
        return;
    sEluna.Push(sEluna.L, vehicle);
    sEluna.VehicleEventBindings.ExecuteCall();
    sEluna.VehicleEventBindings.EndCall();
}

void HookMgr::OnReset(Vehicle* vehicle)
{
    if (!sEluna.VehicleEventBindings.BeginCall(VEHICLE_EVENT_ON_RESET))
        return;
    sEluna.Push(sEluna.L, vehicle);
    sEluna.VehicleEventBindings.ExecuteCall();
    sEluna.VehicleEventBindings.EndCall();
}

void HookMgr::OnInstallAccessory(Vehicle* vehicle, Creature* accessory)
{
    if (!sEluna.VehicleEventBindings.BeginCall(VEHICLE_EVENT_ON_INSTALL_ACCESSORY))
        return;
    sEluna.Push(sEluna.L, vehicle);
    sEluna.Push(sEluna.L, accessory);
    sEluna.VehicleEventBindings.ExecuteCall();
    sEluna.VehicleEventBindings.EndCall();
}

void HookMgr::OnAddPassenger(Vehicle* vehicle, Unit* passenger, int8 seatId)
{
    if (!sEluna.VehicleEventBindings.BeginCall(VEHICLE_EVENT_ON_ADD_PASSENGER))
        return;
    sEluna.Push(sEluna.L, vehicle);
    sEluna.Push(sEluna.L, passenger);
    sEluna.Push(sEluna.L, seatId);
    sEluna.VehicleEventBindings.ExecuteCall();
    sEluna.VehicleEventBindings.EndCall();
}

void HookMgr::OnRemovePassenger(Vehicle* vehicle, Unit* passenger)
{
    if (!sEluna.VehicleEventBindings.BeginCall(VEHICLE_EVENT_ON_REMOVE_PASSENGER))
        return;
    sEluna.Push(sEluna.L, vehicle);
    sEluna.Push(sEluna.L, passenger);
    sEluna.VehicleEventBindings.ExecuteCall();
    sEluna.VehicleEventBindings.EndCall();
}

// areatrigger
bool HookMgr::OnAreaTrigger(Player* pPlayer, AreaTriggerEntry const* pTrigger)
{
    if (!sEluna.ServerEventBindings.BeginCall(TRIGGER_EVENT_ON_TRIGGER))
        return false;
    sEluna.Push(sEluna.L, pPlayer);
    sEluna.Push(sEluna.L, pTrigger->id);
    sEluna.ServerEventBindings.ExecuteCall();
    sEluna.ServerEventBindings.EndCall();
    return false;
}
// weather
void HookMgr::OnChange(Weather* weather, WeatherState state, float grade)
{
    if (!sEluna.ServerEventBindings.BeginCall(WEATHER_EVENT_ON_CHANGE))
        return;
    sEluna.Push(sEluna.L, (weather->GetZone()));
    sEluna.Push(sEluna.L, state);
    sEluna.Push(sEluna.L, grade);
    sEluna.ServerEventBindings.ExecuteCall();
    sEluna.ServerEventBindings.EndCall();
}
// transport
void HookMgr::OnAddPassenger(Transport* transport, Player* player)
{
}
void HookMgr::OnAddCreaturePassenger(Transport* transport, Creature* creature)
{
}
void HookMgr::OnRemovePassenger(Transport* transport, Player* player)
{
}
void HookMgr::OnRelocate(Transport* transport, uint32 waypointId, uint32 mapId, float x, float y, float z)
{
}
// Auction House
void HookMgr::OnAdd(AuctionHouseObject* ah)
{
    if (!sEluna.ServerEventBindings.BeginCall(AUCTION_EVENT_ON_ADD))
        return;
    sEluna.Push(sEluna.L, (ah));
    sEluna.ServerEventBindings.ExecuteCall();
    sEluna.ServerEventBindings.EndCall();
}

void HookMgr::OnRemove(AuctionHouseObject* ah)
{
    if (!sEluna.ServerEventBindings.BeginCall(AUCTION_EVENT_ON_REMOVE))
        return;
    sEluna.Push(sEluna.L, (ah));
    sEluna.ServerEventBindings.ExecuteCall();
    sEluna.ServerEventBindings.EndCall();
}

void HookMgr::OnSuccessful(AuctionHouseObject* ah)
{
    if (!sEluna.ServerEventBindings.BeginCall(AUCTION_EVENT_ON_SUCCESSFUL))
        return;
    sEluna.Push(sEluna.L, (ah));
    sEluna.ServerEventBindings.ExecuteCall();
    sEluna.ServerEventBindings.EndCall();
}

void HookMgr::OnExpire(AuctionHouseObject* ah)
{
    if (!sEluna.ServerEventBindings.BeginCall(AUCTION_EVENT_ON_EXPIRE))
        return;
    sEluna.Push(sEluna.L, (ah));
    sEluna.ServerEventBindings.ExecuteCall();
    sEluna.ServerEventBindings.EndCall();
}

// Packet
bool HookMgr::OnPacketSend(WorldSession* session, WorldPacket& packet)
{
    bool result = true;
    Player* player = NULL;
    if (session)
        player = session->GetPlayer();
    if (sEluna.ServerEventBindings.BeginCall(SERVER_EVENT_ON_PACKET_SEND))
    {
        sEluna.Push(sEluna.L, &packet);
        sEluna.Push(sEluna.L, player);
        sEluna.ServerEventBindings.ExecuteCall();
        for (int i = 1; i <= lua_gettop(sEluna.L); ++i)
        {
            if (lua_isnoneornil(sEluna.L, i))
                continue;
            WorldPacket* data = sEluna.CHECK_PACKET(sEluna.L, 1);
            if (data)
                packet = *data;
            if (!lua_toboolean(sEluna.L, 1))
                result = false;
        }
        sEluna.ServerEventBindings.EndCall();
    }
    if (sEluna.PacketEventBindings.BeginCall(packet.GetOpcode()))
    {
        sEluna.Push(sEluna.L, &packet);
        sEluna.Push(sEluna.L, player);
        sEluna.PacketEventBindings.ExecuteCall();
        for (int i = 1; i <= lua_gettop(sEluna.L); ++i)
        {
            if (lua_isnoneornil(sEluna.L, i))
                continue;
            WorldPacket* data = sEluna.CHECK_PACKET(sEluna.L, 1);
            if (data)
                packet = *data;
            if (!lua_toboolean(sEluna.L, 1))
                result = false;
        }
        sEluna.PacketEventBindings.EndCall();
    }
    return result;
}
bool HookMgr::OnPacketReceive(WorldSession* session, WorldPacket& packet)
{
    bool result = true;
    Player* player = NULL;
    if (session)
        player = session->GetPlayer();
    if (sEluna.ServerEventBindings.BeginCall(SERVER_EVENT_ON_PACKET_RECEIVE))
    {
        sEluna.Push(sEluna.L, &packet);
        sEluna.Push(sEluna.L, player);
        sEluna.ServerEventBindings.ExecuteCall();
        for (int i = 1; i <= lua_gettop(sEluna.L); ++i)
        {
            if (lua_isnoneornil(sEluna.L, i))
                continue;
            WorldPacket* data = sEluna.CHECK_PACKET(sEluna.L, 1);
            if (data)
                packet = *data;
            if (!lua_toboolean(sEluna.L, 1))
                result = false;
        }
        sEluna.ServerEventBindings.EndCall();
    }
    if (sEluna.PacketEventBindings.BeginCall(packet.GetOpcode()))
    {
        sEluna.Push(sEluna.L, &packet);
        sEluna.Push(sEluna.L, player);
        sEluna.PacketEventBindings.ExecuteCall();
        for (int i = 1; i <= lua_gettop(sEluna.L); ++i)
        {
            if (lua_isnoneornil(sEluna.L, i))
                continue;
            WorldPacket* data = sEluna.CHECK_PACKET(sEluna.L, 1);
            if (data)
                packet = *data;
            if (!lua_toboolean(sEluna.L, 1))
                result = false;
        }
        sEluna.PacketEventBindings.EndCall();
    }
    return result;
}

struct ElunaWorldAI : public WorldScript
{
public:
    ElunaWorldAI() : WorldScript("ElunaWorldAI") { }
    ~ElunaWorldAI() { }

    void OnOpenStateChange(bool open) OVERRIDE
    {
        if (!sEluna.ServerEventBindings.BeginCall(WORLD_EVENT_ON_OPEN_STATE_CHANGE))
            return;
        sEluna.Push(sEluna.L, open);
        sEluna.ServerEventBindings.ExecuteCall();
        sEluna.ServerEventBindings.EndCall();
    }

    void OnConfigLoad(bool reload) OVERRIDE
    {
        if (!sEluna.ServerEventBindings.BeginCall(WORLD_EVENT_ON_CONFIG_LOAD))
            return;
        sEluna.Push(sEluna.L, reload);
        sEluna.ServerEventBindings.ExecuteCall();
        sEluna.ServerEventBindings.EndCall();
    }

    void OnMotdChange(std::string& newMotd) OVERRIDE
    {
        if (!sEluna.ServerEventBindings.BeginCall(WORLD_EVENT_ON_MOTD_CHANGE))
            return;
        sEluna.Push(sEluna.L, newMotd);
        sEluna.ServerEventBindings.ExecuteCall();
        sEluna.ServerEventBindings.EndCall();
    }

    void OnShutdownInitiate(ShutdownExitCode code, ShutdownMask mask) OVERRIDE
    {
        if (!sEluna.ServerEventBindings.BeginCall(WORLD_EVENT_ON_SHUTDOWN_INIT))
            return;
        sEluna.Push(sEluna.L, code);
        sEluna.Push(sEluna.L, mask);
        sEluna.ServerEventBindings.ExecuteCall();
        sEluna.ServerEventBindings.EndCall();
    }

    void OnShutdownCancel() OVERRIDE
    {
        if (!sEluna.ServerEventBindings.BeginCall(WORLD_EVENT_ON_SHUTDOWN_CANCEL))
            return;
        sEluna.ServerEventBindings.ExecuteCall();
        sEluna.ServerEventBindings.EndCall();
    }

    void OnUpdate(uint32 diff) OVERRIDE
    {
        sEluna.m_EventMgr.Update(diff);
        if (!sEluna.ServerEventBindings.BeginCall(WORLD_EVENT_ON_UPDATE))
            return;
        sEluna.Push(sEluna.L, diff);
        sEluna.ServerEventBindings.ExecuteCall();
        sEluna.ServerEventBindings.EndCall();
    }

    void OnStartup() OVERRIDE
    {
        if (!sEluna.ServerEventBindings.BeginCall(WORLD_EVENT_ON_STARTUP))
            return;
        sEluna.ServerEventBindings.ExecuteCall();
        sEluna.ServerEventBindings.EndCall();
    }

    void OnShutdown() OVERRIDE
    {
        if (!sEluna.ServerEventBindings.BeginCall(WORLD_EVENT_ON_SHUTDOWN))
            return;
        sEluna.ServerEventBindings.ExecuteCall();
        sEluna.ServerEventBindings.EndCall();
    }
};

struct ElunaCreatureAI : ScriptedAI
{
    ElunaCreatureAI(Creature* creature) : ScriptedAI(creature) { }
    ~ElunaCreatureAI() { }

    //Called at World update tick
    void UpdateAI(uint32 diff) OVERRIDE
    {
        ScriptedAI::UpdateAI(diff);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_AIUPDATE);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_AIUPDATE);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, diff);
        sEluna.ExecuteCall(3, 0);
    }

    //Called for reaction at enter to combat if not in combat yet (enemy can be NULL)
    //Called at creature aggro either by MoveInLOS or Attack Start
    void EnterCombat(Unit* target) OVERRIDE
    {
        ScriptedAI::EnterCombat(target);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_ENTER_COMBAT);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_ENTER_COMBAT);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, target);
        sEluna.ExecuteCall(3, 0);
    }

    // Called at any Damage from any attacker (before damage apply)
    void DamageTaken(Unit* attacker, uint32& damage) OVERRIDE
    {
        ScriptedAI::DamageTaken(attacker, damage);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_DAMAGE_TAKEN);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_DAMAGE_TAKEN);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, attacker);
        sEluna.Push(sEluna.L, damage);
        sEluna.ExecuteCall(4, 0);
    }

    //Called at creature death
    void JustDied(Unit* killer) OVERRIDE
    {
        ScriptedAI::JustDied(killer);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_DIED);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_DIED);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, killer);
        sEluna.ExecuteCall(3, 0);
    }

    //Called at creature killing another unit
    void KilledUnit(Unit* victim) OVERRIDE
    {
        ScriptedAI::KilledUnit(victim);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_TARGET_DIED);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_TARGET_DIED);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, victim);
        sEluna.ExecuteCall(3, 0);
    }

    // Called when the creature summon successfully other creature
    void JustSummoned(Creature* summon) OVERRIDE
    {
        ScriptedAI::JustSummoned(summon);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_JUST_SUMMONED_CREATURE);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_JUST_SUMMONED_CREATURE);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, summon);
        sEluna.ExecuteCall(3, 0);
    }

    // Called when a summoned creature is despawned
    void SummonedCreatureDespawn(Creature* summon) OVERRIDE
    {
        ScriptedAI::SummonedCreatureDespawn(summon);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_SUMMONED_CREATURE_DESPAWN);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_SUMMONED_CREATURE_DESPAWN);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, summon);
        sEluna.ExecuteCall(3, 0);
    }

    // Called when hit by a spell
    void SpellHit(Unit* caster, SpellInfo const* spell) OVERRIDE
    {
        ScriptedAI::SpellHit(caster, spell);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_HIT_BY_SPELL);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_HIT_BY_SPELL);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, caster);
        sEluna.Push(sEluna.L, spell->Id); // Pass spell object?
        sEluna.ExecuteCall(4, 0);
    }

    // Called when spell hits a target
    void SpellHitTarget(Unit* target, SpellInfo const* spell) OVERRIDE
    {
        ScriptedAI::SpellHitTarget(target, spell);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_SPELL_HIT_TARGET);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_SPELL_HIT_TARGET);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, target);
        sEluna.Push(sEluna.L, spell->Id); // Pass spell object?
        sEluna.ExecuteCall(4, 0);
    }

    //Called at waypoint reached or PointMovement end
    void MovementInform(uint32 type, uint32 id) OVERRIDE
    {
        ScriptedAI::MovementInform(type, id);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_REACH_WP);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_REACH_WP);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, type);
        sEluna.Push(sEluna.L, id);
        sEluna.ExecuteCall(4, 0);
    }

    // Called when AI is temporarily replaced or put back when possess is applied or removed
    void OnPossess(bool apply) OVERRIDE
    {
        ScriptedAI::OnPossess(apply);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_POSSESS);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_POSSESS);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, apply);
        sEluna.ExecuteCall(3, 0);
    }

    //Called at creature reset either by death or evade
    void Reset() OVERRIDE
    {
        ScriptedAI::Reset();
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_RESET);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_RESET);
        sEluna.Push(sEluna.L, me);
        sEluna.ExecuteCall(2, 0);
    }

    // Called before EnterCombat even before the creature is in combat.
    void AttackStart(Unit* target) OVERRIDE
    {
        ScriptedAI::AttackStart(target);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_PRE_COMBAT);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_PRE_COMBAT);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, target);
        sEluna.ExecuteCall(3, 0);
    }

    // Called in Creature::Update when deathstate = DEAD. Inherited classes may maniuplate the ability to respawn based on scripted events.
    bool CanRespawn() OVERRIDE
    {
        ScriptedAI::CanRespawn();
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_CAN_RESPAWN);
        if (!bind)
            return true;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_CAN_RESPAWN);
        sEluna.Push(sEluna.L, me);
        sEluna.ExecuteCall(2, 0);
        return true;
    }

    // Called for reaction at stopping attack at no attackers or targets
    void EnterEvadeMode() OVERRIDE
    {
        ScriptedAI::EnterEvadeMode();
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_LEAVE_COMBAT);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_LEAVE_COMBAT);
        sEluna.Push(sEluna.L, me);
        sEluna.ExecuteCall(2, 0);
    }

    // Called when the creature is summoned successfully by other creature
    void IsSummonedBy(Unit* summoner) OVERRIDE
    {
        ScriptedAI::IsSummonedBy(summoner);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_SUMMONED);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_SUMMONED);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, summoner);
        sEluna.ExecuteCall(3, 0);
    }

    void SummonedCreatureDies(Creature* summon, Unit* killer) OVERRIDE
    {
        ScriptedAI::SummonedCreatureDies(summon, killer);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_SUMMONED_CREATURE_DIED);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_SUMMONED_CREATURE_DIED);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, summon);
        sEluna.Push(sEluna.L, killer);
        sEluna.ExecuteCall(4, 0);
    }

    // Called when the creature is target of hostile action: swing, hostile spell landed, fear/etc)
    void AttackedBy(Unit* attacker) OVERRIDE
    {
        ScriptedAI::AttackedBy(attacker);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_ATTACKED_AT);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_ATTACKED_AT);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, attacker);
        sEluna.ExecuteCall(3, 0);
    }

    // Called when creature is spawned or respawned (for reseting variables)
    void JustRespawned() OVERRIDE
    {
        ScriptedAI::JustRespawned();
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_SPAWN);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_SPAWN);
        sEluna.Push(sEluna.L, me);
        sEluna.ExecuteCall(2, 0);
    }

    void OnCharmed(bool apply) OVERRIDE
    {
        ScriptedAI::OnCharmed(apply);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_CHARMED);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_CHARMED);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, apply);
        sEluna.ExecuteCall(3, 0);
    }

    // Called at reaching home after evade
    void JustReachedHome() OVERRIDE
    {
        ScriptedAI::JustReachedHome();
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_REACH_HOME);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_REACH_HOME);
        sEluna.Push(sEluna.L, me);
        sEluna.ExecuteCall(2, 0);
    }

    // Called at text emote receive from player
    void ReceiveEmote(Player* player, uint32 emoteId) OVERRIDE
    {
        ScriptedAI::ReceiveEmote(player, emoteId);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_RECEIVE_EMOTE);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_RECEIVE_EMOTE);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, player);
        sEluna.Push(sEluna.L, emoteId);
        sEluna.ExecuteCall(4, 0);
    }

    // Called when owner takes damage
    void OwnerAttackedBy(Unit* attacker) OVERRIDE
    {
        ScriptedAI::OwnerAttackedBy(attacker);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_OWNER_ATTACKED_AT);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_OWNER_ATTACKED_AT);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, attacker);
        sEluna.ExecuteCall(3, 0);
    }

    // Called when owner attacks something
    void OwnerAttacked(Unit* target) OVERRIDE
    {
        ScriptedAI::OwnerAttacked(target);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_OWNER_ATTACKED);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_OWNER_ATTACKED);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, target);
        sEluna.ExecuteCall(3, 0);
    }

    // called when the corpse of this creature gets removed
    void CorpseRemoved(uint32& respawnDelay) OVERRIDE
    {
        ScriptedAI::CorpseRemoved(respawnDelay);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_CORPSE_REMOVED);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_CORPSE_REMOVED);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, respawnDelay);
        sEluna.ExecuteCall(3, 0);
    }

    void PassengerBoarded(Unit* passenger, int8 seatId, bool apply) OVERRIDE
    {
        ScriptedAI::PassengerBoarded(passenger, seatId, apply);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_PASSANGER_BOARDED);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_PASSANGER_BOARDED);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, passenger);
        sEluna.Push(sEluna.L, seatId);
        sEluna.Push(sEluna.L, apply);
        sEluna.ExecuteCall(5, 0);
    }

    void OnSpellClick(Unit* clicker, bool& result) OVERRIDE
    {
        ScriptedAI::OnSpellClick(clicker, result);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_SPELL_CLICK);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_SPELL_CLICK);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, clicker);
        sEluna.Push(sEluna.L, result);
        sEluna.ExecuteCall(4, 0);
    }

    void MoveInLineOfSight(Unit* who) OVERRIDE
    {
        ScriptedAI::MoveInLineOfSight(who);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_MOVE_IN_LOS);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_MOVE_IN_LOS);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, who);
        sEluna.ExecuteCall(3, 0);
    }

    // Called if IsVisible(Unit* who) is true at each who move, reaction at visibility zone enter
    void MoveInLineOfSight_Safe(Unit* who)
    {
        ScriptedAI::MoveInLineOfSight_Safe(who);
        int bind = sEluna.CreatureEventBindings.GetBind(me->GetEntry(), CREATURE_EVENT_ON_VISIBLE_MOVE_IN_LOS);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, CREATURE_EVENT_ON_VISIBLE_MOVE_IN_LOS);
        sEluna.Push(sEluna.L, me);
        sEluna.Push(sEluna.L, who);
        sEluna.ExecuteCall(3, 0);
    }
};

struct ElunaGameObjectAI : public GameObjectAI
{
    ElunaGameObjectAI(GameObject* _go) : GameObjectAI(_go) { }
    ~ElunaGameObjectAI()
    {
    }

    void UpdateAI(uint32 diff) OVERRIDE
    {
        int bind = sEluna.GameObjectEventBindings.GetBind(go->GetEntry(), GAMEOBJECT_EVENT_ON_AIUPDATE);
        if (!bind)
            return;
        sEluna.BeginCall(bind);
        sEluna.Push(sEluna.L, GAMEOBJECT_EVENT_ON_AIUPDATE);
        sEluna.Push(sEluna.L, go);
        sEluna.Push(sEluna.L, diff);
        sEluna.ExecuteCall(3, 0);
    }

    // executed when a timed event fires
    void OnScriptEvent(int funcRef, uint32 delay, uint32 calls) OVERRIDE
    {
        sEluna.BeginCall(funcRef);
        sEluna.Push(sEluna.L, funcRef);
        sEluna.Push(sEluna.L, delay);
        sEluna.Push(sEluna.L, calls);
        sEluna.Push(sEluna.L, go);
        sEluna.ExecuteCall(4, 0);
    }

    void Reset() OVERRIDE
    {
        sEluna.BeginCall(sEluna.GameObjectEventBindings.GetBind(go->GetEntry(), GAMEOBJECT_EVENT_ON_RESET));
        sEluna.Push(sEluna.L, GAMEOBJECT_EVENT_ON_RESET);
        sEluna.Push(sEluna.L, go);
        sEluna.ExecuteCall(2, 0);
    }
};

void HookMgr::OnAddMember(Guild* guild, Player* player, uint32 plRank)
{
    if (!sEluna.GuildEventBindings.BeginCall(GUILD_EVENT_ON_ADD_MEMBER))
        return;
    sEluna.Push(sEluna.L, guild);
    sEluna.Push(sEluna.L, player);
    sEluna.Push(sEluna.L, plRank);
    sEluna.GuildEventBindings.ExecuteCall();
    sEluna.GuildEventBindings.EndCall();
}

void HookMgr::OnRemoveMember(Guild* guild, Player* player, bool isDisbanding, bool isKicked) // IsKicked not a part of Mangos, implement?
{
    if (!sEluna.GuildEventBindings.BeginCall(GUILD_EVENT_ON_REMOVE_MEMBER))
        return;
    sEluna.Push(sEluna.L, guild);
    sEluna.Push(sEluna.L, player);
    sEluna.Push(sEluna.L, isDisbanding);
    sEluna.Push(sEluna.L, isKicked); // IsKicked not a part of Mangos, implement?
    sEluna.GuildEventBindings.ExecuteCall();
    sEluna.GuildEventBindings.EndCall();
}

void HookMgr::OnMOTDChanged(Guild* guild, const std::string& newMotd)
{
    if (!sEluna.GuildEventBindings.BeginCall(GUILD_EVENT_ON_MOTD_CHANGE))
        return;
    sEluna.Push(sEluna.L, guild);
    sEluna.Push(sEluna.L, newMotd);
    sEluna.GuildEventBindings.ExecuteCall();
    sEluna.GuildEventBindings.EndCall();
}

void HookMgr::OnInfoChanged(Guild* guild, const std::string& newInfo)
{
    if (!sEluna.GuildEventBindings.BeginCall(GUILD_EVENT_ON_INFO_CHANGE))
        return;
    sEluna.Push(sEluna.L, guild);
    sEluna.Push(sEluna.L, newInfo);
    sEluna.GuildEventBindings.ExecuteCall();
    sEluna.GuildEventBindings.EndCall();
}

void HookMgr::OnCreate(Guild* guild, Player* leader, const std::string& name)
{
    if (!sEluna.GuildEventBindings.BeginCall(GUILD_EVENT_ON_CREATE))
        return;
    sEluna.Push(sEluna.L, guild);
    sEluna.Push(sEluna.L, leader);
    sEluna.Push(sEluna.L, name);
    sEluna.GuildEventBindings.ExecuteCall();
    sEluna.GuildEventBindings.EndCall();
}

void HookMgr::OnDisband(Guild* guild)
{
    if (!sEluna.GuildEventBindings.BeginCall(GUILD_EVENT_ON_DISBAND))
        return;
    sEluna.Push(sEluna.L, guild);
    sEluna.GuildEventBindings.ExecuteCall();
    sEluna.GuildEventBindings.EndCall();
}

void HookMgr::OnMemberWitdrawMoney(Guild* guild, Player* player, uint32 &amount, bool isRepair) // isRepair not a part of Mangos, implement?
{
    if (!sEluna.GuildEventBindings.BeginCall(GUILD_EVENT_ON_MONEY_WITHDRAW))
        return;
    sEluna.Push(sEluna.L, guild);
    sEluna.Push(sEluna.L, player);
    sEluna.Push(sEluna.L, amount);
    sEluna.Push(sEluna.L, isRepair); // isRepair not a part of Mangos, implement?
    sEluna.GuildEventBindings.ExecuteCall();
    sEluna.GuildEventBindings.EndCall();
}

void HookMgr::OnMemberDepositMoney(Guild* guild, Player* player, uint32 &amount)
{
    if (!sEluna.GuildEventBindings.BeginCall(GUILD_EVENT_ON_MONEY_DEPOSIT))
        return;
    sEluna.Push(sEluna.L, guild);
    sEluna.Push(sEluna.L, player);
    sEluna.Push(sEluna.L, amount);
    sEluna.GuildEventBindings.ExecuteCall();
    sEluna.GuildEventBindings.EndCall();
}

void HookMgr::OnItemMove(Guild* guild, Player* player, Item* pItem, bool isSrcBank, uint8 srcContainer, uint8 srcSlotId,
                         bool isDestBank, uint8 destContainer, uint8 destSlotId)
{
    if (!sEluna.GuildEventBindings.BeginCall(GUILD_EVENT_ON_ITEM_MOVE))
        return;
    sEluna.Push(sEluna.L, guild);
    sEluna.Push(sEluna.L, player);
    sEluna.Push(sEluna.L, pItem);
    sEluna.Push(sEluna.L, isSrcBank);
    sEluna.Push(sEluna.L, srcContainer);
    sEluna.Push(sEluna.L, srcSlotId);
    sEluna.Push(sEluna.L, isDestBank);
    sEluna.Push(sEluna.L, destContainer);
    sEluna.Push(sEluna.L, destSlotId);
    sEluna.GuildEventBindings.ExecuteCall();
    sEluna.GuildEventBindings.EndCall();
}

void HookMgr::OnEvent(Guild* guild, uint8 eventType, uint32 playerGuid1, uint32 playerGuid2, uint8 newRank)
{
    if (!sEluna.GuildEventBindings.BeginCall(GUILD_EVENT_ON_EVENT))
        return;
    sEluna.Push(sEluna.L, guild);
    sEluna.Push(sEluna.L, eventType);
    sEluna.Push(sEluna.L, playerGuid1);
    sEluna.Push(sEluna.L, playerGuid2);
    sEluna.Push(sEluna.L, newRank);
    sEluna.GuildEventBindings.ExecuteCall();
    sEluna.GuildEventBindings.EndCall();
}

void HookMgr::OnBankEvent(Guild* guild, uint8 eventType, uint8 tabId, uint32 playerGuid, uint32 itemOrMoney, uint16 itemStackCount, uint8 destTabId)
{
    if (!sEluna.GuildEventBindings.BeginCall(GUILD_EVENT_ON_BANK_EVENT))
        return;
    sEluna.Push(sEluna.L, guild);
    sEluna.Push(sEluna.L, eventType);
    sEluna.Push(sEluna.L, tabId);
    sEluna.Push(sEluna.L, playerGuid);
    sEluna.Push(sEluna.L, itemOrMoney);
    sEluna.Push(sEluna.L, itemStackCount);
    sEluna.Push(sEluna.L, destTabId);
    sEluna.GuildEventBindings.ExecuteCall();
    sEluna.GuildEventBindings.EndCall();
}
// Group
void HookMgr::OnAddMember(Group* group, uint64 guid)
{
    if (!sEluna.GroupEventBindings.BeginCall(GROUP_EVENT_ON_MEMBER_ADD))
        return;
    sEluna.Push(sEluna.L, group);
    sEluna.Push(sEluna.L, guid);
    sEluna.GroupEventBindings.ExecuteCall();
    sEluna.GroupEventBindings.EndCall();
}

void HookMgr::OnInviteMember(Group* group, uint64 guid)
{
    if (!sEluna.GroupEventBindings.BeginCall(GROUP_EVENT_ON_MEMBER_INVITE))
        return;
    sEluna.Push(sEluna.L, group);
    sEluna.Push(sEluna.L, guid);
    sEluna.GroupEventBindings.ExecuteCall();
    sEluna.GroupEventBindings.EndCall();
}

void HookMgr::OnRemoveMember(Group* group, uint64 guid, uint8 method, uint64 kicker, const char* reason) // Kicker and Reason not a part of Mangos, implement?
{
    if (!sEluna.GroupEventBindings.BeginCall(GROUP_EVENT_ON_MEMBER_REMOVE))
        return;
    sEluna.Push(sEluna.L, group);
    sEluna.Push(sEluna.L, guid);
    sEluna.Push(sEluna.L, method);
    sEluna.Push(sEluna.L, kicker); // Kicker and Reason not a part of Mangos, implement?
    sEluna.Push(sEluna.L, reason);
    sEluna.GroupEventBindings.ExecuteCall();
    sEluna.GroupEventBindings.EndCall();
}

void HookMgr::OnChangeLeader(Group* group, uint64 newLeaderGuid, uint64 oldLeaderGuid)
{
    if (!sEluna.GroupEventBindings.BeginCall(GROUP_EVENT_ON_LEADER_CHANGE))
        return;
    sEluna.Push(sEluna.L, group);
    sEluna.Push(sEluna.L, newLeaderGuid);
    sEluna.Push(sEluna.L, oldLeaderGuid);
    sEluna.GroupEventBindings.ExecuteCall();
    sEluna.GroupEventBindings.EndCall();
}

void HookMgr::OnDisband(Group* group)
{
    if (!sEluna.GroupEventBindings.BeginCall(GROUP_EVENT_ON_DISBAND))
        return;
    sEluna.Push(sEluna.L, group);
    sEluna.GroupEventBindings.ExecuteCall();
    sEluna.GroupEventBindings.EndCall();
}

void HookMgr::OnCreate(Group* group, uint64 leaderGuid, GroupType groupType)
{
    if (!sEluna.GroupEventBindings.BeginCall(GROUP_EVENT_ON_CREATE))
        return;
    sEluna.Push(sEluna.L, group);
    sEluna.Push(sEluna.L, leaderGuid);
    sEluna.Push(sEluna.L, groupType);
    sEluna.GroupEventBindings.ExecuteCall();
    sEluna.GroupEventBindings.EndCall();
}

CreatureAI* HookMgr::GetAI(Creature* creature)
{
    if (!sEluna.CreatureEventBindings.GetBindMap(creature->GetEntry()))
        return NULL;
    return new ElunaCreatureAI(creature);
}

GameObjectAI* HookMgr::GetAI(GameObject* gameObject)
{
    if (!sEluna.GameObjectEventBindings.GetBindMap(gameObject->GetEntry()))
        return NULL;
    return new ElunaGameObjectAI(gameObject);
}

void AddElunaScripts()
{
    new ElunaWorldAI();
}
