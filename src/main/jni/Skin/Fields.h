extern uintptr_t Item2InventoryAddress;
extern uintptr_t WeaponConfAddress;
extern uintptr_t WeaponConfExtraAddress;
extern uintptr_t WeaponFireEffectAddress;
extern uintptr_t CharacterModelAddress;
extern uintptr_t BRDeadboxSkinAddress;
extern uintptr_t BRDropPlaneSkinAddress;
extern uintptr_t WeaponAssetGroupAddress;
extern uintptr_t MythicArmorConfigAddress;
extern uintptr_t MythicSightConfigAddress;
extern uintptr_t KillEffectItemConfConfigAddress;
extern uintptr_t WeaponSkinAddress;
extern uintptr_t ItemResourceAddress;
extern uintptr_t RoleConfAddress;
extern uintptr_t RoleSkinAddress;
extern uintptr_t RolePackConfAddress;
extern uintptr_t WeaponConfName;
extern uintptr_t GetDropPlaneName;
extern uintptr_t GetNameRoleSkin;
extern uintptr_t GetRoleConfName;
extern uintptr_t GetDeadBoxIDAddress;
extern std::vector<void *> itemInventoryInstance;
extern std::vector<void *> weaponExtraInstance;
extern std::vector<void *> weaponFireEffectInstance;
extern std::vector<void *> weaponConfInstance;
extern std::vector<void *> weaponAssetGroupInstance;
extern std::vector<void *> mythicArmorInstance;
extern std::vector<void *> mythicSightInstance;
extern std::vector<void *> killEffectItemInstance;
extern std::vector<void *> weaponSkinConfigInstance;
extern std::vector<void *> itemResourceConfigInstance;
extern std::vector<void *> CharacterModelConfigInstance;
extern std::vector<void *> RoleConfConfigInstance;
extern std::vector<void *> RoleSkinConfigInstance;
extern std::vector<void *> RolePackConfConfigInstance;
extern std::vector<void *> BRDeadboxSkinConfigInstance;
extern std::vector<void *> BRDropPlaneSkinConfigInstance;
extern std::unordered_map<std::string, bool> sBool;
extern std::unordered_map<int, int> activeKillEffects;
extern std::unordered_map<int, int> activeBulletTrackEffects;
extern std::unordered_map<int, int> activeWeaponFireEffects;
extern std::unordered_map<int, int> activeWeaponBrocast;
extern int g_selectedTargetCharIndex;

struct TargetChar {
    std::string name;
    int traitor1p;
    int traitor3p;
    int itemID;
    int roleID;
    int rolepackID;
};

extern std::vector<TargetChar> g_targetCharacters;

struct itemInfo
{
    std::string itemName;
    std::array<int, 4> Item2Inventory;
    std::array<int, 5> WeaponConf;
    std::array<int, 5> WeaponExtra;
    std::array<int, 3> WeaponAsset;
    std::array<int, 3> ItemResInt;
    std::array<void *, 4> ItemRes;
};
extern std::vector<itemInfo> itemData;

struct charInfo
{
    std::string charName;
    std::array<int, 12> charModel;
    std::array<int, 3> charRes;
    std::array<void *, 4> charRes2;
    std::array<int, 8> charRole;
    std::array<void *, 1> charRole2;
    std::array<int, 6> charPack;
};
extern std::vector<charInfo> charData;

struct watcher
{
    std::string watchname;
    int watchvalue;
};
extern std::vector<watcher> watch;

struct deadbox
{
    std::string deadname;
    std::array<int, 6> dead;
};
extern std::vector<deadbox> deadboxF;

struct planeID
{
    std::string planename;
    std::array<int, 6> plane;
};
extern std::vector<planeID> dropplane;

extern char searchQuery[256];
extern std::string ToLower(std::string str);
extern uintptr_t getRealOffset(uintptr_t offset);

