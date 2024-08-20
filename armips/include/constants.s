// keeping this in armips/include for the time being

// types

.equ TYPE_NORMAL, 0
.equ TYPE_FIGHTING, 1
.equ TYPE_FLYING, 2
.equ TYPE_POISON, 3
.equ TYPE_GROUND, 4
.equ TYPE_ROCK, 5
.equ TYPE_BUG, 6
.equ TYPE_GHOST, 7
.equ TYPE_STEEL, 8
.equ TYPE_MYSTERY, 9
.equ TYPE_FAIRY, 9
.equ TYPE_FIRE, 10
.equ TYPE_WATER, 11
.equ TYPE_GRASS, 12
.equ TYPE_ELECTRIC, 13
.equ TYPE_PSYCHIC, 14
.equ TYPE_ICE, 15
.equ TYPE_DRAGON, 16
.equ TYPE_DARK, 17

// body colors

.equ BODY_COLOR_RED, 0
.equ BODY_COLOR_BLUE, 1
.equ BODY_COLOR_YELLOW, 2
.equ BODY_COLOR_GREEN, 3
.equ BODY_COLOR_BLACK, 4
.equ BODY_COLOR_BROWN, 5
.equ BODY_COLOR_PURPLE, 6
.equ BODY_COLOR_GRAY, 7
.equ BODY_COLOR_WHITE, 8
.equ BODY_COLOR_PINK, 9
.equ BODY_COLOR_EGG, 10

// growth rates

.equ GROWTH_MEDIUM_FAST, 0
.equ GROWTH_ERRATIC, 1
.equ GROWTH_FLUCTUATING, 2
.equ GROWTH_MEDIUM_SLOW, 3
.equ GROWTH_FAST, 4
.equ GROWTH_SLOW, 5

// egg groups

.equ EGG_GROUP_NONE, 0
.equ EGG_GROUP_MONSTER, 1
.equ EGG_GROUP_WATER_1, 2
.equ EGG_GROUP_BUG, 3
.equ EGG_GROUP_FLYING, 4
.equ EGG_GROUP_FIELD, 5
.equ EGG_GROUP_FAIRY, 6
.equ EGG_GROUP_GRASS, 7
.equ EGG_GROUP_HUMAN_LIKE, 8
.equ EGG_GROUP_WATER_3, 9
.equ EGG_GROUP_MINERAL, 10
.equ EGG_GROUP_AMORPHOUS, 11
.equ EGG_GROUP_WATER_2, 12
.equ EGG_GROUP_DITTO, 13
.equ EGG_GROUP_DRAGON, 14
.equ EGG_GROUP_UNDISCOVERED, 15

// evo method constants

.equ EVO_NONE, 0
.equ EVO_FRIENDSHIP, 1
.equ EVO_FRIENDSHIP_DAY, 2
.equ EVO_FRIENDSHIP_NIGHT, 3
.equ EVO_LEVEL, 4
.equ EVO_TRADE, 5
.equ EVO_TRADE_ITEM, 6
.equ EVO_STONE, 7
.equ EVO_LEVEL_ATK_GT_DEF, 8
.equ EVO_LEVEL_ATK_EQ_DEF, 9
.equ EVO_LEVEL_ATK_LT_DEF, 10
.equ EVO_LEVEL_PID_LO, 11
.equ EVO_LEVEL_PID_HI, 12
.equ EVO_LEVEL_NINJASK, 13
.equ EVO_LEVEL_SHEDINJA, 14
.equ EVO_BEAUTY, 15
.equ EVO_STONE_MALE, 16
.equ EVO_STONE_FEMALE, 17
.equ EVO_ITEM_DAY, 18
.equ EVO_ITEM_NIGHT, 19
.equ EVO_HAS_MOVE, 20
.equ EVO_OTHER_PARTY_MON, 21
.equ EVO_LEVEL_MALE, 22
.equ EVO_LEVEL_FEMALE, 23
.equ EVO_LEVEL_ELECTRIC_FIELD, 24
.equ EVO_LEVEL_MOSSY_STONE, 25
.equ EVO_LEVEL_ICY_STONE, 26
.equ EVO_LEVEL_DAY, 27
.equ EVO_LEVEL_NIGHT, 28
.equ EVO_LEVEL_DUSK, 29
.equ EVO_LEVEL_RAIN, 30
.equ EVO_HAS_MOVE_TYPE, 31
.equ EVO_LEVEL_DARK_TYPE_MON_IN_PARTY, 32
.equ EVO_TRADE_SPECIFIC_MON, 33
.equ EVO_LEVEL_NATURE_AMPED, 34
.equ EVO_LEVEL_NATURE_LOW_KEY, 35
.equ EVO_AMOUNT_OF_CRITICAL_HITS, 36
.equ EVO_HURT_IN_BATTLE_AMOUNT, 37
//EVO_DARK_SCROLL  // implemented through an item use forme-change-esque cut scene
//EVO_WATER_SCROLL // implemented through an item use forme-change-esque cut scene

