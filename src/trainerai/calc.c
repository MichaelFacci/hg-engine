#include "../../include/battle.h"
#include "../../include/config.h"
#include "../../include/debug.h"
#include "../../include/overlay.h"
#include "../../include/pokemon.h"
#include "../../include/types.h"
#include "../../include/constants/ability.h"
#include "../../include/constants/hold_item_effects.h"
#include "../../include/constants/file.h"
#include "../../include/constants/item.h"
#include "../../include/constants/move_effects.h"
#include "../../include/constants/moves.h"
#include "../../include/constants/species.h"
#include "../../include/q412.h"
#include "../../include/custom/custom_ai.h"


// this has been moved to src/battle/other_battle_calculators.c so it can be used in
extern const u8 HeldItemPowerUpTable[36][2];

extern const u16 PunchingMovesTable[24];

extern const u16 StrongJawMovesTable[10];

extern const u16 MegaLauncherMovesTable[7];

extern const u16 SharpnessMovesTable[24];

extern const u16 sLowKickWeightToPower[6][2];

static const u8 StatBoostModifiersTemp[][2] = {
    // numerator, denominator
   { 2, 8 },
   { 2, 7 },
   { 2, 6 },
   { 2, 5 },
   { 2, 4 },
   { 2, 3 },
   { 2, 2 },
   { 3, 2 },
   { 4, 2 },
   { 5, 2 },
   { 6, 2 },
   { 7, 2 },
   { 8, 2 },
};

extern const int typeToBerryMapping[18];

extern u8 TypeEffectivenessTable[][3];


void LONG_CALL FillDamageStructFromPartyMon(void* bw UNUSED, struct BattleStruct* sp, struct AI_sDamageCalc* monStruct, struct PartyPokemon* pp)
{
    monStruct->species = GetMonData(pp, MON_DATA_SPECIES, 0);
    monStruct->hp = GetMonData(pp, MON_DATA_HP, 0);
    monStruct->maxhp = GetMonData(pp, MON_DATA_MAXHP, 0);
	monStruct->percenthp = (monStruct->hp * 100) / monStruct->maxhp;
    monStruct->item = GetMonData(pp, MON_DATA_HELD_ITEM, 0);
    monStruct->item_held_effect = BattleItemDataGet(sp, monStruct->item, 1);
    monStruct->item_power = BattleItemDataGet(sp, monStruct->item, 2);

    monStruct->ability = GetMonData(pp, MON_DATA_ABILITY, 0);
    monStruct->sex = GetMonData(pp, MON_DATA_GENDER, 0);
    monStruct->type1 = GetMonData(pp, MON_DATA_TYPE_1, 0);
    monStruct->type2 = GetMonData(pp, MON_DATA_TYPE_2, 0);

    monStruct->condition = GetMonData(pp, MON_DATA_STATUS, 0);
    monStruct->condition = 0;
    monStruct->isGrounded = IsPartyPokemonGrounded(sp, pp);

    monStruct->speed = GetMonData(pp, MON_DATA_SPEED, 0);
    monStruct->weight = 1;

    monStruct->attack = GetMonData(pp, MON_DATA_SPECIAL_ATTACK, 0);
    monStruct->defense = GetMonData(pp, MON_DATA_SPECIAL_ATTACK, 0);
    monStruct->sp_attack = GetMonData(pp, MON_DATA_SPECIAL_ATTACK, 0);
    monStruct->sp_defense = GetMonData(pp, MON_DATA_SPECIAL_ATTACK, 0);

    for (int i = 0; i < 8; i++)
        monStruct->states[i] = 0; // Reset all states to 0

    monStruct->level = GetMonData(pp, MON_DATA_LEVEL, 0);
    monStruct->form = GetMonData(pp, MON_DATA_FORM, 0);
    //ArchiveDataLoadOfs(&monStruct->weight, ARC_DEX_LISTS, 1, PokeOtherFormMonsNoGet(monStruct->species, monStruct->form) * sizeof(s32), sizeof(s32));

    monStruct->hasMoldBreaker = FALSE;
    if (monStruct->ability == ABILITY_MOLD_BREAKER || monStruct->ability == ABILITY_TERAVOLT || monStruct->ability == ABILITY_TURBOBLAZE)
        monStruct->hasMoldBreaker = TRUE;

    monStruct->effect_of_moves = 0;
    monStruct->flashFireActivated = FALSE;

    monStruct->slowStartCount = 0;
    monStruct->furyCutterCount = 0;
    monStruct->metronomeTurns = 0;
}

void LONG_CALL FillDamageStructFromBattleMon(void* bw, struct BattleStruct* sp, struct AI_sDamageCalc* monStruct, int numSlot)
{
    monStruct->species = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_SPECIES, NULL);
    monStruct->hp = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_HP, NULL);
    monStruct->maxhp = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_MAX_HP, NULL);
    monStruct->percenthp = (monStruct->hp * 100) / monStruct->maxhp;
    monStruct->item = GetBattleMonItem(sp, numSlot);
    monStruct->item_held_effect = BattleItemDataGet(sp, monStruct->item, 1);
    monStruct->item_power = BattleItemDataGet(sp, monStruct->item, 2);

    monStruct->condition = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_MAX_CONDITION, NULL);
    monStruct->condition2 = sp->battlemon[numSlot].condition2;
    monStruct->isGrounded = IsClientGrounded(sp, numSlot);

    monStruct->ability = GetBattlerAbility(sp, numSlot);
    monStruct->sex = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_SEX, NULL);
    monStruct->type1 = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_TYPE1, NULL);
    monStruct->type2 = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_TYPE2, NULL);

    monStruct->speed = sp->effectiveSpeed[numSlot];
    monStruct->weight = GetPokemonWeight(bw, sp, numSlot);

    monStruct->form = sp->battlemon[numSlot].form_no;
    monStruct->attack = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_ATK, NULL);
    monStruct->defense = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_DEF, NULL);
    monStruct->sp_attack = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_SPATK, NULL);
    monStruct->sp_defense = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_SPDEF, NULL);
    for (int i = 0; i < 8; i++)
        monStruct->states[i] = 0; // Reset all states to 0

    monStruct->states[STAT_ATTACK] = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_STATE_ATK, NULL) - 6;
    monStruct->states[STAT_SPATK] = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_STATE_SPATK, NULL) - 6;
    monStruct->states[STAT_DEFENSE] = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_STATE_DEF, NULL) - 6;
    monStruct->states[STAT_SPDEF] = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_STATE_SPDEF, NULL) - 6;
    monStruct->states[STAT_SPEED] = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_STATE_SPE, NULL) - 6;
    monStruct->states[STAT_ACCURACY] = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_STATE_ACCURACY, NULL) - 6;
    monStruct->states[STAT_EVASION] = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_STATE_EVASIVENESS, NULL) - 6;

    monStruct->level = BattlePokemonParamGet(sp, numSlot, BATTLE_MON_DATA_LEVEL, NULL);
    monStruct->form = sp->battlemon[numSlot].form_no;

    monStruct->hasMoldBreaker = FALSE;
    if (monStruct->ability == ABILITY_MOLD_BREAKER || monStruct->ability == ABILITY_TERAVOLT || monStruct->ability == ABILITY_TURBOBLAZE)
        monStruct->hasMoldBreaker = TRUE;

    monStruct->effect_of_moves = sp->battlemon[numSlot].effect_of_moves;
    monStruct->flashFireActivated = FALSE;
    if (sp->battlemon[numSlot].moveeffect.flashFire)
        monStruct->flashFireActivated = TRUE;
		
    monStruct->slowStartCount = (sp->total_turn - sp->battlemon[numSlot].moveeffect.slowStartTurns);
    monStruct->furyCutterCount = sp->battlemon[numSlot].moveeffect.furyCutterCount;
    monStruct->metronomeTurns = sp->battlemon[numSlot].moveeffect.metronomeTurns;
    //monStruct->lastResortCount = sp->battlemon[numSlot].moveeffect.lastResortCount;
}

