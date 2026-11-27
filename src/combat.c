#include "combat.h"

#include <stddef.h>
#include <string.h>

#include "platform/platform.h"
#include "equipment.h"
#include "explore.h"
#include "game.h"
#include "item_names.h"
#include "items.h"
#include "map.h"
#include "message.h"
#include "monsters.h"
#include "move.h"
#include "num.h"
#include "player.h"
#include "rng.h"
#include "state.h"
#include "texts.h"
#include "view.h"

// nem időkritikus kód: méretre fordítva, itt a hely fontosabb a sebességnél
#pragma optimize(push)
#pragma optimize(size)

#if MONSTER_STRENGTH > STRENGTH_MAX
    #error "a szörnyek ereje nagyobb a legnagyobb erőnél"
#endif

const int8_t strength_hit_bonus[STRENGTH_MAX + 1] = {
    -7, -6, -5, -4, -3, -2, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};

const int8_t strength_damage_bonus[STRENGTH_MAX + 1] = {
    -7, -6, -5, -4, -3, -2, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6
};

const struct DamageSegment weapon_melee_damage[WEAPON_SUBTYPE_COUNT] = {
    { 2, 4 },   // mace
    { 3, 4 },   // long sword
    { 1, 1 },   // short bow
    { 1, 1 },   // arrow
    { 1, 6 },   // dagger
    { 4, 4 },   // two handed sword
    { 1, 1 },   // dart
    { 1, 2 },   // shuriken
    { 2, 3 },   // spear
};

static const struct DamageSegment staff_melee_damage = { STAFF_MELEE_DICE, STAFF_MELEE_SIDES };
static const struct DamageSegment wand_melee_damage = { WAND_MELEE_DICE, WAND_MELEE_SIDES };
static const struct DamageSegment no_melee_damage = { 0, 0 };
static const struct DamageSegment empty_hand_damage = { EMPTY_HAND_DAMAGE_DICE, EMPTY_HAND_DAMAGE_SIDES };

// a kézben tartott tárgy sebzése közelharcban: fegyvernél a típusé, a pálcánál és a botnál az anyaguké (fém: pálca, fa: bot),
// minden más tárgynál nincs
static const struct DamageSegment *get_melee_damage(const struct Item *item)
{
    switch (item->type) {
        case ITEM_TYPE_WEAPON:
            return &weapon_melee_damage[item->which];
        case ITEM_TYPE_STICK:
            return game.stick_materials[item->which] < STICK_METAL_COUNT ? &wand_melee_damage : &staff_melee_damage;
        default:
            return &no_melee_damage;
    }
}

// az f-harc állapota: tart-e, merre, kamikaze-e (HP-határ nélkül), és a harc legnagyobb egyszeri sebzése a játékoson;
// a harc alatt nincs parancsolvasás, így mentés sem, ezért nem a játékállapotban van
static bool is_fight_active;
static int8_t fight_step_x;
static int8_t fight_step_y;
static bool is_fight_kamikaze;
static int16_t max_hit_this_fight;

// a támadás szegmensenként dob a találatra, és a találó szegmensek sebzése (a kockák és a sebzésbónusz, legalább 0)
// levonódik a védő HP-jából; igaz, ha legalább egy szegmens talált (roll_em)
static bool roll_attack(uint8_t attacker_level, int16_t hit_bonus, int16_t damage_bonus, int8_t defender_armor,
                        const struct DamageSegment *segments, uint8_t segment_count, int16_t *defender_hit_points)
{
    // a szint legfeljebb 255, a páncél legalább −128, így 8 biten nem fér el
    int16_t required = HIT_TARGET_BASE - (int16_t)attacker_level - defender_armor;
    bool did_hit = false;

    for (uint8_t segment_index = 0; segment_index < segment_count; segment_index++) {
        if ((int16_t)rnd(HIT_ROLL_SIDES) + hit_bonus >= required) {
            const struct DamageSegment *segment = &segments[segment_index];
            int16_t damage = (int16_t)ROLL(segment->dice, segment->sides) + damage_bonus;
            if (damage > 0) {
                *defender_hit_points -= damage;
            }
            did_hit = true;
        }
    }
    return did_hit;
}