// shadow size constants

.equ SHADOW_NO_SHADOW, 0
.equ SHADOW_SIZE_SMALL, 1
.equ SHADOW_SIZE_MEDIUM, 2
.equ SHADOW_SIZE_LARGE, 3

// overworld constants

.equ OVERWORLD_CAN_ENTER, 0
.equ OVERWORLD_NO_ENTRY, 1

.equ OVERWORLD_BOUNCE_FAST, 0x00
.equ OVERWORLD_BOUNCE_MED, 0x10
.equ OVERWORLD_BOUNCE_SLOW, 0x11

// pokedex data constants
.equ DEX_END_AREA_DATA, 0x0

// special areas:
.equ DEX_SPROUT_TOWER, 0x1
.equ DEX_RUINS_OF_ALPH, 0x2
.equ DEX_UNION_CAVE, 0x3
.equ DEX_SLOWPOKE_WELL, 0x4
.equ DEX_ILEX_FOREST, 0x5
.equ DEX_NATIONAL_PARK, 0x6
.equ DEX_BURNED_TOWER, 0x7
.equ DEX_BELL_TOWER, 0x8
.equ DEX_WHIRL_ISLANDS, 0x9
.equ DEX_MT_MORTAR, 0xA
.equ DEX_ICE_PATH, 0xB
.equ DEX_DRAGONS_DEN, 0xC
.equ DEX_DARK_CAVE, 0xD
.equ DEX_MT_MOON, 0xE
.equ DEX_SEAFOAM_ISLANDS, 0xF
.equ DEX_MT_SILVER_CAVE, 0x10
.equ DEX_CLIFF_EDGE_GATE, 0x11
.equ DEX_CLIFF_CAVE, 0x12
.equ DEX_OLIVINE_CITY_SPECIAL, 0x13 // appears in the bottom right though
.equ DEX_ROCK_TUNNEL, 0x14
.equ DEX_VICTORY_ROAD, 0x15
.equ DEX_TOHJO_FALLS, 0x16
.equ DEX_DIGLETTS_CAVE, 0x17
.equ DEX_VIRIDIAN_FOREST, 0x18
.equ DEX_CERULEAN_CAVE, 0x19
/*.equ DEX_FARAWAY_PLACE_KANTO, 0x1A // no flashing but in kanto
.equ DEX_SAFFRON_CITY, 0x1B // in top right corner of johto
.equ DEX_FARAWAY_PLACE_2, 0x1C // no flashing but in kanto
.equ DEX_SAFFRON_CITY_2, 0x1D // no flashing but in kanto*/

