#include "../../include/battle.h"
#include "../../include/config.h"
#include "../../include/debug.h"
#include "../../include/pokemon.h"
#include "../../include/types.h"
#include "../../include/constants/ability.h"
#include "../../include/constants/hold_item_effects.h"
#include "../../include/constants/battle_script_constants.h"
#include "../../include/constants/item.h"
#include "../../include/constants/move_effects.h"
#include "../../include/constants/moves.h"
#include "../../include/constants/species.h"
#include "../../include/constants/file.h"
#include "../../include/overlay.h"
#include "../../include/trainer_ai.h"
#include "../../include/q412.h"
#include "../../include/custom/custom_ai.h"




// function declarations
void AITypeCalc(struct BattleStruct *sp, u32 move, u32 type, int atkAbility, int defAbility, int held_effect, int type1, int type2, u32 *flag);
int LONG_CALL BattleAI_PostKOSwitchIn(struct BattleSystem *battleSys, int battler);


/**
 *  @brief set up type calc flags for AI to respect and make decisions based on
 *
 *  @param sp global battle structure
 *  @param move index of the move being used
 *  @param atkAbility ability of the attacker
 *  @param defAbility ability of the defender
 *  @param held_effect held item effect of the DEFENDER (this was erroneously labeled before)
 *  @param type1 primary type of defender
 *  @param type2 secondary type of defender
 *  @param flag flags to modify so that the AI knows what decision to make
 */
void AITypeCalc(struct BattleStruct *sp, u32 move, u32 type, int atkAbility, int defAbility, int held_effect, int type1, int type2, u32 *flag)
{
    int i;
    u8 typeLocal;

    if (move == MOVE_STRUGGLE)
    {
        return;
    }

    typeLocal = GetAdjustedMoveTypeBasics(sp, move, atkAbility, type); // not just normalize, now others

    if ((atkAbility != ABILITY_MOLD_BREAKER)
     && (defAbility == ABILITY_LEVITATE)
     && (typeLocal == TYPE_GROUND)
     && ((sp->field_condition & FIELD_STATUS_GRAVITY) == 0)
     && (held_effect != HOLD_EFFECT_SPEED_DOWN_GROUNDED))
    {
        flag[0] |= MOVE_STATUS_FLAG_NOT_EFFECTIVE; // not "not very effective", ineffective
    }
    else if ((typeLocal == TYPE_GROUND)
          && ((sp->field_condition & FIELD_STATUS_GRAVITY) == 0)
          && (held_effect == HOLD_EFFECT_UNGROUND_DESTROYED_ON_HIT))
    {
        flag[0] |= MOVE_STATUS_FLAG_NOT_EFFECTIVE; // not "not very effective", ineffective
    }
    else
    {
        i = 0;
        while (TypeEffectivenessTable[i][0] != TYPE_ENDTABLE)
        {
            if (TypeEffectivenessTable[i][0] == TYPE_FORESIGHT)
            {
                if (atkAbility == ABILITY_SCRAPPY || atkAbility == ABILITY_MINDS_EYE)
                {
                    break;
                }
                else
                {
                    i++;
                    continue;
                }
            }
            // TODO: Handle Primal Weathers so that the AI knows about them
            if (TypeEffectivenessTable[i][0] == typeLocal)
            {
                if (TypeEffectivenessTable[i][1] == type1)
                {
                    if (AI_ShouldUseNormalTypeEffCalc(sp, held_effect, i) == TRUE)
                    {
                        AI_TypeCheckCalc(TypeEffectivenessTable[i][2], flag);
                    }
                }
                if ((TypeEffectivenessTable[i][1] == type2) && (type1 != type2)) // haven't already run the type yet
                {
                    if (AI_ShouldUseNormalTypeEffCalc(sp, held_effect, i) == TRUE)
                    {
                        AI_TypeCheckCalc(TypeEffectivenessTable[i][2], flag);
                    }
                }
            }
            i++;
        }
    }

    if ((atkAbility != ABILITY_MOLD_BREAKER)
     && (defAbility == ABILITY_WONDER_GUARD)
     && (ShouldDelayTurnEffectivenessChecking(sp, move))
     && (((flag[0] & MOVE_STATUS_FLAG_SUPER_EFFECTIVE) == 0) || ((flag[0] & (MOVE_STATUS_FLAG_SUPER_EFFECTIVE | MOVE_STATUS_FLAG_NOT_VERY_EFFECTIVE)) == (MOVE_STATUS_FLAG_SUPER_EFFECTIVE | MOVE_STATUS_FLAG_NOT_VERY_EFFECTIVE))))
    {
        flag[0] |= MOVE_STATUS_FLAG_NOT_EFFECTIVE; // not "not very effective", ineffective
    }

    return;
}