// a buffers szakaszban ($C400-$C7FF, platform.c)
#pragma bss(buffers)
static char monster_name_text[MONSTER_NAME_TEXT_SIZE];
#pragma bss(bss)

const char *prepare_monster_name(const struct Monster *monster)
{
    // ide jön: szörnyérzékeléssel (SEEMONST) a nem látott szörny is a nevén
    if (!is_monster_visible(monster)) {
        copy_text(monster_name_text, text_something, MONSTER_NAME_TEXT_SIZE - 1);
        return monster_name_text;
    }

    // ide jön: hallucinálva a képernyőn látott betűhöz tartozó név
    char *type_name = copy_text(monster_name_text, text_monster_article, MONSTER_NAME_TEXT_SIZE - 1);
    copy_text_from_table(type_name, text_monster_names, monster->type - 'A', MONSTER_NAME_TEXT_SIZE - 1 - (type_name - monster_name_text));
    return monster_name_text;
}

// a hit/miss üzenet a támadás után, a változatok közül véletlenül; f-harc közben nincs üzenet
static void report_attack(const char *const *hit_texts, const char *const *miss_texts, bool did_hit,
                          const char *attacker_name, const char *defender_name)
{
    if (is_fight_active) return;

    const char *const *texts = did_hit ? hit_texts : miss_texts;
    message_format(get_text_from_table(texts, rnd(ATTACK_TEXT_VARIANT_COUNT)), attacker_name, defender_name, 0);
}

bool roll_player_attack(struct Monster *monster)
{
    // az erő és a kézben tartott fegyver bónusza; ide jön: a dexterity és az increase damage gyűrű bónusza
    int16_t hit_bonus = strength_hit_bonus[game.player.strength];
    int16_t damage_bonus = strength_damage_bonus[game.player.strength];
    const struct DamageSegment *damage = &empty_hand_damage;

    if (game.wielded_item != NO_ITEM) {
        const struct Item *item = get_item(game.wielded_item);
        damage = get_melee_damage(item);
        // a bónuszok csak fegyvernél: más tárgy rekordjában a helyükön más adat áll (pl. a pálca töltése)
        if (item->type == ITEM_TYPE_WEAPON) {
            hit_bonus += item->properties.weapon.hit_bonus;
            damage_bonus += item->properties.weapon.damage_bonus;
        }
    }
    return roll_attack(game.player.experience_level, hit_bonus, damage_bonus, monster->armor, damage, 1, &monster->hit_points);
}

bool roll_monster_attack(const struct Monster *monster)
{
    const struct MonsterType *type_data = &monster_types[monster->type - 'A'];
    int16_t hit_bonus = strength_hit_bonus[MONSTER_STRENGTH];
    int16_t damage_bonus = strength_damage_bonus[MONSTER_STRENGTH];
    // ugyanaz, mint a játékos ISRUN-jának vizsgálata: az ISRUN pontosan akkor hiányzik, amikor a játékos cselekvésképtelen (J1)
    if (game.turns_unable_to_act > 0) {
        hit_bonus += INACTIVE_DEFENDER_HIT_BONUS;
    }

    return roll_attack(monster->experience_level, hit_bonus, damage_bonus, calculate_player_armor(),
                       type_data->damage, type_data->damage_segment_count, &game.player.hit_points);
}

