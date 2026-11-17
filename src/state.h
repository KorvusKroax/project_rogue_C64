#ifndef STATE_H
#define STATE_H

#include <stdint.h>

// a logikai térkép mérete; térképmező: x = 0..MAP_COLUMNS-1, y = MAP_TOP_ROW..MAP_TOP_ROW+MAP_ROWS-1
#define MAP_COLUMNS  80                         // default: 80
#define MAP_ROWS     22                         // default: 22
#define MAP_TOP_ROW  1                          // a 0. sor az üzenetsor (default: 1)

// a koordináták 8 bitesek, és két oszlop különbsége (dist2, nézőablak) előjelesen 8 biten is elfér
#if MAP_COLUMNS > 128 || MAP_TOP_ROW + MAP_ROWS > 128
    #error "a térkép túl nagy a 8 bites koordinátákhoz"
#endif

// a logikai képernyő: a térkép fölött az üzenetsor, alatta a státuszsor
#define LOGICAL_SCREEN_ROWS  (MAP_TOP_ROW + MAP_ROWS + 1)     // default: 24

#define IN_PASSAGE  0xfe                        // a lény nem szobában, hanem folyosón áll; a labirintus szobának számít

struct Player {
    uint8_t  x;                                 // a pozíció a térképen
    uint8_t  y;
    uint8_t  room;                              // a szoba indexe, amelyben áll, vagy IN_PASSAGE
    uint8_t  strength;                          // 3..31
    uint8_t  experience_level;
    int16_t  hit_points;
    int16_t  max_hit_points;
    uint32_t experience;
};

#define DESTINATION_PLAYER  0                   // a szörny célpontja a játékos, nem egy tárgy

// egy szörny a szörnytáblában; az ereje, a sebzése, a neve és a carry értéke a típustáblából jön (monsters.h)
struct Monster {
    uint8_t  x;                                 // a pozíció a térképen
    uint8_t  y;
    uint8_t  room;                              // a szoba indexe, amelyben áll, vagy IN_PASSAGE
    uint8_t  type;                              // 'A'..'Z'; a típustábla indexe a betű − 'A'
    uint8_t  experience_level;
    int8_t   armor;
    int16_t  hit_points;
    int16_t  max_hit_points;
    uint16_t experience;                        // ennyi XP-t ad, ha megölik
    uint16_t flags;                             // MONSTER_ flagek (monsters.h)
    uint8_t  character_below;                   // a mező képernyőjele a szörny alatt: ez rajzolódik ki, ha elmegy vagy nem látszik (oldch)
    uint8_t  disguise;                          // ezzel a jellel rajzolódik ki: a saját betűje, a xeroc egy tárgyjel
    uint8_t  destination_item;                  // a padlón fekvő tárgy indexe, amely felé tart, vagy DESTINATION_PLAYER
    uint8_t  first_pack_item;                   // a nála lévő tárgyak listájának eleje (0 = nincs)
};

// a szörnytábla mérete; a szörnyekre 1..MAX_MONSTERS index hivatkozik, a 0 a „nincs”
#define MAX_MONSTERS  40                        // default: 40

// a szörnyindex 8 bites, és a 0 foglalt
#if MAX_MONSTERS > 255
    #error "túl sok szörny: a szörnyindex 8 bites"
#endif

// a logikai képernyő SECTOR_GRID_SIZE × SECTOR_GRID_SIZE szektorra osztva
#define SECTOR_GRID_SIZE  3                     // default: 3

// a szobák száma: szektoronként egy, a szoba indexe a szektor indexe
#define ROOM_COUNT  (SECTOR_GRID_SIZE * SECTOR_GRID_SIZE)      // default: 9

#define MAX_EXTRA_CORRIDORS  4                  // a feszítőfa után legfeljebb ennyi extra folyosó-próbálkozás (default: 4)

// a folyosók legnagyobb száma egy szinten: a szomszédos szektorpárok száma, illetve a feszítőfa és az extra folyosók száma közül a kisebb
#define ADJACENT_SECTOR_PAIR_COUNT  (2 * SECTOR_GRID_SIZE * (SECTOR_GRID_SIZE - 1))          // default: 12
#define CORRIDOR_ATTEMPT_LIMIT      (ROOM_COUNT - 1 + MAX_EXTRA_CORRIDORS)                   // default: 12
#define MAX_CORRIDORS  (ADJACENT_SECTOR_PAIR_COUNT < CORRIDOR_ATTEMPT_LIMIT ? ADJACENT_SECTOR_PAIR_COUNT : CORRIDOR_ATTEMPT_LIMIT)   // default: 12

// az ajtótábla mérete: minden folyosónak két vége van
#define MAX_EXITS  (2 * MAX_CORRIDORS)          // default: 24

