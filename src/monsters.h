#ifndef MONSTERS_H
#define MONSTERS_H

#include <stdint.h>

// a szörnyflagek közül azok, amelyek a típustáblában is szerepelnek; a táblában egy bájton tárolódnak
#define MONSTER_MEAN       0x01                 // belépéskor felébredhet (ISMEAN)
#define MONSTER_FLYING     0x02                 // a játékostól távol kétszer lép (ISFLY)
#define MONSTER_GREEDY     0x04                 // előbb a szobabeli aranyra megy (ISGREED)
#define MONSTER_INVISIBLE  0x08                 // csak láthatatlan-látással látszik (ISINVIS)

#define MONSTER_TYPE_FLAGS  (MONSTER_MEAN | MONSTER_FLYING | MONSTER_GREEDY | MONSTER_INVISIBLE)

#if MONSTER_TYPE_FLAGS > 0xff
    #error "a típustábla flagjei nem férnek el egy bájton"
#endif

#define MONSTER_TYPE_COUNT  26                  // 'A'..'Z'

#define MONSTER_STRENGTH  10                    // minden szörny ereje; nem tárolódik

#define DAMAGE_SEGMENTS_MAX  3

// a Venus flytrap sebzésének kockaszáma helyett: a támadás pillanatában a vf_hit
#define DAMAGE_DICE_FLYTRAP  0xff

// egy sebzés-szegmens: dice darab sides oldalú kocka; a (0, 0) is valódi szegmens, egy külön dobás a találatra
struct DamageSegment {
    uint8_t dice;
    uint8_t sides;
};

struct MonsterType {
    uint8_t carry_chance;                       // ennyi százalék eséllyel kap tárgyat, és ennyivel keres tárgyat a szobájában
    uint8_t flags;                              // MONSTER_TYPE_FLAGS bitjei
    uint8_t experience_level;
    int8_t  armor;
    uint16_t experience;                        // ennyi XP-t ad a mélységi módosítók előtt
    uint8_t damage_segment_count;               // 1..DAMAGE_SEGMENTS_MAX
    struct DamageSegment damage[DAMAGE_SEGMENTS_MAX];
};

// a típusok a betűjük sorrendjében: az index a betű − 'A'; a nevük a text_monster_names-ben
extern const struct MonsterType monster_types[MONSTER_TYPE_COUNT];

#endif
