#ifndef TELEPORTSYSTEM_H
#define TELEPORTSYSTEM_H

struct TeleItem
{
    bool operator<(const TeleItem& other) const { return ID < other.ID; }
    bool HasChilds() const { return !Childs.empty(); }

    TeleItem* Search(int id)
    {
        if (ID == id)
            return this;
        for (auto& item : Childs)
        {
            auto ret = item.second.Search(id);
            if (ret)
                return ret;
        }
        return nullptr;
    }

    int32 ID;
    std::string Name;
    int32 Parent;
    int32 MapID;
    float PosX;
    float PosY;
    float PosZ;
    float PosO;
    uint8 MinLevel;
    uint8 MaxLevel;
    int32 RequiredFaction;
    int32 RequiredClass;
    int32 RequiredQuest;
    int32 RequiredAchievement;
    int32 RequiredSpell;
    uint8 RequiredSecurity;
    uint8 Option;
    int32 ReturnId;
    std::string BoxText;

    std::map<decltype(ID), TeleItem> Childs;
};

class TeleSystem
{
    TeleSystem() {}

public:
    static TeleSystem& instance()
    {
        static TeleSystem tpsys;
        return tpsys;
    }

    auto& GetLocations() { return m_Locations; }

    void Load();

private:
    std::map<decltype(TeleItem::ID), TeleItem> m_Locations;
};

#define sTeleSystem TeleSystem::instance()

#endif
