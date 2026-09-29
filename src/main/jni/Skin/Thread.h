std::unordered_map<std::string, bool> sBool;
std::unordered_map<int, int> activeKillEffects;
std::unordered_map<int, int> activeBulletTrackEffects;
std::unordered_map<int, int> activeWeaponFireEffects;
std::unordered_map<int, int> activeWeaponBrocast;
std::vector<TargetChar> g_targetCharacters;
int g_selectedTargetCharIndex = 0;

std::vector<void *> itemInventoryInstance;
std::vector<void *> weaponExtraInstance;
std::vector<void *> weaponFireEffectInstance;
std::vector<void *> weaponConfInstance;
std::vector<void *> weaponAssetGroupInstance;
std::vector<void *> mythicArmorInstance;
std::vector<void *> mythicSightInstance;
std::vector<void *> killEffectItemInstance;
std::vector<void *> weaponSkinConfigInstance;
std::vector<void *> itemResourceConfigInstance;
std::vector<void *> CharacterModelConfigInstance;
std::vector<void *> RoleConfConfigInstance;
std::vector<void *> RoleSkinConfigInstance;
std::vector<void *> RolePackConfConfigInstance;
std::vector<void *> BRDeadboxSkinConfigInstance;
std::vector<void *> BRDropPlaneSkinConfigInstance;

std::vector<itemInfo> itemData;
std::vector<charInfo> charData;
std::vector<watcher> watch;
std::vector<deadbox> deadboxF;
std::vector<planeID> dropplane;

RoleSkinFields *roleskinFields;
CharacterModelFields *characterfields;
RolePackFields *packfields;
RoleConfFields *roleFields;
ItemResourceFields *itemFields;
WeaponConfFields *weaponconfFields;
Item2InventoryFields *item2Fields;
WeaponAssetGroupFields *weaponAssetFields;
WeaponFireEffectFields *weaponfireFields;
WeaponConfExtraFields *weaponextraFields;
MythicArmorFields *mythicarmorFields;
MythicSightFields *mythicsightFields;
KillEffectItemFields *killeffectFields;
BRDeadboxSkinFields *deadboxFields;
BRDropPlaneSkinFields *dropplaneFields;

uintptr_t Item2InventoryAddress = 0x6971D34;
uintptr_t WeaponConfAddress = 0xAF7E7FC;
uintptr_t WeaponConfExtraAddress = 0xAF80080;
uintptr_t WeaponFireEffectAddress = 0xAF81990;
uintptr_t CharacterModelAddress = 0xB1A9738;
uintptr_t BRDeadboxSkinAddress = 0x6FF3160;
uintptr_t BRDropPlaneSkinAddress = 0xB1981D4;
uintptr_t WeaponAssetGroupAddress = 0xAF7A080;
uintptr_t MythicArmorConfigAddress = 0x5C90E84;
uintptr_t MythicSightConfigAddress = 0x90DDEEC;
uintptr_t KillEffectItemConfConfigAddress = 0x90C54CC;
uintptr_t WeaponSkinAddress = 0xAF8B4D4;
uintptr_t ItemResourceAddress = 0x6985AB8;
uintptr_t RoleConfAddress = 0x90F20BC;
uintptr_t RoleSkinAddress = 0x90F95C8;
uintptr_t RolePackConfAddress = 0x90F784C;

uintptr_t WeaponConfName = 0xAF7FDC8;
uintptr_t GetDropPlaneName = 0xB198B08;
uintptr_t GetNameRoleSkin = 0x90F98F4;
uintptr_t GetRoleConfName = 0x90F4214;
uintptr_t GetDeadBoxIDAddress = 0x90F2CA0;

typedef void (*Item2InventoryCtor)(void*);
typedef void (*WeaponConfExtraCtor)(void*);
typedef void (*WeaponFireEffectCtor)(void*);
typedef void (*WeaponConfCtor)(void*);
typedef void (*WeaponAssetGroupCtor)(void*);
typedef void (*MythicArmorCtor)(void*);
typedef void (*MythicSightCtor)(void*);
typedef void (*WeaponSkinCtor)(void*);
typedef void (*KillEffectItemCtor)(void*);
typedef void (*ItemResourceCtor)(void*);
typedef void (*CharacterModelCtor)(void*);
typedef void (*RoleConfCtor)(void*);
typedef void (*RoleSkinCtor)(void*);
typedef void (*RolePackConfCtor)(void*);
typedef void (*BRDeadboxSkinCtor)(void*);
typedef void (*BRDropPlaneSkinCtor)(void*);