/**
 *  @brief see if bind should restrain switching for AI
 *
 *  @param bw battle work structure
 *  @param sp global battle structure
 *  @param battler battler whose bind counter to check
 *  @return TRUE if the switch should not be able to happen, FALSE otherwise
 */
BOOL SeeIfBindShouldRestrainSwitch(struct BattleSystem *bw UNUSED, struct BattleStruct *sp, u32 battler)
{
    //debug_printf("Battler %d can%s switch out.\n", battler, (sp->binding_turns[battler] != 0) ? "'t" : "");
    return (sp->binding_turns[battler] != 0);
}

//in hooks
//0012 BattleAI_PostKOSwitchIn 02258800 2
int LONG_CALL BattleAI_PostKOSwitchIn(struct BattleSystem* bsys, int attacker)
{
    //return 6;

    u32 offset;
    int ret;
    int(*internalFunc)(struct BattleSystem* bsys, int attacker);

    u32 loadNeeded = IsOverlayLoaded(OVERLAY_BATTLE_ANIMS) ? OVERLAY_BATTLE_ANIMS : 0;
    if (loadNeeded)
		UnloadOverlayByID(OVERLAY_BATTLE_ANIMS); // unload colliding overlay so that this can be loaded

    offset = 0x0221BE20 | 1; //this is *almost* BattleAI_PostKOSwitchIn_Internal in ov10
    HandleLoadOverlay(OVERLAY_TRAINER_AI, 2);
    internalFunc = (int (*)(struct BattleSystem* bsys, int attacker))(offset);
    attacker = attacker + 10;
    ret = internalFunc(bsys, attacker);
    UnloadOverlayByID(OVERLAY_TRAINER_AI);

    if (loadNeeded)
        HandleLoadOverlay(OVERLAY_BATTLE_ANIMS, 2);

    return ret;
}

u8 LONG_CALL BattleAI_CalcSpeed(void* bw, struct BattleStruct* sp, int client1, struct PartyPokemon* partyMon, int flag)
{
    u8 ret = 0;
    u32 speed1, speed2;
    u8 hold_effect1;
    u8 hold_effect2;
    s8 priority1 = sp->clientPriority[client1];
    s8 priority2 = 0;
    u8 move_last1 = 0, move_last2 = 0;

    int ability1;
    int ability2;
    int stat_stage_spd1;
    int stat_stage_spd2;

    u32 speedModifier1 = UQ412__1_0;
    u32 speedModifier2 = UQ412__1_0;

    ability1 = GetBattlerAbility(sp, client1);
    ability2 = GetMonData(partyMon, MON_DATA_ABILITY, 0);

    hold_effect1 = HeldItemHoldEffectGet(sp, client1);
    hold_effect2 = BattleItemDataGet(sp, GetMonData(partyMon, MON_DATA_HELD_ITEM, 0), 1);

    stat_stage_spd1 = sp->battlemon[client1].states[STAT_SPEED];
    stat_stage_spd2 = 6;  //stage 0
    if (sp->side_condition[1] & SIDE_STATUS_STICKY_WEB)
        stat_stage_spd2 = stat_stage_spd2 - 1;

    // Begin calculating Speed Modifiers

    // https://web.archive.org/web/20241226231016/https://www.trainertower.com/dawoblefets-damage-dissertation/
    // NormalRound is QMul_RoundUp
    // pokeRound is QMul_RoundDown

#ifdef DEBUG_SPEED_CALC
    char client1Nickname[12];
    char client2Nickname[12];
    GetMonData(partyMon, MON_DATA_NICKNAME, client2Nickname);
    LoadNicknameToCharArray(sp->battlemon[client1].nickname, client1Nickname);
    //LoadNicknameToCharArray(sp->battlemon[client2].nickname, client2Nickname);
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] client1: %s\n", client1Nickname);
    debug_printf("[CalcSpeed] client2: %s\n", client2Nickname);
