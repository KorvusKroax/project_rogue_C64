#include "potions.h"

#include <stdbool.h>
#include <stddef.h>

#include "explore.h"
#include "item_names.h"
#include "items.h"
#include "message.h"
#include "monsters.h"
#include "pack.h"
#include "player.h"
#include "rng.h"
#include "state.h"
#include "texts.h"
#include "timers.h"

// nem időkritikus kód: méretre fordítva, itt a hely fontosabb a sebességnél
#pragma optimize(push)
#pragma optimize(size)

// a spread(n) legnagyobb értéke; az időzítők 16 bitesek
#define SPREAD_MAX(n)  ((n) - (n) / 20 + (n) / 10 - 1)

#if SPREAD_MAX(CONFUSION_DURATION) > 0xffff || SPREAD_MAX(HALLUCINATION_DURATION) > 0xffff || SPREAD_MAX(SEE_INVISIBLE_DURATION) > 0xffff \
    || SPREAD_MAX(BLINDNESS_DURATION) > 0xffff || SPREAD_MAX(LEVITATION_DURATION) > 0xffff || MONSTER_DETECTION_DURATION > 0xffff \
    || HASTE_DURATION_MIN + HASTE_DURATION_RANGE - 1 > 0xffff
    #error "a bájital hatásának ideje nem fér a 16 bites időzítőbe"
#endif

// a cselekvésképtelenség számlálója 8 bites
#if HASTE_FAINT_TURNS_RANGE - 1 > 0xff
    #error "az ájulás ideje nem fér a 8 bites számlálóba"
#endif

// az erő változása előjeles 8 bites (change_strength)
#if POISON_STRENGTH_LOSS_MIN + POISON_STRENGTH_LOSS_RANGE - 1 > 127
    #error "a méreg erővesztése nem fér az erő változásába"
#endif

// a gyógyítás (healing, extra healing): a HP a szintnyi kockával nő; ha így a max. HP fölé ér, a max. HP eggyel nő, az extra healingnél
// kettővel, ha a HP a max. HP + szint + 1-et is meghaladta, és a HP a max. HP lesz
static void heal_player(bool is_extra)
{
    uint8_t die_sides = is_extra ? EXTRA_HEALING_DIE_SIDES : HEALING_DIE_SIDES;
    game.player.hit_points += ROLL(game.player.experience_level, die_sides);
    if (game.player.hit_points <= game.player.max_hit_points) return;

    if (is_extra && game.player.hit_points > game.player.max_hit_points + game.player.experience_level + 1) {
        game.player.max_hit_points++;
    }
    game.player.max_hit_points++;
    game.player.hit_points = game.player.max_hit_points;
}

// raise level: az XP a következő szint küszöbe + 1, és a szint ehhez igazodik; a legmagasabb szinten nincs következő, és semmi
// nem változik (J36)
static void raise_experience_level(void)
{
    uint8_t level = game.player.experience_level;
    if (level == EXPERIENCE_LEVEL_MAX) return;

    game.player.experience = experience_thresholds[level - 1] + 1;
    update_experience_level();
}

// a szinten van-e szörny, amelyet a játékos nem lát
static bool has_unseen_monster(void)
{
    for (uint8_t index = game.monster_count; index != NO_MONSTER; index--) {
        if (!is_monster_visible(get_monster(index))) return true;
    }
    return false;
}

// a tárgylistában (a padlón lévő tárgyak, egy szörny csomagja) van-e mágikus tárgy (is_magic_item)
static bool has_magic_item(uint8_t list_first)
{
    for (uint8_t index = list_first; index != NO_ITEM; index = get_item(index)->next) {
        if (is_magic_item(get_item(index))) return true;
    }
    return false;
}

// a magic detection mutatna-e valamit: csak ha a padlón van tárgy (az arany is), és akkor a padló vagy valamelyik szörny csomagja
// mágikus tárgyat tartalmaz (3.2)
static bool has_magic_to_show(void)
{
    if (game.first_floor_item == NO_ITEM) return false;
    if (has_magic_item(game.first_floor_item)) return true;
    for (uint8_t index = game.monster_count; index != NO_MONSTER; index--) {
        if (has_magic_item(get_monster(index)->first_pack_item)) return true;
    }
    return false;
}