// a mező körüli 3×3 egy véletlen üres mezője, a mező maga is (fallpos): padló vagy folyosó jelű, és nem a játékosé;
// hamis, ha nincs ilyen
static bool choose_fall_position(uint8_t center_x, uint8_t center_y, uint8_t *x, uint8_t *y)
{
    // az első jelölt mindig felülírja (rnd(1) == 0); a kezdőérték csak a fordítónak kell, hogy lássa: az eredmény mindig kap értéket
    *x = center_x;
    *y = center_y;
    uint8_t count = 0;
    for (int8_t offset_y = -1; offset_y <= 1; offset_y++) {
        for (int8_t offset_x = -1; offset_x <= 1; offset_x++) {
            // a 0. oszlop (sor) előtti mező 255 lesz: azt az is_on_map kívülinek veszi
            uint8_t tile_x = center_x + offset_x;
            uint8_t tile_y = center_y + offset_y;
            if (!is_on_map(tile_x, tile_y)) continue;
            if (tile_x == game.player.x && tile_y == game.player.y) continue;
            uint8_t character = get_map_character(tile_x, tile_y);
            if (character != CHAR_FLOOR && character != CHAR_PASSAGE) continue;

            // az eddigi jelöltek közül egyenletes eséllyel
            count++;
            if (rnd(count) == 0) {
                *x = tile_x;
                *y = tile_y;
            }
        }
    }
    return count != 0;
}

// a szörny csomagjának tárgyai a mezője köré esnek (fall), mielőtt a szörny kikerül a táblából; amelyiknek nincs helye, elvész;
// ha a játékos látja a mezőt, a tárgy kirajzolódik, illetve ha szörny áll rajta (a haldokló is), az alatta lévő jel lesz a tárgyé,
// így a haldokló mezőjére esett tárgy a kivételkor jelenik meg
static void drop_monster_pack(struct Monster *monster)
{
    while (monster->first_pack_item != NO_ITEM) {
        uint8_t index = monster->first_pack_item;
        monster->first_pack_item = get_item(index)->next;

        uint8_t x;
        uint8_t y;
        if (!choose_fall_position(monster->x, monster->y, &x, &y)) {
            free_item(index);
            continue;
        }
        put_item_on_floor(index, x, y);
        if (!is_tile_visible(x, y)) continue;

        uint8_t character = get_item_character(get_item(index)->type);
        uint8_t monster_index = find_monster_at(x, y);
        if (monster_index != NO_MONSTER) {
            get_monster(monster_index)->character_below = character;
        } else {
            draw_tile(x, y, character);
        }
    }
}

// a mágia elleni mentődobás (save_magic): igaz, ha a játékos ellenáll; magasabb szinten könnyebb
static bool roll_magic_save(void)
{
    // ide jön: a protection gyűrűk módja is könnyít
    return (int16_t)rnd(SAVING_THROW_SIDES) + 1 >= MAGIC_SAVE_TARGET - game.player.experience_level / 2;
}

// a leprechaun aranyának további dobásai összesen: LEPRECHAUN_EXTRA_GOLD_ROLLS külön aranydobás
static uint16_t roll_extra_leprechaun_gold(void)
{
    uint16_t amount = 0;
    for (uint8_t roll = 0; roll < LEPRECHAUN_EXTRA_GOLD_ROLLS; roll++) {
        amount += roll_gold_amount();
    }
    return amount;
}

// a megölt leprechaun aranya a csomagjába kerül, így vele együtt leesik: ha a mezője körül van hova esnie, és a szint a legmélyebb
// elért; egy aranydobásnyi, sikeres mágia elleni mentődobásnál a további dobásokkal több; kimarad, ha betelt a tárgytábla
static void add_leprechaun_gold(struct Monster *leprechaun)
{
    // a hely a leeséskor dől el, itt csak az számít, hogy van-e
    uint8_t x;
    uint8_t y;
    if (!choose_fall_position(leprechaun->x, leprechaun->y, &x, &y)) return;
    if (game.dungeon_level < game.max_level) return;

    uint8_t index = create_item(ITEM_TYPE_GOLD, 0);
    if (index == NO_ITEM) return;
    uint16_t amount = roll_gold_amount();
    if (roll_magic_save()) {
        amount += roll_extra_leprechaun_gold();
    }
    get_item(index)->properties.gold_amount = amount;
    insert_item_after(&leprechaun->first_pack_item, NO_ITEM, index);
}