// a bejegyzések száma 8 bites
#if MAX_EXITS > 255
    #error "túl sok kijárat: az ajtótábla számlálója 8 bites"
#endif

// a folyosórendszerek legnagyobb száma: mindegyikben van legalább egy folyosó
#define MAX_PASSAGES  MAX_CORRIDORS             // default: 12

// a szobatábla: a szobák, utánuk a folyosórendszerek rekordjai
#define ROOM_TABLE_SIZE  (ROOM_COUNT + MAX_PASSAGES)       // default: 21

// a szobaindex 8 bites: a szobatábla ciklusai uint8_t számlálóval mennek, és a 0xfe és a 0xff szabad marad (IN_PASSAGE, ANY_ROOM)
#if ROOM_TABLE_SIZE > 254
    #error "túl sok szoba: a szobaindex 8 bites"
#endif

// szobaflagek
#define ROOM_PHANTOM  0x01                      // nem igazi szoba: folyosó-csomópont vagy folyosórendszer
#define ROOM_DARK     0x02                      // sötét szoba
#define ROOM_MAZE     0x04                      // labirintus

struct Room {
    uint8_t x;                                  // a bal felső sarok, fallal együtt; fantom szobánál maga a folyosó-csomópont, folyosórendszernél 0
    uint8_t y;
    uint8_t width;                              // a méret falakkal együtt; fantom szobánál és folyosórendszernél 0
    uint8_t height;
    uint8_t flags;
};

#define NO_PASSAGE_NUMBER  0                    // a kijárat nem csatlakozik folyosórendszerhez: a labirintus kijárata maga is folyosó-mező

// egy kijárat az ajtótáblában: folyosóvég egy nem fantom szobán; normál szobánál ajtó (a titkos is), labirintusnál a kijárat folyosó-mezője
struct Exit {
    uint8_t x;
    uint8_t y;
    uint8_t room;                               // a szoba indexe, amelyhez a kijárat tartozik
    uint8_t passage_number;                     // a folyosórendszer sorszáma, amelyhez az ajtó csatlakozik, vagy NO_PASSAGE_NUMBER
};

// a játékállapot-blokk: a mentés és a betöltés egyben írja és olvassa (kezdőcím: &game, méret: sizeof(game))
// csak ide kerülhet perzisztens adat; mutató nem, csak index
#define DUNGEON_LEVEL_MAX  255                  // a mélység 8 bites; a legmélyebb szinten a lépcső új ugyanilyen mély szintet generál

struct GameState {
    uint32_t seed;
    uint8_t  dungeon_level;                     // 1..DUNGEON_LEVEL_MAX

    uint32_t gold;
    struct Player player;
    uint8_t  max_strength;                      // a legnagyobb elért erő, gyűrűk nélkül
    uint8_t  hunger_state;                      // 0: rendben, 1: Hungry, 2: Weak, 3: Faint

    struct Room rooms[ROOM_TABLE_SIZE];         // a szobák, utánuk a folyosórendszerek: az n. rendszer indexe ROOM_COUNT + n - 1
    struct Exit exits[MAX_EXITS];               // az ajtótábla: a szint összes kijárata, a felvétel sorrendjében
    uint8_t  exit_count;
    struct Monster monsters[MAX_MONSTERS];      // a szörnytábla: tömör, a szörnyek a létrehozás sorrendjében (monsters.h)
    uint8_t  monster_count;                     // a szörnyek száma: az 1..monster_count indexek foglaltak

    uint8_t  map_characters[MAP_ROWS][MAP_COLUMNS]; // a MAP karakterrétege: a mező valódi jele, a logikai y sora a [y - MAP_TOP_ROW]
    uint8_t  map_flags[MAP_ROWS][MAP_COLUMNS];      // a MAP flagrétege: a mezőflagek (map.h)

    uint8_t  view_window_x;                     // a nézőablak bal szélének logikai oszlopa
    uint8_t  seen_map[MAP_ROWS][MAP_COLUMNS];   // a SCREEN réteg, a logikai y sora a [y - MAP_TOP_ROW]; 7. bit = inverz
};

extern struct GameState game;

// a beállítások bitjei (4.2.5)
#define OPTION_FOLLOW_PASSAGES  0x01            // futásnál a folyosó kanyarjainak követése
#define OPTION_JUMP_TO_RUN_END  0x02            // futás közben nincs rajzolás, csak a végén

// a beállításblokk: a GameState-en kívül, mert a képernyőmodul is használja; alapból minden kikapcsolva (nulla)
struct Options {
    uint8_t flags;                              // OPTION_ bitek
};

extern struct Options options;

#endif
