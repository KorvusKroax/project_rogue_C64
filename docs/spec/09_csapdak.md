# 9. Csapdák

## 9.1 Rálépés
```
trigger_trap(pos):
  if player.ISLEVIT: return               // lebegve a rejtett csapda rejtve is marad
  F_REAL be a pos-on; SCREEN[pos] = TRAP  // felfedődik
  trap_type = a pos csapdatípusa (F_TMASK)
  futás/ismétlés megszakad
  switch trap_type:
    TRAPDOOR: level = min(level + 1, 255) (E16); new_level() (max_level frissül); "you fell into a trap!"
    ARROW:    7.5 (a viselt páncél is számít, J10)
    SLEEP:    no_command += 5; ISRUN le; "a strange white mist envelops you and you fall asleep"
    BEAR:     bear_trap += 3; "you are caught in a bear trap"
    TELEPORT: teleport()   (a csapda jele a régi helyén megmarad)
    DART:     7.5
    RUST:     "a gush of water hits you on the head"; rust_armor()  (6.7)
    MYSTERY:  rnd(11) szerint egy üzenet, hatás nincs ({szín} = véletlen bájitalszín, 8.8):
      "you are suddenly in a parallel dimension"
      "the light in here suddenly seems {szín}"
      "you feel a sting in the side of your neck"
      "multi-colored lines swirl around you, then fade"
      "a {szín} light flashes in your eyes"
      "a spike shoots past your ear!"
      "{szín} sparks dance across your armor"
      "you suddenly feel very thirsty"
      "you feel time speed up suddenly"
      "time now seems to be going slower"
      "your pack turns {szín}!"                       // J17: eredetileg "you pack turns"
  billentyűpuffer ürítése
```
Rálépéskor a játék nem ír csapdanevet, így hallucináció alatt sem cserél nevet; a véletlen név a `^` paranccsal és a keresés („you found …”) üzenetében jelenik meg.
A csapdák állandók, minden rálépéskor újra aktiválódnak.

## 9.2 Keresés (`s`, searching gyűrű)
```
search():
  for each of 8 neighbours cell, ha rejtett (F_REAL nélkül):
    penalty = (ISHALU ? 3 : 0) + (ISBLIND ? 2 : 0)  // hallucinálva és vakon nehezebb megtalálni
    titkos ajtó (fal jele):       if rnd(5 + penalty) == 0: MAP[cell] = DOOR; reveal(cell); "a secret door"
    rejtett csapda (padló jele):  if rnd(2 + penalty) == 0: MAP[cell] = TRAP; reveal(cell)
                                    "you found <csapdanév>"   // hallucinálva véletlen csapdanév, különben a valódi, és a mező F_SEEN-t kap
    titkos folyosó (kőzet jele):  if rnd(3 + penalty) == 0: MAP[cell] = PASSAGE; reveal(cell)
  reveal(cell): F_REAL be, futás/ismétlés megszakad
  // a felfedett mezőt a következő look rajzolja ki (3.1)
```