#endif

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] %s's base speed: %d\n", client1Nickname, sp->battlemon[client1].speed);
    debug_printf("[CalcSpeed] %s's base speed: %d\n", client2Nickname, GetMonData(partyMon, MON_DATA_SPEED, 0));
    //debug_printf("[CalcSpeed] %s's base speed: %d\n", client2Nickname, sp->battlemon[client2].speed);
#endif

    speed1 = (sp->battlemon[client1].speed * StatBoostModifiers[stat_stage_spd1][0] / StatBoostModifiers[stat_stage_spd1][1]) % 65536;
    speed2 = (GetMonData(partyMon, MON_DATA_SPEED, 0) * StatBoostModifiers[stat_stage_spd2][0] / StatBoostModifiers[stat_stage_spd2][1]) % 65536;

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] %s's speed1 after stat changes: %d\n", client1Nickname, speed1);
    debug_printf("[CalcSpeed] %s's speed2 after stat changes: %d\n", client2Nickname, speed2);
#endif

    // Step 1: 2x Abilities

    if ((CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, 0, ABILITY_CLOUD_NINE) == 0)
        && (CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, 0, ABILITY_AIR_LOCK) == 0)) {
        if (((ability1 == ABILITY_SWIFT_SWIM) && (sp->field_condition & WEATHER_RAIN_ANY))
            || ((ability1 == ABILITY_CHLOROPHYLL) && (sp->field_condition & WEATHER_SUNNY_ANY))
            || ((ability1 == ABILITY_SAND_RUSH) && (sp->field_condition & WEATHER_SANDSTORM_ANY))
            || ((ability1 == ABILITY_SLUSH_RUSH) && (sp->field_condition & (WEATHER_HAIL_ANY | WEATHER_SNOW_ANY)))) {
            speedModifier1 = QMul_RoundUp(speedModifier1, UQ412__2_0);
        }
        if (((ability2 == ABILITY_SWIFT_SWIM) && (sp->field_condition & WEATHER_RAIN_ANY))
            || ((ability2 == ABILITY_CHLOROPHYLL) && (sp->field_condition & WEATHER_SUNNY_ANY))
            || ((ability2 == ABILITY_SAND_RUSH) && (sp->field_condition & WEATHER_SANDSTORM_ANY))
            || ((ability2 == ABILITY_SLUSH_RUSH) && (sp->field_condition & (WEATHER_HAIL_ANY | WEATHER_SNOW_ANY)))) {
            speedModifier2 = QMul_RoundUp(speedModifier2, UQ412__2_0);
        }
    }

    if ((sp->terrainOverlay.type == ELECTRIC_TERRAIN && sp->terrainOverlay.numberOfTurnsLeft > 0)) {
        if (ability1 == ABILITY_SURGE_SURFER) {
            speedModifier1 = QMul_RoundUp(speedModifier1, UQ412__2_0);
        }

        if (ability2 == ABILITY_SURGE_SURFER) {
            speedModifier2 = QMul_RoundUp(speedModifier2, UQ412__2_0);
        }
    }

    if ((ability1 == ABILITY_UNBURDEN)
        && (sp->battlemon[client1].moveeffect.knockOffFlag)
        && (sp->battlemon[client1].item == 0)) {
        speedModifier1 = QMul_RoundUp(speedModifier1, UQ412__2_0);
    }

    if ((ability1 == ABILITY_UNBURDEN)
        && sp->terrainOverlay.numberOfTurnsLeft > 0 
        && ((sp->terrainOverlay.type == ELECTRIC_TERRAIN && hold_effect2 == HOLD_EFFECT_BOOST_DEF_ON_ELECRIC_TERRAIN)
			|| (sp->terrainOverlay.type == GRASSY_TERRAIN && hold_effect2 == HOLD_EFFECT_BOOST_DEF_ON_GRASSY_TERRAIN)
			|| (sp->terrainOverlay.type == MISTY_TERRAIN && hold_effect2 == HOLD_EFFECT_BOOST_SPDEF_ON_MISTY_TERRAIN)
			|| (sp->terrainOverlay.type == PSYCHIC_TERRAIN && hold_effect2 == HOLD_EFFECT_BOOST_SPDEF_ON_PSYCHIC_TERRAIN)))
    {
        speedModifier2 = QMul_RoundUp(speedModifier1, UQ412__2_0);
    }

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 1: 2x Abilities\n");
    debug_printf("[CalcSpeed] %s's speedModifier1: %d\n", client1Nickname, speedModifier1);
    debug_printf("[CalcSpeed] %s's speedModifier2: %d\n", client2Nickname, speedModifier2);
