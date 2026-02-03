#include "../../include/types.h"
#include "../../include/config.h"
#include "../../include/battle.h"
#include "../../include/mega.h"
#include "../../include/pokemon.h"
#include "../../include/trainer_ai.h"
#include "../../include/constants/ability.h"
#include "../../include/constants/move_effects.h"
#include "../../include/constants/species.h"
#include "../../include/constants/battle_script_constants.h"
#include "../../include/constants/battle_message_constants.h"
#include "../../include/constants/item.h"
#include "../../include/item.h"
#include "../../include/custom/custom_ai.h"
#include "../../include/overlay.h"
#include "../../include/constants/file.h"




struct PACKED AIContext{

    struct AI_sDamageCalc attackerMon;
    struct AI_sDamageCalc defenderMon; //"player"

    /*Field/Pokemon state relavent variables*/
    BOOL defenderImmuneToPoison;
    BOOL defenderImmuneToParalysis;
    BOOL defenderImmuneToBurn;
    BOOL defenderImmuneToSleep;
    BOOL attackerKnowsPsychUp;
    BOOL attackerHasSupereffectiveMove;
    BOOL defenderHasHazards;
    BOOL trickRoomActive;
    BOOL hasStatChange;
    BOOL attackerHasDamagingMove;
    BOOL defenderHasBadItem;
    u8 attackerMovesFirst;
    u8 defenderMovesFirst;
    u8 isSpeedTie;
    u32 flingPower;
    u32 attackerMoveEffectiveness[4];
    int chosenTarget;
    int attacker;
    int defender;
    int defenderSide;
    int attackerSide;
    int attackerAbility;
    int attackerLevel;
    int defenderAbility;
    int attackerItem;
    int defenderItem;
    int holdEffect;
    int defenderType1;
    int defenderType2;
    int attackerType1;
    int attackerType2;
    int attackerHP;
    int attackerMaxHP;
    int attackerPercentHP;
    int defenderHP;
    int defenderMaxHP;
    int defenderPercentHP;
    int attackerSpeed;
    int defenderSpeed;
    int partySizeAttacker;
    int livingMembersAttacker;
    int partySizeDefender;
    int livingMembersDefender;
    int damagedMonsDefender;
    int statusedMonsAttacker;
    int attackerMovesKnown;
    int attackerPartyIndex;
    int attackerLastUsedMove;
    int defenderLastUsedMove;
    int defenderLastUsedMoveEffect;
    int defenderTurnsOnField;
    int attackerTurnsOnField;
    int differenceInAttackStages;
    int differenceInSpAtkStages;
    int differenceInDefenseStages;
    int differenceInSpDefStages;
    int maxDamageReceived;


    /*Specific to multi/double/tag battles*/
    int partner;
    int partnerHP;
    int partnerPercentHP;
    int partnerAbility;
    int partnerItem;

    /*Move-relevant variables*/
    int attackerMove;
    int attackerMoveEffect;
    int attackerMoveType;
    int attackerMovePPRemaining;
    int attackerAvgRollMoveDamages[4];
    int attackerMaxDamageOutputMinRoll;
};

typedef struct {
    u32 flag;
    int (*evaluator)(struct BattleSystem *bsys, int attacker, int moveIndex, struct AIContext *ai);
} MoveEvaluator;

/*Flag functions return a move score, given the index of the current move*/
int BasicFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai);
int EvaluateAttackFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai);
int ExpertFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai);
int SetupFirstTurnFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai);
int RiskyFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai);
int PrioritizeDamageFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai);
int BatonPassFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai);
int TagStrategyFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai);
int CheckHPFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai);
int WeatherFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai);
int HarassmentFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai);

/*Add your own custom flags to this list*/
static const MoveEvaluator moveEvaluators[] = {
    { AI_FLAG_BASIC,                BasicFlag },
    { AI_FLAG_EVAL_ATTACK,          EvaluateAttackFlag },
    { AI_FLAG_EXPERT,               ExpertFlag },
    { AI_FLAG_SETUP_FIRST_TURN,     SetupFirstTurnFlag },
    { AI_FLAG_RISKY,                RiskyFlag },
    { AI_FLAG_DAMAGE_PRIORITY,      PrioritizeDamageFlag },
    { AI_FLAG_BATON_PASS,           BatonPassFlag },
    { AI_FLAG_TAG_STRATEGY,         TagStrategyFlag },
    { AI_FLAG_CHECK_HP,             CheckHPFlag },
    { AI_FLAG_WEATHER,              WeatherFlag },
    { AI_FLAG_HARRASSMENT,          HarassmentFlag },
};

/*Helper Functions*/
int AttackerMonWithHighestDamage(struct BattleSystem *bsys, int attacker, struct AIContext *ai);
int BattlerPositiveStatChangesSum(struct BattleSystem *bsys, u32 battler, struct AIContext *ai);
BOOL BattlerHasStatBoostGreater(struct BattleSystem *bsys, u32 battler, u32 boost_amount, struct AIContext *ai);
BOOL BattlerHasStatBoostLesser(struct BattleSystem *bsys, u32 battler, u32 drop_amount, struct AIContext *ai);
BOOL BattlerKnowsMove(struct BattleSystem *bsys, u32 battler, u32 move, struct AIContext *ai);
BOOL BattlerHasMoveSplit(struct BattleSystem *bsys, u32 battler, u32 move_split, struct AIContext *ai);
BOOL BattlerHasMoveEffect(struct BattleSystem *bsys, u32 battler, u32 move_effect, struct AIContext *ai);
BOOL BattlerHasTypeDamagingMove (struct BattleSystem *bsys, u32 battler, u32 type, struct AIContext *ai);
BOOL ItemIsBadToTrade(u32 item, struct AIContext *ai);
BOOL ItemIsDisruptive(u32 item, struct AIContext *ai);
BOOL IsDesirableAbility(u32 ability);
BOOL LONG_CALL DropsStatsAsStatus(u32 moveEffect);
BOOL LONG_CALL IsInMirrorMoveList(u32 moveEffect);
BOOL LONG_CALL IsInList(u32 moveEffect, const u16 StatList[], u16 ListLength);
BOOL LONG_CALL BattlerKnowsMoveInList(struct BattleSystem *bsys, u32 battler, const u16 MoveList[], u16 listLength, struct AIContext *ai);
BOOL LONG_CALL BattlerMovesFirstDoubles(struct BattleSystem *bsys, struct BattleStruct *ctx, int mainBattler, int flag, struct AIContext *ai);
BOOL LONG_CALL MoveIsStrongest(struct BattleSystem *bsys, struct BattleStruct *ctx, int moveIndex, struct AIContext *ai);
void SetupStateVariables(struct BattleSystem *bsys, int attacker, u32 defender, struct AIContext *ai);
int AdjustUnusualMoveDamage(struct BattleSystem *bsys, int attacker, u32 defender, int damage, int moveEffect, struct AIContext *ai);
int AdjustUnusualMovePower(struct BattleSystem *bsys, int attacker, u32 defender, int moveEffect, struct AIContext *ai);

unsigned int __attribute__((section (".init"))) TrainerAI_Main(struct BattleSystem *bsys, int attacker)
    {
    debug_printf("In TrainerAI_Main:\n");
    struct BattleStruct *ctx = bsys->sp;
    struct AIContext aictx = {0};
    struct AIContext *ai = &aictx;
    unsigned int result = 0;
    //enum AIActionChoice result = AI_ENEMY_ATTACK_1, highest_damage_something = 0;
    debug_printf("After enum\n");
    unsigned int score = 0;
    if (attacker >= 10)
		return BattleAI_PostKOSwitchIn_Internal(bsys, attacker - 10, &score);
    debug_printf("After attacker>=10 thing\n");
    unsigned int highest_move_score = 0;
    unsigned int moveScores[4][4];
    for (unsigned int i = 0; i < 4; i++) {           //don't want to get negative (unsigned ints) numbers, so start high at 100
        for (unsigned int j = 0; j < 4; j++) {
            moveScores[i][j] = 100;
        }
    } 
    debug_printf("After moveScore setup\n");
    unsigned int max_scores[4] = {0};                //highest score over all of the 4 moves the attacker has, measured against each mon on the field (self is always 0)
    unsigned int num_defender_ties = 0;
    unsigned int defender_tie_indices[4] = {0};
    unsigned int num_move_score_ties = 0;
    unsigned int move_tie_indices[4] = {0};
    unsigned int target = 0;
    unsigned int defender = BATTLER_OPPONENT(attacker);   //default for singles -- updated in the doubles section
    debug_printf("Before setup vars\n");
    SetupStateVariables(bsys, attacker, defender, ai);
    debug_printf("After Setup state vars\n");
    /*For more than a 1v1 battle, loop over all battlers and compute the highest score for each.
    The highest score among them determines the target.*/
    if(BattleTypeGet(bsys) & (BATTLE_TYPE_MULTI | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TAG)){
        /*ALWAYS turn on tag strategy in double battles. Prevents user errors. Otherwise targeting and scoring will be incorrect.*/
        bsys->trainers[ai->attacker].aibit |= AI_FLAG_TAG_STRATEGY;
        for(unsigned int battler_no = 0; battler_no < CLIENT_MAX; battler_no++){
            ai->defender = battler_no;
            debug_printf("\nfor Attacker: %d, Defender: %d\n", ai->attacker, ai->defender);
            SetupStateVariables(bsys, attacker, ai->defender, ai);             //need to reset the ai vars for each defender 

            if(battler_no == ai->attacker || ctx->battlemon[ai->defender].hp == 0){     //edge case for doubles when only one mon remains alive. Not including this causes incorrect scoring.
                for(unsigned int i = 0; i < 4; i ++){
                    moveScores[battler_no][i] = 0;                                      //prevent ai from thinking it is also the defender for calculations
                }
            }
            else{
                /*Main loop over moves and select the best one*/
                for (unsigned int i = 0; i < 4; i++)
                {  
                    /*Move-relevant variables*/
                    ai->attackerMove = ctx->battlemon[ai->attacker].move[i];
                    ai->attackerMoveEffect = ctx->moveTbl[ai->attackerMove].effect;
                    //ai->attackerMoveEffectiveness = 0;   
                    ai->attackerMoveType = ctx->moveTbl[ai->attackerMove].type;
                    ai->attackerMovePPRemaining = ctx->battlemon[ai->attacker].pp[i];
                    //BattleAI_GetTypeEffectiveness(bsys, ctx, ai->attackerMoveType,  &(ai->attackerMoveEffectiveness), &ai->attackerMon, &ai->defenderMon);
                    //AITypeCalc(ctx, ai->attackerMove, ai->attackerMoveType, ai->attackerAbility, ai->defenderAbility, ai->holdEffect, ai->defenderType1, ai->defenderType2, & ai->attackerMoveEffectiveness);
                    
/*
                    moveScores[battler_no][i] += BasicFlag(bsys, attacker, i, ai);
                    moveScores[battler_no][i] += EvaluateAttackFlag(bsys, attacker, i, ai);
                    moveScores[battler_no][i] += ExpertFlag(bsys, attacker, i, ai);
                    moveScores[battler_no][i] += TagStrategyFlag(bsys, attacker, i, ai);
*/
                    
                    if (ai->attackerMove != MOVE_NONE &&
                        (ai->attackerMove == ctx->battlemon[ai->attacker].moveeffect.moveNoChoice ||
                        ai->attackerMove == ctx->battlemon[ai->attacker].moveeffect.encoredMove)){
                        moveScores[battler_no][i] += 100; //force the user to use the move if choice locked
                    }                
                    
                    for (unsigned int j = 0; j < sizeof(moveEvaluators) / sizeof(moveEvaluators[0]); j++) {
                        debug_printf("in move evaluators");
                        if(BattleTypeGet(bsys) &  BATTLE_TYPE_DOUBLE){
                            if (bsys->trainers[1].aibit & moveEvaluators[j].flag) { //hardcoding double battles to ONLY read the first trainer's aibit, since a second trainer's does not exist.
                                                                                    //not doing this will result in the left side using random moves. This also fixes the "left side ai problem"
                                //debug_printf("for Move: %d, using Flag: %d\n", i, moveEvaluators[j].flag);
                                moveScores[battler_no][i] += moveEvaluators[j].evaluator(bsys, ai->attacker, i, ai);
                            }
                        }
                        else{
                            if (bsys->trainers[ai->attacker].aibit & moveEvaluators[j].flag) { //grab the associated ai bit for the attacker.
                                //debug_printf("for Move: %d, using Flag: %d\n", i, moveEvaluators[j].flag);
                                moveScores[battler_no][i] += moveEvaluators[j].evaluator(bsys, ai->attacker, i, ai);   
                            }
                        }
                        debug_printf("move evaluators loop j=%d\n",j);


                    }
                    debug_printf("after move evaluators");
                    if(moveScores[battler_no][i] > max_scores[battler_no]){
                        max_scores[battler_no] = moveScores[battler_no][i];             //track the highest score for this potential target
                    }
                    if(max_scores[battler_no] > highest_move_score){
                        highest_move_score = max_scores[battler_no];                    //track the absolute largest score over all potential targets
                    }
                    //debug_printf("Move: %d, Score: %d\n", i, moveScores[battler_no][i]);
                }
            }
            //debug_printf("Max score for defender %d: %d\n\n\n", battler_no, max_scores[battler_no]);

        }
        debug_printf("before tie break");
        //debug_printf("Highest move score: %d\n", highest_move_score);
        unsigned int j_tie_index = 0;
        for(unsigned int battler_no = 0; battler_no < 4; battler_no++){
            if(highest_move_score == max_scores[battler_no]){                           //find all defenders that tied for the maximum score 
                num_defender_ties++;
                defender_tie_indices[j_tie_index] = battler_no;
                j_tie_index++;
            }
        }
        target = defender_tie_indices[BattleRand(bsys) % num_defender_ties];        //randomly pick a target among the tie
        debug_printf("after target");
        ctx->aiWorkTable.ai_dir_select_client[ai->attacker] = target;                   //assign the correct target for this attacker.
        for(unsigned int i = 0; i < 4; i++){
            debug_printf("the target for battler %d is %d",i,ctx->aiWorkTable.ai_dir_select_client[i]);
        }
        debug_printf("Target: %d\n", target);

    }
    else{ //single battles
        
        /*Main loop over moves and select the best one*/
        for (unsigned int i = 0; i < 4; i++)
        {   
            /*Move-relevant variables*/
            ai->attackerMove = ctx->battlemon[ai->attacker].move[i];
            ai->attackerMoveEffect = ctx->moveTbl[ai->attackerMove].effect;
            //ai->attackerMoveEffectiveness = 0;       
            ai->attackerMoveType = ctx->moveTbl[ai->attackerMove].type;
            ai->attackerMovePPRemaining = ctx->battlemon[ai->attacker].pp[i];
            //AITypeCalc(ctx, ai->attackerMove, ai->attackerMoveType, ai->attackerAbility, ai->defenderAbility, ai->holdEffect, ai->defenderType1, ai->defenderType2, & ai->attackerMoveEffectiveness);
            if (ai->attackerMove != MOVE_NONE &&
                (ai->attackerMove == ctx->battlemon[ai->attacker].moveeffect.moveNoChoice ||
                ai->attackerMove == ctx->battlemon[ai->attacker].moveeffect.encoredMove)){
                moveScores[target][i] += 100; //force the user to use the move if choice locked
            }     
            for (unsigned int j = 0; j < sizeof(moveEvaluators) / sizeof(moveEvaluators[0]); j++) {
                if (bsys->trainers[ai->attacker].aibit & moveEvaluators[j].flag) {
                    moveScores[target][i] += moveEvaluators[j].evaluator(bsys, ai->attacker, i, ai);
                }
            }
            debug_printf("Move: %d, Score: %d\n", i, moveScores[target][i]);
        }
        ctx->aiWorkTable.ai_dir_select_client[ai->attacker] = target;                   //target is always 0 in single battles (the player)
    }
    for(unsigned int i = 0; i < 4; i++){
        if(moveScores[target][i] > moveScores[target][result]){
            result = i;
        }
    }

    highest_move_score = moveScores[target][result];
    unsigned int j_tie_index = 0;

    for (unsigned int moveno = 0; moveno < 4; moveno++){                                         //check for ties
        if(moveScores[target][moveno] == highest_move_score){
            num_move_score_ties++;
            move_tie_indices[j_tie_index] = moveno;
            j_tie_index++;
        }
    }
    result  = move_tie_indices[BattleRand(bsys) % num_move_score_ties];             //randomly pick a move among the tie


    return result;
}

/*Lists of effects, moves, etc...*/
const u16 DropStatList[] = {
    MOVE_EFFECT_ATK_DOWN,
    MOVE_EFFECT_ATK_DOWN_2,
    MOVE_EFFECT_ATK_DOWN_3,
    MOVE_EFFECT_DEF_DOWN,
    MOVE_EFFECT_DEF_DOWN_2,
    MOVE_EFFECT_DEF_DOWN_3,
    MOVE_EFFECT_SPEED_DOWN,
    MOVE_EFFECT_SPEED_DOWN_2,
    MOVE_EFFECT_SPEED_DOWN_3,
    MOVE_EFFECT_SP_ATK_DOWN,
    MOVE_EFFECT_SP_ATK_DOWN_2,
    MOVE_EFFECT_SP_ATK_DOWN_3,
    MOVE_EFFECT_SP_ATK_DOWN_2_OPPOSITE_GENDER,
    MOVE_EFFECT_SP_DEF_DOWN,
    MOVE_EFFECT_SP_DEF_DOWN_2,
    MOVE_EFFECT_SP_DEF_DOWN_3,
    MOVE_EFFECT_FAINT_AND_ATK_SP_ATK_DOWN_2,
    MOVE_EFFECT_ATK_SP_ATK_DOWN,
    MOVE_EFFECT_VENOM_DRENCH,
    MOVE_EFFECT_ATK_DEF_DOWN,
    MOVE_EFFECT_EVA_DOWN,
    MOVE_EFFECT_EVA_DOWN_2,
    MOVE_EFFECT_EVA_DOWN_3,
    MOVE_EFFECT_ACC_DOWN,
    MOVE_EFFECT_ACC_DOWN_2,
    MOVE_EFFECT_ACC_DOWN_3
};

const u16 RaiseStatList[] = {
    MOVE_EFFECT_ATK_UP,
    MOVE_EFFECT_ATK_UP_2,
    MOVE_EFFECT_ATK_UP_3,
    MOVE_EFFECT_DEF_UP,
    MOVE_EFFECT_DEF_UP_2,
    MOVE_EFFECT_DEF_UP_3,
    MOVE_EFFECT_SPEED_UP,
    MOVE_EFFECT_SPEED_UP_2,
    MOVE_EFFECT_SPEED_UP_3,
    MOVE_EFFECT_SP_ATK_UP,
    MOVE_EFFECT_SP_ATK_UP_2,
    MOVE_EFFECT_SP_ATK_UP_3,
    MOVE_EFFECT_SP_DEF_UP,
    MOVE_EFFECT_SP_DEF_UP_2,
    MOVE_EFFECT_SP_DEF_UP_3,
    MOVE_EFFECT_ACC_UP,
    MOVE_EFFECT_ACC_UP_2,
    MOVE_EFFECT_ACC_UP_3,
    MOVE_EFFECT_EVA_UP,
    MOVE_EFFECT_EVA_UP_2,
    MOVE_EFFECT_EVA_UP_2_MINIMIZE,
    MOVE_EFFECT_EVA_UP_3,
    MOVE_EFFECT_ATK_SPEED_UP,
    MOVE_EFFECT_ATK_SP_ATK_UP,
    MOVE_EFFECT_DEF_UP_DOUBLE_ROLLOUT_POWER,
    MOVE_EFFECT_DEF_SP_DEF_UP,
    MOVE_EFFECT_SP_ATK_SP_DEF_UP

};
/*Stat increases*/

//Shell Smash, Quiver Dance, Dragon Dance, Tidy Up, Shift Gear
const u16 DualRaiseList[] = {
    MOVE_EFFECT_ATK_SP_ATK_SPEED_UP_2_DEF_SP_DEF_DOWN,
    MOVE_EFFECT_SP_ATK_SP_DEF_SPEED_UP,
    MOVE_EFFECT_ATK_SPEED_UP,
    MOVE_EFFECT_TIDY_UP,
    MOVE_EFFECT_SPEED_UP_2_ATK_UP,
};

const u16 AttackRaiseList[] = {
    MOVE_EFFECT_ATK_UP,
    MOVE_EFFECT_ATK_UP_2,
    MOVE_EFFECT_ATK_UP_3,
    MOVE_EFFECT_ATK_SPEED_UP, //dragon dance
    //MOVE_EFFECT_MAX_ATK_LOSE_HALF_MAX_HP, //belly drum
    MOVE_EFFECT_RAISE_ALL_STATS_LOSE_THIRD_MAX_HP, //clangorous soul
    MOVE_EFFECT_ATK_DEF_ACC_UP,
    MOVE_EFFECT_SP_ATK_UP, //growth
    MOVE_EFFECT_ATK_SP_ATK_UP, //work up
    MOVE_EFFECT_ATK_ACC_UP, //hone claws
    //MOVE_EFFECT_RAISE_ATTACK_HIT, //powerup punch
   // MOVE_EFFECT_SPEED_UP_2_ATK_UP, //shift gear is part of speed raise list
    MOVE_EFFECT_ATK_SP_ATK_SPEED_UP_2_DEF_SP_DEF_DOWN, //shell smash
    MOVE_EFFECT_TIDY_UP, //tidy up is basically ddance
    MOVE_EFFECT_ATK_DEF_SPEED_UP, //victory dance
    MOVE_EFFECT_ATK_DEF_ACC_UP, //coil
};

const u16 DefenseRaiseList[] = {
    MOVE_EFFECT_DEF_UP,
    MOVE_EFFECT_DEF_UP_2,
    MOVE_EFFECT_DEF_UP_3,
    MOVE_EFFECT_DEF_UP_DOUBLE_ROLLOUT_POWER,
    MOVE_EFFECT_DEF_SP_DEF_UP,
};

const u16 SpAtkRaiseList[] = {
    MOVE_EFFECT_SP_ATK_UP,
    MOVE_EFFECT_SP_ATK_UP_2,
    MOVE_EFFECT_SP_ATK_UP_3,
    MOVE_EFFECT_ATK_SP_ATK_UP,  //work up
    MOVE_EFFECT_SP_ATK_SP_DEF_UP, //calm mind
    //MOVE_EFFECT_ATK_SP_ATK_SPEED_UP_2_LOSE_HALF_MAX_HP, //fillet away
    MOVE_EFFECT_RAISE_SP_ATK_HIT, //mystical power, torch song
    MOVE_EFFECT_ATK_SP_ATK_SPEED_UP_2_DEF_SP_DEF_DOWN, //shell smash
    MOVE_EFFECT_SP_ATK_SP_DEF_SPEED_UP, //quiver dance
    MOVE_EFFECT_TAKE_HEART,
};

const u16 SpDefRaiseList[] = {
    MOVE_EFFECT_SP_DEF_UP,
    MOVE_EFFECT_SP_DEF_UP_2,
    MOVE_EFFECT_SP_DEF_UP_3,
    //MOVE_EFFECT_SP_ATK_SP_DEF_UP
};

const u16 SpeedRaiseList[] = {
    MOVE_EFFECT_SPEED_UP,
    MOVE_EFFECT_SPEED_UP_2,
    MOVE_EFFECT_SPEED_UP_3, //dragon dance is intentionally excluded
    MOVE_EFFECT_SPEED_UP_2_ATK_UP, //shift gear is considered a speed raise, not attack raise since it give +2 speed
};

const u16 AccRaiseList[] = {
    MOVE_EFFECT_ACC_UP,
    MOVE_EFFECT_ACC_UP_2,
    MOVE_EFFECT_ACC_UP_3
};
const u16 EvaRaiseList[] = {
    MOVE_EFFECT_EVA_UP,
    MOVE_EFFECT_EVA_UP_2,
    MOVE_EFFECT_EVA_UP_2_MINIMIZE,
    MOVE_EFFECT_EVA_UP_3
};

/*Stat decreases*/
const u16 AttackDropList[] = {
    MOVE_EFFECT_ATK_DOWN,
    MOVE_EFFECT_ATK_DOWN_2,
    MOVE_EFFECT_ATK_DOWN_3,
    MOVE_EFFECT_ATK_SP_ATK_DOWN,
    MOVE_EFFECT_ATK_DEF_DOWN,
    //MOVE_EFFECT_FAINT_AND_ATK_SP_ATK_DOWN_2,
    //MOVE_EFFECT_VENOM_DRENCH
};

const u16 DefenseDropList[] = {
    MOVE_EFFECT_DEF_DOWN,
    MOVE_EFFECT_DEF_DOWN_2,
    MOVE_EFFECT_DEF_DOWN_3,
    MOVE_EFFECT_ATK_DEF_DOWN
};

const u16 SpAtkDropList[] = {
    MOVE_EFFECT_SP_ATK_DOWN,
    MOVE_EFFECT_SP_ATK_DOWN_2,
    MOVE_EFFECT_SP_ATK_DOWN_3,
    //MOVE_EFFECT_FAINT_AND_ATK_SP_ATK_DOWN_2,
    MOVE_EFFECT_SP_ATK_DOWN_2_OPPOSITE_GENDER,
    MOVE_EFFECT_ATK_SP_ATK_DOWN,
    //MOVE_EFFECT_VENOM_DRENCH
};

const u16 SpDefDropList[] = {
    MOVE_EFFECT_SP_DEF_DOWN,
    MOVE_EFFECT_SP_DEF_DOWN_2,
    MOVE_EFFECT_SP_DEF_DOWN_3
};

const u16 SpeedDropList[] = {
    MOVE_EFFECT_SPEED_DOWN,
    MOVE_EFFECT_SPEED_DOWN_2,
    MOVE_EFFECT_SPEED_DOWN_3,
    MOVE_EFFECT_VENOM_DRENCH
};

const u16 AccDropList[] = {
    MOVE_EFFECT_ACC_DOWN,
    MOVE_EFFECT_ACC_DOWN_2,
    MOVE_EFFECT_ACC_DOWN_3
};

const u16 EvaDropList[] = {
    MOVE_EFFECT_EVA_DOWN,
    MOVE_EFFECT_EVA_DOWN_2,
    MOVE_EFFECT_EVA_DOWN_3
};  