// cities/routes and the like:
.equ DEX_NEW_BARK_TOWN, 0x1
.equ DEX_ROUTE_29, 0x2
.equ DEX_CHERRYGROVE_CITY, 0x3
.equ DEX_ROUTE_30, 0x4
.equ DEX_ROUTE_31, 0x5
.equ DEX_VIOLET_CITY, 0x6
.equ DEX_ROUTE_32, 0x7
.equ DEX_ROUTE_33, 0x8
.equ DEX_ROUTE_34, 0x9
.equ DEX_ROUTE_35, 0xA
.equ DEX_ROUTE_36, 0xB
.equ DEX_ROUTE_37, 0xC
.equ DEX_ECRUTEAK_CITY, 0xD
.equ DEX_ROUTE_38, 0xE
.equ DEX_ROUTE_39, 0xF
.equ DEX_OLIVINE_CITY, 0x10
.equ DEX_ROUTE_40, 0x11
.equ DEX_ROUTE_41, 0x12
.equ DEX_CIANWOOD_CITY, 0x13
.equ DEX_ROUTE_42, 0x14
.equ DEX_ROUTE_43, 0x15
.equ DEX_LAKE_OF_RAGE, 0x16
.equ DEX_ROUTE_44, 0x17
.equ DEX_BLACKTHORN_CITY, 0x18
.equ DEX_ROUTE_45, 0x19
.equ DEX_ROUTE_46, 0x1A
.equ DEX_ROUTE_47, 0x1B
.equ DEX_MT_SILVER, 0x1C
.equ DEX_ROUTE_12, 0x1D
.equ DEX_ROUTE_19, 0x1E
.equ DEX_ROUTE_20, 0x1F
.equ DEX_PALLET_TOWN, 0x20
.equ DEX_VIRIDIAN_CITY, 0x21
.equ DEX_CERULEAN_CITY, 0x22
.equ DEX_VERMILION_CITY, 0x23
.equ DEX_CELADON_CITY, 0x24
.equ DEX_FUCHSIA_CITY, 0x25
.equ DEX_CINNABAR_CITY, 0x26
.equ DEX_ROUTE_48, 0x27
.equ DEX_ROUTE_26, 0x28
.equ DEX_ROUTE_27, 0x29
.equ DEX_ROUTE_28, 0x2A
.equ DEX_ROUTE_1, 0x2B
.equ DEX_ROUTE_2, 0x2C
.equ DEX_ROUTE_3, 0x2D
.equ DEX_ROUTE_4, 0x2E
.equ DEX_ROUTE_5, 0x2F
.equ DEX_ROUTE_6, 0x30
.equ DEX_ROUTE_7, 0x31
.equ DEX_ROUTE_8, 0x32
.equ DEX_ROUTE_9, 0x33
.equ DEX_ROUTE_10, 0x34
.equ DEX_ROUTE_11, 0x35
.equ DEX_ROUTE_13, 0x36
.equ DEX_ROUTE_14, 0x37
.equ DEX_ROUTE_15, 0x38
.equ DEX_ROUTE_16, 0x39
.equ DEX_ROUTE_17, 0x3A
.equ DEX_ROUTE_18, 0x3B
.equ DEX_ROUTE_21, 0x3C
.equ DEX_ROUTE_22, 0x3D
.equ DEX_ROUTE_24, 0x3E
.equ DEX_ROUTE_25, 0x3F
.equ DEX_ROUTE_2_2, 0x40 // same as above route 2
.equ DEX_PEWTER_CITY, 0x41
.equ DEX_AZALEA_TOWN, 0x42
.equ DEX_SAFARI_ZONE_GATE, 0x43
.equ DEX_ROUTE_16_2, 0x44 // same as above route 16
/*.equ DEX_FARAWAY_PLACE_JOHTO, // johto faraway place, no blinking
.equ DEX_ROUTE_1_2, // route 1 no blinking*/

// these constants used for organization i guess
.equ DEX_MORNING, 0
.equ DEX_DAY, 1
.equ DEX_NIGHT, 2
.equ DEX_SPECIAL, 3 // like headbutt trees (blink the area red)

// tm constants

