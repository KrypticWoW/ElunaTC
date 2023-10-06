#include <Chat.h>
#include <Item.h>
#include <ItemTemplate.h>
#include <Log.h>
#include <ObjectMgr.h>
#include <ScriptedGossip.h>
#include <SpellMgr.h>
#include <WorldSession.h>
#include <Bag.h>

enum TierGossipID
{
    TIER_ADD_GEAR = 0,
    TIER_ADD_WEAPON,
    TIER_ADD_BAGS,

    TIER_ADD_GEAR_TIER1,
    TIER_ADD_GEAR_TIER5,
    TIER_ADD_GEAR_TIER9,
    TIER_ADD_GEAR_TIER13,
    TIER_ADD_GEAR_TIER17,
    TIER_ADD_GEAR_TIER19,

    TIER_ADD_GEAR_TANK,
    TIER_ADD_GEAR_HEALER,
    TIER_ADD_GEAR_CASTER,
    TIER_ADD_GEAR_STR,
    TIER_ADD_GEAR_AGI,

    TIER_ADD_WEAPON_TIER1,
    TIER_ADD_WEAPON_TIER2,
    TIER_ADD_WEAPON_TIER3,
    TIER_ADD_WEAPON_TIER4,
    TIER_ADD_WEAPON_TIER5,
    TIER_ADD_WEAPON_TIER6,

    TIER_ADD_WEAPON_NEXT,
    TIER_ADD_WEAPON_PREV,
};

enum TierID
{
    Tier_1  = 1,
    Tier_5  = 5,
    Tier_9  = 9,
    Tier_13 = 13,
    Tier_17 = 17,
    Tier_19 = 19
};

enum Role
{
    ROLE_TANK = 0,
    ROLE_HEALER,
    ROLE_CASTER,
    ROLE_STR_DPS,
    ROLE_AGI_DPS
};

class TierTestNpc : public CreatureScript
{
public:
    TierTestNpc() : CreatureScript("TierTestNpc") {}

    struct TierTestNpc_AI : public ScriptedAI
    {
        TierTestNpc_AI(Creature* creature) : ScriptedAI(creature) {}

        bool ItemIsRemovable(Item* item)
        {
            if (!item)
                return false;

            auto iTemp = item->GetTemplate();

            if (iTemp->ItemId >= 61000 && iTemp->ItemId <= 64951) // Gear
                return true;
            if (iTemp->ItemId >= 65200 && iTemp->ItemId <= 65424) // Rings & Trinkets
                return true;

            return false;
        }

        void RemoveAllGear(Player* p)
        {
            uint32 count = 0;

            for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; i++)
                if (Item* pItem = p->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                    if (ItemIsRemovable(pItem))
                    {
                        count = pItem->GetCount();
                        p->DestroyItemCount(pItem, count, true);
                    }

            for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
                for (uint8 j = 0; j < MAX_BAG_SIZE; j++)
                    if (Item* pItem = p->GetItemByPos(((i << 8) | j)))
                        if (ItemIsRemovable(pItem))
                        {
                            count = pItem->GetCount();
                            p->DestroyItemCount(pItem, count, true);
                        }
        }

