#include "../include/constants/file.h"
#include "../include/repel.h"
#include "../include/roamer.h"
#include "../include/script.h"
#include "../include/types.h"
#include "../include/window.h"
#include "../include/sprite.h"
#include "../include/pokemon.h"

#define SCRIPT_NEW_CMD_REPEL_USE 0
#define SCRIPT_NEW_CMD_FLOATING_TEXT_SHOW 1
#define SCRIPT_NEW_CMD_FLOATING_TEXT_HIDE 2
#define SCRIPT_NEW_CMD_MAX 256

#define FLOATING_TEXTBOX_TILE_MARGIN 32

typedef struct FloatingTextboxWork {
    BOOL active;
    u8 x, y, u, v;
    struct Window window;
    String *msg;
} FloatingTextboxWork;

static FloatingTextboxWork sFloatingTextbox;

BOOL Script_RunNewCmd(SCRIPTCONTEXT *ctx)
{
    u8 sw = ScriptReadByte(ctx);
    u16 UNUSED arg0 = ScriptReadHalfword(ctx);

    switch (sw) {
    case SCRIPT_NEW_CMD_REPEL_USE:;
#ifdef IMPLEMENT_REUSABLE_REPELS
        u16 most_recent_repel = Repel_GetMostRecent();
        SetScriptVar(arg0, most_recent_repel);
        Repel_Use(most_recent_repel, HEAPID_MAIN_HEAP);
#endif
        break;

    case SCRIPT_NEW_CMD_FLOATING_TEXT_SHOW:
        //These should be set in dspre (i.e. SetVar 0x8004 4)
        sFloatingTextbox.x = GetScriptVar(0x8004);//8;//
        sFloatingTextbox.y = GetScriptVar(0x8005);//2; //
        sFloatingTextbox.u = GetScriptVar(0x8006);//16; //;
        sFloatingTextbox.v = GetScriptVar(0x8007);//10; //; //these are correctly set in dspre.
        debug_printf("After Var Set. Values are %d, %d, %d, %d \n", sFloatingTextbox.x, sFloatingTextbox.y, sFloatingTextbox.u, sFloatingTextbox.v);
        if (!sFloatingTextbox.active) {
            void *bgConfig = ctx->fsys->bg_config;

            //u8 bgId = 3;
            //u16 baseTile = 10;
            //u8 paletteNum = 11;
            //u8 frameId = 0;

            u8 bgId = 3;
            u8 paletteNum = 12;
            u16 baseTile = 707;

            struct Window *msgWindow = (struct Window *)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_WINDOW);
            //u16 baseTile = msgWindow->baseTile + (msgWindow->width * msgWindow->height) + FLOATING_TEXTBOX_TILE_MARGIN;
            //debug_printf("baseTile is %d \n", baseTile);
            //debug_printf("paletteNum is %d \n", msgWindow->paletteNum);
            //debug_printf("bgId is %d \n", msgWindow->bgId);

            //AddWindowParameterized(msgWindow->bgConfig, &sFloatingTextbox.window, msgWindow->bgId,sFloatingTextbox.x, sFloatingTextbox.y,sFloatingTextbox.u, sFloatingTextbox.v,  msgWindow->paletteNum, baseTile);
            AddWindowParameterized(bgConfig, &sFloatingTextbox.window, bgId,
                                    sFloatingTextbox.x, sFloatingTextbox.y,
                                    sFloatingTextbox.u, sFloatingTextbox.v,
                                    paletteNum, baseTile);

            FillWindowPixelBuffer(&sFloatingTextbox.window, 0xFF);


        
            //(bgConfig, bgId, baseTile, framePalette, frameId, HEAPID_MAIN_HEAP);
            //
            //LoadFontPal0(0, 0x20, 0);


            //DrawFrameAndWindow1(&sFloatingTextbox.window, FALSE, 1, 8);
            LoadUserFrameGfx1(bgConfig, bgId, 1, 8, 0, HEAPID_FIELD1);
            DrawFrameAndWindow1(&sFloatingTextbox.window, TRUE, 1, 8);

            sFloatingTextbox.msg = NewString_ReadMsgData((MsgData *)ctx->msg_data, arg0);

            AddTextPrinterParameterized(&sFloatingTextbox.window, 0, sFloatingTextbox.msg, 0, 0, 0, 0);
 
            sFloatingTextbox.active = TRUE;
        }
        break;
    case SCRIPT_NEW_CMD_FLOATING_TEXT_HIDE:
        if (sFloatingTextbox.active) {
            RemoveWindow(&sFloatingTextbox.window);
 
            if (sFloatingTextbox.msg != NULL) {
                sys_FreeMemoryEz(sFloatingTextbox.msg);
                sFloatingTextbox.msg = NULL;
            }
 
            sFloatingTextbox.active = FALSE;
        }
        break;

    default:
        break;
    }

    return FALSE;
}

BOOL LONG_CALL ScrCmd_CreateRoamer(SCRIPTCONTEXT *ctx)
{
    u8 roamerNo = ScriptReadByte(ctx);
    Save_CreateRoamerByID(ctx->fsys->savedata, roamerNo);
    return FALSE;
}