.equ TM001, 1 << 0
.equ TM002, 1 << 1
.equ TM003, 1 << 2
.equ TM004, 1 << 3
.equ TM005, 1 << 4
.equ TM006, 1 << 5
.equ TM007, 1 << 6
.equ TM008, 1 << 7
.equ TM009, 1 << 8
.equ TM010, 1 << 9
.equ TM011, 1 << 10
.equ TM012, 1 << 11
.equ TM013, 1 << 12
.equ TM014, 1 << 13
.equ TM015, 1 << 14
.equ TM016, 1 << 15
.equ TM017, 1 << 16
.equ TM018, 1 << 17
.equ TM019, 1 << 18
.equ TM020, 1 << 19
.equ TM021, 1 << 20
.equ TM022, 1 << 21
.equ TM023, 1 << 22
.equ TM024, 1 << 23
.equ TM025, 1 << 24
.equ TM026, 1 << 25
.equ TM027, 1 << 26
.equ TM028, 1 << 27
.equ TM029, 1 << 28
.equ TM030, 1 << 29
.equ TM031, 1 << 30
.equ TM032, 1 << 31
.equ TM033, 1 << 0
.equ TM034, 1 << 1
.equ TM035, 1 << 2
.equ TM036, 1 << 3
.equ TM037, 1 << 4
.equ TM038, 1 << 5
.equ TM039, 1 << 6
.equ TM040, 1 << 7
.equ TM041, 1 << 8
.equ TM042, 1 << 9
.equ TM043, 1 << 10
.equ TM044, 1 << 11
.equ TM045, 1 << 12
.equ TM046, 1 << 13
.equ TM047, 1 << 14
.equ TM048, 1 << 15
.equ TM049, 1 << 16
.equ TM050, 1 << 17
.equ TM051, 1 << 18
.equ TM052, 1 << 19
.equ TM053, 1 << 20
.equ TM054, 1 << 21
.equ TM055, 1 << 22
.equ TM056, 1 << 23
.equ TM057, 1 << 24
.equ TM058, 1 << 25
.equ TM059, 1 << 26
.equ TM060, 1 << 27
.equ TM061, 1 << 28
.equ TM062, 1 << 29
.equ TM063, 1 << 30
.equ TM064, 1 << 31
.equ TM065, 1 << 0
.equ TM066, 1 << 1
.equ TM067, 1 << 2
.equ TM068, 1 << 3
.equ TM069, 1 << 4
.equ TM070, 1 << 5
.equ TM071, 1 << 6
.equ TM072, 1 << 7
.equ TM073, 1 << 8
.equ TM074, 1 << 9
.equ TM075, 1 << 10
.equ TM076, 1 << 11
.equ TM077, 1 << 12
.equ TM078, 1 << 13
.equ TM079, 1 << 14
.equ TM080, 1 << 15
.equ TM081, 1 << 16
.equ TM082, 1 << 17
.equ TM083, 1 << 18
.equ TM084, 1 << 19
.equ TM085, 1 << 20
.equ TM086, 1 << 21
.equ TM087, 1 << 22
.equ TM088, 1 << 23
.equ TM089, 1 << 24
.equ TM090, 1 << 25
.equ TM091, 1 << 26
.equ TM092, 1 << 27
.equ HM001, 1 << 28
.equ HM002, 1 << 29
.equ HM003, 1 << 30
.equ HM004, 1 << 31
.equ HM005, 1 << 0
.equ HM006, 1 << 1
.equ HM007, 1 << 2
.equ HM008, 1 << 3

// tutor field constants

.equ TUTOR_DIVE, 1 << 0
.equ TUTOR_MUD_SLAP, 1 << 1
.equ TUTOR_FURY_CUTTER, 1 << 2
.equ TUTOR_ICY_WIND, 1 << 3
.equ TUTOR_ROLLOUT, 1 << 4
.equ TUTOR_THUNDER_PUNCH, 1 << 5
.equ TUTOR_FIRE_PUNCH, 1 << 6
.equ TUTOR_SUPERPOWER, 1 << 7
.equ TUTOR_ICE_PUNCH, 1 << 8
.equ TUTOR_IRON_HEAD, 1 << 9
.equ TUTOR_AQUA_TAIL, 1 << 10
.equ TUTOR_OMINOUS_WIND, 1 << 11
.equ TUTOR_GASTRO_ACID, 1 << 12
.equ TUTOR_SNORE, 1 << 13
.equ TUTOR_SPITE, 1 << 14
.equ TUTOR_AIR_CUTTER, 1 << 15
.equ TUTOR_HELPING_HAND, 1 << 16
.equ TUTOR_ENDEAVOR, 1 << 17
.equ TUTOR_OUTRAGE, 1 << 18
.equ TUTOR_ANCIENT_POWER, 1 << 19
.equ TUTOR_SYNTHESIS, 1 << 20
.equ TUTOR_SIGNAL_BEAM, 1 << 21
.equ TUTOR_ZEN_HEADBUTT, 1 << 22
.equ TUTOR_VACUUM_WAVE, 1 << 23
.equ TUTOR_EARTH_POWER, 1 << 24
.equ TUTOR_GUNK_SHOT, 1 << 25
.equ TUTOR_TWISTER, 1 << 26
.equ TUTOR_SEED_BOMB, 1 << 27
.equ TUTOR_IRON_DEFENSE, 1 << 28
.equ TUTOR_MAGNET_RISE, 1 << 29
.equ TUTOR_LAST_RESORT, 1 << 30
.equ TUTOR_BOUNCE, 1 << 31
.equ TUTOR_TRICK, 1 << 0
.equ TUTOR_HEAT_WAVE, 1 << 1
.equ TUTOR_KNOCK_OFF, 1 << 2
.equ TUTOR_SUCKER_PUNCH, 1 << 3
.equ TUTOR_SWIFT, 1 << 4
.equ TUTOR_UPROAR, 1 << 5
.equ TUTOR_SUPER_FANG, 1 << 6
.equ TUTOR_PAIN_SPLIT, 1 << 7
.equ TUTOR_STRING_SHOT, 1 << 8
.equ TUTOR_TAILWIND, 1 << 9
.equ TUTOR_GRAVITY, 1 << 10
.equ TUTOR_WORRY_SEED, 1 << 11
.equ TUTOR_MAGIC_COAT, 1 << 12
.equ TUTOR_ROLE_PLAY, 1 << 13
.equ TUTOR_HEAL_BELL, 1 << 14
.equ TUTOR_LOW_KICK, 1 << 15
.equ TUTOR_SKY_ATTACK, 1 << 16
.equ TUTOR_BLOCK, 1 << 17
.equ TUTOR_BUG_BITE, 1 << 18
.equ TUTOR_HEADBUTT, 1 << 19

