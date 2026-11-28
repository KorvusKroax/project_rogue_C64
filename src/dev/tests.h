#ifndef TESTS_H
#define TESTS_H

#include <stdbool.h>
#include <stdint.h>

void run_test_screens(void);        // a tesztképernyők menüje: mindegyik billentyűre indul, ESC-re vissza a menübe; nem tér vissza

// egy önteszt: az i. próbát futtatja, igaz = sikerült
// int, nem bool: az Oscar64 1.32.273 a függvénymutatón át hívott 8 bites visszatérési értéket rossz helyről olvassa
int test_RN(uint16_t i);
int test_rnd(uint16_t i);
int test_ROLL(uint16_t i);
int test_spread(uint16_t i);
int test_saturating_arithmetic(uint16_t i);
int test_format(uint16_t i);
int test_convert_text_to_number(uint16_t i);
int test_key_decode(uint16_t i);
int test_view_follow(uint16_t i);
int test_prepare_message_text(uint16_t i);
int test_text_list_rows(uint16_t i);            // a 40 oszlopos lista sorai: a hosszú tétel szóhatáron törik, a folytatósor behúzással; a szélesség legalább a lista végének szövege
int test_text_list_window_fit(uint16_t i);      // az overwrite ablakba fér-e: az üres oszlop és a lista végének sora is kell; a teli csomag hosszú nevekkel nem fér el
int test_window_restore(uint16_t i);            // az ideiglenes ablak bezárása után a képernyő bájtra azonos a megnyitás előttivel (a megnyitás az üzenetsávot törli)
int test_list_row_not_joined(uint16_t i);       // a tárgylista-sor után a kérdés új sávban kezdődik, és a listasor sem fűződik az előző üzenethez; más üzenetek összefűződnek
int test_cleared_last_message(uint16_t i);      // új játéknál (clear_last_message) a CTRL-P üres sávot hoz, az üzenet és a listasor után is
int test_prompt_input_space(uint16_t i);        // a kérdés csak akkor fűződik az előző üzenethez, ha a beviteli mezője is elfér mögötte
int test_message_chars_fitting_row(uint16_t i);
int test_message_chars_fitting_bar(uint16_t i);
int test_map_flags(uint16_t i);
int test_copy_text(uint16_t i);                 // a szöveg és a táblaelem a lezáró nullával, legfeljebb max_length karakter, a lezáró nullára mutat; a táblaelem címe; a játék szövegeivel is; a $01 és a megszakítás tiltása a hívás után ugyanaz, mint előtte
int test_nmi_during_copy(uint16_t i);           // a RAM-beli NMI-vektor ugyanoda mutat, mint a $0318; a CIA2 időzítőjének NMI-je a kilapozott ROM melletti másolás közben visszatér, és a másolás hibátlan
int test_diagonal_step(uint16_t i);
int test_neighbor_visibility(uint16_t i);
int test_careful_run(uint16_t i);
int test_passage_turn(uint16_t i);
int test_search(uint16_t i);
int test_monster_types(uint16_t i);
int test_monster_table(uint16_t i);
int test_monster_order(uint16_t i);
int test_monster_generation(uint16_t i);
int test_monster_tiles(uint16_t i);
int test_monster_visibility(uint16_t i);
int test_tile_visibility(uint16_t i);           // a mező látszik-e (cansee): világos szobában az egész szoba, sötétben és folyosón a szomszédos, folyosón átlósan sarkon át nem
int test_monster_waking(uint16_t i);
int test_monster_chase(uint16_t i);
int test_monster_destination(uint16_t i);       // a célpont: a szoba tárgya, ha a játékos nincs a szobában és nem látja a szörnyet; a carry 0 típusé és a foglalt tárgy nem; J3
int test_greedy_monster(uint16_t i);            // az ébredő ork a szoba aranyára megy, ráül, nem veszi fel; ha a játékos felveszi, őt üldözi; más szörny felveheti, és a jelző törlődik; tárgy célpontnál arany nélküli szobában a játékos a célpont
int test_destination_taken(uint16_t i);          // ha a célpont tárgyat egy másik szörny vagy a játékos (az aranyat is) felveszi, a rá tartó szörnyek célpontja a játékos lesz, a más tárgyra tartóké marad
int test_monster_pickup(uint16_t i);            // a szörny felveszi a célpont tárgyat, a mező padló; ha a játékos látja, a tárgy jele nem tér vissza (J32), ha nem, az emlék marad; az útjában álló játékost megtámadja (J33)
int test_strength_bonuses(uint16_t i);
int test_monster_names(uint16_t i);
int test_player_hit_rate(uint16_t i);
int test_monster_hit_rate(uint16_t i);
int test_move_attack(uint16_t i);
int test_monster_attack(uint16_t i);
int test_fight_diagonal(uint16_t i);
int test_fight_hit_limit(uint16_t i);
int test_fight_other_attack(uint16_t i);
int test_fight_target_moves(uint16_t i);
int test_monster_kill(uint16_t i);
int test_monster_drops(uint16_t i);             // a megölt szörny tárgyai a körüli 3×3 üres mezőire esnek (a játékosé nem), a láthatók kirajzolódnak; szörny alá esve az alatta lévő jel a tárgyé; hely nélkül a tárgy elvész
int test_monster_removal(uint16_t i);
int test_fight_target_death(uint16_t i);
int test_experience_levels(uint16_t i);
int test_ice_monster(uint16_t i);
int test_vampire(uint16_t i);
int test_wraith(uint16_t i);
int test_rattlesnake(uint16_t i);
int test_strength_change(uint16_t i);
int test_flytrap(uint16_t i);
int test_flytrap_hold(uint16_t i);
int test_xeroc_reveal(uint16_t i);
int test_aquator(uint16_t i);                   // a rozsda a viselt páncélt eggyel rontja, a határig; a védettet, a bőrpáncélt nem, a kikapcsolt Aquator sem; páncél nélkül nincs hatása
int test_nymph(uint16_t i);                     // a nimfa egy használaton kívüli mágikus tárgyat lop, kötegből egy darabot (J31), az amulettet is, és eltűnik a tárgyaival; ha nincs mit, marad
int test_leprechaun(uint16_t i);                // a leprechaun aranyat lop (pénz nélkül is), és eltűnik a tárgyaival; a megölt leprechaun aranya leesik
int test_lit_room_drops(uint16_t i);            // világos szobában a megölt szörny tárgyai a játékostól két mezőre esve is azonnal kirajzolódnak
int test_vanishing_monster_turn(uint16_t i);    // a lopás után eltűnő gyorsított nimfának nincs második lépése, a többi szörny egyszer lép, a TILE_MONSTER bitek rendben; a nimfa a tábla végén és elején
int test_player_death(uint16_t i);
int test_wielded_damage(uint16_t i);            // a kézben tartott tárgy sebzése az erő bónuszával: üres kéz 1x4, bot 2x3, pálca 1x1, más tárgy 0x0; a nem fegyver tárgy rekordja nem bónusz
int test_player_armor(uint16_t i);              // a védelem a viselt páncélé, levéve a BASE_ARMOR; a státuszsorban a kezdő páncéllal Arm:4, levéve Arm:0
int test_item_chances(uint16_t i);
int test_levels_without_food(uint16_t i);
int test_item_table(uint16_t i);                // a tárgytábla betelik, utána nincs hely; felszabadítás után újra van
int test_item_lists(uint16_t i);                // tárgylista: beszúrás az üres listába, az elejére, a végére, a közepére; törlés az elejéről, a közepéről, a végéről
int test_item_initial_values(uint16_t i);       // minden típus kezdőértéke érvényes, a véletlen rész nélkül és vele is
int test_item_groups(uint16_t i);               // kötegekkel teli tárgytáblában is mind különböző a kötegazonosító
int test_item_name_lists(uint16_t i);           // a nevek és az álnevek listái: minden elem kisbetűvel kezdődik, és egy listán belül mind különböző
int test_item_pseudonyms(uint16_t i);           // új játéknál minden típus ismeretlen, az álnevek a lista tartományában, ismétlés nélkül; pálca és bot is van
int test_scroll_titles(uint16_t i);             // az i. seeddel indult játék tekercsfeliratai érvényesek és mind különbözők, az előállításuk nem hívja az RN-t
int test_item_names(uint16_t i);                // kézzel összerakott tárgyak neve betűre a 8.8 példái szerint, az elnevezett típusok és a címkék `called` alakjával, és a használatban lévő tárgy utótagjával is
int test_longest_item_name(uint16_t i);         // a leghosszabb név (20 karakteres típusnévvel, kézben tartott bot) betűre egyezik, és semmilyen név nem hosszabb az ITEM_NAME_LENGTH_MAX-nál, a 20 karakteres típusnévvel, címkével és utótaggal sem
int test_discovery_list(uint16_t i);            // a `D` listája: az ismert és az elnevezett típusok egyszer, a semmi sor, a `*` elválasztói, hívásonként új sorrend, az RN érintetlen
int test_name_pool(uint16_t i);                 // a névkészlet MAX_NAMES nevet tárol, utána betelt; a felszabadított hely újra foglalható, az átnevezés helyben marad; a fejlesztői mód foglaltsági üzenete
int test_label_freed_with_item(uint16_t i);     // az új tárgynak nincs címkéje; a tárgy felszabadítása a címkéjét is felszabadítja, a szintváltáskor a padlón maradtét is
int test_label_merge(uint16_t i);               // a címke nélküli köteg átveszi a beolvadó tárgy címkéjét, a címkés kötegé marad, és a beolvadóé felszabadul
int test_pack_nameable_list(uint16_t i);        // a `c` listájából az étel és az amulett kimarad; ha nincs más, „you don't have anything appropriate”
int test_name_item(uint16_t i);                 // a `c` menete leütésekkel: üres mező, átnevezés elírással, ESC, törlés, üres sáv, azonosított típus, címke, hosszkorlát, étel és amulett, betelt névkészlet
int test_pack_slots(uint16_t i);                // a csomag helyei darabszám szerint, a kötegelt fegyver egy hely; betelt csomagba csak a meglévő köteghez csatolt fegyver fér
int test_pack_letters(uint16_t i);              // egy tárgy kivétele után a többi betűje marad, az új tárgy a legkisebb szabad betűt kapja
int test_pack_order(uint16_t i);                // a csomag típus szerint rendezett: az új tárgy az azonos altípusúak, különben az azonos típusúak után kerül
int test_pickup_by_step(uint16_t i);            // futva a tárgyra lépve felveszi, a futás megáll, és a mező újra folyosó; az `m` a padlón hagyja; a `,` felveszi, és a mező újra padló
int test_pickup_gold(uint16_t i);               // az arany a pénzhez adódik, nem kerül a csomagba, és a rekordja felszabadul
int test_pickup_full_pack(uint16_t i);          // betelt csomagnál a tárgy a jelével együtt a padlón marad
int test_drop_stack_piece(uint16_t i);          // a bájitalkötegből egy darab kerül le új rekordként, a köteg a csomagban marad; a nyílköteg egészben
int test_drop_full_table(uint16_t i);           // betelt tárgytáblánál a köteg darabolása elmarad, és semmi nem változik; egy egész tárgy lerakható
int test_amulet_carried(uint16_t i);            // az amulett felvéve a játékosnál van, lerakva nincs (a csomagból adódik, J29)
int test_wield_item(uint16_t i);                // a `w`: a `*` lista csak fegyver; páncélra és használt tárgyra elmarad; bármi más kézbe vehető; az átkozott a kézben marad; üres kör nincs
int test_wear_armor(uint16_t i);                // a `W`: a `*` lista csak páncél; nem páncélra üres kör nélkül; a páncél egy üres kör után kerül fel, azonosítva; második nem; halálnál elmarad
int test_wear_stolen_armor(uint16_t i);         // a `W` üres körében a nimfa ellopja a felveendő páncélt: az nem kerül fel (J34)
int test_take_off_armor(uint16_t i);            // a `T`: páncél nélkül nem visz időt; az átkozott rajta marad üres kör nélkül; egy üres kör után lekerül; halálnál elmarad
int test_starting_equipment(uint16_t i);        // az i. seeddel: a kezdő csomag betűi és sorrendje, a típusok, a bónuszok, az azonosítás, a nyilak száma; a páncél viselve, a buzogány kézben
int test_report_items_in_use(uint16_t i);       // a `)` és a `]`: „you are wielding nothing” / „wearing nothing”; különben a betű és a név, utótag nélkül
int test_drop_item_in_use(uint16_t i);          // a `d` a használt tárggyal: az átkozott marad; a kéz kiürül, egy darabnál is; hely nélkül a köteg a kézben marad; a páncél üres körrel; halálnál elmarad
int test_timer_expiry(uint16_t i);              // a bájitalok időzítői körönként egyet lépnek, az üres körben is; az egy körben lejárók üzenetei a TIMER_ sorrendben; vakon a hallucináció csendben ér véget; a see invisible és a monster detection vége üzenet nélkül

