#ifndef MONSTERS_H
#define MONSTERS_H

#include <stdint.h>
#include <stdbool.h>

#include "state.h"

// a szörnyflagek (16 bit); az alsó bájtban azok, amelyek a típustáblában is szerepelnek, mert ott egy bájton tárolódnak
#define MONSTER_MEAN          0x0001            // belépéskor felébredhet (ISMEAN)
#define MONSTER_FLYING        0x0002            // a játékostól távol kétszer lép (ISFLY)
#define MONSTER_GREEDY        0x0004            // előbb a szobabeli aranyra megy (ISGREED)
#define MONSTER_INVISIBLE     0x0008            // csak láthatatlan-látással látszik (ISINVIS)
#define MONSTER_RUNNING       0x0010            // ébren van és üldöz; nélküle egyáltalán nem mozog (ISRUN)
#define MONSTER_HELD          0x0020            // egyáltalán nem mozog, a Hold monster tekercstől (ISHELD)
#define MONSTER_SLOW          0x0040            // csak minden második körben cselekszik (ISSLOW)
#define MONSTER_SLOW_TURN     0x0080            // lassított szörnynél: ebben a körben cselekszik; körönként váltakozik (turn)
#define MONSTER_HASTED        0x0100            // két akció egy körben (ISHASTE)
#define MONSTER_CONFUSED      0x0200            // 80%-ban véletlen irányba lép (ISHUH)
#define MONSTER_CANCELLED     0x0400            // a különleges képessége ki van kapcsolva (ISCANC)
#define MONSTER_ABILITY_USED  0x0800            // a különleges képességét már elsütötte, pl. a Medusa pillantása (ISFOUND)
#define MONSTER_TARGET        0x1000            // az f parancs célpontja (ISTARGET)

#define MONSTER_TYPE_FLAGS  (MONSTER_MEAN | MONSTER_FLYING | MONSTER_GREEDY | MONSTER_INVISIBLE)

#if MONSTER_TYPE_FLAGS > 0xff
    #error "a típustábla flagjei nem férnek el egy bájton"
#endif

#define MONSTER_TYPE_COUNT  26                  // 'A'..'Z'

#define MONSTER_STRENGTH  10                    // minden szörny ereje; nem tárolódik

#define DAMAGE_SEGMENTS_MAX  3

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

#define NO_MONSTER  0                           // szörnyindex helyett: nincs szörny

// a szörnytábla tömör: a szörnyek az 1..game.monster_count indexeken, a létrehozás sorrendjében
// egy szörny kivételekor a mögötte állók előrecsúsznak, ezért szörnyindexet semmi nem tárolhat tartósan
// a szörnyek a legújabbtól a legrégebbiig cselekszenek: for (uint8_t index = game.monster_count; index != NO_MONSTER; index--)
struct Monster *get_monster(uint8_t index);     // index: 1..game.monster_count
uint8_t allocate_monster(void);                 // új, kitöltetlen hely a tábla végén; NO_MONSTER, ha betelt
void clear_monsters(void);                      // a szörnytábla üres: új szintnél
uint8_t find_monster_at(uint8_t x, uint8_t y);  // az x, y mezőn álló szörny indexe, vagy NO_MONSTER; a táblát csak TILE_MONSTER bitnél nézi végig

// a szörny kivétele a táblából (remove_mon): a mezőjén törlődik a TILE_MONSTER bit, és kirajzolódik az alatta lévő jel;
// ha ő volt az f-harc célpontja, a harc véget ér; a mögötte állók egy hellyel előrecsúsznak, így a mutató utána a következőre mutat
void remove_monster(struct Monster *monster);

// a game.dungeon_level mélységhez illő típus ('A'..'Z'), a kezdő vagy a vándorló sorrendből
uint8_t choose_monster_type(bool wandering);

// új szörny a megadott típussal az (x, y) mezőre, a mélység szerinti statokkal, a mezőn a TILE_MONSTER bittel; NO_MONSTER, ha betelt a tábla
// a mezőn nem állhat másik szörny
uint8_t create_monster(uint8_t type, uint8_t x, uint8_t y);

// új szörny a mélységhez illő véletlen típussal; betelt táblánál nincs dobás, NO_MONSTER
uint8_t generate_monster(bool wandering, uint8_t x, uint8_t y);

// a játékos meglátja (look_around), vagy belép a szobájába, illetve kilép onnan (enter_room, leave_room):
// az alvó aljas szörny 2/3 eséllyel felébred, a kapzsi mindig, és a játékos szobájának aranyára megy; a többit csak
// a start_monster_running indítja el
void wake_monster(struct Monster *monster);

// a szörny mindenképp felébred és üldöz (runto), a fogva tartás (MONSTER_HELD) is megszűnik: pl. ha a játékos megtámadja;
// a célpontját a choose_monster_destination adja
void start_monster_running(struct Monster *monster);

// a szörny célpontja (find_dest): a játékos, ha a típus nem gyűjt tárgyat (carry 0), a játékos szobájában van, vagy a játékos látja;
// különben a szobájában fekvő tárgyak közül az első, amelyre a carry értékének százalékos esélyével rádob, és nem egy másik szörny
// célpontja (J3); ha nincs ilyen, a játékos
uint8_t choose_monster_destination(const struct Monster *monster);

// a tárgy lekerült a padlóról (take_item_from_floor): minden szörny, amelyik felé tartott, a játékost veszi célba,
// így a célpont mindig egy padlón fekvő tárgy vagy a játékos
void redirect_monsters_to_player(uint8_t item_index);

#endif