// trainer classes

.equ CLASS_PKMN_TRAINER_0, 0
.equ CLASS_PKMN_TRAINER_1, 1
.equ CLASS_YOUNGSTER, 2
.equ CLASS_LASS, 3
.equ CLASS_CAMPER, 4
.equ CLASS_PICNICKER, 5
.equ CLASS_BUG_CATCHER, 6
.equ CLASS_AROMA_LADY, 7
.equ CLASS_TWINS, 8
.equ CLASS_HIKER, 9
.equ CLASS_BATTLE_GIRL, 10
.equ CLASS_FISHERMAN, 11
.equ CLASS_CYCLIST_M, 12
.equ CLASS_CYCLIST_F, 13
.equ CLASS_BLACK_BELT, 14
.equ CLASS_ARTIST, 15
.equ CLASS_PKMN_BREEDER_M, 16
.equ CLASS_PKMN_BREEDER_F, 17
.equ CLASS_COWGIRL, 18
.equ CLASS_JOGGER, 19
.equ CLASS_POKEFAN_M, 20
.equ CLASS_POKEFAN_F, 21
.equ CLASS_POKE_KID, 22
.equ CLASS_IRIS_OLD, 23
.equ CLASS_ACE_TRAINER_M, 24
.equ CLASS_ACE_TRAINER_F, 25
.equ CLASS_WAITRESS, 26
.equ CLASS_VETERAN, 27
.equ CLASS_NINJA_BOY, 28
.equ CLASS_DRAGON_TAMER, 29
.equ CLASS_BIRD_KEEPER, 30
.equ CLASS_JUGGLER, 31
.equ CLASS_RICH_BOY, 32
.equ CLASS_LADY, 33
.equ CLASS_GENTLEMAN, 34
.equ CLASS_SOCIALITE, 35
.equ CLASS_BEAUTY, 36
.equ CLASS_COLLECTOR, 37
.equ CLASS_POLICEMAN, 38
.equ CLASS_PKMN_RANGER_M, 39
.equ CLASS_PKMN_RANGER_F, 40
.equ CLASS_SCIENTIST, 41
.equ CLASS_SWIMMER_M, 42
.equ CLASS_SWIMMER_F, 43
.equ CLASS_PLASMA_M, 44
.equ CLASS_PLASMA_F, 45
.equ CLASS_SAILOR, 46
.equ CLASS_TRAINER_N, 47
.equ CLASS_RUIN_MANIAC, 48
.equ CLASS_PSYCHIC_M, 49
.equ CLASS_PSYCHIC_F, 50
.equ CLASS_PI, 51
.equ CLASS_GUITARIST, 52
.equ CLASS_ACE_TRAINER_M_1, 53
.equ CLASS_ACE_TRAINER_F_1, 54
.equ CLASS_GALACTIC_M, 55
.equ CLASS_SKIER, 56
.equ CLASS_ROUGHNECK, 57
.equ CLASS_CLOWN, 58
.equ CLASS_WORKER, 59
.equ CLASS_SCHOOL_KID_M, 60
.equ CLASS_SCHOOL_KID_F, 61
.equ CLASS_GALACTIC_F, 62
.equ CLASS_BURGLAR, 63
.equ CLASS_FIREBREATHER, 64
.equ CLASS_BIKER, 65
.equ CLASS_VOLKNER, 66
.equ CLASS_FLINT, 67
.equ CLASS_POKE_MANIAC, 68
.equ CLASS_BIRD_KEEPER_1, 69
.equ CLASS_WHITNEY, 70
.equ CLASS_RANCHER, 71
.equ CLASS_MORTY, 72
.equ CLASS_PRYCE, 73
.equ CLASS_JASMINE, 74
.equ CLASS_CHUCK, 75
.equ CLASS_CLAIR, 76
.equ CLASS_TEACHER, 77
.equ CLASS_SUPER_NERD, 78
.equ CLASS_SAGE, 79
.equ CLASS_PARASOL_LADY, 80
.equ CLASS_WAITER, 81
.equ CLASS_MEDIUM, 82
.equ CLASS_CAMERAMAN, 83
.equ CLASS_REPORTER, 84
.equ CLASS_COLRESS, 85
.equ CLASS_STEVEN, 86
.equ CLASS_ELITE_FOUR_0, 87
.equ CLASS_ELITE_FOUR_1, 88
.equ CLASS_ELITE_FOUR_2, 89
.equ CLASS_PKMN_TRAINER_2, 90
.equ CLASS_PKMN_TRAINER_3, 91
.equ CLASS_PKMN_TRAINER_4, 92
.equ CLASS_PKMN_TRAINER_5, 93
.equ CLASS_PKMN_TRAINER_6, 94
.equ CLASS_PKMN_TRAINER_7, 95
.equ CLASS_PKMN_TRAINER_8, 96
.equ CLASS_DARKRAI, 97
.equ CLASS_BROCK, 98
.equ CLASS_ZINZOLIN, 99
.equ CLASS_GHETSIS, 100
.equ CLASS_GARDENIA, 101
.equ CLASS_ROXIE, 102
.equ CLASS_MARLON, 103
.equ CLASS_TATE_AND_LIZA, 104
.equ CLASS_BROKEN, 105
.equ CLASS_WINONA, 106
.equ CLASS_CYNTHIA_OPPONENT, 107
.equ CLASS_HANNAH, 108
.equ CLASS_RED, 109
.equ CLASS_BLUE, 110
.equ CLASS_ELDER, 111
.equ CLASS_ELITE_FOUR_3, 112
.equ CLASS_SCIENTIST_1, 113
.equ CLASS_JUPITER, 114
.equ CLASS_BOARDER, 115
.equ CLASS_CYRUS, 116
.equ CLASS_SATURN, 117
.equ CLASS_MARS, 118
.equ CLASS_IRIS, 119
.equ CLASS_MYSTERY_MAN, 120
.equ CLASS_DOUBLE_TEAM, 121
.equ CLASS_YOUNG_COUPLE, 122
.equ CLASS_PKMN_TRAINER_10, 123
.equ CLASS_ROCKET_BOSS, 124
.equ CLASS_PKMN_TRAINER_11, 125
.equ CLASS_CYNTHIA_PARTNER, 126
.equ CLASS_PKMN_TRAINER_13, 127
.equ CLASS_PKMN_TRAINER_14, 128