#define SCROLL_TITLE_TEST_SEED_COUNT  200       // ennyi seeddel indul új játék a tekercsfeliratok tesztjében
#define STARTING_EQUIPMENT_TEST_SEED_COUNT  100 // ennyi seeddel készül a kezdőfelszerelés a tesztjében (a nyilak száma véletlen)

struct Item;

bool is_item_type_and_subtype_valid(uint8_t type, uint8_t which);   // a véletlen tárgy típusa és altípusa érvényes (test_items.c)
bool are_item_initial_values_valid(const struct Item *item);        // a tárgy kezdőértékei a típusa szerint érvényesek (test_items.c)
uint8_t get_tested_subtype_count(uint8_t type);                     // a típus altípusainak száma; az aranynak és az amulettnek egy van, which = 0 (test_items.c)
bool is_item_list(uint8_t list_first, const uint8_t *expected, uint8_t count);  // a lista elemei sorban éppen az expected első count eleme, utána vége (test_items.c)
bool does_message_bar_start_with(const char *text);     // az üzenetsáv első sora ezzel a szöveggel kezdődik (test_text_list.c)
bool is_message_bar_empty(void);                        // az üzenetsáv első sora üres (test_text_list.c)
// az üzenetsáv sorai a záró szóközök nélkül, a sortörésnél szóközzel összefűzve éppen ez a szöveg; a sor, amelyben véget ér, utána üres,
// a következő sorokat nem nézi (test_text_list.c)
bool is_message_bar_text(const char *text);