/*Other Lists*/
const u16 MirrorMoveEffectList[] ={
    MOVE_EFFECT_STATUS_SLEEP,
    MOVE_EFFECT_ACC_DOWN,
    MOVE_EFFECT_ONE_HIT_KO,
    MOVE_EFFECT_STATUS_BADLY_POISON,
    MOVE_EFFECT_HIGH_CRITICAL,
    MOVE_EFFECT_STATUS_CONFUSE,
    MOVE_EFFECT_DEF_DOWN_2,
    MOVE_EFFECT_SPEED_DOWN_2,
    MOVE_EFFECT_SP_DEF_DOWN_2,
    MOVE_EFFECT_STATUS_PARALYZE,
    MOVE_EFFECT_STATUS_POISON,
    MOVE_EFFECT_CONFUSE_HIT,
    MOVE_EFFECT_ONE_HIT_KO,
    MOVE_EFFECT_STEAL_HELD_ITEM,
    MOVE_EFFECT_INFATUATE,
    MOVE_EFFECT_ATK_UP_2_STATUS_CONFUSION,
    MOVE_EFFECT_TORMENT,
    MOVE_EFFECT_SP_ATK_UP_CAUSE_CONFUSION,
    MOVE_EFFECT_SWITCH_HELD_ITEMS,
    MOVE_EFFECT_USER_ATK_DEF_DOWN_HIT,
    MOVE_EFFECT_SWITCH_ABILITIES,
    MOVE_EFFECT_TRANSFER_STATUS,
    MOVE_EFFECT_SWAP_ATK_SP_ATK_STAT_CHANGES,
    MOVE_EFFECT_SWAP_DEF_SP_DEF_STAT_CHANGES,
    MOVE_EFFECT_HIT_FIRST_IF_TARGET_ATTACKING,
    MOVE_EFFECT_PRIORITY_1,
    MOVE_EFFECT_SWAP_STAT_CHANGES,
    MOVE_EFFECT_SP_ATK_DOWN_2_OPPOSITE_GENDER
};

const u16 SetupFirstTurnList[] = {
    MOVE_EFFECT_CONVERSION,
    MOVE_EFFECT_SET_REFLECT,
    MOVE_EFFECT_SET_LIGHT_SCREEN,
    MOVE_EFFECT_STATUS_PARALYZE,
    MOVE_EFFECT_STATUS_POISON,
    MOVE_EFFECT_STATUS_BADLY_POISON,
    MOVE_EFFECT_STATUS_BURN,
    MOVE_EFFECT_STATUS_SLEEP,
    MOVE_EFFECT_STATUS_CONFUSE,
    MOVE_EFFECT_ATK_UP_2_STATUS_CONFUSION,
    MOVE_EFFECT_SP_ATK_UP_CAUSE_CONFUSION,
    MOVE_EFFECT_STATUS_LEECH_SEED,
    MOVE_EFFECT_SET_SUBSTITUTE,
    MOVE_EFFECT_EVA_UP_2_MINIMIZE,
    MOVE_EFFECT_TORMENT,
    MOVE_EFFECT_GROUND_TRAP_USER_CONTINUOUS_HEAL,
    MOVE_EFFECT_MAKE_SHARED_MOVES_UNUSEABLE,
    MOVE_EFFECT_DOUBLE_SPEED_3_TURNS,
    MOVE_EFFECT_PREVENT_CRITS,
    MOVE_EFFECT_GIVE_GROUND_IMMUNITY,
    MOVE_EFFECT_REMOVE_HAZARDS_SCREENS_EVA_DOWN,
    MOVE_EFFECT_WHIRLPOOL,
    MOVE_EFFECT_BIND_HIT
};

const u16 RiskyFlagList[] = {
    MOVE_EFFECT_STATUS_SLEEP,
    MOVE_EFFECT_HALVE_DEFENSE,
    MOVE_EFFECT_COPY_MOVE,
    MOVE_EFFECT_ONE_HIT_KO,
    MOVE_EFFECT_HIGH_CRITICAL,
    MOVE_EFFECT_HIGH_CRITICAL_POISON_HIT,
    MOVE_EFFECT_SLEEP_POISON_PARALYZE_HIT,
    MOVE_EFFECT_HIGH_CRITICAL_RAISE_SPEED_HIT,
    MOVE_EFFECT_HIGH_CRITICAL_POISON_HIT,
    MOVE_EFFECT_CALL_RANDOM_MOVE,
    MOVE_EFFECT_RANDOM_DAMAGE_1_TO_150_LEVEL,
    MOVE_EFFECT_COUNTER,
    MOVE_EFFECT_MIRROR_COAT,
    MOVE_EFFECT_METAL_BURST,
    MOVE_EFFECT_KO_MON_THAT_DEFEATED_USER,
    MOVE_EFFECT_ATK_UP_2_STATUS_CONFUSION,
    MOVE_EFFECT_INFATUATE,
    MOVE_EFFECT_RANDOM_POWER_MAYBE_HEAL,
    MOVE_EFFECT_RAISE_ALL_STATS_HIT,
    MOVE_EFFECT_MAX_ATK_LOSE_HALF_MAX_HP,
    MOVE_EFFECT_HIT_LAST_WHIFF_IF_HIT,
    MOVE_EFFECT_RANDOM_STAT_UP_2,
    MOVE_EFFECT_DOUBLE_POWER_IF_HIT,
    MOVE_EFFECT_USE_MOVE_FIRST,
    MOVE_EFFECT_HIT_FIRST_IF_TARGET_ATTACKING
};


const u16 PrioritizeDamageFlagList[] = {
    MOVE_EFFECT_HALVE_DEFENSE,
    MOVE_EFFECT_RECOVER_DAMAGE_SLEEP,
    MOVE_EFFECT_CHARGE_TURN_HIGH_CRIT,
    MOVE_EFFECT_CHARGE_TURN_DEF_UP,
    MOVE_EFFECT_CHARGE_TURN_HIGH_CRIT_FLINCH,
    MOVE_EFFECT_CHARGE_TURN_SUN_SKIPS,
    MOVE_EFFECT_CHARGE_TURN_SP_ATK_UP,
    MOVE_EFFECT_CHARGE_TURN_SP_ATK_UP_RAIN_SKIPS,
    MOVE_EFFECT_RECHARGE_AFTER,
    MOVE_EFFECT_SPIT_UP,
    MOVE_EFFECT_HIT_LAST_WHIFF_IF_HIT,
    MOVE_EFFECT_USER_ATK_DEF_DOWN_HIT,
    MOVE_EFFECT_INCREASE_POWER_WITH_LESS_HP,
    MOVE_EFFECT_HIT_FIRST_IF_TARGET_ATTACKING,
    MOVE_EFFECT_RECOIL_HALF,
    MOVE_EFFECT_RANDOM_TYPE_BASED_ON_IVS,
    MOVE_EFFECT_POWER_BASED_ON_LOW_SPEED,
    MOVE_EFFECT_NATURAL_GIFT,
    MOVE_EFFECT_JUDGMENT,
    MOVE_EFFECT_POWER_BASED_ON_FRIENDSHIP,
    MOVE_EFFECT_POWER_BASED_ON_LOW_FRIENDSHIP,
    MOVE_EFFECT_INCREASE_POWER_WITH_WEIGHT,
    MOVE_EFFECT_RANDOM_DAMAGE_1_TO_150_LEVEL,
    MOVE_EFFECT_LEVEL_DAMAGE_FLAT,
    MOVE_EFFECT_40_DAMAGE_FLAT,
    MOVE_EFFECT_10_DAMAGE_FLAT
};

const u16 BatonPassFlagList[] = {
    MOVE_EFFECT_ATK_UP_2,
    MOVE_EFFECT_ATK_SPEED_UP,
    MOVE_EFFECT_SP_ATK_SP_DEF_UP,
    MOVE_EFFECT_SP_ATK_UP_2
};

const u16 HarassmentFlagList[] = {//also included in harassment flag are all statdrop list moves
    MOVE_EFFECT_STATUS_PARALYZE,
    MOVE_EFFECT_STATUS_POISON,
    MOVE_EFFECT_STATUS_BADLY_POISON,
    MOVE_EFFECT_STATUS_BURN,
    MOVE_EFFECT_STATUS_SLEEP,
    MOVE_EFFECT_STATUS_CONFUSE,
    MOVE_EFFECT_ATK_UP_2_STATUS_CONFUSION,
    MOVE_EFFECT_SP_ATK_UP_CAUSE_CONFUSION,
    MOVE_EFFECT_INFATUATE,
    MOVE_EFFECT_STATUS_LEECH_SEED,
    MOVE_EFFECT_ENCORE,
    MOVE_EFFECT_DECREASE_LAST_MOVE_PP,
    MOVE_EFFECT_SET_SPIKES,
    MOVE_EFFECT_TORMENT,
    MOVE_EFFECT_NATURE_POWER,
    MOVE_EFFECT_REMOVE_HELD_ITEM,
    MOVE_EFFECT_MAKE_SHARED_MOVES_UNUSEABLE,
    MOVE_EFFECT_SECRET_POWER,
    MOVE_EFFECT_ATK_DEF_DOWN,
    MOVE_EFFECT_CAMOUFLAGE,
    MOVE_EFFECT_PREVENT_ITEM_USE,
    MOVE_EFFECT_TRANSFER_STATUS,
    MOVE_EFFECT_TOXIC_SPIKES,
    MOVE_EFFECT_REMOVE_HAZARDS_SCREENS_EVA_DOWN,
    MOVE_EFFECT_SP_ATK_DOWN_2_OPPOSITE_GENDER
};

const u16 CheckHPFlagList_1[] = {
    MOVE_EFFECT_RESTORE_HALF_HP,
    MOVE_EFFECT_HEAL_HALF_MORE_IN_SUN,
    MOVE_EFFECT_KO_MON_THAT_DEFEATED_USER,
    MOVE_EFFECT_INCREASE_POWER_WITH_LESS_HP,
    MOVE_EFFECT_FAINT_AND_ATK_SP_ATK_DOWN_2,
    MOVE_EFFECT_FAINT_AND_FULL_HEAL_NEXT_MON,
    MOVE_EFFECT_REMOVE_ALL_PP_ON_DEFEAT
};

const u16 CheckHPFlagList_2[] = {
    MOVE_EFFECT_CRIT_UP_2,
    MOVE_EFFECT_BIDE,
    MOVE_EFFECT_CONVERSION,
    MOVE_EFFECT_CONVERSION2,
    MOVE_EFFECT_SET_LIGHT_SCREEN,
    MOVE_EFFECT_SET_REFLECT,
    MOVE_EFFECT_PREVENT_STAT_REDUCTION,
    MOVE_EFFECT_PREVENT_STATUS,
    MOVE_EFFECT_MAX_ATK_LOSE_HALF_MAX_HP
};

const u16 CheckHPFlagList_3[] = {
    MOVE_EFFECT_PREVENT_CRITS,
    MOVE_EFFECT_SWAP_ATK_SP_ATK_STAT_CHANGES,
    MOVE_EFFECT_SWAP_DEF_SP_DEF_STAT_CHANGES,
};

const u16 CheckHPFlagList_4[] = {
    MOVE_EFFECT_RAISE_ATK_WHEN_HIT,
    MOVE_EFFECT_NEXT_ATTACK_ALWAYS_HITS,
    MOVE_EFFECT_COPY_STAT_CHANGES,
    MOVE_EFFECT_MIRROR_COAT,
    MOVE_EFFECT_METAL_BURST,
    MOVE_EFFECT_DECREASE_POWER_WITH_LESS_USER_HP,
    MOVE_EFFECT_HALVE_ELECTRIC_DAMAGE,
    MOVE_EFFECT_HALVE_FIRE_DAMAGE,
    MOVE_EFFECT_RANDOM_STAT_UP_2
};

const u16 CheckHPFlagList_5[] = {
    MOVE_EFFECT_STATUS_POISON,
    MOVE_EFFECT_PREVENT_STAT_REDUCTION,
    MOVE_EFFECT_AVERAGE_HP,
    MOVE_EFFECT_PREVENT_STATUS,
    MOVE_EFFECT_RANDOM_STAT_UP_2,
    MOVE_EFFECT_INCREASE_POWER_WITH_MORE_HP,
    MOVE_EFFECT_ALL_FAINT_3_TURNS
};

const u16 CheckHPFlagList_6[] = {
    MOVE_EFFECT_STATUS_PARALYZE,
    MOVE_EFFECT_STATUS_POISON,
    MOVE_EFFECT_STATUS_BADLY_POISON,
    MOVE_EFFECT_STATUS_BURN,
    MOVE_EFFECT_STATUS_SLEEP,
    MOVE_EFFECT_STATUS_CONFUSE,
    MOVE_EFFECT_ATK_UP_2_STATUS_CONFUSION,
    MOVE_EFFECT_SP_ATK_UP_CAUSE_CONFUSION,
    MOVE_EFFECT_BIDE,
    MOVE_EFFECT_CONVERSION,
    MOVE_EFFECT_CONVERSION2,
    MOVE_EFFECT_SET_LIGHT_SCREEN,
    MOVE_EFFECT_SET_REFLECT,
    MOVE_EFFECT_ONE_HIT_KO,
    MOVE_EFFECT_HALVE_HP,
    MOVE_EFFECT_NEXT_ATTACK_ALWAYS_HITS,
    MOVE_EFFECT_DECREASE_LAST_MOVE_PP,
    MOVE_EFFECT_DOUBLE_POWER_EACH_TURN_LOCK_INTO,
    MOVE_EFFECT_COPY_STAT_CHANGES,
    MOVE_EFFECT_MIRROR_COAT,
    MOVE_EFFECT_COUNTER,
    MOVE_EFFECT_HALVE_DEFENSE
};

const u16 TagStrategyFlagList_SkillSwap[] = {
    MOVE_FIRE_BLAST,
    MOVE_THUNDER,
    MOVE_CROSS_CHOP,
    MOVE_HYDRO_PUMP,
    MOVE_DYNAMIC_PUNCH,
    MOVE_BLIZZARD,
    MOVE_ZAP_CANNON,
    MOVE_MEGAHORN,
    MOVE_FOCUS_BLAST,
    MOVE_GUNK_SHOT,
    MOVE_MAGMA_STORM,
    MOVE_POWER_WHIP,
    MOVE_SEED_FLARE,
    MOVE_HEAD_SMASH
};

const u16 MovesEffectsWithFlatDamageOrOHKO[] = {
    MOVE_EFFECT_ONE_HIT_KO,
    MOVE_EFFECT_METAL_BURST,
    MOVE_EFFECT_COUNTER,
    MOVE_EFFECT_BIDE,
    MOVE_EFFECT_40_DAMAGE_FLAT,
    MOVE_EFFECT_SET_HP_EQUAL_TO_USER,
    MOVE_EFFECT_HALVE_HP,
    MOVE_EFFECT_MIRROR_COAT,
    MOVE_EFFECT_LEVEL_DAMAGE_FLAT,
    MOVE_EFFECT_10_DAMAGE_FLAT
};

const u16 SetupAttackEffectsList[] = {
    
};

/*Flag's Scoring:

This next section is the meat of the AI. Each of these flags individually modifies the score of a move,
given an attacker and a defender. The scores start at 100, and are modified accordingly based on the 
the power, usefulness, or lackthereof of the move. If you want to add a flag, you can mirror the way
these are written, and add it to the list of moveEvaluators. The flags are all defined in trainer_ai.h,
so change those names there, too.
*/

/*BasicFlag:
Heavily penalize stupid decisions that would fail, do nothing, or objectively hurt the user.*/

