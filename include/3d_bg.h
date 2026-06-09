#include "types.h"
#include "item.h"
#include "sprite.h"
#include "pokemon.h"
#include "task.h"
#include "save.h"
#include "constants/moves.h"


typedef u8 FSFile[72];
typedef u16 GXRgb;

typedef enum
{
	FS_SEEK_SET,
	FS_SEEK_CUR,
	FS_SEEK_END
} FSSeekFileMode;


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


typedef enum {
    GF_PLANE_TOGGLE_OFF,
    GF_PLANE_TOGGLE_ON,
} GFPlaneToggle;

typedef enum GFBgLayer {
    GF_BG_LYR_MAIN_0 = 0,
    GF_BG_LYR_MAIN_1,
    GF_BG_LYR_MAIN_2,
    GF_BG_LYR_MAIN_3,
    GF_BG_LYR_SUB_0,
    GF_BG_LYR_SUB_1,
    GF_BG_LYR_SUB_2,
    GF_BG_LYR_SUB_3,
    GF_BG_LYR_MAIN_CNT = 4,
    GF_BG_LYR_SUB_CNT = 4,
    GF_BG_LYR_MAIN_FIRST = GF_BG_LYR_MAIN_0,
    GF_BG_LYR_SUB_FIRST = GF_BG_LYR_SUB_0,
    GF_BG_LYR_MAX = 8,

    GF_BG_LYR_MAIN_0_F = 1 << (GF_BG_LYR_MAIN_0 - GF_BG_LYR_MAIN_FIRST),
    GF_BG_LYR_MAIN_1_F = 1 << (GF_BG_LYR_MAIN_1 - GF_BG_LYR_MAIN_FIRST),
    GF_BG_LYR_MAIN_2_F = 1 << (GF_BG_LYR_MAIN_2 - GF_BG_LYR_MAIN_FIRST),
    GF_BG_LYR_MAIN_3_F = 1 << (GF_BG_LYR_MAIN_3 - GF_BG_LYR_MAIN_FIRST),
    GF_BG_LYR_SUB_0_F = 1 << (GF_BG_LYR_SUB_0 - GF_BG_LYR_SUB_FIRST),
    GF_BG_LYR_SUB_1_F = 1 << (GF_BG_LYR_SUB_1 - GF_BG_LYR_SUB_FIRST),
    GF_BG_LYR_SUB_2_F = 1 << (GF_BG_LYR_SUB_2 - GF_BG_LYR_SUB_FIRST),
    GF_BG_LYR_SUB_3_F = 1 << (GF_BG_LYR_SUB_3 - GF_BG_LYR_SUB_FIRST),

    GF_BG_LYR_UNALLOC = 0xFF,
} GFBgLayer;

 
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
 
// NNSG3dRenderObj — opaque, sized conservatively at 0x100 bytes
typedef struct { u8 opaque[0x100]; } NNSG3dRenderObj;
 
// NNSFndAllocator — heap allocator handle
typedef struct { u32 heap; u32 allocFunc; u32 freeFunc; } NNSFndAllocator;

typedef enum {
    NNS_G3D_GLB_FLAG_FLUSH_WVP              = 0x00000001,
    NNS_G3D_GLB_FLAG_FLUSH_VP               = 0x00000002,
    NNS_G3D_GLB_FLAG_INVBASE_UPTODATE       = 0x00000004,
    NNS_G3D_GLB_FLAG_INVCAMERA_UPTODATE     = 0x00000008,
    NNS_G3D_GLB_FLAG_INVPROJ_UPTODATE       = 0x00000010,
    NNS_G3D_GLB_FLAG_INVBASECAMERA_UPTODATE = 0x00000020,
    NNS_G3D_GLB_FLAG_INVCAMERAPROJ_UPTODATE = 0x00000040,

    NNS_G3D_GLB_FLAG_BASECAMERA_UPTODATE    = 0x00000080,

    NNS_G3D_GLB_FLAG_SRTCAMERA_UPTODATE     = NNS_G3D_GLB_FLAG_BASECAMERA_UPTODATE,
    NNS_G3D_GLB_FLAG_FLUSH_ALT              = NNS_G3D_GLB_FLAG_FLUSH_WVP
} NNSG3dGlbFlag;


// Matrices
typedef union {
    struct {
        fx32 _00, _01, _02, _03;
        fx32 _10, _11, _12, _13;
        fx32 _20, _21, _22, _23;
        fx32 _30, _31, _32, _33;
    };
    fx32 m[4][4];
    fx32 a[16];
} MtxFx44;

typedef union {
    struct {
        fx32 _00, _01, _02;
        fx32 _10, _11, _12;
        fx32 _20, _21, _22;
        fx32 _30, _31, _32;
    };
    fx32 m[4][3];
    fx32 a[12];
} MtxFx43;

typedef union {
    struct {
        fx32 _00, _01, _02;
        fx32 _10, _11, _12;
        fx32 _20, _21, _22;
    };
    fx32 m[3][3];
    fx32 a[9];
} MtxFx33;