int LONG_CALL BattleAI_CalcBaseDamage(void* bw, struct BattleStruct* sp, int moveno, u32 side_cond, u32 field_cond, u16 pow, u8 type, u8 critical, u8 attackerSlot, u8 defenderSlot, struct AI_sDamageCalc* attacker, struct AI_sDamageCalc* defender)
{
    u8 i = 0;
    u32 p;
    int positiveStatBoosts = 0;
    u16 movepower;
    u8 movetype;
    u32 attack;
    u32 defense;
    u32 sp_attack;
    u32 sp_defense;
    u32 calculatedAttack = 0;
    u32 calculatedDefense = 0;
    u8 movesplit = GetMoveSplit(sp, moveno);
    u32 basePowerModifier = UQ412__1_0;
    u32 attackModifier = UQ412__1_0;
    u32 defenseModifier = UQ412__1_0;
    u32 baseDamage = 0;
    BOOL isDoubleBattle = (BattleTypeGet(bw) & (BATTLE_TYPE_MULTI | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TAG));

    /*
    if (!attacker->hasMoldBreaker && defender->ability == ABILITY_DISGUISE && defender->hp == defender->maxhp) //SPECIES_MIMIKYU
        return 0;

    if (!attacker->hasMoldBreaker && defender->ability == ABILITY_ICE_FACE && defender->form_no == 0) //SPECIES_EISCUE
        return 0;
    */
    struct BattleMove move = sp->moveTbl[moveno];
    movepower = move.power;
    movetype = GetAdjustedMoveTypeBasics(sp, moveno, attacker->ability, move.type);

    switch (moveno)
    {
        // Speed-based
    case MOVE_GYRO_BALL:
        if (attacker->speed == 0)
        {
            movepower = 1;
        }
        else
        {
            movepower = (25 * defender->speed) / attacker->speed;
            movepower = movepower > 150 ? 150 : movepower;
        }
        break;
    case MOVE_ELECTRO_BALL:
        if (defender->speed == 0)
        {
            movepower = 40;
        }
        else
        {
            switch (attacker->speed / defender->speed)
            {
            case 0:
                movepower = 40;
                break;
            case 1:
                movepower = 60;
                break;
            case 2:
                movepower = 80;
                break;
            case 3:
                movepower = 120;
                break;
                // 4 or higher
            default:
                movepower = 150;
                break;
            }
        }
        break;
        // Weight-based
    case MOVE_LOW_KICK:
    case MOVE_GRASS_KNOT: //TODO
        while (sLowKickWeightToPower[i][0] != 0xFFFF)
        {
            if (sLowKickWeightToPower[i][0] >= defender->weight)
            {
                break;
            }
            i++;
        }

        if (sLowKickWeightToPower[i][0] != 0xFFFF)
            movepower = sLowKickWeightToPower[i][1];
        else
            movepower = 120;
        break;
    case MOVE_HEAVY_SLAM:
    case MOVE_HEAT_CRASH:
        switch (attacker->weight / defender->weight)
        {
        case 2:
            movepower = 60;
            break;
        case 3:
            movepower = 80;
            break;
        case 4:
            movepower = 100;
            break;
        case 5:
            movepower = 120;
            break;
            // less than 2
        default:
            movepower = 40;
            break;
        }
        break;
        // HP-based
    case MOVE_ERUPTION:
    case MOVE_WATER_SPOUT:
        movepower = (150 * attacker->hp) / attacker->maxhp;
        break;
    case MOVE_FLAIL:
        p = (48 * attacker->hp) / attacker->maxhp;
        if (p >= 32) {
            movepower = 20;
            break;
        }
        if (p >= 17) {
            movepower = 40;
            break;
        }
        if (p >= 10) {
            movepower = 80;
            break;
        }
        if (p >= 5) {
            movepower = 100;
            break;
        }
        if (p >= 2) {
            movepower = 150;
            break;
        }
        if (p <= 1) {
            movepower = 200;
            break;
        }
        break;
    case MOVE_CRUSH_GRIP:
    case MOVE_WRING_OUT:
        // TODO: Check correctness
        movepower = QMul_RoundDown(120 * 100, (defender->hp * 4096) / defender->maxhp) / 100;
        break;
        //case MOVE_RETURN:
        //case MOVE_FRUSTRATION:
        //case MOVE_FURY_CUTTER:
        //case MOVE_ROLLOUT:
        //case MOVE_ICE_BALL:
        //case MOVE_SPIT_UP:
        //case MOVE_PUNISHMENT:
    case MOVE_STORED_POWER:
        positiveStatBoosts = 0;
        for (int stat = 0; stat < 8; stat++)
        {
            if (attacker->states[stat] > 0)
            {
                positiveStatBoosts += attacker->states[stat];
            }
        }
        movepower = 20 + 20 * positiveStatBoosts;
        break;
        // Dichotomous Base Power
    case MOVE_ACROBATICS:
        if (attacker->item == ITEM_NONE || attacker->item == ITEM_FLYING_GEM)
            movepower *= 2;
        break;
        //case MOVE_ASSURANCE:
        //case MOVE_REVENGE:
        //case MOVE_WATER_PLEDGE:
        //case MOVE_FIRE_PLEDGE:
        //case MOVE_GRASS_PLEDGE:
        //case MOVE_GUST:
        //case MOVE_TWISTER:
            // TODO: handle charging turn of Sky Drop
        if (defender->effect_of_moves & MOVE_EFFECT_FLAG_FLYING_IN_AIR)
            movepower *= 2;
        break;
    case MOVE_HEX:
        if (defender->condition & STATUS_ALL)
            movepower *= 2;
        break;
        //case MOVE_PAYBACK:
        //case MOVE_PURSUIT:
    case MOVE_ROUND:
        // TODO: Implement Round
        break;
    case MOVE_SMELLING_SALTS:
        if (defender->condition & STATUS_PARALYSIS)
            movepower *= 2;
        break;
    case MOVE_STOMPING_TANTRUM:
        // TODO: Implement Stomping Tantrum
        break;
    case MOVE_WAKE_UP_SLAP:
        if (defender->condition & STATUS_SLEEP)
            movepower *= 2;
        break;
    case MOVE_WEATHER_BALL:
        if (!CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, attackerSlot, ABILITY_CLOUD_NINE)
            && !CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, attackerSlot, ABILITY_AIR_LOCK))
        {
            if ((sp->field_condition & FIELD_CONDITION_WEATHER)
                && !(sp->field_condition & (WEATHER_STRONG_WINDS | WEATHER_SNOW_ANY)))
            {
                movepower *= 2;
            }
        }
        break;
    case MOVE_WATER_SHURIKEN:
        if (attacker->species == SPECIES_GRENINJA && attacker->form == 1)
            movepower = 20;
        break;
    case MOVE_BOLT_BEAK:
    case MOVE_FISHIOUS_REND:
        if (attacker->speed >= defender->speed)
            movepower *= 2;
        break;
    case MOVE_RISING_VOLTAGE:
        if ((sp->terrainOverlay.numberOfTurnsLeft > 0) && (sp->terrainOverlay.type == ELECTRIC_TERRAIN)) {
            movepower = 140;
        }
        break;
        // Item-based
    case MOVE_FLING:
        // TODO
        break;
    case MOVE_NATURAL_GIFT:
        break;
        // Other
    //case MOVE_BEAT_UP:
        break;
    case MOVE_ECHOED_VOICE:
        // TODO
        break;
    case MOVE_HIDDEN_POWER:
        movepower = 60;
        break;
    case MOVE_MAGNITUDE:
        movepower = 71;
        break;
        //case MOVE_PRESENT:
    case MOVE_TRIPLE_KICK:
    case MOVE_TRUMP_CARD:
    default:
        break;
    }

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 1. Custom BP\n");
    debug_printf("[CalcBaseDamage] moveno: %d\n", moveno);
    debug_printf("[CalcBaseDamage] movepower: %d\n", movepower);
#endif

    switch (moveno) {
    case MOVE_FACADE:
        if ((attacker->condition & STATUS_FACADE_BOOST)) {
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__2_0);
        }
        break;
    case MOVE_BRINE:
        if (defender->hp <= defender->maxhp / 2) {
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__2_0);
        }
        break;
    case MOVE_KNOCK_OFF:
        if (CanKnockOffApply(bw, sp)) { //TODO port to AI
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_5);
        }
        break;
    case MOVE_VENOSHOCK:
        if (defender->condition & STATUS_POISON_ALL) {
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__2_0);
        }
        break;
    case MOVE_RETALIATE:
        // TODO
        break;
        //case MOVE_FUSION_FLARE:
        //case MOVE_FUSION_BOLT:
    case MOVE_GRAV_APPLE:
        if ((sp->field_condition & FIELD_STATUS_GRAVITY))
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_5);
        break;
    case MOVE_EXPANDING_FORCE:
        if ((sp->terrainOverlay.numberOfTurnsLeft > 0) && (sp->terrainOverlay.type == MISTY_TERRAIN))
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_5);
        break;
    case MOVE_MISTY_EXPLOSION:
        if ((sp->terrainOverlay.numberOfTurnsLeft > 0) && (sp->terrainOverlay.type == MISTY_TERRAIN))
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_5);
        break;
        // case MOVE_LASH_OUT:
    default:
        break;
    }

    /* Not considered in AI:  Helping Hand, Me First */

    // handle Charge
    if ((attacker->effect_of_moves & MOVE_EFFECT_FLAG_CHARGE) && (movetype == TYPE_ELECTRIC))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__2_0);


    if ((CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, attackerSlot, ABILITY_CLOUD_NINE) == 0)
        && (CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, attackerSlot, ABILITY_AIR_LOCK) == 0))
    {
        if ((field_cond & (FIELD_STATUS_FOG | WEATHER_HAIL_ANY | WEATHER_SANDSTORM_ANY | WEATHER_RAIN_ANY | WEATHER_SNOW_ANY))
            && (moveno == MOVE_SOLAR_BEAM || moveno == MOVE_SOLAR_BLADE))
        {
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__0_5);
        }
    }

    // handle Terrain overlays
    if (sp->terrainOverlay.numberOfTurnsLeft > 0)
    {
        switch (sp->terrainOverlay.type)
        {
        case GRASSY_TERRAIN:
            if (attacker->isGrounded && movetype == TYPE_GRASS)
                basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_3);
            if (moveno == MOVE_EARTHQUAKE || moveno == MOVE_MAGNITUDE || moveno == MOVE_BULLDOZE)
                basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__0_5);
            break;
        case ELECTRIC_TERRAIN:
            if (attacker->isGrounded && movetype == TYPE_ELECTRIC)
                basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_3);
            break;
        case MISTY_TERRAIN:
            if (defender->isGrounded && movetype == TYPE_DRAGON)
                basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__0_5);
            break;
        case PSYCHIC_TERRAIN:
            if (attacker->isGrounded && movetype == TYPE_PSYCHIC)
                basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_3);
            break;
        default:
            break;
        }
    }

    /* Not considered in AI:  Mud Sport, Water Sport, Dark Aura, Fairy Aura, Aura Beak  */

     // handle Rivalry
    if (attacker->ability == ABILITY_RIVALRY)
    {
        if (attacker->sex == defender->sex && attacker->sex != POKEMON_GENDER_UNKNOWN)
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_25);
        else if (attacker->sex != defender->sex && attacker->sex != POKEMON_GENDER_UNKNOWN && defender->sex != POKEMON_GENDER_UNKNOWN)
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__0_75);
    }

    // handle Aerilate, Pixilate, etc - 20% boost if a Normal type move was changed to a xxx type move. Does not boost xxx type moves themselves
    if (MoveIsAffectedByNormalizeVariants(moveno))
    {
        if (attacker->ability == ABILITY_AERILATE && movetype == TYPE_FLYING && move.type == TYPE_NORMAL)
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);

        if (attacker->ability == ABILITY_PIXILATE && movetype == TYPE_FAIRY && move.type == TYPE_NORMAL)
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);

        if (attacker->ability == ABILITY_GALVANIZE && movetype == TYPE_ELECTRIC && move.type == TYPE_NORMAL)
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);

        if (attacker->ability == ABILITY_REFRIGERATE && movetype == TYPE_ICE && move.type == TYPE_NORMAL)
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);

        if (attacker->ability == ABILITY_NORMALIZE && movetype == TYPE_NORMAL)
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);
    }

    // handle Iron Fist
    if ((attacker->ability == ABILITY_IRON_FIST) && IsElementInArray(PunchingMovesTable, (u16*)&moveno, NELEMS(PunchingMovesTable), sizeof(PunchingMovesTable[0])))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);

    if ((attacker->ability == ABILITY_RECKLESS)
        && (move.effect == MOVE_EFFECT_CRASH_ON_MISS)
        && (move.effect == MOVE_EFFECT_RECOIL_QUARTER)
        && (move.effect == MOVE_EFFECT_RECOIL_THIRD)
        && (move.effect == MOVE_EFFECT_RECOIL_BURN_HIT)
        && (move.effect == MOVE_EFFECT_RECOIL_PARALYZE_HIT)
        && (move.effect == MOVE_EFFECT_RECOIL_HALF)
        && (move.effect == MOVE_EFFECT_CONFUSE_AND_CRASH_IF_MISS))
    {
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);
    }

    // handle Sheer Force
    if (attacker->ability == ABILITY_SHEER_FORCE && IsMoveBoostedBySheerForce(moveno, move.effect))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_3);

    // Sand Force boosts damage in sand for certain move types
    if ((attacker->ability == ABILITY_SAND_FORCE)
        && (field_cond & WEATHER_SANDSTORM_ANY)
        && (movetype == TYPE_GROUND || movetype == TYPE_ROCK || movetype == TYPE_STEEL))
    {
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_3);
    }

    // handle Analytic
    if (attacker->ability == ABILITY_ANALYTIC)
    {
        if (attacker->speed < defender->speed)
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_3);
    }

    // handle Tough Claws
    if ((attacker->ability == ABILITY_TOUGH_CLAWS) && (BattleAI_IsContactBeingMade(sp, attacker->ability, attacker->item_held_effect, moveno)))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_3);

    // handle Technician
    if ((attacker->ability == ABILITY_TECHNICIAN) && (moveno != MOVE_STRUGGLE) && (movepower <= 60))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_5);

    // handle Flare Boost
    if ((attacker->ability == ABILITY_FLARE_BOOST) && (attacker->condition & STATUS_BURN))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_5);

    // handle Toxic Boost
    if ((attacker->ability == ABILITY_TOXIC_BOOST) && (attacker->condition & (STATUS_BAD_POISON | STATUS_POISON)))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_5);


    // handle Strong Jaw
    if ((attacker->ability == ABILITY_STRONG_JAW) && IsElementInArray(StrongJawMovesTable, (u16*)&moveno, NELEMS(StrongJawMovesTable), sizeof(StrongJawMovesTable[0])))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_5);

    // handle Mega Launcher
    if ((attacker->ability == ABILITY_MEGA_LAUNCHER) && IsElementInArray(MegaLauncherMovesTable, (u16*)&moveno, NELEMS(MegaLauncherMovesTable), sizeof(MegaLauncherMovesTable[0])))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_5);


    // handle Sharpness
    if ((attacker->ability == ABILITY_SHARPNESS) && IsElementInArray(SharpnessMovesTable, (u16*)&moveno, NELEMS(SharpnessMovesTable), sizeof(SharpnessMovesTable[0])))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_5);



    // handle Punk Rock
    if (attacker->ability == ABILITY_PUNK_ROCK && IsMoveSoundBased(sp->current_move_index))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_3);

    // handle Steely Spirit for the attacker--can stack //TODO
    if (movetype == TYPE_STEEL && attacker->ability == ABILITY_STEELY_SPIRIT)
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_5);

    if (isDoubleBattle)
    {
        // handle Power Spot
        if (GetBattlerAbility(sp, BATTLER_ALLY(attackerSlot)) == ABILITY_POWER_SPOT)
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_3);

        // Handle Battery
        if ((GetBattlerAbility(sp, BATTLER_ALLY(attackerSlot)) == ABILITY_BATTERY) && (movesplit == SPLIT_SPECIAL))
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_33);

        // handle Steely Spirit for the ally
        if (movetype == TYPE_STEEL && GetBattlerAbility(sp, BATTLER_ALLY(attackerSlot)) == ABILITY_STEELY_SPIRIT)
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_5);
    }

    if (movetype == TYPE_FIRE && defender->ability == ABILITY_DRY_SKIN && !attacker->hasMoldBreaker)
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_25);

    //items

    if ((attacker->item_held_effect == HOLD_EFFECT_POWER_UP_PHYS) && (movesplit == SPLIT_PHYSICAL))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_1);


    if ((attacker->item_held_effect == HOLD_EFFECT_POWER_UP_SPEC) && (movesplit == SPLIT_SPECIAL))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_1);

    // type boosting held items
    {
        u8 typeBoostElement[2] = { attacker->item_held_effect, movetype };
        if (IsElementInArray(HeldItemPowerUpTable, typeBoostElement, NELEMS(HeldItemPowerUpTable), sizeof(typeBoostElement)))
            basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);
    }

    // handle Adamant Orb
    if ((attacker->item_held_effect == HOLD_EFFECT_DIALGA_BOOST)
        && ((movetype == TYPE_DRAGON) || (movetype == TYPE_STEEL))
        && (attacker->species == SPECIES_DIALGA))
    {
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);
    }

    // handle Lustrous Orb
    if ((attacker->item_held_effect == HOLD_EFFECT_PALKIA_BOOST)
        && ((movetype == TYPE_DRAGON) || (movetype == TYPE_WATER))
        && (attacker->species == SPECIES_PALKIA))
    {
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);
    }

    // handle Griseous Orb
    if ((attacker->item_held_effect == HOLD_EFFECT_GIRATINA_BOOST)
        && ((movetype == TYPE_DRAGON) || (movetype == TYPE_GHOST))
        && (attacker->species == SPECIES_GIRATINA))
    {
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);
    }

    // handle Adamant Crystal, lustrous globe & griseous core
    if ((attacker->item_held_effect == HOLD_EFFECT_DIALGA_BOOST_AND_TRANSFORM)
        && ((movetype == TYPE_DRAGON) || (movetype == TYPE_STEEL))
        && (attacker->species == SPECIES_DIALGA))
    {
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);
    }

    // handle Lustrous Globe
    if ((attacker->item_held_effect == HOLD_EFFECT_PALKIA_BOOST_AND_TRANSFORM)
        && ((movetype == TYPE_DRAGON) || (movetype == TYPE_WATER))
        && (attacker->species == SPECIES_PALKIA))
    {
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);
    }

    // handle Griseous Core
    if ((attacker->item_held_effect == HOLD_EFFECT_GIRATINA_BOOST_AND_TRANSFORM)
        && ((movetype == TYPE_DRAGON) || (movetype == TYPE_GHOST))
        && (attacker->species == SPECIES_GIRATINA))
    {
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);
    }

    // handle Soul Dew
    if ((attacker->item_held_effect == HOLD_EFFECT_LATI_SPECIAL)
        && ((attacker->species == SPECIES_LATIOS) || (attacker->species == SPECIES_LATIAS))
        && (movetype == TYPE_DRAGON || movetype == TYPE_PSYCHIC))
    {
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);
    }

    // handle Gems
    /*
    if (IS_ITEM_GEM(attacker->item) && attacker->item_power == movetype)
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_3);
    */

    // handle Punching Glove
    if ((attacker->item_held_effect == HOLD_EFFECT_INCREASE_PUNCHING_MOVE_DMG) && IsElementInArray(PunchingMovesTable, (u16*)&moveno, NELEMS(PunchingMovesTable), sizeof(PunchingMovesTable[0])))
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_1_BUT_HIGHER);

    // handle Wellspring Mask
    if ((attacker->item_held_effect == HOLD_EFFECT_WELLSPRING_MASK)
        && (attacker->species == SPECIES_OGERPON)
        && (attacker->form == 1 || attacker->form == 5))
    {
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);
    }

    // handle Hearthflame Mask
    if ((attacker->item_held_effect == HOLD_EFFECT_HEARTHFLAME_MASK)
        && (attacker->species == SPECIES_OGERPON)
        && (attacker->form == 2 || attacker->form == 6))
    {
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);
    }

    // handle Cornerstone Mask
    if ((attacker->item_held_effect == HOLD_EFFECT_CORNERSTONE_MASK)
        && (attacker->species == SPECIES_OGERPON)
        && (attacker->form == 3 || attacker->form == 7))
    {
        basePowerModifier = QMul_RoundUp(basePowerModifier, UQ412__1_2);
    }

    movepower = QMul_RoundDown(movepower, basePowerModifier);
    movepower = movepower < 1 ? 1 : movepower;
    movepower = movepower % 65536;

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 2. Base Power Modifiers\n");
    debug_printf("[CalcBaseDamage] basePowerModifier: %d\n", basePowerModifier);
    debug_printf("[CalcBaseDamage] movepower: %d\n", movepower);