// trainer data flags

.equ TRAINER_DATA_TYPE_NOTHING, (0x00)
.equ TRAINER_DATA_TYPE_MOVES, (0x01)
.equ TRAINER_DATA_TYPE_ITEMS, (0x02)
.equ TRAINER_DATA_TYPE_ABILITY, (0x04)
.equ TRAINER_DATA_TYPE_BALL, (0x08)
.equ TRAINER_DATA_TYPE_IV_EV_SET, (0x10)
.equ TRAINER_DATA_TYPE_NATURE_SET, (0x20)
.equ TRAINER_DATA_TYPE_SHINY_LOCK, (0x40)

.equ TRAINER_DATA_TYPE_ADDITIONAL_FLAGS, (0x80)

// additional trainer data flags

.equ TRAINER_DATA_EXTRA_TYPE_NOTHING, 0x00
.equ TRAINER_DATA_EXTRA_TYPE_STATUS, 0x01
.equ TRAINER_DATA_EXTRA_TYPE_HP, 0x02
.equ TRAINER_DATA_EXTRA_TYPE_ATK, 0x04
.equ TRAINER_DATA_EXTRA_TYPE_DEF, 0x08
.equ TRAINER_DATA_EXTRA_TYPE_SPEED, 0x10
.equ TRAINER_DATA_EXTRA_TYPE_SP_ATK, 0x20
.equ TRAINER_DATA_EXTRA_TYPE_SP_DEF, 0x40
.equ TRAINER_DATA_EXTRA_TYPE_TYPES, 0x80
.equ TRAINER_DATA_EXTRA_TYPE_PP_COUNTS, 0x100
.equ TRAINER_DATA_EXTRA_TYPE_NICKNAME, 0x200

// trainer ai flags