// szint-önteszt: hosszú, VICE-ben warp módban
#define LEVEL_TEST_LEVEL_COUNT  1000            // ennyi véletlen szint, rögzített magokkal
#define LEVEL_TEST_DEPTH_COUNT  4               // ennyi mélységen, felváltva (test_level.c)

#define LEVEL_MOVE_TEST_LEVEL_COUNT  100        // ennyi szinten lépnek a szörnyek, ugyanazokkal a magokkal, mint a generálás tesztjében

int test_level_generation(uint16_t i);          // az i. szint generálása és ellenőrzése: szobák, labirintusok, összekötés, tárgyak, a szörnyek kezdő tárgyai
int test_level_monster_moves(uint16_t i);       // az i. szinten körök a játékos lépésével, előbb 5, aztán minden szörny üldöz (a tárgygyűjtők a szobájuk tárgyait is felveszik): a lépéseik után is érvényesek a szörnymezők és a tárgylisták; közben a kör idejét méri
#define LEVEL_AMULET_TEST_LEVEL_COUNT  10       // ennyi maggal, magonként két szint
int test_level_amulet_loot(uint16_t i);         // amulettel a legmélyebb elért szint fölött üres a padló, a legmélyebb elért szinten van arany vagy tárgy, de újabb amulett nincs
bool show_level_room_ratios(void);              // mélységenként a sötét szobák és a labirintusok várt és kapott száma, a test_level_generation után; igaz, ha mind a tűrésen belül (leállított futásnál nem ítél, igazat ad)
bool show_level_generation_times(void);         // mélységenként a leglassabb szintgenerálás képkockában, a test_level_generation után; igaz, ha mind a 3 másodperces kereten belül
void show_level_turn_times(void);               // mélységenként egy kör ideje a képkocka százalékában, a test_level_monster_moves után

#endif
