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
#include "../../include/3d_bg.h"


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





 
static Battle3DBgState sBattle3DBg;
static BOOL            sBattle3DInitialized = FALSE;
 
// ---------------------------------------------------------------------------
// Replace YOUR_NARC_ID / YOUR_NSBMD_MEMBER / YOUR_NSBCA_MEMBER with real values
// ---------------------------------------------------------------------------
#define NARC_ID       195 //507 for new file with FS loading, 195 is title screen graphics replaced with some flat-ish land with trees as a test
#define NSBMD_MEMBER  5   
#define NSBCA_MEMBER  1   

/*Not using this for now*/
void* LoadStandaloneModelByID(u32 fileId, u32 heapID) {
    FSFile file;
    void *buffer = NULL;
    
    FS_InitFile(&file);
    void* romArchive = FS_FindArchive("rom", 3);

    // Open the file (507 is snowy.nsbmd)
    if (FS_OpenFileFast(&file, romArchive, fileId)) {
        
        // 1. HARDCODE THE SIZE! (From your Tinke screenshot)
        u32 fileSize = 27664; 
        
        // 2. Allocate exactly 27,664 bytes on the heap
        buffer = sys_AllocMemory(heapID, fileSize);
        
        if (buffer != NULL) {
            // 3. Read the file into our new bucket
            FS_ReadFile(&file, buffer, fileSize);
            debug_printf("SUCCESS: Read %d bytes from file ID %d into memory!\n", fileSize, fileId);
        } else {
            debug_printf("CRITICAL: Failed to allocate %d bytes on heap %d!\n", fileSize, heapID);
        }
        
        FS_CloseFile(&file);
    } else {
        debug_printf("CRITICAL: FS_OpenFileFast failed to find file ID %d!\n", fileId);
    }
    
    return buffer;
}



void Battle3DBg_Init(u32 heapID) {
    
        // set display mode to graphics, BG0 as 3D
    //u32 dispcnt = reg_GX_DISPCNT;
    //dispcnt &= ~(7 << 0);   // clear bg mode bits
    //dispcnt |=  (1 << 3);   // enable BG0 as 3D (GX_BG0_AS_3D)
    //reg_GX_DISPCNT = dispcnt;

    debug_printf(" NARC: %d Member: %d Heap: %d \n", 
                     NARC_ID, NSBMD_MEMBER, heapID);
    //sBattle3DBg.modelRes = ReadNarcMemberSafely(NARC_ID, NSBMD_MEMBER, heapID);
    //sBattle3DBg.modelRes = LoadStandaloneModelByID(507, heapID); //AllocAndReadWholeNarcMemberByIdPair(NARC_ID, NSBMD_MEMBER, heapID);
    //sBattle3DBg.modelRes = AllocAndReadWholeNarcMemberByIdPair(NARC_ID, NSBMD_MEMBER, heapID);

    //try loading the model from path similar to easy3d:
    //void *outResource = AllocAndReadWholeNarcMemberByIdPair(NARC_ID, NSBMD_MEMBER, heapID);
    void *outResource;

    outResource = AllocAndReadWholeNarcMemberByIdPair(195 + 8, 0    , heapID);

    u8 *b = (u8*)outResource;
    debug_printf("member %d: %c%c%c%c\n", 0, b[0], b[1], b[2], b[3]);
    u8 *bytes = (u8 *)outResource;
    debug_printf("magic: %c%c%c%c\n", bytes[0], bytes[1], bytes[2], bytes[3]);
    debug_printf("bytes[4-7]: %02X %02X %02X %02X\n", bytes[4], bytes[5], bytes[6], bytes[7]);
    debug_printf("filesize field: 0x%08X\n", *(u32*)(bytes + 8));
    
    if (outResource == NULL) {
        debug_printf("CRITICAL: NSBMD load failed!\n");
        return;
    }
    debug_printf("modelRes = 0x%08X\n", (u32)outResource);

    void *texture = NNS_G3dGetTex(outResource);
    debug_printf("texture = 0x%08X\n", (u32)texture);

    DC_FlushAll();
    NNS_G3dResDefaultSetup(outResource);
    sBattle3DBg.modelRes = outResource;
    void *mdlSet = NNS_G3dGetMdlSet(outResource);
    debug_printf("mdlSet = 0x%08X\n", (u32)mdlSet);

    sBattle3DBg.resMdl = NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(outResource), 0);
    debug_printf("resMdl = 0x%08X\n", (u32)sBattle3DBg.resMdl);

    //resMdl is having the issues

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
    sBattle3DBg.rot = (MtxFx33){ .a = { FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE } };
    sBattle3DBg.frame = 0;
 
    // 6. Camera
    VecFx32 camTarget = { 0, FX32_CONST(100), FX32_CONST(-18) };
    VecFx32 camPos    = { 0, FX32_CONST(192), FX32_CONST(600) };
    sBattle3DBg.camera = Camera_New(heapID);
    Camera_Init_FromTargetAndPos(
        &camTarget, &camPos,
        2912,//FX_DEG_TO_IDX(FX32_CONST(15.996f)),
        0,      // perspective type — verify against camera.o
        FALSE,
        sBattle3DBg.camera);
    Camera_SetPerspectiveClippingPlane(FX32_CONST(1), FX32_CONST(1000), sBattle3DBg.camera);
    Camera_ApplyPerspectiveType(0, sBattle3DBg.camera);
    Camera_PushLookAtToNNSGlb(sBattle3DBg.camera);
 
    // 7. 3D render state
    GfGfx_EngineATogglePlanes(GF_BG_LYR_MAIN_0_F, GF_PLANE_TOGGLE_ON);
    G2_SetBG0Priority(1);

    G3X_AntiAlias(TRUE);
    G3X_AlphaBlend(TRUE);
    G3X_SetClearColor(GX_RGB(0, 0, 0), 0, 0x7FFF, 63, 0);
    G3_ViewPort(0, 0, 255, 191);

    DC_FlushAll();
}
 