typedef union {
    struct {
        fx32 _00, _01;
        fx32 _10, _11;
    };
    fx32 m[2][2];
    fx32 a[4];
} MtxFx22;

typedef struct {
    NNSFndAllocator  allocator;
    
    NNSG3dAnmObj    *anmObj;
    void            *resMdl;
    void            *modelRes;
    void            *animRes;
    void            *camera;
    VecFx32          pos;
    VecFx32          scale;
    MtxFx33           rot;
    fx32             frame;
    NNSG3dRenderObj  renderObj;
} Battle3DBgState;

typedef struct NNSG3dResDataBlockHeader_ {
    union {
        u32 kind;
        char chr[4];
    };
    u32 size;
} NNSG3dResDataBlockHeader;

typedef struct NNSG3dResDictTreeNode_ {
    u8 refBit;
    u8 idxLeft;
    u8 idxRight;
    u8 idxEntry;
} NNSG3dResDictTreeNode;

typedef struct NNSG3dResDict_ {
    u8 revision;
    u8 numEntry;
    u16 sizeDictBlk;
    u16 dummy_;
    u16 ofsEntry;
    NNSG3dResDictTreeNode node[1];
} NNSG3dResDict;

typedef struct NNSG3dResDictMdlSetData_ {
    u32 offset;
} NNSG3dResDictMdlSetData;


typedef struct NNSG3dResMdlInfo_ {
    u8 sbcType;
    u8 scalingRule;
    u8 texMtxMode;
    u8 numNode;
    u8 numMat;
    u8 numShp;
    u8 firstUnusedMtxStackID;
    u8 dummy_;
    fx32 posScale;
    fx32 invPosScale;
    u16 numVertex;
    u16 numPolygon;
    u16 numTriangle;
    u16 numQuad;
    fx16 boxX, boxY, boxZ;
    fx16 boxW, boxH, boxD;
    fx32 boxPosScale;
    fx32 boxInvPosScale;
} NNSG3dResMdlInfo;

typedef struct NNSG3dResNodeInfo_ {
    NNSG3dResDict dict;
} NNSG3dResNodeInfo;

typedef struct NNSG3dResMdl_ {
    u32 size;
    u32 ofsSbc;
    u32 ofsMat;
    u32 ofsShp;
    u32 ofsEvpMtx;
    NNSG3dResMdlInfo info;
    NNSG3dResNodeInfo nodeInfo;
} NNSG3dResMdl;

typedef struct NNSG3dResMdlSet_ {
    NNSG3dResDataBlockHeader header;
    NNSG3dResDict dict;
} NNSG3dResMdlSet;

typedef struct NNSG3dResDictEntryHeader_ {
    u16 sizeUnit;
    u16 ofsName;
    u8 data[4];
} NNSG3dResDictEntryHeader;

typedef struct {
    u32 cmd0;
    u32 mtxmode_proj;
    MtxFx44 projMtx;
    u32 mtxmode_posvec;
    MtxFx43 cameraMtx;
    u32 cmd1;
    u32 lightVec[4];
    u32 cmd2;
    u32 prmMatColor0;
    u32 prmMatColor1;
    u32 prmPolygonAttr;
    u32 prmViewPort;
    u32 cmd3;
    u32 lightColor[4];
    u32 cmd4;
    MtxFx33 prmBaseRot;
    VecFx32 prmBaseTrans;
    VecFx32 prmBaseScale;
    u32 prmTexImageParam;
    u32 flag;
    MtxFx43 invCameraMtx;
    MtxFx43 srtCameraMtx;
    MtxFx43 invSrtCameraMtx;
    MtxFx43 invBaseMtx;
    MtxFx44 invProjMtx;
    MtxFx44 invCameraProjMtx;
    VecFx32 camPos;
    VecFx32 camUp;
    VecFx32 camTarget;
} NNSG3dGlb;

extern NNSG3dGlb NNS_G3dGlb;

 
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


// 3D background begin----------------------------------------------------------------------------------------
int LONG_CALL BattleSystem_GetBackgroundId(struct BattleSystem *bsys);
void LONG_CALL BattleSystem_SetBackground_Original(int param1);
BOOL LONG_CALL Battle_Run_Original(struct BattleSystem *bsys, int *battleState, int unused3, int unused4);

