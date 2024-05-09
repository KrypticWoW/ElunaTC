#include <World.h>
#include <Spell.h>
#include <Group.h>
#include <GameObject.h>
#include <GameObjectAI.h>
#include <ScriptedGossip.h>
#include <WorldSession.h>
#include <Chat.h>
#include <DatabaseEnv.h>
#include <Custom/MallArena/MallArena.h>
#include <Trainer.h>
#include <ObjectMgr.h>

class ArenaChallengerCreature : public CreatureScript
{
public:
    ArenaChallengerCreature() : CreatureScript("ArenaChallengerCreature") {}

    enum ArenaChallengeGossip
    {
        GOSSIP_CHALLENGE_DUEL = 0,
        GOSSIP_CHALLENGE_MAKGORA,
        GOSSIP_CHALLENGE_AMT,
        GOSSIP_CHALLENGE_REPLY_YES,
        GOSSIP_CHALLENGE_REPLY_NO,
        GOSSIP_VIEW_RESULTS,
        GOSSIP_RESULTS,
        GOSSIP_BACK,
        GOSSIP_EXIT
    };

    struct ArenaChallengerCreature_AI : public ScriptedAI
    {
        ArenaChallengerCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        void JustAppeared() override
        {
            me->setActive(true);
            sMAS.SetArenaCreature(me);
        }

        bool ChallengePlayer(Player* challenger, Player* opponent, bool makgora, uint32 gold)
        {
            me->Say("Challenge!", LANG_COMMON);
            sMAS.ChallengePlayer(challenger, opponent, makgora, gold);

            ClearGossipMenuFor(opponent);
            InitGossipMenuFor(opponent, ArenaChallengeMenu);
            if (makgora)
                AddGossipItemFor(opponent, GOSSIP_ICON_TRAINER, challenger->GetName() + " has challenged you to MakGora!. **|cffff0000WARNING|r** Makgora is a ritual duel, where if you die, there is no coming back.", GOSSIP_EXIT, 0, "Do you accept this challenge?.", 0, false);
            else
                AddGossipItemFor(opponent, GOSSIP_ICON_TRAINER, challenger->GetName() + " has challenged you to a duel." + (gold ? " Betting " + std::to_string(gold) + " Gold." : ""), GOSSIP_EXIT, 0, "Are you sure you want to decline?", 0, false);
            AddGossipItemFor(opponent, GOSSIP_ICON_TRAINER, "I accept the challenge", GOSSIP_CHALLENGE_REPLY_YES, 0, "Are you sure you want to accept?", 0, false);
            AddGossipItemFor(opponent, GOSSIP_ICON_TRAINER, "I decline the challenge", GOSSIP_CHALLENGE_REPLY_NO, 0, "Are you sure you want to decline?", 0, false);
            SendGossipMenuFor(opponent, DEFAULT_GOSSIP_MESSAGE, opponent->GetGUID());
            return true;
        }

        bool OnGossipHello(Player* p) override
        {
            ClearGossipMenuFor(p);

            if (sMAS.GetArenaState() == ARENA_STATE_INACTIVE)
            {
                uint32 Cooldown = sMAS.GetCooldown(p->GetGUID());

                if (Cooldown > 0)
                    AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "You can not challenge someone for another " + std::to_string(Cooldown / 1000) + " seconds.", GOSSIP_BACK, 0);
                else
                {
                    AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge Player to a Duel.", GOSSIP_CHALLENGE_DUEL, 0);
                    if (sMAS.AllowMakgora())
                        AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge Player to Mak'Gora!", GOSSIP_CHALLENGE_MAKGORA, 999, "Enter the players name you wish to challenge.", 0, true);
                }
            }
            else
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, sMAS.GetMemberName(true) + " has challenged " + sMAS.GetMemberName(false) + "to a duel.", GOSSIP_BACK, 0);