.equ F_PRIORITIZE_SUPER_EFFECTIVE, (1 << 0)
.equ F_EVALUATE_ATTACKS, (1 << 1)
.equ F_EXPERT_ATTACKS, (1 << 2)
.equ F_PRIORITIZE_STATUS_MOVES, (1 << 3)
.equ F_RISKY_ATTACKS, (1 << 4)
.equ F_PRIORITIZE_DAMAGE, (1 << 5)
.equ F_MULTI_BATTLE_PARTNER, (1 << 6)
.equ F_DOUBLE_BATTLE, (1 << 7)
.equ F_PRIORITIZE_HEALING, (1 << 8)
.equ F_USE_WEATHER, (1 << 9)
.equ F_HARRASSMENT, (1 << 10)
.equ F_ROAMING_MON, (1 << 11)
.equ F_SAFARI_ZONE, (1 << 12)
.equ F_CATCHING_DEMO, (1 << 13)

// battle types

.equ SINGLE_BATTLE, 0
.equ DOUBLE_BATTLE, 2

// trainer text types

.equ TEXT_NOTICE_IN_OVERWORLD, 0
.equ TEXT_DEFEATED_IN_BATTLE, 1
.equ TEXT_DEFEATED_IN_OVERWORLD, 2
.equ TEXT_DOUBLE_NOTICE_IN_OVERWORLD_1, 3
.equ TEXT_DOUBLE_DEFEATED_IN_BATTLE_1, 4
.equ TEXT_DOUBLE_DEFEATED_IN_OVERWORLD_1, 5
.equ TEXT_DOUBLE_ONLY_1_POKEMON_1, 6
.equ TEXT_DOUBLE_NOTICE_IN_OVERWORLD_2, 7
.equ TEXT_DOUBLE_DEFEATED_IN_BATTLE_2, 8
.equ TEXT_DOUBLE_DEFEATED_IN_OVERWORLD_2, 9
.equ TEXT_DOUBLE_ONLY_1_POKEMON_2, 10
.equ TEXT_LAST_MON_CRITICAL, 15
.equ TEXT_LAST_MON_SENT_OUT, 16
.equ TEXT_REMATCH_IN_OVERWORLD, 17 // 17
.equ TEXT_REMATCH_IN_OVERWORLD_DOUBLE_1, 18 // 18
.equ TEXT_REMATCH_IN_OVERWORLD_DOUBLE_2, 19 // 19
.equ TEXT_PLAYER_LOSES, 20

// natures

.equ NATURE_HARDY,  (0)
.equ NATURE_LONELY,  (1)
.equ NATURE_BRAVE,  (2)
.equ NATURE_ADAMANT,  (3)
.equ NATURE_NAUGHTY,  (4)
.equ NATURE_BOLD,  (5)
.equ NATURE_DOCILE,  (6)
.equ NATURE_RELAXED,  (7)
.equ NATURE_IMPISH,  (8)
.equ NATURE_LAX,  (9)
.equ NATURE_TIMID, (10)
.equ NATURE_HASTY, (11)
.equ NATURE_SERIOUS, (12)
.equ NATURE_JOLLY, (13)
.equ NATURE_NAIVE, (14)
.equ NATURE_MODEST, (15)
.equ NATURE_MILD, (16)
.equ NATURE_QUIET, (17)
.equ NATURE_BASHFUL, (18)
.equ NATURE_RASH, (19)
.equ NATURE_CALM, (20)
.equ NATURE_GENTLE, (21)
.equ NATURE_SASSY, (22)
.equ NATURE_CAREFUL, (23)
.equ NATURE_QUIRKY, (24)

// letter defines for nicknames