Item2InventoryCtor orig_Item2InventoryCtor = nullptr;
WeaponConfExtraCtor orig_WeaponConfExtraCtor = nullptr;
WeaponFireEffectCtor orig_WeaponFireEffectCtor = nullptr;
WeaponConfCtor orig_WeaponConfCtor = nullptr;
WeaponAssetGroupCtor orig_WeaponAssetGroupCtor = nullptr;
MythicArmorCtor orig_MythicArmorCtor = nullptr;
MythicSightCtor orig_MythicSightCtor = nullptr;
WeaponSkinCtor orig_WeaponSkinCtor = nullptr;
KillEffectItemCtor orig_KillEffectItemCtor = nullptr;
ItemResourceCtor orig_ItemResourceCtor = nullptr;
CharacterModelCtor orig_CharacterModelCtor = nullptr;
RoleConfCtor orig_RoleConfCtor = nullptr;
RoleSkinCtor orig_RoleSkinCtor = nullptr;
RolePackConfCtor orig_RolePackConfCtor = nullptr;
BRDeadboxSkinCtor orig_BRDeadboxSkinCtor = nullptr;
BRDropPlaneSkinCtor orig_BRDropPlaneSkinCtor = nullptr;

std::mutex g_dataMutex;

void my_Item2InventoryCtor(void* instance) {
    orig_Item2InventoryCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        itemInventoryInstance.push_back(instance);
    }
}

void my_WeaponConfExtraCtor(void* instance) {
    orig_WeaponConfExtraCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        weaponExtraInstance.push_back(instance);
    }
}

void my_WeaponFireEffectCtor(void* instance) {
    orig_WeaponFireEffectCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        weaponFireEffectInstance.push_back(instance);
    }
}

void my_WeaponConfCtor(void* instance) {
    orig_WeaponConfCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        weaponConfInstance.push_back(instance);
    }
}

void my_WeaponAssetGroupCtor(void* instance) {
    orig_WeaponAssetGroupCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        weaponAssetGroupInstance.push_back(instance);
    }
}

void my_MythicArmorCtor(void* instance) {
    orig_MythicArmorCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        mythicArmorInstance.push_back(instance);
    }
}

void my_MythicSightCtor(void* instance) {
    orig_MythicSightCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        mythicSightInstance.push_back(instance);
    }
}

void my_WeaponSkinCtor(void* instance) {
    orig_WeaponSkinCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        weaponSkinConfigInstance.push_back(instance);
    }
}

void my_KillEffectItemCtor(void* instance) {
    orig_KillEffectItemCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        killEffectItemInstance.push_back(instance);
    }
}

void my_ItemResourceCtor(void* instance) {
    orig_ItemResourceCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        itemResourceConfigInstance.push_back(instance);
    }
}

void my_CharacterModelCtor(void* instance) {
    orig_CharacterModelCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        CharacterModelConfigInstance.push_back(instance);
    }
}

void my_RoleConfCtor(void* instance) {
    orig_RoleConfCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        RoleConfConfigInstance.push_back(instance);
    }
}

void my_RoleSkinCtor(void* instance) {
    orig_RoleSkinCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        RoleSkinConfigInstance.push_back(instance);
    }
}

void my_RolePackConfCtor(void* instance) {
    orig_RolePackConfCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        RolePackConfConfigInstance.push_back(instance);
    }
}

void my_BRDeadboxSkinCtor(void* instance) {
    orig_BRDeadboxSkinCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        BRDeadboxSkinConfigInstance.push_back(instance);
    }
}

void my_BRDropPlaneSkinCtor(void* instance) {
    orig_BRDropPlaneSkinCtor(instance);
    if (instance) {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        BRDropPlaneSkinConfigInstance.push_back(instance);
    }
}

char searchQuery[256] = "";

bool loadskinhack = false;
bool loadCharacter = false;
bool isLoad = false;

std::string lastKnownName = "";
int emptyNameCount = 0;

std::unordered_map<std::string, int> nameCountMap;
std::unordered_map<std::string, int> nameCountChar;
std::unordered_map<std::string, bool> getplane;
std::unordered_map<std::string, bool> getguns;

uintptr_t location = 0;

uintptr_t getRealOffset(uintptr_t offset)
{
    while (location <= 0)
    {
        location = Tools::GetBaseAddress("libunity.so");
        if (location <= 0)
        {
            usleep(1000);
        }
    }
    return location + offset;
}

std::string GetNameString(uintptr_t off, void *getadd)
{
    auto getC = (String * (*)(void *))(getRealOffset(off));
    if (getadd && Tools::IsPtrValid((void *)getC))
    {
        auto getV = getC(getadd);
        if (getV && Tools::IsPtrValid(getV))
        {
            return getV->CString();
        }
    }
    return "";
}