            if (sMAS.GetListSize() > 0)
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "View past results.", GOSSIP_VIEW_RESULTS, 0);

            AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Exit.", GOSSIP_EXIT, 0);

            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            return true;
        }

        bool OnGossipSelect(Player* p, uint32 /*menu_id*/, uint32 gossipListId) override
        {
            uint32 sender = GetGossipSenderFor(p, gossipListId);
            ClearGossipMenuFor(p);

            switch (sender)
            {

            case GOSSIP_CHALLENGE_DUEL:
            {
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge for Fun", GOSSIP_CHALLENGE_AMT, 0, "Enter the players name you wish to challenge.", 0, true);
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge for 50g", GOSSIP_CHALLENGE_AMT, 50, "Enter the players name you wish to challenge.", 0, true);
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge for 150g", GOSSIP_CHALLENGE_AMT, 150, "Enter the players name you wish to challenge.", 0, true);
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Challenge for 300g", GOSSIP_CHALLENGE_AMT, 300, "Enter the players name you wish to challenge.", 0, true);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case GOSSIP_VIEW_RESULTS:
            case GOSSIP_RESULTS:
            {
                for (auto& itr : sMAS.GetResults())
                    AddGossipItemFor(p, GOSSIP_ICON_TRAINER, itr, GOSSIP_RESULTS, 0);

                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Back", GOSSIP_BACK, 0);
                SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);
            } break;

            case GOSSIP_BACK:
            {
                OnGossipHello(p);
            } break;

            case GOSSIP_EXIT:
            {
                if (p == sMAS.GetPlayer(false))
                    sMAS.DeclineChallenge();

                CloseGossipMenuFor(p);
            } break;

            }

            return true;

        }

        bool OnGossipSelectCode(Player* p, uint32 menu_id, uint32 gossipListId, char const* code) override
        {
            uint32 sender = (gossipListId == 999 ? menu_id : p->PlayerTalkClass->GetGossipOptionSender(gossipListId));
            uint32 action = GetGossipActionFor(p, gossipListId);
            if (action == 999) action = 0;
            uint32 Wager = action * 10000;
            std::cout << sender << " : " << action << " : " << Wager << std::endl;
            ClearGossipMenuFor(p);
            CloseGossipMenuFor(p);

            std::string playerName = code;
            Player* target = ObjectAccessor::FindConnectedPlayerByName(playerName);
            if (!target)
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to challenge " + playerName + ".");
                return false;
            }

            if (target->IsLoading() || !target->IsInWorld())
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to challenge " + playerName + ".");
                return false;
            }

            if (!target->HasEnoughMoney(Wager) || target->IsInCombat())
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to challenge " + playerName + ".");
                return false;
            }

            if (!p->HasEnoughMoney(Wager))
            {
                ChatHandler(p->GetSession()).SendSysMessage("You don't have enough money.");
                return false;
            }

            if (target->GetZoneId() != me->GetZoneId())
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to challenge " + playerName + ", they are in a different zone.");
                return false;
            }

            if (target->IsInSameGroupWith(p))
            {
                ChatHandler(p->GetSession()).SendSysMessage("Unable to challenge " + playerName + ", You are both in the same group.");
                return false;
            }

            ChallengePlayer(p, target, sender == GOSSIP_CHALLENGE_MAKGORA, action);

            return true;
        }

        void UpdateAI(uint32 diff) override
        {
            sMAS.Update(diff);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new ArenaChallengerCreature_AI(creature);
    }
};

class ClassTrainerCreature : public CreatureScript
{
public:
    ClassTrainerCreature() : CreatureScript("ClassTrainerCreature") {}

    struct ClassTrainerCreature_AI : public ScriptedAI
    {
        ClassTrainerCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        bool OnGossipHello(Player* p) override
        {
            ClearGossipMenuFor(p);

            uint16 Trainer = me->GetEntry() - 50003;
            if (Trainer == 10)
                Trainer = 11;

            if (p->GetClass() == Trainer)
            {
                AddGossipItemFor(p, GOSSIP_ICON_TRAINER, "Train me.", 0, 0);
                AddGossipItemFor(p, GOSSIP_ICON_TAXI, "I wish to unlearn my talents.", 1, 0);
                if (p->GetSpecsCount() == 1)
                    AddGossipItemFor(p, GOSSIP_ICON_TAXI, "I wish to learn Dual Specialization. ", 2, 0, "", 10000000, false);
                SendGossipMenuFor(p, me->GetEntry() + 9996, me);
            }
            else
                SendGossipMenuFor(p, 60010, me);

            return true;
        }