.equ _0, 0x0121
.equ _1, 0x0122
.equ _2, 0x0123
.equ _3, 0x0124
.equ _4, 0x0125
.equ _5, 0x0126
.equ _6, 0x0127
.equ _7, 0x0128
.equ _8, 0x0129
.equ _9, 0x012A
.equ _A, 0x012B
.equ _B, 0x012C
.equ _C, 0x012D
.equ _D, 0x012E
.equ _E, 0x012F
.equ _F, 0x0130
.equ _G, 0x0131
.equ _H, 0x0132
.equ _I, 0x0133
.equ _J, 0x0134
.equ _K, 0x0135
.equ _L, 0x0136
.equ _M, 0x0137
.equ _N, 0x0138
.equ _O, 0x0139
.equ _P, 0x013A
.equ _Q, 0x013B
.equ _R, 0x013C
.equ _S, 0x013D
.equ _T, 0x013E
.equ _U, 0x013F
.equ _V, 0x0140
.equ _W, 0x0141
.equ _X, 0x0142
.equ _Y, 0x0143
.equ _Z, 0x0144
.equ _a_, 0x0145
.equ _b_, 0x0146
.equ _c_, 0x0147
.equ _d_, 0x0148
.equ _e_, 0x0149
.equ _f_, 0x014A
.equ _g_, 0x014B
.equ _h_, 0x014C
.equ _i_, 0x014D
.equ _j_, 0x014E
.equ _k_, 0x014F
.equ _l_, 0x0150
.equ _m_, 0x0151
.equ _n_, 0x0152
.equ _o_, 0x0153
.equ _p_, 0x0154
.equ _q_, 0x0155
.equ _r_, 0x0156
.equ _s_, 0x0157
.equ _t_, 0x0158
.equ _u_, 0x0159
.equ _v_, 0x015A
.equ _w_, 0x015B
.equ _x_, 0x015C
.equ _y_, 0x015D
.equ _z_, 0x015E
.equ _endstr, 0xFFFF

// Vanilla Terrains (Fields)

.equ TERRAIN_PLAIN, (0)
.equ TERRAIN_SAND, (1)
.equ TERRAIN_GRASS, (2)
.equ TERRAIN_PUDDLE, (3)
.equ TERRAIN_MOUNTAIN, (4)
.equ TERRAIN_CAVE, (5)
.equ TERRAIN_SNOW, (6)
.equ TERRAIN_WATER, (7)
.equ TERRAIN_ICE, (8)
.equ TERRAIN_BUILDING, (9)
.equ TERRAIN_GREAT_MARSH, (10)  // unused
.equ TERRAIN_UNKNOWN, (11)   // unused
.equ TERRAIN_WILL, (12)
.equ TERRAIN_KOGA, (13)
.equ TERRAIN_BRUNO, (14)
.equ TERRAIN_KAREN, (15)
.equ TERRAIN_LANCE, (16)
.equ TERRAIN_DISTORTION_WORLD, (17)  // unused
.equ TERRAIN_BATTLE_TOWER, (18)
.equ TERRAIN_BATTLE_FACTORY, (29)
.equ TERRAIN_BATTLE_ARCADE, (20)
.equ TERRAIN_BATTLE_CASTLE, (21)
.equ TERRAIN_BATTLE_HALL, (22)
.equ TERRAIN_GIRATINA, (23)  // unused
.equ TERRAIN_MAX, (24)
.equ TERRAIN_CURRENT, -1

// Terrain Overlays

.equ TERRAIN_NONE, (0)
.equ GRASSY_TERRAIN, (1)
.equ MISTY_TERRAIN, (2)
.equ ELECTRIC_TERRAIN, (3)
.equ PSYCHIC_TERRAIN, (4)

// battle backgrounds

.equ BATTLE_BG_GENERAL, 0
.equ BATTLE_BG_OCEAN, 1
.equ BATTLE_BG_CITY, 2
.equ BATTLE_BG_FOREST, 3
.equ BATTLE_BG_MOUNTAIN, 4
.equ BATTLE_BG_SNOW, 5
.equ BATTLE_BG_BUILDING_1, 6
.equ BATTLE_BG_BUILDING_2, 7
.equ BATTLE_BG_BUILDING_3, 8
.equ BATTLE_BG_CAVE_1, 9
.equ BATTLE_BG_CAVE_2, 10
.equ BATTLE_BG_CAVE_3, 11
.equ BATTLE_BG_WILL, 12
.equ BATTLE_BG_KOGA, 13
.equ BATTLE_BG_BRUNO, 14
.equ BATTLE_BG_KAREN, 15
.equ BATTLE_BG_LANCE, 16
.equ BATTLE_BG_DISTORTION_WORLD, 17
.equ BATTLE_BG_BATTLE_TOWER, 18
.equ BATTLE_BG_BATTLE_FACTORY, 19
.equ BATTLE_BG_BATTLE_ARCADE, 20
.equ BATTLE_BG_BATTLE_CASTLE, 21
.equ BATTLE_BG_BATTLE_HALL, 22
.equ BATTLE_BG_ELECTRIC_TERRAIN, 23
.equ BATTLE_BG_MISTY_TERRAIN, 24
.equ BATTLE_BG_GRASSY_TERRAIN, 25
.equ BATTLE_BG_PSYCHIC_TERRAIN, 26
.equ BATTLE_BG_CURRENT, -1

.equ FALSE, (0)
.equ TRUE, (1)