// a szörny halála (killed): a játékos megkapja az XP-jét, a csomagja a környező mezőkre esik, a szörny kikerül a táblából,
// „you have defeated <név>”, és jöhet a szintlépés
static void kill_monster(struct Monster *monster)
{
    game.player.experience = capped_add(game.player.experience, monster->experience);
    // bármelyik Venus flytrap halálával a játékos kiszabadul
    if (monster->type == 'F') {
        game.flytrap_grip = 0;
    }
    if (monster->type == 'L') {
        add_leprechaun_gold(monster);
    }
    drop_monster_pack(monster);

    // a név a kivétel előtt, amíg a szörny még a helyén van
    const char *monster_name = prepare_monster_name(monster);
    remove_monster(monster);
    message_format(text_defeated, monster_name, NULL, 0);
    update_experience_level();
}

// az álcázott xeroc leleplezése: a jele a saját betűje lesz; igaz, ha álcázva volt
static bool reveal_xeroc(struct Monster *monster)
{
    // ide jön: vakon nincs leleplezés
    if (monster->type != 'X' || monster->disguise == 'X') return false;

    monster->disguise = 'X';
    // ide jön: hallucinálva a jele azonnal egy véletlen nagybetű
    return true;
}

bool attack_monster(struct Monster *monster)
{
    stop_running_and_repeating();
    // ide jön: a regen-számláló nullázása
    start_monster_running(monster);
    // az álcázott xeroc megtámadása csak leleplezi: sebzés nincs, de a kör eltelik
    if (reveal_xeroc(monster)) {
        // ide jön: hallucinálva „heavy! That's a nasty critter!”
        message(text_xeroc_revealed);
        return false;
    }

    const char *monster_name = prepare_monster_name(monster);
    bool did_hit = roll_player_attack(monster);
    report_attack(text_player_hits, text_player_misses, did_hit, text_you, monster_name);

    // ide jön: CANHUH
    if (did_hit && monster->hit_points <= 0) {
        kill_monster(monster);
    }
    return did_hit;
}

// az Ice monster fagyasztása: a fagyott körök összeadódnak, és ha túl sok lesz, a játékos megfagy;
// üzenet csak akkor, ha a játékos eddig cselekvőképes volt
static void freeze_player(const char *monster_name)
{
    if (game.turns_unable_to_act == 0) {
        message_format(text_frozen_by, monster_name, NULL, 0);
    }
    game.turns_unable_to_act += FREEZE_TURNS_MIN + rnd(FREEZE_TURNS_RANDOM);
    if (game.turns_unable_to_act > HYPOTHERMIA_TURNS) {
        kill_player(DEATH_CAUSE_HYPOTHERMIA);
    }
}

// a Vampire és a Wraith szívása: a HP és a max. HP ugyanannyival csökken, de a HP legalább 1 marad;
// ha a max. HP elfogy, a játékos meghal
static void drain_hit_points(uint8_t amount, uint8_t cause)
{
    game.player.hit_points -= amount;
    game.player.max_hit_points -= amount;
    if (game.player.hit_points <= 0) {
        game.player.hit_points = 1;
    }
    if (game.player.max_hit_points <= 0) {
        kill_player(cause);
        return;
    }
    message(text_suddenly_weaker);
}

// a Wraith szintszívása: egy szinttel lejjebb, és az XP az új szint alsó küszöbére áll, így a következő XP-szerzés
// nem adja vissza a szintet (J2); XP nélkül a játékos meghal
static void drain_experience_level(void)
{
    if (game.player.experience == 0) {
        kill_player('W');
        return;
    }
    game.player.experience_level--;
    if (game.player.experience_level <= 1) {
        game.player.experience_level = 1;
        game.player.experience = 0;
    } else {
        game.player.experience = experience_thresholds[game.player.experience_level - 2];
    }
    drain_hit_points(ROLL(1, WRAITH_DRAIN_SIDES), 'W');
}

// a mérgezés elleni mentődobás (save_poison): igaz, ha a játékos ellenáll; magasabb szinten könnyebb
static bool roll_poison_save(void)
{
    return (int16_t)rnd(SAVING_THROW_SIDES) + 1 >= POISON_SAVE_TARGET - game.player.experience_level / 2;
}