int BasicFlag (struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai){
    int moveScore = 0;
    struct BattleStruct *ctx = bsys->sp;
    /*
    if(ctx->battlemon[ai->defender].hp == 0){
        return -60;
    }
    */
    /*Never use moves that are out of pp*/
    if(ctx->battlemon[ai->attacker].pp[i] == 0){
        moveScore -= 40;
    }
    if(ctx->battlemon[ai->attacker].moveeffect.tauntTurns > 0 && 
        ctx->moveTbl[ai->attackerMove].split == SPLIT_STATUS){
        moveScore -= 50; //taunted, so no status moves
    }

    if(ctx->battlemon[ai->attacker].moveeffect.disabledMove == ai->attackerMove &&
        ctx->battlemon[ai->attacker].moveeffect.disabledTurns > 0){
        moveScore -= 40; //disabled, will softlock
    }


    /*Check for ai->defender type immunities.*/
    if(ai->attackerMoveEffectiveness[i] == TYPE_MUL_NO_EFFECT && ctx->moveTbl[ai->attackerMove].split != SPLIT_STATUS){
        moveScore -= 25;
    }
    if(ctx->moveTbl[ai->attackerMove].split == SPLIT_STATUS && ai->defenderAbility == ABILITY_MAGIC_BOUNCE){
        moveScore -= 15; //status moves that bounce back to the user
    }
    if(ai->attackerMove == MOVE_THUNDER_WAVE && HasType(ctx, ai->defender, TYPE_GROUND)){
        moveScore -= 25;
    }

    /*Check for wonder guard*/
    if(ai->attackerMoveEffectiveness[i] < TYPE_MUL_SUPER_EFFECTIVE &&
        ai->defenderAbility == ABILITY_WONDER_GUARD && ai->attackerAbility != ABILITY_MOLD_BREAKER){
        moveScore -= 15;
    }

    //Ability immunities
    if(ai->attackerAbility != ABILITY_MOLD_BREAKER){
        if(ai->attackerMoveType == TYPE_ELECTRIC && //electric
            (ai->defenderAbility == ABILITY_VOLT_ABSORB ||
            ai->defenderAbility == ABILITY_LIGHTNING_ROD ||
            ai->defenderAbility == ABILITY_MOTOR_DRIVE) ){
            moveScore -= 15;
        }
        if(ai->attackerMoveType == TYPE_WATER && //water
            (ai->defenderAbility == ABILITY_STORM_DRAIN ||
            ai->defenderAbility == ABILITY_WATER_ABSORB ||
            ai->defenderAbility == ABILITY_DRY_SKIN || 
            ctx->field_condition & WEATHER_EXTREMELY_HARSH_SUNLIGHT)  ){
            moveScore -= 15;
        }
        if(ai->attackerMoveType == TYPE_FIRE && //fire
            (ai->defenderAbility == ABILITY_FLASH_FIRE || 
            ai->defenderAbility == ABILITY_WELL_BAKED_BODY || 
            ai->defenderAbility == ABILITY_THERMAL_EXCHANGE ||
            ctx->field_condition & WEATHER_HEAVY_RAIN) ){
            moveScore -= 15;
        }
        if(ai->attackerMoveType == TYPE_GRASS && //grass
            (ai->defenderAbility == ABILITY_SAP_SIPPER) ){
            moveScore -= 15;
        }
        if(ai->attackerMoveType == TYPE_GROUND && //ground
            (ai->defenderAbility == ABILITY_LEVITATE ||
            ai->defenderAbility == ABILITY_EARTH_EATER) ){
            moveScore -= 15;
        }
        if(IsMoveSoundBased(ai->attackerMove) && //sound based moves
            ai->defenderAbility == ABILITY_SOUNDPROOF){ 
            moveScore -= 15;
        }
        if(ctx->moveTbl[ai->attackerMove].split == SPLIT_STATUS &&
            ai->defenderAbility == ABILITY_GOOD_AS_GOLD && 
            !(ctx->moveTbl[ai->attackerMove].target == RANGE_USER ||  //do not include things like light screen, stealth rocks, self-targetting moves.
              ctx->moveTbl[ai->attackerMove].target == RANGE_OPPONENT_SIDE ||
              (ctx->moveTbl[ai->attackerMove].target == RANGE_USER_SIDE && ai->attackerMove != MOVE_LIFE_DEW )) ){
            moveScore -= 15;
        }
    }

    /*Check for grass immunity to powder moves*/
    // Isnt working????
    if(((IsPowderMove(ai->attackerMove) || ai->attackerMove == MOVE_LEECH_SEED) && HasType(ctx, ai->defender, TYPE_GRASS))
        || ((ai->attackerMove == MOVE_SLEEP_POWDER || ai->attackerMove == MOVE_STUN_SPORE) && HasType(ctx, ai->defender, TYPE_GRASS))){
        moveScore -= 20;
    }

    if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_LEECH_SEED){
        if(HasType(ctx, ai->defender, TYPE_GRASS) ||
         ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_LEECH_SEED_ACTIVE ||
         ctx->battlemon[ai->defender].condition2 & STATUS2_SUBSTITUTE){
            moveScore -= 20;
        }
    }

    //ai->attackerMove == MOVE_RAGE_POWDER  rage powder is self-targetting, so will grass types never click rage powder?

    /*Prankster:
    Prankster fails if:
    1.) Move target is dark type
    2.) Target is not self
    3.) Move is priority boosted by prankster (i.e. status moves)
    
    We check if the base priority of the move is zero or greater, since 
    it would be boosted by prankster to +1 or higher if it is status*/
    if((ai->defenderType1 == TYPE_DARK || ai->defenderType2 == TYPE_DARK) && 
        (ctx->moveTbl[ai->attackerMove].split == SPLIT_STATUS &&
        ai->attackerAbility == ABILITY_PRANKSTER &&
        ctx->moveTbl[ai->attackerMove].priority >= 0)
        && attacker != ai->defender){
        moveScore -= 15;
    }

    /*Check for immunity to paralysis*/
    if((ai->attackerMoveEffect == MOVE_EFFECT_STATUS_PARALYZE) && ai->defenderImmuneToParalysis ){
        moveScore -= 15;
    }

    /*Check for immunity to burn*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_STATUS_BURN) && ai->defenderImmuneToBurn){
        moveScore -= 15;
    }

    /*Check for immunity to poison/badly poison*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_STATUS_POISON ||
        ai->attackerMoveEffect == MOVE_EFFECT_STATUS_BADLY_POISON) && ai->defenderImmuneToPoison){
        moveScore -= 15;
    }

    /*Check for immunity to sleep*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_STATUS_SLEEP ||
        ai->attackerMoveEffect == MOVE_EFFECT_STATUS_SLEEP_NEXT_TURN) &&  ai->defenderImmuneToSleep){
        moveScore -= 15;
    }
    else if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_SLEEP_NEXT_TURN){
        if(ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_YAWN_COUNTER ||
        !(ctx->battlemon[ai->defender].condition & STATUS_NONE)){
            moveScore -= 15; //yawn counter is active, or defender is already asleep
        }
    }

    /*Check for immunity to confusion*/ 
    else if((ai->attackerMoveEffect == MOVE_EFFECT_STATUS_CONFUSE ||
        ai->attackerMoveEffect == MOVE_EFFECT_ATK_UP_2_STATUS_CONFUSION || //swagger
        ai->attackerMoveEffect == MOVE_EFFECT_SP_ATK_UP_CAUSE_CONFUSION|| //flatter
        ai->attackerMoveEffect == MOVE_EFFECT_CONFUSE_ALL_ADJACENT ) && //teeter dance, need to change for double battles
        (ctx->battlemon[ai->defender].condition2 & STATUS2_CONFUSION || 
        ctx->side_condition[ai->defenderSide] & SIDE_STATUS_SAFEGUARD ||
        ai->defenderAbility == ABILITY_OWN_TEMPO )){
        moveScore -= 15;
    }
    /*Check for immunity to infatuation*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_INFATUATE) && 
        (ctx->battlemon[ai->defender].condition2 & STATUS2_ATTRACT || 
        ai->defenderAbility == ABILITY_OBLIVIOUS ||
        ctx->battlemon[ai->attacker].sex == ctx->battlemon[ai->defender].sex ||
        ctx->battlemon[ai->defender].sex == POKEMON_GENDER_UNKNOWN)){
        moveScore -= 15;
    }
    /*Terrain Setting*/
    else if(ai->attackerMove == MOVE_GRASSY_TERRAIN){
        if(ctx->terrainOverlay.type == GRASSY_TERRAIN && ctx->terrainOverlay.numberOfTurnsLeft > 0){
            moveScore -= 15;
        }
    }
    else if(ai->attackerMove == MOVE_MISTY_TERRAIN){
        if(ctx->terrainOverlay.type == MISTY_TERRAIN && ctx->terrainOverlay.numberOfTurnsLeft > 0){
            moveScore -= 15;
        }
    }
    else if(ai->attackerMove == MOVE_ELECTRIC_TERRAIN){
        if(ctx->terrainOverlay.type == ELECTRIC_TERRAIN && ctx->terrainOverlay.numberOfTurnsLeft > 0){
            moveScore -= 15;
        }
    }
    else if(ai->attackerMove == MOVE_PSYCHIC_TERRAIN){
        if(ctx->terrainOverlay.type == PSYCHIC_TERRAIN && ctx->terrainOverlay.numberOfTurnsLeft > 0){
            moveScore -= 15;
        }
    }
    /*Priority moves on Psychic Terrain*/
    else if(ctx->terrainOverlay.type == PSYCHIC_TERRAIN && ctx->terrainOverlay.numberOfTurnsLeft > 0 && IsClientGrounded(ctx, ai->defender) && 
        (ctx->moveTbl[ai->attackerMove].priority > 0 ||
       (ctx->moveTbl[ai->attackerMove].split == SPLIT_STATUS &&
        ai->attackerAbility == ABILITY_PRANKSTER && ctx->moveTbl[ai->attackerMove].target != RANGE_USER))){
        moveScore -= 20; //priority moves are negated by psychic terrain
    }
    /*Check for consecutive destiny bonds*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_KO_MON_THAT_DEFEATED_USER &&
            ctx->battlemon[ai->attacker].condition2 & STATUS2_DESTINY_BOND){
        moveScore -= 15; //destiny bond is already active, don't use it again in a row
    }

    /*Check for self-killing moves */
    else if((ai->attackerMoveEffect == MOVE_EFFECT_HALVE_DEFENSE ) && 
    ((ai->defenderAbility == ABILITY_DAMP && ai->attackerAbility != ABILITY_MOLD_BREAKER) ||
        (ai->livingMembersAttacker == 1 && ai->livingMembersDefender > 1))){
        moveScore -= 15; //prevents ai auto-losing the battle
    }
    else if((ai->attackerMoveEffect == MOVE_EFFECT_HALVE_DEFENSE) &&
        (ai->livingMembersAttacker == 1 && ai->livingMembersDefender == 1)){
        moveScore = -1; //Ai will consider exploding if both parties have one Pokemon left
    }

    /*Handle nightmare*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_NIGHTMARE && 
    (ctx->battlemon[ai->defender].condition2 & STATUS2_NIGHTMARE ||
        ai->defenderAbility == ABILITY_MAGIC_GUARD)){
        moveScore -= 15;
    }
    else if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_NIGHTMARE &&
        !(ctx->battlemon[ai->defender].condition & STATUS_SLEEP)){
        moveScore -= 15;
    }

    /*Handle dream eater*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_RECOVER_DAMAGE_SLEEP &&
        !(ctx->battlemon[ai->defender].condition & STATUS_SLEEP)){ //must be asleep
        moveScore -= 15;
    }

    /*Handle belly drum*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_MAX_ATK_LOSE_HALF_MAX_HP &&
        (ai->attackerHP * 100 / ai->attackerMaxHP < 51 )){
        moveScore -= 15;
    }

    /*Handle bad stat boosts*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_SPEED_UP_3 || //trick room + only speed boost status move
        ai->attackerMoveEffect == MOVE_EFFECT_SPEED_UP_2 ||
        ai->attackerMoveEffect == MOVE_EFFECT_SPEED_UP) &&
        (ctx->field_condition & FIELD_STATUS_TRICK_ROOM)){
        moveScore -= 15;
    }

    else if((IsInList(ai->attackerMoveEffect, AccRaiseList, NELEMS(AccRaiseList)) ||
            IsInList(ai->attackerMoveEffect, EvaRaiseList, NELEMS(EvaRaiseList))) &&
        (ai->attackerAbility == ABILITY_NO_GUARD)){
        moveScore -= 15;
    }

    /*Avoid increasing already maxed stats using status moves*/

    // attack
    else if((IsInList(ai->attackerMoveEffect, AttackRaiseList, NELEMS(AttackRaiseList))) &&
        (ctx->battlemon[attacker].states[STAT_ATTACK] >= 12)){
        moveScore -= 15; 
    }
    // defense
    else if((IsInList(ai->attackerMoveEffect, DefenseRaiseList, NELEMS(DefenseRaiseList))) &&
        (ctx->battlemon[attacker].states[STAT_DEFENSE] >= 12)){
        moveScore -= 15;
    }
    // sp. atk
    else if((IsInList(ai->attackerMoveEffect, SpAtkRaiseList, NELEMS(SpAtkRaiseList))) &&
        (ctx->battlemon[attacker].states[STAT_SPATK] >= 12)){
        moveScore -= 15;
    }
    //sp.def
    else if((IsInList(ai->attackerMoveEffect, SpDefRaiseList, NELEMS(SpDefRaiseList))) &&
        (ctx->battlemon[attacker].states[STAT_SPDEF] >= 12)){
        moveScore -= 15;
    }
    //speed
    else if((IsInList(ai->attackerMoveEffect, SpeedRaiseList, NELEMS(SpeedRaiseList))) &&
        (ctx->battlemon[attacker].states[STAT_SPEED] >= 12)){
        moveScore -= 15;
    }

    /*Check for abilities preventing atk drop*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_ATK_DOWN_2 ||
        ai->attackerMoveEffect == MOVE_EFFECT_ATK_DOWN) &&
        (ai->defenderAbility == ABILITY_HYPER_CUTTER ||
        ai->defenderAbility == ABILITY_WHITE_SMOKE ||
        ai->defenderAbility == ABILITY_CLEAR_BODY)){
        moveScore -= 15;
    }
    else if((IsInList(ai->attackerMoveEffect, AccDropList, NELEMS(AccDropList)) ||
            IsInList(ai->attackerMoveEffect, EvaDropList, NELEMS(EvaDropList))) &&
        (ai->attackerAbility == ABILITY_NO_GUARD ||
        ai->defenderAbility == ABILITY_NO_GUARD)){
        moveScore -= 15;
    }

    else if(IsInList(ai->attackerMoveEffect, AccDropList, NELEMS(AccDropList)) &&
        (ai->defenderAbility == ABILITY_KEEN_EYE)){
        moveScore -= 15;
    }

    /*Handle clear body & white smoke*/
    else if(DropsStatsAsStatus(ai->attackerMoveEffect) &&
        ((ai->defenderAbility == ABILITY_CLEAR_BODY ||
        ai->defenderAbility == ABILITY_WHITE_SMOKE) &&
        ai->attackerAbility != ABILITY_MOLD_BREAKER)){
            moveScore -= 15;
    }

    /*Handle any status stat-dropping move when at +6 or -6.
    Keep in mind that -6 to +6 goes instead from 0 to 12.*/

    // Attack
    else if((IsInList(ai->attackerMoveEffect, AttackDropList, NELEMS(AttackDropList))) &&
        (ctx->battlemon[ai->defender].states[STAT_ATTACK] <= 0)){
        moveScore -= 15;
    }
    // Defense
    else if((IsInList(ai->attackerMoveEffect, DefenseDropList, NELEMS(DefenseDropList))) &&
        (ctx->battlemon[ai->defender].states[STAT_DEFENSE] <= 0)){
        moveScore -= 15;
    }
    // Sp. Atk
    else if((IsInList(ai->attackerMoveEffect, SpAtkDropList, NELEMS(SpAtkDropList))) &&
        (ctx->battlemon[ai->defender].states[STAT_SPATK] <= 0)){
        moveScore -= 15;
    }
    //Sp.Def
    else if((IsInList(ai->attackerMoveEffect, SpDefDropList, NELEMS(SpDefDropList))) &&
        (ctx->battlemon[ai->defender].states[STAT_SPDEF] <= 0)){
        moveScore -= 15;
    }
    //Speed
    else if((IsInList(ai->attackerMoveEffect, SpeedDropList, NELEMS(SpeedDropList))) &&
        (ctx->battlemon[ai->defender].states[STAT_SPEED] <= 0)){
        moveScore -= 15;
    }
    //Evasion
    else if((IsInList(ai->attackerMoveEffect, EvaDropList, NELEMS(EvaDropList))) &&
        (ctx->battlemon[ai->defender].states[STAT_EVASION] <= 0)){
        moveScore -= 15;
    }
    //Accuracy
    else if((IsInList(ai->attackerMoveEffect, AccDropList, NELEMS(AccDropList)) ) &&
        (ctx->battlemon[ai->defender].states[STAT_ACCURACY] <= 0)){
        moveScore -= 15;
    }

    /*Check for exclusively speed dropping moves in Trick Room*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SPEED_DOWN_2 && //string shot and scary face
        (ctx->field_condition & FIELD_STATUS_TRICK_ROOM)){
            moveScore -= 15;
    }

    /*Handle Haze, Psych Up, Heart Swap*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_RESET_STAT_CHANGES ||
    ai->attackerMoveEffect == MOVE_EFFECT_SWAP_STAT_CHANGES ||
    ai->attackerMoveEffect == MOVE_EFFECT_COPY_STAT_CHANGES) &&
        !ai->hasStatChange){
        moveScore -= 15;
    }

    /*Handle forced switching moves*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_FORCE_SWITCH && 
    (ai->livingMembersDefender == 1 || 
        (ai->defenderAbility == ABILITY_SUCTION_CUPS && ai->attackerAbility != ABILITY_MOLD_BREAKER))){
        moveScore -= 15;
    }

    /*Handle recovery moves*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_RESTORE_HALF_HP || 
        ai->attackerMoveEffect == MOVE_EFFECT_HEAL_HALF_MORE_IN_SUN ||
        ai->attackerMoveEffect == MOVE_EFFECT_SWALLOW || 
        ai->attackerMoveEffect == MOVE_EFFECT_HEAL_HALF_REMOVE_FLYING_TYPE) &&
        ai->attackerHP == ai->attackerMaxHP){
        moveScore -= 8;
    }

    /*OHKO moves*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_ONE_HIT_KO) &&
        ((ai->defenderAbility == ABILITY_STURDY && ai->attackerAbility != ABILITY_MOLD_BREAKER) || 
        ctx->battlemon[attacker].level < ctx->battlemon[ai->defender].level)){
            moveScore -= 15;
    }

    /*Misc persistent effects
    Focus Energy / Ingrain / Mud Sport / Water Sport / Camouflage /
    Power Trick / Lucky Chant / Aqua Ring*/
    else if((ctx->battlemon[ai->attacker].condition2 & STATUS2_FOCUS_ENERGY  &&
    ai->attackerMoveEffect == MOVE_EFFECT_CRIT_UP_2)||
    (ctx->battlemon[attacker].effect_of_moves & MOVE_EFFECT_FLAG_INGRAIN  &&
        ai->attackerMoveEffect == MOVE_EFFECT_GROUND_TRAP_USER_CONTINUOUS_HEAL) ||
        (ctx->battlemon[attacker].effect_of_moves & MOVE_EFFECT_FLAG_WATER_SPORT  &&
            ai->attackerMoveEffect == MOVE_EFFECT_HALVE_ELECTRIC_DAMAGE) ||
            (ctx->battlemon[attacker].effect_of_moves & MOVE_EFFECT_FLAG_MUD_SPORT &&
                ai->attackerMoveEffect == MOVE_EFFECT_HALVE_FIRE_DAMAGE) ||
                (ctx->battlemon[attacker].effect_of_moves & MOVE_EFFECT_FLAG_CAMOUFLAGE &&
                    ai->attackerMoveEffect == MOVE_EFFECT_CAMOUFLAGE) ||
                    (ctx->battlemon[attacker].effect_of_moves & MOVE_EFFECT_FLAG_POWER_TRICK &&
                        ai->attackerMoveEffect == MOVE_EFFECT_SWAP_ATK_DEF) ||
                        (ctx->side_condition[ai->attackerSide] & SIDE_STATUS_LUCKY_CHANT &&
                            ai->attackerMoveEffect == MOVE_EFFECT_PREVENT_CRITS) ||
                            (ctx->battlemon[attacker].effect_of_moves & MOVE_EFFECT_FLAG_AQUA_RING &&
                                ai->attackerMoveEffect == MOVE_EFFECT_RESTORE_HP_EVERY_TURN)){
        moveScore -= 25;
    }
    else if(ai->attackerMoveEffect == MOVE_EFFECT_TAUNT){
        if(ctx->battlemon[ai->defender].moveeffect.tauntTurns > 0){
            moveScore -= 15; //taunted, so no status moves
        }
    }

    /*Magnet Rise*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_GIVE_GROUND_IMMUNITY &&
        (ctx->battlemon[attacker].effect_of_moves & MOVE_EFFECT_FLAG_MAGNET_RISE || ai->attackerType1  == TYPE_FLYING || ai->attackerType2  == TYPE_FLYING || ai->attackerAbility == ABILITY_LEVITATE)){
        debug_printf("Magnet rise effect of moves is %d\n", ctx->battlemon[attacker].effect_of_moves & MOVE_EFFECT_FLAG_MAGNET_RISE);
        debug_printf("Attacker type 1 is %d, type 2 is %d\n", ai->attackerType1  == TYPE_FLYING, ai->attackerType2  == TYPE_FLYING);
        debug_printf("Attacker ability is %d\n", ai->attackerAbility == ABILITY_LEVITATE);
            moveScore -= 15; //magnet rise is already active, or flying type, or levitate
    }

    /*Handle substitute*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SET_SUBSTITUTE && 
        (ctx->battlemon[attacker].condition2 & STATUS2_SUBSTITUTE || ai->attackerHP*100/ai->attackerMaxHP < 26) ){
        moveScore -= 15;
    } //might have to consider status moves that are blocked by sub (unrelated)

    /*Handle leech seed*/
    else if(ai->attackerMove == MOVE_LEECH_SEED && 
        (ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_LEECH_SEED_ACTIVE ||
        ai->defenderType1 == TYPE_GRASS || ai->defenderType2 == TYPE_GRASS ||
        ai->defenderAbility == ABILITY_MAGIC_GUARD )){
        moveScore -= 15;
    }

    /*Handle disable & encore*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_DISABLE && 
        (ctx->battlemon[ai->defender].moveeffect.disabledTurns != 0) ){
        moveScore -= 8;
    }
    else if(ai->attackerMoveEffect == MOVE_EFFECT_ENCORE && 
        (ctx->battlemon[ai->defender].moveeffect.encoredTurns != 0) ){
        moveScore -= 8;
    }

    /*Handle sleep-necessitated moves*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_USE_RANDOM_LEARNED_MOVE_SLEEP || //snore and sleep talk
        ai->attackerMoveEffect == MOVE_EFFECT_DAMAGE_WHILE_ASLEEP) && 
        !(ctx->battlemon[attacker].condition & STATUS_SLEEP) ){
        moveScore -= 15;
    }

    /*Other miscellaneous persistent effects
    Lock On / Mean Look / Foresight / Perish Song / 
    Torment / Miracle Eye / Heal Block / Gastro Acid*/
    else if((ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_LOCK_ON  &&
    (ai->attackerMoveEffect == MOVE_EFFECT_NEXT_ATTACK_ALWAYS_HITS ||
        ai->defenderAbility == ABILITY_NO_GUARD || ai->attackerAbility == ABILITY_NO_GUARD))||
    (ctx->battlemon[ai->defender].condition2 & STATUS2_MEAN_LOOK  &&
        ai->attackerMoveEffect == MOVE_EFFECT_PREVENT_ESCAPE) ||
        (ctx->battlemon[ai->defender].condition2 & STATUS2_FORESIGHT &&
            ai->attackerMoveEffect == MOVE_EFFECT_IGNORE_EVASION_REMOVE_GHOST_IMMUNE) ||
            (ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_PERISH_SONG_ACTIVE &&
                ai->attackerMoveEffect == MOVE_EFFECT_ALL_FAINT_3_TURNS) ||
                (ctx->battlemon[ai->defender].condition2 & STATUS2_TORMENT &&
                    ai->attackerMoveEffect == MOVE_EFFECT_TORMENT) ||
                    (ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_MIRACLE_EYE &&
                        ai->attackerMoveEffect == MOVE_EFFECT_IGNORE_EVATION_REMOVE_DARK_IMMUNE) ||
                        (ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_HEAL_BLOCK &&
                            ai->attackerMoveEffect == MOVE_EFFECT_PREVENT_HEALING) ||
                            (ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_GASTRO_ACID &&
                                (ai->attackerMoveEffect == MOVE_EFFECT_GASTRO_ACID ||ai->defenderAbility == ABILITY_MULTITYPE ||
                                    ai->defenderAbility == ABILITY_TRUANT || ai->defenderAbility == ABILITY_SLOW_START ||
                                    ai->defenderAbility == ABILITY_STENCH || ai->defenderAbility == ABILITY_RUN_AWAY || 
                                    ai->defenderAbility == ABILITY_PICKUP || ai->defenderAbility == ABILITY_HONEY_GATHER))){
        moveScore -= 15;
    }

    /*Handle Curse*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_CURSE && 
        (ai->attackerType1 == TYPE_GHOST || ai->attackerType2 == TYPE_GHOST ||
        ctx->battlemon[ai->defender].condition2 == STATUS2_CURSE || ai->defenderAbility == ABILITY_MAGIC_GUARD) )||
        (ai->attackerMoveEffect == MOVE_EFFECT_CURSE && 
            (ai->attackerType1 != TYPE_GHOST || ai->attackerType2 == TYPE_GHOST) &&
            (ctx->battlemon[attacker].states[STAT_ATTACK] >= 12 || ctx->battlemon[attacker].states[STAT_DEFENSE] >= 12))){
        moveScore -= 15;
    }

    /*Handle hazards*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_STEALTH_ROCK && 
        (ctx->side_condition[ai->defenderSide] & SIDE_STATUS_STEALTH_ROCK || ai->livingMembersDefender == 1 )) ||
        (ai->attackerMoveEffect == MOVE_EFFECT_SET_SPIKES && 
            (ctx->scw[ai->defenderSide].spikesLayers >= 3 || ai->livingMembersDefender == 1) )||
            (ai->attackerMoveEffect == MOVE_EFFECT_TOXIC_SPIKES && 
                (ctx->scw[ai->defenderSide].toxicSpikesLayers >= 2 || ai->livingMembersDefender == 1))){
        moveScore -= 15;
    }

    /*Handle weather*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_SANDSTORM && 
        (ctx->field_condition & WEATHER_SANDSTORM_ANY)) ||
        (ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_HAIL && 
            (ctx->field_condition & WEATHER_HAIL_ANY )) ||
            (ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_SNOW && 
                (ctx->field_condition & WEATHER_SNOW_ANY )) ||
            (ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_RAIN && 
                (ctx->field_condition & WEATHER_RAIN_ANY)) ||
                (ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_SUN && 
                    (ctx->field_condition & WEATHER_SUNNY_ANY))){
        moveScore -= 8;
    }

    else if((ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_RAIN && (ai->attackerAbility != ABILITY_SWIFT_SWIM && ai->attackerAbility != ABILITY_HYDRATION)) && 
        (!(ctx->battlemon[ai->defender].condition & STATUS_NONE) &&
        ctx->battlemon[ai->defender].ability == ABILITY_HYDRATION) ){
        moveScore -= 8;
    } //Basically don't cure opponent's status with rain dance

    if((ai->attackerMove == MOVE_HAIL) && (ai->defenderAbility == ABILITY_ICE_BODY)){
        moveScore -= 8;
    }
    if((ai->attackerMove == MOVE_HAIL) && (ai->attackerAbility == ABILITY_ICE_BODY)){
        moveScore += 8;
    }//Nice logic, gamefreak

    /*Handle doom desire and future sight*/
    if((ai->attackerMoveEffect == MOVE_EFFECT_HIT_IN_3_TURNS) &&
        ctx->fcc.future_prediction_count[ai->defender] !=0 ){
        moveScore -= 12;
    }
    /*Handle baton pass, shed tail*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_PASS_STATS_AND_STATUS || ai->attackerMoveEffect == MOVE_EFFECT_SHED_TAIL) &&
        ai->livingMembersAttacker == 1){
        moveScore -= 15;
    }
    /*Handle fake out after turn one*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_ALWAYS_FLINCH_FIRST_TURN_ONLY && ai->attackerTurnsOnField > 0){
        moveScore -= 25;
    }
    /*Handle stockpile*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_STOCKPILE && ctx->battlemon[attacker].moveeffect.stockpileCount < 3){
        moveScore -= 15;
    }
    /*Spit up / Swallow*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_SPIT_UP || ai->attackerMoveEffect == MOVE_EFFECT_SWALLOW) 
    && ctx->battlemon[attacker].moveeffect.stockpileCount == 0){
        moveScore -= 15;
    }
    /*Handle Memento*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_FAINT_AND_ATK_SP_ATK_DOWN_2) && (((ai->defenderAbility == ABILITY_WHITE_SMOKE ||
        ai->defenderAbility == ABILITY_CLEAR_BODY) && ai->attackerAbility != ABILITY_MOLD_BREAKER) ||
        (ctx->battlemon[attacker].states[STAT_ATTACK] <= 0 ||ctx->battlemon[attacker].states[STAT_SPATK] <= 0))){
            moveScore -= 15;
    }

    /*Handle Helping Hand*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_BOOST_ALLY_POWER_BY_50_PERCENT && !(BattleTypeGet(bsys) & (BATTLE_TYPE_MULTI | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TAG))){
        moveScore -= 15;
    }

    /*Trick, Switcheroo, Knock Off*/ 
    //These checks from game freak make absolutely no sense. Knock off can still do boosted damage
    //against sticky hold targets, and you can still give your item to your opponent if they
    //dont have their own. I'm going to skip these for now.

    /*Handle imprison*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_MAKE_SHARED_MOVES_UNUSEABLE && 
        (ctx->battlemon[attacker].effect_of_moves & MOVE_EFFECT_FLAG_IMPRISONED ||
            ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_IMPRISONED )){
                moveScore -= 15;
    }

    /*Handle Refresh*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_HEAL_STATUS && 
        !(ctx->battlemon[ai->defender].condition & STATUS_POISON_ALL ||
            ctx->battlemon[ai->defender].condition & STATUS_PARALYSIS ||
            ctx->battlemon[ai->defender].condition & STATUS_BURN )){
                moveScore -= 15;
    }

    /*Handle moves that change more than one type of stat*/
    /*Tickle*/
    if(ai->attackerMoveEffect == MOVE_EFFECT_ATK_DEF_DOWN && //tickle, in particular
        (ctx->battlemon[ai->defender].states[STAT_ATTACK] <= 0 )){
                moveScore -= 15;
    }
    if(ai->attackerMoveEffect == MOVE_EFFECT_ATK_DEF_DOWN &&
        (ctx->battlemon[ai->defender].states[STAT_DEFENSE] <= 0)){
                moveScore -= 8;
    }// Punishes each stat if already dropped to the max
    // The clear body interactions are handled above

    /*Bulk Up*/
    if(ai->attackerMoveEffect == MOVE_EFFECT_ATK_DEF_UP && 
        (ctx->battlemon[attacker].states[STAT_ATTACK] <= 0 )){
                moveScore -= 15;
    }
    if(ai->attackerMoveEffect == MOVE_EFFECT_ATK_DEF_UP &&
        (ctx->battlemon[attacker].states[STAT_DEFENSE] <= 0)){
                moveScore -= 8;
    }// Punishes each stat if already raised to +6

    /*Calm Mind*/
    if(ai->attackerMoveEffect == MOVE_EFFECT_SP_ATK_SP_DEF_UP && 
        (ctx->battlemon[attacker].states[STAT_SPATK] <= 0)){
                moveScore -= 15;
    }
    if(ai->attackerMoveEffect == MOVE_EFFECT_SP_ATK_SP_DEF_UP &&
        (ctx->battlemon[attacker].states[STAT_SPDEF] <= 0)){
                moveScore -= 8;
    }// Punishes each stat if already raised to +6

    /*Dragon Dance*/
    if(ai->attackerMoveEffect == MOVE_EFFECT_ATK_SPEED_UP && 
        (ctx->battlemon[attacker].states[STAT_ATTACK] <= 0 )){
                moveScore -= 15;
    }
    if(ai->attackerMoveEffect == MOVE_EFFECT_ATK_SPEED_UP &&
        (ctx->battlemon[attacker].states[STAT_SPEED] <= 0)){
                moveScore -= 8;
    }// Punishes each stat if already raised to +6

    /*Tailwind*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_DOUBLE_SPEED_3_TURNS &&
    (ctx->side_condition[ai->attackerSide] & SIDE_STATUS_TAILWIND ||
        ctx->field_condition & FIELD_STATUS_TRICK_ROOM)){
        moveScore -= 15;
    }

    /*Gravity*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_GRAVITY &&
    (ctx->field_condition & FIELD_STATUS_GRAVITY)){
        moveScore -= 15;
    }

    /*Trick Room*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_TRICK_ROOM &&
    (ai->attackerMovesFirst || ai->isSpeedTie)){
        moveScore -= 15;
    }

    /*Healing Wish and Lunar Dance*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_FAINT_FULL_RESTORE_NEXT_MON){
        moveScore = -20;
    }

    if((ai->attackerMoveEffect == MOVE_EFFECT_FAINT_AND_FULL_HEAL_NEXT_MON)  &&
        (ai->damagedMonsDefender > 0 || ai->statusedMonsAttacker > 0)){
        moveScore -= 15;
    }
    if((ai->attackerMoveEffect == MOVE_EFFECT_FAINT_FULL_RESTORE_NEXT_MON) &&
        (ai->livingMembersAttacker == 1)){
        moveScore -= 15;//can penalize twice
    }

    /*Natural Gift*/
    if(ai->attackerMoveEffect == MOVE_EFFECT_NATURAL_GIFT &&
        (!IS_ITEM_BERRY(ai->attackerItem))){
            moveScore -= 15;
    }

    /*Acupressure*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_RANDOM_STAT_UP_2 &&
        (ctx->battlemon[attacker].states[STAT_ATTACK] >= 12 ||
        ctx->battlemon[attacker].states[STAT_DEFENSE] >= 12 ||
        ctx->battlemon[attacker].states[STAT_SPATK] >= 12 ||
        ctx->battlemon[attacker].states[STAT_SPDEF] >= 12 ||
        ctx->battlemon[attacker].states[STAT_SPEED] >= 12 ||
        ctx->battlemon[attacker].states[STAT_EVASION] >= 12 ||
        ctx->battlemon[attacker].states[STAT_ACCURACY] >= 12)){
            moveScore -= 15;
    }

    /*Metal Burst*/
    if(ai->attackerMoveEffect == MOVE_EFFECT_METAL_BURST &&
        (ai->defenderAbility == ABILITY_STALL || ai->attackerMovesFirst)){
            moveScore -= 15;
    }
    if(ai->attackerMoveEffect == MOVE_EFFECT_METAL_BURST && ai->attackerAbility == ABILITY_STALL){
        moveScore += 10;
    }

    /*Embargo*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_PREVENT_ITEM_USE &&
        (ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_EMBARGO)){
            moveScore -= 15;
    }

    /*Fling*/
    /*TODO: immunities to fling effects like burn from flame orb on a fire type ai->defender*/
    if(ai->attackerMoveEffect == MOVE_EFFECT_FLING &&
        (ai->attackerAbility == ABILITY_MULTITYPE || ai->attackerItem == ITEM_NONE || 
        ai->flingPower == 0)){
        moveScore -= 15;
    }

    /*Psycho Shift*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_TRANSFER_STATUS &&
        (ctx->battlemon[ai->defender].condition & STATUS_ALL || 
            ctx->side_condition[ai->defenderSide] & SIDE_STATUS_SAFEGUARD ||
        (ctx->battlemon[attacker].condition & STATUS_BURN && ai->defenderImmuneToBurn) ||
        (ctx->battlemon[attacker].condition & STATUS_POISON_ALL && ai->defenderImmuneToPoison) ||
        (ctx->battlemon[attacker].condition & STATUS_PARALYSIS && ai->defenderImmuneToParalysis) )){
            moveScore -= 15;
    }

    /*Copycat*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_USE_LAST_USED_MOVE){
        if(ai->attackerMovesFirst && ctx->total_turn == 0 && ai->attackerMovesFirst){
            moveScore -= 15;
        }
    }

    /*Power Swap / Guard Swap*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SWAP_ATK_SP_ATK_STAT_CHANGES &&
        (ctx->battlemon[attacker].states[STAT_ATTACK] > ctx->battlemon[ai->defender].states[STAT_ATTACK] &&
            ctx->battlemon[attacker].states[STAT_SPATK] > ctx->battlemon[ai->defender].states[STAT_SPATK] )){
            moveScore -= 15;
    }
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SWAP_DEF_SP_DEF_STAT_CHANGES &&
        (ctx->battlemon[attacker].states[STAT_DEFENSE] > ctx->battlemon[ai->defender].states[STAT_DEFENSE] &&
            ctx->battlemon[attacker].states[STAT_SPDEF] > ctx->battlemon[ai->defender].states[STAT_SPDEF] )){
            moveScore -= 15;
    }

    /*Last Resort*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_FAIL_IF_NOT_USED_ALL_OTHER_MOVES &&
        (ctx->battlemon[attacker].moveeffect.lastResortCount < ai->attackerMovesKnown - 1)){
            moveScore -= 15;
    }

    /*Worry Seed*/
    BOOL defender_knows_sleepy_moves = 0;
    for (int i = 0; i < 4; i++){
        if(ctx->battlemon[ai->defender].move[i] == MOVE_SLEEP_TALK ||
            ctx->battlemon[ai->defender].move[i] == MOVE_SNORE){
                defender_knows_sleepy_moves = 1;
        }
    }

    if(ai->attackerMoveEffect == MOVE_EFFECT_SET_ABILITY_TO_INSOMNIA &&
    (ai->defenderAbility == ABILITY_TRUANT || ai->defenderAbility == ABILITY_INSOMNIA 
        ||ai->defenderAbility == ABILITY_VITAL_SPIRIT ||ai->defenderAbility == ABILITY_MULTITYPE ||
        (ctx->battlemon[ai->defender].condition & STATUS_SLEEP && defender_knows_sleepy_moves == 0))){
        moveScore -= 15;
    }

    /*Defog*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_REMOVE_HAZARDS_SCREENS_EVA_DOWN &&
        (ai->livingMembersDefender == 1 || 
            (ctx->scw[ai->attackerSide].toxicSpikesLayers == 0 &&
            ctx->scw[ai->attackerSide].spikesLayers == 0 &&
            !(ctx->side_condition[ai->defenderSide] & SIDE_STATUS_STEALTH_ROCK)))){
        moveScore -= 15;
    }

    /*Captivate*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SP_ATK_DOWN_2_OPPOSITE_GENDER &&
    (ctx->battlemon[ai->defender].sex == ctx->battlemon[ai->attacker].sex ||
        ctx->battlemon[ai->defender].sex == POKEMON_GENDER_UNKNOWN ||
        ctx->battlemon[ai->defender].states[STAT_SPATK] == 0)){
        moveScore -= 15;
    }
    return moveScore;
}

/*Rank moves based on their damage output, ability to ohko, 2hko, */
int EvaluateAttackFlag (struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai){
    int moveScore = 0;
    struct BattleStruct *ctx = bsys->sp;
    BOOL is_current_move_not_strongest = 0;
    if(ctx->moveTbl[ai->attackerMove].split == SPLIT_STATUS || ai->attackerMoveEffect == MOVE_EFFECT_HALVE_DEFENSE){
        return 0;
    }
    for(int j = 0; j < ai->attackerMovesKnown; j++){
        if ( i != j && ai->attackerAvgRollMoveDamages[i] < ai->attackerAvgRollMoveDamages[j]){
            is_current_move_not_strongest = 1;
        }
    }
    for(int j = 0; j < 4; j++){
        //debug_printf("Move %d: Max damage roll %d\n", j, ai->attackerAvgRollMoveDamages[j]);
    }
    /*If AI is about to die, get some priority damage*/
    if(ai->maxDamageReceived > ai->attackerHP && ai->defenderMovesFirst && !BattlerHasMoveEffect(bsys, ai->attacker, MOVE_EFFECT_PROTECT, ai)){//last argument is a failsafe for double battles
        if(ctx->moveTbl[ai->attackerMove].priority > 0 ){
            moveScore += 12;
        }
    }
    /*Check if the current move kills*/
    else if (ai->attackerAvgRollMoveDamages[i] >= ai->defenderHP){
 
        if(ai->attackerMoveEffect == MOVE_EFFECT_HIT_BEFORE_SWITCH && //pursuit while AI moves first
                ai->attackerMovesFirst){
            moveScore += 15;
        }
        else if(ai->attackerMoveEffect == MOVE_EFFECT_HIT_BEFORE_SWITCH && //pursuit while AI moves second, but survives enemy attack
                (ai->defenderMovesFirst && ai->maxDamageReceived < ai->attackerHP)){
            moveScore += 14;
        }
        //Prioritize priority moves that kill
        else if((ai->attackerMoveEffect == MOVE_EFFECT_ALWAYS_FLINCH_FIRST_TURN_ONLY &&
                ai->attackerTurnsOnField == 0)){
                moveScore += 14;
        }
        else if(!(ai->attackerMoveEffect == MOVE_EFFECT_ALWAYS_FLINCH_FIRST_TURN_ONLY)  && (ctx->moveTbl[ai->attackerMove].priority > 0 ||
                (ai->attackerMove == MOVE_GRASSY_GLIDE && ctx->terrainOverlay.type == GRASSY_TERRAIN && ctx->terrainOverlay.numberOfTurnsLeft > 0))){
            moveScore += 14;
        }
        /*Prioritize fast kills*/
        else if(ai->attackerMovesFirst){
            moveScore += 12;
        }
        /*Slow Kill (slower or speed tie)*/
        else{
            moveScore += 10;
        }
    }

    /*+6 priority is reserved for speed dropping moves like bulldoze (expert flag)*/

    /*+5 priority is reserved for 2 hit KO with setup (expert flag)*/

    /*Prioritize damaging moves if we can do a 2-hit KO,
    This is notably a lower score than using setup moves that
    help kill in the same number of turns
    
    It also ONLY happens if the move isn't being boosted by another effect*/
    else if((ai->attackerAvgRollMoveDamages[i] >= ai->defenderHP / 2)){
        
        moveScore += 4;
    }

    /*Rank moves based on relative damage output, if we don't see a kill.
    This fixes a quirk that will cause 2nd & 3rd highest move damages to tie in score if the strongest move can't be used.
    We exclude fake out from this scoring since the point of the move is not damage, but a free flinch*/
    if(ai->attackerMoveEffect == MOVE_EFFECT_ALWAYS_FLINCH_FIRST_TURN_ONLY){
        return moveScore;
    }

    if(ai->attackerAvgRollMoveDamages[i] < ai->defenderHP && is_current_move_not_strongest != 0){
        for(int j = 0; j < 4; j++){
            if(ai->attackerAvgRollMoveDamages[j] > ai->attackerAvgRollMoveDamages[i]){
                
                moveScore -= 1; //for each move that is stronger than this one, subtract a score. Avoids random move when highest power move cannot be used.  
            }
        }
    }

    debug_printf("Move score returned from evaluate attack flag: %d\n", moveScore);
    return moveScore;
}