#endif

    // Step 2: Quick Feet

    if ((ability1 == ABILITY_QUICK_FEET) && (sp->battlemon[client1].condition & STATUS_ANY_PERSISTENT)) {
        speedModifier1 = QMul_RoundUp(speedModifier1, UQ412__1_5);
    }

    if ((ability2 == ABILITY_QUICK_FEET) && (GetMonData(partyMon, MON_DATA_STATUS, 0) & STATUS_ANY_PERSISTENT)) {
        speedModifier2 = QMul_RoundUp(speedModifier2, UQ412__1_5);
    }

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 2: Quick Feet\n");
    debug_printf("[CalcSpeed] %s's speedModifier1: %d\n", client1Nickname, speedModifier1);
    debug_printf("[CalcSpeed] %s's speedModifier2: %d\n", client2Nickname, speedModifier2);
#endif

    // Step 3: Slow Start

    if ((ability1 == ABILITY_SLOW_START)
        && ((sp->total_turn - sp->battlemon[client1].moveeffect.slowStartTurns) < 5)) {
        speedModifier1 = QMul_RoundUp(speedModifier1, UQ412__0_5);
    }

    if (ability2 == ABILITY_SLOW_START) {
        speedModifier2 = QMul_RoundUp(speedModifier2, UQ412__0_5);
    }
#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 3: Slow Start\n");
    debug_printf("[CalcSpeed] %s's speedModifier1: %d\n", client1Nickname, speedModifier1);
    debug_printf("[CalcSpeed] %s's speedModifier2: %d\n", client2Nickname, speedModifier2);
#endif

    // Step 4: Quick Powder

    if ((hold_effect1 == HOLD_EFFECT_DITTO_SPEED_UP) && (sp->battlemon[client1].species == SPECIES_DITTO)
        // Not transformed
        && !(sp->battlemon[client1].condition2 & STATUS2_TRANSFORMED)) {
        speedModifier1 = QMul_RoundUp(speedModifier1, UQ412__2_0);
    }

    if (hold_effect2 == HOLD_EFFECT_DITTO_SPEED_UP && GetMonData(partyMon, MON_DATA_SPECIES, 0) == SPECIES_DITTO) {
        speedModifier2 = QMul_RoundUp(speedModifier2, UQ412__2_0);
    }

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 4: Quick Powder\n");
    debug_printf("[CalcSpeed] %s's speedModifier1: %d\n", client1Nickname, speedModifier1);
    debug_printf("[CalcSpeed] %s's speedModifier2: %d\n", client2Nickname, speedModifier2);
#endif

    // Step 5: Choice Scarf

    if (hold_effect1 == HOLD_EFFECT_CHOICE_SPEED) {
        speedModifier1 = QMul_RoundUp(speedModifier1, UQ412__1_5);
    }

    if (hold_effect2 == HOLD_EFFECT_CHOICE_SPEED) {
        speedModifier2 = QMul_RoundUp(speedModifier2, UQ412__1_5);
    }

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 5: Choice Scarf\n");
    debug_printf("[CalcSpeed] %s's speedModifier1: %d\n", client1Nickname, speedModifier1);
    debug_printf("[CalcSpeed] %s's speedModifier2: %d\n", client2Nickname, speedModifier2);