// a Venus flytrap megfogja a játékost, vagy szorosabban fogja: a szorítás eggyel nő (255-nél megáll), és már a fogás is sebez
static void grip_player(void)
{
    if (game.flytrap_grip < UINT8_MAX) {
        game.flytrap_grip++;
    }
    game.player.hit_points--;
    if (game.player.hit_points <= 0) {
        kill_player('F');
    }
}

// az Aquator rozsdája a viselt páncélon: a védelme eggyel romlik („your armor appears to be weaker now. Oh my!”); páncél nélkül,
// bőrpáncélon és RUST_ARMOR_CLASS_LIMIT védelemtől nincs hatása; a védett páncélról (ITEM_PROTECTED) f-harcon kívül
// „the rust vanishes instantly”
static void rust_armor(void)
{
    if (game.worn_armor == NO_ITEM) return;
    struct Item *armor = get_item(game.worn_armor);
    if (armor->which == ARMOR_LEATHER || armor->properties.armor_class >= RUST_ARMOR_CLASS_LIMIT) return;

    // ide jön: a maintain armor gyűrű is megvédi
    if (armor->flags & ITEM_PROTECTED) {
        if (!is_fight_active) {
            message(text_rust_vanishes);
        }
        return;
    }
    armor->properties.armor_class++;
    message(text_armor_weaker);
}

// a szörny eltűnik a szintről (a nimfa és a leprechaun a lopás után): a tárgyai vele vesznek, és kikerül a táblából
static void make_monster_vanish(struct Monster *monster)
{
    free_item_list(&monster->first_pack_item);
    remove_monster(monster);
}

// a leprechaun aranyat lop: egy aranydobásnyit, sikertelen mágia elleni mentődobásnál a további dobásokkal többet, de a pénz
// 0 alá nem megy; utána eltűnik; „your purse feels lighter”, ha a pénz csökkent
static void steal_gold(struct Monster *leprechaun)
{
    uint16_t amount = roll_gold_amount();
    if (!roll_magic_save()) {
        amount += roll_extra_leprechaun_gold();
    }
    uint32_t gold_before = game.gold;
    game.gold = game.gold > amount ? game.gold - amount : 0;

    make_monster_vanish(leprechaun);
    if (game.gold != gold_before) {
        message(text_purse_lighter);
    }
}

// a csomag egy használaton kívüli mágikus tárgya, egyenletes eséllyel; NO_ITEM, ha nincs ilyen
static uint8_t choose_item_to_steal(void)
{
    uint8_t chosen = NO_ITEM;
    uint8_t count = 0;
    for (uint8_t index = game.first_pack_item; index != NO_ITEM; index = get_item(index)->next) {
        if (is_item_in_use(index) || !is_magic_item(get_item(index))) continue;

        count++;
        if (rnd(count) == 0) {
            chosen = index;
        }
    }
    return chosen;
}

// a nimfa ellop egy használaton kívüli mágikus tárgyat, kötegből egy darabot (J31), és eltűnik vele; „she stole <a darab neve>!”;
// ha nincs ilyen tárgy, nincs hatása
static void steal_magic_item(struct Monster *nymph)
{
    uint8_t index = choose_item_to_steal();
    if (index == NO_ITEM) return;

    make_monster_vanish(nymph);

    struct Item *item = get_item(index);
    uint8_t count = item->count;
    item->count = 1;
    message_format(text_she_stole, format_item_name(item), NULL, 0);
    if (count > 1) {
        item->count = count - 1;
    } else {
        remove_item_from_list(&game.first_pack_item, index);
        free_item(index);
    }
}

// a szörny különleges hatása a sikeres találata után; a nimfa és a leprechaun közben eltűnhet, utána a mutató a következő szörnyre
// mutat (remove_monster)
static void apply_special_attack(struct Monster *monster, const char *monster_name)
{
    switch (monster->type) {
        case 'A':
            rust_armor();
            break;
        case 'F':
            grip_player();
            break;
        case 'I':
            freeze_player(monster_name);
            break;
        case 'L':
            steal_gold(monster);
            break;
        case 'N':
            steal_magic_item(monster);
            break;
        case 'R':
            if (!roll_poison_save()) {
                // ide jön: sustain strength gyűrűvel nincs erőcsökkenés, és f-harcon kívül „a bite momentarily weakens you”
                change_strength(-1);
                message(text_bite_weaker);
            }
            break;
        case 'V':
            if (rnd(100) < VAMPIRE_DRAIN_CHANCE) {
                drain_hit_points(ROLL(1, VAMPIRE_DRAIN_SIDES), 'V');
            }
            break;
        case 'W':
            if (rnd(100) < WRAITH_DRAIN_CHANCE) {
                drain_experience_level();
            }
            break;
    }
}

