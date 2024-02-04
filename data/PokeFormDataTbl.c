#include "../include/types.h"
#include "../include/config.h"
#include "../include/pokemon.h"
#include "../include/constants/species.h"


// when editing, make sure to hit up src/pokemon.c and edit NELEMS_POKEFORMDATATBL too
const struct FormData UNUSED PokeFormDataTbl[]=
{
#ifdef MEGA_EVOLUTIONS
    {
        .species = SPECIES_VENUSAUR,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_VENUSAUR,
    },
    {
        .species = SPECIES_CHARIZARD,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_CHARIZARD_X,
    },
    {
        .species = SPECIES_CHARIZARD,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_MEGA_CHARIZARD_Y,
    },
    {
        .species = SPECIES_BLASTOISE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_BLASTOISE,
    },
    {
        .species = SPECIES_BEEDRILL,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_BEEDRILL,
    },
    {
        .species = SPECIES_PIDGEOT,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_PIDGEOT,
    },
    {
        .species = SPECIES_ALAKAZAM,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_ALAKAZAM,
    },
    {
        .species = SPECIES_SLOWBRO,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_SLOWBRO,
    },
    {
        .species = SPECIES_GENGAR,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_GENGAR,
    },
    {
        .species = SPECIES_KANGASKHAN,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_KANGASKHAN,
    },
    {
        .species = SPECIES_PINSIR,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_PINSIR,
    },
    {
        .species = SPECIES_GYARADOS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_GYARADOS,
    },
    {
        .species = SPECIES_AERODACTYL,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_AERODACTYL,
    },
    {
        .species = SPECIES_MEWTWO,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_MEWTWO_X,
    },
    {
        .species = SPECIES_MEWTWO,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_MEGA_MEWTWO_Y,
    },
    {
        .species = SPECIES_AMPHAROS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_AMPHAROS,
    },
    {
        .species = SPECIES_STEELIX,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_STEELIX,
    },
    {
        .species = SPECIES_SCIZOR,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_SCIZOR,
    },
    {
        .species = SPECIES_HERACROSS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_HERACROSS,
    },
    {
        .species = SPECIES_HOUNDOOM,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_HOUNDOOM,
    },
    {
        .species = SPECIES_TYRANITAR,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_TYRANITAR,
    },
    {
        .species = SPECIES_SCEPTILE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_SCEPTILE,
    },
    {
        .species = SPECIES_BLAZIKEN,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_BLAZIKEN,
    },
    {
        .species = SPECIES_SWAMPERT,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_SWAMPERT,
    },
    {
        .species = SPECIES_GARDEVOIR,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_GARDEVOIR,
    },
    {
        .species = SPECIES_SABLEYE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_SABLEYE,
    },
    {
        .species = SPECIES_MAWILE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_MAWILE,
    },
    {
        .species = SPECIES_AGGRON,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_AGGRON,
    },
    {
        .species = SPECIES_MEDICHAM,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_MEDICHAM,
    },
    {
        .species = SPECIES_MANECTRIC,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_MANECTRIC,
    },
    {
        .species = SPECIES_SHARPEDO,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_SHARPEDO,
    },
    {
        .species = SPECIES_CAMERUPT,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_CAMERUPT,
    },
    {
        .species = SPECIES_ALTARIA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_ALTARIA,
    },
    {
        .species = SPECIES_BANETTE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_BANETTE,
    },
    {
        .species = SPECIES_ABSOL,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_ABSOL,
    },
    {
        .species = SPECIES_GLALIE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_GLALIE,
    },
    {
        .species = SPECIES_SALAMENCE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_SALAMENCE,
    },
    {
        .species = SPECIES_METAGROSS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_METAGROSS,
    },
    {
        .species = SPECIES_LATIAS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_LATIAS,
    },
    {
        .species = SPECIES_LATIOS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_LATIOS,
    },
    {
        .species = SPECIES_RAYQUAZA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_RAYQUAZA,
    },
    {
        .species = SPECIES_LOPUNNY,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_LOPUNNY,
    },
    {
        .species = SPECIES_GARCHOMP,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_GARCHOMP,
    },
    {
        .species = SPECIES_LUCARIO,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_LUCARIO,
    },
    {
        .species = SPECIES_ABOMASNOW,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_ABOMASNOW,
    },
    {
        .species = SPECIES_GALLADE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_GALLADE,
    },
    {
        .species = SPECIES_AUDINO,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_AUDINO,
    },
    {
        .species = SPECIES_DIANCIE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEGA_DIANCIE,
    },
#endif // MEGA_EVOLUTIONS

#ifdef PRIMAL_REVERSION
    {
        .species = SPECIES_KYOGRE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_KYOGRE_PRIMAL,
    },
    {
        .species = SPECIES_GROUDON,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_GROUDON_PRIMAL,
    },
#endif // PRIMAL_REVERSION

    /**alolan forms**/
    {
        .species = SPECIES_RATTATA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_RATTATA_ALOLAN,
    },
    {
        .species = SPECIES_RATICATE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_RATICATE_ALOLAN,
    },
    {
        .species = SPECIES_RAICHU,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_RAICHU_ALOLAN,
    },
    {
        .species = SPECIES_SANDSHREW,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_SANDSHREW_ALOLAN,
    },
    {
        .species = SPECIES_SANDSLASH,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_SANDSLASH_ALOLAN,
    },
    {
        .species = SPECIES_VULPIX,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_VULPIX_ALOLAN,
    },
    {
        .species = SPECIES_NINETALES,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_NINETALES_ALOLAN,
    },
    {
        .species = SPECIES_DIGLETT,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_DIGLETT_ALOLAN,
    },
    {
        .species = SPECIES_DUGTRIO,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_DUGTRIO_ALOLAN,
    },
    {
        .species = SPECIES_MEOWTH,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEOWTH_ALOLAN,
    },
    {
        .species = SPECIES_PERSIAN,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_PERSIAN_ALOLAN,
    },
    {
        .species = SPECIES_GEODUDE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_GEODUDE_ALOLAN,
    },
    {
        .species = SPECIES_GRAVELER,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_GRAVELER_ALOLAN,
    },
    {
        .species = SPECIES_GOLEM,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_GOLEM_ALOLAN,
    },
    {
        .species = SPECIES_GRIMER,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_GRIMER_ALOLAN,
    },
    {
        .species = SPECIES_MUK,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MUK_ALOLAN,
    },
    {
        .species = SPECIES_EXEGGUTOR,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_EXEGGUTOR_ALOLAN,
    },
    {
        .species = SPECIES_MAROWAK,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MAROWAK_ALOLAN,
    },

    /**galarian forms**/
    {
        .species = SPECIES_MEOWTH,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_MEOWTH_GALARIAN,
    },
    {
        .species = SPECIES_PONYTA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_PONYTA_GALARIAN,
    },
    {
        .species = SPECIES_RAPIDASH,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_RAPIDASH_GALARIAN,
    },
    {
        .species = SPECIES_SLOWPOKE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_SLOWPOKE_GALARIAN,
    },
    { // slowpoke is really fun because slowbro has two forms so we have this duplicate entry here to map it properly during the evolution sequence
        .species = SPECIES_SLOWPOKE,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_SLOWPOKE_GALARIAN,
    },
    {
        .species = SPECIES_SLOWBRO,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_SLOWBRO_GALARIAN,
    },
    {
        .species = SPECIES_FARFETCHD,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_FARFETCHD_GALARIAN,
    },
    {
        .species = SPECIES_WEEZING,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_WEEZING_GALARIAN,
    },
    {
        .species = SPECIES_MR_MIME,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MR_MIME_GALARIAN,
    },
    {
        .species = SPECIES_ARTICUNO,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ARTICUNO_GALARIAN,
    },
    {
        .species = SPECIES_ZAPDOS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ZAPDOS_GALARIAN,
    },
    {
        .species = SPECIES_MOLTRES,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MOLTRES_GALARIAN,
    },
    {
        .species = SPECIES_SLOWKING,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_SLOWKING_GALARIAN,
    },
    {
        .species = SPECIES_CORSOLA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_CORSOLA_GALARIAN,
    },
    {
        .species = SPECIES_ZIGZAGOON,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ZIGZAGOON_GALARIAN,
    },
    {
        .species = SPECIES_LINOONE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_LINOONE_GALARIAN,
    },
    {
        .species = SPECIES_DARUMAKA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_DARUMAKA_GALARIAN,
    },
    {
        .species = SPECIES_DARMANITAN,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_DARMANITAN_GALARIAN,
    },
    {
        .species = SPECIES_YAMASK,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_YAMASK_GALARIAN,
    },
    {
        .species = SPECIES_STUNFISK,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_STUNFISK_GALARIAN,
    },

    /**cosmetic forms**/
    {
        .species = SPECIES_PIKACHU,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_COSPLAY,
    },
    {
        .species = SPECIES_PIKACHU,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_ROCK_STAR,
    },
    {
        .species = SPECIES_PIKACHU,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_BELLE,
    },
    {
        .species = SPECIES_PIKACHU,
        .form_no = 4,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_POP_STAR,
    },
    {
        .species = SPECIES_PIKACHU,
        .form_no = 5,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_PH_D,
    },
    {
        .species = SPECIES_PIKACHU,
        .form_no = 6,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_LIBRE,
    },
    {
        .species = SPECIES_PIKACHU,
        .form_no = 7,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_ORIGINAL_CAP,
    },
    {
        .species = SPECIES_PIKACHU,
        .form_no = 8,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_HOENN_CAP,
    },
    {
        .species = SPECIES_PIKACHU,
        .form_no = 9,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_SINNOH_CAP,
    },
    {
        .species = SPECIES_PIKACHU,
        .form_no = 10,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_UNOVA_CAP,
    },
    {
        .species = SPECIES_PIKACHU,
        .form_no = 11,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_KALOS_CAP,
    },
    {
        .species = SPECIES_PIKACHU,
        .form_no = 12,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_ALOLA_CAP,
    },
    {
        .species = SPECIES_PIKACHU,
        .form_no = 13,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_PARTNER_CAP,
    },
    {
        .species = SPECIES_PIKACHU,
        .form_no = 14,
        .need_rev = 0,
        .file = SPECIES_PIKACHU_WORLD_CAP,
    },
    {
        .species = SPECIES_BASCULIN,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_BASCULIN_RED_STRIPED,
    },
    {
        .species = SPECIES_BASCULIN,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_BASCULIN_BLUE_STRIPED,
    },
    {
        .species = SPECIES_DEERLING,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_DEERLING_SUMMER,
    },
    {
        .species = SPECIES_DEERLING,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_DEERLING_AUTUMN,
    },
    {
        .species = SPECIES_DEERLING,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_DEERLING_WINTER,
    },
    {
        .species = SPECIES_SAWSBUCK,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_SAWSBUCK_SUMMER,
    },
    {
        .species = SPECIES_SAWSBUCK,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_SAWSBUCK_AUTUMN,
    },
    {
        .species = SPECIES_SAWSBUCK,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_SAWSBUCK_WINTER,
    },
    {
        .species = SPECIES_TORNADUS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_TORNADUS_THERIAN,
    },
    {
        .species = SPECIES_THUNDURUS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_THUNDURUS_THERIAN,
    },
    {
        .species = SPECIES_LANDORUS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_LANDORUS_THERIAN,
    },
    {
        .species = SPECIES_KYUREM,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_KYUREM_WHITE,
    },
    {
        .species = SPECIES_KYUREM,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_KYUREM_BLACK,
    },
    {
        .species = SPECIES_KELDEO,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_KELDEO_RESOLUTE,
    },
    {
        .species = SPECIES_GENESECT,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_GENESECT_DOUSE_DRIVE,
    },
    {
        .species = SPECIES_GENESECT,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_GENESECT_SHOCK_DRIVE,
    },
    {
        .species = SPECIES_GENESECT,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_GENESECT_BURN_DRIVE,
    },
    {
        .species = SPECIES_GENESECT,
        .form_no = 4,
        .need_rev = 0,
        .file = SPECIES_GENESECT_CHILL_DRIVE,
    },
    {
        .species = SPECIES_GRENINJA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_GRENINJA_BATTLE_BOND,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_POLAR,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_TUNDRA,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_CONTINENTAL,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 4,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_GARDEN,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 5,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_ELEGANT,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 6,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_MEADOW,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 7,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_MODERN,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 8,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_MARINE,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 9,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_ARCHIPELAGO,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 10,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_HIGH_PLAINS,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 11,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_SANDSTORM,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 12,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_RIVER,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 13,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_MONSOON,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 14,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_SAVANNA,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 15,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_SUN,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 16,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_OCEAN,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 17,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_JUNGLE,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 18,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_FANCY,
    },
    {
        .species = SPECIES_VIVILLON,
        .form_no = 19,
        .need_rev = 0,
        .file = SPECIES_VIVILLON_POKE_BALL,
    },
    {
        .species = SPECIES_FLABEBE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_FLABEBE_YELLOW_FLOWER,
    },
    {
        .species = SPECIES_FLABEBE,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_FLABEBE_ORANGE_FLOWER,
    },
    {
        .species = SPECIES_FLABEBE,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_FLABEBE_BLUE_FLOWER,
    },
    {
        .species = SPECIES_FLABEBE,
        .form_no = 4,
        .need_rev = 0,
        .file = SPECIES_FLABEBE_WHITE_FLOWER,
    },
    {
        .species = SPECIES_FLOETTE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_FLOETTE_YELLOW_FLOWER,
    },
    {
        .species = SPECIES_FLOETTE,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_FLOETTE_ORANGE_FLOWER,
    },
    {
        .species = SPECIES_FLOETTE,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_FLOETTE_BLUE_FLOWER,
    },
    {
        .species = SPECIES_FLOETTE,
        .form_no = 4,
        .need_rev = 0,
        .file = SPECIES_FLOETTE_WHITE_FLOWER,
    },
    {
        .species = SPECIES_FLOETTE,
        .form_no = 5,
        .need_rev = 0,
        .file = SPECIES_FLOETTE_ETERNAL_FLOWER,
    },
    {
        .species = SPECIES_FLORGES,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_FLORGES_YELLOW_FLOWER,
    },
    {
        .species = SPECIES_FLORGES,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_FLORGES_ORANGE_FLOWER,
    },
    {
        .species = SPECIES_FLORGES,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_FLORGES_BLUE_FLOWER,
    },
    {
        .species = SPECIES_FLORGES,
        .form_no = 4,
        .need_rev = 0,
        .file = SPECIES_FLORGES_WHITE_FLOWER,
    },
    {
        .species = SPECIES_FURFROU,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_FURFROU_HEART,
    },
    {
        .species = SPECIES_FURFROU,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_FURFROU_STAR,
    },
    {
        .species = SPECIES_FURFROU,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_FURFROU_DIAMOND,
    },
    {
        .species = SPECIES_FURFROU,
        .form_no = 4,
        .need_rev = 0,
        .file = SPECIES_FURFROU_DEBUTANTE,
    },
    {
        .species = SPECIES_FURFROU,
        .form_no = 5,
        .need_rev = 0,
        .file = SPECIES_FURFROU_MATRON,
    },
    {
        .species = SPECIES_FURFROU,
        .form_no = 6,
        .need_rev = 0,
        .file = SPECIES_FURFROU_DANDY,
    },
    {
        .species = SPECIES_FURFROU,
        .form_no = 7,
        .need_rev = 0,
        .file = SPECIES_FURFROU_LA_REINE,
    },
    {
        .species = SPECIES_FURFROU,
        .form_no = 8,
        .need_rev = 0,
        .file = SPECIES_FURFROU_KABUKI,
    },
    {
        .species = SPECIES_FURFROU,
        .form_no = 9,
        .need_rev = 0,
        .file = SPECIES_FURFROU_PHARAOH,
    },
    {
        .species = SPECIES_PUMPKABOO,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_PUMPKABOO_SMALL,
    },
    {
        .species = SPECIES_PUMPKABOO,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_PUMPKABOO_LARGE,
    },
    {
        .species = SPECIES_PUMPKABOO,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_PUMPKABOO_SUPER,
    },
    {
        .species = SPECIES_GOURGEIST,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_GOURGEIST_SMALL,
    },
    {
        .species = SPECIES_GOURGEIST,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_GOURGEIST_LARGE,
    },
    {
        .species = SPECIES_GOURGEIST,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_GOURGEIST_SUPER,
    },
    {
        .species = SPECIES_HOOPA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_HOOPA_UNBOUND,
    },
    {
        .species = SPECIES_ORICORIO,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ORICORIO_POM_POM,
    },
    {
        .species = SPECIES_ORICORIO,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_ORICORIO_PAU,
    },
    {
        .species = SPECIES_ORICORIO,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_ORICORIO_SENSU,
    },
    {
        .species = SPECIES_ROCKRUFF,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ROCKRUFF_OWN_TEMPO,
    },
    {
        .species = SPECIES_LYCANROC,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_LYCANROC_MIDNIGHT,
    },
    {
        .species = SPECIES_LYCANROC,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_LYCANROC_DUSK,
    },
    {
        .species = SPECIES_MAGEARNA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MAGEARNA_ORIGINAL,
    },
    {
        .species = SPECIES_TOXTRICITY,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_TOXTRICITY_LOW_KEY,
    },
    {
        .species = SPECIES_SINISTEA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_SINISTEA_ANTIQUE,
    },
    {
        .species = SPECIES_POLTEAGEIST,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_POLTEAGEIST_ANTIQUE,
    },
    {
        .species = SPECIES_ALCREMIE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ALCREMIE_BERRY_SWEET,
    },
    {
        .species = SPECIES_ALCREMIE,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_ALCREMIE_LOVE_SWEET,
    },
    {
        .species = SPECIES_ALCREMIE,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_ALCREMIE_STAR_SWEET,
    },
    {
        .species = SPECIES_ALCREMIE,
        .form_no = 4,
        .need_rev = 0,
        .file = SPECIES_ALCREMIE_CLOVER_SWEET,
    },
    {
        .species = SPECIES_ALCREMIE,
        .form_no = 5,
        .need_rev = 0,
        .file = SPECIES_ALCREMIE_FLOWER_SWEET,
    },
    {
        .species = SPECIES_ALCREMIE,
        .form_no = 6,
        .need_rev = 0,
        .file = SPECIES_ALCREMIE_RIBBON_SWEET,
    },
    {
        .species = SPECIES_URSHIFU,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_URSHIFU_RAPID_STRIKE,
    },
    {
        .species = SPECIES_ZARUDE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ZARUDE_DADA,
    },
    {
        .species = SPECIES_CALYREX,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_CALYREX_ICE_RIDER,
    },
    {
        .species = SPECIES_CALYREX,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_CALYREX_SHADOW_RIDER,
    },

    /**Battle Forms**/
    {
        .species = SPECIES_CASTFORM,
        .form_no = 1,
        .need_rev = 1,
        .file = SPECIES_CASTFORM_SUNNY,
    },
    {
        .species = SPECIES_CASTFORM,
        .form_no = 2,
        .need_rev = 1,
        .file = SPECIES_CASTFORM_RAINY,
    },
    {
        .species = SPECIES_CASTFORM,
        .form_no = 3,
        .need_rev = 1,
        .file = SPECIES_CASTFORM_SNOWY,
    },
    {
        .species = SPECIES_CHERRIM,
        .form_no = 1,
        .need_rev = 1,
        .file = SPECIES_CHERRIM_SUNSHINE,
    },
    {
        .species = SPECIES_SHELLOS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_SHELLOS_EAST_SEA,
    },
    {
        .species = SPECIES_GASTRODON,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_GASTRODON_EAST_SEA,
    },
    {
        .species = SPECIES_DIALGA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_DIALGA_ORIGIN,
    },
    {
        .species = SPECIES_PALKIA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_PALKIA_ORIGIN,
    },
    {
        .species = SPECIES_DARMANITAN,
        .form_no = 2,
        .need_rev = 1,
        .file = SPECIES_DARMANITAN_ZEN_MODE,
    },
    {
        .species = SPECIES_DARMANITAN,
        .form_no = 3,
        .need_rev = 1,
        .file = SPECIES_DARMANITAN_ZEN_MODE_GALARIAN,
    },
    {
        .species = SPECIES_MELOETTA,
        .form_no = 1,
        .need_rev = 1,
        .file = SPECIES_MELOETTA_PIROUETTE,
    },
    {
        .species = SPECIES_GRENINJA,
        .form_no = 2,
        .need_rev = 1,
        .file = SPECIES_GRENINJA_ASH,
    },
    {
        .species = SPECIES_AEGISLASH,
        .form_no = 1,
        .need_rev = 1,
        .file = SPECIES_AEGISLASH_BLADE,
    },
    {
        .species = SPECIES_XERNEAS,
        .form_no = 1,
        .need_rev = 1,
        .file = SPECIES_XERNEAS_ACTIVE,
    },
    {
        .species = SPECIES_ZYGARDE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ZYGARDE_10,
    },
    {
        .species = SPECIES_ZYGARDE,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_ZYGARDE_10_POWER_CONSTRUCT,
    },
    {
        .species = SPECIES_ZYGARDE,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_ZYGARDE_50_POWER_CONSTRUCT,
    },
    {
        .species = SPECIES_ZYGARDE,
        .form_no = 4,
        .need_rev = 1,
        .file = SPECIES_ZYGARDE_10_COMPLETE,
    },
    {
        .species = SPECIES_ZYGARDE,
        .form_no = 5,
        .need_rev = 1,
        .file = SPECIES_ZYGARDE_50_COMPLETE,
    },
    {
        .species = SPECIES_WISHIWASHI,
        .form_no = 1,
        .need_rev = 1,
        .file = SPECIES_WISHIWASHI_SCHOOL,
    },
    {
        .species = SPECIES_MINIOR,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MINIOR_METEOR_ORANGE,
    },
    {
        .species = SPECIES_MINIOR,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_MINIOR_METEOR_YELLOW,
    },
    {
        .species = SPECIES_MINIOR,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_MINIOR_METEOR_GREEN,
    },
    {
        .species = SPECIES_MINIOR,
        .form_no = 4,
        .need_rev = 0,
        .file = SPECIES_MINIOR_METEOR_BLUE,
    },
    {
        .species = SPECIES_MINIOR,
        .form_no = 5,
        .need_rev = 0,
        .file = SPECIES_MINIOR_METEOR_INDIGO,
    },
    {
        .species = SPECIES_MINIOR,
        .form_no = 6,
        .need_rev = 0,
        .file = SPECIES_MINIOR_METEOR_VIOLET,
    },
    {
        .species = SPECIES_MINIOR,
        .form_no = 7,
        .need_rev = 1,
        .file = SPECIES_MINIOR_CORE_RED,
    },
    {
        .species = SPECIES_MINIOR,
        .form_no = 8,
        .need_rev = 1,
        .file = SPECIES_MINIOR_CORE_ORANGE,
    },
    {
        .species = SPECIES_MINIOR,
        .form_no = 9,
        .need_rev = 1,
        .file = SPECIES_MINIOR_CORE_YELLOW,
    },
    {
        .species = SPECIES_MINIOR,
        .form_no = 10,
        .need_rev = 1,
        .file = SPECIES_MINIOR_CORE_GREEN,
    },
    {
        .species = SPECIES_MINIOR,
        .form_no = 11,
        .need_rev = 1,
        .file = SPECIES_MINIOR_CORE_BLUE,
    },
    {
        .species = SPECIES_MINIOR,
        .form_no = 12,
        .need_rev = 1,
        .file = SPECIES_MINIOR_CORE_INDIGO,
    },
    {
        .species = SPECIES_MINIOR,
        .form_no = 13,
        .need_rev = 1,
        .file = SPECIES_MINIOR_CORE_VIOLET,
    },
    {
        .species = SPECIES_MIMIKYU,
        .form_no = 1,
        .need_rev = 1,
        .file = SPECIES_MIMIKYU_BUSTED,
    },
    {
        .species = SPECIES_NECROZMA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_NECROZMA_DUSK_MANE,
    },
    {
        .species = SPECIES_NECROZMA,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_NECROZMA_DAWN_WINGS,
    },
    {
        .species = SPECIES_NECROZMA,
        .form_no = 3,
        .need_rev = 1,
        .file = SPECIES_NECROZMA_ULTRA_DUSK_MANE,
    },
    {
        .species = SPECIES_NECROZMA,
        .form_no = 4,
        .need_rev = 1,
        .file = SPECIES_NECROZMA_ULTRA_DAWN_WINGS,
    },
    {
        .species = SPECIES_CRAMORANT,
        .form_no = 1,
        .need_rev = 1,
        .file = SPECIES_CRAMORANT_GULPING,
    },
    {
        .species = SPECIES_CRAMORANT,
        .form_no = 2,
        .need_rev = 1,
        .file = SPECIES_CRAMORANT_GORGING,
    },
    {
        .species = SPECIES_EISCUE,
        .form_no = 1,
        .need_rev = 1,
        .file = SPECIES_EISCUE_NOICE_FACE,
    },
    {
        .species = SPECIES_MORPEKO,
        .form_no = 1,
        .need_rev = 1,
        .file = SPECIES_MORPEKO_HANGRY,
    },
    {
        .species = SPECIES_ZACIAN,
        .form_no = 1,
        .need_rev = 1,
        .file = SPECIES_ZACIAN_CROWNED,
    },
    {
        .species = SPECIES_ZAMAZENTA,
        .form_no = 1,
        .need_rev = 1,
        .file = SPECIES_ZAMAZENTA_CROWNED,
    },
    {
        .species = SPECIES_ETERNATUS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ETERNATUS_ETERNAMAX,
    },
    {
        .species = SPECIES_ENAMORUS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ENAMORUS_THERIAN,
    },

    /**hisuian forms**/
    {
        .species = SPECIES_GROWLITHE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_GROWLITHE_HISUIAN,
    },
    {
        .species = SPECIES_ARCANINE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ARCANINE_HISUIAN,
    },
    {
        .species = SPECIES_VOLTORB,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_VOLTORB_HISUIAN,
    },
    {
        .species = SPECIES_ELECTRODE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ELECTRODE_HISUIAN,
    },
    {
        .species = SPECIES_TYPHLOSION,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_TYPHLOSION_HISUIAN,
    },
    {
        .species = SPECIES_QWILFISH,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_QWILFISH_HISUIAN,
    },
    {
        .species = SPECIES_SNEASEL,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_SNEASEL_HISUIAN,
    },
    {
        .species = SPECIES_SAMUROTT,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_SAMUROTT_HISUIAN,
    },
    {
        .species = SPECIES_LILLIGANT,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_LILLIGANT_HISUIAN,
    },
    {
        .species = SPECIES_ZORUA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ZORUA_HISUIAN,
    },
    {
        .species = SPECIES_ZOROARK,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_ZOROARK_HISUIAN,
    },
    {
        .species = SPECIES_BRAVIARY,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_BRAVIARY_HISUIAN,
    },
    {
        .species = SPECIES_SLIGGOO,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_SLIGGOO_HISUIAN,
    },
    {
        .species = SPECIES_GOODRA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_GOODRA_HISUIAN,
    },
    {
        .species = SPECIES_AVALUGG,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_AVALUGG_HISUIAN,
    },
    {
        .species = SPECIES_DECIDUEYE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_DECIDUEYE_HISUIAN,
    },

    /**significant gender differences**/
    {
        .species = SPECIES_UNFEZANT,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_UNFEZANT_FEMALE,
    },
    {
        .species = SPECIES_FRILLISH,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_FRILLISH_FEMALE,
    },
    {
        .species = SPECIES_JELLICENT,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_JELLICENT_FEMALE
    },
    {
        .species = SPECIES_PYROAR,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_PYROAR_FEMALE
    },
    {
        .species = SPECIES_MEOWSTIC,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MEOWSTIC_FEMALE,
    },
    {
        .species = SPECIES_INDEEDEE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_INDEEDEE_FEMALE,
    },
    {
        .species = SPECIES_BASCULEGION,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_BASCULEGION_FEMALE,
    },
    {
        .species = SPECIES_MAUSHOLD,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_MAUSHOLD_FAMILY_OF_THREE,
    },
    {
        .species = SPECIES_SQUAWKABILLY,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_SQUAWKABILLY_BLUE_PLUMAGE,
    },
    {
        .species = SPECIES_SQUAWKABILLY,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_SQUAWKABILLY_YELLOW_PLUMAGE,
    },
    {
        .species = SPECIES_SQUAWKABILLY,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_SQUAWKABILLY_WHITE_PLUMAGE,
    },
    {
        .species = SPECIES_PALAFIN,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_PALAFIN_HERO,
    },
    {
        .species = SPECIES_TATSUGIRI,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_TATSUGIRI_DROOPY,
    },
    {
        .species = SPECIES_TATSUGIRI,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_TATSUGIRI_STRETCHY,
    },
    {
        .species = SPECIES_DUDUNSPARCE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_DUDUNSPARCE_THREE_SEGMENT,
    },
    {
        .species = SPECIES_GIMMIGHOUL,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_GIMMIGHOUL_ROAMING,
    },
    {
        .species = SPECIES_TERAPAGOS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_TERAPAGOS_TERASTAL,
    },
    {
        .species = SPECIES_TERAPAGOS,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_TERAPAGOS_STELLAR,
    },
    {
        .species = SPECIES_WOOPER,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_WOOPER_PALDEAN,
    },
    {
        .species = SPECIES_TAUROS,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_TAUROS_COMBAT,
    },
    {
        .species = SPECIES_TAUROS,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_TAUROS_BLAZE,
    },
    {
        .species = SPECIES_TAUROS,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_TAUROS_AQUA,
    },
    {
        .species = SPECIES_OINKOLOGNE,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_OINKOLOGNE_FEMALE,
    },
    {
        .species = SPECIES_REVAVROOM,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_REVAVROOM_SEGIN,
    },
    {
        .species = SPECIES_REVAVROOM,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_REVAVROOM_SCHEDAR,
    },
    {
        .species = SPECIES_REVAVROOM,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_REVAVROOM_NAVI,
    },
    {
        .species = SPECIES_REVAVROOM,
        .form_no = 4,
        .need_rev = 0,
        .file = SPECIES_REVAVROOM_RUCHBAH,
    },
    {
        .species = SPECIES_REVAVROOM,
        .form_no = 5,
        .need_rev = 0,
        .file = SPECIES_REVAVROOM_CAPH,
    },
    {
        .species = SPECIES_POLTCHAGEIST,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_POLTCHAGEIST_MASTERPIECE,
    },
    {
        .species = SPECIES_SINISTCHA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_SINISTCHA_MASTERPIECE,
    },
    {
        .species = SPECIES_OGERPON,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_OGERPON_WELLSPRING_MASK,
    },
    {
        .species = SPECIES_OGERPON,
        .form_no = 2,
        .need_rev = 0,
        .file = SPECIES_OGERPON_HEARTHFLAME_MASK,
    },
    {
        .species = SPECIES_OGERPON,
        .form_no = 3,
        .need_rev = 0,
        .file = SPECIES_OGERPON_CORNERSTONE_MASK,
    },
    {
        .species = SPECIES_URSALUNA,
        .form_no = 1,
        .need_rev = 0,
        .file = SPECIES_URSALUNA_BLOODMOON,
    },
};
