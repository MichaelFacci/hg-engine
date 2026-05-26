#include "../../include/types.h"
#include "../../include/config.h"
#include "../../include/battle.h"
#include "../../include/item.h"
#include "../../include/mega.h"
#include "../../include/pokemon.h"
#include "../../include/constants/ability.h"
#include "../../include/constants/battle_script_constants.h"
#include "../../include/constants/file.h"
#include "../../include/constants/item.h"
#include "../../include/constants/moves.h"
#include "../../include/constants/move_effects.h"
#include "../../include/constants/species.h"
#include "../../include/constants/system_control.h"
#include "../../include/overlay.h"

#ifdef DEBUG_BATTLE_SCENARIOS
#include "../../include/test_battle.h"
#endif // DEBUG_BATTLE_SCENARIOS

/********************************************************************************************************************/
/********************************************************************************************************************/
//                                                战斗前准备
/********************************************************************************************************************/
/********************************************************************************************************************/
// bubble lives on in the repository o7
// would not be here without him

/**
 *  @brief initialize the global battle structure and return it
 *
 *  @param bw battle work structure
 */
struct BattleStruct *ServerInit(struct BattleSystem *bw) {
    struct BattleStruct *sp;

    sp = sys_AllocMemory(HEAPID_BATTLE_HEAP, sizeof(struct BattleStruct));
    memset(sp, 0, sizeof(struct BattleStruct));
    BattleStructureInit(sp);
    BattleStructureCounterInit(bw, sp);
    ServerMoveAIInit(bw, sp);
    DumpMoveTableData(&sp->moveTbl[0]);
    sp->aiWorkTable.item = ItemDataTableLoad(HEAPID_BATTLE_HEAP);

#ifdef RESTORE_ITEMS_AT_BATTLE_END

    // store items for the player's party in sp so we can restore them at the end
    struct Party *party = SaveData_GetPlayerPartyPtr(SaveBlock2_get());
    for (int i = 0; i < party->count; i++) {
        newBS.itemsToRestore[i] = GetMonData(Party_GetMonByIndex(party, i), MON_DATA_HELD_ITEM, NULL);
    }

#endif  // RESTORE_ITEMS_AT_BATTLE_END

    sp->original_terrain = bw->terrain;
    sp->original_bgId = bw->bgId;
    bw->sp = sp;
    BattleBgExpansionLoader(bw);

    gBattleSystem = bw;

#ifdef DEBUG_BATTLE_SCENARIOS
    // Apply test battle state (recharge, charging, etc.)
    TestBattle_ApplyBattleState(sp);
#endif

    return sp;
}


/**
 *  @brief actions that take precedence over all moves
 *         loads in battle sub script and queues it up by setting server_seq_no to 22
 *
 *  @param bw battle work structure
 *  @param sp global battle structure
 */
u32 ServerBeforeAct_restoreOverlay = 0;

void ServerBeforeAct(struct BattleSystem *bsys, struct BattleStruct *ctx) {
    u32 ovyId = OVERLAY_SERVERBEFOREACT, offset = 0x021FD900 | 1;

    void (*internalFunc)(struct BattleSystem *bsys, struct BattleStruct *ctx);

    // if ctx->sba_seq_no == SBA_RESET_DEFIANT before func is called, it is the first call
    //if (ctx->sba_seq_no == SBA_RESET_DEFIANT)
    {
        if (IsOverlayLoaded(OVERLAY_WIFI)) { // we are taking overlay 0's place
            ServerBeforeAct_restoreOverlay = TRUE;
            UnloadOverlayByID(OVERLAY_WIFI);
        } else if (IsOverlayLoaded(OVERLAY_POKEDEX)) {
            ServerBeforeAct_restoreOverlay = OVERLAY_POKEDEX;
            UnloadOverlayByID(18);
        }

//#ifdef DEBUG_BEFORE_MOVE_LOGIC
//        debug_printf("Load OVERLAY_SERVERBEFOREACT\n");
//#endif

        HandleLoadOverlay(ovyId, 2);
    }
    internalFunc = (void (*)(struct BattleSystem *bsys, struct BattleStruct *ctx))(offset);
    internalFunc(bsys, ctx);

    //if (ctx->sba_seq_no == SBA_RESET_DEFIANT)
    {
        if (ServerBeforeAct_restoreOverlay) {
            UnloadOverlayByID(ovyId);
//#ifdef DEBUG_BEFORE_MOVE_LOGIC
//            debug_printf("Restoring overlay %d...\n", (ServerBeforeAct_restoreOverlay == 1 ? 0 : ServerBeforeAct_restoreOverlay));
//#endif
            HandleLoadOverlay((ServerBeforeAct_restoreOverlay == 1 ? OVERLAY_WIFI : ServerBeforeAct_restoreOverlay), 2);
        }
    }
}