#endif

    // Step 6: Iron Ball

    if (hold_effect1 == HOLD_EFFECT_SPEED_DOWN_GROUNDED && ability1 != ABILITY_KLUTZ)
    {
        speedModifier1 = QMul_RoundUp(speedModifier1, UQ412__0_5);
    }

    if (hold_effect2 == HOLD_EFFECT_SPEED_DOWN_GROUNDED && ability2 != ABILITY_KLUTZ)
    {
        speedModifier2 = QMul_RoundUp(speedModifier2, UQ412__0_5);
    }

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 6: Iron Ball / Macho Brace / Power EV items\n");
    debug_printf("[CalcSpeed] %s's speedModifier1: %d\n", client1Nickname, speedModifier1);
    debug_printf("[CalcSpeed] %s's speedModifier2: %d\n", client2Nickname, speedModifier2);
#endif

    // Step 7: Tailwind

    if (sp->tailwindCount[IsClientEnemy(bw, client1)]) { // new tailwind handling
        speedModifier1 = QMul_RoundUp(speedModifier1, UQ412__2_0);
    }

    //if (sp->tailwindCount[IsClientEnemy(bw, client2)]) { // new tailwind handling
    if (sp->tailwindCount[1]) { // new tailwind handling
        speedModifier2 = QMul_RoundUp(speedModifier2, UQ412__2_0);
    }

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 7: Tailwind\n");
    debug_printf("[CalcSpeed] %s's speedModifier1: %d\n", client1Nickname, speedModifier1);
    debug_printf("[CalcSpeed] %s's speedModifier2: %d\n", client2Nickname, speedModifier2);
#endif

    // Step 8: Swamp

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 8: Swamp\n");
    debug_printf("[CalcSpeed] %s's speedModifier1: %d\n", client1Nickname, speedModifier1);
    debug_printf("[CalcSpeed] %s's speedModifier2: %d\n", client2Nickname, speedModifier2);
#endif

    // Step 9: Apply limit
    // https://www.smogon.com/forums/threads/sword-shield-battle-mechanics-research.3655528/page-59#post-8704137

    speedModifier1 = speedModifier1 < 410 ? 410 : speedModifier1;
    speedModifier2 = speedModifier2 < 410 ? 410 : speedModifier2;

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 9: Apply limit\n");
    debug_printf("[CalcSpeed] %s's speedModifier1: %d\n", client1Nickname, speedModifier1);
    debug_printf("[CalcSpeed] %s's speedModifier2: %d\n", client2Nickname, speedModifier2);
#endif

    // Step 10: Apply the chained modifier to the starting speed

    speed1 = QMul_RoundDown(speed1, speedModifier1);
    speed2 = QMul_RoundDown(speed2, speedModifier2);

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 10: Apply the chained modifier to the starting speed\n");
    debug_printf("[CalcSpeed] %s's speed1: %d\n", client1Nickname, speed1);
    debug_printf("[CalcSpeed] %s's speed2: %d\n", client2Nickname, speed2);
#endif

    // Step 11: Paralysis

    if ((ability1 != ABILITY_QUICK_FEET)
        && sp->battlemon[client1].condition & STATUS_PARALYSIS) {
        speed1 = QMul_RoundUp(speed1, UQ412__0_5);  // gen 7 on only halves speed for paralysis
    }

    if ((ability2 != ABILITY_QUICK_FEET) && (GetMonData(partyMon, MON_DATA_STATUS, 0) & STATUS_PARALYSIS)) {
        speed2 = QMul_RoundUp(speed2, UQ412__0_5);  // gen 7 on only halves speed for paralysis
    }

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 11: Paralysis\n");
    debug_printf("[CalcSpeed] %s's speed1: %d\n", client1Nickname, speed1);
    debug_printf("[CalcSpeed] %s's speed2: %d\n", client2Nickname, speed2);
