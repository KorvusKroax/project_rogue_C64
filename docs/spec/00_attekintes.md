# 0. Áttekintés

A *Rogue: Exploring the Dungeons of Doom* 5.4.4-es változatának specifikációja **Commodore 64-re**: a játék minden szabálya, képlete, táblázata és üzenete, valamint mindaz, amit a C64 hardvere miatt másképp kell megoldani. A program ez alapján, az eredeti forrás nélkül megírható.

**Futási környezet:** Commodore 64 (PAL vagy NTSC), 40×25 karakteres képernyő saját karakterkészlettel, a C64 billentyűzete (SHIFT, CTRL, RUN/STOP), 1541-kompatibilis lemezegység (8–11. eszköz) a mentéshez, a ranglistához és a beállításokhoz. A hardverközeli részletek: [10. Platform](10_platform.md).

**Jelölések:**
- A pszeudokód C-szerű, egész aritmetikával (osztás lefelé kerekít). „MUST”-szintű szabály minden, ami le van írva – a szokatlannak tűnő viselkedések is (ezeket *Megjegyzés* jelöli).
- **C64-eltérés (E*n*)**: a platform miatt az eredetitől eltérő megoldás (képernyő, billentyűzet, lemez, memória).
- **C64-javítás (J*n*)**: az eredeti forrás egy hibájának vagy kérdéses viselkedésének javítása. Mindegyiknél le van írva az *eredeti* viselkedés is.
- A játék adattáblái annál a fejezetnél vannak, amelyik a szabályaikat leírja; az összes üzenetét a [függelék](fuggelek_uzenetkatalogus.md) tartalmazza. A játék szövegei angolul maradnak.

**A játék röviden:**
- Körökre osztott, egyképernyős, karakteres roguelike.
- A játékos az 1. dungeon szinten kezd. A cél: megszerezni az **Amulettet** (a 26. szinten és mélyebben jelenik meg), majd felmenni vele a lépcsőkön a felszínre. Győzelem: az 1. szinten `<` az Amulettel (`level` 0-ra csökken).
- Minden szint új, véletlenszerűen generált; a korábbi szintek nem tárolódnak (lépcsőn bármelyik irányba mindig új szint generálódik).
- A szint logikailag 80×22 mezős (mint az eredetiben); a 40 oszlopos képernyőn egy görgetett ablak mutatja ([3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés)).
- Halál: HP ≤ 0, éhhalál, fagyás (Ice monster), szint/maxhp-elszívás.

## 0.1 Indítás

> **C64-eltérés (E5):** parancssor, környezeti változó és felhasználónév nincs. Az eredeti `rogue`, `rogue <fájl>`, `rogue -r`, `rogue -s` kapcsolók helyett címképernyő-menü van; a `rogue -d` teszt-halál a fejlesztői mód része (lásd az M03-04 issue-t). A terminálméret-ellenőrzés („Sorry, the screen must be at least 24x80”) kimarad.

```
bekapcsolás / betöltés után:
  hardver előkészítése (10.2): memória-konfiguráció, képernyő, karakterkészlet, megszakítás, billentyűzet
  rogue.cfg beolvasása a lemezről, ha van (4.2.5); hiba esetén csendben az alapértékek
  címképernyő:
     Rogue: Exploring the Dungeons of Doom
     (c64 version of rogue 5.4.4)
     N  new game
     R  restore saved game
     S  top ten scores
     O  options
  N: "what is your name? " (alapérték: a legutóbbi név a rogue.cfg-ből, ennek hiányában "rogue"; max. 15 karakter)
     seed beállítása (0.2)
     "Hello <név>, just a moment while I dig the dungeon..."
     init: valószínűségi táblák, játékos (5.1), tekercsnevek, bájitalszínek, gyűrűkövek, pálcaanyagok (8.8)
     new_level() (2)
     démonok indítása: szörnymozgás, regeneráció, éhség (AFTER); vándorló-szörny időzítő spread(70) (AFTER)
     főciklus (4.1)
  R: mentett játék betöltése (4.2.5), utána főciklus
  S: ranglista (4.3), szóközre vissza a címképernyőre
  O: beállítások (4.2.5), utána vissza a címképernyőre
játék vége (halál, kilépés, győzelem, mentés) után: vissza a címképernyőre
```
A forrásban lévő „varázsló” (wizard) mód egy külön fordítási változat hibakereső eszköze, a játék része nem; helyette a fejlesztői mód szolgál (lásd az M03-04 issue-t), amely csak külön fordítási beállítással kerül a programba.