/*
void Battle3DBg_Render_derwil(void) {
    if (sBattle3DBg.resMdl == NULL) {
        return;
    }

    Camera_SetAsActive(sCamera);
    Camera_ComputeProjectionMatrix(CAMERA_PROJECTION_PERSPECTIVE, sCamera);
    Camera_ComputeViewMatrix();
    NNS_G3dGlbFlush();
    NNS_G3dGeFlushBuffer();

     // --- Dynamic arena model transform ---
    G3_PushMtx();

    fx32 t[3];
    u16  r[3];
    fx32 sc;

    // Prefer scene entity arena transform; fall back to ArenaConfigEntry
    BOOL useScene = (sScene != NULL && sScene->arenaEntityIndex >= 0
        && sScene->entities[sScene->arenaEntityIndex].visible);

    if (useScene) {
        SceneTransform *st = &sScene->entities[sScene->arenaEntityIndex].localTransform;
        t[0] = st->position.x;
        t[1] = st->position.y;
        t[2] = st->position.z;
        r[0] = st->rotation[0];
        r[1] = st->rotation[1];
        r[2] = st->rotation[2];
        sc   = st->uniformScale;
    } else {
        const ArenaConfigEntry *cfg = ArenaConfig_Lookup(sArenaMapHeader);
        t[0] = cfg->translation[0];
        t[1] = cfg->translation[1];
        t[2] = cfg->translation[2];
        r[0] = cfg->rotation[0];
        r[1] = cfg->rotation[1];
        r[2] = cfg->rotation[2];
        sc   = cfg->scale;
    }

    G3_Translate(t[0], t[1], t[2]);
    G3_RotX(FX_SinIdx(r[0]), FX_CosIdx(r[0]));
    G3_RotY(FX_SinIdx(r[1]), FX_CosIdx(r[1]));
    G3_RotZ(FX_SinIdx(r[2]), FX_CosIdx(r[2]));
    G3_Scale(sc, sc, sc);

    Gen5Battle3DArena_Draw();

    G3_PopMtx(1);

    Camera_ClearActive();
    G3_ResetG3X();
    NNS_G2dSetupSoftwareSpriteCamera();
}
*/