/*In the case of custom ai hacks, this should probably where edits happen.
A lot of this logic adds in probability.*/

/*Expert Flag is where the meat and bones of moves happens.
It evaluates how good a non-damaging move is, or a move with extra effects that are relevant.*/
int ExpertFlag (struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai){
    int moveScore = 0;
    struct BattleStruct *ctx = bsys->sp;
    debug_printf("In expert flag\n");

    /*Sleep-status OR yawn*/
    /*IRIDIUM: 50% chance if AI doesn't see a 2 hit KO
    Keep in mind that +4 is ai seeing a 2 hit KO*/
    if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_SLEEP ){
        if(ctx->moveTbl[ai->attackerMove].accuracy == 100 || ai->attackerAbility == ABILITY_COMPOUND_EYES){
            moveScore += 5; //on par with setup 2hko
        }
        else if(BattleRand(bsys) % 2 < 1){
            moveScore += 2;
        }
    }

    else if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_SLEEP_NEXT_TURN){
        if(BattleRand(bsys) % 2 < 1){
            moveScore += 2;
        }
    }

    /*Burn*/
    /*IRIDIUM check for physical stat being higher*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_BURN){
        if(ctx->battlemon[ai->defender].attack > ctx->battlemon[ai->defender].spatk){
            moveScore += 2;
        }
        if(BattlerHasMoveEffect(bsys, attacker, MOVE_EFFECT_DOUBLE_DAMAGE_ON_STATUS, ai)){
            moveScore += 2; //these +2 scores will stack
        }
        if(ai->attackerMaxDamageOutputMinRoll < ai->defenderHP / 3){
            moveScore += 1;
        }
    }

    /*Poison should only be used if user/ai->defender is > 50% HP*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_STATUS_POISON ||
        ai->attackerMoveEffect == MOVE_EFFECT_STATUS_BADLY_POISON)){
        if(ai->defenderPercentHP < 50 || ai->attackerPercentHP < 50){
            moveScore -= 1;
        }
        else if(ai->attackerMaxDamageOutputMinRoll < ai->defenderHP / 3){ //if we can't 3 shot
            moveScore += 3;
        }
        else if(ai->attackerMaxDamageOutputMinRoll < ai->defenderHP / 2){//if we can't 2 shot
            moveScore += 2;
        }
            
    }

    /*Paralysis*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_PARALYZE){
        if(ai->defenderMovesFirst && !ai->trickRoomActive){
            moveScore += 3;
        }
        if(ai->attackerPercentHP <= 70){
            moveScore -= 1;
        }
    }

    /*Confusion inducing moves*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_CONFUSE ||
        ai->attackerMoveEffect == MOVE_EFFECT_SP_ATK_UP_CAUSE_CONFUSION ||
        ai->attackerMoveEffect == MOVE_EFFECT_ATK_UP_2_STATUS_CONFUSION){//confuse ray, supersonic, etc.
       if(ctx->battlemon[ai->defender].condition & STATUS_PARALYSIS){
            moveScore += 2;
       }
       if(ai->attackerTurnsOnField > 1){
            moveScore -= 2;
       }
       if(BattleRand(bsys)% 2 < 1 ){
            moveScore += 2;
       }
    }
    /*IRIDIUM: handle setup moves*/

    /*Meteor Beam & Electro Shot & Geomancy*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_CHARGE_TURN_SP_ATK_UP_RAIN_SKIPS){
        if(ctx->field_condition & WEATHER_RAIN_ANY || ai->attackerItem == ITEM_POWER_HERB){
            moveScore += 2;
        }
        else{
            moveScore -= 2;
        }
    }
    else if(ai->attackerMoveEffect == MOVE_EFFECT_CHARGE_TURN_SP_ATK_UP){
        if( ai->attackerItem == ITEM_POWER_HERB){
            moveScore += 2;
        }
        else{
            moveScore -= 2;
        }
    }
    else if(ai->attackerMoveEffect == MOVE_EFFECT_CHARGE_TURN_ATK_SP_ATK_SPEED_UP_2){
        if( ai->attackerItem == ITEM_POWER_HERB){
            moveScore += 8;
        }
        else{
            moveScore -= 2;
        }
    }

    /*Handle Damaging moves that also boost our speed OR drop enemy speed
    Examples include Trailblaze or Bulldoze*/
    /*We also include thunder wave and nuzzle*/
    else if(((ai->attackerMoveEffect == MOVE_EFFECT_RAISE_SPEED_HIT && ai->attackerAbility != ABILITY_SHEER_FORCE && ctx->moveTbl[ai->attackerMove].secondaryEffectChance == 100) ||
        (ai->attackerMoveEffect == MOVE_EFFECT_LOWER_SPEED_HIT && ai->attackerAbility != ABILITY_SHEER_FORCE && ctx->moveTbl[ai->attackerMove].secondaryEffectChance == 100) || 
        ai->attackerMoveEffect == MOVE_EFFECT_STATUS_PARALYZE ||
        (ai->attackerMoveEffect == MOVE_EFFECT_PARALYZE_HIT && ai->attackerAbility != ABILITY_SHEER_FORCE && ctx->moveTbl[ai->attackerMove].secondaryEffectChance == 100 && ctx->battlemon[ai->defender].condition & CONDITION_NONE)) &&
        ai->attackerAvgRollMoveDamages[i] < ai->defenderHP &&
        ai->attackerMoveEffectiveness[i] != TYPE_MUL_NO_EFFECT){ //only do this if we can't kill, to preserve random move if we can kill

        if(!(ai->trickRoomActive) && ai->defenderMovesFirst){ //this is tiered below slow kills, but above 2 hit KOs and setup into OHKO
            debug_printf("Defender moves first is true.\n");
            return 6;
        }
        else if(ai->trickRoomActive){ //let's not go boosting the enemy turn order
            moveScore -= 2;
        }
    }
    else if(IsInList(ai->attackerMoveEffect, SpeedDropList, NELEMS(SpeedDropList))){

            if(ai->maxDamageReceived > ai->attackerHP || ai->attackerTurnsOnField > 2){
                return -3;
            }
            if((ai->defenderMovesFirst || ai->isSpeedTie) && !ai->trickRoomActive ){
                moveScore += 2;
            }
            else if(ai->attackerMovesFirst){
                moveScore -= 3;
            }
    }

    /*IRIDIUM: Handle moves that boost speed and another stat.
     This includes Shell Smash, Quiver Dance, Dragon Dance, Tidy Up, Shift Gear*/
    /*First check to see if it is wise to raise attack*/
    else if(IsInList(ai->attackerMoveEffect,DualRaiseList, NELEMS(DualRaiseList))){
        if(ai->maxDamageReceived > ai->attackerHP || ai->attackerTurnsOnField > 2 || (ai->trickRoomActive && ai->attackerMovesFirst)){//last one is avoiding speed boost in trick room if we were moving first already (slower)
            return -3;
        }
        //Only boost if we aren't already +2 or higher, and we can't kill with the move, AND the defender has neither encore nor taunt
        if(ctx->battlemon[attacker].states[STAT_ATTACK] < 8 && ctx->battlemon[attacker].states[STAT_SPATK] < 8 && ai->attackerMaxDamageOutputMinRoll < ai->defenderHP &&
             !(BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_TAUNT, ai)) && !(BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_ENCORE, ai))){ 

            /*If the opponent needs at least 4 attacks to kill us, we should take the opportunity to set up*/
            if(ai->maxDamageReceived < ai->attackerHP / 3){ 
                return 5;
            }
            
            /*If we are faster, and opponent cannot 1 shot us from our current HP (*1.5 after def drop for shell smash), we should take the opportunity to set up*/
            if(ai->attackerMovesFirst){
                if((ai->maxDamageReceived * 3 / 2  < ai->attackerHP && ai->attackerMoveEffect == MOVE_EFFECT_ATK_SP_ATK_SPEED_UP_2_DEF_SP_DEF_DOWN) ||
                    (ai->maxDamageReceived < ai->attackerHP && ai->attackerMoveEffect != MOVE_EFFECT_ATK_SP_ATK_SPEED_UP_2_DEF_SP_DEF_DOWN) ){
                    /*Setup such that we can get the kill in the same amount of turns*/
                    if((ai->attackerMaxDamageOutputMinRoll * 2 > ai->defenderHP && ai->attackerMoveEffect == MOVE_EFFECT_ATK_SP_ATK_SPEED_UP_2_DEF_SP_DEF_DOWN) || //shell smash +2 stats
                        (ai->attackerMaxDamageOutputMinRoll * 3 / 2 > ai->defenderHP && ai->attackerMoveEffect != MOVE_EFFECT_ATK_SP_ATK_SPEED_UP_2_DEF_SP_DEF_DOWN)){ //other +1 dual stat boosts
                        return 5;
                    }
                    else if(ai->attackerTurnsOnField == 0){
                        return 3;
                    }
                    else{
                        return -1;
                    }
                }
            }
            /*Now if the ai is slower, the calculations change slightly. We can use these moves for speed control instead! */
            else{ 
                    if((ai->attackerMaxDamageOutputMinRoll * 2 > ai->defenderHP && ai->attackerMoveEffect == MOVE_EFFECT_ATK_SP_ATK_SPEED_UP_2_DEF_SP_DEF_DOWN) || //shell smash +2 stats
                        (ai->attackerMaxDamageOutputMinRoll * 3 / 2 > ai->defenderHP && ai->attackerMoveEffect != MOVE_EFFECT_ATK_SP_ATK_SPEED_UP_2_DEF_SP_DEF_DOWN)){
                        return 6;
                    }
                    else{
                        return 5;
                    }
                
            }
        }

    }

    else if(IsInList(ai->attackerMoveEffect,AttackRaiseList, NELEMS(AttackRaiseList)) ||
            (ai->attackerMoveEffect == MOVE_EFFECT_RAISE_ATTACK_HIT && ctx->moveTbl[ai->attackerMove].secondaryEffectChance == 100)){

        if(ai->maxDamageReceived > ai->attackerHP || ai->attackerTurnsOnField > 2){
            return -3;
        }
        
        //Only boost if we aren't already +2 or higher, and we can't kill with the move, AND the defender has neither encore nor taunt
        if(ctx->battlemon[attacker].states[STAT_ATTACK] < 8 && ai->attackerMaxDamageOutputMinRoll < ai->defenderHP &&
             !(BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_TAUNT, ai)) && !(BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_ENCORE, ai))){ 

            /*If the opponent cannot even 3 shot us, we should take the opportunity to set up*/
            if(ai->maxDamageReceived < ai->attackerHP / 3){ 
                return 5;
            }
            
            /*If we are faster, and opponent cannot 1 shot us from our current HP, we should take the opportunity to set up*/
            if(ai->attackerMovesFirst){
                if(ai->maxDamageReceived < ai->attackerHP){
                    /*Setup such that we can get the kill in the same amount of turns*/
                    if(ai->attackerMaxDamageOutputMinRoll * 2 > ai->defenderHP &&
                         (ai->attackerMoveEffect == MOVE_EFFECT_ATK_UP_2 || ai->attackerMoveEffect == MOVE_EFFECT_ATK_SP_ATK_SPEED_UP_2_DEF_SP_DEF_DOWN)){
                        return 5;
                    }

                    else if(ai->attackerMaxDamageOutputMinRoll * 3 /2 > ai->defenderHP){
                        return 5;
                    }
                    else if(ai->attackerTurnsOnField == 0){
                        return 1;
                    }
                    else{
                        return -1;
                    }
                }
            }
            /*Now if the ai is slower, the calculations change slightly. We check that the opponent cannot 2-shot us instead. */
            else{ 
                if(ai->maxDamageReceived < ai->attackerHP / 2){
                    /*Setup such that we can get the kill in the same amount of turns*/

                    if(ai->attackerMaxDamageOutputMinRoll * 2 > ai->defenderHP / 2 &&
                         (ai->attackerMoveEffect == MOVE_EFFECT_ATK_UP_2 )){
                        return 5;
                    }
                    else if(ai->attackerMaxDamageOutputMinRoll * 3 /2 > ai->defenderHP){
                        return 5;
                    }
                    else if(ai->attackerTurnsOnField == 0){
                        return 1;
                    }
                    else{
                        return -1;
                    }
                }
            }
        }
    }
    /*Special Attack*/
    else if(IsInList(ai->attackerMoveEffect,SpAtkRaiseList, NELEMS(SpAtkRaiseList))){

        if(ai->maxDamageReceived > ai->attackerHP || ai->attackerTurnsOnField > 2){
            return -3;
        }
        //Only boost if we aren't already +2 or higher, and we can't kill with the move, AND the defender has neither encore nor taunt
        if(ctx->battlemon[attacker].states[STAT_SPATK] < 8 && ai->attackerMaxDamageOutputMinRoll  < ai->defenderHP &&
             !(BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_TAUNT, ai)) && !(BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_ENCORE, ai))){ 

            /*If the opponent cannot even 3 shot us, we should take the opportunity to set up*/
            if(ai->maxDamageReceived < ai->attackerHP / 3){ 
                return 5;
            }
            
            /*If we are faster, and opponent cannot 1 shot us from our current HP, we should take the opportunity to set up*/
            /*Now if the ai is slower, the calculations change slightly. We check that the opponent cannot 2-shot us instead. */
            if((ai->attackerMovesFirst && ai->maxDamageReceived < ai->attackerHP ) || (!(ai->attackerMovesFirst) && ai->maxDamageReceived < ai->attackerHP / 2)){

                    /*Setup such that we can get the kill in the same amount of turns*/
                    if(ai->attackerMaxDamageOutputMinRoll * 5 /2 > ai->defenderHP && ai->attackerMoveEffect == MOVE_EFFECT_SP_ATK_UP_3){
                        return 5;
                    }
                    else if(ai->attackerMaxDamageOutputMinRoll * 2 > ai->defenderHP &&
                         (ai->attackerMoveEffect == MOVE_EFFECT_SP_ATK_UP_2 || ai->attackerMoveEffect == MOVE_EFFECT_ATK_SP_ATK_SPEED_UP_2_DEF_SP_DEF_DOWN)){
                        return 5;
                    }

                    else if(ai->attackerMaxDamageOutputMinRoll * 3 /2 > ai->defenderHP){
                        return 5;
                    }   
            }
        }
    }

    /*Handle Terrain setting*/
    else if(ai->attackerMove == MOVE_GRASSY_TERRAIN){
        if(BattlerHasTypeDamagingMove (bsys, attacker, TYPE_GRASS, ai) ||
            (ai->attackerAbility == ABILITY_GRASS_PELT && ctx->battlemon[ai->defender].attack > ctx->battlemon[ai->defender].spatk) ||
            BattlerKnowsMove(bsys, attacker, MOVE_GRASSY_GLIDE, ai)){
            moveScore += 2;
        }
        else if(ai->attackerItem == ITEM_TERRAIN_EXTENDER){
            moveScore += 2;
        }
    }
    else if(ai->attackerMove == MOVE_PSYCHIC_TERRAIN){
        if(BattlerHasTypeDamagingMove (bsys, attacker, TYPE_PSYCHIC, ai)){
            moveScore += 2;
        }
        else if(ai->attackerItem == ITEM_TERRAIN_EXTENDER){
            moveScore += 2;
        }
    }
    else if(ai->attackerMove == MOVE_ELECTRIC_TERRAIN){
        if(BattlerHasTypeDamagingMove (bsys, attacker, TYPE_ELECTRIC, ai)){
            moveScore += 2;
        }
        else if(ai->attackerItem == ITEM_TERRAIN_EXTENDER){
            moveScore += 2;
        }
    }

    
    /*IRIDIUM: Growl, Leer, charm etc...
    Handle Stat-dropping moves*/

    /*Attack dropping status moves*/
    else if(IsInList(ai->attackerMoveEffect,AttackDropList, NELEMS(AttackDropList))){
    // ||(ai->attackerMoveEffect == MOVE_EFFECT_LOWER_ATTACK_HIT && ctx->moveTbl[ai->attackerMove].secondaryEffectChance == 100)
        if(ai->maxDamageReceived > ai->attackerHP || ai->attackerTurnsOnField > 2){
            return -3;
        }
        /*Only bother dropping the opponent's offensive stat
        if it's the dominant one*/
        if(ctx->battlemon[ai->defender].attack > ctx->battlemon[ai->defender].spatk){
            if(ctx->battlemon[ai->defender].states[STAT_ATTACK] >= 6){
                moveScore += 2;
            }
            else if(ctx->battlemon[ai->defender].states[STAT_ATTACK] >= 5){
                moveScore += 1;
            }
            else{
                moveScore -= 3;
            }
        }
        else{
            moveScore -= 3;
        }
    }

    /*Sp. Atk dropping status moves*/
    else if(IsInList(ai->attackerMoveEffect,SpAtkDropList, NELEMS(SpAtkDropList))){
    // || (ai->attackerMoveEffect == MOVE_EFFECT_LOWER_SP_ATK_HIT && ctx->moveTbl[ai->attackerMove].secondaryEffectChance == 100)
        if(ai->maxDamageReceived > ai->attackerHP || ai->attackerTurnsOnField > 2){
            return -3;
        }
        /*Only bother dropping the opponent's offensive stat
        if it's the dominant one*/
        if(ctx->battlemon[ai->defender].spatk > ctx->battlemon[ai->defender].attack){
            if(ctx->battlemon[ai->defender].states[STAT_SPATK] >= 6){
                moveScore += 2;
            }
            else if(ctx->battlemon[ai->defender].states[STAT_SPATK] >= 5){
                moveScore += 1;
            }
            else{
                moveScore -= 3;
            }
        }
        else{
            moveScore -= 3;
        }
    }

    /*Defense dropping status moves*/
    else if(IsInList(ai->attackerMoveEffect,DefenseDropList, NELEMS(DefenseDropList)) ||
            (ai->attackerMoveEffect == MOVE_EFFECT_LOWER_DEFENSE_HIT && ctx->moveTbl[ai->attackerMove].secondaryEffectChance == 100)){

        debug_printf("expert flag, def drop\n");
        if(ai->maxDamageReceived > ai->attackerHP || ai->attackerTurnsOnField > 2){
            return -3;
        }

        /*Keep in mind that 2 hit KOs are +4 moveScore, so this will only
        happen if AI is seeing a 3-hit KO range or worse*/
        if(ctx->battlemon[ai->defender].states[STAT_DEFENSE] >= 6){
            moveScore += 2;
        }
        else if(ctx->battlemon[ai->defender].states[STAT_DEFENSE] >= 5){
            moveScore += 1;
        }
        else{
            moveScore -= 3;
        }
    }

    /*Sp. Def dropping status moves*/
    else if(IsInList(ai->attackerMoveEffect,SpDefDropList, NELEMS(SpDefDropList)) ||
            (ai->attackerMoveEffect == MOVE_EFFECT_LOWER_SP_DEF_HIT && ctx->moveTbl[ai->attackerMove].secondaryEffectChance == 100)){

        if(ai->maxDamageReceived > ai->attackerHP || ai->attackerTurnsOnField > 2){
            return -3;
        }

        /*Keep in mind that 2 hit KOs are +4 moveScore, so this will only
        happen if AI is seeing a 3-hit KO range or worse*/
        if(ctx->battlemon[ai->defender].states[STAT_SPDEF] >= 6){
            moveScore += 2;
        }
        else if(ctx->battlemon[ai->defender].states[STAT_SPDEF] >= 5){
            moveScore += 1;
        }
        else{
            moveScore -= 3;
        }
    }
    else if(ai->attackerMoveEffect == MOVE_EFFECT_TAUNT){
        if(BattlerHasMoveSplit(bsys, ai->defender, SPLIT_STATUS, ai) && ai->maxDamageReceived < ai->attackerHP / 2){ //taunt if we cant be 2 shot
            moveScore += 3;
        }
    }
    /*IRIDIUM: Iron Defense, Cotton Guard*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_DEF_UP_2 || ai->attackerMoveEffect == MOVE_EFFECT_DEF_UP_3){
        if(ctx->battlemon[ai->defender].spatk < ctx->battlemon[ai->defender].attack){
            if(ctx->battlemon[ai->attacker].states[STAT_DEFENSE] <= 6){
                moveScore += 3;
            }
            else if(ctx->battlemon[ai->attacker].states[STAT_DEFENSE] >= 8){
                moveScore -= 3;
            }
            else{
                moveScore += 1;
            }
        }
        else{
            moveScore -= 3;
        }
    }
    /*IRIDIUM: Amnesia*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SP_DEF_UP_2){
        if(ctx->battlemon[ai->defender].spatk < ctx->battlemon[ai->defender].attack){
            if(ctx->battlemon[ai->attacker].states[STAT_SPDEF] <= 6){
                moveScore += 3;
            }
            else if(ctx->battlemon[ai->attacker].states[STAT_SPDEF] >= 8){
                moveScore -= 3;
            }
            else{
                moveScore += 1;
            }
        }
        else{
            moveScore -= 3;
        }
    }
    
    /*IRIDIUM: Destiny Bond*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_KO_MON_THAT_DEFEATED_USER){
        if(ai->maxDamageReceived > ai->attackerHP && ai->attackerMovesFirst){
            if(BattleRand(bsys) % 2 < 1){
                moveScore += 6;
            }
        }
        else if(ai->maxDamageReceived > ai->attackerHP / 2 && ai->defenderMovesFirst){
            if(BattleRand(bsys) % 2 < 1){
                moveScore += 6;
            }
        }
        else{
            moveScore -= 3; //if we are not in danger, don't use destiny bond
        }
    }
    /*Ingrain, aqua ring*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_GROUND_TRAP_USER_CONTINUOUS_HEAL  ||
            ai->attackerMoveEffect == MOVE_EFFECT_RESTORE_HP_EVERY_TURN){
            if(ai->maxDamageReceived < ai->attackerHP / 2){ //opponent needs 3 hits to kill us
                moveScore += 2;
            }
    }
    /*IRIDIUM: Eva Up and Acc Down status moves*/
    else if((IsInList(ai->attackerMoveEffect, AccDropList, NELEMS(AccDropList)) ||
    IsInList(ai->attackerMoveEffect, EvaRaiseList, NELEMS(EvaRaiseList)))) {
        if(ctx->battlemon[ai->attacker].states[STAT_EVASION] <= 6 || ctx->battlemon[ai->defender].states[STAT_ACCURACY] >= 6){
            moveScore += 3;
        }
        else if(ctx->battlemon[ai->defender].states[STAT_EVASION] <= 8 || ctx->battlemon[ai->defender].states[STAT_ACCURACY] >= 4){
            moveScore += 1;
        }
        else{
            moveScore -= 4;
        }
    }
    /*IRIDIUM: make this extremely simple*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_INFATUATE){
        moveScore += 3;
    }

    /*Explosion, Self-destruct, Memento*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_HALVE_DEFENSE ||
        ai->attackerMoveEffect == MOVE_EFFECT_FAINT_AND_ATK_SP_ATK_DOWN_2){
            if(ai->attackerPercentHP >= 88){
                if(BattleRand(bsys) % 20 < 1){
                    moveScore += 8; //above everything except for slow kills and above
                }
                else{
                    moveScore -= 4;
                }
            }
            else if(ai->attackerPercentHP >= 76){
                if(BattleRand(bsys) % 20 < 2){
                    moveScore += 8; //above everything except for slow kills and above
                }
                else{
                    moveScore -= 4;
                }
            }
            else if(ai->attackerPercentHP >= 64){
                if(BattleRand(bsys) % 20 < 4){
                    moveScore += 8; //above everything except for slow kills and above
                }
                else{
                    moveScore -= 4;
                }
            }
            else if(ai->attackerPercentHP >= 52){
                if(BattleRand(bsys) % 20 < 6){
                    moveScore += 8; //above everything except for slow kills and above
                }
                else{
                    moveScore -= 4;
                }
            }
            else if(ai->attackerPercentHP >= 40){
                if(BattleRand(bsys) % 20 < 10){
                    moveScore += 8; //above everything except for slow kills and above
                }
                else{
                    moveScore -= 4;
                }
            }
            else if(ai->attackerPercentHP >= 28){
                if(BattleRand(bsys) % 20 < 13){
                    moveScore += 8; //above everything except for slow kills and above
                }
                else{
                    moveScore -= 4;
                }
            }
            else if(ai->attackerPercentHP >= 16){
                if(BattleRand(bsys) % 20 < 17){
                    moveScore += 8; //above everything except for slow kills and above
                }
                else{
                    moveScore -= 4;
                }
            }
            else{
                if(BattleRand(bsys) % 20 < 19){
                    moveScore += 8; //above everything except for slow kills and above
                }
                else{
                    moveScore -= 4;
                }
            }

    }
    /*Healing Wish, Lunar Dance*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_FAINT_FULL_RESTORE_NEXT_MON){

    }

    /*Mirror Move*/
    else if((ai->attackerMoveEffect == MOVE_EFFECT_COPY_MOVE || ai->attackerMoveEffect == MOVE_EFFECT_USE_LAST_USED_MOVE) && ai->attackerMovesFirst){
        if(IsInMirrorMoveList(ai->attackerMoveEffect)){
                if(BattleRand(bsys) % 2 < 1){
                    moveScore += 2;
                }
            }
        /*Copycat specific instructions*/
        else if(ai->attackerMoveEffect == MOVE_EFFECT_USE_LAST_USED_MOVE){
            if(ai->defenderMovesFirst){
                if(BattleRand(bsys) % 10 < 7){
                    moveScore -= 1;
                }
            }
        }
        else{
            if(BattleRand(bsys) % 3 < 1){
                moveScore -= 1;
            }
        }
    }

    /*Attract*/
    if(ai->attackerMoveEffect == MOVE_EFFECT_CRIT_UP_2){
        moveScore += 2;
    }

    /*Acupressure*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_RANDOM_STAT_UP_2){
        if(ai->attackerPercentHP <= 50){
            moveScore -= 1;
        }
        else if(ai->attackerPercentHP > 90){
            if(BattleRand(bsys) % 4 < 1){
                moveScore += 1;
            }
        }
        else{
            if(BattleRand(bsys) % 8 < 3){
                moveScore += 1;
            }
        }
    }

    /*Vital Throw*/


    /*Haze*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_RESET_STAT_CHANGES){
        if(BattlerHasStatBoostGreater(bsys, attacker, 9, ai) || //+3 stage
            BattlerHasStatBoostLesser(bsys, ai->defender, 3, ai)){ //-3 stage
                if(BattleRand(bsys) % 10 < 8){
                    moveScore -= 3;
                }
        }
        else if(BattlerHasStatBoostLesser(bsys, attacker, 3, ai) ||
                BattlerHasStatBoostGreater(bsys, ai->defender, 9, ai)){
                if(BattleRand(bsys) % 10 < 8){
                    moveScore += 3;
                }
        }
        else{
            moveScore -= 1;
        }
    }

    /*Bide*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_BIDE){
        if(ai->attackerPercentHP <= 90){
            moveScore -= 2;
        }
    }

    /*Switch forcing moves*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_FORCE_SWITCH){
        if(ai->defenderTurnsOnField > 3){
            if(BattleRand(bsys) % 4 < 3){
                moveScore += 2;
            }
        }
        else if(ai->defenderHasHazards == 1){
            if(BattleRand(bsys) % 2 < 1){
                moveScore += 2;
            }
        }
        else if(BattlerHasStatBoostGreater(bsys, ai->defender, 9, ai)){
            if(BattleRand(bsys) % 2 < 1){
                moveScore += 2;
            }
        }
        else{
            moveScore -= 3;
        }
    }

    /*Conversion*/

    /*Recovery Moves*/
    /*IRIDIUM: heavily modified this logic*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_RESTORE_HALF_HP ||
        ai->attackerMoveEffect == MOVE_EFFECT_HEAL_HALF_MORE_IN_SUN ||
        ai->attackerMoveEffect == MOVE_EFFECT_SWALLOW ||
        ai->attackerMoveEffect == MOVE_EFFECT_HEAL_HALF_REMOVE_FLYING_TYPE){
            if(ai->attackerPercentHP >= 80){
                moveScore -= 3;
            }
            if(ai->attackerMovesFirst){
                if(ai->maxDamageReceived < ai->attackerMaxHP / 2 && ai->maxDamageReceived > ai->attackerHP){ //if move would kill us but we recover more HP than we lose
                    moveScore += 3;
                }
            }
            else if(ai->defenderMovesFirst){
                if(ai->maxDamageReceived < ai->attackerMaxHP / 2 && ai->maxDamageReceived < ai->attackerHP){ //if move would NOT kill us and we recover more HP than we lose
                    moveScore += 3;
                }
            }

            if(ai->attackerMoveEffect == MOVE_EFFECT_HEAL_HALF_MORE_IN_SUN){
                if(ctx->field_condition & WEATHER_SANDSTORM_ANY ||
                    ctx->field_condition & WEATHER_RAIN_ANY ||
                    ctx->field_condition & WEATHER_HAIL_ANY ||
                    ctx->field_condition & WEATHER_SNOW_ANY){
                        moveScore -= 2;
                }
            }
    }

    /*Rest*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_RECOVER_HEALTH_AND_SLEEP){

    }

    /*Leech Seed*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_LEECH_SEED){
        if(ai->maxDamageReceived <= ai->attackerHP / 2){
            if(ai->attackerTurnsOnField == 0){
                moveScore += 3;
            }
            else{
                moveScore += 1;
            }
        }
        
    }
  
    /*Light Screen*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SET_LIGHT_SCREEN){
        if(ctx->side_condition[ai->attackerSide] & SIDE_STATUS_LIGHT_SCREEN){
            return -10;
        }
        if(ai->attackerItem == ITEM_LIGHT_CLAY){
            moveScore += 5;
        }
        if(ctx->battlemon[ai->defender].spatk  > ctx->battlemon[ai->defender].attack){
            moveScore += 3;
        }
        if(ai->attackerAbility == ABILITY_PRANKSTER){
            moveScore += 1;
        }
    }

    /*Reflect*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SET_REFLECT){
        if(ctx->side_condition[ai->attackerSide] & SIDE_STATUS_REFLECT){
            return -10;
        }
        if(ai->attackerItem == ITEM_LIGHT_CLAY){
            moveScore += 5;
        }
        if(ctx->battlemon[ai->defender].attack  > ctx->battlemon[ai->defender].spatk){
            moveScore += 3;
        }
        if(ai->attackerAbility == ABILITY_PRANKSTER){
            moveScore += 1;
        }
    }

    /*TODO: more logic here*/
    /*OH-KO moves*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_ONE_HIT_KO){
        if(BattleRand(bsys) % 4 < 1){
            moveScore += 1;
        }
    }

    /*Charge moves without invulnerability*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_CHARGE_TURN_HIGH_CRIT ||
        ai->attackerMoveEffect == MOVE_EFFECT_CHARGE_TURN_DEF_UP ||
        ai->attackerMoveEffect == MOVE_EFFECT_CHARGE_TURN_HIGH_CRIT_FLINCH ||
        ai->attackerMoveEffect == MOVE_EFFECT_CHARGE_TURN_SUN_SKIPS ||
        ai->attackerMoveEffect == MOVE_EFFECT_CHARGE_TURN_SP_ATK_UP ||
        ai->attackerMoveEffect == MOVE_EFFECT_CHARGE_TURN_SP_ATK_UP_RAIN_SKIPS){
        if(ai->attackerMoveEffectiveness[i] == TYPE_MUL_NO_EFFECT ||
            ai->attackerMoveEffectiveness[i] == TYPE_MUL_NOT_EFFECTIVE){
                moveScore -= 2;
        }
        else if((ai->attackerMoveEffect == MOVE_EFFECT_CHARGE_TURN_SUN_SKIPS &&
            ctx->field_condition & WEATHER_SUNNY_ANY) || 
            (ai->attackerMoveEffect == MOVE_EFFECT_CHARGE_TURN_SP_ATK_UP_RAIN_SKIPS &&
                ctx->field_condition & WEATHER_RAIN_ANY)){
                moveScore += 2;
        }
        else if(ai->attackerItem == ITEM_POWER_HERB){
            moveScore += 2;
        }
        if(BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_PROTECT, ai)){
            moveScore -= 2;
        }
    }

    /*Charge moves with invulnerability*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_DIVE ||
        ai->attackerMoveEffect == MOVE_EFFECT_FLY ||
        ai->attackerMoveEffect == MOVE_EFFECT_BOUNCE ||
        ai->attackerMoveEffect == MOVE_EFFECT_DIG ||
        ai->attackerMoveEffect == MOVE_EFFECT_SHADOW_FORCE){
        if(ai->attackerItem == ITEM_POWER_HERB){
            moveScore += 2;
        }
        if(BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_PROTECT, ai) &&
            ai->attackerMoveEffect != MOVE_EFFECT_SHADOW_FORCE){
            moveScore -= 1;
        }

        if(ctx->battlemon[ai->defender].condition & STATUS_BAD_POISON ||
            ctx->battlemon[ai->defender].condition2 == STATUS2_CURSE ||
            ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_LEECH_SEED_ACTIVE){
                moveScore += 1;
        }
        if(((ctx->field_condition & WEATHER_HAIL_ANY)&& (ai->attackerAbility == ABILITY_SNOW_CLOAK || ai->attackerAbility == ABILITY_OVERCOAT || ai->attackerAbility == ABILITY_MAGIC_GUARD || HasType(ctx, attacker, TYPE_ICE)))||
        ((ctx->field_condition & WEATHER_SANDSTORM_ANY) && (ai->attackerAbility == ABILITY_SAND_VEIL || ai->attackerAbility == ABILITY_SAND_RUSH || ai->defenderAbility == ABILITY_SAND_FORCE || ai->defenderAbility == ABILITY_OVERCOAT || ai->defenderAbility == ABILITY_MAGIC_GUARD ||  HasType(ctx, attacker, TYPE_GROUND) || HasType(ctx, attacker, TYPE_ROCK) ||HasType(ctx, attacker, TYPE_STEEL)))){
            if(BattleRand(bsys) % 10 < 7){
                moveScore += 1;
            }
        }
        if(ai->attackerMovesFirst && ai->defenderLastUsedMoveEffect != MOVE_EFFECT_BYPASS_ACCURACY){
            if(BattleRand(bsys) % 10 < 7){
                moveScore += 1;
            }
        }
    }  

    /*Fake Out*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_ALWAYS_FLINCH_FIRST_TURN_ONLY &&  ai->attackerTurnsOnField > 0){
        if(ai->defenderAbility != ABILITY_INNER_FOCUS &&
            (ai->defenderAbility != ABILITY_STEADFAST && !ai->trickRoomActive) &&
            ai->defenderAbility != ABILITY_SHIELD_DUST &&
            ai->defenderItem != ITEM_COVERT_CLOAK){
            moveScore += 7;
        }
    }

    /*Spit Up*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SPIT_UP &&
        ctx->battlemon[attacker].moveeffect.stockpileCount >= 2){
            if(BattleRand(bsys) % 10 < 7){
                moveScore += 2;
            }
    }

    /*Super Fang*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_HALVE_HP && ai->defenderPercentHP <= 50){
        moveScore -= 1;
    }
    
    /*Binding moves*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_BIND_HIT){
        if(ctx->battlemon[ai->defender].condition & STATUS_BAD_POISON ||
        ctx->battlemon[ai->defender].condition2 == STATUS2_CURSE ||
        ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_PERISH_SONG_ACTIVE ||
        ctx->battlemon[ai->defender].condition2 & STATUS2_ATTRACT){
            moveScore += 3;
        }
        else{
            moveScore += 2;
        }

        if(ctx->binding_turns[ai->defender] > 0){
            moveScore -= 0;
        }
        
    }


    /*High crit rate moves*/
    /*IRIDIUM: This is so incredibly stupid, and should be moved to calc damage ahead of time*/
    /*
    else if(ai->attackerMoveEffect == MOVE_EFFECT_HIGH_CRITICAL_POISON_HIT ||
        ai->attackerMoveEffect == MOVE_EFFECT_HIGH_CRITICAL ||
        ai->attackerMoveEffect == MOVE_EFFECT_SLEEP_POISON_PARALYZE_HIT ||
        ai->attackerMoveEffect == MOVE_EFFECT_HIGH_CRITICAL_RAISE_SPEED_HIT ||
        ai->attackerMoveEffect == MOVE_EFFECT_HIGH_CRITICAL_POISON_HIT){
            if(ai->attackerMoveEffectiveness == MOVE_STATUS_FLAG_SUPER_EFFECTIVE){
                if(BattleRand(bsys) % 2 < 1){
                    moveScore += 1;
                }
            }
            else if(ai->attackerMoveEffectiveness != MOVE_STATUS_FLAG_NOT_EFFECTIVE ||
                ai->attackerMoveEffectiveness != MOVE_STATUS_FLAG_NOT_VERY_EFFECTIVE){
                    if(BattleRand(bsys) % 4 < 1){
                        moveScore += 1;
                    }
                }
    }*/

    /*Recoil moves*/


    /*Speed lowering moves*/
    /*
    else if(ai->attackerMoveEffect == MOVE_EFFECT_LOWER_SPEED_HIT){
        //handle same as normal speed reducing moves
        if(ai->defenderMovesFirst && !(ai->trickRoomActive)){
            return 6;
        }
        if(ai->trickRoomActive){
            return -2;
        }
    }*/

    /*Recharge moves*/


    /*Disable*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_DISABLE){
        if(ai->defenderMovesFirst){
            moveScore += 0;
        }
        else if(ctx->moveTbl[ai->defenderLastUsedMove].split == SPLIT_STATUS){
            if(BattleRand(bsys) % 10 < 6){
                moveScore -= 1;
            }
        }
        else if(ctx->moveTbl[ai->defenderLastUsedMove].split != SPLIT_STATUS){
            moveScore += 1;
        }
    }

    /*Encore*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_DISABLE){
        if(ctx->battlemon[ai->defender].moveeffect.disabledTurns != 0){
            if(BattleRand(bsys) % 10 < 9){
                moveScore += 3;
            }
        }
        if(ai->defenderMovesFirst){
            moveScore -= 2;
        }
        /*TODO: there's more unknown logic in the ai doc*/
    }


    /*Counter, Mirror Coat*/
    /*TODO*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_COUNTER ||
        ai->attackerMoveEffect == MOVE_EFFECT_MIRROR_COAT){
        
    }

    /*Metal Burst*/
    /*TODO: this logic can definitely be improved*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_METAL_BURST){

    }

    /*Pain Split*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_AVERAGE_HP){
        if(ai->defenderPercentHP < 80){
            moveScore -= 1;
        }
        else if(ai->defenderMovesFirst){
            if(ai->attackerPercentHP > 60){
                moveScore -= 1;
            }
            else{
                moveScore += 1;
            }
        }
        else if(ai->attackerPercentHP > 40){
            moveScore -= 1;
        }
        else{
            moveScore -= 1;
        }
    }

    /*Nightmare*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_NIGHTMARE){
        moveScore += 2;
    }

    /*Lock On & Mind Reader*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_NEXT_ATTACK_ALWAYS_HITS){

    }

    /*Sleep Talk*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_USE_RANDOM_LEARNED_MOVE_SLEEP){
        if(ctx->battlemon[attacker].condition & STATUS_SLEEP){
            moveScore += 10;
        }
        else{
            moveScore -= 5;
        }
    }

    /*Reversal & Flail*/


    /*Heal Bell & Aromatherapy*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_CURE_PARTY_STATUS){
        if(ai->statusedMonsAttacker == 0){
            moveScore -= 5;
        }
    }

    /*Thief & Covet*/
    /*TODO: need list of encouraged effects*/

    /*Curse*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_CURSE){

    }

    /*Protect / Detect */
    else if(ai->attackerMoveEffect == MOVE_EFFECT_PROTECT){
        debug_printf("protect success turns: %d\n",ctx->protectSuccessTurns[ai->attacker])
        if(BattleTypeGet(bsys) & (BATTLE_TYPE_MULTI | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TAG)){ //Protect for doubles is handled in TagStrategy Flag
            moveScore += 0;
        }
        else{
            if(ctx->protectSuccessTurns[ai->attacker] > 0){// don't go for double protect
                moveScore -= 4;
            }
            else if(ai->defenderMovesFirst && !(ai->trickRoomActive) && ai->attackerAbility == ABILITY_SPEED_BOOST){ //gain speed advantage
                moveScore += 6;
            }
            else if(ai->attackerAbility == ABILITY_GUTS && (ai->attackerItem == ITEM_FLAME_ORB || ai->attackerItem == ITEM_TOXIC_ORB)){//same as a setup move -> OHKO score
                moveScore += 5;
            }
            else if(ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_LEECH_SEED_ACTIVE){ //slow play recovery
                moveScore += 3;
            }
            else if(ctx->binding_turns[ai->defender] > 0){ //free bind damage
                moveScore += 3;
            }
            else if(ctx->battlemon[ai->attacker].effect_of_moves & MOVE_EFFECT_FLAG_INGRAIN ||
                    ctx->battlemon[ai->attacker].effect_of_moves & MOVE_EFFECT_FLAG_AQUA_RING){
                moveScore += 3; //free recovery
            }
            else if(ctx->battlemon[ai->defender].condition & STATUS_POISON_ALL || ctx->battlemon[ai->defender].condition & STATUS_BURN){
                moveScore += 2;
            }
        }
    }



    /*Foresight & Odor Sleuth*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_IGNORE_EVASION_REMOVE_GHOST_IMMUNE){

    }

    /*Miracle Eye*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_IGNORE_EVATION_REMOVE_DARK_IMMUNE){

    }

    /*Endure
    IRIDIUM: This sucks lmao, changing it to be defender-aware*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SURVIVE_WITH_1_HP){
        debug_printf("In endure\n")
        if(ctx->protectSuccessTurns[ai->attacker] >= 1 || ai->attackerLastUsedMove == MOVE_ENDURE){
            moveScore -= 5;
        }
        else if((ai->maxDamageReceived > ai->attackerHP)){
            moveScore += 7;
        }
        else{
            moveScore -= 4;
        }
    }



    /*Substitute*/
    /*This logic seems to create a discrete distribution 
    of probability depending on the user's HP*/
    /*IRIDIUM: Completely re-wrote this*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SET_SUBSTITUTE){
        if(ai->attackerPercentHP > 50){ //only set up sub if we have more than 50% hp
            if(ai->maxDamageReceived < ai->attackerHP / 4){ //If opponent does less damage than our sub's hp
                moveScore += 8;
            }
            else if(ai->maxDamageReceived < ai->attackerHP / 3){ //If we are not threatened, set up sub.
                moveScore += 5;
            }
            else if(ai->maxDamageReceived < ai->attackerHP / 2){ //Opponent requires 3 hits to kill
                moveScore += 3;
            }
            else{
                moveScore -= 3;
            }
        }
        else{
            moveScore -= 3;
        }

    }

    /*Baton Pass*/
    /*This also has its own flag*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_PASS_STATS_AND_STATUS){
        if(BattlerHasStatBoostGreater(bsys, attacker, 9, ai)){//+3
            if(ai->defenderMovesFirst && ai->attackerPercentHP <= 70){
                if(BattleRand(bsys) % 10 < 7){
                    moveScore += 2;
                } 
            }
            else if(ai->attackerMovesFirst && ai->attackerPercentHP <= 60){
                if(BattleRand(bsys) % 10 < 7){
                    moveScore += 2;
                } 
            }
        }
        else if(BattlerHasStatBoostLesser(bsys, attacker, 8, ai) ){
            if(ai->defenderMovesFirst && ai->attackerPercentHP <= 70){
                moveScore -= 2;
            }
            else if(ai->attackerMovesFirst && ai->attackerPercentHP <= 60){
                moveScore -= 2;
            }
        }
        else{
            moveScore -= 2;
        }
    }

    /*Pursuit*/


    /*Rain Dance*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_RAIN){
        if(ai->attackerItem == ITEM_DAMP_ROCK){
            moveScore += 5;
        }
        if(ai->defenderMovesFirst && ai->attackerAbility == ABILITY_SWIFT_SWIM && !ai->trickRoomActive){
            moveScore += 4;
        }
        /*
        else if(ai->attackerAbility == ABILITY_RAIN_DISH ||
                (ai->attackerAbility == ABILITY_HYDRATION && !(ctx->battlemon[attacker].condition & STATUS_NONE))){
                    moveScore += 1;
            }
            */
        else{
            moveScore += 3;
        }
    }

    /*Sandstorm*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_SANDSTORM){
        if(ai->attackerItem == ITEM_SMOOTH_ROCK){
            moveScore += 5;
        }
        if(ai->defenderMovesFirst && ai->attackerAbility == ABILITY_SAND_RUSH && !ai->trickRoomActive){
            moveScore += 4;
        }
        else if(ai->attackerAbility == ABILITY_SAND_VEIL){
            moveScore += 3;
        }
        else{
            moveScore += 2;
        }
    }
    
    /*Sunny Day*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_SUN){
        if(ai->attackerItem == ITEM_HEAT_ROCK){
            moveScore += 5;
        }
        if(ai->defenderMovesFirst && ai->attackerAbility == ABILITY_CHLOROPHYLL && !ai->trickRoomActive){
            moveScore += 4;
        }
        else if(ai->attackerAbility == ABILITY_SOLAR_POWER ||
                ai->attackerAbility == ABILITY_HARVEST){
                    moveScore += 3;
            }
        else{
            moveScore += 2;
        }
    }


    /*Hail & Snow*/  
    else if(ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_SNOW ||
        ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_HAIL){
        if(ai->defenderMovesFirst && ai->attackerAbility == ABILITY_SLUSH_RUSH && !ai->trickRoomActive){
            moveScore += 1;
        }
        else{
            if(ai->attackerPercentHP < 40){
                moveScore -= 1;
            }
            else if(BattlerHasMoveEffect(bsys, attacker, MOVE_EFFECT_BLIZZARD, ai)){
                moveScore += 2;
            }
            else if(ctx->field_condition & WEATHER_RAIN_ANY ||
                ctx->field_condition & WEATHER_SANDSTORM_ANY ||
                ctx->field_condition & WEATHER_SUNNY_ANY){
                    moveScore += 1;
            }
            
            else if(ai->attackerAbility == ABILITY_ICE_BODY){
                    moveScore += 2;
            }
        }
    }

    /*Gravity*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_GRAVITY){
        if(ai->defenderAbility == ABILITY_LEVITATE ||
            ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_MAGNET_RISE || 
            HasType(ctx, ai->defender, TYPE_FLYING)){
            if(BattleRand(bsys) % 4 < 3){
                moveScore += 1;
            }
        }
        else if(ai->attackerPercentHP >= 60){
            if(BattleRand(bsys) % 10 < 4){
                moveScore += 1;
            }
        }
    }

    /*Tailwind*/
    /*Modified from vanilla for IRIDIUM.
    Should also get a section in TagStrategy Flag*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_DOUBLE_SPEED_3_TURNS){
        if(BattleTypeGet(bsys) & (BATTLE_TYPE_MULTI | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TAG)){ //Tailwind for doubles is handled in TagStrategy Flag
            moveScore += 0;
        }
        else if(ai->attackerMovesFirst){
            moveScore -= 3;
        }
        else if(!(ai->trickRoomActive) && ai->maxDamageReceived < ai->attackerHP){ //not trick room and survive opponent's attack
            if(ai->defenderMovesFirst){
                moveScore += 6;
            }
        }
    }

    /*Belly Drum*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_MAX_ATK_LOSE_HALF_MAX_HP){
        if(ai->attackerPercentHP < 90){
            moveScore -= 2;
        }
    }

    /*Psych Up*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_COPY_STAT_CHANGES){
        if(BattlerHasStatBoostGreater(bsys, ai->defender, 9, ai)){//greater than +3
            if(ctx->battlemon[attacker].states[STAT_EVASION] <= 6){//+0 or lower
                moveScore += 2;
            }
            else if(ctx->battlemon[attacker].states[STAT_ATTACK] <= 6 ||
                ctx->battlemon[attacker].states[STAT_DEFENSE] <= 6 ||
                ctx->battlemon[attacker].states[STAT_SPATK] <= 6 ||
                ctx->battlemon[attacker].states[STAT_SPDEF] <= 6){
                    moveScore += 1;
                }
            else{
                if(BattleRand(bsys) % 10 < 8){
                    moveScore -= 2;
                }
            }
        }
        else{
            moveScore -= 2;
        }
    }

    /*Facade*/


    /*Focus Punch*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_HIT_LAST_WHIFF_IF_HIT){
        if(ai->attackerMoveEffectiveness[i] == TYPE_MUL_NO_EFFECT ||
            ai->attackerMoveEffectiveness[i] == TYPE_MUL_NOT_EFFECTIVE){
                moveScore -= 1;
        }
        else if(ctx->battlemon[attacker].condition2 & STATUS2_SUBSTITUTE){
            moveScore += 5;
        }
        else if(ctx->battlemon[ai->defender].condition & STATUS_SLEEP){
            moveScore += 1;
        }
        else if(ctx->battlemon[ai->defender].condition2 & STATUS2_CONFUSION ||
            ctx->battlemon[ai->defender].condition2 & STATUS2_ATTRACT){
            if(BattleRand(bsys) % 10 < 6){
                moveScore += 1;
            }
        }
        else if(ai->attackerTurnsOnField != 0){
            if(BattleRand(bsys) % 5 < 1){
                moveScore += 1;
            }
        }
    }

    /*Smelling Salt*/

    
    /*Wake-Up Slap*/


    /*Trick & Switcheroo*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SWITCH_HELD_ITEMS){
        if(ItemIsDisruptive(ai->attackerItem, ai)){
            if(ItemIsBadToTrade(ai->defenderItem, ai) ){
                moveScore -= 3;
            }
            else {
                moveScore += 5;
            }
        }
        else if(ai->attackerItem == ITEM_TOXIC_ORB){
            if(ItemIsBadToTrade(ai->defenderItem, ai) ){
                moveScore -= 3;
            }
            else{
                if(!(ctx->battlemon[ai->defender].condition & STATUS_NONE) ||
                ctx->side_condition[ai->defenderSide] & SIDE_STATUS_SAFEGUARD ||
                ai->defenderImmuneToPoison || ai->defenderAbility == ABILITY_KLUTZ){
                    moveScore -= 3;
                }
                else{
                    moveScore += 5;
                }
            }
        }
        else if(ai->attackerItem == ITEM_FLAME_ORB){
            if(ItemIsBadToTrade(ai->defenderItem, ai) ){
                moveScore -= 3;
            }
            else{
                if(!(ctx->battlemon[ai->defender].condition & STATUS_NONE) ||
                ctx->side_condition[ai->defenderSide] & SIDE_STATUS_SAFEGUARD ||
                ai->defenderImmuneToBurn || ai->defenderAbility == ABILITY_KLUTZ){
                    moveScore -= 3;
                }
                else{
                    moveScore += 5;
                }
            }
        }
        else if(ai->attackerItem == ITEM_BLACK_SLUDGE){
            if(ItemIsBadToTrade(ai->defenderItem, ai) ){
                moveScore -= 3;
            }
            else{
                if(HasType(ctx, ai->defender, TYPE_POISON) || 
                    ai->defenderAbility == ABILITY_MAGIC_GUARD || ai->defenderAbility == ABILITY_KLUTZ){
                    moveScore -= 3;
                }
                else{
                    moveScore += 5;
                }
            }
        }
        //TODO: add a check for flavor berries?
    }

    /*Skill Swap & Role Play*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_COPY_ABILITY ||
        ai->attackerMoveEffect == MOVE_EFFECT_SWITCH_ABILITIES){
            if(IsDesirableAbility(attacker)){
                moveScore -= 1;
            }
            else if(IsDesirableAbility(ai->defender)){
                if(BattleRand(bsys) % 10 < 8){
                    moveScore += 2;
                }
            }
    }

    /*Eruption & Water Spout (moves that decrease power with % hp)*/


    /*Superpower*/


    /*Magic Coat*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_MAGIC_COAT){
        if(ai->defenderPercentHP <= 30){
            if(BattleRand(bsys) % 10 < 6){
                moveScore -= 1;
            }
        }
        if(ai->attackerTurnsOnField == 0){
            if(BattleRand(bsys) % 10 < 4){
                moveScore += 1;
            }
        }
        else{
            if(BattleRand(bsys) % 10 < 9){
                moveScore -= 1;
            }
        }
    }

    /*Recycle*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_RECYCLE){
        if(ctx->recycle_item[attacker] != ITEM_CHESTO_BERRY && 
            ctx->recycle_item[attacker] != ITEM_CHESTO_BERRY && 
            ctx->recycle_item[attacker] != ITEM_STARF_BERRY){
            moveScore -= 2;
        }
        else{
            if(BattleRand(bsys) % 10 < 8){
                moveScore -= 1;
            }
        }
    }
    
    /*Avalanche & Revenge*/


    /*Brick Break*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_REMOVE_SCREENS){
        if(ctx->side_condition[ai->defenderSide] & SIDE_STATUS_REFLECT ||
            ctx->side_condition[ai->defenderSide] & SIDE_STATUS_LIGHT_SCREEN){
            moveScore += 1;
        }
    }

    /*Knock Off*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_REMOVE_HELD_ITEM){

    }

    /*Endeavor*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SET_HP_EQUAL_TO_USER){
        if(ai->attackerPercentHP < 25){
            moveScore += 6;
        }
    }

    /*Imprison*/
    /*TODO: improve this vanilla logic to just check if the defender shares a move with us*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_MAKE_SHARED_MOVES_UNUSEABLE){

    }

    /*Refresh*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_HEAL_STATUS){

    }

    /*Snatch*/


    /*Mud Sport and Water Sport*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_HALVE_FIRE_DAMAGE){
        if(ai->attackerPercentHP < 50){
            moveScore -= 1;
        }
        else if(HasType(ctx, ai->defender, TYPE_FIRE)){
            moveScore += 1;
        }
    }

    else if(ai->attackerMoveEffect == MOVE_EFFECT_HALVE_ELECTRIC_DAMAGE){
        if(ai->attackerPercentHP < 50){
            moveScore -= 1;
        }
        else if(HasType(ctx, ai->defender, TYPE_ELECTRIC)){
            moveScore += 1;
        }
    }

    /*-2 Sp.Atk Moves: Overheat, Draco Meteor, Leaf Storm etc.*/

    /*Hammer Arm*/
    /*TODO: incentivize in trick room*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_USER_SPEED_DOWN_HIT){
        if(ai->attackerMoveEffectiveness[i] == TYPE_MUL_NO_EFFECT ||
            ai->attackerMoveEffectiveness[i] == TYPE_MUL_NOT_EFFECTIVE){
            moveScore -= 1;
        }
        else if(ai->defenderMovesFirst){
            moveScore += 1;
        }
    }

    /*Brine*/

    /*Feint*/

    /*Pluck and Bug Bite*/

    /*U-turn & Volt Switch*/
    /*TODO: change this logic*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SWITCH_HIT){
        if(ai->livingMembersAttacker > 1 && ai->attackerPercentHP < 33){

            if(ai->attackerMaxDamageOutputMinRoll < ai->defenderHP / 2){ //Get out if we cant 2hko
                if(ai->defenderMovesFirst && ai->maxDamageReceived < ai->attackerHP){
                    moveScore += 5;
                }
            }
            else if(ai->maxDamageReceived > ai->attackerHP && ai->attackerMovesFirst){
                moveScore += 5;
            }
        }
    }


    /*Payback*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_DOUBLE_POWER_IF_HIT){

    }

    /*Assurance*/
    /*TODO: check for doubles*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_DOUBLE_POWER_IF_TARGET_HIT){

    }

    /*Embargo*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_PREVENT_ITEM_USE){
        if(BattleRand(bsys) % 2 < 1){
            moveScore += 1;
        }
    }

    /*Fling*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_FLING){
        if(ai->attackerMoveEffectiveness[i] == TYPE_MUL_NOT_EFFECTIVE ||
            ai->attackerMoveEffectiveness[i] == TYPE_MUL_NO_EFFECT ||
        !(ai->attackerItem == ITEM_KINGS_ROCK || ai->attackerItem == ITEM_RAZOR_FANG ||
          ai->attackerItem == ITEM_POISON_BARB || ai->attackerItem == ITEM_TOXIC_ORB ||
          ai->attackerItem == ITEM_FLAME_ORB || ai->attackerItem == ITEM_LIGHT_BALL)){
            moveScore -= 1;
        }
        else if(ai->flingPower < 30){
            moveScore -= 2;
        }
        else if(ai->flingPower > 90){
            if(BattleRand(bsys) % 4 < 3){
                moveScore += 1;
            }
            if(ai->attackerMoveEffectiveness[i] >= TYPE_MUL_SUPER_EFFECTIVE){
                moveScore += 4;
            }
            else{
                if(BattleRand(bsys) % 2 < 1){
                    moveScore += 1;
                }
            }
        }
        else if(ai->flingPower > 60){
            if(BattleRand(bsys) % 4 < 3){
                moveScore += 1;
            }
        }
        else{
            if(BattleRand(bsys) % 2 < 1){
                moveScore += 1;
            }
        }
    }

    /*Psycho Shift*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_TRANSFER_STATUS){
        if(ctx->battlemon[attacker].condition & STATUS_NONE){
            moveScore -= 15;
        }
        else if(ai->defenderPercentHP >= 30){
            if(BattleRand(bsys) % 2 < 1){
                moveScore += 1;
            }
        }
    }

    /*Trump Card*/
    /*needs to be in calc damage?*/

    /*Heal Block*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_PREVENT_HEALING){
        if(BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_RECOVER_DAMAGE_SLEEP, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_RESTORE_HALF_HP, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_HEAL_HALF_REMOVE_FLYING_TYPE, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_HEAL_HALF_MORE_IN_SUN, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_RECOVER_HEALTH_AND_SLEEP, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_SWALLOW, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_RECOVER_HALF_DAMAGE_DEALT, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_GROUND_TRAP_USER_CONTINUOUS_HEAL, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_RESTORE_HP_EVERY_TURN, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_STATUS_LEECH_SEED, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_FAINT_FULL_RESTORE_NEXT_MON, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_RECOVER_THREE_QUARTERS_DAMAGE_DEALT, ai)){
            if(BattleRand(bsys) % 10 < 9){
                moveScore += 1;
            }
        }
        else if(ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_LEECH_SEED_ACTIVE ||
            ctx->battlemon[ai->defender].effect_of_moves & MOVE_EFFECT_FLAG_INGRAIN ||
            ctx->battlemon[attacker].effect_of_moves & MOVE_EFFECT_FLAG_AQUA_RING){
            if(BattleRand(bsys) % 10 < 9){
                moveScore += 1;
            }
        }
        else{
            if(BattleRand(bsys) % 2 < 1){
                moveScore += 1;
            }
        }
    }

    /*Wring Out & Crush Grip*/
    /*are these handled in damage calc?*/

    /*Power trick*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SWAP_ATK_DEF){
        if(ai->attackerPercentHP > 90){
            if(BattleRand(bsys) % 10 < 6){
                moveScore += 1;
            }
        }
        else if(ai->attackerPercentHP > 60){
            if(BattleRand(bsys) % 2 < 1){
                moveScore += 1;
            }
        }
        else if(ai->attackerPercentHP > 30){
            if(BattleRand(bsys) % 3 < 1){
                moveScore += 1;
            }
        }
    }


    /*Lucky Chant*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_PREVENT_CRITS){
        if(ai->attackerPercentHP < 70){
            moveScore -= 1;
        }
        else if(BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_HIGH_CRITICAL_POISON_HIT, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_HIGH_CRITICAL, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_SLEEP_POISON_PARALYZE_HIT, ai) ||
        BattlerHasMoveEffect(bsys, ai->defender, MOVE_EFFECT_HIGH_CRITICAL_RAISE_SPEED_HIT, ai)){
            moveScore += 1;
        }
        else{
            if(BattleRand(bsys) % 4 < 1){
                moveScore += 1;
            }
        }
    }

   
    /*Sucker Punch*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_HIT_FIRST_IF_TARGET_ATTACKING){
        /*Not sure if this needs a special AI decision*/
    }

    /*Toxic Spikes*/
    /*Spikes*/
    /*Stealth Rock*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_SET_SPIKES || ai->attackerMoveEffect == MOVE_EFFECT_TOXIC_SPIKES || ai->attackerMoveEffect == MOVE_EFFECT_STEALTH_ROCK || ai->attackerMoveEffect == MOVE_EFFECT_STICKY_WEB){
        if(ai->maxDamageReceived > ai->defenderHP){
            return -3; //Don't set toxic spikes if we are going to die
        }
        if(ai->attackerTurnsOnField == 0){
            moveScore += 3;
        }
        else if(ai->attackerTurnsOnField == 1){
            moveScore += 1;
        }
        else if(ai->attackerTurnsOnField == 2){
            moveScore += 0;
        }
        else{
            moveScore -= 5;
        }
    }

    /*Magnet Rise*/
    //IRIDIUM/GARBAGE GOLD: make this include more ground moves, or just check for the type
    else if(ai->attackerMoveEffect == MOVE_EFFECT_GIVE_GROUND_IMMUNITY){

        if(BattlerHasTypeDamagingMove(bsys, ai->defender, TYPE_GROUND, ai)){
            moveScore += 5;
            debug_printf("Has Ground Move\n");
        }
        else{
            moveScore -= 15;
            debug_printf("No Ground Move\n");
        }
        
    }

    /*Trick Room*/
    else if(ai->attackerMoveEffect == MOVE_EFFECT_TRICK_ROOM){
        if(BattleTypeGet(bsys) & (BATTLE_TYPE_MULTI | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TAG)){ //Trick Room for doubles is handled in TagStrategy Flag
            moveScore += 0;
        }
        else{
            if(ai->defenderMovesFirst){
                moveScore += 6;
            }
        }
    }
    debug_printf("end of expert flag\n");
    return moveScore;
}


int SetupFirstTurnFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai){
    int moveScore = 0;
    struct BattleStruct *ctx = bsys->sp;
    if(ctx->total_turn == 0){
        if(IsInList(ai->attackerMoveEffect, RaiseStatList, NELEMS(RaiseStatList))||
           IsInList(ai->attackerMoveEffect, DropStatList, NELEMS(DropStatList)) ||
           IsInList(ai->attackerMoveEffect, SetupFirstTurnList, NELEMS(SetupFirstTurnList))){
            if(BattleRand(bsys) % 10 < 7){
                moveScore += 2;
            }
        }
    }
    else{
        moveScore += 0;
    }
    return moveScore;
}


int RiskyFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai){
    int moveScore = 0;
    struct BattleStruct *ctx = bsys->sp;
    if(IsInList(ai->attackerMoveEffect, RiskyFlagList, NELEMS(RiskyFlagList))){
        if(BattleRand(bsys) % 2 < 1){
            moveScore += 2;
        }
    }
    return moveScore;
}

int PrioritizeDamageFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai){
    int moveScore = 0;
    struct BattleStruct *ctx = bsys->sp;
    if(IsInList(ai->attackerMoveEffect, PrioritizeDamageFlagList, NELEMS(PrioritizeDamageFlagList))){
        if(BattleRand(bsys) % 10 < 6){
            moveScore += 2;
        }
    }
    return moveScore;
}

int BatonPassFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai){
    int moveScore = 0;
    struct BattleStruct *ctx = bsys->sp;
    if(ai->livingMembersAttacker == 1 || ctx->moveTbl[ai->attackerMove].split != SPLIT_STATUS ){
        moveScore += 0;
    }
    else if(!BattlerHasMoveEffect(bsys, attacker, MOVE_EFFECT_PASS_STATS_AND_STATUS, ai) && BattleRand(bsys) % 10 < 3){
        moveScore += 0;
    }
    else{
        if(IsInList(ai->attackerMoveEffect, BatonPassFlagList, NELEMS(BatonPassFlagList))){
            if(ctx->total_turn == 0){
                moveScore += 5;
            }
            else if(ai->attackerPercentHP >= 60){
                moveScore += 1;
            }
            else{
                moveScore -= 15;
            }
        }
        else if(ai->attackerMoveEffect == MOVE_EFFECT_PROTECT){
            if(ctx->protectSuccessTurns[ai->attacker] > 0){
                moveScore -=2;
            }
            else{
                moveScore += 2;
            }
        }
        else if(ai->attackerMoveEffect == MOVE_EFFECT_PASS_STATS_AND_STATUS){
            if(ctx->total_turn == 0){
                moveScore -= 2;
            }
            else{
                if(ctx->battlemon[attacker].states[STAT_ATTACK] >= 9 ||
                    ctx->battlemon[attacker].states[STAT_SPATK] >= 9 ){
                    moveScore += 3;
                }
                else if(ctx->battlemon[attacker].states[STAT_ATTACK] >= 8 ||
                    ctx->battlemon[attacker].states[STAT_SPATK] >= 8 ){
                    moveScore += 2;
                }
                else if(ctx->battlemon[attacker].states[STAT_ATTACK] >= 7 ||
                    ctx->battlemon[attacker].states[STAT_SPATK] >= 7 ){
                    moveScore += 1;
                }
            }
        }
        else{
            if(BattleRand(bsys) % 10 < 9){
                moveScore += 3;
            }
        }
    }
    return moveScore;
}
int TagStrategyFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai){
    debug_printf("entering tag strategy\n");
    int moveScore = 0;
    struct BattleStruct *ctx = bsys->sp;
    ai->partner = BATTLER_ALLY(ai->attacker);
    ai->partnerPercentHP = ctx->battlemon[ai->partner].hp * 100 / ctx->battlemon[ai->partner].maxhp;
    ai->partnerHP = ctx->battlemon[ai->partner].hp;
    ai->partnerAbility = ctx->battlemon[ai->partner].ability;
    ai->partnerItem = ctx->battlemon[ai->partner].item;
    u32 effectivenessOnPartner;
    debug_printf("tag strategy\n");
    /*This section of the flag only contributes to move score if attacker has a living partner
    and they are the target of certain moves.*/
    if(ai->defender == ai->partner){
        if(ctx->battlemon[ai->partner].hp == 0){//if partner is dead, dont use anything in this flag
            moveScore -= 30;
        }
        else{
            /*Skill Swap*/
            if(ai->attackerMoveEffect == MOVE_EFFECT_SWITCH_ABILITIES){//Skill Swap
                if(ai->partnerAbility == ABILITY_TRUANT ||
                    ai->partnerAbility == ABILITY_SLOW_START){
                    moveScore += 10;
                }
                else if(ai->attackerAbility == ABILITY_LEVITATE){ //give levitate to electric type partner
                    if(HasType(ctx, ai->partner, TYPE_ELECTRIC)){
                        moveScore += 1;
                        if(ctx->battlemon[ai->partner].type1 == TYPE_ELECTRIC &&
                            ctx->battlemon[ai->partner].type2 == TYPE_ELECTRIC){
                            moveScore += 1; //Extra +1 if mono-electric
                        }
                    }
                }
                else if(ai->attackerAbility == ABILITY_COMPOUND_EYES || //give these abilities to partner with low acc moves
                        ai->attackerAbility == ABILITY_NO_GUARD){
                            if(BattlerKnowsMoveInList(bsys, ai->partner, TagStrategyFlagList_SkillSwap, NELEMS(TagStrategyFlagList_SkillSwap), ai)){
                                moveScore += 3;
                            }
                        }
                else{
                    moveScore -= 30;
                }
            }
            /*Burn*/
            else if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_BURN){
                if( ai->partnerAbility == ABILITY_FLASH_FIRE){
                    if(ctx->battlemon[ai->defender].moveeffect.flashFire == 0){
                        moveScore += 3;
                    }
                    else{
                        moveScore -= 30;
                    }                    
                }
                else if(ctx->battlemon[ai->partner].ability == ABILITY_GUTS && 
                        ctx->battlemon[ai->partner].condition & STATUS_NONE && 
                        !HasType(ctx, ai->partner, TYPE_FIRE) && 
                        ai->partnerItem != ITEM_FLAME_ORB &&
                        ai->partnerItem != ITEM_TOXIC_ORB &&
                        ai->partnerPercentHP >=  81){
                            moveScore += 5;
                }
                else{
                    moveScore -= 30;
                }
            }
            /*Paralyze*/
            else if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_PARALYZE){
                if(HasType(ctx, ai->partner,TYPE_GROUND)){
                    moveScore -= 30;
                }
                else if(ai->partnerAbility == ABILITY_MOTOR_DRIVE ||
                    ai->partnerAbility == ABILITY_VOLT_ABSORB){
                    //handle like damaging electric move
                }
                else{
                    moveScore -= 30;
                }
            }
            /*Poison*/
            else if(ai->attackerMoveEffect == MOVE_EFFECT_STATUS_BADLY_POISON ||
                    ai->attackerMoveEffect == MOVE_EFFECT_STATUS_POISON){
                if(ctx->battlemon[ai->partner].ability == ABILITY_POISON_HEAL &&
                    ctx->battlemon[ai->partner].condition & STATUS_NONE &&
                    ctx->battlemon[ai->partner].item != ITEM_TOXIC_ORB &&
                    !HasType(ctx,ai->partner, TYPE_POISON) &&
                    !HasType(ctx,ai->partner, TYPE_STEEL) &&
                    ai->partnerPercentHP >=  81){
                        moveScore += 5;
                    }
                else {
                    moveScore -= 30;
                }      
            }
            /*Helping Hand*/
            else if(ai->attackerMoveEffect == MOVE_EFFECT_BOOST_ALLY_POWER_BY_50_PERCENT){
                if(ai->partnerPercentHP > 50 || BattlerMovesFirstDoubles(bsys, ctx, ai->partner, 0, ai)){
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore += 2;
                    }
                    else{
                        moveScore -= 1;
                    }
                }
                else{
                    moveScore -= 3;
                }
            }
            /*Swagger*/
            else if(ai->attackerMoveEffect == MOVE_EFFECT_ATK_UP_2_STATUS_CONFUSION){
                if(ai->partnerItem != ITEM_PERSIM_BERRY &&
                   ai->partnerItem != ITEM_LUM_BERRY ){
                    moveScore -= 30;
                }
                else{
                    if(ctx->battlemon[ai->partner].states[STAT_ATTACK] < 8){//Less than +2
                        moveScore += 3;
                    }
                    else{
                        moveScore -= 30;
                    }
                }

            }
            /*Gastro Acid*/
            else if(ai->attackerMoveEffect == MOVE_EFFECT_SUPRESS_ABILITY){
                if(ctx->battlemon[ai->partner].effect_of_moves & MOVE_EFFECT_GASTRO_ACID ){
                    moveScore -= 30;
                }
                else if(ai->partnerAbility == ABILITY_TRUANT ||
                    ai->partnerAbility == ABILITY_SLOW_START){
                        moveScore += 5;
                }
                else{
                    moveScore -= 30;
                }

            }
            /*Acupressure*/
            else if(ai->attackerMoveEffect == MOVE_EFFECT_RANDOM_STAT_UP_2){
                if(BattlerHasStatBoostGreater(bsys, ai->partner, 12, ai)){ //if any stats are at +6{
                    moveScore -= 30;
                }
                else if(ai->partnerPercentHP <= 50){
                    moveScore -= 1;
                }
                else if(ai->partnerPercentHP > 90){
                    if(BattleRand(bsys) % 10 < 7){
                        moveScore += 2;
                    }
                }
                else{
                    if(BattleRand(bsys) % 10 < 7){
                        moveScore += 2;
                    }
                }
            }
            /*Electric moves with Motor Drive and Volt Absorb*/
            else if(ctx->moveTbl[ai->attackerMove].type == TYPE_ELECTRIC && ctx->moveTbl[ai->attackerMove].power){
                if(ai->partnerAbility == ABILITY_MOTOR_DRIVE){
                    if(BattleRand(bsys) % 10 < 6){
                        moveScore += 0;
                    }
                    else{
                        if(ctx->battlemon[ai->partner].states[STAT_SPEED] == 12){
                            moveScore -= 30;
                        }
                        else{
                            moveScore += 3;
                        }
                    }
                }
                else if(ai->partnerAbility == ABILITY_VOLT_ABSORB){
                    if(ai->partnerPercentHP  == 100){
                        moveScore -= 15;
                    }
                    else if(ai->partnerPercentHP  > 90){
                        moveScore += 0;
                    }
                    else if(ai->partnerPercentHP  > 75){
                        if(BattleRand(bsys) % 4 < 1){
                            moveScore += 3;
                        }
                    }
                    else if(ai->partnerPercentHP  > 50){
                        if(BattleRand(bsys) % 2 < 1){
                            moveScore += 3;
                        }
                    }
                    else{
                        if(BattleRand(bsys) % 4 < 3){
                            moveScore += 3;
                        }
                    }
                }
                else{
                    moveScore -= 30;
                }
            }
            /*Water moves with Dry Skin and Water Absorb*/
            else if(ctx->moveTbl[ai->attackerMove].type == TYPE_WATER && ctx->moveTbl[ai->attackerMove].power){
                if(ai->partnerAbility == ABILITY_DRY_SKIN ||
                   ai->partnerAbility == ABILITY_WATER_ABSORB){
                    if(ai->partnerPercentHP  == 100){
                        moveScore -= 15;
                    }
                    else if(ai->partnerPercentHP  > 90){
                        moveScore += 0;
                    }
                    else if(ai->partnerPercentHP  > 75){
                        if(BattleRand(bsys) % 4 < 1){
                            moveScore += 3;
                        }
                    }
                    else if(ai->partnerPercentHP  > 50){
                        if(BattleRand(bsys) % 2 < 1){
                            moveScore += 3;
                        }
                    }
                    else{
                        if(BattleRand(bsys) % 4 < 3){
                            moveScore += 3;
                        }
                    }
                }
                else{
                    moveScore -= 30;
                }
            }
            /*DO NOT attack our partner otherwise!*/
            else{
                moveScore -= 30; 
            }


        }
    }
    else{

        /*In a double battle, if the move can OHKO the opponent from their current HP,
        prioritize using a spread move over a single target move, for extra damage on the partner.
        NOTE: this will encourage moves like Surf that can hit a partner, too.*/
        if(ctx->moveTbl[ai->attackerMove].split != SPLIT_STATUS &&
           ai->attackerAvgRollMoveDamages[i] >= ai->defenderHP &&
           (ctx->moveTbl[ai->attackerMove].target == RANGE_ALL_ADJACENT || ctx->moveTbl[ai->attackerMove].target == RANGE_ADJACENT_OPPONENTS)){
            moveScore += 1;
        }
        
        

        /*Protect in Double Battles functions differently than singles
        Essentially, we want to protect ourselves from an attack that would kill us,
        but only if our partner isn't in danger. We want to avoid a double protect.*/
        if(ai->attackerMoveEffect == MOVE_EFFECT_PROTECT){
            if(ai->maxDamageReceived > ai->attackerHP && ai->defenderMovesFirst && ctx->protectSuccessTurns[ai->attacker] == 0){ //if the player can fast kill us, we need to protect
                moveScore += 13;
            }
        }


        /*Skill Swap*/
        if(ai->attackerMoveEffect == MOVE_EFFECT_SWITCH_ABILITIES){
            if(ai->attackerAbility == ABILITY_TRUANT ||
                ai->attackerAbility == ABILITY_SLOW_START ||
                ai->attackerAbility == ABILITY_KLUTZ || 
                ai->attackerAbility == ABILITY_STALL){
                    moveScore += 5;
            }
            else if(ai->defenderAbility == ABILITY_SHADOW_TAG||
                ai->defenderAbility == ABILITY_PURE_POWER ||
                ai->defenderAbility == ABILITY_HUGE_POWER || 
                ai->defenderAbility == ABILITY_MOLD_BREAKER ||
                ai->defenderAbility == ABILITY_SOLID_ROCK ||
                ai->defenderAbility == ABILITY_FILTER || 
                ai->defenderAbility == ABILITY_FLOWER_GIFT){
                    moveScore += 2;
            }
        }
        /*Earthquake and Magnitude*/
        else if((ai->attackerMoveEffect == MOVE_EFFECT_RANDOM_POWER_10_CASES ||
            ai->attackerMoveEffect == MOVE_EFFECT_DOUBLE_DAMAGE_DIG) && ai->partnerHP != 0){
                AITypeCalc(ctx, MOVE_EARTHQUAKE, TYPE_GROUND, ai->attackerAbility, ai->partnerAbility, BattleItemDataGet(ctx, ai->partnerItem, 1), ctx->battlemon[ai->partner].type1, ctx->battlemon[ai->partner].type1, & effectivenessOnPartner);
                if(ai->partnerAbility == ABILITY_LEVITATE ||
                    HasType(ctx, ai->partner, TYPE_FLYING) ||
                    ctx->battlemon[ai->partner].effect_of_moves & MOVE_EFFECT_FLAG_MAGNET_RISE){
                        moveScore += 2;
                }
                else if(effectivenessOnPartner == MOVE_STATUS_FLAG_SUPER_EFFECTIVE){
                    moveScore -= 15;
                }
                else{
                    moveScore -= 3;
                }
            
        }
        /*Future Sight & Doom Desire*/
        else if(ai->attackerMoveEffect == MOVE_EFFECT_HIT_IN_3_TURNS){
            if(ai->partnerHP == 0){
                moveScore += 0;
            }
            else{
                if(BattlerHasMoveEffect(bsys, ai->partner, MOVE_EFFECT_HIT_IN_3_TURNS, ai)){
                    if(CalcSpeed(bsys, ctx, ai->attacker, ai->partner, 0) == 1){
                        moveScore -= 3;
                    }
                    else if(CalcSpeed(bsys, ctx, ai->attacker, ai->partner, 0) == 2){
                        if(BattleRand(bsys) % 2 < 1){
                            moveScore -= 3;
                        }
                    }
                }
            }
        }
        /*Weather Section:*/
        /*Rain Dance*/
        else if(ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_RAIN){
            if((ai->attackerAbility == ABILITY_HYDRATION && !(ctx->battlemon[ai->attacker].condition & STATUS_NONE)) ||
                (ai->attackerAbility == ABILITY_DRY_SKIN || ai->attackerAbility == ABILITY_RAIN_DISH ||
                 (ai->attackerAbility == ABILITY_SWIFT_SWIM && !ai->trickRoomActive))){
                    moveScore += 2;
            }
            if((ai->partnerAbility == ABILITY_HYDRATION && !(ctx->battlemon[ai->partner].condition & STATUS_NONE)) ||
            (ai->partnerAbility == ABILITY_DRY_SKIN || ai->partnerAbility == ABILITY_RAIN_DISH ||
             (ai->partnerAbility == ABILITY_SWIFT_SWIM && !ai->trickRoomActive))){
                moveScore += 2;
            }
        }
        /*Sunny Day*/
        else if(ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_SUN){
            if((ai->attackerAbility == ABILITY_LEAF_GUARD && !(ctx->battlemon[ai->attacker].condition & STATUS_NONE) &&
                ai->attackerPercentHP >= 30) || ai->attackerAbility == ABILITY_FLOWER_GIFT){
                    moveScore += 2;
            }        
            else if(ai->attackerAbility == ABILITY_DRY_SKIN){
                moveScore -= 2;
            }
            else if(ai->attackerAbility == ABILITY_SOLAR_POWER){
                if(ai->attackerPercentHP >= 50){
                    moveScore += 1;
                }
                else{
                    if(BattleRand(bsys) % 2 < 1){
                        moveScore -= 2;
                    }   
                }
            }

            if((ai->partnerAbility == ABILITY_LEAF_GUARD && !(ctx->battlemon[ai->partner].condition & STATUS_NONE) &&
                ai->partnerPercentHP >= 30) || ai->partnerAbility == ABILITY_FLOWER_GIFT){
                    moveScore += 2;
            }        
            else if(ai->partnerAbility == ABILITY_DRY_SKIN){
                moveScore -= 2;
            }
            else if(ai->partnerAbility == ABILITY_SOLAR_POWER){
                if(ai->partnerPercentHP >= 50){
                    moveScore += 1;
                }
                else{
                    if(BattleRand(bsys) % 2 < 1){
                        moveScore -= 2;
                    }   
                }
            }
        }

        /*Hail / Snow*/
        else if(ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_HAIL || ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_SNOW){
            if(ai->attackerAbility == ABILITY_ICE_BODY || 
                ai->attackerAbility == ABILITY_SNOW_CLOAK ||
                BattlerHasMoveEffect(bsys, ai->attacker, MOVE_EFFECT_BLIZZARD, ai)){
                    moveScore += 2;
            }
            if(ai->partnerAbility == ABILITY_ICE_BODY || 
                ai->partnerAbility == ABILITY_SNOW_CLOAK ||
                BattlerHasMoveEffect(bsys, ai->partner, MOVE_EFFECT_BLIZZARD, ai)){
                    moveScore += 2;
            }
        }

        /*Sandstorm*/
        else if(ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_SANDSTORM){
            if(ai->attackerAbility == ABILITY_SAND_VEIL || HasType(ctx, ai->attacker, TYPE_ROCK) || 
                HasType(ctx, ai->attacker, TYPE_GROUND) || HasType(ctx, ai->attacker, TYPE_STEEL)){
                    moveScore += 2;
            }
            if(ai->partnerAbility == ABILITY_SAND_VEIL || HasType(ctx, ai->partner, TYPE_ROCK) || 
            HasType(ctx, ai->partner, TYPE_GROUND) || HasType(ctx, ai->partner, TYPE_STEEL)){
                    moveScore += 2;
            }
        }

        /*Gravity*/
        else if(ai->attackerMoveEffect == MOVE_EFFECT_GRAVITY){
            if(ctx->field_condition & FIELD_STATUS_GRAVITY){
                moveScore -= 30;
            }
            else{
                for (int j = 0; j < 4; j++){
                    if(ctx->battlemon[j].hp != 0){
                        if(BATTLER_IS_ENEMY(j) == ai->attackerSide){
                            if(ctx->battlemon[j].ability == ABILITY_LEVITATE || 
                                ctx->battlemon[j].effect_of_moves & MOVE_EFFECT_FLAG_MAGNET_RISE ||
                                HasType(ctx, j, TYPE_FLYING)){
                                    moveScore -= 5;
                            }
                        }
                        else{
                            if(ctx->battlemon[j].ability == ABILITY_LEVITATE || 
                                ctx->battlemon[j].effect_of_moves & MOVE_EFFECT_FLAG_MAGNET_RISE){
                                if(BattleRand(bsys) % 4 < 3){
                                    moveScore += 3;
                                }         
                            }
                        }
                    }
                }
            }
        }

        /*Trick Room*/
        else if(ai->attackerMoveEffect == MOVE_EFFECT_TRICK_ROOM){
            if(ai->livingMembersAttacker == 1 || ai-> livingMembersDefender == 1){
                moveScore -= 30;
            }
            else if(ctx->turnOrder[ai->attacker] == 0){//if attacker moves first
                if(ctx->turnOrder[ai->partner] == 0 || ctx->turnOrder[ai->partner] == 1){ //partner moves first or second
                    moveScore -= 30;
                }
                else{
                    moveScore -= 5;
                }
            }
            else if(ctx->turnOrder[ai->attacker] == 1){//if attacker moves second
                if(ctx->turnOrder[ai->partner] == 0 ){ //partner moves first
                    moveScore -= 30;
                }
                else{
                    moveScore -= 5;
                }
            }
            else if(ctx->turnOrder[ai->attacker] == 2){//if attacker moves third
                if(ctx->turnOrder[ai->partner] != 3 ){ //partner doesn't move last
                    moveScore -= 5;
                }
                else{
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore += 5;
                    }
                    else{
                        moveScore -= 5;
                    }
                }
            }
            else if(ctx->turnOrder[ai->attacker] == 3){//if attacker moves last
                if(ctx->turnOrder[ai->partner] != 2 ){ //partner doesn't move third
                    moveScore -= 5;
                }
                else{
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore += 5;
                    }
                    else{
                        moveScore -= 5;
                    }
                }
            }
        }

        /*Follow Me*/
        if(ai->attackerMoveEffect == MOVE_EFFECT_MAKE_GLOBAL_TARGET ){
            if(ai->partnerHP == 0){
                return -10;
            }
            if(ai->attackerPercentHP > 90){
                if(ai->partnerPercentHP > 90){
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore -= 1;
                    }
                }
                else if(ai->partnerPercentHP > 50){
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore += 1;
                    }
                }
                else if(ai->partnerPercentHP > 30){
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore += 2;
                    }
                }
                else{
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore += 3;
                    }
                }
            }
            else if(ai->attackerPercentHP > 50){
                if(ai->partnerPercentHP > 90){
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore -= 2;
                    }
                }
                else if(ai->partnerPercentHP > 50){
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore -= 1;
                    }
                }
                else if(ai->partnerPercentHP > 30){
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore += 1;
                    }
                }
                else{
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore += 2;
                    }
                }
            }
            else if(ai->attackerPercentHP > 30){
                if(ai->partnerPercentHP > 90){
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore -= 2;
                    }
                }
                else if(ai->partnerPercentHP > 50){
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore -= 2;
                    }
                }
                else if(ai->partnerPercentHP > 30){
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore += 1;
                    }
                }
                else{
                    if(BattleRand(bsys) % 4 < 3){
                        moveScore += 2;
                    }
                }
            }
            else{
                if(BattleRand(bsys) % 4 < 3){
                    moveScore -= 5;
                }
            }
        }

        /*Discharge & Parabolic Thingy*/
        else if(ai->attackerMoveType == TYPE_ELECTRIC && ctx->moveTbl[ai->attackerMove].target == RANGE_ALL_ADJACENT){
            AITypeCalc(ctx, MOVE_DISCHARGE, TYPE_ELECTRIC, ai->attackerAbility, ai->partnerAbility, BattleItemDataGet(ctx, ai->partnerItem, 1), ctx->battlemon[ai->partner].type1, ctx->battlemon[ai->partner].type1, & effectivenessOnPartner);

            if(ai->partnerAbility == ABILITY_MOTOR_DRIVE ||
                ai->partnerAbility == ABILITY_VOLT_ABSORB ||
                ai->partnerAbility == ABILITY_LIGHTNING_ROD){
                    moveScore += 3;
            }
            else if(effectivenessOnPartner == MOVE_STATUS_FLAG_NOT_EFFECTIVE){
                moveScore += 3;
            }
            else if(effectivenessOnPartner == MOVE_STATUS_FLAG_SUPER_EFFECTIVE){
                moveScore -= 3;
            }
            else{
                moveScore -= 3;
            }
        }

        /*Single Target Electric Moves*/
        else if(ai->attackerMoveType == TYPE_ELECTRIC && ctx->moveTbl[ai->attackerMove].target == RANGE_SINGLE_TARGET){
            if(ctx->battlemon[ai->defender].ability == ABILITY_LIGHTNING_ROD || 
                (ctx->battlemon[BATTLER_ALLY(ai->defender)].ability == ABILITY_LIGHTNING_ROD && ctx->battlemon[BATTLER_ALLY(ai->defender)].hp != 0)){
                    moveScore -= 9;
            }
            else if(ctx->battlemon[ai->partner].ability == ABILITY_LIGHTNING_ROD){
                moveScore -= 15;
            }
        }

        /*Lava Plume */
        else if(ai->attackerMoveType == TYPE_FIRE && ctx->moveTbl[ai->attackerMove].target == RANGE_ALL_ADJACENT){
            AITypeCalc(ctx, MOVE_LAVA_PLUME, TYPE_FIRE, ai->attackerAbility, ai->partnerAbility, BattleItemDataGet(ctx, ai->partnerItem, 1), ctx->battlemon[ai->partner].type1, ctx->battlemon[ai->partner].type1, & effectivenessOnPartner);

            if(ctx->battlemon[ai->defender].moveeffect.flashFire == 1){
                moveScore += 1; //this is kinda weird to add this instead of subtracting
            }

            if(ai->partnerAbility == ABILITY_FLASH_FIRE){
                moveScore += 3;
            }
            else if(ai->partnerAbility == ABILITY_DRY_SKIN || ai->partnerAbility == ABILITY_FLUFFY){
                moveScore -= 3;
            }
            else if(effectivenessOnPartner == MOVE_STATUS_FLAG_SUPER_EFFECTIVE){
                moveScore -= 3;
            }
            else if(effectivenessOnPartner == MOVE_STATUS_FLAG_NOT_VERY_EFFECTIVE){
                moveScore += 1;
            }
            else{
                moveScore -= 3; //TODO: this might want to be excluded...
            }
        }

        /*Surf*/
        else if(ai->attackerMoveType == TYPE_FIRE && ctx->moveTbl[ai->attackerMove].target == RANGE_ALL_ADJACENT){
            AITypeCalc(ctx, MOVE_SURF, TYPE_WATER, ai->attackerAbility, ai->partnerAbility, BattleItemDataGet(ctx, ai->partnerItem, 1), ctx->battlemon[ai->partner].type1, ctx->battlemon[ai->partner].type1, & effectivenessOnPartner);
            if(ai->partnerAbility == ABILITY_DRY_SKIN || ai->partnerAbility == ABILITY_WATER_ABSORB){
                moveScore += 3;
            }
            else if(effectivenessOnPartner == MOVE_STATUS_FLAG_SUPER_EFFECTIVE){
                moveScore -= 3;
            }
            else{
                moveScore -= 3;//this might want to be excluded...
            }
        }

        /*Single Target Water moves*/
        else if(ai->attackerMoveType == TYPE_WATER && ctx->moveTbl[ai->attackerMove].target == RANGE_SINGLE_TARGET){
            if(ctx->battlemon[ai->defender].ability == ABILITY_STORM_DRAIN || 
                (ctx->battlemon[BATTLER_ALLY(ai->defender)].ability == ABILITY_STORM_DRAIN && ctx->battlemon[BATTLER_ALLY(ai->defender)].hp != 0)){
                    moveScore -= 15;
            }
            else if(ctx->battlemon[ai->partner].ability == ABILITY_STORM_DRAIN){
                moveScore -= 15;
            }
        }
        /*Encourage Damaging moves if partner has Helping Hand. We exclude flat damaging moves for obvious reasons.*/
        else if(BattlerHasMoveEffect(bsys, ai->partner, MOVE_EFFECT_BOOST_ALLY_POWER_BY_50_PERCENT, ai)){
            if(!IsInList(ai->attackerMoveEffect, MovesEffectsWithFlatDamageOrOHKO, NELEMS(MovesEffectsWithFlatDamageOrOHKO))){
                moveScore += 1;
            }
        }
    }
    debug_printf("tag strategy end\n");
    //debug_printf("TagStrategyFlag moveScore: %d\n", moveScore);
    return moveScore;
}
int CheckHPFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai){
    int moveScore = 0;
    struct BattleStruct *ctx = bsys->sp;

    //attacker hp considered first
    if(ai->attackerMoveEffect == MOVE_EFFECT_HALVE_DEFENSE){
        if(ai->attackerPercentHP >= 31){
            if(BattleRand(bsys) % 10 < 8){
                moveScore -= 2;
            }
        }
    }
    else if(IsInList(ai->attackerMoveEffect, CheckHPFlagList_1, NELEMS(CheckHPFlagList_1))){
        if(ai->attackerPercentHP >= 71){
            if(BattleRand(bsys) % 10 < 8){
                moveScore -= 2;
            }
        }
    }
    else if(IsInList(ai->attackerMoveEffect, CheckHPFlagList_2, NELEMS(CheckHPFlagList_2))){
        if(ai->attackerPercentHP < 70){
            if(BattleRand(bsys) % 10 < 8){
                moveScore -= 2;
            }
        }
    }
    else if(IsInList(ai->attackerMoveEffect, CheckHPFlagList_3, NELEMS(CheckHPFlagList_3))){
        if(ai->attackerPercentHP >= 31 && ai->attackerPercentHP <= 70){
            if(BattleRand(bsys) % 10 < 8){
                moveScore -= 2;
            }
        }
    }
    else if(IsInList(ai->attackerMoveEffect, CheckHPFlagList_4, NELEMS(CheckHPFlagList_4))){
        if(ai->attackerPercentHP < 31){
            if(BattleRand(bsys) % 10 < 8){
                moveScore -= 2;
            }
        }
    }

    //defender hp considered next
    if(ai->defenderPercentHP > 71){
        moveScore += 0;
    }
    else{
        if(IsInList(ai->attackerMoveEffect, CheckHPFlagList_5, NELEMS(CheckHPFlagList_5))){
            if(BattleRand(bsys) % 10 < 8){
                moveScore -= 2;
            }
        }
        else if(IsInList(ai->attackerMoveEffect, CheckHPFlagList_6, NELEMS(CheckHPFlagList_6))){
            if(ai->attackerPercentHP <= 30){
                if(BattleRand(bsys) % 10 < 8){
                    moveScore -= 2;
                }
            }
        }
    }


    return moveScore;
}
int WeatherFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai){
    int moveScore = 0;
    struct BattleStruct *ctx = bsys->sp;
    
    if((ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_SANDSTORM && 
        !(ctx->field_condition & WEATHER_SANDSTORM_ANY)) ||
        (ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_HAIL && 
            !(ctx->field_condition & WEATHER_HAIL_ANY )) ||
            (ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_SNOW && 
                !(ctx->field_condition & WEATHER_SNOW_ANY )) ||
            (ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_RAIN && 
                !(ctx->field_condition & WEATHER_RAIN_ANY)) ||
                (ai->attackerMoveEffect == MOVE_EFFECT_WEATHER_SUN && 
                    !(ctx->field_condition & WEATHER_SUNNY_ANY))){
                moveScore += 5;
    }
    return moveScore;
}
int HarassmentFlag(struct BattleSystem *bsys, int attacker, int i, struct AIContext *ai){
    int moveScore = 0;
    struct BattleStruct *ctx = bsys->sp;
    if(IsInList(ai->attackerMoveEffect, HarassmentFlagList, NELEMS(HarassmentFlagList)) ||
       IsInList(ai->attackerMoveEffect, DropStatList, NELEMS(DropStatList))){
        if(BattleRand(bsys) % 2 < 1){
            moveScore += 2;
        }
    }
    return moveScore;
}