        void AddGear(Player* p, TierID tier, Role role)
        {
            RemoveAllGear(p);

            uint16 TierID = 1;
            int16 placeHolder = tier;

            while (placeHolder > 1)
            {
                placeHolder -=  4;
                TierID++;
            }

            uint16 ClassID = p->GetClass();
            uint32 ItemBaseID = 61000 + ((tier -1) * 200);
            std::vector<uint16> AllowedRoles;

            switch (ClassID)
            {
            case CLASS_WARRIOR:
                AllowedRoles = { ROLE_TANK, ROLE_STR_DPS };
                break;
            case CLASS_PALADIN:
                AllowedRoles = { ROLE_TANK, ROLE_HEALER, ROLE_STR_DPS };
                ItemBaseID += 16;
                break;
            case CLASS_HUNTER:
                AllowedRoles = { ROLE_AGI_DPS };
                ItemBaseID += 40;
                break;
            case CLASS_ROGUE:
                AllowedRoles = { ROLE_AGI_DPS };
                ItemBaseID += 48;
                break;
            case CLASS_PRIEST:
                AllowedRoles = { ROLE_HEALER, ROLE_CASTER };
                ItemBaseID += 56;
                break;
            case CLASS_DEATH_KNIGHT:
                AllowedRoles = { ROLE_TANK, ROLE_STR_DPS };
                ItemBaseID += 72;
                break;
            case CLASS_SHAMAN:
                AllowedRoles = { ROLE_HEALER, ROLE_CASTER, ROLE_AGI_DPS };
                ItemBaseID += 88;
                break;
            case CLASS_MAGE:
                AllowedRoles = { ROLE_CASTER };
                ItemBaseID += 112;
                break;
            case CLASS_WARLOCK:
                AllowedRoles = { ROLE_CASTER };
                ItemBaseID += 120;
                break;
            case CLASS_DRUID:
                AllowedRoles = { ROLE_TANK, ROLE_HEALER, ROLE_CASTER, ROLE_AGI_DPS };
                ItemBaseID += 128;
                break;
            }

            for (int i = 0; i < AllowedRoles.size(); i++)
            {
                if (role == AllowedRoles[i])
                {
                    if (ClassID == CLASS_DRUID && role == ROLE_AGI_DPS)
                        break;

                        ItemBaseID += i * 8;

                    break;
                }
            }

            p->EquipNewItem(EQUIPMENT_SLOT_HEAD, ItemBaseID, true);
            p->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, ItemBaseID + 1, true);
            p->EquipNewItem(EQUIPMENT_SLOT_CHEST, ItemBaseID + 2, true);
            p->EquipNewItem(EQUIPMENT_SLOT_WRISTS, ItemBaseID + 3, true);
            p->EquipNewItem(EQUIPMENT_SLOT_HANDS, ItemBaseID + 4, true);
            p->EquipNewItem(EQUIPMENT_SLOT_WAIST, ItemBaseID + 5, true);
            p->EquipNewItem(EQUIPMENT_SLOT_LEGS, ItemBaseID + 6, true);
            p->EquipNewItem(EQUIPMENT_SLOT_FEET, ItemBaseID + 7, true);

            uint32 TrinketID = 65400 + ((TierID - 1) * 5);
            uint32 RingBase = 65200 + ((TierID - 1) * 15);

            switch (role)
            {
            case ROLE_TANK:
                break;
            case ROLE_HEALER:
                TrinketID += 1;
                RingBase += 3;
                break;
            case ROLE_CASTER:
                TrinketID += 2;
                RingBase += 6;
                break;
            case ROLE_STR_DPS:
                TrinketID += 3;
                RingBase += 9;
                break;
            case ROLE_AGI_DPS:
                TrinketID += 4;
                RingBase += 12;
                break;
            }