#endif

    // Step 3.1. handle Unaware
    if (!attacker->hasMoldBreaker && defender->ability == ABILITY_UNAWARE)
    {
        attacker->states[STAT_ATTACK] = 0;
        attacker->states[STAT_SPATK] = 0;
    }

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 3.1. handle Unaware\n");
    debug_printf("[CalcBaseDamage] attacker->atkstate: %d\n", attacker->states[STAT_ATTACK]);
    debug_printf("[CalcBaseDamage] attacker->spatkstate: %d\n", attacker->states[STAT_SPATK]);
#endif

    // Step 3.2. handle Foul Play
    if (moveno == MOVE_FOUL_PLAY)
    {
        attacker->attack = defender->attack;
        attacker->states[STAT_ATTACK] = defender->states[STAT_ATTACK];
    }

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 3.2. handle Foul Play\n");
    debug_printf("[CalcBaseDamage] attacker->attack: %d\n", attacker->attack);
    debug_printf("[CalcBaseDamage] attacker->atkstate: %d\n", attacker->atkstate);
#endif

    // Step 3.3. Critical hit
    if (critical > 1) {
        // critical hits ignore attacker attack drops
        attacker->states[STAT_ATTACK] = attacker->states[STAT_ATTACK] < 0 ? 0 : attacker->states[STAT_ATTACK];
        attacker->states[STAT_SPATK] = attacker->states[STAT_SPATK] < 0 ? 0 : attacker->states[STAT_SPATK];
    }

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 3.3. Critical hit\n");
    debug_printf("[CalcBaseDamage] attacker->atkstate: %d\n", attacker->states[STAT_ATTACK]);
    debug_printf("[CalcBaseDamage] attacker->spatkstate: %d\n", attacker->states[STAT_SPATK]);
#endif

    // Step 3.4. Attack boosts/drops
    attack = attacker->attack * StatBoostModifiersTemp[attacker->states[STAT_ATTACK] + 6][0];
    attack /= StatBoostModifiersTemp[attacker->states[STAT_ATTACK] + 6][1];
    attack = attack % 65536;

    sp_attack = attacker->sp_attack * StatBoostModifiersTemp[attacker->states[STAT_SPATK] + 6][0];
    sp_attack /= StatBoostModifiersTemp[attacker->states[STAT_SPATK] + 6][1];
    sp_attack = sp_attack % 65536;

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 3.4. Attack boosts/drops\n");
    debug_printf("[CalcBaseDamage] attack: %d\n", attack);
    debug_printf("[CalcBaseDamage] sp_attack: %d\n", sp_attack);
#endif

    // Step 3.5. Hustle
    if (attacker->ability == ABILITY_HUSTLE)
        attack = QMul_RoundDown(attack, UQ412__1_5);

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 3.5. Hustle\n");
    debug_printf("[CalcBaseDamage] attack: %d\n", attack);