int (*orig_GetCurrentWeaponKillEffect)(Weapon);
int _GetCurrentWeaponKillEffect(Weapon weapon) {
    Pawn* localPawn = GamePlay::get_LocalPawn();
    if (!localPawn) return 0;
    Weapon* currentWeapon = localPawn->get_CurrentWeapon();
    if (!currentWeapon) return 0;
    int currentID = currentWeapon->get_WeaponID();
    auto it = activeKillEffects.find(currentID);
    if (it != activeKillEffects.end()) {
        return it->second;
    }
    return 0;
}

int (*orig_GetCurrentBulletTrackEffect)(Weapon);
int _GetCurrentBulletTrackEffect(Weapon weapon) {
    Pawn* localPawn = GamePlay::get_LocalPawn();
    if (!localPawn) return 0;
    Weapon* currentWeapon = localPawn->get_CurrentWeapon();
    if (!currentWeapon) return 0;
    int currentID = currentWeapon->get_WeaponID();
    auto it = activeBulletTrackEffects.find(currentID);
    if (it != activeBulletTrackEffects.end()) {
        return it->second;
    }
    return 0;
}

int (*orig_GetCurrentWeaponFireEffect)(Weapon);
int _GetCurrentWeaponFireEffect(Weapon weapon) {
    Pawn* localPawn = GamePlay::get_LocalPawn();
    if (!localPawn) return 0;
    Weapon* currentWeapon = localPawn->get_CurrentWeapon();
    if (!currentWeapon) return 0;
    int currentID = currentWeapon->get_WeaponID();
    auto it = activeWeaponFireEffects.find(currentID);
    if (it != activeWeaponFireEffects.end()) {
        return it->second;
    }
    return 0;
}

int (*orig_GetCurrentWeaponBrocast)(Weapon);
int _GetCurrentWeaponBrocast(Weapon weapon) {
    Pawn* localPawn = GamePlay::get_LocalPawn();
    if (!localPawn) return 0;
    Weapon* currentWeapon = localPawn->get_CurrentWeapon();
    if (!currentWeapon) return 0;
    int currentID = currentWeapon->get_WeaponID();
    auto it = activeWeaponBrocast.find(currentID);
    if (it != activeWeaponBrocast.end()) {
        return it->second;
    }
    return 0;
}

#define ReadInt(base, offset) (*(int *)((uintptr_t)(base) + (offset)))
#define ReadBool(base, offset) (*(bool *)((uintptr_t)(base) + (offset)))
#define ReadFloat(base, offset) (*(float *)((uintptr_t)(base) + (offset)))
#define ReadByte(base, offset) (*(uint8_t *)((uintptr_t)(base) + (offset)))
#define READ_PTR(type, base, offset) (*(type **)((uintptr_t)(base) + (offset)))

inline std::string GetRarityPrefix(int colorID) {
    switch (colorID) {
        case 5: return "[M] ";
        case 4: return "[L] ";
        case 3: return "[E] ";
        default: return "[C] ";
    }
}