/*-------------------------------Helper Functions--------------------------------*/


/*returns the index of the pokemon on the attacker's (ai's)
team with the largest damage against the target*/
int AttackerMonWithHighestDamage (struct BattleSystem *bsys, int attacker, struct AIContext *ai){
    struct BattleStruct *ctx = bsys->sp;
    int maxDamage = 0;
    int maxDamageIndex = 0;
    //loop over party pokemon that aren't fainted
    for(int i = 0; i < ai->partySizeAttacker; i++){
        struct PartyPokemon * currentMonAttacking = Battle_GetClientPartyMon(bsys, attacker, i);
        //loop over each pokemon's moves
        if(GetMonData(currentMonAttacking, MON_DATA_HP, 0) != 0){
            for (int attack_index = 0; attack_index < 4; attack_index++){
                int current_move = GetMonData(currentMonAttacking, MON_DATA_MOVE1 + attack_index, NULL);
                int damage = CalcBaseDamage(bsys, ctx, current_move, ctx->side_condition[ai->defenderSide], ctx->field_condition, 0, 0, attacker, ai->defender, 0);
                if(damage > maxDamage){
                    maxDamage = damage;
                    maxDamageIndex = i;
                }
            }
        }
    }
    return maxDamageIndex;
}

/*Returns the sum of all stat changes, only looking at boosted moves.*/
int BattlerPositiveStatChangesSum (struct BattleSystem *bsys, u32 battler, struct AIContext *ai){
    struct BattleStruct *ctx = bsys->sp;
    int statSum = 0;

    for(int stat = 0; stat < STAT_MAX; stat++){
        if(ctx->battlemon[battler].states[stat] > 6){
            statSum += ctx->battlemon[battler].states[stat];
        }
    }
    return statSum;
}

/*Returns true if the battler has >= boost_amount in any stat*/
BOOL BattlerHasStatBoostGreater (struct BattleSystem *bsys, u32 battler, u32 boost_amount, struct AIContext *ai){
    BOOL battlerHasStatBoost = 0;
    struct BattleStruct *ctx = bsys->sp;
    if(ctx->battlemon[battler].states[STAT_ATTACK] >= boost_amount ||
        ctx->battlemon[battler].states[STAT_DEFENSE] >= boost_amount ||
        ctx->battlemon[battler].states[STAT_SPATK] >= boost_amount ||
        ctx->battlemon[battler].states[STAT_SPDEF] >= boost_amount ||
        ctx->battlemon[battler].states[STAT_SPEED] >= boost_amount ||
        ctx->battlemon[battler].states[STAT_EVASION] >= boost_amount ||
        ctx->battlemon[battler].states[STAT_ACCURACY] >= boost_amount){
            battlerHasStatBoost = 1;
    }
    return battlerHasStatBoost;
}

/*Returns true if the battler has <= boost_amount in any stat*/
BOOL BattlerHasStatBoostLesser (struct BattleSystem *bsys, u32 battler, u32 drop_amount, struct AIContext *ai){
    BOOL battlerHasStatBoost = 0;
    struct BattleStruct *ctx = bsys->sp;
    if(ctx->battlemon[battler].states[STAT_ATTACK] <= drop_amount ||
        ctx->battlemon[battler].states[STAT_DEFENSE] <= drop_amount ||
        ctx->battlemon[battler].states[STAT_SPATK] <= drop_amount ||
        ctx->battlemon[battler].states[STAT_SPDEF] <= drop_amount ||
        ctx->battlemon[battler].states[STAT_SPEED] <= drop_amount ||
        ctx->battlemon[battler].states[STAT_EVASION] <= drop_amount ||
        ctx->battlemon[battler].states[STAT_ACCURACY] <= drop_amount){
            battlerHasStatBoost = 1;
    }
    return battlerHasStatBoost;
}

/*Returns true if user has a particular move in their 4 move slots.*/
BOOL BattlerKnowsMove (struct BattleSystem *bsys, u32 battler, u32 move, struct AIContext *ai){
    BOOL knowsMove = 0;
    struct BattleStruct *ctx = bsys->sp;
    for(unsigned int i = 0; i < 4; i++){
        u32 battler_move_check = ctx->battlemon[battler].move[i];
        if(battler_move_check == move){
            knowsMove = 1;
        }
    }
    return knowsMove;
}

BOOL BattlerHasTypeDamagingMove (struct BattleSystem *bsys, u32 battler, u32 type, struct AIContext *ai){
    BOOL hasMove = FALSE;
    struct BattleStruct *ctx = bsys->sp;
    for(int i = 0; i < 4; i++){
        int battler_move_check = ctx->battlemon[battler].move[i];
        if(ctx->moveTbl[battler_move_check].type == type && ctx->moveTbl[battler_move_check].split != SPLIT_STATUS){
            hasMove = TRUE;
        }
    }
    return hasMove;
}

/*Returns true if user has a particular move split (physical, special, or status) in their 4 move slots.*/
BOOL BattlerHasMoveSplit (struct BattleSystem *bsys, u32 battler, u32 move_split, struct AIContext *ai){
    BOOL hasMoveSplit = 0;
    struct BattleStruct *ctx = bsys->sp;

    for(unsigned int i = 0; i < 4; i++){
        u32 battler_move_split = ctx->moveTbl[ctx->battlemon[battler].move[i]].split ;
        if(battler_move_split == move_split){
            hasMoveSplit = 1;
        }
    }
    return hasMoveSplit;
}
/*Returns true if user has a particular move effect in their 4 move slots.*/
BOOL BattlerHasMoveEffect (struct BattleSystem *bsys, u32 battler, u32 move_effect, struct AIContext *ai){

    BOOL hasMoveEffect = 0;
    struct BattleStruct *ctx = bsys->sp;

    for(int i = 0; i < 4; i++){
        int battler_move_effect = ctx->moveTbl[ctx->battlemon[battler].move[i]].effect;
        if(battler_move_effect == move_effect){
            hasMoveEffect = 1;
        }
    }
    return hasMoveEffect;
}

/*Returns true if user has a particular damaging move type.*/
BOOL BattlerHasDamagingMoveType (struct BattleSystem *bsys, u32 battler, u32 type, struct AIContext *ai){

    BOOL hasMoveType = FALSE;
    struct BattleStruct *ctx = bsys->sp;

    for(int i = 0; i < 4; i++){
        u32 battler_move_type = ctx->moveTbl[ctx->battlemon[battler].move[i]].type;
        if(battler_move_type == type && ctx->moveTbl[ctx->battlemon[battler].move[i]].split != SPLIT_STATUS){
            hasMoveType = TRUE;
        }
    }
    return hasMoveType;
}

/*Returns true if the item has some sort of negative effect on holder.*/
BOOL ItemIsBadToTrade(u32 item, struct AIContext *ai){
    if((item == ITEM_IRON_BALL && !ai->trickRoomActive)||
    item == ITEM_LAGGING_TAIL ||item == ITEM_TOXIC_ORB ||
    item == ITEM_FLAME_ORB || item == ITEM_RING_TARGET ||
    item == ITEM_STICKY_BARB || item == ITEM_CHOICE_SCARF ||
    item == ITEM_CHOICE_SPECS || item == ITEM_CHOICE_BAND ||
    item == ITEM_BLACK_SLUDGE || item == ITEM_MACHO_BRACE ||
    item == ITEM_POWER_BRACER || item == ITEM_POWER_BAND ||
    item == ITEM_POWER_ANKLET || item == ITEM_POWER_BELT|| 
    item == ITEM_POWER_LENS || item == ITEM_POWER_WEIGHT){
        return TRUE;
    }
    else{
        return FALSE;
    }
}
/*Returns true if the item has some sort of negative effect on holder -- excludes some of the above functions' items.*/
BOOL ItemIsDisruptive(u32 item, struct AIContext *ai){
    if((item == ITEM_IRON_BALL && !ai->trickRoomActive)||
    item == ITEM_CHOICE_SCARF ||
    item == ITEM_CHOICE_SPECS || item == ITEM_CHOICE_BAND ||
    item == ITEM_STICKY_BARB ||
    item == ITEM_POWER_BRACER || item == ITEM_POWER_BAND ||
    item == ITEM_POWER_ANKLET || item == ITEM_POWER_BELT|| 
    item == ITEM_POWER_LENS || item == ITEM_POWER_WEIGHT){
        return 1;
    }
    else{
        return 0;
    }
}