bool attack_player(struct Monster *monster)
{
    stop_running_and_repeating();
    // ide jön: a regen-számláló nullázása
    if (is_fight_active && !(monster->flags & MONSTER_TARGET)) {
        stop_fight();
    }
    // az álcázott xeroc támadása üzenet nélkül leleplezi, és a támadás normálisan lefut
    reveal_xeroc(monster);

    const char *monster_name = prepare_monster_name(monster);
    int16_t hit_points_before = game.player.hit_points;
    bool did_hit = roll_monster_attack(monster);
    // a Venus flytrap mellé ütése is sebez a szorításával, a kikapcsolt flytrapé is; ha ez öl, a miss üzenet elmarad
    if (!did_hit && monster->type == 'F') {
        game.player.hit_points -= game.flytrap_grip;
    }
    // az Ice monster támadása nem ír hit/miss üzenetet: a találatáról a fagyasztás szól
    if (monster->type != 'I' && (did_hit || game.player.hit_points > 0)) {
        report_attack(text_monster_hits, text_monster_misses, did_hit, monster_name, text_you);
    }

    // a halállal minden véget ér: az f-harc sem folytatódik, a speciális támadás elmarad
    if (game.player.hit_points <= 0) {
        kill_player(monster->type);
        stop_fight();
        return did_hit;
    }
    if (did_hit && is_fight_active && !is_fight_kamikaze) {
        int16_t damage = hit_points_before - game.player.hit_points;
        if (damage > max_hit_this_fight) {
            max_hit_this_fight = damage;
        }
        if (game.player.hit_points <= max_hit_this_fight) {
            stop_fight();
        }
    }
    if (did_hit && !(monster->flags & MONSTER_CANCELLED)) {
        apply_special_attack(monster, monster_name);
        // a különleges hatás is ölhet: ilyenkor az f-harc sem folytatódik
        if (is_player_dead()) {
            stop_fight();
        }
    }
    return did_hit;
}

bool start_fight(int8_t step_x, int8_t step_y, bool is_kamikaze)
{
    // a 0. oszlop (sor) előtti mező 255 lesz: azt az is_on_map kívülinek veszi
    uint8_t target_x = game.player.x + step_x;
    uint8_t target_y = game.player.y + step_y;
    uint8_t monster_index = is_on_map(target_x, target_y) ? find_monster_at(target_x, target_y) : NO_MONSTER;

    // ide jön: szörnyérzékeléssel (SEEMONST) a nem látott szörny is lehet célpont
    if (monster_index == NO_MONSTER || !is_monster_visible(get_monster(monster_index))) {
        message(text_no_monster_there);
        return false;
    }
    // szabálytalan átlónál semmi nem történik (J15)
    if (!is_diagonal_step_allowed(game.player.x, game.player.y, target_x, target_y)) return false;

    get_monster(monster_index)->flags |= MONSTER_TARGET;
    is_fight_active = true;
    fight_step_x = step_x;
    fight_step_y = step_y;
    is_fight_kamikaze = is_kamikaze;
    max_hit_this_fight = 0;
    return take_fight_step();
}

void stop_fight(void)
{
    is_fight_active = false;
    for (uint8_t index = 1; index <= game.monster_count; index++) {
        get_monster(index)->flags &= ~MONSTER_TARGET;
    }
}

bool is_fighting(void)
{
    return is_fight_active;
}

bool take_fight_step(void)
{
    if (!move_player(fight_step_x, fight_step_y)) {
        stop_fight();
        return false;
    }
    return true;
}

#pragma optimize(pop)