#endif

    switch (movesplit) {
    case SPLIT_PHYSICAL:
        calculatedAttack = attack;
        break;
    case SPLIT_SPECIAL:
        calculatedAttack = sp_attack;
        break;

    default:
        GF_ASSERT(movesplit != SPLIT_STATUS);
        break;
    }

    // Abilities
            // handle Slow Start
    if (attacker->ability == ABILITY_SLOW_START && (attacker->slowStartCount < 5) && (movesplit == SPLIT_PHYSICAL || MoveIsZMove(moveno)))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__0_5);

    // handle Defeatist
    if ((attacker->ability == ABILITY_DEFEATIST) && (attacker->hp <= attacker->maxhp / 2))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__0_5);

    // handle weather boosts
    if ((CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, attackerSlot, ABILITY_CLOUD_NINE) == 0)
        && (CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, attackerSlot, ABILITY_AIR_LOCK) == 0))
    {
        if ((field_cond & WEATHER_SUNNY_ANY) && (attacker->ability == ABILITY_SOLAR_POWER) && (movesplit == SPLIT_SPECIAL))
            attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);
        if ((field_cond & WEATHER_SUNNY_ANY)
            && (attacker->ability == ABILITY_FLOWER_GIFT || (isDoubleBattle && GetBattlerAbility(sp, BATTLER_ALLY(attackerSlot)) == ABILITY_FLOWER_GIFT))
            && (movesplit == SPLIT_PHYSICAL))
        {
            attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);
        }
    }

    // handle Guts
    if ((attacker->ability == ABILITY_GUTS) && (attacker->condition) && (movesplit == SPLIT_PHYSICAL))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);

    // handle Overgrow
    if ((movetype == TYPE_GRASS) && (attacker->ability == ABILITY_OVERGROW) && (attacker->hp <= attacker->maxhp / 3))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);

    // handle Blaze
    if ((movetype == TYPE_FIRE) && (attacker->ability == ABILITY_BLAZE) && (attacker->hp <= attacker->maxhp / 3))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);

    // handle Torrent
    if ((movetype == TYPE_WATER) && (attacker->ability == ABILITY_TORRENT) && (attacker->hp <= attacker->maxhp / 3))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);

    // handle Swarm
    if ((movetype == TYPE_BUG) && (attacker->ability == ABILITY_SWARM) && (attacker->hp <= attacker->maxhp / 3))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);

    // handle Flash Fire
    if (attacker->flashFireActivated && (movetype == TYPE_FIRE))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);

    // handle Plus/Minus
    if (((attacker->ability == ABILITY_PLUS) || (attacker->ability == ABILITY_MINUS))
        && (CheckSideAbility(bw, sp, CHECK_ABILITY_SAME_SIDE_HP, attackerSlot, ABILITY_MINUS)
            || CheckSideAbility(bw, sp, CHECK_ABILITY_SAME_SIDE_HP, attackerSlot, ABILITY_PLUS))
        && (movesplit == SPLIT_SPECIAL))
    {
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);
    }

    // handle Steelworker
    if (attacker->ability == ABILITY_STEELWORKER && (movetype == TYPE_STEEL))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);

    // handle Huge Power/Pure Power
    if (((attacker->ability == ABILITY_HUGE_POWER) || (attacker->ability == ABILITY_PURE_POWER)) && (movesplit == SPLIT_PHYSICAL))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__2_0);

    // handle Water Bubble
    if ((attacker->ability == ABILITY_WATER_BUBBLE) && (movetype == TYPE_WATER))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__2_0);

    // handle Gorilla Tactics
    if (attacker->ability == ABILITY_GORILLA_TACTICS)
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);

    // handle Dragon's Maw
    // TODO: confirm location
    if (attacker->ability == ABILITY_DRAGONS_MAW && (movetype == TYPE_DRAGON))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);

    // handle Transistor
    // TODO: confirm location
    if (attacker->ability == ABILITY_TRANSISTOR && (movetype == TYPE_ELECTRIC))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_3);

    // handle Rocky Payload
    if (attacker->ability == ABILITY_ROCKY_PAYLOAD && (movetype == TYPE_ROCK))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);

    if ((movetype == TYPE_FIRE) && !attacker->hasMoldBreaker && defender->ability == ABILITY_HEATPROOF)
        attackModifier = QMul_RoundUp(attackModifier, UQ412__0_5);

    // handle Thick Fat
    if ((movetype == TYPE_FIRE || movetype == TYPE_ICE) && !attacker->hasMoldBreaker && defender->ability == ABILITY_THICK_FAT)
        attackModifier = QMul_RoundUp(attackModifier, UQ412__0_5);

    // handle Water Bubble
    if ((defender->ability == ABILITY_WATER_BUBBLE) && (movetype == TYPE_FIRE))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__0_5);

    // handle Purifying Salt
    if ((defender->ability == ABILITY_PURIFYING_SALT) && (movetype == TYPE_GHOST))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__0_5);

    // Items

    // handle Choice Band
    if ((attacker->item_held_effect == HOLD_EFFECT_CHOICE_ATK) && (movesplit == SPLIT_PHYSICAL))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);

    // handle Choice Specs
    if ((attacker->item_held_effect == HOLD_EFFECT_CHOICE_SPATK) && (movesplit == SPLIT_SPECIAL))
        attackModifier = QMul_RoundUp(attackModifier, UQ412__1_5);

    // handle Thick Club
    if ((attacker->item_held_effect == HOLD_EFFECT_CUBONE_ATK_UP)
        && ((attacker->species == SPECIES_CUBONE) || (attacker->species == SPECIES_MAROWAK))
        // it�s not a Ditto/Smeargle/Mew Transformed into the species
        && !(attacker->condition2 & STATUS2_TRANSFORMED)
        && (movesplit == SPLIT_PHYSICAL))
    {
        attackModifier = QMul_RoundUp(attackModifier, UQ412__2_0);
    }

    // handle Deep Sea Tooth
    if ((attacker->item_held_effect == HOLD_EFFECT_CLAMPERL_SPATK)
        && (attacker->species == SPECIES_CLAMPERL)
        // it�s not a Ditto/Smeargle/Mew Transformed into the species
        && !(attacker->condition2 & STATUS2_TRANSFORMED)
        && (movesplit == SPLIT_SPECIAL))
    {
        attackModifier = QMul_RoundUp(attackModifier, UQ412__2_0);
    }

    // handle Light Ball
    if ((attacker->item_held_effect == HOLD_EFFECT_PIKA_SPATK_UP)
        && (attacker->species == SPECIES_PIKACHU)
        // it�s not a Ditto/Smeargle/Mew Transformed into the species
        && !(attacker->condition2 & STATUS2_TRANSFORMED))
    {
        attackModifier = QMul_RoundUp(attackModifier, UQ412__2_0);
    }

    // Apply the chained modifier to the current attack. That is, multiply the current attack by the chained attack modifiers, divide by 4096, and pokeRound the result. If the current attack would now be less than 1, make it 1. Finally, if the attack is greater than 65,535, make it the attack modulo 65,536 (attack % 65536).
    calculatedAttack = QMul_RoundDown(calculatedAttack, attackModifier);
    calculatedAttack = calculatedAttack < 1 ? 1 : calculatedAttack;
    calculatedAttack = calculatedAttack % 65536;

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 3.6. Attack Modifiers\n");
    debug_printf("[CalcBaseDamage] attackModifier: %d\n", attackModifier);
    debug_printf("[CalcBaseDamage] calculatedAttack: %d\n", calculatedAttack);
#endif

    //TODO
    // Handle Tablets of Ruin
    // Handle Vessel of Ruin

    //=====Step 4. Defense Modifiers=====

    // handle Terrain+Seeds. This works for switching, since item is not consumed
    if (sp->terrainOverlay.numberOfTurnsLeft > 0
        && ((sp->terrainOverlay.type == ELECTRIC_TERRAIN && defender->item_held_effect == HOLD_EFFECT_BOOST_DEF_ON_ELECRIC_TERRAIN)
            || (sp->terrainOverlay.type == GRASSY_TERRAIN && defender->item_held_effect == HOLD_EFFECT_BOOST_DEF_ON_GRASSY_TERRAIN)))
    {
		defender->states[STAT_DEFENSE] = defender->states[STAT_DEFENSE] + 1;
    }
    if (sp->terrainOverlay.numberOfTurnsLeft > 0
        && ((sp->terrainOverlay.type == MISTY_TERRAIN && defender->item_held_effect == HOLD_EFFECT_BOOST_SPDEF_ON_PSYCHIC_TERRAIN)
            || (sp->terrainOverlay.type == PSYCHIC_TERRAIN && defender->item_held_effect == HOLD_EFFECT_BOOST_SPDEF_ON_PSYCHIC_TERRAIN)))
    {
        defender->states[STAT_SPDEF] = defender->states[STAT_SPDEF] + 1;
    }

    // Step 4.1. handle Unaware
    // Step 4.2. Chip Away / Sacred Sword
    if (attacker->ability == ABILITY_UNAWARE || moveno == MOVE_CHIP_AWAY || moveno == MOVE_SACRED_SWORD)
    {
        defender->states[STAT_DEFENSE] = 0;
        defender->states[STAT_SPDEF] = 0;
    }

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 4.2. Chip Away / Sacred Sword\n");
    debug_printf("[CalcBaseDamage] defender->defstate: %d\n", defender->states[STAT_DEFENSE]);
    debug_printf("[CalcBaseDamage] defender->spdefstate: %d\n", defender->states[STAT_SPDEF]);
#endif

    // Step 4.3. Psyshock / Psystrike / Secret Sword
    if (moveno == MOVE_PSYSHOCK || moveno == MOVE_PSYSTRIKE || moveno == MOVE_SECRET_SWORD)
        defender->sp_defense = defender->defense;

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 4.3. Psyshock / Psystrike / Secret Sword\n");
    debug_printf("[CalcBaseDamage] defender->sp_defense: %d\n", defender->sp_defense);
#endif

    // Step 4.4. Wonder Room
    // TODO

    // Step 4.5. Critical hit
    if (critical > 1) {
        // critical hits ignore defender's stat boosts
        defender->states[STAT_DEFENSE] = defender->states[STAT_DEFENSE] > 0 ? 0 : defender->states[STAT_DEFENSE];
        defender->states[STAT_SPDEF] = defender->states[STAT_SPDEF] > 0 ? 0 : defender->states[STAT_SPDEF];
    }

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 4.5. Critical hit\n");
    debug_printf("[CalcBaseDamage] defender->defstate: %d\n", defender->states[STAT_DEFENSE]);
    debug_printf("[CalcBaseDamage] defender->spdefstate: %d\n", defender->states[STAT_SPDEF]);
#endif

    // Step 4.6. Defense boosts/drops
    defense = defender->defense * StatBoostModifiersTemp[defender->states[STAT_DEFENSE] + 6][0];
    defense /= StatBoostModifiersTemp[defender->states[STAT_DEFENSE] + 6][1];
    defense = defense % 65536;

    sp_defense = defender->sp_defense * StatBoostModifiersTemp[defender->states[STAT_SPDEF] + 6][0];
    sp_defense /= StatBoostModifiersTemp[defender->states[STAT_SPDEF] + 6][1];
    sp_defense = sp_defense % 65536;

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 4.6. Defense boosts/drops\n");
    debug_printf("[CalcBaseDamage] defense: %d\n", defense);
    debug_printf("[CalcBaseDamage] sp_defense: %d\n", sp_defense);
#endif

    // Step 4.7. Sandstorm + Rock-type
    if ((CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, attackerSlot, ABILITY_CLOUD_NINE) == 0)
        && (CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, attackerSlot, ABILITY_AIR_LOCK) == 0))
    {
        if ((field_cond & WEATHER_SANDSTORM_ANY) && ((defender->type1 == TYPE_ROCK) || (defender->type2 == TYPE_ROCK)))
        {
            sp_defense = QMul_RoundDown(sp_defense, UQ412__1_5);
        }
        if ((field_cond & WEATHER_SNOW_ANY) && ((defender->type1 == TYPE_ICE) || (defender->type2 == TYPE_ICE)))
        {
            defense = QMul_RoundDown(defense, UQ412__1_5);
        }
    }

#ifdef DEBUG_DAMAGE_CALC
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 4.7. Sandstorm + Rock-type\n");
    debug_printf("[CalcBaseDamage] defense: %d\n", defense);
    debug_printf("[CalcBaseDamage] sp_defense: %d\n", sp_defense);