        bool OnGossipSelect(Player* p, uint32 /*menu_id*/, uint32 gossipListId) override
        {
            uint32 sender = GetGossipSenderFor(p, gossipListId);
            ClearGossipMenuFor(p);

            switch (sender)
            {
            case 0:
                p->GetSession()->SendTrainerList(me);
                break;
            case 1:
                p->SendTalentWipeConfirm(me->GetGUID());
                break;
            case 2:
                if (p->HasEnoughMoney(10000000))
                {
                    p->ModifyMoney(-10000000);
                    p->CastSpell(p, 63680, p->GetGUID());
                    p->CastSpell(p, 63624, p->GetGUID());
                }
                break;
            }

            CloseGossipMenuFor(p);
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new ClassTrainerCreature_AI(creature);
    }
};

class MariaLucenteCreature : public CreatureScript
{
public:
    MariaLucenteCreature() : CreatureScript("MariaLucenteCreature") {}

    struct MariaLucenteCreature_AI : public ScriptedAI
    {
        MariaLucenteCreature_AI(Creature* creature) : ScriptedAI(creature) {}

        bool OnGossipHello(Player* p) override
        {
            ClearGossipMenuFor(p);

            if (p->HasItemCount(60011))
            {
                AddGossipItemFor(p, GOSSIP_ICON_TAXI, "Oh....", GOSSIP_SENDER_MAIN, 0, "What am I?", 0, true);
                AddGossipItemFor(p, GOSSIP_ICON_CHAT, "Nevermind", GOSSIP_SENDER_MAIN, 1);
            }

            SendGossipMenuFor(p, DEFAULT_GOSSIP_MESSAGE, me);

            return true;
        }

        bool OnGossipSelect(Player* p, uint32 /*menu_id*/, uint32 /*gossipListId*/) override
        {
            CloseGossipMenuFor(p);
            return true;
        }

        bool OnGossipSelectCode(Player* p, uint32 /*menu_id*/, uint32 /*gossipListId*/, char const* code) override
        {
            ClearGossipMenuFor(p);
            std::string message = code;

            for (int i = 0; i < message.size(); i++)
                message[i] = std::tolower(message[i]);

            if (message == "pen")
                me->Say("Reward", LANG_UNIVERSAL);

            CloseGossipMenuFor(p);
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new MariaLucenteCreature_AI(creature);
    }
};

class custom_go_banker : public GameObjectScript
{
public:
    custom_go_banker() : GameObjectScript("custom_go_banker") { }

    struct custom_go_bankerAI : public GameObjectAI
    {
        custom_go_bankerAI(GameObject* go) : GameObjectAI(go) { }

        bool OnGossipHello(Player* player) override
        {
            player->GetSession()->SendShowBank(me->GetGUID());
            return true;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new custom_go_bankerAI(go);
    }
};

class custom_prof_trainer : public CreatureScript
{
public:
    custom_prof_trainer() : CreatureScript("custom_prof_trainer") { }

    struct custom_prof_trainerAI : public ScriptedAI
    {
        custom_prof_trainerAI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            ClearGossipMenuFor(player);

            AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface/ICONS/trade_alchemy:22:22:-26:0|tTrain Alchemy|r", 0, TRADESKILL_ALCHEMY);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface/ICONS/trade_blacksmithing:22:22:-26:0|tTrain Blacksmithing|r", 0, TRADESKILL_BLACKSMITHING);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface/ICONS/trade_engraving:22:22:-26:0|tTrain Enchanting|r", 0, TRADESKILL_ENCHANTING);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface/ICONS/trade_engineering:22:22:-26:0|tTrain Engineering|r", 0, TRADESKILL_ENGINEERING);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface/ICONS/inv_inscription_tradeskill01:22:22:-26:0|tTrain Inscription|r", 0, TRADESKILL_INSCRIPTION);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface/ICONS/inv_misc_gem_01:22:22:-26:0|tTrain Jewelcrafting|r", 0, TRADESKILL_JEWLCRAFTING);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface/ICONS/trade_leatherworking:22:22:-26:0|tTrain Leatherworking|r", 0, TRADESKILL_LEATHERWORKING);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface/ICONS/trade_tailoring:22:22:-26:0|tTrain Tailoring|r", 0, TRADESKILL_TAILORING);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface/ICONS/trade_herbalism:22:22:-26:0|tTrain Herbalism|r", 0, TRADESKILL_HERBALISM);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface/ICONS/trade_mining:22:22:-26:0|tTrain Mining|r", 0, TRADESKILL_MINING);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface/ICONS/Inv_misc_pelt_wolf_01:22:22:-26:0|tTrain Skinning|r", 0, TRADESKILL_SKINNING);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface/ICONS/inv_misc_food_15:22:22:-26:0|tTrain Cooking|r", 0, TRADESKILL_COOKING);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "|TInterface/ICONS/Spell_holy_sealofsacrifice:22:22:-26:0|tTrain First Aid|r", 0, TRADESKILL_FIRSTAID);

            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
        {
            uint32 Action = GetGossipActionFor(player, gossipListId);
            ClearGossipMenuFor(player);
            uint32 TrainerID = 0;

            switch (Action)
            {
            case TRADESKILL_ALCHEMY: TrainerID = 28703; break;
            case TRADESKILL_BLACKSMITHING: TrainerID = 28694; break;
            case TRADESKILL_ENCHANTING: TrainerID = 28693; break;
            case TRADESKILL_ENGINEERING: TrainerID = 28697; break;
            case TRADESKILL_INSCRIPTION: TrainerID = 28702; break;
            case TRADESKILL_JEWLCRAFTING: TrainerID = 28701; break;
            case TRADESKILL_LEATHERWORKING: TrainerID = 28700; break;
            case TRADESKILL_TAILORING: TrainerID = 28699; break;
            case TRADESKILL_HERBALISM: TrainerID = 28704; break;
            case TRADESKILL_MINING: TrainerID = 28698; break;
            case TRADESKILL_SKINNING: TrainerID = 28696; break;
            case TRADESKILL_COOKING: TrainerID = 28705; break;
            case TRADESKILL_FIRSTAID: TrainerID = 28706; break;
            }

            Trainer::Trainer const* trainer = sObjectMgr->GetTrainer(TrainerID);
            if (trainer)
                trainer->SendSpells(me, player, player->GetSession()->GetSessionDbLocaleIndex());

            CloseGossipMenuFor(player);

            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_prof_trainerAI(creature);
    }
};

class custom_cataclysm_transmog_vendor : public CreatureScript
{
public:
    custom_cataclysm_transmog_vendor() : CreatureScript("custom_cataclysm_transmog_vendor") { }

    struct custom_cataclysm_transmog_vendorAI : public ScriptedAI
    {
        custom_cataclysm_transmog_vendorAI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            ClearGossipMenuFor(player);

            AddGossipItemFor(player, GOSSIP_ICON_DOT, "Tier 11 Raid Gear", 0, 0);
            AddGossipItemFor(player, GOSSIP_ICON_DOT, "Tier 12 Raid Gear", 0, 1);

            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
        {
            uint32 Action = GetGossipActionFor(player, gossipListId);
            ClearGossipMenuFor(player);
            uint32 VendorID = 0;

            switch (Action)
            {
            case 0: VendorID = 50056; break;
            case 1: VendorID = 50057; break;
            }

            player->GetSession()->SendListInventory(me->GetGUID(), VendorID);
            CloseGossipMenuFor(player);

            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_cataclysm_transmog_vendorAI(creature);
    }
};

void AddSC_MallScripts()
{
    new custom_go_banker();
    new custom_prof_trainer();
    new ClassTrainerCreature();
    new MariaLucenteCreature();
    new ArenaChallengerCreature();
    new custom_cataclysm_transmog_vendor();
}