struct RoleSkinFields
{
    uint8_t ClothType;
    uint8_t ColorID;
    uint8_t GoGetPos;
    uint8_t HatSubMode;
    uint8_t IsCanBeUsedForAllRole;
    bool IsCloseScreenSaver;
    uint8_t SecondTab;
    bool ShowInBag;
    bool ShowRare;
    int BRBagAssetID;
    int BRClothAssetID;
    int BRHatAssetID;
    int BRLeftArmAssetID;
    int BRSuitAssetID;
    int BRWingAssetID;
    int BagAssetID;
    int CamoID;
    int ClothAssetID;
    int FxAssetIDHandGestureUI;
    int FxAssetIDShow;
    int FxAssetID_1P;
    int FxAssetID_3P;
    int FxAssetID_UI;
    int HatAssetID;
    uint ID;
    int LeftArm1PAssetID;
    int LeftArmAssetID;
    int ShowBagAssetID;
    int ShowClothAssetID;
    int ShowHatAssetID;
    int SuitAssetID;
    int WingAssetID;
};
extern RoleSkinFields *roleskinFields;

struct CharacterModelFields
{
    bool Female;
    uint AvatarAnimationType;
    int BRBagModel;
    int BRHeadModel;
    int BRLobby;
    int BRModel;
    int BindEffect1P;
    int ChangeClipEffect1P;
    int DefaultModelID;
    int Guarder1P;
    int Guarder3P;
    int GuarderBagModel;
    int GuarderHeadModel;
    int GuarderLobby;
    int HeadIgnoreGUPSkin;
    int SpReceiveAnimType;
    int Traitor1P;
    int Traitor3P;
    float *BagOffset;
    string *Desc;
    float *VehicleOffset;
    int *VestOtherModelID;
    uintptr_t ItemID;
    uintptr_t LitePackDefault;
};
extern CharacterModelFields *characterfields;

struct RolePackFields
{
    int mMatchRoomEasterEggType;
    uint mMatchRoomEasterEggCharacterID;
    String *mMatchRoomEasterEggActionName;
    bool EntryDefaultAppearance;
    bool UseVFWFeature;
    int AirItem1;
    int AnimationWhenSell;
    int CabinIndivID;
    int DeadBoxID;
    int DynamicRoomAssetID;
    int EntryAnimID;
    int EntryUIEffectAssetID;
    int GestureId;
    int HandEffectUI;
    int KillStreakSkinID;
    int LoadingFrame;
    int LobbySceneType;
    int MVPIndivID;
    int MatchRoomSort;
    int OpenSceneIndivID;
    int PendantShowID;
    int PetID;
    int PetSwitchEffectAssetID;
    uint RolePackID;
    int SkillSkinID;
    int SkillSkinUI;
    int SpecialSceneFrameGroupAssetID;
    int VTOLShowAssetID;
    String *AirPoseBR;
    String *EntryAnim;
    String *EntryIdleAnim;
    void *EntryIdleItems;
    void *EntryItems;
    void *EntryItemsAnim;
    void *EntryItemsIdleAnim;
    String *EntryUIAnimationStateName;
};
extern RolePackFields *packfields;

struct RoleConfFields
{
    uint BaseCharacterID;
    uint roleLeftArmID;
    uint roleFinalSuitID;
    uint roleBasicHologramID;
    uint8_t ArchiveSwitch;
    uint8_t ArmorType;
    bool BlockBack;
    uint8_t ColorID;
    uint8_t ColorSubID;
    uint8_t DefaultPerk_1;
    uint8_t DefaultPerk_2;
    uint8_t DefaultPerk_3;
    bool DefaultShow;
    uint8_t DefaultSkill_1;
    bool ExternalUnVisible;
    bool IsVFW;
    uint8_t Loadout_Magnifier_Flag;
    uint8_t RoleLvGroupID;
    bool ShowInBag;
    uint8_t ShowOrder;
    bool ShowRare;
    uint ID;
    int JetPackInfoID;
    int KeepInCatalog;
    float RTPCMaxValue;
    int RolePackID;
    void *AudioSwitch;
    void *BRBankNameEX;
    void *BasicAudioBank;
    void *BasicAudioBank2;
    void *BasicAudioBank3;
    void *DefaultAudioBank;
    void *DefaultAudioBank2;
    void *DefaultAudioEventEx;
    void *FoleyAudioBank;
    void *FoleyAudioNameEx;
    void *LOCID_Desc;
    void *LOCID_Name;
    void *LitePackageKeyName;
    void *MPBankNameEx;
    void *PVEBankNameEX;
    void *RTPCName;
    void *RoleSkinBox;
    void *SpecialAudioBank;
    void *SpecialAudioBank2;
    void *SpecialAudioEventEx;
    void *SpecialTips;
    uint64_t WinnerCircle;
};
extern RoleConfFields *roleFields;