void LoadCharacterSkins() {
    if (loadCharacter) return;

    std::vector<void*> charModels, itemRes, roleConfs, roleSkins, rolePacks, deadboxSkins;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        charModels = CharacterModelConfigInstance;
        itemRes = itemResourceConfigInstance;
        roleConfs = RoleConfConfigInstance;
        roleSkins = RoleSkinConfigInstance;
        rolePacks = RolePackConfConfigInstance;
        deadboxSkins = BRDeadboxSkinConfigInstance;
    }

    if (charModels.empty() || itemRes.empty() || roleConfs.empty() ||
        roleSkins.empty() || rolePacks.empty() || deadboxSkins.empty()) {
        return;
    }

    std::unordered_map<uintptr_t, void*> itemResByAvatarModelID;
    std::unordered_map<int, void*>        rolePackByCharID;
    std::unordered_map<int, void*>        itemResByID;
    std::unordered_map<int, void*>        roleConfByID;
    std::unordered_map<int, void*>        rolePackByID;
    std::unordered_map<int, void*>        roleSkinByID;

    for (void* res : itemRes) {
        if (!res || !Tools::IsPtrValid(res)) continue;
        auto* fields = (ItemResourceFields*)((uintptr_t)res + 0x10);
        if (!Tools::IsPtrValid(fields)) continue;
        if (fields->AvatarModelID != 0)
            itemResByAvatarModelID[fields->AvatarModelID] = res;
        itemResByID[fields->ID] = res;
    }
    
    for (void* conf : roleConfs) {
        if (!conf || !Tools::IsPtrValid(conf)) continue;
        auto* fields = (RoleConfFields*)((uintptr_t)conf + 0x10);
        if (!Tools::IsPtrValid(fields)) continue;
        roleConfByID[fields->ID] = conf;
    }

    for (void* pack : rolePacks) {
        if (!pack || !Tools::IsPtrValid(pack)) continue;
        auto* packFields = (RolePackFields*)((uintptr_t)pack + 0x10);
        if (!Tools::IsPtrValid(packFields)) continue;
        rolePackByID[packFields->RolePackID] = pack;
        if (packFields->mMatchRoomEasterEggCharacterID != 0) {
            rolePackByCharID[packFields->mMatchRoomEasterEggCharacterID] = pack;
        }
    }

    for (void* skin : roleSkins) {
        if (!skin || !Tools::IsPtrValid(skin)) continue;
        auto* fields = (RoleSkinFields*)((uintptr_t)skin + 0x18);
        if (!Tools::IsPtrValid(fields)) continue;
        roleSkinByID[fields->ID] = skin;
        if (fields->FxAssetID_1P != 0) {
            std::string name = GetNameString(GetNameRoleSkin, skin);
            if (!name.empty()) {
                std::string prefix = GetRarityPrefix(fields->ColorID);
                watch.push_back({prefix + name, fields->FxAssetID_1P});
            }
        }
    }

    for (void* deadbox : deadboxSkins)
    {
        if (!deadbox || !Tools::IsPtrValid(deadbox))
            continue;
        auto* deadboxFields =
            (BRDeadboxSkinFields*)((uintptr_t)deadbox + 0x10);
        if (!Tools::IsPtrValid(deadboxFields))
            continue;
        for (void* roleConf : roleConfs)
        {
            if (!roleConf || !Tools::IsPtrValid(roleConf))
                continue;
            auto* roleFields =
                (RoleConfFields*)((uintptr_t)roleConf + 0x10);
            if (!Tools::IsPtrValid(roleFields))
                continue;
            if ((deadboxFields->ID & 0xFFFFFFF) == 0)
                continue;
            auto getDeadboxID =
                (int (*)(void*))(getRealOffset(GetDeadBoxIDAddress));
            if (!Tools::IsPtrValid((void*)getDeadboxID))
                continue;
            int roleDeadboxID = getDeadboxID(roleConf);
            if (deadboxFields->ID == roleDeadboxID)
            {
                std::string roleName =
                    GetNameString(GetRoleConfName, roleConf);
                std::string prefix =
                    GetRarityPrefix(deadboxFields->ColorID);
                deadboxF.push_back({
                    prefix + roleName,
                    {
                        deadboxFields->ColorID,
                        deadboxFields->DeadBoxEffectAsset,
                        deadboxFields->Flag,
                        deadboxFields->FlagAsset,
                        deadboxFields->ModelAsset3P,
                        deadboxFields->ModelAssetUI
                    }
                });
                break;
            }
        }
    }

    for (void* charModel : charModels) {
        if (!charModel || !Tools::IsPtrValid(charModel)) continue;
        auto* charFields = (CharacterModelFields*)((uintptr_t)charModel + 0x10);
        if (!Tools::IsPtrValid(charFields)) continue;
        
        uint64_t avatarModelID = charFields->ItemID;
        auto itRes = itemResByAvatarModelID.find(avatarModelID);
        if (itRes == itemResByAvatarModelID.end()) continue;
        void* itemResource = itRes->second;
        auto* itemFields = (ItemResourceFields*)((uintptr_t)itemResource + 0x10);
        if (!Tools::IsPtrValid(itemFields)) continue;

        int itemID = itemFields->ID;
        auto itRole = roleConfByID.find(itemID);
        if (itRole == roleConfByID.end()) continue;
        void* roleConf = itRole->second;
        auto* roleFields = (RoleConfFields*)((uintptr_t)roleConf + 0x10);
        if (!Tools::IsPtrValid(roleFields)) continue;
        
        int rolePackID = roleFields->RolePackID;
        int loadingFrame, entryAnimID, gestureId, handEffectUI, killStreakSkinID = 0;
        auto itPack = rolePackByCharID.find(itemID);
        if (itPack != rolePackByCharID.end()) {
            void* pack = itPack->second;
            auto* packFields = (RolePackFields*)((uintptr_t)pack + 0x10);
            if (Tools::IsPtrValid(packFields)) {
                loadingFrame = packFields->LoadingFrame;
                entryAnimID = packFields->EntryAnimID;
                gestureId = packFields->GestureId;
                handEffectUI = packFields->HandEffectUI;
                killStreakSkinID = packFields->KillStreakSkinID;
            }
        }

        std::string charName = GetNameString(GetRoleConfName, roleConf);
        std::string displayName = GetRarityPrefix(roleFields->ColorID) + charName;
        if (!charName.empty()) {
            charData.push_back({
                displayName,
                { charFields->BRBagModel, charFields->BRHeadModel, charFields->BRLobby, charFields->BRModel,
                charFields->BindEffect1P, charFields->ChangeClipEffect1P, charFields->DefaultModelID,
                charFields->Guarder1P, charFields->Guarder3P, charFields->GuarderBagModel,
                charFields->GuarderHeadModel, charFields->GuarderLobby },
                { itemFields->FxAssetID, itemFields->InventoryModelID, itemFields->ModelAssetIDRaw },
                { itemFields->UISmallSpriteName, itemFields->UIMiniSpriteName, itemFields->UISpriteName, itemFields->UISquareSpriteName },
                { roleFields->roleLeftArmID, roleFields->roleFinalSuitID, roleFields->roleBasicHologramID,
                roleFields->ColorID, roleFields->ColorSubID, roleFields->ShowRare, roleFields->RoleLvGroupID, roleFields->RolePackID },
                { roleFields->LOCID_Name },
                { rolePackID, entryAnimID, gestureId, handEffectUI, loadingFrame, killStreakSkinID }
            });
        }
        
        if (!charName.empty()) {
            g_targetCharacters.push_back({charName, charFields->Traitor1P, charFields->Traitor3P, itemFields->ID, (int)roleFields->ID, rolePackID });
        }
    }

    if (!g_targetCharacters.empty() && g_selectedTargetCharIndex == 0) {
        for (size_t i = 0; i < g_targetCharacters.size(); ++i) {
            if (ToLower(g_targetCharacters[i].name) == "charly") {
                g_selectedTargetCharIndex = (int)i;
                break;
            }
        }
    }

    loadCharacter = true;
}