/*Returns true if the ability passed is "good", which is pretty subjective.*/
BOOL IsDesirableAbility(u32 ability){
    if(ability == ABILITY_SPEED_BOOST ||
        ability == ABILITY_BATTLE_ARMOR ||
        ability == ABILITY_SAND_VEIL ||
        ability == ABILITY_STATIC ||
        ability == ABILITY_FLASH_FIRE||
        ability == ABILITY_WONDER_GUARD ||
        ability == ABILITY_SWIFT_SWIM ||
        ability == ABILITY_HUGE_POWER ||
        ability == ABILITY_RAIN_DISH ||
        ability == ABILITY_CUTE_CHARM ||
        ability == ABILITY_SHED_SKIN ||
        ability == ABILITY_MARVEL_SCALE ||
        ability == ABILITY_PURE_POWER||
        ability == ABILITY_CHLOROPHYLL ||
        ability == ABILITY_SHIELD_DUST ||
        ability == ABILITY_ADAPTABILITY ||
        ability == ABILITY_MAGIC_GUARD||
        ability == ABILITY_MOLD_BREAKER ||
        ability == ABILITY_SUPER_LUCK ||
        ability == ABILITY_UNAWARE ||
        ability == ABILITY_TINTED_LENS||
        ability == ABILITY_FILTER ||
        ability == ABILITY_SOLID_ROCK ||
        ability == ABILITY_RECKLESS ||
        ability == ABILITY_SHELL_ARMOR){
            return TRUE;
        }
    else{
        return FALSE;
    }
    
}

/**
 * @brief Check if the current move is a stat dropping move (Status moves only)
 * @param moveEffect move's effect
 * @return TRUE if it is a stat dropping move
*/
BOOL LONG_CALL DropsStatsAsStatus(u32 moveEffect) {
    u8 output = FALSE;
    for (u16 i = 0; i < NELEMS(DropStatList); i++) {
        if (moveEffect == DropStatList[i]) {
            output = TRUE;
            break;
        }
    }
    return output;
}

/**
 * @brief Check if the current move is a favored by mirror move
 * @param moveEffect move's effect
 * @return TRUE if it is in the mirror move list
*/
BOOL LONG_CALL IsInMirrorMoveList(u32 moveEffect) {
    u8 output = FALSE;
    for (u16 i = 0; i < NELEMS(MirrorMoveEffectList); i++) {
        if (moveEffect == MirrorMoveEffectList[i]) {
            output = TRUE;
            break;
        }
    }
    return output;
}

/*Returns true if a given list has a certain value in it.*/
BOOL LONG_CALL IsInList(u32 value, const u16 list[], u16 ListLength) {
    u8 output = FALSE;
    for (u16 i = 0; i < ListLength; i++) {
        if (value == list[i]) {
            output = TRUE;
            break;
        }
    }
    return output;
}

/*Returns true if the given battler has a move in a list.*/
BOOL LONG_CALL BattlerKnowsMoveInList(struct BattleSystem *bsys, u32 battler, const u16 MoveList[], u16 listLength, struct AIContext *ai) {
    struct BattleStruct *ctx = bsys->sp;
    BOOL knowsMove = FALSE;
    for (int i = 0; i < 4; i++) {
        for (int listIndex = 0; listIndex < listLength; listIndex++){
            if (ctx->battlemon[battler].move[i] == MoveList[listIndex]) {
                knowsMove = TRUE;
                break;
            }
        }
    }
    return knowsMove;
}

/*Returns true if the battler moves first. 
This function particularly accounts for 4 battlers, not just 2.*/
BOOL LONG_CALL BattlerMovesFirstDoubles(struct BattleSystem *bsys, struct BattleStruct *ctx, int mainBattler, int flag, struct AIContext *ai){
    BOOL movesFirst = TRUE;
    for (int otherBattler = 0; otherBattler < 4; otherBattler++){
        if(ctx->battlemon[otherBattler].hp != 0 && mainBattler != otherBattler){
            if(CalcSpeed(bsys, ctx, otherBattler, mainBattler, flag) != 1){
                return FALSE;
            }
        }
    }
    return movesFirst;
}

/*Returns true if the attacker's move index (0-3) has the highest max damage roll of all other move indices.*/
BOOL LONG_CALL MoveIsStrongest(struct BattleSystem *bsys, struct BattleStruct *ctx, int moveIndex, struct AIContext *ai){
    BOOL is_strongest = TRUE;
    for (int i = 0; i < 4; i++){
        if(i != moveIndex){
            if(ai->attackerAvgRollMoveDamages[moveIndex] < ai->attackerAvgRollMoveDamages[i]){
                return FALSE;
            }
        }
    }
    return is_strongest;
}

/*Set up all of the AI variables, exlcuding the double battle related ones, which are setup explicitly in TagStrategy Flag*/
void SetupStateVariables(struct BattleSystem *bsys, int attacker, u32 defender, struct AIContext *ai){
    struct BattleStruct *ctx = bsys->sp;
    u32 effectivenessFlag = 0;
    u8 critical = 0;
    u8 speedCalc;
    int work;
    debug_printf("Before setting up ai->\n");
    ai->attacker = attacker;
    ai->defender = defender;
    ai->attackerSide = BATTLER_IS_ENEMY(ai->attacker);
    ai->defenderSide = BATTLER_IS_ENEMY(ai->defender);
    ai->attackerLevel = ctx->battlemon[attacker].level;
    ai->attackerAbility = ctx->battlemon[attacker].ability;
    ai->defenderAbility = ctx->battlemon[ai->defender].ability;
    ai->attackerItem = ctx->battlemon[attacker].item;
    ai->defenderItem = ctx->battlemon[ai->defender].item;
    ai->holdEffect = BattleItemDataGet(ctx, ai->defenderItem, 1); //this was mistakenly documented as attacker's held item in ai.c -- fixed now
    ai->defenderType1 = ctx->battlemon[ai->defender].type1;
    ai->defenderType2 = ctx->battlemon[ai->defender].type2;
    ai->attackerType1 = ctx->battlemon[attacker].type1;
    ai->attackerType2 = ctx->battlemon[attacker].type2;
    ai->attackerHP = ctx->battlemon[attacker].hp;
    ai->defenderHP = ctx->battlemon[ai->defender].hp;
    ai->attackerMaxHP = ctx->battlemon[attacker].maxhp;
    ai->defenderMaxHP = ctx->battlemon[ai->defender].maxhp;
    ai->attackerPercentHP = ai->attackerHP * 100 / ai->attackerMaxHP;
    ai->defenderPercentHP = ai->defenderHP * 100 / ai->defenderMaxHP; 
    ai->attackerSpeed = ctx->battlemon[attacker].speed;
    ai->defenderSpeed = ctx->battlemon[ai->defender].speed;
    ai->attackerPartyIndex = ctx->sel_mons_no[attacker];
    ai->attackerLastUsedMove = ctx->waza_no_old[ai->attacker];
    ai->defenderLastUsedMove = ctx->waza_no_old[ai->defender];
    ai->defenderLastUsedMoveEffect = ctx->moveTbl[ai->defenderLastUsedMove].effect;
    ai->defenderTurnsOnField = ctx->total_turn - ctx->battlemon[ai->defender].moveeffect.fakeOutCount;
    ai->attackerTurnsOnField = ctx->total_turn - ctx->battlemon[attacker].moveeffect.fakeOutCount;
    ai->flingPower = BattleItemDataGet(ctx, ai->attackerItem, ITEM_PARAM_FLING_POWER);
    ai->differenceInAttackStages = ctx->battlemon[attacker].states[STAT_ATTACK] - ctx->battlemon[ai->defender].states[STAT_ATTACK];
    ai->differenceInSpAtkStages = ctx->battlemon[attacker].states[STAT_SPATK] - ctx->battlemon[ai->defender].states[STAT_SPATK];
    ai->differenceInDefenseStages = ctx->battlemon[attacker].states[STAT_DEFENSE] - ctx->battlemon[ai->defender].states[STAT_DEFENSE];
    ai->differenceInSpDefStages = ctx->battlemon[attacker].states[STAT_SPDEF] - ctx->battlemon[ai->defender].states[STAT_SPDEF];
    ai->defenderMovesFirst = 0;
    ai->attackerMovesFirst = 0;
    ai->isSpeedTie = 0;
    ai->trickRoomActive = 0;
    ai->defenderHasBadItem = 0;
    ai->maxDamageReceived = 0;
    ai->attackerMaxDamageOutputMinRoll = 0;
    debug_printf("After setting up ai->\n");
    FillDamageStructFromBattleMon(bsys, ctx, &ai->attackerMon, attacker);
    FillDamageStructFromBattleMon(bsys, ctx, &ai->defenderMon, defender);
    debug_printf("After filldamage struct\n");
    speedCalc = CalcSpeed(bsys, ctx, ai->defender, attacker, CALCSPEED_FLAG_NO_PRIORITY); //checks actual turn order with field state considered
    debug_printf("after calc speed\n");
    //evaluates to 0 if ai->defender > attacker (false)
    //and 1 if ai->defender < attacker (true)
    //if speed tie, then 2.
    if(speedCalc == 0){
        ai->defenderMovesFirst = 1;
    }
    else if(speedCalc == 1){
        ai->attackerMovesFirst = 1;
    }
    else if(speedCalc == 2){
        ai->isSpeedTie = 1;
    }

    if(ctx->field_condition & FIELD_STATUS_TRICK_ROOM){
        ai->trickRoomActive = 1;
    }
    if((ai->defenderItem == ITEM_IRON_BALL && !ai->trickRoomActive)||ai->defenderItem == ITEM_LAGGING_TAIL ||ai->defenderItem == ITEM_TOXIC_ORB ||ai->defenderItem == ITEM_FLAME_ORB || ai->defenderItem == ITEM_RING_TARGET ||
    ai->defenderItem == ITEM_STICKY_BARB || ai->defenderItem == ITEM_CHOICE_SCARF || ai->defenderItem == ITEM_CHOICE_SPECS || ai->defenderItem == ITEM_CHOICE_BAND){
        ai->defenderHasBadItem = 1;
    }
    debug_printf("before stat change\n");

    /*Check if target or user has stat change*/
    ai->hasStatChange = 0;
    for(int i = 0; i < STAT_MAX; i++){
        if (ctx->battlemon[attacker].states[i] != 0 ||
            ctx->battlemon[ai->defender].states[i] != 0){
            ai->hasStatChange = 1;
        }
    }
        debug_printf("before hazards\n");

    /*Check if ai->defender has any IMPACTFUL hazards up (spikes at level 2 or higher)*/
    if(ctx->side_condition[ai->defenderSide] & SIDE_STATUS_STEALTH_ROCK ||
        ctx->scw[ai->defenderSide].spikesLayers >= 2 ||
        ctx->scw[ai->defenderSide].toxicSpikesLayers >= 1){
        ai->defenderHasHazards = 1;
    }
    else{
        ai->defenderHasHazards = 0;
    }
    debug_printf("before immunities\n");
    ai->defenderImmuneToPoison = 
        (ai->defenderType1 == TYPE_POISON || ai->defenderType2 == TYPE_POISON || //TODO need to consider corrosion
        ai->defenderType1 == TYPE_STEEL || ai->defenderType2 == TYPE_STEEL ||
        ctx->battlemon[ai->defender].condition & STATUS_ALL || 
        ctx->side_condition[ai->defenderSide] & SIDE_STATUS_SAFEGUARD ||
        ai->defenderAbility == ABILITY_MAGIC_GUARD ||
        ai->defenderAbility == ABILITY_IMMUNITY ||
        ai->defenderAbility == ABILITY_POISON_HEAL ||
        (ai->defenderAbility == ABILITY_LEAF_GUARD && ctx->field_condition & WEATHER_SUNNY_ANY)|| 
        (ai->defenderAbility == ABILITY_HYDRATION && ctx->field_condition & WEATHER_RAIN_ANY) ) ||
        (IsClientGrounded(ctx, ai->defender) && ctx->terrainOverlay.type == MISTY_TERRAIN);
    ai->defenderImmuneToParalysis =
        (ai->defenderType1 == TYPE_ELECTRIC || ai->defenderType2 == TYPE_ELECTRIC ||
        ctx->battlemon[ai->defender].condition & STATUS_ALL || 
        ctx->side_condition[ai->defenderSide] & SIDE_STATUS_SAFEGUARD ||
        ai->defenderAbility == ABILITY_LIMBER ||
        (ai->defenderAbility == ABILITY_LEAF_GUARD && ctx->field_condition & WEATHER_SUNNY_ANY)|| 
        (ai->defenderAbility == ABILITY_HYDRATION && ctx->field_condition & WEATHER_RAIN_ANY) ||
        (ai->defenderAbility == ABILITY_MAGIC_GUARD && ctx->battlemon[attacker].speed > ctx->battlemon[ai->defender].speed)) ||
        (IsClientGrounded(ctx, ai->defender) && ctx->terrainOverlay.type == MISTY_TERRAIN) ;
    ai->defenderImmuneToBurn =  
        (ai->defenderType1 == TYPE_FIRE || ai->defenderType2 == TYPE_FIRE ||
        ctx->battlemon[ai->defender].condition & STATUS_ALL || 
        ctx->side_condition[ai->defenderSide] & SIDE_STATUS_SAFEGUARD ||
        ai->defenderAbility == ABILITY_MAGIC_GUARD ||
        ai->defenderAbility == ABILITY_WATER_VEIL ||
        ai->defenderAbility == ABILITY_THERMAL_EXCHANGE ||
        ai->defenderAbility == ABILITY_WATER_BUBBLE) ||
        (ai->defenderAbility == ABILITY_LEAF_GUARD && ctx->field_condition & WEATHER_SUNNY_ANY)|| 
        (ai->defenderAbility == ABILITY_HYDRATION && ctx->field_condition & WEATHER_RAIN_ANY) ||
        (IsClientGrounded(ctx, ai->defender) && ctx->terrainOverlay.type == MISTY_TERRAIN);
    ai->defenderImmuneToSleep = (ctx->battlemon[ai->defender].condition & STATUS_ALL || 
        ctx->side_condition[ai->defenderSide] & SIDE_STATUS_SAFEGUARD ||
        ai->defenderAbility == ABILITY_VITAL_SPIRIT ||
        ai->defenderAbility == ABILITY_INSOMNIA || 
        (ai->defenderAbility == ABILITY_LEAF_GUARD && ctx->field_condition & WEATHER_SUNNY_ANY)|| 
        (ai->defenderAbility == ABILITY_HYDRATION && ctx->field_condition & WEATHER_RAIN_ANY) ||
        (ai->defenderAbility == ABILITY_MAGIC_GUARD && ctx->battlemon[attacker].speed > ctx->battlemon[ai->defender].speed)||
        (IsClientGrounded(ctx, ai->defender) && (ctx->terrainOverlay.type == ELECTRIC_TERRAIN ||ctx->terrainOverlay.type == MISTY_TERRAIN)));
    //ai->attackerMoveEffectiveness = 0;
    ai->partySizeAttacker = Battle_GetClientPartySize(bsys, attacker);
    ai->livingMembersAttacker = 0;
    ai->partySizeDefender = Battle_GetClientPartySize(bsys, ai->defender);
    ai->livingMembersDefender = 0;
    ai->statusedMonsAttacker = 0;
    ai->damagedMonsDefender = 0;
    debug_printf("before partyattacker size loop\n");
    /*Check if attacker's team has anyone that is statused or damaged.
    Also compute number of living members on attacker's team.*/
    for (int i = 0; i < ai->partySizeAttacker; i++) {
        struct PartyPokemon * currentMonAttacking = Battle_GetClientPartyMon(bsys, attacker, i);

        if(!(GetMonData(currentMonAttacking, MON_DATA_HP, 0) == 0 ||
        GetMonData(currentMonAttacking, MON_DATA_SPECIES_OR_EGG, 0) == 0||
        GetMonData(currentMonAttacking, MON_DATA_SPECIES_OR_EGG, 0) == 494)){

            ai->livingMembersAttacker++;
            work = GetMonData(currentMonAttacking, MON_DATA_STATUS, NULL);
            if(GetMonData(currentMonAttacking, MON_DATA_HP, 0) !=
            GetMonData(currentMonAttacking, MON_DATA_MAXHP, 0)){
                ai->damagedMonsDefender++;
            }
            if(!(work & STATUS_NONE)){
                ai->statusedMonsAttacker++;
            }
        }
    }
    debug_printf("Before defending team loop\n");
    /*Loop over defending team*/
    for (int i = 0; i < ai->partySizeDefender; i++) {
        struct PartyPokemon * currentMonDefending = Battle_GetClientPartyMon(bsys, ai->defender, i);
        if(!(GetMonData(currentMonDefending, MON_DATA_HP, 0) == 0 ||
        GetMonData(currentMonDefending, MON_DATA_SPECIES_OR_EGG, 0) == 0||
        GetMonData(currentMonDefending, MON_DATA_SPECIES_OR_EGG, 0) == 494)){
            ai->livingMembersDefender++;
        }
    }

    ai->damagedMonsDefender = 0;
    ai->statusedMonsAttacker = 0;
    ai->attackerKnowsPsychUp = 0;
    ai->attackerMovesKnown = GetBattlerLearnedMoveCount(bsys, ctx, attacker);
    ai->attackerHasSupereffectiveMove = 0;
    ai->attackerHasDamagingMove = 0;

    int attackerEffectCheck;
    int specialMovePower = 0;

    /*Loop over defender's moves, and compute the most damage AI can take*/
    int currentReceivedDamage = 0;
    for (int i = 0; i < GetBattlerLearnedMoveCount(bsys, ctx, ai->defender); i++){
        specialMovePower = 0;
        u32 defenderMoveno = ctx->battlemon[defender].move[i];
        struct BattleMove defenderMove = ctx->moveTbl[defenderMoveno];
        struct AI_damage damages = { 0 };
        if(ctx->moveTbl[ctx->battlemon[ai->defender].move[i]].split != SPLIT_STATUS){
            //specialMovePower = AdjustUnusualMovePower(bsys, ai->defender, ai->attacker, ctx->moveTbl[ctx->battlemon[ai->defender].move[i]].effect, ai);
            currentReceivedDamage = BattleAI_CalcDamage(bsys, ctx, ctx->battlemon[ai->defender].move[i], ctx->side_condition[BATTLER_IS_ENEMY(ai->defender)], ctx->field_condition, defenderMove.power, defenderMove.type, 0, ai->defender, ai->attacker,&damages, &ai->defenderMon, &ai->attackerMon);
            //currentReceivedDamage = CalcBaseDamage(bsys, ctx, ctx->battlemon[ai->defender].move[i], ctx->side_condition[ai->attackerSide],ctx->field_condition, specialMovePower, 0, ai->defender, ai->attacker, 0, 0, 0, NULL);
            //currentReceivedDamage = ServerDoTypeCalcMod(bsys, ctx, ctx->battlemon[ai->defender].move[i], 0, ai->defender, ai->attacker, currentReceivedDamage, &temp)*92 / 100; // looking at MIN roll. //*85 / 100 for min roll
            debug_printf("current received damage in main.c done\n");
            //currentReceivedDamage = AdjustUnusualMoveDamage(bsys, ai->defender, ai->attacker, currentReceivedDamage, ctx->moveTbl[ctx->battlemon[ai->defender].move[i]].effect, ai);
            currentReceivedDamage = BattleAI_AdjustUnusualMoveDamage(ai->defenderMon.level, ai->defenderMon.hp, ai->attackerMon.hp, currentReceivedDamage, defenderMove.effect, ai->defenderMon.ability, ai->defenderMon.item);
            debug_printf("damage received for move index %d is: %d\n", i, currentReceivedDamage);
            if(currentReceivedDamage > ai->maxDamageReceived){
                
                ai->maxDamageReceived = currentReceivedDamage;
            }
        }
        //debug_printf("Max damage received for move %d is: %d\n",i,currentReceivedDamage);
    }
    //debug_printf("Max damage received PERIOD is: %d\n",ai->maxDamageReceived);

    /*Loop over all moves for checking certain conditions*/
    /*Set up max roll damage calculations for all known moves.
    Also check if user has a super-effective move*/
    for(int i = 0; i < ai->attackerMovesKnown; i++){
        struct AI_damage damages = { 0 };
        int attackerMoveno = ctx->battlemon[attacker].move[i];
        struct BattleMove attackerMove = ctx->moveTbl[attackerMoveno];
        //attackerMoveCheck = ctx->battlemon[attacker].move[i];
        //attackerEffectCheck = ctx->moveTbl[attackerMoveCheck].effect;
        //attackerMoveTypeCheck = ctx->moveTbl[attackerMoveCheck].type;
        //specialMovePower = 0;
        if(attackerMoveno == MOVE_PSYCH_UP){
            ai->attackerKnowsPsychUp = 1;
        }
        if(ctx->moveTbl[attackerMoveno].split != SPLIT_STATUS){
            if(attackerEffectCheck == MOVE_EFFECT_RANDOM_POWER_10_CASES){ //average magnitude power
                specialMovePower = 71;
            }

            ai->attackerAvgRollMoveDamages[i] = BattleAI_CalcDamage(bsys, ctx, attackerMoveno, ctx->side_condition[BATTLER_IS_ENEMY(attacker)], ctx->field_condition, attackerMove.power, attackerMove.type, critical, attacker, defender, &damages, &ai->attackerMon, &ai->defenderMon);

            ai->attackerAvgRollMoveDamages[i] = BattleAI_AdjustUnusualMoveDamage(ai->attackerMon.level, ai->attackerMon.hp, ai->defenderMon.hp, damages.damageRoll, attackerMove.effect, ai->attackerMon.ability, ai->attackerMon.item);
            
            //specialMovePower = AdjustUnusualMovePower(bsys, attacker, ai->defender, attackerEffectCheck, ai);
            //ai->attackerAvgRollMoveDamages[i] = CalcBaseDamage(bsys, ctx, attackerMoveCheck, ctx->side_condition[ai->defenderSide],ctx->field_condition, specialMovePower, 0, ai->attacker, ai->defender, 0, 0, 0, NULL);
            //ai->attackerAvgRollMoveDamages[i] = ServerDoTypeCalcMod(bsys, ctx, attackerMoveCheck, 0, attacker, ai->defender, ai->attackerAvgRollMoveDamages[i], &temp)*92 / 100; //85% is min roll. // *85 / 100 FOR MIN ROLL
            //ai->attackerAvgRollMoveDamages[i] = AdjustUnusualMoveDamage(bsys, ai->attacker, ai->defender, ai->attackerAvgRollMoveDamages[i], attackerEffectCheck, ai);

        }
       
        /*Record our highest damage output*/
        if(ai->attackerAvgRollMoveDamages[i] > ai->attackerMaxDamageOutputMinRoll){
            ai->attackerMaxDamageOutputMinRoll = ai->attackerAvgRollMoveDamages[i];
        }

        if(ai->attackerAvgRollMoveDamages[i] > 0){
            ai->attackerHasDamagingMove = TRUE;
        }
        ai->attackerMoveEffectiveness[i] = BattleAI_GetTypeEffectiveness(bsys, ctx, attackerMove.type, &effectivenessFlag, &ai->attackerMon, &ai->defenderMon);
        //AITypeCalc(ctx, attackerMoveCheck, attackerMoveTypeCheck, ai->attackerAbility, ai->defenderAbility, ai->holdEffect, ai->defenderType1, ai->defenderType2, & ai->attackerMoveEffectiveness);
        if(ai->attackerMoveEffectiveness[i] == TYPE_MUL_SUPER_EFFECTIVE){
            ai->attackerHasSupereffectiveMove = TRUE;
        }

    }
}

/*Adjusts the computed damage for attacks like multihit or flat damage moves.*/
int AdjustUnusualMoveDamage(struct BattleSystem *bsys, int attacker, u32 defender, int damage, int moveEffect, struct AIContext *ai){
    struct BattleStruct *ctx = bsys->sp;
    debug_printf("moveeffect in unusualdamage is: %d\n", moveEffect);
    switch(moveEffect){
        case MOVE_EFFECT_MULTI_HIT: //2-5 hit moves
            if(ctx->battlemon[attacker].ability == ABILITY_SKILL_LINK){
                return damage *= 5; //skill link guarantees 5 hits
            }
            if(ctx->battlemon[attacker].item == ITEM_LOADED_DICE){
                return damage *= 4;
            }
            return damage *= 3;
        case MOVE_EFFECT_LEVEL_DAMAGE_FLAT: //night shade, seismic toss
        case MOVE_EFFECT_RANDOM_DAMAGE_1_TO_150_LEVEL: //psybeam
            debug_printf("Setting defender's HP to attacker's level in AdjustUnusualMoveDamage\n");
            return ctx->battlemon[attacker].level;
        case MOVE_EFFECT_10_DAMAGE_FLAT: //sonic boom
            return 20;
        case MOVE_EFFECT_40_DAMAGE_FLAT: //dragon rage
            return 40;
        case MOVE_EFFECT_POISON_MULTI_HIT: //twinneedle
        case MOVE_EFFECT_HIT_TWICE: //double hit, dual wingbeat, etc...
            debug_printf("Setting defender's HP to half in AdjustUnusualMoveDamage\n");
            return damage *= 2;
            
        case MOVE_EFFECT_HALVE_HP: //super fang, nature's madness
            debug_printf("Setting defender's HP to half in AdjustUnusualMoveDamage\n");
            return ctx->battlemon[defender].hp / 2;
        case MOVE_EFFECT_SET_HP_EQUAL_TO_USER: //endeavor
            debug_printf("Setting defender's HP equal to attacker's HP in AdjustUnusualMoveDamage\n");
            return ctx->battlemon[defender].hp - ctx->battlemon[attacker].hp;
        case MOVE_EFFECT_DOUBLE_DAMAGE_ON_STATUS:
            if(!(ctx->battlemon[defender].condition & STATUS_NONE)){
                return damage *= 2;
            }
        case MOVE_EFFECT_DOUBLE_POWER_EACH_TURN_LOCK_INTO:
            return damage * 3 / 2; //next two turns average damage 
        default:
            return damage;
    }
    return damage;
}

/*Returns the true move power of variable power moves like reversal or magnitude*/
int AdjustUnusualMovePower(struct BattleSystem *bsys, int attacker, u32 defender, int moveEffect, struct AIContext *ai){
    struct BattleStruct *ctx = bsys->sp;
    switch(moveEffect){
        case MOVE_EFFECT_INCREASE_POWER_WITH_WEIGHT://low kick and grass knot (measured in lbs)
            if(ctx->battlemon[defender].weight < 22){
                return 20;
            }
            else if(ctx->battlemon[defender].weight < 55){
                return 40;
            }
            else if(ctx->battlemon[defender].weight < 110){
                return 60;
            }
            else if(ctx->battlemon[defender].weight < 220){
                return 80;
            }
            else if(ctx->battlemon[defender].weight < 440){
                return 100;
            }
            else{
                return 120;
            }
        case MOVE_EFFECT_RANDOM_POWER_10_CASES:
            return 71; //average power
        case MOVE_EFFECT_INCREASE_POWER_WITH_LESS_HP:
            if(ai->attackerPercentHP <= 4){
                return 200;
            }
            else if(ai->attackerPercentHP <= 10){
                return 150;
            }
            else if(ai->attackerPercentHP <= 21){
                return 100;
            }
            else if(ai->attackerPercentHP <= 35){
                return 80;
            }
            else if(ai->attackerPercentHP <= 69){
                return 40;
            }
            else{
                return 20;
            }
    }
    return 0;
}