---

## 0.2 Alapprimitívek

```
RN              -> seed = seed * 11109 + 13849;  return (seed >> 16) & 0xFFFF  // 32 bites aritmetika, túlcsordulás eldobva
rnd(n)          -> n == 0 ? 0 : RN % |n|  // 0..|n|-1; n == 0-nál az RN nem fut le
ROLL(count, sides) -> count darab sides oldalú kocka összege: sum(i<count) (rnd(sides) + 1)
spread(n)       -> n - n/20 + rnd(n/10)      // kb. n * 0.95..1.05
dist2(a, b)     -> (a.x-b.x)^2 + (a.y-b.y)^2  // legfeljebb 79²+21² = 6682, 16 biten elfér
```
- Az `RN` a `seed` 16..31. bitjét adja, ezért mindig 0..65535, és az eredeti `abs()` elhagyható. A szorzó (11109 = $2B65) 16 bites, így a lépés egy 32×16 bites szorzás és egy összeadás (6502-n gyors rutinnal, [10.5](10_platform.md#105-aritmetika)).
- Az `rnd` maradékos osztása 16 bites előjel nélküli osztás.

> **C64-eltérés (E13):** **seed** – az eredeti `idő + folyamatazonosító` helyett: a címképernyőn az `N` lenyomásakor a CIA1 A időzítőjének (16 bit), a raszter-sornak és a jiffy-órának (3 bájt) összekeverése 32 bitre ([10.6](10_platform.md#106-időzítés)). Fejlesztői módban a mag kézzel is megadható. A ranglistán az eredetiben szereplő játéksorszám (`dnum`) nincs.

**Sebzés-string** (`"NxM"` vagy `"NxM/NxM/..."`): `/` mentén szegmensekre bontva, mindegyik szegmens egy önálló támadás `ROLL(N, M)` sebzéssel. `"0x0"` = nulla sebzés (a találat ettől még számít, pl. speciális hatásnál). C64-en nem szövegként, hanem bájtpárok listájaként tárolódik ([1.4](01_adatmodell_es_flagek.md#14-ábrázolás-c64-en)); a táblákban az olvashatóság kedvéért szövegként szerepel.

**Irányok** (8 irány, vi-billentyűk):

| bill. | dx,dy | bill. | dx,dy |
|---|---|---|---|
| h | -1, 0 | y | -1,-1 |
| j | 0, +1 | u | +1,-1 |
| k | 0, -1 | b | -1,+1 |
| l | +1, 0 | n | +1,+1 |

---

## 0.3 Eltérések és javítások jegyzéke

### C64-eltérések (platform)
| # | röviden | hol |
|---|---|---|
| E1 | 40×25 képernyő; a 80×22-es szint görgetett nézőablakban | [1.3](01_adatmodell_es_flagek.md#13-térkép-és-képernyő), [3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés) |
| E2 | kétsoros üzenetsáv szótördeléssel (a 2. sor ideiglenesen a térkép fölé); a MAXMSG = 71 marad | [3.4](03_latoter_es_megjelenites.md#34-üzenetsor) |
| E3 | kétsoros státuszsor (23–24. sor) | [3.3](03_latoter_es_megjelenites.md#33-státuszsor) |
| E4 | C64 billentyűzet: mátrixolvasás, ESC = RUN/STOP vagy `←`, ismétlés | [4.2.0](04_jatekciklus_es_parancsok.md#420-billentyűzet) |
| E5 | címképernyő-menü a parancssor helyett; nincs terminálméret-ellenőrzés | [0.1](#01-indítás) |
| E6 | beállítások: menü + `rogue.cfg` a `ROGUEOPTS` helyett; új `device` opció; `inven` alapértéke clear; nincs baud-függés | [4.2.5](04_jatekciklus_es_parancsok.md#425-rendszer) |
| E7 | mentés lemezre; nincs link-/jogosultság-ellenőrzés és szignálos mentés | [4.2.5](04_jatekciklus_es_parancsok.md#425-rendszer), [10.7](10_platform.md#107-lemezkezelés) |
| E8 | ranglista lemezen, név szerinti egyediséggel, zárolás nélkül | [4.3](04_jatekciklus_es_parancsok.md#43-szintváltás-és-játék-vége) |
| E9 | nincs alshell (`!`) | [4.2.5](04_jatekciklus_es_parancsok.md#425-rendszer) |
| E10 | fix méretű táblák (szörny, tárgy, címke), betelt táblánál meghatározott viselkedés | [1.4](01_adatmodell_es_flagek.md#14-ábrázolás-c64-en) |
| E11 | 40 oszlopra tervezett képernyők: csomaglista, súgó (lapozva), sírkő, győzelem, ranglista, észlelés-képernyők | [3.2](03_latoter_es_megjelenites.md#32-állapotok-hatása-a-kijelzésre), [4.2.6](04_jatekciklus_es_parancsok.md#426-tárgyválasztás-listák-és-tárgyparancsok), [4.3](04_jatekciklus_es_parancsok.md#43-szintváltás-és-játék-vége) |
| E12 | szöveghosszak: név, gyümölcs ≤ 15, elnevezés ≤ 20, fájlnév ≤ 16 karakter | [1.4](01_adatmodell_es_flagek.md#14-ábrázolás-c64-en) |
| E13 | seed a hardveres időzítőkből | [0.2](#02-alapprimitívek) |
| E14 | a játékban hatástalan mezők (`turn`, csapdalista, `F_DROPPED`, `ISREGEN`) elhagyhatók | [1.1](01_adatmodell_es_flagek.md#11-rekordok) |
| E15 | animációk (dobás, sugár) képkockában időzítve | [10.6](10_platform.md#106-időzítés) |
| E16 | a mélység legfeljebb 255 | [4.3](04_jatekciklus_es_parancsok.md#43-szintváltás-és-játék-vége) |
| E17 | a sírkövön nincs évszám (nincs valós idejű óra) | [4.3](04_jatekciklus_es_parancsok.md#43-szintváltás-és-játék-vége) |

### C64-javítások (az eredeti hibái)
| # | röviden | hol |
|---|---|---|
| J1 | éhség-állapotváltás nem veszi el a játékos ISRUN-ját (eredetileg: +4 a szörnyeknek) | [5.4](05_jatekos.md#54-éhség) |
| J2 | a Wraith szintszívása tartós (eredetileg a következő XP visszaadta a szintet) | [6.7](06_szornyek.md#67-speciális-támadások-sikeres-találat-után-ha-nincs-iscanc) |
| J3 | `find_dest`: a foglaltság-ellenőrzés nem írja felül a vizsgált szörnyet | [6.5](06_szornyek.md#65-célpont) |
| J4 | a Venus flytrap sebzése nem módosítja a típustáblát | [6.7](06_szornyek.md#67-speciális-támadások-sikeres-találat-után-ha-nincs-iscanc) |
| J5 | see invisible: a bájital és a gyűrű hatása független egymástól | [4.1.1](04_jatekciklus_es_parancsok.md#411-időzített-hatások-fuse-ok), [8.2](08_targyak.md#82-bájitalok-q) |
| J6 | a sárkány lángja által sebzett szörny nem a játékos támadásaként számít | [7.6](07_harc.md#76-elemi-sugár-lightning--fire--cold-pálca-sárkánytűz) |
| J7 | kincses szobában nem kerül két tárgy egy mezőre | [2.8](02_szintgeneralas.md#28-tárgyak-put_things) |
| J8 | a „vanishes as it hits the ground” üzenet a tárgy saját nevét használja | [7.4](07_harc.md#74-hajítás-t) |
| J9 | a magic missile fegyver nélkül is sebez | [8.7](08_targyak.md#87-pálcákbotok-z) |
| J10 | a nyíl- és dartcsapdánál a viselt páncél és a védelem-gyűrű is számít | [7.5](07_harc.md#75-csapda-támadás-nyíldart) |
| J11 | a státuszsor Arm értéke a védelem-gyűrűt is tartalmazza | [3.3](03_latoter_es_megjelenites.md#33-státuszsor) |
| J12 | a dobott tárgy, a sugár és a lövedék nem repül át az álcázott Xerocon | [7.4](07_harc.md#74-hajítás-t) |
| J13 | a Medusa pillantásához a Medusa szobájának világossága számít | [6.4](06_szornyek.md#64-ébresztés) |
| J14 | a haste self ivása is időbe kerül | [8.2](08_targyak.md#82-bájitalok-q) |
| J15 | `f` szabálytalan átlós irányba nem visz el időt | [4.2.2](04_jatekciklus_es_parancsok.md#422-harc-és-célzás) |
| J16 | lépcső nem kerül rejtett csapdára | [2.10](02_szintgeneralas.md#210-lépcső) |
| J17 | elírások javítása az üzenetekben | [függelék](fuggelek_uzenetkatalogus.md) |