void Battle3DBg_Render(void) {

    //reg_GX_DISPCNT |= (1 << 3);
    // re-apply camera every frame
    Camera_ApplyPerspectiveType(0, sBattle3DBg.camera);
    Camera_PushLookAtToNNSGlb(sBattle3DBg.camera);
 

    NNS_G3dGlbSetBaseTrans(&sBattle3DBg.pos);
    NNS_G3dGlbSetBaseRot(&sBattle3DBg.rot);
    NNS_G3dGlbSetBaseScale(&sBattle3DBg.scale);
    NNS_G3dGlbFlushP();
 
    debug_printf("before draw\n");
    // draw the model
    if (sBattle3DBg.resMdl != NULL) {
        //NNS_G3dDraw1Mat1Shp(sBattle3DBg.resMdl, 0, 0, TRUE);
        NNS_G3dDraw(&sBattle3DBg.renderObj);
        debug_printf("drawn!\n");
    }
    
    //NNS_G2dSetupSoftwareSpriteCamera();
    G3_RequestSwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
}

void Battle3DBg_Render_gem(void) {
    if (sBattle3DBg.resMdl == NULL) return;

    // --- 1. SAVE HARDWARE MATRIX STATE ---
    *((volatile u32 *)0x04000440) = 0; // Proj mode
    *((volatile u32 *)0x04000444) = 0; // Push
    *((volatile u32 *)0x04000440) = 2; // Pos/Vec mode
    *((volatile u32 *)0x04000444) = 0; // Push

    // --- 2. SETUP CAMERA & GLOBAL TRANSFORMS ---
    Camera_ApplyPerspectiveType(0, sBattle3DBg.camera);
    Camera_PushLookAtToNNSGlb(sBattle3DBg.camera);
 
    NNS_G3dGlbSetBaseTrans(&sBattle3DBg.pos);
    NNS_G3dGlbSetBaseScale(&sBattle3DBg.scale);
    NNS_G3dGlbFlushP(); 
 
    // --- 3. DYNAMIC SAFE DRAW (Bypassing NNS_G3dDraw) ---
    // Read the exact material and shape counts from the model header
    u8 numMat = *((u8*)sBattle3DBg.resMdl + 0x18);
    u8 numShp = *((u8*)sBattle3DBg.resMdl + 0x19);

    debug_printf("Model has %d materials and %d shapes\n", numMat, numShp);

    for (int i = 0; i < numShp; i++) {
        // If there are more shapes than materials (e.g. a texture atlas),
        // clamp the material index to the highest available material to prevent a crash.
        int matIdx = (i < numMat) ? i : (numMat - 1);
        
        NNS_G3dDraw1Mat1Shp(sBattle3DBg.resMdl, matIdx, i, TRUE);
    }

    // --- 4. RESTORE HARDWARE MATRIX STATE ---
    *((volatile u32 *)0x04000440) = 2; // Pos/Vec mode
    *((volatile u32 *)0x04000448) = 1; // Pop
    *((volatile u32 *)0x04000440) = 0; // Proj mode
    *((volatile u32 *)0x04000448) = 1; // Pop
    *((volatile u32 *)0x04000440) = 2; // Reset mode back to Pos/Vec

    // --- 5. SWAP BUFFERS ---
    G3_RequestSwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
}



BOOL LONG_CALL Battle_Run_hook(struct BattleSystem *bsys, int *battleState, int unused3, int unused4) {
    BOOL result = Battle_Run_Original(bsys, battleState, unused3, unused4);
    
    if (battleState[0] > 0) {
        if (!sBattle3DInitialized) {
            Battle3DBg_Init(0); 
            sBattle3DInitialized = TRUE;
        }
        u32 current = (reg_GX_DISPCNT & REG_GX_DISPCNT_DISPLAY_MASK) >> REG_GX_DISPCNT_DISPLAY_SHIFT;
        //GX_SetVisiblePlane(current & ~(1 << 3));
        GX_SetVisiblePlane((current | (1 << 0)) & ~(1 << 3));
        Battle3DBg_Render();
    }
    
    return result;
}