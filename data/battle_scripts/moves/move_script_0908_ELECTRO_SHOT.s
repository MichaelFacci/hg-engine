#include "constants/battle_constants.h"
.include "battle_commands.inc"

.data

_000:
    /*BufferMessage 2133, TAG_NICKNAME, BATTLER_CATEGORY_ATTACKER -- old*/
    // {0} absorbed electricity!                                   --new
    // BufferMessage 1480, TAG_NICKNAME, BATTLER_CATEGORY_ATTACKER
    GoToEffectScript 