void LoadWeaponSkins() {
    if (loadskinhack) return;

    std::vector<void*> weaponConfs, itemInvs, weaponAssets, weaponFires, weaponExtras, killEffects, mythicArmors, mythicSights, itemRes;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        weaponConfs = weaponConfInstance;
        itemInvs = itemInventoryInstance;
        weaponAssets = weaponAssetGroupInstance;
        weaponFires = weaponFireEffectInstance;
        weaponExtras = weaponExtraInstance;
        killEffects = killEffectItemInstance;
        mythicArmors = mythicArmorInstance;
        mythicSights = mythicSightInstance;
        itemRes = itemResourceConfigInstance;
    }

    if (weaponConfs.empty() || itemInvs.empty() || weaponAssets.empty() || weaponFires.empty() ||
        weaponExtras.empty() || killEffects.empty() || mythicArmors.empty() || mythicSights.empty() || itemRes.empty()) {
        return;
    }

    std::unordered_map<int, void*> itemInvByItemID;
    std::unordered_map<int, void*> weaponAssetByID;
    std::unordered_map<int, void*> weaponFireByID;
    std::unordered_map<int, void*> weaponExtraByID;
    std::unordered_map<int, int>   mythicArmorBySecondTab;
    std::unordered_map<int, int>   mythicSightByWeapon;
    std::unordered_map<int, int>   killEffectByWeapon;
    std::unordered_map<int, void*> itemResByID;

    for (void* inv : itemInvs) {
        if (!inv || !Tools::IsPtrValid(inv)) continue;
        int itemID = *(int*)((uintptr_t)inv + 0x20);
        itemInvByItemID[itemID] = inv;
    }

    for (void* asset : weaponAssets) {
        if (!asset || !Tools::IsPtrValid(asset)) continue;
        int id = *(int*)((uintptr_t)asset + 0x44);
        weaponAssetByID[id] = asset;
    }

    for (void* fire : weaponFires) {
        if (!fire || !Tools::IsPtrValid(fire)) continue;
        int id = *(int*)((uintptr_t)fire + 0x80);
        weaponFireByID[id] = fire;
    }

    for (void* extra : weaponExtras) {
        if (!extra || !Tools::IsPtrValid(extra)) continue;
        weaponextraFields = (WeaponConfExtraFields*)((uintptr_t)extra + 0x10);
        if (!Tools::IsPtrValid(weaponextraFields)) continue;
        weaponExtraByID[weaponextraFields->ID] = extra;
    }

    for (void* armor : mythicArmors) {
        if (!armor || !Tools::IsPtrValid(armor)) continue;
        mythicarmorFields = (MythicArmorFields*)((uintptr_t)armor + 0x14);
        if (!Tools::IsPtrValid(mythicarmorFields)) continue;
        if (mythicarmorFields->ThirdTab == 5) {
            mythicArmorBySecondTab[mythicarmorFields->SecondTab] = mythicarmorFields->AssetID;
        }
    }

    for (void* sight : mythicSights) {
        if (!sight || !Tools::IsPtrValid(sight)) continue;
        mythicsightFields = (MythicSightFields*)((uintptr_t)sight + 0x10);
        if (!Tools::IsPtrValid(mythicsightFields)) continue;
        auto* equipArray = *(Array<int>**)((uintptr_t)sight + 0x38);
        if (equipArray && Tools::IsPtrValid(equipArray) && equipArray->getLength() > 0) {
            int lastIndex = equipArray->getLength();
            int weaponID = equipArray->m_Items[lastIndex];
            int sightID = *(int*)((uintptr_t)sight + 0x14);
            mythicSightByWeapon[weaponID] = sightID;
        }
    }

    for (void* kill : killEffects) {
        if (!kill || !Tools::IsPtrValid(kill)) continue;
        killeffectFields = (KillEffectItemFields*)((uintptr_t)kill + 0x10);
        if (!Tools::IsPtrValid(killeffectFields)) continue;
        auto* equipArray = *(Array<int>**)((uintptr_t)kill + 0x90);
        if (equipArray && Tools::IsPtrValid(equipArray) && equipArray->getLength() > 0) {
            int lastIndex = equipArray->getLength();
            int weaponID = equipArray->m_Items[lastIndex];
            auto* realAssetIDs = *(Array<int>**)((uintptr_t)kill + 0x10);
            if (realAssetIDs && Tools::IsPtrValid(realAssetIDs) && realAssetIDs->getLength() > 0) {
                int lastAssetIndex = realAssetIDs->getLength();
                int lastAssetID = realAssetIDs->m_Items[lastAssetIndex];
                killEffectByWeapon[weaponID] = lastAssetID;
            }
        }
    }

    for (void* res : itemRes) {
        if (!res || !Tools::IsPtrValid(res)) continue;
        itemFields = (ItemResourceFields*)((uintptr_t)res + 0x10);
        if (!Tools::IsPtrValid(itemFields)) continue;
        itemResByID[itemFields->ID] = res;
    }

    for (void* conf : weaponConfs) {
        if (!conf || !Tools::IsPtrValid(conf)) continue;
        int baseID = *(int*)((uintptr_t)conf + 0x34);
        int confID = *(int*)((uintptr_t)conf + 0x40);

        auto itItem = itemInvByItemID.find(confID);
        if (itItem == itemInvByItemID.end()) continue;
        void* item = itItem->second;
        if (!item || !Tools::IsPtrValid(item)) continue;
        int itemIDbase = *(int*)((uintptr_t)item + 0x20);
        int itemBaseModified;
        int itemBase;
        if (baseID == itemIDbase) {
            itemBase = *(int*)((uintptr_t)item + 0x20);
            itemBaseModified = itemBase + 200;
        }
        if (confID == itemIDbase) {
            uint8_t confColorID = *(uint8_t*)((uintptr_t)conf + 0x22);
            int itemIDskin2 = *(int*)((uintptr_t)item + 0x24);
            int itemIDskin3 = *(int*)((uintptr_t)item + 0x28);

            std::string weaponName = GetNameString(WeaponConfName, conf);
            std::string displayName = GetRarityPrefix(confColorID) + weaponName;

            if (nameCountMap.find(displayName) != nameCountMap.end()) {
                nameCountMap[displayName]++;
                displayName += " +" + std::to_string(nameCountMap[displayName]);
            } else {
                nameCountMap[displayName] = 0;
            }

            if (!displayName.empty()) {
                int fireIds = 0, fireIds2 = 0, assetIds = 0;
                int originalFireID = 0;

                auto itAsset = weaponAssetByID.find(itemIDskin2);
                if (itAsset != weaponAssetByID.end()) {
                    void* asset = itAsset->second;
                    int fireEffectID = *(int*)((uintptr_t)asset + 0x40);
                    assetIds = itemIDskin2;

                    auto itFire = weaponFireByID.find(fireEffectID);
                    if (itFire != weaponFireByID.end()) {
                        void* fireConf = itFire->second;
                        int fireID = *(int*)((uintptr_t)fireConf + 0x80);
                        int assetIdBulletSmoke = *(int*)((uintptr_t)fireConf + 0x1C);

                        if (displayName.find("[M]") != std::string::npos) {
                            if (fireEffectID == fireID) {
                                if (assetIdBulletSmoke != 0) {
                                    fireIds = fireID;
                                    originalFireID = fireID;
                                } else {
                                    int nextFireID = fireID + 1;
                                    bool found = false;
                                    for (int i = 0; i < 10; i++) {
                                        auto itNext = weaponFireByID.find(nextFireID);
                                        if (itNext != weaponFireByID.end()) {
                                            void* nextFireConf = itNext->second;
                                            int nextAssetIdBulletSmoke = *(int*)((uintptr_t)nextFireConf + 0x1C);
                                            if (nextAssetIdBulletSmoke != 0) {
                                                fireIds = nextFireID;
                                                found = true;
                                                break;
                                            }
                                        }
                                        nextFireID++;
                                    }
                                    if (fireIds == 0) fireIds = fireID;
                                    fireIds2 = fireID;
                                }
                            }
                        }
                    }
                }

                int confBaseSkin = 0, confSkinID = 0, confBrocastID = 0, confColor = 0, confBluePrintID = 0;
                if (confID == itemIDbase) {
                    confBaseSkin = ReadInt(conf, 0x34);
                    confColor = ReadByte(conf, 0x22);
                    confSkinID = ReadInt(conf, 0x38);
                    confBrocastID = ReadInt(conf, 0x3C);
                    confBluePrintID = ReadByte(conf, 0x26);
                }

                int mythicArmor = 0, deadReplay = 0, killEffect = 0, extraOrig = 0;

                auto itExtraBase = weaponExtraByID.find(baseID);
                if (itExtraBase != weaponExtraByID.end()) {
                    void* extra = itExtraBase->second;
                    weaponextraFields = (WeaponConfExtraFields*)((uintptr_t)extra + 0x10);
                    if (Tools::IsPtrValid(weaponextraFields)) {
                        extraOrig = weaponextraFields->ID;
                    }
                }
                auto itExtraConf = weaponExtraByID.find(confID);
                if (itExtraConf != weaponExtraByID.end()) {
                    void* extra = itExtraConf->second;
                    weaponextraFields = (WeaponConfExtraFields*)((uintptr_t)extra + 0x10);
                    if (Tools::IsPtrValid(weaponextraFields)) {
                        deadReplay = weaponextraFields->DefaultDeadReplayEffectId;
                        killEffect = weaponextraFields->DefaultKillEffectId;
                    }
                }

                auto itMythicArmor = mythicArmorBySecondTab.find(itemIDskin3);
                if (itMythicArmor != mythicArmorBySecondTab.end()) {
                    if (displayName.find("[M]") != std::string::npos)
                        mythicArmor = itMythicArmor->second;
                }

                int sightMythic = 0;
                auto itMythicSight = mythicSightByWeapon.find(itemIDskin3);
                if (itMythicSight != mythicSightByWeapon.end()) {
                    if (displayName.find("[M]") != std::string::npos)
                        sightMythic = itMythicSight->second;
                }

                int killEffectFromItem = 0;
                auto itKillEffect = killEffectByWeapon.find(itemIDskin3);
                if (itKillEffect != killEffectByWeapon.end())
                    killEffectFromItem = itKillEffect->second;
                else {
                    itKillEffect = killEffectByWeapon.find(confID);
                    if (itKillEffect != killEffectByWeapon.end())
                        killEffectFromItem = itKillEffect->second;
                    else {
                        itKillEffect = killEffectByWeapon.find(baseID);
                        if (itKillEffect != killEffectByWeapon.end())
                            killEffectFromItem = itKillEffect->second;
                        else {
                            itKillEffect = killEffectByWeapon.find(itemBase);
                            if (itKillEffect != killEffectByWeapon.end())
                                killEffectFromItem = itKillEffect->second;
                        }
                    }
                }
                if (killEffectFromItem != 0)
                    killEffect = killEffectFromItem;

                void* spr1 = nullptr, *spr2 = nullptr, *spr3 = nullptr, *spr4 = nullptr;
                int xItem1 = 0, xItem2 = 0, xItem3 = 0;

                auto itItemRes = itemResByID.find(confID);
                if (itItemRes != itemResByID.end()) {
                    void* itemResource = itItemRes->second;
                    itemFields = (ItemResourceFields*)((uintptr_t)itemResource + 0x10);
                    if (Tools::IsPtrValid(itemFields)) {
                        xItem1 = itemFields->FxAssetID;
                        xItem2 = itemFields->InventoryModelID;
                        xItem3 = itemFields->ModelAssetIDRaw;
                        spr1 = itemFields->UISmallSpriteName;
                        spr2 = itemFields->UIMiniSpriteName;
                        spr3 = itemFields->UISpriteName;
                        spr4 = itemFields->UISquareSpriteName;
                    }
                }

                itemData.push_back({displayName,
                    {itemBase, itemIDskin2, itemIDskin3, itemBaseModified},
                    {confBaseSkin, confColor, confID, confBrocastID, confBluePrintID},
                    {extraOrig, mythicArmor, sightMythic, deadReplay, killEffect},
                    {assetIds, fireIds, fireIds2},
                    {xItem1, xItem2, xItem3},
                    {spr1, spr2, spr3, spr4}});
            }
        }
    }

    loadskinhack = true;
}