// a gyorsítás: ha már fut, üzenet nélkül véget ér, és a játékos elájul; különben elindul
static void haste_player(void)
{
    if (game.timers[TIMER_HASTE] == 0) {
        game.timers[TIMER_HASTE] = rnd(HASTE_DURATION_RANGE) + HASTE_DURATION_MIN;
        message(text_moving_faster);
        return;
    }

    game.timers[TIMER_HASTE] = 0;
    // a játékos épp cselekszik, így a számláló eddig 0 volt
    game.turns_unable_to_act = rnd(HASTE_FAINT_TURNS_RANGE);
    message(text_faint_from_exhaustion);
}

// a bájital hatása (8.2); ide jön: hallucinálva az üzenetek hallucinált változata
static void apply_potion_effect(uint8_t which)
{
    switch (which) {
        case POTION_CONFUSION:
            extend_timer(TIMER_CONFUSION, CONFUSION_DURATION);
            message(text_huh_what_who);
            break;
        case POTION_HALLUCINATION:
            // ide jön: ha eddig nem hallucinált, a szörnyérzékelés jeleinek és a hallucinációnak a kirajzolása
            extend_timer(TIMER_HALLUCINATION, HALLUCINATION_DURATION);
            message(text_seems_cosmic);
            break;
        case POTION_POISON:
            // ide jön: sustain strength gyűrűvel csak „you feel momentarily sick”
            change_strength(-(int8_t)(rnd(POISON_STRENGTH_LOSS_RANGE) + POISON_STRENGTH_LOSS_MIN));
            message(text_very_sick);
            end_timer_early(TIMER_HALLUCINATION);
            break;
        case POTION_GAIN_STRENGTH:
            change_strength(1);
            message(text_feel_stronger);
            break;
        case POTION_SEE_INVISIBLE:
            // ide jön: ha eddig nem volt CANSEE, a látható láthatatlan szörnyek kirajzolása
            extend_timer(TIMER_SEE_INVISIBLE, SEE_INVISIBLE_DURATION);
            message_format(text_tastes_like_juice, text_fruit_name, NULL, 0);
            end_timer_early(TIMER_BLINDNESS);
            break;
        case POTION_HEALING:
            heal_player(false);
            end_timer_early(TIMER_BLINDNESS);
            message(text_feel_better);
            break;
        case POTION_MONSTER_DETECTION:
            // ide jön: minden szörny kirajzolása, a nem látottak inverzen
            if (game.timers[TIMER_MONSTER_DETECTION] == 0) {
                game.timers[TIMER_MONSTER_DETECTION] = MONSTER_DETECTION_DURATION;
            }
            if (!has_unseen_monster()) {
                message_format(text_passing_feeling, text_strange, NULL, 0);
            }
            break;
        case POTION_MAGIC_DETECTION:
            if (has_magic_to_show()) {
                // ide jön: a mágia-képernyő a padló és a szörnyek csomagjainak mágikus tárgyaival, „You sense the presence of magic on this level”
                identify_item_type(ITEM_TYPE_POTION, POTION_MAGIC_DETECTION);
            } else {
                message_format(text_passing_feeling, text_strange, NULL, 0);
            }
            break;
        case POTION_RAISE_LEVEL:
            message(text_more_skillful);
            raise_experience_level();
            break;
        case POTION_EXTRA_HEALING:
            heal_player(true);
            end_timer_early(TIMER_BLINDNESS);
            end_timer_early(TIMER_HALLUCINATION);
            message(text_feel_much_better);
            break;
        case POTION_HASTE_SELF:
            haste_player();
            break;
        case POTION_RESTORE_STRENGTH:
            // ide jön: a viselt add strength gyűrűk modja előtte lejön, utána vissza (5.6)
            if (game.player.strength < game.max_strength) {
                game.player.strength = game.max_strength;
            }
            message(text_feel_warm);
            break;
        case POTION_BLINDNESS:
            // ide jön: a kijelzés frissítése
            extend_timer(TIMER_BLINDNESS, BLINDNESS_DURATION);
            message(text_cloak_of_darkness);
            break;
        case POTION_LEVITATION:
            extend_timer(TIMER_LEVITATION, LEVITATION_DURATION);
            message(text_start_to_float);
            break;
    }
}

void quaff_potion(uint8_t index)
{
    const struct Item *item = get_item(index);
    if (item->type != ITEM_TYPE_POTION) {
        message(text_undrinkable);
        return;
    }

    // az altípus a darab elfogyása előtt: az utolsó darab rekordja felszabadul
    uint8_t which = item->which;
    use_up_pack_item(index);
    apply_potion_effect(which);
    // ide jön: az automatikus azonosítás, és ha a típus így sem ismert, és nincs elnevezve, a „what do you want to call it?” kérdés
}

#pragma optimize(pop)