#endif

    switch (movesplit) {
    case SPLIT_PHYSICAL:
        calculatedDefense = defense;
        break;
    case SPLIT_SPECIAL:
        calculatedDefense = sp_defense;
        break;

    default:
        GF_ASSERT(movesplit != SPLIT_STATUS);
        break;
    }

    // Step 4.8. Remaining defense modifiers

    // Abilities
            // handle weather boosts
    if ((CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, 0, ABILITY_CLOUD_NINE) == 0)
        && (CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, 0, ABILITY_AIR_LOCK) == 0))
    {
        if ((field_cond & WEATHER_SUNNY_ANY) && movesplit == SPLIT_SPECIAL
            && (GetBattlerAbility(sp, defenderSlot) == ABILITY_FLOWER_GIFT
                || (isDoubleBattle && GetBattlerAbility(sp, BATTLER_ALLY(defenderSlot)) == ABILITY_FLOWER_GIFT)))
        {
            defenseModifier = QMul_RoundUp(defenseModifier, UQ412__1_5);
        }
    }
    // handle Marvel Scale
    if (!attacker->hasMoldBreaker && defender->ability == ABILITY_MARVEL_SCALE && (defender->condition) && (movesplit == SPLIT_PHYSICAL))
        defenseModifier = QMul_RoundUp(defenseModifier, UQ412__1_5);

    // handle Grass Pelt
    if (!attacker->hasMoldBreaker && defender->ability == ABILITY_GRASS_PELT && (sp->terrainOverlay.type == GRASSY_TERRAIN && sp->terrainOverlay.numberOfTurnsLeft > 0)
        && (movesplit == SPLIT_PHYSICAL))
    {
        defenseModifier = QMul_RoundUp(defenseModifier, UQ412__1_5);
    }

    // handle Fur Coat
    if (!attacker->hasMoldBreaker && defender->ability == ABILITY_FUR_COAT && (movesplit == SPLIT_PHYSICAL))
        defenseModifier = QMul_RoundUp(defenseModifier, UQ412__2_0);


    // Items
    // handle Eviolite
    if (defender->item_held_effect == HOLD_EFFECT_EVIOLITE)
    {
        u16 speciesWithForm;
        speciesWithForm = PokeOtherFormMonsNoGet(defender->species, defender->form);

        struct Evolution* evoTable;
        evoTable = sys_AllocMemory(0, MAX_EVOS_PER_POKE * sizeof(struct Evolution));
        ArchiveDataLoad(evoTable, ARC_EVOLUTIONS, speciesWithForm);

        // If a Pok�mon has any evolutions, there should be an entry at the top that isn't EVO_NONE.
        // In that case, the Pok�mon is capable of evolving, and so the effect of Eviolite should apply.
        if (evoTable[0].method != EVO_NONE) {
            defenseModifier = QMul_RoundUp(defenseModifier, UQ412__1_5);
        }

        sys_FreeMemoryEz(evoTable);
    }

    // handle Assault Vest
    if ((defender->item_held_effect == HOLD_EFFECT_SPDEF_BOOST_NO_STATUS_MOVES) && (movesplit == SPLIT_SPECIAL))
        defenseModifier = QMul_RoundUp(defenseModifier, UQ412__1_5);

    // handle Deep Sea Scale
    if ((defender->item_held_effect == HOLD_EFFECT_CLAMPERL_SPDEF)
        && (defender->species == SPECIES_CLAMPERL)
        // it�s not a Ditto/Smeargle/Mew Transformed into the species
        && !(defender->condition2 & STATUS2_TRANSFORMED)
        && (movesplit == SPLIT_SPECIAL))
    {
        defenseModifier = QMul_RoundUp(defenseModifier, UQ412__2_0);
    }

    // handle Metal Powder
    if ((defender->item_held_effect == HOLD_EFFECT_DITTO_DEF_UP)
        && (defender->species == SPECIES_DITTO)
        // it�s not a Ditto/Smeargle/Mew Transformed into the species
        && !(defender->condition2 & STATUS2_TRANSFORMED)
        && (movesplit == SPLIT_PHYSICAL))
    {
        defenseModifier = QMul_RoundUp(defenseModifier, UQ412__2_0);
    }


    // Apply the chained modifier to the starting defense. That is, multiply the starting defense by the chained defense modifiers, divide by 4096, and pokeRound the result. If the current defense would now be less than 1, make it 1. Finally, if the defense is greater than 65,535, make it the defense modulo 65,536 (defense % 65536). If the defense stat is 0 because of this modifier, the result of base damage will always be 2.
    calculatedDefense = QMul_RoundDown(calculatedDefense, defenseModifier);
    calculatedDefense = calculatedDefense < 1 ? 1 : calculatedDefense;
    calculatedDefense = calculatedDefense % 65536;

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 4.8. Defense Modifiers\n");
    debug_printf("[CalcBaseDamage] defenseModifier: %d\n", defenseModifier);
    debug_printf("[CalcBaseDamage] calculatedDefense: %d\n", calculatedDefense);
#endif
    //TODO
    // Handle Sword of Ruin
    // Handle Beads of Ruin

    //=====End of Step 4=====

    //=====Step 5. Base Damage=====

    baseDamage = ((2 * attacker->level / 5) + 2);

    // https://www.smogon.com/forums/threads/ultra-sun-ultra-moon-battle-mechanics-research-read-post-2.3620030/post-8198555
    if (calculatedDefense != 0) {
        baseDamage = (baseDamage * movepower * calculatedAttack / calculatedDefense);
    }
    else {
        baseDamage = 0;
    }

    baseDamage = (baseDamage / 50) + 2;

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 5. Base Damage\n");
    debug_printf("[CalcBaseDamage] baseDamage: %d\n", baseDamage);
#endif

    //=====End of Step 5=====

    return baseDamage;
}

int LONG_CALL BattleAI_CalcDamage(void* bw, struct BattleStruct* sp, int moveno, u32 side_cond, u32 field_cond, u16 pow, u8 type, u8 critical, u8 attackerSlot, u8 defenderSlot, struct AI_damage* damages, struct AI_sDamageCalc* attacker, struct AI_sDamageCalc* defender)
{

    u8 movetype;
    u8 movesplit = GetMoveSplit(sp, moveno);
    u32 damage = 0;
    u32 moveEffectiveness;
    u32 finalModifier = UQ412__1_0;

    struct BattleMove move = sp->moveTbl[moveno];
    movetype = GetAdjustedMoveTypeBasics(sp, moveno, attacker->ability, move.type);

    if (!attacker->hasMoldBreaker)
    {
        switch (defender->ability)
        {
        case ABILITY_FLASH_FIRE:
        case ABILITY_WELL_BAKED_BODY:
            if (movetype == TYPE_FIRE)
                return 0;
            break;
        case ABILITY_LIGHTNING_ROD:
        case ABILITY_VOLT_ABSORB:
        case ABILITY_MOTOR_DRIVE:
            if (movetype == TYPE_ELECTRIC)
                return 0;
            break;
        case ABILITY_WATER_ABSORB:
        case ABILITY_STORM_DRAIN:
        case ABILITY_DRY_SKIN:
            if (movetype == TYPE_WATER)
                return 0;
            break;
        case ABILITY_SAP_SIPPER:
            if (movetype == TYPE_GRASS)
                return 0;
            break;
        case ABILITY_LEVITATE:
        case ABILITY_EARTH_EATER:
            if (movetype == TYPE_GROUND)
                return 0;
            break;
        case ABILITY_BULLETPROOF:
            if (IsBallOrBombMove(moveno))
                return 0;
            break;
        case ABILITY_DAZZLING:
        case ABILITY_QUEENLY_MAJESTY:
        case ABILITY_ARMOR_TAIL:
            if (move.priority > 0)
                return 0;
            break;
        case ABILITY_SOUNDPROOF:
            if (IsMoveSoundBased(moveno))
                return 0;
            break;
        default:
            break;
        }
    }
    //MIMIKYU damage = 0?


    if (!attacker->hasMoldBreaker && defender->ability == ABILITY_ICE_FACE && defender->form == 0 && !(defender->condition2 & STATUS2_TRANSFORMED) && movesplit == SPLIT_PHYSICAL) //SPECIES_EISCUE
        return 0;

    if (attacker->item == ITEM_SCOPE_LENS && attacker->ability == ABILITY_SUPER_LUCK && defender->ability != ABILITY_SHELL_ARMOR && defender->ability != ABILITY_BATTLE_ARMOR)
    {
        if (move.effect == MOVE_EFFECT_HIGH_CRITICAL)
            critical = 2;
    }
    damage = BattleAI_CalcBaseDamage(bw, sp, moveno, side_cond, field_cond, pow, movetype, critical, attackerSlot, defenderSlot, attacker, defender);
	debug_printf("calc base damage complete\n");
    //=====Step 6. General Damage Modifiers=====

    // 6.1 Spread Move Modifier
    // TODO: the vanilla implementation is probably wrong
    BOOL isDoubleBattle = (BattleTypeGet(bw) & (BATTLE_TYPE_MULTI | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TAG));
    if (isDoubleBattle)
    {
        //TODO check if 2 battlers can be hit
        if ((move.target == RANGE_ADJACENT_OPPONENTS || move.target == RANGE_ALL_ADJACENT))
        {
            damage = QMul_RoundDown(damage, UQ412__0_75);
        }
    }
    debug_printf("after is double battle\n");
    // TODO
    // handle parental bond

    // 6.3 Weather Modifier
    if ((CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, 0, ABILITY_CLOUD_NINE) == 0) &&
        (CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, 0, ABILITY_AIR_LOCK) == 0))
    {
        if (sp->field_condition & WEATHER_RAIN_ANY)
        {
            switch (type)
            {
            case TYPE_FIRE:
                damage = QMul_RoundDown(damage, UQ412__0_5);
                break;
            case TYPE_WATER:
                damage = QMul_RoundDown(damage, UQ412__1_5);
                break;
            }
        }

        if (sp->field_condition & WEATHER_SUNNY_ANY)
        {
            switch (type)
            {
            case TYPE_FIRE:
                damage = QMul_RoundDown(damage, UQ412__1_5);
                break;
            case TYPE_WATER:
                // If the current weather is Sunny Day and the user is not holding Utility Umbrella, this move's damage is multiplied by 1.5 instead of halved for being Water type.
                if (moveno == MOVE_HYDRO_STEAM && attacker->item != ITEM_UTILITY_UMBRELLA)
                {
                    damage = QMul_RoundDown(damage, UQ412__1_5);
                }
                else
                {
                    damage = QMul_RoundDown(damage, UQ412__0_5);
                }
                break;
            }
        }
    }
#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] 6.3 Weather Modifier\n");
    debug_printf("[CalcBaseDamage] damage: %d\n", damage);
#endif
    // 6.3.5 Glaive Rush

    // 6.4 Critical hit modifier
    if (critical > 1) {
        damage = damage * 150 / 100;
    }

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] 6.4 Critical hit modifier\n");
    debug_printf("[CalcBaseDamage] damage: %d\n", damage);
#endif

    u32 roll = 8; //(BattleRand(bw) % 16);
    damages->damageRoll = damage * (100 - roll);  // 85-100% damage roll
    damages->damageRoll = damages->damageRoll / 100;

    for (int u = 0; u < 16; u++)
    {
        damages->damageRange[u] = damage * (85 + u) / 100;
    }

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] 6.5 Random Factor Modifier\n");
    debug_printf("[CalcBaseDamage] damage: %d\n", damages->damageRoll);
#endif

    if (attacker->type1 == movetype || attacker->type2 == type || attacker->ability == ABILITY_PROTEAN || attacker->ability == ABILITY_LIBERO)
    {
        if (attacker->ability == ABILITY_ADAPTABILITY)
        {
            damages->damageRoll = QMul_RoundDown(damages->damageRoll, UQ412__2_0);
            for (int u = 0; u < 16; u++)
            {
                damages->damageRange[u] = QMul_RoundDown(damages->damageRange[u], UQ412__2_0);
            }
        }
        else
        {
            damages->damageRoll = QMul_RoundDown(damages->damageRoll, UQ412__1_5);
            for (int u = 0; u < 16; u++)
            {
                damages->damageRange[u] = QMul_RoundDown(damages->damageRange[u], UQ412__1_5);
            }
        }
    }
   
#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] 6.6 Same-Type Attack Bonus (STAB) Modifier\n");
    debug_printf("[CalcBaseDamage] damage: %d\n", damages->damageRoll);