// NNS G3D 
void  LONG_CALL NNS_G3dResDefaultSetup(void *nsbmd);                                          // 020C26C0
void* LONG_CALL NNS_G3dGetMdlSet(void *nsbmd);                                                // 020C3B40
void  LONG_CALL NNS_G3dRenderObjInit(void *renderObj, void *model);                           // 020BE120
void  LONG_CALL NNS_G3dRenderObjAddAnmObj(void *renderObj, void *anmObj);                     // 020BE294
void* LONG_CALL NNS_G3dAllocAnmObj(void *allocator, void *anim, void *model);                 // 020C2B7C
void  LONG_CALL NNS_G3dFreeAnmObj(void *allocator, void *anmObj);                             // 020C2BA0
void  LONG_CALL NNS_G3dAnmObjInit(void *anmObj, void *anim, void *model, void *texRes);       // 020BE008
void* LONG_CALL NNS_G3dGetAnmByIdx(void *animRes, u32 idx);                                   // 020C3B90
void* LONG_CALL NNS_G3dGetTex(void *modelRes);                                                // 020C3B50
void  LONG_CALL NNS_G3dDraw(void *renderObj);                                                 // 020BFCC8
                                      
void  LONG_CALL NNS_G2dSetupSoftwareSpriteCamera(void);                                // 020BFCC8

// NARC
void* LONG_CALL AllocAndReadWholeNarcMemberByIdPair(u32 narcIdx, u32 memberIdx, u32 heapID);   // 02007524

// Heap
void  LONG_CALL HeapExp_FndInitAllocator(void *allocator, u32 heapID, u32 align);              // 0201AC14
void  LONG_CALL Heap_Free(void *ptr);                                                          // 0201AB0C

// Camera
void* LONG_CALL Camera_New(u32 heapID);                                                        // 02023114
void  LONG_CALL Camera_Delete(void *camera);                                                   // 02023120
void  LONG_CALL Camera_Init_FromTargetAndPos(VecFx32 *target, VecFx32 *pos,
                    u32 fov, u32 projType, BOOL param5, void *camera);                         // 02023308
void  LONG_CALL Camera_SetPerspectiveClippingPlane(fx32 near, fx32 far, void *camera);         // 02023240
void  LONG_CALL Camera_ApplyPerspectiveType(u32 projType, void *camera);                       // 020233D8
void  LONG_CALL Camera_PushLookAtToNNSGlb(void *camera);                                       // 02023154

void  LONG_CALL DC_FlushAll();

void* LONG_CALL NARC_New(u32 narcId, u32 heapId);
void  LONG_CALL NARC_Delete(void *narc);
void* LONG_CALL NARC_AllocAndReadWholeMember(void *narc, u32 memberIdx, u32 heapId);

void LONG_CALL NNS_G3dGlbSetBaseTrans(const VecFx32 *trans);
void LONG_CALL NNS_G3dGlbSetBaseScale(const VecFx32 *scale);
void LONG_CALL NNS_G3dDraw1Mat1Shp(void *resMdl, u32 matIdx, u32 shpIdx, BOOL draw);

//MI STUFF
void LONG_CALL MI_Copy36B(const MtxFx33 *pRot, MtxFx33 *prmBaseRot);

BOOL LONG_CALL FS_SeekFile(FSFile *p_file, s32 offset, FSSeekFileMode origin);
s32  LONG_CALL FS_ReadFile(FSFile *p_file, void *dst, s32 len);
BOOL LONG_CALL FS_CloseFile(FSFile *p_file);
BOOL LONG_CALL FS_OpenFileFast(FSFile* p_file, void* archivePtr, int file_id);
void LONG_CALL FS_InitFile(FSFile *p_file);
void* LONG_CALL FS_FindArchive(const char* name, int len);

void LONG_CALL GfGfx_EngineATogglePlanes(u8 planeMask, u8 enable);

void LONG_CALL G3X_SetClearColor(GXRgb rgb, int alpha, int depth, int polygonID, BOOL fog);

// 3D background end----------------------------------------------------------------------------------------





static inline void* NNS_G3dGetResDataByIdx(const NNSG3dResDict* dict, u32 idx) {
    NNSG3dResDictEntryHeader* hdr;

    if (dict != NULL && idx < dict->numEntry) {
        hdr = (NNSG3dResDictEntryHeader*)((u8*)dict + dict->ofsEntry);
        return (void*)(&hdr->data[0] + hdr->sizeUnit * idx);
    } else {
        return NULL;
    }
}

static inline NNSG3dResMdl* NNS_G3dGetMdlByIdx(const NNSG3dResMdlSet* mdlSet, u32 idx) {
    NNSG3dResDictMdlSetData* data;

    if (mdlSet) {
        data = (NNSG3dResDictMdlSetData*)NNS_G3dGetResDataByIdx(&mdlSet->dict, idx);
        if (data) {
            return (NNSG3dResMdl*)((u8*)mdlSet + data->offset);
        }
    }
    return NULL;
}

static inline void NNS_G3dGlbSetBaseRot(const MtxFx33 *pRot) {
    MI_Copy36B(pRot, &NNS_G3dGlb.prmBaseRot);
    NNS_G3dGlb.flag &= ~(NNS_G3D_GLB_FLAG_BASECAMERA_UPTODATE | NNS_G3D_GLB_FLAG_INVBASE_UPTODATE | NNS_G3D_GLB_FLAG_INVBASECAMERA_UPTODATE);
}