/********************************************************************************************************************/
/********************************************************************************************************************/
//                                                技能使用前判断
/********************************************************************************************************************/
/********************************************************************************************************************/

// 08014ACC

/**
 *  @brief run through everything before any of the moves are used
 *         modified for protean and stance change and megas
 *
 *  @param bw battle work structure
 *  @param sp global battle structure
 */
u32 ServerWazaBefore_restoreOverlay = 0;

void ServerWazaBefore(void *bw, struct BattleStruct *sp) {
//#ifdef DEBUG_BEFORE_MOVE_LOGIC
//    debug_printf("In BattleController_BeforeMove landing pad\n");
//#endif
    u32 ovyId, offset;

    void (*internalFunc)(void *bw, struct BattleStruct *sp);

    // if wb_seq_no == BEFORE_MOVE_START_FLAG_UNLOAD before func is called, it is the first call
    if (sp->wb_seq_no == BEFORE_MOVE_START_FLAG_UNLOAD)
    {
        if (IsOverlayLoaded(OVERLAY_WIFI)) { // we are taking overlay 0's place
            ServerWazaBefore_restoreOverlay = TRUE;
            UnloadOverlayByID(OVERLAY_WIFI);
        } else if (IsOverlayLoaded(OVERLAY_POKEDEX)) {
            ServerWazaBefore_restoreOverlay = OVERLAY_POKEDEX;
            UnloadOverlayByID(OVERLAY_POKEDEX);
        }

//#ifdef DEBUG_BEFORE_MOVE_LOGIC
//        debug_printf("Load OVERLAY_BATTLECONTROLLER_BEFOREMOVE\n");
//#endif
        ovyId = OVERLAY_BATTLECONTROLLER_BEFOREMOVE;

        //offset = 0x023C0400 | 1;
        HandleLoadOverlay(ovyId, 2);
    }

    offset = 0x021E5900 | 1;
    internalFunc = (void (*)(void *bw, struct BattleStruct *sp))(offset);
    internalFunc(bw, sp);

    // if wb_seq_no == BEFORE_MOVE_START_FLAG_UNLOAD after the func is called, it is the last call
    if (sp->wb_seq_no == BEFORE_MOVE_START_FLAG_UNLOAD)
    {
        // needs to unload regardless of if it took OVERLAY_POKEDEX space
        UnloadOverlayByID(ovyId);
        if (ServerWazaBefore_restoreOverlay) {
//#ifdef DEBUG_BEFORE_MOVE_LOGIC
//            debug_printf("Restoring overlay %d...\n", (ServerWazaBefore_restoreOverlay == 1 ? 0 : ServerWazaBefore_restoreOverlay));
//#endif
            HandleLoadOverlay((ServerWazaBefore_restoreOverlay == 1 ? OVERLAY_WIFI : ServerWazaBefore_restoreOverlay), 2);
            ServerWazaBefore_restoreOverlay = FALSE;
        }
    }
}



int LONG_CALL BattleSystem_GetBackgroundId(struct BattleSystem *bsys){

    debug_printf("BattleSystem_GetBackgroundId called, returning %d\n", bsys->bgId);
    return bsys->bgId;

}

void LONG_CALL BattleSystem_SetBackground_hook(int param1) {
    debug_printf("BattleSystem_SetBackground called with param1 = %d\n", param1);
    BattleSystem_SetBackground_Original(param1);
}


/********************************************************************************************************************/
/********************************************************************************************************************/
//                                            3D Battle Background
/********************************************************************************************************************/
/********************************************************************************************************************/
 
// ---------------------------------------------------------------------------
// Fixed-point macros (not in types.h)
// ---------------------------------------------------------------------------
#define FX32_ONE            (1 << FX32_SHIFT)
#define FX32_CONST(x)       ((fx32)((x) * FX32_ONE))
#define FX_DEG_TO_IDX(x)    ((u16)((u32)((x) * 65536.0 / 360.0)))
 
// ---------------------------------------------------------------------------
// 3D hardware registers
// ---------------------------------------------------------------------------
#define REG_DISP3DCNT       (*(vu16*)0x04000060)
#define REG_GXSTAT          (*(vu32*)0x04000600)
#define REG_GXFIFO          (*(vu32*)0x04000400)
#define GX_FIFO_BUSY        (1 << 27)
 
// NDS geometry command for SWAP_BUFFERS is 0x50
#define GX_SORTMODE_MANUAL  0
#define GX_BUFFERMODE_W     1
 
static inline void G3X_AntiAlias(BOOL enable) {
    if (enable) REG_DISP3DCNT |=  (1 << 4);
    else        REG_DISP3DCNT &= ~(1 << 4);
}
 