void LoadPlaneSkins() {
    std::vector<void*> dropPlaneSkins;
    {
        std::lock_guard<std::mutex> lock(g_dataMutex);
        dropPlaneSkins = BRDropPlaneSkinConfigInstance;
    }

    if (dropPlaneSkins.empty()) return;

    for (void* plane : dropPlaneSkins) {
        if (!plane || !Tools::IsPtrValid(plane)) continue;
        dropplaneFields = (BRDropPlaneSkinFields*)((uintptr_t)plane + 0x10);
        if (!Tools::IsPtrValid(dropplaneFields)) continue;
        std::string planeName = GetNameString(GetDropPlaneName, plane);
        if (dropplaneFields->ModelAsset1P != 0 && !getplane[planeName]) {
            getplane[planeName] = true;
            std::string prefix = GetRarityPrefix(dropplaneFields->ColorID);
            dropplane.push_back({prefix + planeName, {dropplaneFields->ColorID, dropplaneFields->ModelAsset1P, dropplaneFields->ModelAsset3P, dropplaneFields->ModelAssetCutScene, dropplaneFields->ModelAssetUI, dropplaneFields->Priority}});
            if (getplane.size() == dropPlaneSkins.size())
                break;
        }
    }
}

void Skins_Thread()
{
    while (!m_unity)
    {
        sleep(1);
    }

    Tools::Hook((void*)(m_unity + 0xA61AC70), (void*)_GetCurrentWeaponKillEffect, (void**)&orig_GetCurrentWeaponKillEffect);
    Tools::Hook((void*)(m_unity + 0xA61AAA8), (void*)_GetCurrentWeaponFireEffect, (void**)&orig_GetCurrentWeaponFireEffect);
    Tools::Hook((void*)(m_unity + 0xA61AB8C), (void*)_GetCurrentBulletTrackEffect, (void**)&orig_GetCurrentBulletTrackEffect);
    Tools::Hook((void*)(m_unity + 0xA61ADFC), (void*)_GetCurrentWeaponBrocast, (void**)&orig_GetCurrentWeaponBrocast);

    DobbyHook((void*)getRealOffset(Item2InventoryAddress), (void*)my_Item2InventoryCtor, (void**)&orig_Item2InventoryCtor);
    DobbyHook((void*)getRealOffset(WeaponConfExtraAddress), (void*)my_WeaponConfExtraCtor, (void**)&orig_WeaponConfExtraCtor);
    DobbyHook((void*)getRealOffset(WeaponFireEffectAddress), (void*)my_WeaponFireEffectCtor, (void**)&orig_WeaponFireEffectCtor);
    DobbyHook((void*)getRealOffset(WeaponConfAddress), (void*)my_WeaponConfCtor, (void**)&orig_WeaponConfCtor);
    DobbyHook((void*)getRealOffset(WeaponAssetGroupAddress), (void*)my_WeaponAssetGroupCtor, (void**)&orig_WeaponAssetGroupCtor);
    DobbyHook((void*)getRealOffset(MythicArmorConfigAddress), (void*)my_MythicArmorCtor, (void**)&orig_MythicArmorCtor);
    DobbyHook((void*)getRealOffset(MythicSightConfigAddress), (void*)my_MythicSightCtor, (void**)&orig_MythicSightCtor);
    DobbyHook((void*)getRealOffset(WeaponSkinAddress), (void*)my_WeaponSkinCtor, (void**)&orig_WeaponSkinCtor);
    DobbyHook((void*)getRealOffset(KillEffectItemConfConfigAddress), (void*)my_KillEffectItemCtor, (void**)&orig_KillEffectItemCtor);
    DobbyHook((void*)getRealOffset(ItemResourceAddress), (void*)my_ItemResourceCtor, (void**)&orig_ItemResourceCtor);
    DobbyHook((void*)getRealOffset(CharacterModelAddress), (void*)my_CharacterModelCtor, (void**)&orig_CharacterModelCtor);
    DobbyHook((void*)getRealOffset(RoleConfAddress), (void*)my_RoleConfCtor, (void**)&orig_RoleConfCtor);
    DobbyHook((void*)getRealOffset(RoleSkinAddress), (void*)my_RoleSkinCtor, (void**)&orig_RoleSkinCtor);
    DobbyHook((void*)getRealOffset(RolePackConfAddress), (void*)my_RolePackConfCtor, (void**)&orig_RolePackConfCtor);
    DobbyHook((void*)getRealOffset(BRDeadboxSkinAddress), (void*)my_BRDeadboxSkinCtor, (void**)&orig_BRDeadboxSkinCtor);
    DobbyHook((void*)getRealOffset(BRDropPlaneSkinAddress), (void*)my_BRDropPlaneSkinCtor, (void**)&orig_BRDropPlaneSkinCtor);

    while (!Tools::GetBaseAddress("libRoosterNN.so"))
    {
        sleep(1);
    }
    
    sleep(5);
    
    while (true)
    {
        LoadCharacterSkins();
        LoadWeaponSkins();
        LoadPlaneSkins();
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }
}