#endif

    // Step 12: Apply limit

    speed1 = speed1 % 65536;
    speed1 = speed1 > 10000 ? 10000 : speed1;
    speed2 = speed2 % 65536;
    speed2 = speed2 > 10000 ? 10000 : speed2;

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 12: Apply limit\n");
    debug_printf("[CalcSpeed] %s's speed1: %d\n", client1Nickname, speed1);
    debug_printf("[CalcSpeed] %s's speed2: %d\n", client2Nickname, speed2);
#endif

    // Step 13: Speed calculations stop here for the purposes of Gyro Ball / Electro Ball

    sp->effectiveSpeed[client1] = speed1;
    //sp->effectiveSpeed[client2] = speed2;

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 13: Speed calculations stop here for the purposes of Gyro Ball / Electro Ball\n");
    debug_printf("[CalcSpeed] %s's speed1: %d\n", client1Nickname, speed1);
    debug_printf("[CalcSpeed] %s's speed2: %d\n", client2Nickname, speed2);
#endif

    // Step 14: Trick Room

    if (sp->field_condition & FIELD_STATUS_TRICK_ROOM) {
        speed1 = 10000 - speed1;
        speed2 = 10000 - speed2;
    }

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 14: Trick Room\n");
    debug_printf("[CalcSpeed] %s's speed1: %d\n", client1Nickname, speed1);
    debug_printf("[CalcSpeed] %s's speed2: %d\n", client2Nickname, speed2);
#endif

    // Step 15: Apply Limit

    speed1 = speed1 % 8192;
    speed2 = speed2 % 8192;

#ifdef DEBUG_SPEED_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcSpeed] Step 15: Apply Limit\n");
    debug_printf("[CalcSpeed] %s's speed1: %d\n", client1Nickname, speed1);
    debug_printf("[CalcSpeed] %s's speed2: %d\n", client2Nickname, speed2);
    debug_printf("[CalcSpeed] End of calculating Speed Modifiers\n");
    debug_printf("\n=================\n");
#endif

    // End of calculating Speed Modifiers

    if (hold_effect1 == HOLD_EFFECT_PRIORITY_DOWN) {
        move_last1 = 1;
    }

    if (hold_effect2 == HOLD_EFFECT_PRIORITY_DOWN) {
        move_last2 = 1;
    }

    if (flag & CALCSPEED_FLAG_NO_PRIORITY)
    {
        priority1 = 0;
        priority2 = 0;
    }

    if (priority1 == priority2)
    {

        if ((move_last1) && (move_last2)) // both clients have lagging tail
        {
            if (speed1 > speed2) // if client1 is faster with lagging tail, it moves last
            {
                ret = 1; // client 2 moves first
            }
            else if ((speed1 == speed2) && (BattleRand(bw) & 1)) // random roll
            {
                ret = 2;
            }
        }
        else if ((move_last1) && (move_last2 == 0)) // client1 has lagging tail
        {
            ret = 1;
        }
        else if ((move_last1 == 0) && (move_last2)) // client2 has lagging tail
        {
            ret = 0;
        }
        else if ((ability1 == ABILITY_STALL) && (ability2 == ABILITY_STALL))
        {
            if (speed1 > speed2)
            {
                ret = 1;
            }
            else if ((speed1 == speed2) && (BattleRand(bw) & 1))
            {
                ret = 2;
            }
        }
        else if ((ability1 == ABILITY_STALL) && (ability2 != ABILITY_STALL))
        {
            ret = 1;
        }
        else if ((ability1 != ABILITY_STALL) && (ability2 == ABILITY_STALL))
        {
            ret = 0;
        }
        else
        {
            if (speed1 < speed2)
            {
                ret = 1;
            }
            if ((speed1 == speed2) && (BattleRand(bw) & 1))
            {
                ret = 2;
            }
        }
    }

    debug_printf("[CalcSpeed] s1=%d, s2=%d\n", speed1, speed2);
    return ret;
}