static inline void G3X_AlphaBlend(BOOL enable) {
    if (enable) REG_DISP3DCNT |=  (1 << 3);
    else        REG_DISP3DCNT &= ~(1 << 3);
}
 
static inline void G3_RequestSwapBuffers(u32 sortMode, u32 bufferMode) {
    while (REG_GXSTAT & GX_FIFO_BUSY);
    REG_GXFIFO = 0x00000050; // SWAP_BUFFERS command
    REG_GXFIFO = (sortMode & 1) | ((bufferMode & 1) << 1);
}
 
// ---------------------------------------------------------------------------
// NNS G3D type definitions
// ---------------------------------------------------------------------------
 
// NNSG3dAnmObj — layout from public NNS SDK
// frame field is at offset 0x04
typedef struct NNSG3dAnmObj_t {
    struct NNSG3dAnmObj_t *next;   // 0x00
    fx32                   frame;  // 0x04
    fx32                   blend;  // 0x08
    u16                    numFrame; // 0x0C
    u16                    pad;    // 0x0E
    // remainder opaque
} NNSG3dAnmObj;
 
// NNSG3dRenderObj — opaque, sized conservatively at 0x60 bytes
typedef struct { u8 opaque[0x60]; } NNSG3dRenderObj;
 
// NNSFndAllocator — heap allocator handle
typedef struct { u32 heap; u32 allocFunc; u32 freeFunc; } NNSFndAllocator;
 
// ---------------------------------------------------------------------------
// NNS G3D dict structures for GetFirstModel
// NNSi_G3dResDictEntry: 28 bytes total
// NNSG3dResDict: 4 byte header + entries
// Entry[1].offset is at byte 4 + 28 + 8 = 0x28 from dict start
// ---------------------------------------------------------------------------
static inline void *GetFirstModel(void *mdlSet) {
    u32 offset = *(u32 *)((u8 *)mdlSet + 0x28);
    return (void *)((u8 *)mdlSet + offset);
}
 
// GetAnmNumFrames — reads numFrame field at offset 0x0C from NNSG3dAnmObj
static inline u16 GetAnmNumFrames(NNSG3dAnmObj *anmObj) {
    return anmObj->numFrame;
}
 
// ---------------------------------------------------------------------------
// External function declarations
// All addresses must be in the linker symbols file
// ---------------------------------------------------------------------------
 
// NNS G3D (all ARM mode)


void  LONG_CALL NNS_G3dGlbFlushP(void);   // used as G3_ResetG3X
 

// ---------------------------------------------------------------------------
// Battle 3D BG state
// --------------------------------------------------------------------------1-
typedef struct {
    NNSFndAllocator  allocator;
    NNSG3dRenderObj  renderObj;
    NNSG3dAnmObj    *anmObj;
    void            *resMdl;
    void            *modelRes;
    void            *animRes;
    void            *camera;
    VecFx32          pos;
    VecFx32          scale;
    fx32             frame;
} Battle3DBgState;
 
static Battle3DBgState sBattle3DBg;
static BOOL            sBattle3DInitialized = FALSE;
 
// ---------------------------------------------------------------------------
// Replace YOUR_NARC_ID / YOUR_NSBMD_MEMBER / YOUR_NSBCA_MEMBER with real values
// ---------------------------------------------------------------------------
#define NARC_ID       195   
#define NSBMD_MEMBER  5   
#define NSBCA_MEMBER  1   
 
void* ReadNarcMemberSafely(u32 narcId, u32 memberIdx, u32 heapId) {
    // 1. Open the NARC file system handle
    debug_printf("before narc_new\n");
    void *narcHandle = NARC_New(narcId, heapId);
    debug_printf("after narc_new\n");
    if (narcHandle == NULL) {
        debug_printf("CRITICAL: Failed to open NARC %d handle!\n", narcId);
        return NULL;
    }
    
    // 2. Allocate and read JUST the specific member (the BMD0 file)
    void *fileData = NARC_AllocAndReadWholeMember(narcHandle, memberIdx, heapId);
    
    if (fileData == NULL) {
        debug_printf("CRITICAL: Failed to read member %d from NARC %d!\n", memberIdx, narcId);
    }
    
    // 3. Clean up the NARC handle so we don't leak memory
    NARC_Delete(narcHandle);
    
    return fileData;
}