            if (TierID <= 5)
            {
                p->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, TrinketID, true);
                p->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, TrinketID, true);
            }

            for (int i = 0; i < 3; i++)
                p->AddItem(RingBase + i, 1);
        }

        void AddBags(Player* p)
        {
            uint32 BagID = 23162;
            uint32 Count = p->GetItemCount(BagID, true);

            if (Count < 4)
                p->AddItem(BagID, 4 - Count);
        }

        bool OnGossipHello(Player* p) override
        {
            ClearGossipMenuFor(p);
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Add my gear.",    TIER_ADD_GEAR, 0);
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Add my weapons.", TIER_ADD_WEAPON, 0);
            AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Add my bags. ",   TIER_ADD_BAGS, 0);
            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            return true;
        }

        void SendGearChoiceGossip(Player* p)
        {
            std::vector<uint32> RoleMask = { 1059, 1106, 1488, 35, 1100 };

            if (RoleMask[0] & p->GetClassMask())
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "I'd like to tank.", TIER_ADD_GEAR_TANK, 0);
            if (RoleMask[1] & p->GetClassMask())
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "I'd like to heal.", TIER_ADD_GEAR_HEALER, 0);
            if (RoleMask[2] & p->GetClassMask())
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "I'd like to caster dps.", TIER_ADD_GEAR_CASTER, 0);
            if (RoleMask[3] & p->GetClassMask())
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "I'd like to str dps.", TIER_ADD_GEAR_STR, 0);
            if (RoleMask[4] & p->GetClassMask())
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "I'd like to agi dps.", TIER_ADD_GEAR_AGI, 0);
        }

        void SendWeaponChoiceGossip(Player* p, uint16 Tier, uint16 Page)
        {
            uint32 StartID = 65500 + (Tier * 100);
            uint32 EndID = StartID + 89;
            int Ctr = 0;
            int PageID = 0;

            const static uint32 item_weapon_skills[MAX_ITEM_SUBCLASS_WEAPON] =
            {
                SKILL_AXES,     SKILL_2H_AXES,  SKILL_BOWS,          SKILL_GUNS,      SKILL_MACES,
                SKILL_2H_MACES, SKILL_POLEARMS, SKILL_SWORDS,        SKILL_2H_SWORDS, 0,
                SKILL_STAVES,   0,              0,                   SKILL_FIST_WEAPONS,   0,
                SKILL_DAGGERS,  SKILL_THROWN,   SKILL_ASSASSINATION, SKILL_CROSSBOWS, SKILL_WANDS,
                SKILL_FISHING
            }; //Copy from function Item::GetSkill()

            for (uint32 i = StartID; i <= EndID; i++)
            {
                const ItemTemplate* temp = sObjectMgr->GetItemTemplate(i);
                
                if ((temp->AllowableClass & p->GetClassMask()) == 0 || (temp->AllowableRace & p->GetRaceMask()) == 0)
                    continue;
                if (temp->RequiredSkill > 0 && !(p->HasSkill(temp->RequiredSkill)))
                    continue;
                if (temp->Class == ITEM_CLASS_WEAPON && p->GetSkillValue(item_weapon_skills[temp->SubClass]) == 0)
                    continue;
                if (temp->InventoryType == INVTYPE_RELIC)
                {
                    if (temp->SubClass == ITEM_SUBCLASS_ARMOR_LIBRAM && p->GetClass() != CLASS_PALADIN)
                        continue;
                    if (temp->SubClass == ITEM_SUBCLASS_ARMOR_SIGIL && p->GetClass() != CLASS_DEATH_KNIGHT)
                        continue;
                    if (temp->SubClass == ITEM_SUBCLASS_ARMOR_TOTEM && p->GetClass() != CLASS_SHAMAN)
                        continue;
                    if (temp->SubClass == ITEM_SUBCLASS_ARMOR_IDOL && p->GetClass() != CLASS_DRUID)
                        continue;
                }

                if (Ctr == 30)
                {
                    if (PageID == Page)
                    {
                        AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Next Page", TIER_ADD_WEAPON_NEXT, CurPage + 1);
                    }
                    Ctr = 0;
                    PageID++;
                }
                else
                    Ctr++;

                if (PageID == Page)
                    AddGossipItemFor(p, GOSSIP_ICON_CHAT, temp->Name1, i, 0);
            }

            if (Page > 0)
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Previous Page", TIER_ADD_WEAPON_PREV, CurPage - 1);
        }

        bool OnGossipSelect(Player* p, uint32 menu_id, uint32 gossipListId) override
        {
            CurPage = GetGossipActionFor(p, gossipListId);

            uint32 sender = gossipListId == 999 ? menu_id : GetGossipSenderFor(p, gossipListId);
            ClearGossipMenuFor(p);

            std::vector<uint32> RoleMask = { 0, 0, 0, 0, 0 };

            if (sender > 60000)
            {
                p->AddItem(sender, 1);

                CloseGossipMenuFor(p);
                return true;
            }

            switch (sender)
            {
            case TIER_ADD_GEAR:
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Deadmines", TIER_ADD_GEAR_TIER1, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Zul'Farrak", TIER_ADD_GEAR_TIER5, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Shattered Halls", TIER_ADD_GEAR_TIER9, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Ahn'Kahet", TIER_ADD_GEAR_TIER13, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Halls of Stone", TIER_ADD_GEAR_TIER17, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Scarlet Monestary", TIER_ADD_GEAR_TIER19, 0);
                break;

            case TIER_ADD_WEAPON:
            {
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Deadmines", TIER_ADD_WEAPON_TIER1, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Zul'Farrak", TIER_ADD_WEAPON_TIER2, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Shattered Halls", TIER_ADD_WEAPON_TIER3, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Ahn'Kahet", TIER_ADD_WEAPON_TIER4, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Halls of Stone", TIER_ADD_WEAPON_TIER5, 0);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Scarlet Monestary", TIER_ADD_WEAPON_TIER6, 0);
            } break;

            case TIER_ADD_BAGS:
                AddBags(p);
                CloseGossipMenuFor(p);
                return true;
                break;

            case TIER_ADD_GEAR_TIER1:
                tier = Tier_1;
                SendGearChoiceGossip(p);
                break;

            case TIER_ADD_GEAR_TIER5:
                tier = Tier_5;
                SendGearChoiceGossip(p);
                break;

            case TIER_ADD_GEAR_TIER9:
                tier = Tier_9;
                SendGearChoiceGossip(p);
                break;

            case TIER_ADD_GEAR_TIER13:
                tier = Tier_13;
                SendGearChoiceGossip(p);
                break;

            case TIER_ADD_GEAR_TIER17:
                tier = Tier_17;
                SendGearChoiceGossip(p);
                break;

            case TIER_ADD_GEAR_TIER19:
                tier = Tier_19;
                SendGearChoiceGossip(p);
                break;

            case TIER_ADD_GEAR_TANK:
            case TIER_ADD_GEAR_HEALER:
            case TIER_ADD_GEAR_CASTER:
            case TIER_ADD_GEAR_STR:
            case TIER_ADD_GEAR_AGI:
                AddGear(p, tier, static_cast<Role>(sender - TIER_ADD_GEAR_TANK));
                CloseGossipMenuFor(p);
                return true;

            case TIER_ADD_WEAPON_TIER1:
                WeaponTier = 0;
                SendWeaponChoiceGossip(p, WeaponTier, CurPage);
                break;

            case TIER_ADD_WEAPON_TIER2:
                WeaponTier = 2;
                SendWeaponChoiceGossip(p, WeaponTier, CurPage);
                break;

            case TIER_ADD_WEAPON_TIER3:
                WeaponTier = 4;
                SendWeaponChoiceGossip(p, WeaponTier, CurPage);
                break;

            case TIER_ADD_WEAPON_TIER4:
                WeaponTier = 6;
                SendWeaponChoiceGossip(p, WeaponTier, CurPage);
                break;

            case TIER_ADD_WEAPON_TIER5:
                WeaponTier = 7;
                SendWeaponChoiceGossip(p, WeaponTier, CurPage);
                break;

            case TIER_ADD_WEAPON_TIER6:
                WeaponTier = 8;
                SendWeaponChoiceGossip(p, WeaponTier, CurPage);
                break;

            case TIER_ADD_WEAPON_NEXT:
            case TIER_ADD_WEAPON_PREV:
                SendWeaponChoiceGossip(p, WeaponTier, CurPage);
                break;
            }

            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            return true;
        }

        TierID tier = Tier_1;
        uint16 WeaponTier = 0;
        uint16 CurPage = 0;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new TierTestNpc_AI(creature);
    }
};

void AddSC_TierTestNpc()
{
    new TierTestNpc();
}