struct ItemResourceFields
{
    int FxAssetID;
    int ID;
    int InventoryModelID;
    int ModelAssetIDRaw;
    void *UIMiniSpriteName;
    void *UISmallSpriteName;
    void *UISpriteName;
    void *UISquareSpriteName;
    uint64_t AvatarModelID;
};
extern ItemResourceFields *itemFields;

struct WeaponConfFields
{
    uint8_t Accuracy;
    bool CanEquipOptic;
    uint8_t ColorID;
    uint8_t ColorSubID;
    uint8_t Control;
    uint8_t Damage;
    bool ExternalUnVisible;
    uint8_t FireRate;
    uint8_t LOCID_HowToGet;
    uint8_t MaxAttach;
    uint8_t MaxSkill;
    uint8_t Mobility;
    uint8_t Range;
    bool ShowRare;
    uint8_t SuperShowGroup;
    uint8_t WeaponBluePrintType;
    uint8_t WinnerCircleAnimID;
    uint BaseWeaponId;
    int DefWeaponSkinID;
    int DefaultKillBrocast;
    uint ID;
    int WinnerCircleWeaponMountType;
    void *EnableCamoBeta;
    void *LOCID_Desc;
    void *LOCID_Name;
    void *ModelRotation;
    void *UIProperty;
    void *UIPropertyUnlockLevel;
};
extern WeaponConfFields *weaponconfFields;

struct Item2InventoryFields
{
    int ItemID;
    int WeaponAssetGroupID;
    int WeaponIconID;
    void *AssetName;
    void *ConfigFilePathH;
    void *ConfigFilePath_BRH;
    void *ConfigFilePath_DMZH;
    void *ConfigFilePath_PVEH;
    void *WeaponNameH;
};
extern Item2InventoryFields *item2Fields;

struct WeaponAssetGroupFields
{
    int FireEffectGroupID;
    int Id;
    int MeshAssetID1P;
    int MeshAssetID3P;
    int MeshAssetID3P_L;
    int MeshAssetIDUI;
    int ProjAssetID;
    int ProjExplosionAssetID;
    int ProjFlightAssetID;
    int ProjFlightAssetID3P;
    int SecendBRProjExplosionAssetID;
    int SecendProjAssetID;
    int SecendProjExplosionAssetID;
    int SecendProjFlightAssetID;
    int SecendProjFlightAssetID3P;
    int SecondaryFireEffectGroupID;
    int SpecialUnAimingEffectAssetID1P;
    int UnusePupil;
    int WeaponPickUpAssetID;
    void *LeftLaserRotation;
    void *RightLaserRotation;
    void *ScopeDistancePartIDs;
    void *ScopeDistanceRatios;
};
extern WeaponAssetGroupFields *weaponAssetFields;