#endif

    // 6.7 Type Effectiveness Modifier
    // TODO: need to factor in Tera Shell
    u32 flag = 0;
    moveEffectiveness = BattleAI_GetTypeEffectiveness(bw, sp, movetype, &flag, attacker, defender);
	damages->moveEffectiveness = moveEffectiveness;

    switch (moveEffectiveness)
    {
    case TYPE_MUL_NO_EFFECT:
        damages->damageRoll = 0;
        for (int u = 0; u < 16; u++)
        {
            damages->damageRange[u] = 0;
        }
        break;
    case TYPE_MUL_TRIPLE_NOT_EFFECTIVE:
        damages->damageRoll = damages->damageRoll >> 3;
        for (int u = 0; u < 16; u++)
        {
            damages->damageRange[u] = damages->damageRange[u] >> 3;
        }
        break;
    case TYPE_MUL_DOUBLE_NOT_EFFECTIVE:
        damages->damageRoll = damages->damageRoll >> 2;
        for (int u = 0; u < 16; u++)
        {
            damages->damageRange[u] = damages->damageRange[u] >> 2;
        }
        break;
    case TYPE_MUL_NOT_EFFECTIVE:
        damages->damageRoll = damages->damageRoll >> 1;
        for (int u = 0; u < 16; u++)
        {
            damages->damageRange[u] = damages->damageRange[u] >> 1;
        }
        break;
    case TYPE_MUL_NORMAL:
        break;
    case TYPE_MUL_SUPER_EFFECTIVE:
        damages->damageRoll = damages->damageRoll << 1;
        for (int u = 0; u < 16; u++)
        {
            damages->damageRange[u] = damages->damageRange[u] << 1;
        }
        break;
    case TYPE_MUL_DOUBLE_SUPER_EFFECTIVE:
        damages->damageRoll = damages->damageRoll << 2;
        for (int u = 0; u < 16; u++)
        {
            damages->damageRange[u] = damages->damageRange[u] << 2;
        }
        break;
    case TYPE_MUL_TRIPLE_SUPER_EFFECTIVE:
        damages->damageRoll = damages->damageRoll << 3;
        for (int u = 0; u < 16; u++)
        {
            damages->damageRange[u] = damages->damageRange[u] << 2;
        }
        break;
    default:
        GF_ASSERT_INTERNAL();
        break;
    }

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] 6.7 Type Effectiveness Modifier\n");
    debug_printf("[CalcBaseDamage] moveEffectiveness: %d\n", moveEffectiveness);
    debug_printf("[CalcBaseDamage] damage: %d\n", damages->damageRoll);
#endif
    // 6.8 Burn Modifier

    if (movesplit == SPLIT_PHYSICAL)
    {
        // burns halve physical damage.  this is ignored by guts and facade (as of gen 6)
        if ((attacker->condition & STATUS_BURN) && (attacker->ability != ABILITY_GUTS) && (moveno != MOVE_FACADE))
        {
            damages->damageRoll = QMul_RoundDown(damages->damageRoll, UQ412__0_5);
            for (int u = 0; u < 16; u++)
            {
                damages->damageRange[u] = QMul_RoundDown(damages->damageRange[u], UQ412__0_5);
            }
        }
    }

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] 6.8 Burn Modifier\n");
    debug_printf("[CalcBaseDamage] damage: %d\n", damages->damageRoll);
#endif

    // 6.9 Final Modifiers

    // Move effects

    // 6.9.14 Doubled-damage moves

    // 6.9.14.1 Minimize
    if (defender->effect_of_moves & MOVE_EFFECT_FLAG_MINIMIZED && IsMoveInMinimizeVulnerabilityMovesList(moveno)) // && !dynamxed
        finalModifier = QMul_RoundUp(finalModifier, UQ412__2_0);

    // 6.9.14.2 Dig
    if ((defender->effect_of_moves & MOVE_EFFECT_FLAG_DIGGING) && moveno == MOVE_EARTHQUAKE)
        finalModifier = QMul_RoundUp(finalModifier, UQ412__2_0);

    // 6.9.14.3 Dive
    if ((defender->effect_of_moves & MOVE_EFFECT_FLAG_IS_DIVING) && (moveno == MOVE_SURF || moveno == MOVE_WHIRLPOOL))
        finalModifier = QMul_RoundUp(finalModifier, UQ412__2_0);

    // 6.9.14.4 Behemoth Blade/Behemoth Bash/Dynamax Cannon

     // 6.9.14.5 Collision Course
    if (moveEffectiveness == TYPE_MUL_SUPER_EFFECTIVE || moveEffectiveness == TYPE_MUL_DOUBLE_SUPER_EFFECTIVE || moveEffectiveness == TYPE_MUL_TRIPLE_SUPER_EFFECTIVE)
    {
        if (moveno == MOVE_COLLISION_COURSE || moveno == MOVE_ELECTRO_DRIFT)
            finalModifier = QMul_RoundUp(finalModifier, UQ412__1_3333);
    }

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] 6.9.14 Doubled-damage moves\n");
    debug_printf("[CalcBaseDamage] finalModifier: %d\n", finalModifier);
#endif


    // Effects relative to a particular side of the field
     // 6.9.1 Screens
     // TODO: handle Aurora Veil
     // handle Reflect
    if ((movesplit == SPLIT_PHYSICAL)
        && ((side_cond & SIDE_STATUS_REFLECT) != 0)
        && (sp->critical == 1)
        && (move.effect != MOVE_EFFECT_REMOVE_SCREENS)
        && (attacker->ability != ABILITY_INFILTRATOR))
    {
        if (isDoubleBattle)
            finalModifier = QMul_RoundUp(finalModifier, UQ412__0_6666);
        else
            finalModifier = QMul_RoundUp(finalModifier, UQ412__0_5);
    }
    // handle Light Screen
    if ((movesplit == SPLIT_SPECIAL)
        && ((side_cond & SIDE_STATUS_LIGHT_SCREEN) != 0)
        && (sp->critical == 1)
        && (move.effect != MOVE_EFFECT_REMOVE_SCREENS)
        && (attacker->ability != ABILITY_INFILTRATOR))
    {
        if (isDoubleBattle)
            finalModifier = QMul_RoundUp(finalModifier, UQ412__0_6666);
        else
            finalModifier = QMul_RoundUp(finalModifier, UQ412__0_5);
    }

#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] 6.9.1 Screens\n");
    debug_printf("[CalcBaseDamage] finalModifier: %d\n", finalModifier);
#endif

    if (moveEffectiveness == TYPE_MUL_TRIPLE_NOT_EFFECTIVE || moveEffectiveness == TYPE_MUL_DOUBLE_NOT_EFFECTIVE || moveEffectiveness == TYPE_MUL_NOT_EFFECTIVE)
    {
        if (attacker->ability == ABILITY_TINTED_LENS)
            finalModifier = QMul_RoundUp(finalModifier, UQ412__1_25);

    }

    if (moveEffectiveness == TYPE_MUL_SUPER_EFFECTIVE || moveEffectiveness == TYPE_MUL_DOUBLE_SUPER_EFFECTIVE || moveEffectiveness == TYPE_MUL_TRIPLE_SUPER_EFFECTIVE)
    {
        if (attacker->ability == ABILITY_NEUROFORCE)
            finalModifier = QMul_RoundUp(finalModifier, UQ412__1_25);
        if (defender->ability == ABILITY_SOLID_ROCK || defender->ability == ABILITY_FILTER || defender->ability == ABILITY_PRISM_ARMOR)
            finalModifier = QMul_RoundUp(finalModifier, UQ412__0_75);
    }

    // 6.9.3 Sniper
    if ((attacker->ability == ABILITY_SNIPER) && (critical > 1))
        finalModifier = QMul_RoundUp(finalModifier, UQ412__1_5);

    if (defender->ability == ABILITY_FLUFFY)
    {
        // 6.9.6 Fluffy (contact moves)
        if (BattleAI_IsContactBeingMade(sp, attacker->ability, attacker->item_held_effect, moveno))
            finalModifier = QMul_RoundUp(finalModifier, UQ412__0_5);

        // 6.9.10 Fluffy (Fire-type moves)
        if (type == TYPE_FIRE)
            finalModifier = QMul_RoundUp(finalModifier, UQ412__2_0);
    }

    // 6.9.5 Multiscale / Shadow Shield
    if ((defender->ability == ABILITY_MULTISCALE || defender->ability == ABILITY_SHADOW_SHIELD) && (defender->hp == defender->maxhp))
        finalModifier = QMul_RoundUp(finalModifier, UQ412__0_5);

    // 6.9.7 Friend Guard
    if (isDoubleBattle && GetBattlerAbility(sp, BATTLER_ALLY(defenderSlot)) == ABILITY_FRIEND_GUARD)
        finalModifier = QMul_RoundUp(finalModifier, UQ412__0_75);

    // 6.9.15 Punk Rock
    if (defender->ability == ABILITY_PUNK_ROCK && IsMoveSoundBased(moveno))
        finalModifier = QMul_RoundUp(finalModifier, UQ412__0_5);

    // 6.9.16 Ice Scales - halve damage if move is special, regardless of if it uses defense stat
    if (!attacker->hasMoldBreaker && defender->ability == ABILITY_ICE_SCALES && movesplit == SPLIT_SPECIAL)
        finalModifier = QMul_RoundUp(finalModifier, UQ412__0_5);


    //items
    // 6.9.9 Metronome (item)
    if (attacker->item_held_effect == HOLD_EFFECT_BOOST_REPEATED)
    {
        switch (attacker->metronomeTurns)
        {
        case 0:
            break;
        case 1:
            finalModifier = QMul_RoundUp(finalModifier, UQ412__1_2);
            break;
        case 2:
            finalModifier = QMul_RoundUp(finalModifier, UQ412__1_4);
            break;
        case 3:
            finalModifier = QMul_RoundUp(finalModifier, UQ412__1_6);
            break;
        case 4:
            finalModifier = QMul_RoundUp(finalModifier, UQ412__1_8);
            break;
        case 5:
        default:
            finalModifier = QMul_RoundUp(finalModifier, UQ412__2_0);
            break;
        }
    }

    switch (moveEffectiveness)
    {
    case TYPE_MUL_NO_EFFECT:
    case TYPE_MUL_TRIPLE_NOT_EFFECTIVE:
    case TYPE_MUL_DOUBLE_NOT_EFFECTIVE:
    case TYPE_MUL_NOT_EFFECTIVE:
        break;
    case TYPE_MUL_NORMAL:
        if (movetype == TYPE_NORMAL && defender->item == ITEM_CHILAN_BERRY)
            finalModifier = QMul_RoundUp(finalModifier, UQ412__0_5);
        break;
    case TYPE_MUL_SUPER_EFFECTIVE:
    case TYPE_MUL_DOUBLE_SUPER_EFFECTIVE:
    case TYPE_MUL_TRIPLE_SUPER_EFFECTIVE:
        // 6.9.11 Expert Belt
        if (attacker->item_held_effect == HOLD_EFFECT_POWER_UP_SE)
            finalModifier = QMul_RoundUp(finalModifier, UQ412__1_2);

        // 6.9.13 Resist Berries
        if ((u32)typeToBerryMapping[movetype] == defender->item)
            finalModifier = QMul_RoundUp(finalModifier, UQ412__0_5);
        break;
    default:
        break;
    }

    // 6.9.12 Life Orb
    if (attacker->item_held_effect == HOLD_EFFECT_HP_DRAIN_ON_ATK)
        finalModifier = QMul_RoundUp(finalModifier, UQ412__1_3_BUT_LOWER);

    damages->damageRoll = QMul_RoundDown(damages->damageRoll, finalModifier);
    for (int u = 0; u < 16; u++)
    {
        damages->damageRange[u] = QMul_RoundDown(damages->damageRange[u], finalModifier);
    }
#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Step 9. Item Modifier\n");
    debug_printf("[CalcBaseDamage] damage: %d\n", damages->damageRoll);