void Battle3DBg_Init(u32 heapID) {
    // 1. Load NSBMD and NSBCA
    debug_printf(" NARC: %d Member: %d Heap: %d \n", 
                     NARC_ID, NSBMD_MEMBER, heapID);
    //sBattle3DBg.modelRes = ReadNarcMemberSafely(NARC_ID, NSBMD_MEMBER, heapID);
    sBattle3DBg.modelRes = AllocAndReadWholeNarcMemberByIdPair(NARC_ID, NSBMD_MEMBER, heapID);

    //sBattle3DBg.animRes  = AllocAndReadWholeNarcMemberByIdPair(NARC_ID, NSBCA_MEMBER, heapID);
    debug_printf(" NARC %d Member %d loaded successfully! \n", 
                     NARC_ID, NSBMD_MEMBER);
    // 2. Set up model resource (binds textures/palettes into VRAM)
    //NNS_G3dResDefaultSetup(sBattle3DBg.modelRes);
 
    // 3. Get model and init render obj
    void *mdlSet = NNS_G3dGetMdlSet(sBattle3DBg.modelRes);
    sBattle3DBg.resMdl = GetFirstModel(mdlSet);
    NNS_G3dRenderObjInit(&sBattle3DBg.renderObj, sBattle3DBg.resMdl);
 
    // 4. Init animation
    //HeapExp_FndInitAllocator(&sBattle3DBg.allocator, heapID, 4);
    //void *anim             = NNS_G3dGetAnmByIdx(sBattle3DBg.animRes, 0);
    //sBattle3DBg.anmObj     = NNS_G3dAllocAnmObj(&sBattle3DBg.allocator, anim, model);
    //void *texRes           = NNS_G3dGetTex(sBattle3DBg.modelRes);
    //NNS_G3dAnmObjInit(sBattle3DBg.anmObj, anim, model, texRes);
    //NNS_G3dRenderObjAddAnmObj(&sBattle3DBg.renderObj, sBattle3DBg.anmObj);
 
    // 5. Position and scale
    sBattle3DBg.pos   = (VecFx32){ 0, 0, 0 };
    sBattle3DBg.scale = (VecFx32){ FX32_ONE, FX32_ONE, FX32_ONE };
    sBattle3DBg.frame = 0;
 
    // 6. Camera
    VecFx32 camTarget = { 0, 0, 0 };
    VecFx32 camPos    = { 0, FX32_CONST(192), FX32_CONST(600) };
    sBattle3DBg.camera = Camera_New(heapID);
    Camera_Init_FromTargetAndPos(
        &camTarget, &camPos,
        FX_DEG_TO_IDX(FX32_CONST(15.996f)),
        0,      // perspective type — verify against camera.o
        FALSE,
        sBattle3DBg.camera);
    Camera_SetPerspectiveClippingPlane(FX32_CONST(0), FX32_CONST(300), sBattle3DBg.camera);
    Camera_ApplyPerspectiveType(0, sBattle3DBg.camera);
    Camera_PushLookAtToNNSGlb(sBattle3DBg.camera);
 
    // 7. 3D render state
    G3X_AntiAlias(TRUE);
    G3X_AlphaBlend(TRUE);
    G2_SetBG0Priority(3);

    DC_FlushAll();
}
 
void Battle3DBg_Render(void) {
    // flush NNS globals (camera, lights) to hardware
    NNS_G3dGlbFlushP();
 

    // re-apply camera every frame
    Camera_ApplyPerspectiveType(0, sBattle3DBg.camera);
    Camera_PushLookAtToNNSGlb(sBattle3DBg.camera);
 

    NNS_G3dGlbSetBaseTrans(&sBattle3DBg.pos);
    NNS_G3dGlbSetBaseScale(&sBattle3DBg.scale);
    NNS_G3dGlbFlushP();

    // flush data cache before DMA/GPU reads
    
 
    debug_printf("before draw\n");
    // draw the model
    if (sBattle3DBg.resMdl != NULL) {
        NNS_G3dDraw1Mat1Shp(sBattle3DBg.resMdl, 0, 0, TRUE);
        debug_printf("drawn!\n");
    }
    
    // advance animation frame, loop using numFrame from the anm object
    //sBattle3DBg.frame += FX32_ONE;
    //if (sBattle3DBg.frame >= ((fx32)GetAnmNumFrames(sBattle3DBg.anmObj) << FX32_SHIFT)) {
    //    sBattle3DBg.frame = 0;
    //}
    //sBattle3DBg.anmObj->frame = sBattle3DBg.frame;
 
    // swap geometry buffers
    //G3_RequestSwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
}



BOOL LONG_CALL Battle_Run_hook(struct BattleSystem *bsys, int *battleState, int unused3, int unused4) {
    BOOL result = Battle_Run_Original(bsys, battleState, unused3, unused4);
    
    if (battleState[0] > 0) {
        if (!sBattle3DInitialized) {
            Battle3DBg_Init(HEAPID_BATTLE_HEAP); 
            sBattle3DInitialized = TRUE;
        }
        u32 current = (reg_GX_DISPCNT & REG_GX_DISPCNT_DISPLAY_MASK) >> REG_GX_DISPCNT_DISPLAY_SHIFT;
        GX_SetVisiblePlane(current & ~(1 << 3));
        Battle3DBg_Render();
    }
    
    return result;
}