struct WeaponFireEffectFields
{
    int AssetIdFor1P;
    int AssetIdFor3P;
    int AssetIdForBulletSmoke1P;
    int AssetIdForBulletSmoke3P;
    int AssetIdForBulletTrack1P;
    int AssetIdForBulletTrack3P;
    int AssetIdForCharging1P;
    int AssetIdForCrossHair;
    int AssetIdForDecalEffect;
    int AssetIdForExtra1P;
    int AssetIdForExtraBulletSmoke1P;
    int AssetIdForExtraBulletSmoke3P;
    int AssetIdForHip1P;
    int AssetIdForHip3P;
    int AssetIdForHitEnemy;
    int AssetIdForInspectionScreenEffect;
    int AssetIdForSecondBulletSmoke1P;
    int AssetIdForSecondBulletSmoke3P;
    int AssetIdForSecondHip1P;
    int AssetIdForSecondHip3P;
    int AssetIdForShellDrop1P;
    int AssetIdForShellDrop3P;
    int AttachAssetIdFor1P;
    int AttachAssetIdFor3P;
    int AttachAssetIdForHip1P;
    int AttachAssetIdForHip3P;
    int DefaultKillEffectLevelUI;
    float HitEffectDelayDestroyTime;
    int HitEffectType;
    int Id;
    float LevelEffectDelayTimeUI;
    float LevelEffectDelayTimeUI_King;
    void *AssetIdForInspectionEffect;
    void *AssetIdForLevelEffects;
    void *AssetIdForLevelEffectsUI;
    void *AssetIdsForCircleBluntDecal;
    void *AssetIdsForCrustDecal;
    void *AssetIdsForExtraEffects;
    void *AssetIdsForExtraEffects3P;
    void *AssetIdsForExtraEffectsUI;
    void *AssetIdsForMeleeAttack;
    void *AssetIdsForMetalDecal;
    void *AssetIdsForShellDrop1P;
    void *AssetIdsForShellDrop3P;
    void *AssetIdsForStripBluntDecal;
    void *BRKillCountTimePlus;
    void *BRKillCountTimePlus_King;
    void *FireSoundExtend;
    void *HitEffectAssetIds;
    void *KillCountForLevelEffect;
    void *KillCountForLevelEffect_King;
    void *MPKillCountTimePlus;
    void *MPKillCountTimePlus_King;
    void *MeleeEffectDetachParent;
    void *SocketForInspectionEffect;
    void *SocketForLevelEffects;
    void *SocketForLevelEffectsUI;
    void *SocketNameForExtra1P;
    void *SocketsForExtraEffects;
    void *SocketsForExtraEffects3P;
    void *SocketsForExtraEffectsUI;
    void *SocketsForMeleeAttack;
    void *SocketsForShellDrop1P;
    void *SocketsForShellDrop3P;
};
extern WeaponFireEffectFields *weaponfireFields;

struct WeaponConfExtraFields
{
    uint8_t DefaultMythicArmor;
    uint8_t DefaultMythicSig;
    uint8_t GoGetPos;
    uint8_t SecondTab;
    uint8_t ThirdTab;
    int DefaultDeadReplayEffectId;
    int DefaultKillEffectId;
    int FireEffectGroupID;
    uint ID;
    void *GoGetUrl;
    void *IdleAnimationIndex;
    void *OpenViewSkill;
    void *OpenViewSkillUnlockLevel;
    void *PropertyModifys;
};
extern WeaponConfExtraFields *weaponextraFields;

struct MythicArmorFields
{
    int AssetID;
    int ColorID;
    int ColorSubID;
    int GoGetPos;
    uint ID;
    int MythicArmorLevel;
    int SecondTab;
    int ThirdTab;
    void *EquippedWeaponIcon;
    void *GoGetUrl;
    void *LOCID_Desc;
    void *LOCID_HowToGet;
    void *LOCID_Name;
    void *WeaponCanEquip;
};
extern MythicArmorFields *mythicarmorFields;

struct MythicSightFields
{
    bool ShowInBag;
    char pad_11[3];
    int AssetID;
    int ColorID;
    int ColorSubID;
    uint ID;
    int RelateMythicSight;
    void *LOCID_Desc;
    void *LOCID_Name;
    void *WeaponCanEquip;
};
extern MythicSightFields *mythicsightFields;

struct KillEffectItemFields
{
    void *RealAssetIDs;
    bool IsAutoDemolition;
    bool ShowInBag;
    char pad_1A[2];
    int ColorID;
    int ColorSubID;
    uint CurrencyID;
    int GoGetPos;
    uint ID;
    int SecondTab;
    int SellPrice;
    int SellProtection;
    int ThirdTab;
    void *AssetID;
    void *AssetID_CN;
    void *AssetID_GA;
    void *AssetID_KR;
    void *AssetID_VNG;
    void *GoGetUrl;
    void *KillCount;
    void *LOCID_Desc;
    void *LOCID_HowToGet;
    void *LOCID_Name;
    void *WeaponCanEquip;
};
extern KillEffectItemFields *killeffectFields;

struct BRDeadboxSkinFields
{
    int ColorID;
    int DeadBoxEffectAsset;
    int Flag;
    int FlagAsset;
    uint ID;
    int ModelAsset3P;
    int ModelAssetUI;
};
extern BRDeadboxSkinFields *deadboxFields;

struct BRDropPlaneSkinFields
{
    int ColorID;
    uint ID;
    int ModelAsset1P;
    int ModelAsset3P;
    int ModelAssetCutScene;
    int ModelAssetUI;
    int Priority;
};
extern BRDropPlaneSkinFields *dropplaneFields;