#endif

    // Step 10. Z-move into Protecting Move Modifier
    // Dynamax move Protect 0.25x place is same as Z-move Protect.
    // TODO: handle other protecting moves such as Quick Guard and Wide Guard
    /*
    if ((MoveIsZMove(moveno) || MoveIsMaxMove(moveno)))/{ //&& sclient can protect) {
        damage = QMul_RoundDown(damage, UQ412__0_25);
        for (int u = 0; u < 16; u++)
        {
            damages->damageRange[u] = QMul_RoundDown(damages->damageRange[u], UQ412__0_25);
        }
    }*/


    // Step 11. One Damage Check
    // Step 12. 65,535 Damage Check
    damages->damageRoll = damages->damageRoll == 0 ? 1 : damages->damageRoll;
    damages->damageRoll = damages->damageRoll % 65536;
    for (int u = 0; u < 16; u++)
    {
        damages->damageRange[u] = damages->damageRange[u] == 0 ? 1 : damages->damageRange[u];
        damages->damageRange[u] = damages->damageRange[u] % 65536;
    }
    

//#ifdef DEBUG_DAMAGE_CALC_AI
    debug_printf("\n=================\n");
    debug_printf("[CalcBaseDamage] Final damage: %d\n", damage);
    debug_printf("Unrolled damage: %d -- Battler %d hit battler %d for %d (%dth roll) damage.\n", damages->damageRange[0], attackerSlot, defenderSlot, damages->damageRoll, 15-roll);
    debug_printf("Rolls:[");
	BOOL first = TRUE;
    for (int u = 0; u < 16; u++)
    {
        if (first)
            first = FALSE;
        else
            debug_printf(", ");
        debug_printf("%d", damages->damageRange[u]);
    }
    debug_printf("]\n");
    debug_printf("\n=================\n");
//#endif //DEBUG_DAMAGE_CALC_AI

    return damages->damageRoll;
}

int LONG_CALL BattleAI_GetTypeEffectiveness(void* bw, struct BattleStruct* sp, int move_type, u32* flag, struct AI_sDamageCalc* attacker, struct AI_sDamageCalc* defender)
{
    int i = 0;
    u8 defender_type_1 = defender->type1 & 0x1F;
    u8 defender_type_2 = defender->type2 & 0x1F;

    u32 type1Effectiveness = TYPE_MUL_NORMAL;
    u32 type2Effectiveness = TYPE_MUL_NORMAL;

    // TODO: handle Ring Target, Thousand Arrows, Freeze-Dry, Flying Press

    while (TypeEffectivenessTable[i][0] != TYPE_ENDTABLE)
    {
        if (TypeEffectivenessTable[i][0] == TYPE_FORESIGHT)  // handle foresight
        {
            if ((defender->condition2 & STATUS2_FORESIGHT) || attacker->ability == ABILITY_SCRAPPY || attacker->ability == ABILITY_MINDS_EYE)
            {
                break;
            }
            else
            {
                i++;
                continue;
            }
        }
        if (TypeEffectivenessTable[i][0] == move_type)
        {
            if (TypeEffectivenessTable[i][1] == defender_type_1)
            {
                if (AI_ShouldUseNormalTypeEffCalc(sp, defender->item_held_effect, i) == TRUE 
                    && !(!CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, 0, ABILITY_CLOUD_NINE) 
                        && !CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, 0, ABILITY_AIR_LOCK) 
                        && sp->field_condition & WEATHER_STRONG_WINDS 
                        && (TypeEffectivenessTable[i][2] == 20) 
                        && defender_type_1 == TYPE_FLYING))
                {
                    type1Effectiveness = TypeEffectivenessTable[i][2];
                    //AI_TypeCheckCalc(TypeEffectivenessTable[i][2], flag);
                    //TypeCheckCalc(sp, attack_client, type1Effectiveness, 42, 42, flag);
                }
            }
            if ((TypeEffectivenessTable[i][1] == defender_type_2) && (defender_type_1 != defender_type_2))
            {
                if (AI_ShouldUseNormalTypeEffCalc(sp, defender->item_held_effect, i) == TRUE 
                    && !(!CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, 0, ABILITY_CLOUD_NINE) 
                        && !CheckSideAbility(bw, sp, CHECK_ABILITY_ALL_HP, 0, ABILITY_AIR_LOCK) 
                        && sp->field_condition & WEATHER_STRONG_WINDS 
                        && (TypeEffectivenessTable[i][2] == 20) 
                        && defender_type_2 == TYPE_FLYING))
                {

                    type2Effectiveness = TypeEffectivenessTable[i][2];
                    //AI_TypeCheckCalc(TypeEffectivenessTable[i][2], flag);
                    //TypeCheckCalc(sp, attack_client, type1Effectiveness, 42, 42, flag);
                }
            }
            // TODO: Handle type3, Tera Type
        }
        i++;
    }

    // TODO: Refactor!!!
    if (type1Effectiveness == TYPE_MUL_NO_EFFECT || type2Effectiveness == TYPE_MUL_NO_EFFECT) {
        return TYPE_MUL_NO_EFFECT;
    }
    if (type1Effectiveness == TYPE_MUL_NOT_EFFECTIVE && type2Effectiveness == TYPE_MUL_NOT_EFFECTIVE) {
        return TYPE_MUL_DOUBLE_NOT_EFFECTIVE;
    }
    if ((type1Effectiveness == TYPE_MUL_SUPER_EFFECTIVE && type2Effectiveness == TYPE_MUL_NOT_EFFECTIVE)
        || (type2Effectiveness == TYPE_MUL_SUPER_EFFECTIVE && type1Effectiveness == TYPE_MUL_NOT_EFFECTIVE)) {
        return TYPE_MUL_NORMAL;
    }
    if (type1Effectiveness == TYPE_MUL_NORMAL) {
        return type2Effectiveness;
    }
    if (type2Effectiveness == TYPE_MUL_NORMAL) {
        return type1Effectiveness;
    }
    if (type1Effectiveness == type2Effectiveness && type1Effectiveness == TYPE_MUL_SUPER_EFFECTIVE) {
        return TYPE_MUL_DOUBLE_SUPER_EFFECTIVE;
    }
    return TYPE_MUL_NO_EFFECT;
}


BOOL LONG_CALL BattleAI_IsContactBeingMade(struct BattleStruct* sp, u32 ability, u32 itemHoldEffect, u32 moveno)
{
    if (ability == ABILITY_LONG_REACH)
        return FALSE;

    if (itemHoldEffect == HOLD_EFFECT_PREVENT_CONTACT_EFFECTS || (itemHoldEffect == HOLD_EFFECT_INCREASE_PUNCHING_MOVE_DMG && IsMovePunchingMove(moveno)))
        return FALSE;

    if (itemHoldEffect == HOLD_EFFECT_PREVENT_CONTACT_EFFECTS)
        return FALSE;

    if (sp->moveTbl[moveno].flag & FLAG_CONTACT)
        return TRUE;

    return FALSE;
}

BOOL IsMoveBoostedBySheerForce(u32 moveno, u32 moveeffect)
{
    BOOL isBoosted = FALSE;
    switch (moveeffect)
    {
    case MOVE_EFFECT_FLINCH_HIT:
    case MOVE_EFFECT_RAISE_ALL_STATS_HIT:
    case MOVE_EFFECT_BLIZZARD:
    case MOVE_EFFECT_PARALYZE_HIT:
    case MOVE_EFFECT_LOWER_SPEED_HIT:
    case MOVE_EFFECT_RAISE_SP_ATK_HIT:
    case MOVE_EFFECT_CONFUSE_HIT:
    case MOVE_EFFECT_LOWER_DEFENSE_HIT:
    case MOVE_EFFECT_LOWER_SP_DEF_HIT:
    case MOVE_EFFECT_BURN_HIT:
    case MOVE_EFFECT_FLINCH_BURN_HIT:
    case MOVE_EFFECT_RAISE_SPEED_HIT:
    case MOVE_EFFECT_POISON_HIT:
    case MOVE_EFFECT_FREEZE_HIT:
    case MOVE_EFFECT_FLINCH_FREEZE_HIT:
    case MOVE_EFFECT_RAISE_ATTACK_HIT:
    case MOVE_EFFECT_LOWER_ACCURACY_HIT:
    case MOVE_EFFECT_BADLY_POISON_HIT:
    case MOVE_EFFECT_SECRET_POWER:
    case MOVE_EFFECT_LOWER_SP_ATK_HIT:
    case MOVE_EFFECT_THUNDER:
    case MOVE_EFFECT_HURRICANE:
    case MOVE_EFFECT_FLINCH_PARALYZE_HIT:
    case MOVE_EFFECT_FLINCH_DOUBLE_DAMAGE_FLY_OR_BOUNCE:
    case MOVE_EFFECT_LOWER_SP_DEF_2_HIT:
    case MOVE_EFFECT_LOWER_ATTACK_HIT:
    case MOVE_EFFECT_THAW_AND_BURN_HIT:
    case MOVE_EFFECT_CHATTER:
    case MOVE_EFFECT_FLINCH_MINIMIZE_DOUBLE_HIT:
    case MOVE_EFFECT_RANDOM_PRIMARY_STATUS_HIT:
    case MOVE_EFFECT_PREVENT_HEALING_HIT: // Psychic Noise
    case MOVE_EFFECT_POISON_MULTI_HIT: // twineedle
    case MOVE_EFFECT_HIGH_CRITICAL_BURN_HIT: // blaze kick
    case MOVE_EFFECT_HIGH_CRITICAL_POISON_HIT: // cross poison
    case MOVE_EFFECT_RECOIL_BURN_HIT: // flare blitz
    case MOVE_EFFECT_RECOIL_PARALYZE_HIT:
        isBoosted = TRUE;
        break;
    default:
        break;
    }

    switch (moveno)
    {
    case MOVE_SPARKLING_ARIA:
    case MOVE_SPIRIT_SHACKLE:
    case MOVE_ANCHOR_SHOT:
    case MOVE_CEASELESS_EDGE:
    case MOVE_STONE_AXE:
    case MOVE_ELECTRO_SHOT:
        isBoosted = TRUE;
        break;
    default:
        break;
    }
    return isBoosted;
}

int LONG_CALL BattleAI_AdjustUnusualMoveDamage(u32 attackerLevel, u32 attackerHP, u32 defenderHP, u32 damage, u32 moveEffect, u32 attackerAbility, u32 attackerItem)
{
    //struct BattleStruct* ctx = bsys->sp;
    switch (moveEffect)
    {
    case MOVE_EFFECT_UP_TO_10_HITS:
        if (attackerAbility == ABILITY_SKILL_LINK)
            return damage * 10;
        else if (attackerItem == ITEM_LOADED_DICE)
            return damage *= 5; //4-10
        return damage *= 3;
    case MOVE_EFFECT_HIT_THREE_TIMES_ALWAYS_CRITICAL: //surge Strikes
    case MOVE_EFFECT_HIT_THREE_TIMES: //triple dive
    case MOVE_EFFECT_HIT_THREE_TIMES_INCREMENT_BASE_POWER_10: // triple kick
        return damage *= 3;
    case MOVE_EFFECT_HIT_THREE_TIMES_INCREMENT_BASE_POWER_20: // triple axel
        return damage *= 6;
    case MOVE_EFFECT_MULTI_HIT: //2-5 hit moves
        if (attackerAbility == ABILITY_SKILL_LINK)
            return damage * 5;
        else if (attackerItem == ITEM_LOADED_DICE)
            return damage *= 4; //4-5
        return damage *= 3;
    case MOVE_EFFECT_LEVEL_DAMAGE_FLAT: //night shade, seismic toss
    case MOVE_EFFECT_RANDOM_DAMAGE_1_TO_150_LEVEL: //psywave
        return attackerLevel;
    case MOVE_EFFECT_10_DAMAGE_FLAT: //sonic boom
        return 20;
    case MOVE_EFFECT_40_DAMAGE_FLAT: //dragon rage
        return 40;
    case MOVE_EFFECT_HIT_TWICE_AND_FLINCH: //double Iron bash
    case MOVE_EFFECT_POISON_MULTI_HIT: //twinneedle
    case MOVE_EFFECT_HIT_TWICE: //double hit, dual wingbeat, etc...
        return damage *= 2;
    case MOVE_EFFECT_HALVE_HP: //super fang, nature's madness
        return defenderHP / 2;
    case MOVE_EFFECT_SET_HP_EQUAL_TO_USER: //endeavor
    {
        if (attackerHP < defenderHP)
            return defenderHP - attackerHP;
        else
            return 0;
    }
    }
    return damage;
}


BOOL LONG_CALL canAttackerOneShotDefender(u32 attackerDamage, u8 split, u32 moveno, struct AI_sDamageCalc* attacker, struct AI_sDamageCalc* defender)
{
    BOOL isMoveMultihit = IsMultiHitMove(moveno);
    BOOL canOneShot = TRUE;

    if (attackerDamage >= defender->hp)
    {
        if (defender->hp == defender->maxhp)
        {

            if (!isMoveMultihit
                && (defender->item == ITEM_FOCUS_SASH
                    || (!attacker->hasMoldBreaker && defender->ability == ABILITY_STURDY)))
            {
                canOneShot = FALSE;
            }

            if (!attacker->hasMoldBreaker && defender->ability == ABILITY_DISGUISE) //SPECIES_MIMIKYU
                canOneShot = FALSE;
        }
        
    }
    else
        canOneShot = FALSE;

    if (!attacker->hasMoldBreaker && defender->ability == ABILITY_ICE_FACE && defender->form == 0 && split == SPLIT_PHYSICAL) //SPECIES_EISCUE
        canOneShot = FALSE;

    return canOneShot;
}

BOOL BattleAI_AttackerHasOnlyIneffectiveMoves(struct BattleStruct* ctx, u32 attacker, int knownMoves, u32 effectiveness[4])
{
    BOOL onlyIneffectiveMoves = TRUE;
    for (int k = 0; k < knownMoves; ++k)
    {
        u32 attackerMoveno = ctx->battlemon[attacker].move[k];
        struct BattleMove attackerMove = ctx->moveTbl[attackerMoveno];
        if (attackerMove.split != SPLIT_STATUS && attackerMove.power > 1)
        {
            switch (effectiveness[k])
            {
            case TYPE_MUL_NORMAL:
            case TYPE_MUL_SUPER_EFFECTIVE:
            case TYPE_MUL_DOUBLE_SUPER_EFFECTIVE:
            case TYPE_MUL_TRIPLE_SUPER_EFFECTIVE:
                onlyIneffectiveMoves = FALSE;
                break;
            default:
                break;
            }
        }
    }
    return onlyIneffectiveMoves;
}


int LONG_CALL BattleAI_PostKOSwitchIn_Internal(struct BattleSystem* bsys, int attacker, int* score)
{
    debug_printf("BattleAI_PostKOSwitchIn_Internal %d\n", attacker);

    struct BattleStruct* ctx = bsys->sp;
    int battleType = BattleTypeGet(bsys);
    struct PartyPokemon* mon;

    struct AI_sDamageCalc attackerMon = { 0 };
    struct AI_sDamageCalc defenderMon = { 0 };

    u8 critical = 0;

    u8 speedCalc;
    u32 defender = BATTLER_OPPONENT(attacker);   //default for singles -- updated in the doubles section
    u8 slot1, slot2;
    u16 moveno = 0;
    u32 monDealsRolledDamage[6] = { 0 };
    u32 monHighestDamageMoveno = 0;
    u32 monReceivingHighestDamageMoveno = 0;
    u32 monReceivesDamage[6] = { 0 };
    u16 switchInScore[6] = { 0 };
    int partySize = 0;
    int picked = 6; // in Order

    slot1 = attacker;
    slot2 = slot1;

    if (battleType & (BATTLE_TYPE_TAG | BATTLE_TYPE_MULTI | BATTLE_TYPE_DOUBLE))
        slot2 = BATTLER_ALLY(attacker);


    FillDamageStructFromBattleMon(bsys, ctx, &defenderMon, defender);

    partySize = Battle_GetClientPartySize(bsys, attacker);
    for (int i = 0; i < partySize; i++)
    {
        mon = Battle_GetClientPartyMon(bsys, attacker, i);
        attackerMon.species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG, 0);
        debug_printf("Slot %d:%d hp:%d,\n", i, attackerMon.species, GetMonData(mon, MON_DATA_HP, 0));
        debug_printf("sel_m1 %d, sel_m2 %d, switchSl1 %d, switchSl1 %d\n", ctx->sel_mons_no[slot1], ctx->sel_mons_no[slot2], ctx->aiSwitchedPartySlot[slot1], ctx->aiSwitchedPartySlot[slot2]);

        if (attackerMon.species != SPECIES_NONE && attackerMon.species != SPECIES_EGG && GetMonData(mon, MON_DATA_HP, 0)
            && i != ctx->sel_mons_no[slot1]
            && i != ctx->sel_mons_no[slot2]
            && i != ctx->aiSwitchedPartySlot[slot1]
            && i != ctx->aiSwitchedPartySlot[slot2])
        {
            switchInScore[i] = 100;

            FillDamageStructFromPartyMon(bsys, ctx, &attackerMon, mon);

            speedCalc = BattleAI_CalcSpeed(bsys, ctx, defender, mon, CALCSPEED_FLAG_NO_PRIORITY); //checks actual turn order with field state considered

            for (u8 j = 0; j < CLIENT_MAX; ++j)
            {
                struct AI_damage damages = { 0 };
                moveno = GetMonData(mon, MON_DATA_MOVE1 + j, NULL);
                if (moveno != MOVE_NONE)
                {
                    struct BattleMove attackerMove = ctx->moveTbl[moveno];

                    if (attackerMove.split != SPLIT_STATUS && attackerMove.power)
                    {
                        damages.damageRoll = BattleAI_CalcDamage(bsys, ctx, moveno, ctx->side_condition[BATTLER_IS_ENEMY(attacker)], ctx->field_condition, attackerMove.power, attackerMove.type, critical, attacker, defender, &damages, &attackerMon, &defenderMon);

                        damages.damageRoll = BattleAI_AdjustUnusualMoveDamage(attackerMon.level, attackerMon.hp, defenderMon.hp, damages.damageRoll, attackerMove.effect, attackerMon.ability, attackerMon.item);
                        for (int u = 0; u < 16; u++)
                        {
                            damages.damageRange[u] = BattleAI_AdjustUnusualMoveDamage(attackerMon.level, attackerMon.hp, defenderMon.hp, damages.damageRange[u], attackerMove.effect, attackerMon.ability, attackerMon.item);
                        }

                        if (damages.damageRoll > monDealsRolledDamage[i])
                        {
							monHighestDamageMoveno = moveno;
                            monDealsRolledDamage[i] = damages.damageRoll;
                        }    
                    }
                    debug_printf("Dealing with move %d: %d deals [%d-%d], roll %d > def.HP %d\n", j, moveno, damages.damageRange[0], damages.damageRange[15], damages.damageRoll, defenderMon.hp);
                }
            }

            for (int k = 0; k < GetBattlerLearnedMoveCount(bsys, ctx, defender); ++k)
            {
                struct AI_damage damages = { 0 };
                u32 defenderMoveno = ctx->battlemon[defender].move[k];
                struct BattleMove defenderMove = ctx->moveTbl[defenderMoveno];

                if (defenderMove.split != SPLIT_STATUS && defenderMove.power)
                {
                    damages.damageRoll = BattleAI_CalcDamage(bsys, ctx, defenderMoveno, ctx->side_condition[BATTLER_IS_ENEMY(defender)], ctx->field_condition, defenderMove.power, defenderMove.type, critical, defender, attacker, &damages, &defenderMon, &attackerMon);

                    damages.damageRoll = BattleAI_AdjustUnusualMoveDamage(defenderMon.level, defenderMon.hp, attackerMon.hp, damages.damageRoll, defenderMove.effect, defenderMon.ability, defenderMon.item);
                    for (int u = 0; u < 16; u++)
                    {
                        damages.damageRange[u] = BattleAI_AdjustUnusualMoveDamage(defenderMon.level, defenderMon.hp, attackerMon.hp, damages.damageRange[u], defenderMove.effect, defenderMon.ability, defenderMon.item);
                    }

                    if (damages.damageRoll > monReceivesDamage[i])
                    {
                        monReceivingHighestDamageMoveno = defenderMoveno;
                        monReceivesDamage[i] = damages.damageRoll;
                    }
                }
                debug_printf("Receiving from move %d: %d is [%d-%d], roll %d > att.HP %d\n", k, defenderMoveno, damages.damageRange[0], damages.damageRange[15], damages.damageRoll, attackerMon.hp);
            }

			//TODO stealth rocks, spikes, toxic spikes, etc...
            u8 aiMonCanOneshotPlayer = canAttackerOneShotDefender(monDealsRolledDamage[i], ctx->moveTbl[monHighestDamageMoveno].split, monHighestDamageMoveno, &attackerMon, &defenderMon);
            u8 playerCanOneShotAiMon = canAttackerOneShotDefender(monReceivesDamage[i], ctx->moveTbl[monReceivingHighestDamageMoveno].split, monReceivingHighestDamageMoveno, &defenderMon, &attackerMon);
            u16 partyMonPercentDamageDealt = (100 * monDealsRolledDamage[i] / defenderMon.hp);
			u16 partyMonPercentDamageReceived = (100 * monReceivesDamage[i] / attackerMon.hp);


            debug_printf("SwitchScore: SpeedCalc %d. Attacker %d deals %d%% to defender %d. Receives %d%%", speedCalc, attacker, (100 * monDealsRolledDamage[i] / defenderMon.hp), defender, (100 * monReceivesDamage[i] / attackerMon.hp));
            
            if (!playerCanOneShotAiMon && (attackerMon.species == SPECIES_WYNAUT || attackerMon.species == SPECIES_WOBBUFFET))
                switchInScore[i] += 2;

            if (speedCalc > 0)
            {
                if (aiMonCanOneshotPlayer)
                    switchInScore[i] += 5;
                else if (partyMonPercentDamageDealt >= partyMonPercentDamageReceived)
                    switchInScore[i] += 3;
                else
                    switchInScore[i] += 1;

                if (attackerMon.species == SPECIES_DITTO)
                    switchInScore[i] += 2;
            }
            else
            {
                if (aiMonCanOneshotPlayer && !playerCanOneShotAiMon)
                    switchInScore[i] += 4;
                else if (partyMonPercentDamageDealt > partyMonPercentDamageReceived)
                    switchInScore[i] += 2;
                else if (playerCanOneShotAiMon)
                    switchInScore[i] -= 1;
            }
			debug_printf(": %d\n", switchInScore[i]);
            //default += 0;
        }
    }

    u16 currentScore = switchInScore[0];
    for (int i = 0; i < partySize; i++)
    {
        if (switchInScore[i] > currentScore)
        {
            picked = i;
			*score = switchInScore[i];
            currentScore = switchInScore[i];
        }
    }
    for (int i = 0; i < partySize; i++)
    {
        debug_printf("%i ", switchInScore[i]);
    }
    debug_printf("-> picked %i\n", picked);

    return picked;
}
