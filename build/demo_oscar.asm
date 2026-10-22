; Compiled with 1.32.273.0
--------------------------------------------------------------------
startup: ; startup
0801 : 0b __ __ INV
0802 : 08 __ __ PHP
0803 : 0a __ __ ASL
0804 : 00 __ __ BRK
0805 : 9e __ __ INV
0806 : 32 __ __ INV
0807 : 30 36 __ BMI $083f ; (startup + 62)
0809 : 31 00 __ AND ($00),y 
080b : 00 __ __ BRK
080c : 00 __ __ BRK
080d : ba __ __ TSX
080e : 8e b7 19 STX $19b7 ; (spentry + 0)
0811 : a2 19 __ LDX #$19
0813 : a0 d8 __ LDY #$d8
0815 : a9 00 __ LDA #$00
0817 : 85 19 __ STA IP + 0 
0819 : 86 1a __ STX IP + 1 
081b : e0 19 __ CPX #$19
081d : f0 0b __ BEQ $082a ; (startup + 41)
081f : 91 19 __ STA (IP + 0),y 
0821 : c8 __ __ INY
0822 : d0 fb __ BNE $081f ; (startup + 30)
0824 : e8 __ __ INX
0825 : d0 f2 __ BNE $0819 ; (startup + 24)
0827 : 91 19 __ STA (IP + 0),y 
0829 : c8 __ __ INY
082a : c0 d8 __ CPY #$d8
082c : d0 f9 __ BNE $0827 ; (startup + 38)
082e : a9 00 __ LDA #$00
0830 : a2 f7 __ LDX #$f7
0832 : d0 03 __ BNE $0837 ; (startup + 54)
0834 : 95 00 __ STA $00,x 
0836 : e8 __ __ INX
0837 : e0 f7 __ CPX #$f7
0839 : d0 f9 __ BNE $0834 ; (startup + 51)
083b : a9 9d __ LDA #$9d
083d : 85 23 __ STA SP + 0 
083f : a9 9f __ LDA #$9f
0841 : 85 24 __ STA SP + 1 
0843 : 20 80 08 JSR $0880 ; (main.s1 + 0)
0846 : a9 4c __ LDA #$4c
0848 : 85 54 __ STA $54 
084a : a9 00 __ LDA #$00
084c : 85 13 __ STA P6 
084e : a9 19 __ LDA #$19
0850 : 85 16 __ STA P9 
0852 : 60 __ __ RTS
--------------------------------------------------------------------
main: ; main()->i16
;  34, "C:/Users/korvus/Projects/C64/project_rogue_C64/src/demo.c"
.s1:
0880 : a5 53 __ LDA T1 + 0 
0882 : 8d 9f 9f STA $9f9f ; (main@stack + 0)
.s4:
0885 : a9 93 __ LDA #$93
0887 : 20 d2 ff JSR $ffd2 
088a : a9 00 __ LDA #$00
088c : 85 53 __ STA T1 + 0 
.l5:
088e : a9 77 __ LDA #$77
0890 : 8d fa 9f STA $9ffa ; (sstack + 0)
0893 : a9 13 __ LDA #$13
0895 : 8d fb 9f STA $9ffb ; (sstack + 1)
0898 : a5 53 __ LDA T1 + 0 
089a : 8d fc 9f STA $9ffc ; (sstack + 2)
089d : ad d4 19 LDA $19d4 ; (seed + 0)
08a0 : 85 1b __ STA ACCU + 0 
08a2 : ad d5 19 LDA $19d5 ; (seed + 1)
08a5 : 85 1c __ STA ACCU + 1 
08a7 : ad d6 19 LDA $19d6 ; (seed + 2)
08aa : 85 1d __ STA ACCU + 2 
08ac : ad d7 19 LDA $19d7 ; (seed + 3)
08af : 85 1e __ STA ACCU + 3 
08b1 : a9 00 __ LDA #$00
08b3 : 8d fd 9f STA $9ffd ; (sstack + 3)
08b6 : 85 05 __ STA WORK + 2 
08b8 : 85 06 __ STA WORK + 3 
08ba : a9 65 __ LDA #$65
08bc : 85 03 __ STA WORK + 0 
08be : a9 2b __ LDA #$2b
08c0 : 85 04 __ STA WORK + 1 
08c2 : 20 09 18 JSR $1809 ; (mul32 + 0)
08c5 : 18 __ __ CLC
08c6 : a5 07 __ LDA WORK + 4 
08c8 : 69 19 __ ADC #$19
08ca : 8d d4 19 STA $19d4 ; (seed + 0)
08cd : a5 08 __ LDA WORK + 5 
08cf : 69 36 __ ADC #$36
08d1 : 8d d5 19 STA $19d5 ; (seed + 1)
08d4 : a5 09 __ LDA WORK + 6 
08d6 : 69 00 __ ADC #$00
08d8 : 85 1b __ STA ACCU + 0 
08da : 8d d6 19 STA $19d6 ; (seed + 2)
08dd : a5 0a __ LDA WORK + 7 
08df : 69 00 __ ADC #$00
08e1 : 85 1c __ STA ACCU + 1 
08e3 : 8d d7 19 STA $19d7 ; (seed + 3)
08e6 : a9 64 __ LDA #$64
08e8 : 20 c9 16 JSR $16c9 ; (divmod + 53)
08eb : a5 05 __ LDA WORK + 2 
08ed : 8d fe 9f STA $9ffe ; (sstack + 4)
08f0 : a9 00 __ LDA #$00
08f2 : 8d ff 9f STA $9fff ; (sstack + 5)
08f5 : 20 2f 09 JSR $092f ; (printf.s1 + 0)
08f8 : e6 53 __ INC T1 + 0 
08fa : a5 53 __ LDA T1 + 0 
08fc : c9 0a __ CMP #$0a
08fe : 90 8e __ BCC $088e ; (main.l5 + 0)
.s6:
0900 : a9 43 __ LDA #$43
0902 : a2 00 __ LDX #$00
.l7:
0904 : 9d 84 04 STA $0484,x 
0907 : 9d 24 05 STA $0524,x 
090a : e8 __ __ INX
090b : e0 0a __ CPX #$0a
090d : d0 f5 __ BNE $0904 ; (main.l7 + 0)
.s8:
090f : a9 42 __ LDA #$42
0911 : 8d ac 04 STA $04ac 
0914 : 8d b5 04 STA $04b5 
0917 : 8d d4 04 STA $04d4 
091a : 8d dd 04 STA $04dd 
091d : 8d fc 04 STA $04fc 
0920 : 8d 05 05 STA $0505 
0923 : a9 00 __ LDA #$00
0925 : 85 1b __ STA ACCU + 0 
0927 : 85 1c __ STA ACCU + 1 
.s3:
0929 : ad 9f 9f LDA $9f9f ; (main@stack + 0)
092c : 85 53 __ STA T1 + 0 
092e : 60 __ __ RTS
--------------------------------------------------------------------
printf: ; printf(const u8*)->void
;  18, "C:/oscar64/include/stdio.h"
.s1:
092f : a2 08 __ LDX #$08
0931 : b5 53 __ LDA T1 + 0,x 
0933 : 9d a3 9f STA $9fa3,x ; (printf@stack + 0)
0936 : ca __ __ DEX
0937 : 10 f8 __ BPL $0931 ; (printf.s1 + 2)
.s4:
0939 : ad fa 9f LDA $9ffa ; (sstack + 0)
093c : 85 58 __ STA T5 + 0 
093e : a9 fc __ LDA #$fc
0940 : 85 56 __ STA T3 + 0 
0942 : a9 9f __ LDA #$9f
0944 : 85 57 __ STA T3 + 1 
0946 : a9 00 __ LDA #$00
0948 : 85 5b __ STA T7 + 0 
094a : ad fb 9f LDA $9ffb ; (sstack + 1)
094d : 85 59 __ STA T5 + 1 
.l5:
094f : a0 00 __ LDY #$00
0951 : b1 58 __ LDA (T5 + 0),y 
0953 : d0 15 __ BNE $096a ; (printf.s8 + 0)
.s6:
0955 : a6 5b __ LDX T7 + 0 
0957 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
095a : f0 03 __ BEQ $095f ; (printf.s3 + 0)
.s7:
095c : 20 8e 0c JSR $0c8e ; (puts@proxy + 0)
.s3:
095f : a2 08 __ LDX #$08
0961 : bd a3 9f LDA $9fa3,x ; (printf@stack + 0)
0964 : 95 53 __ STA T1 + 0,x 
0966 : ca __ __ DEX
0967 : 10 f8 __ BPL $0961 ; (printf.s3 + 2)
0969 : 60 __ __ RTS
.s8:
096a : c9 25 __ CMP #$25
096c : f0 28 __ BEQ $0996 ; (printf.s12 + 0)
.s9:
096e : a6 5b __ LDX T7 + 0 
0970 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0973 : e6 58 __ INC T5 + 0 
0975 : d0 02 __ BNE $0979 ; (printf.s110 + 0)
.s109:
0977 : e6 59 __ INC T5 + 1 
.s110:
0979 : e8 __ __ INX
097a : 86 5b __ STX T7 + 0 
097c : e0 28 __ CPX #$28
097e : 90 cf __ BCC $094f ; (printf.l5 + 0)
.s10:
0980 : a9 b8 __ LDA #$b8
0982 : 85 0d __ STA P0 
0984 : a9 9f __ LDA #$9f
0986 : 85 0e __ STA P1 
0988 : 98 __ __ TYA
0989 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
.s11:
098c : 20 96 0c JSR $0c96 ; (puts.s4 + 0)
098f : a9 00 __ LDA #$00
.s92:
0991 : 85 5b __ STA T7 + 0 
0993 : 4c 4f 09 JMP $094f ; (printf.l5 + 0)
.s12:
0996 : a5 5b __ LDA T7 + 0 
0998 : f0 0c __ BEQ $09a6 ; (printf.s13 + 0)
.s85:
099a : aa __ __ TAX
099b : 98 __ __ TYA
099c : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
099f : 20 8e 0c JSR $0c8e ; (puts@proxy + 0)
09a2 : a9 00 __ LDA #$00
09a4 : 85 5b __ STA T7 + 0 
.s13:
09a6 : 8d b5 9f STA $9fb5 ; (si.sign + 0)
09a9 : 8d b6 9f STA $9fb6 ; (si.left + 0)
09ac : 8d b7 9f STA $9fb7 ; (si.prefix + 0)
09af : a0 01 __ LDY #$01
09b1 : b1 58 __ LDA (T5 + 0),y 
09b3 : a2 20 __ LDX #$20
09b5 : 8e b0 9f STX $9fb0 ; (si.fill + 0)
09b8 : a2 00 __ LDX #$00
09ba : 8e b1 9f STX $9fb1 ; (si.width + 0)
09bd : ca __ __ DEX
09be : 8e b2 9f STX $9fb2 ; (si.precision + 0)
09c1 : a2 0a __ LDX #$0a
09c3 : 8e b4 9f STX $9fb4 ; (si.base + 0)
09c6 : aa __ __ TAX
09c7 : a9 02 __ LDA #$02
09c9 : d0 07 __ BNE $09d2 ; (printf.l14 + 0)
.s81:
09cb : a0 00 __ LDY #$00
09cd : b1 58 __ LDA (T5 + 0),y 
09cf : aa __ __ TAX
09d0 : a9 01 __ LDA #$01
.l14:
09d2 : 18 __ __ CLC
09d3 : 65 58 __ ADC T5 + 0 
09d5 : 85 58 __ STA T5 + 0 
09d7 : 90 02 __ BCC $09db ; (printf.s96 + 0)
.s95:
09d9 : e6 59 __ INC T5 + 1 
.s96:
09db : e0 2b __ CPX #$2b
09dd : d0 07 __ BNE $09e6 ; (printf.s15 + 0)
.s84:
09df : a9 01 __ LDA #$01
09e1 : 8d b5 9f STA $9fb5 ; (si.sign + 0)
09e4 : d0 e5 __ BNE $09cb ; (printf.s81 + 0)
.s15:
09e6 : 8a __ __ TXA
09e7 : e0 30 __ CPX #$30
09e9 : d0 05 __ BNE $09f0 ; (printf.s16 + 0)
.s83:
09eb : 8d b0 9f STA $9fb0 ; (si.fill + 0)
09ee : f0 db __ BEQ $09cb ; (printf.s81 + 0)
.s16:
09f0 : c9 23 __ CMP #$23
09f2 : d0 07 __ BNE $09fb ; (printf.s17 + 0)
.s82:
09f4 : a9 01 __ LDA #$01
09f6 : 8d b7 9f STA $9fb7 ; (si.prefix + 0)
09f9 : d0 d0 __ BNE $09cb ; (printf.s81 + 0)
.s17:
09fb : c9 2d __ CMP #$2d
09fd : d0 07 __ BNE $0a06 ; (printf.s18 + 0)
.s80:
09ff : a9 01 __ LDA #$01
0a01 : 8d b6 9f STA $9fb6 ; (si.left + 0)
0a04 : d0 c5 __ BNE $09cb ; (printf.s81 + 0)
.s18:
0a06 : 85 55 __ STA T2 + 0 
0a08 : c9 30 __ CMP #$30
0a0a : 90 3a __ BCC $0a46 ; (printf.s19 + 0)
.s75:
0a0c : e0 3a __ CPX #$3a
0a0e : b0 66 __ BCS $0a76 ; (printf.s20 + 0)
.s76:
0a10 : a9 00 __ LDA #$00
0a12 : e0 3a __ CPX #$3a
0a14 : b0 2b __ BCS $0a41 ; (printf.s78 + 0)
.s111:
0a16 : 85 43 __ STA T0 + 0 
0a18 : a4 58 __ LDY T5 + 0 
0a1a : 85 58 __ STA T5 + 0 
.l79:
0a1c : a5 43 __ LDA T0 + 0 
0a1e : 0a __ __ ASL
0a1f : 0a __ __ ASL
0a20 : 18 __ __ CLC
0a21 : 65 43 __ ADC T0 + 0 
0a23 : 0a __ __ ASL
0a24 : 18 __ __ CLC
0a25 : 65 55 __ ADC T2 + 0 
0a27 : 38 __ __ SEC
0a28 : e9 30 __ SBC #$30
0a2a : 85 43 __ STA T0 + 0 
0a2c : b1 58 __ LDA (T5 + 0),y 
0a2e : 85 55 __ STA T2 + 0 
0a30 : c8 __ __ INY
0a31 : d0 02 __ BNE $0a35 ; (printf.s108 + 0)
.s107:
0a33 : e6 59 __ INC T5 + 1 
.s108:
0a35 : c9 30 __ CMP #$30
0a37 : 90 04 __ BCC $0a3d ; (printf.s112 + 0)
.s77:
0a39 : c9 3a __ CMP #$3a
0a3b : 90 df __ BCC $0a1c ; (printf.l79 + 0)
.s112:
0a3d : 84 58 __ STY T5 + 0 
0a3f : a5 43 __ LDA T0 + 0 
.s78:
0a41 : 8d b1 9f STA $9fb1 ; (si.width + 0)
0a44 : a5 55 __ LDA T2 + 0 
.s19:
0a46 : c9 2e __ CMP #$2e
0a48 : d0 2c __ BNE $0a76 ; (printf.s20 + 0)
.s70:
0a4a : a9 00 __ LDA #$00
0a4c : a8 __ __ TAY
0a4d : f0 0e __ BEQ $0a5d ; (printf.l71 + 0)
.s74:
0a4f : a5 43 __ LDA T0 + 0 
0a51 : 0a __ __ ASL
0a52 : 0a __ __ ASL
0a53 : 18 __ __ CLC
0a54 : 65 43 __ ADC T0 + 0 
0a56 : 0a __ __ ASL
0a57 : 18 __ __ CLC
0a58 : 65 55 __ ADC T2 + 0 
0a5a : 38 __ __ SEC
0a5b : e9 30 __ SBC #$30
.l71:
0a5d : 85 43 __ STA T0 + 0 
0a5f : b1 58 __ LDA (T5 + 0),y 
0a61 : 85 55 __ STA T2 + 0 
0a63 : e6 58 __ INC T5 + 0 
0a65 : d0 02 __ BNE $0a69 ; (printf.s98 + 0)
.s97:
0a67 : e6 59 __ INC T5 + 1 
.s98:
0a69 : c9 30 __ CMP #$30
0a6b : 90 04 __ BCC $0a71 ; (printf.s72 + 0)
.s73:
0a6d : c9 3a __ CMP #$3a
0a6f : 90 de __ BCC $0a4f ; (printf.s74 + 0)
.s72:
0a71 : a6 43 __ LDX T0 + 0 
0a73 : 8e b2 9f STX $9fb2 ; (si.precision + 0)
.s20:
0a76 : c9 64 __ CMP #$64
0a78 : f0 0c __ BEQ $0a86 ; (printf.s69 + 0)
.s21:
0a7a : c9 44 __ CMP #$44
0a7c : f0 08 __ BEQ $0a86 ; (printf.s69 + 0)
.s22:
0a7e : c9 69 __ CMP #$69
0a80 : f0 04 __ BEQ $0a86 ; (printf.s69 + 0)
.s23:
0a82 : c9 49 __ CMP #$49
0a84 : d0 0f __ BNE $0a95 ; (printf.s24 + 0)
.s69:
0a86 : a9 01 __ LDA #$01
.s68:
0a88 : 85 13 __ STA P6 
0a8a : a0 00 __ LDY #$00
0a8c : b1 56 __ LDA (T3 + 0),y 
0a8e : aa __ __ TAX
0a8f : c8 __ __ INY
0a90 : b1 56 __ LDA (T3 + 0),y 
0a92 : 4c 70 0c JMP $0c70 ; (printf.s66 + 0)
.s24:
0a95 : c9 75 __ CMP #$75
0a97 : f0 04 __ BEQ $0a9d ; (printf.s67 + 0)
.s25:
0a99 : c9 55 __ CMP #$55
0a9b : d0 04 __ BNE $0aa1 ; (printf.s26 + 0)
.s67:
0a9d : a9 00 __ LDA #$00
0a9f : f0 e7 __ BEQ $0a88 ; (printf.s68 + 0)
.s26:
0aa1 : c9 78 __ CMP #$78
0aa3 : f0 04 __ BEQ $0aa9 ; (printf.s65 + 0)
.s27:
0aa5 : c9 58 __ CMP #$58
0aa7 : d0 1d __ BNE $0ac6 ; (printf.s28 + 0)
.s65:
0aa9 : a0 00 __ LDY #$00
0aab : 84 13 __ STY P6 
0aad : a9 10 __ LDA #$10
0aaf : 8d b4 9f STA $9fb4 ; (si.base + 0)
0ab2 : b1 56 __ LDA (T3 + 0),y 
0ab4 : aa __ __ TAX
0ab5 : c8 __ __ INY
0ab6 : b1 56 __ LDA (T3 + 0),y 
0ab8 : a8 __ __ TAY
0ab9 : a5 55 __ LDA T2 + 0 
0abb : 29 e0 __ AND #$e0
0abd : 09 01 __ ORA #$01
0abf : 8d b3 9f STA $9fb3 ; (si.cha + 0)
0ac2 : 98 __ __ TYA
0ac3 : 4c 70 0c JMP $0c70 ; (printf.s66 + 0)
.s28:
0ac6 : c9 6c __ CMP #$6c
0ac8 : d0 03 __ BNE $0acd ; (printf.s29 + 0)
0aca : 4c f5 0b JMP $0bf5 ; (printf.s53 + 0)
.s29:
0acd : c9 4c __ CMP #$4c
0acf : f0 f9 __ BEQ $0aca ; (printf.s28 + 4)
.s30:
0ad1 : c9 66 __ CMP #$66
0ad3 : f0 14 __ BEQ $0ae9 ; (printf.s52 + 0)
.s31:
0ad5 : c9 67 __ CMP #$67
0ad7 : f0 10 __ BEQ $0ae9 ; (printf.s52 + 0)
.s32:
0ad9 : c9 65 __ CMP #$65
0adb : f0 0c __ BEQ $0ae9 ; (printf.s52 + 0)
.s33:
0add : c9 46 __ CMP #$46
0adf : f0 08 __ BEQ $0ae9 ; (printf.s52 + 0)
.s34:
0ae1 : c9 47 __ CMP #$47
0ae3 : f0 04 __ BEQ $0ae9 ; (printf.s52 + 0)
.s35:
0ae5 : c9 45 __ CMP #$45
0ae7 : d0 44 __ BNE $0b2d ; (printf.s36 + 0)
.s52:
0ae9 : a9 b8 __ LDA #$b8
0aeb : 85 0f __ STA P2 
0aed : a9 9f __ LDA #$9f
0aef : 85 10 __ STA P3 
0af1 : a0 00 __ LDY #$00
0af3 : b1 56 __ LDA (T3 + 0),y 
0af5 : 85 11 __ STA P4 
0af7 : c8 __ __ INY
0af8 : b1 56 __ LDA (T3 + 0),y 
0afa : 85 12 __ STA P5 
0afc : c8 __ __ INY
0afd : b1 56 __ LDA (T3 + 0),y 
0aff : 85 13 __ STA P6 
0b01 : c8 __ __ INY
0b02 : b1 56 __ LDA (T3 + 0),y 
0b04 : 85 14 __ STA P7 
0b06 : a5 55 __ LDA T2 + 0 
0b08 : 29 e0 __ AND #$e0
0b0a : 09 01 __ ORA #$01
0b0c : 8d b3 9f STA $9fb3 ; (si.cha + 0)
0b0f : a9 b0 __ LDA #$b0
0b11 : 85 0d __ STA P0 
0b13 : a9 9f __ LDA #$9f
0b15 : 85 0e __ STA P1 
0b17 : a5 55 __ LDA T2 + 0 
0b19 : ed b3 9f SBC $9fb3 ; (si.cha + 0)
0b1c : 18 __ __ CLC
0b1d : 69 61 __ ADC #$61
0b1f : 85 15 __ STA P8 
0b21 : 20 2b 0f JSR $0f2b ; (nformf.s1 + 0)
0b24 : a5 1b __ LDA ACCU + 0 ; (fmt + 2)
0b26 : 85 5b __ STA T7 + 0 
0b28 : a9 04 __ LDA #$04
0b2a : 4c e9 0b JMP $0be9 ; (printf.s90 + 0)
.s36:
0b2d : c9 73 __ CMP #$73
0b2f : f0 2d __ BEQ $0b5e ; (printf.s44 + 0)
.s37:
0b31 : c9 53 __ CMP #$53
0b33 : f0 29 __ BEQ $0b5e ; (printf.s44 + 0)
.s38:
0b35 : c9 63 __ CMP #$63
0b37 : f0 12 __ BEQ $0b4b ; (printf.s43 + 0)
.s39:
0b39 : c9 43 __ CMP #$43
0b3b : f0 0e __ BEQ $0b4b ; (printf.s43 + 0)
.s40:
0b3d : aa __ __ TAX
0b3e : d0 03 __ BNE $0b43 ; (printf.s41 + 0)
0b40 : 4c 4f 09 JMP $094f ; (printf.l5 + 0)
.s41:
0b43 : 8d b8 9f STA $9fb8 ; (buff[0] + 0)
.s42:
0b46 : a9 01 __ LDA #$01
0b48 : 4c 91 09 JMP $0991 ; (printf.s92 + 0)
.s43:
0b4b : a0 00 __ LDY #$00
0b4d : b1 56 __ LDA (T3 + 0),y 
0b4f : 8d b8 9f STA $9fb8 ; (buff[0] + 0)
0b52 : a5 56 __ LDA T3 + 0 
0b54 : 69 01 __ ADC #$01
0b56 : 85 56 __ STA T3 + 0 
0b58 : 90 ec __ BCC $0b46 ; (printf.s42 + 0)
.s106:
0b5a : e6 57 __ INC T3 + 1 
0b5c : b0 e8 __ BCS $0b46 ; (printf.s42 + 0)
.s44:
0b5e : a0 00 __ LDY #$00
0b60 : 84 5a __ STY T6 + 0 
0b62 : b1 56 __ LDA (T3 + 0),y 
0b64 : 85 53 __ STA T1 + 0 
0b66 : c8 __ __ INY
0b67 : b1 56 __ LDA (T3 + 0),y 
0b69 : 85 54 __ STA T1 + 1 
0b6b : a5 56 __ LDA T3 + 0 
0b6d : 69 01 __ ADC #$01
0b6f : 85 56 __ STA T3 + 0 
0b71 : 90 02 __ BCC $0b75 ; (printf.s105 + 0)
.s104:
0b73 : e6 57 __ INC T3 + 1 
.s105:
0b75 : ad b1 9f LDA $9fb1 ; (si.width + 0)
0b78 : f0 0d __ BEQ $0b87 ; (printf.s45 + 0)
.s93:
0b7a : a0 00 __ LDY #$00
0b7c : b1 53 __ LDA (T1 + 0),y 
0b7e : f0 05 __ BEQ $0b85 ; (printf.s94 + 0)
.l51:
0b80 : c8 __ __ INY
0b81 : b1 53 __ LDA (T1 + 0),y 
0b83 : d0 fb __ BNE $0b80 ; (printf.l51 + 0)
.s94:
0b85 : 84 5a __ STY T6 + 0 
.s45:
0b87 : ad b6 9f LDA $9fb6 ; (si.left + 0)
0b8a : 85 55 __ STA T2 + 0 
0b8c : d0 07 __ BNE $0b95 ; (printf.s47 + 0)
.s46:
0b8e : a4 5a __ LDY T6 + 0 
0b90 : cc b1 9f CPY $9fb1 ; (si.width + 0)
0b93 : 90 2a __ BCC $0bbf ; (printf.s50 + 0)
.s47:
0b95 : a5 53 __ LDA T1 + 0 
0b97 : 85 0d __ STA P0 
0b99 : a5 54 __ LDA T1 + 1 
0b9b : 85 0e __ STA P1 
0b9d : 20 96 0c JSR $0c96 ; (puts.s4 + 0)
0ba0 : a5 55 __ LDA T2 + 0 
0ba2 : f0 9c __ BEQ $0b40 ; (printf.s40 + 3)
.s48:
0ba4 : a4 5a __ LDY T6 + 0 
0ba6 : cc b1 9f CPY $9fb1 ; (si.width + 0)
0ba9 : b0 95 __ BCS $0b40 ; (printf.s40 + 3)
.s49:
0bab : ad b0 9f LDA $9fb0 ; (si.fill + 0)
0bae : a2 00 __ LDX #$00
.l88:
0bb0 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0bb3 : e8 __ __ INX
0bb4 : c8 __ __ INY
0bb5 : cc b1 9f CPY $9fb1 ; (si.width + 0)
0bb8 : 90 f6 __ BCC $0bb0 ; (printf.l88 + 0)
.s86:
0bba : 86 5b __ STX T7 + 0 
0bbc : 4c 4f 09 JMP $094f ; (printf.l5 + 0)
.s50:
0bbf : ad b0 9f LDA $9fb0 ; (si.fill + 0)
0bc2 : a2 00 __ LDX #$00
.l89:
0bc4 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0bc7 : e8 __ __ INX
0bc8 : c8 __ __ INY
0bc9 : cc b1 9f CPY $9fb1 ; (si.width + 0)
0bcc : 90 f6 __ BCC $0bc4 ; (printf.l89 + 0)
.s87:
0bce : a9 b8 __ LDA #$b8
0bd0 : 85 0d __ STA P0 
0bd2 : a9 9f __ LDA #$9f
0bd4 : 85 0e __ STA P1 
0bd6 : a9 00 __ LDA #$00
0bd8 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0bdb : 20 96 0c JSR $0c96 ; (puts.s4 + 0)
0bde : a5 53 __ LDA T1 + 0 
0be0 : 85 0d __ STA P0 
0be2 : a5 54 __ LDA T1 + 1 
0be4 : 85 0e __ STA P1 
0be6 : 4c 8c 09 JMP $098c ; (printf.s11 + 0)
.s90:
0be9 : 18 __ __ CLC
0bea : 65 56 __ ADC T3 + 0 
0bec : 85 56 __ STA T3 + 0 
0bee : 90 cc __ BCC $0bbc ; (printf.s86 + 2)
.s99:
0bf0 : e6 57 __ INC T3 + 1 
0bf2 : 4c 4f 09 JMP $094f ; (printf.l5 + 0)
.s53:
0bf5 : a0 00 __ LDY #$00
0bf7 : b1 56 __ LDA (T3 + 0),y 
0bf9 : 85 11 __ STA P4 
0bfb : c8 __ __ INY
0bfc : b1 56 __ LDA (T3 + 0),y 
0bfe : 85 12 __ STA P5 
0c00 : c8 __ __ INY
0c01 : b1 56 __ LDA (T3 + 0),y 
0c03 : 85 13 __ STA P6 
0c05 : c8 __ __ INY
0c06 : b1 56 __ LDA (T3 + 0),y 
0c08 : 85 14 __ STA P7 
0c0a : a5 56 __ LDA T3 + 0 
0c0c : 69 03 __ ADC #$03
0c0e : 85 56 __ STA T3 + 0 
0c10 : 90 02 __ BCC $0c14 ; (printf.s101 + 0)
.s100:
0c12 : e6 57 __ INC T3 + 1 
.s101:
0c14 : a0 00 __ LDY #$00
0c16 : b1 58 __ LDA (T5 + 0),y 
0c18 : aa __ __ TAX
0c19 : e6 58 __ INC T5 + 0 
0c1b : d0 02 __ BNE $0c1f ; (printf.s103 + 0)
.s102:
0c1d : e6 59 __ INC T5 + 1 
.s103:
0c1f : e0 64 __ CPX #$64
0c21 : f0 0c __ BEQ $0c2f ; (printf.s64 + 0)
.s54:
0c23 : e0 44 __ CPX #$44
0c25 : f0 08 __ BEQ $0c2f ; (printf.s64 + 0)
.s55:
0c27 : e0 69 __ CPX #$69
0c29 : f0 04 __ BEQ $0c2f ; (printf.s64 + 0)
.s56:
0c2b : e0 49 __ CPX #$49
0c2d : d0 1c __ BNE $0c4b ; (printf.s57 + 0)
.s64:
0c2f : a9 01 __ LDA #$01
.s91:
0c31 : 85 15 __ STA P8 
.s62:
0c33 : a9 b8 __ LDA #$b8
0c35 : 85 0f __ STA P2 
0c37 : a9 9f __ LDA #$9f
0c39 : 85 0e __ STA P1 
0c3b : a9 9f __ LDA #$9f
0c3d : 85 10 __ STA P3 
0c3f : a9 b0 __ LDA #$b0
0c41 : 85 0d __ STA P0 
0c43 : 20 e0 0d JSR $0de0 ; (nforml.s4 + 0)
0c46 : a5 1b __ LDA ACCU + 0 ; (fmt + 2)
0c48 : 4c 91 09 JMP $0991 ; (printf.s92 + 0)
.s57:
0c4b : e0 75 __ CPX #$75
0c4d : f0 04 __ BEQ $0c53 ; (printf.s63 + 0)
.s58:
0c4f : e0 55 __ CPX #$55
0c51 : d0 03 __ BNE $0c56 ; (printf.s59 + 0)
.s63:
0c53 : 98 __ __ TYA
0c54 : f0 db __ BEQ $0c31 ; (printf.s91 + 0)
.s59:
0c56 : e0 78 __ CPX #$78
0c58 : f0 04 __ BEQ $0c5e ; (printf.s61 + 0)
.s60:
0c5a : e0 58 __ CPX #$58
0c5c : d0 94 __ BNE $0bf2 ; (printf.s99 + 2)
.s61:
0c5e : 84 15 __ STY P8 
0c60 : a9 10 __ LDA #$10
0c62 : 8d b4 9f STA $9fb4 ; (si.base + 0)
0c65 : 8a __ __ TXA
0c66 : 29 e0 __ AND #$e0
0c68 : 09 01 __ ORA #$01
0c6a : 8d b3 9f STA $9fb3 ; (si.cha + 0)
0c6d : 4c 33 0c JMP $0c33 ; (printf.s62 + 0)
.s66:
0c70 : 86 11 __ STX P4 
0c72 : 85 12 __ STA P5 
0c74 : a9 b8 __ LDA #$b8
0c76 : 85 0f __ STA P2 
0c78 : a9 9f __ LDA #$9f
0c7a : 85 0e __ STA P1 
0c7c : a9 9f __ LDA #$9f
0c7e : 85 10 __ STA P3 
0c80 : a9 b0 __ LDA #$b0
0c82 : 85 0d __ STA P0 
0c84 : 20 cb 0c JSR $0ccb ; (nformi.s4 + 0)
0c87 : 85 5b __ STA T7 + 0 
0c89 : a9 02 __ LDA #$02
0c8b : 4c e9 0b JMP $0be9 ; (printf.s90 + 0)
--------------------------------------------------------------------
puts@proxy: ; puts@proxy
0c8e : a9 b8 __ LDA #$b8
0c90 : 85 0d __ STA P0 
0c92 : a9 9f __ LDA #$9f
0c94 : 85 0e __ STA P1 
--------------------------------------------------------------------
puts: ; puts(const u8*)->void
;  12, "C:/oscar64/include/stdio.h"
.s4:
0c96 : a0 00 __ LDY #$00
.l5:
0c98 : b1 0d __ LDA (P0),y ; (str + 0)
0c9a : aa __ __ TAX
0c9b : 18 __ __ CLC
0c9c : a5 0d __ LDA P0 ; (str + 0)
0c9e : 69 01 __ ADC #$01
0ca0 : 85 0d __ STA P0 ; (str + 0)
0ca2 : 8a __ __ TXA
0ca3 : d0 01 __ BNE $0ca6 ; (puts.s6 + 0)
.s3:
0ca5 : 60 __ __ RTS
.s6:
0ca6 : 90 02 __ BCC $0caa ; (puts.s13 + 0)
.s12:
0ca8 : e6 0e __ INC P1 ; (str + 1)
.s13:
0caa : c9 0a __ CMP #$0a
0cac : d0 08 __ BNE $0cb6 ; (puts.s7 + 0)
.s10:
0cae : a9 0d __ LDA #$0d
.s11:
0cb0 : 20 d2 ff JSR $ffd2 
0cb3 : 4c 98 0c JMP $0c98 ; (puts.l5 + 0)
.s7:
0cb6 : c9 09 __ CMP #$09
0cb8 : d0 f6 __ BNE $0cb0 ; (puts.s11 + 0)
.s8:
0cba : a5 d3 __ LDA $d3 
0cbc : 29 03 __ AND #$03
0cbe : aa __ __ TAX
.l9:
0cbf : a9 20 __ LDA #$20
0cc1 : 20 d2 ff JSR $ffd2 
0cc4 : e8 __ __ INX
0cc5 : e0 04 __ CPX #$04
0cc7 : 90 f6 __ BCC $0cbf ; (puts.l9 + 0)
0cc9 : b0 cd __ BCS $0c98 ; (puts.l5 + 0)
--------------------------------------------------------------------
nformi: ; nformi(const struct sinfo*,u8*,i16,bool)->u8
;  79, "C:/oscar64/include/stdio.c"
.s4:
0ccb : a9 00 __ LDA #$00
0ccd : 85 43 __ STA T5 + 0 
0ccf : a0 04 __ LDY #$04
0cd1 : b1 0d __ LDA (P0),y ; (si + 0)
0cd3 : 85 44 __ STA T6 + 0 
0cd5 : a5 13 __ LDA P6 ; (s + 0)
0cd7 : f0 13 __ BEQ $0cec ; (nformi.s5 + 0)
.s34:
0cd9 : 24 12 __ BIT P5 ; (v + 1)
0cdb : 10 0f __ BPL $0cec ; (nformi.s5 + 0)
.s35:
0cdd : 38 __ __ SEC
0cde : a9 00 __ LDA #$00
0ce0 : e5 11 __ SBC P4 ; (v + 0)
0ce2 : 85 11 __ STA P4 ; (v + 0)
0ce4 : a9 00 __ LDA #$00
0ce6 : e5 12 __ SBC P5 ; (v + 1)
0ce8 : 85 12 __ STA P5 ; (v + 1)
0cea : e6 43 __ INC T5 + 0 
.s5:
0cec : a9 10 __ LDA #$10
0cee : 85 45 __ STA T7 + 0 
0cf0 : a5 11 __ LDA P4 ; (v + 0)
0cf2 : 05 12 __ ORA P5 ; (v + 1)
0cf4 : f0 2d __ BEQ $0d23 ; (nformi.s6 + 0)
.s29:
0cf6 : a5 11 __ LDA P4 ; (v + 0)
0cf8 : 85 1b __ STA ACCU + 0 
0cfa : a5 12 __ LDA P5 ; (v + 1)
0cfc : 85 1c __ STA ACCU + 1 
.l30:
0cfe : a5 44 __ LDA T6 + 0 
0d00 : 20 c9 16 JSR $16c9 ; (divmod + 53)
0d03 : a5 05 __ LDA WORK + 2 
0d05 : c9 0a __ CMP #$0a
0d07 : b0 04 __ BCS $0d0d ; (nformi.s33 + 0)
.s31:
0d09 : a9 30 __ LDA #$30
0d0b : 90 06 __ BCC $0d13 ; (nformi.s32 + 0)
.s33:
0d0d : a0 03 __ LDY #$03
0d0f : b1 0d __ LDA (P0),y ; (si + 0)
0d11 : e9 0a __ SBC #$0a
.s32:
0d13 : 18 __ __ CLC
0d14 : 65 05 __ ADC WORK + 2 
0d16 : a6 45 __ LDX T7 + 0 
0d18 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d1b : c6 45 __ DEC T7 + 0 
0d1d : a5 1b __ LDA ACCU + 0 
0d1f : 05 1c __ ORA ACCU + 1 
0d21 : d0 db __ BNE $0cfe ; (nformi.l30 + 0)
.s6:
0d23 : a9 ff __ LDA #$ff
0d25 : a0 02 __ LDY #$02
0d27 : d1 0d __ CMP (P0),y ; (si + 0)
0d29 : d0 04 __ BNE $0d2f ; (nformi.s28 + 0)
.s7:
0d2b : a9 0f __ LDA #$0f
0d2d : d0 05 __ BNE $0d34 ; (nformi.s40 + 0)
.s28:
0d2f : 38 __ __ SEC
0d30 : a9 10 __ LDA #$10
0d32 : f1 0d __ SBC (P0),y ; (si + 0)
.s40:
0d34 : a8 __ __ TAY
0d35 : c4 45 __ CPY T7 + 0 
0d37 : b0 0d __ BCS $0d46 ; (nformi.s8 + 0)
.s27:
0d39 : a9 30 __ LDA #$30
.l41:
0d3b : a6 45 __ LDX T7 + 0 
0d3d : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d40 : c6 45 __ DEC T7 + 0 
0d42 : c4 45 __ CPY T7 + 0 
0d44 : 90 f5 __ BCC $0d3b ; (nformi.l41 + 0)
.s8:
0d46 : a0 07 __ LDY #$07
0d48 : b1 0d __ LDA (P0),y ; (si + 0)
0d4a : f0 1c __ BEQ $0d68 ; (nformi.s9 + 0)
.s25:
0d4c : a5 44 __ LDA T6 + 0 
0d4e : c9 10 __ CMP #$10
0d50 : d0 16 __ BNE $0d68 ; (nformi.s9 + 0)
.s26:
0d52 : a0 03 __ LDY #$03
0d54 : b1 0d __ LDA (P0),y ; (si + 0)
0d56 : a8 __ __ TAY
0d57 : a9 30 __ LDA #$30
0d59 : a6 45 __ LDX T7 + 0 
0d5b : 9d e8 9f STA $9fe8,x ; (buff[0] + 48)
0d5e : 98 __ __ TYA
0d5f : 69 16 __ ADC #$16
0d61 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d64 : ca __ __ DEX
0d65 : ca __ __ DEX
0d66 : 86 45 __ STX T7 + 0 
.s9:
0d68 : a9 00 __ LDA #$00
0d6a : 85 1b __ STA ACCU + 0 
0d6c : a5 43 __ LDA T5 + 0 
0d6e : f0 0c __ BEQ $0d7c ; (nformi.s10 + 0)
.s24:
0d70 : a9 2d __ LDA #$2d
.s23:
0d72 : a6 45 __ LDX T7 + 0 
0d74 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d77 : c6 45 __ DEC T7 + 0 
0d79 : 4c 86 0d JMP $0d86 ; (nformi.s11 + 0)
.s10:
0d7c : a0 05 __ LDY #$05
0d7e : b1 0d __ LDA (P0),y ; (si + 0)
0d80 : f0 04 __ BEQ $0d86 ; (nformi.s11 + 0)
.s22:
0d82 : a9 2b __ LDA #$2b
0d84 : d0 ec __ BNE $0d72 ; (nformi.s23 + 0)
.s11:
0d86 : a6 45 __ LDX T7 + 0 
0d88 : a0 06 __ LDY #$06
0d8a : b1 0d __ LDA (P0),y ; (si + 0)
0d8c : d0 2b __ BNE $0db9 ; (nformi.s18 + 0)
.l12:
0d8e : 8a __ __ TXA
0d8f : 18 __ __ CLC
0d90 : a0 01 __ LDY #$01
0d92 : 71 0d __ ADC (P0),y ; (si + 0)
0d94 : b0 04 __ BCS $0d9a ; (nformi.s16 + 0)
.s17:
0d96 : c9 11 __ CMP #$11
0d98 : 90 0a __ BCC $0da4 ; (nformi.s13 + 0)
.s16:
0d9a : a0 00 __ LDY #$00
0d9c : b1 0d __ LDA (P0),y ; (si + 0)
0d9e : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0da1 : ca __ __ DEX
0da2 : b0 ea __ BCS $0d8e ; (nformi.l12 + 0)
.s13:
0da4 : e0 10 __ CPX #$10
0da6 : b0 0e __ BCS $0db6 ; (nformi.s14 + 0)
.s15:
0da8 : 88 __ __ DEY
.l38:
0da9 : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0dac : 91 0f __ STA (P2),y ; (str + 0)
0dae : c8 __ __ INY
0daf : e8 __ __ INX
0db0 : e0 10 __ CPX #$10
0db2 : 90 f5 __ BCC $0da9 ; (nformi.l38 + 0)
.s39:
0db4 : 84 1b __ STY ACCU + 0 
.s14:
0db6 : a5 1b __ LDA ACCU + 0 
.s3:
0db8 : 60 __ __ RTS
.s18:
0db9 : e0 10 __ CPX #$10
0dbb : b0 1a __ BCS $0dd7 ; (nformi.l19 + 0)
.s21:
0dbd : a0 00 __ LDY #$00
.l36:
0dbf : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0dc2 : 91 0f __ STA (P2),y ; (str + 0)
0dc4 : c8 __ __ INY
0dc5 : e8 __ __ INX
0dc6 : e0 10 __ CPX #$10
0dc8 : 90 f5 __ BCC $0dbf ; (nformi.l36 + 0)
.s37:
0dca : 84 1b __ STY ACCU + 0 
0dcc : b0 09 __ BCS $0dd7 ; (nformi.l19 + 0)
.s20:
0dce : 88 __ __ DEY
0dcf : b1 0d __ LDA (P0),y ; (si + 0)
0dd1 : a4 1b __ LDY ACCU + 0 
0dd3 : 91 0f __ STA (P2),y ; (str + 0)
0dd5 : e6 1b __ INC ACCU + 0 
.l19:
0dd7 : a5 1b __ LDA ACCU + 0 
0dd9 : a0 01 __ LDY #$01
0ddb : d1 0d __ CMP (P0),y ; (si + 0)
0ddd : 90 ef __ BCC $0dce ; (nformi.s20 + 0)
0ddf : 60 __ __ RTS
--------------------------------------------------------------------
nforml: ; nforml(const struct sinfo*,u8*,i32,bool)->u8
; 137, "C:/oscar64/include/stdio.c"
.s4:
0de0 : a9 00 __ LDA #$00
0de2 : 85 43 __ STA T4 + 0 
0de4 : a5 15 __ LDA P8 ; (s + 0)
0de6 : f0 1f __ BEQ $0e07 ; (nforml.s5 + 0)
.s35:
0de8 : 24 14 __ BIT P7 ; (v + 3)
0dea : 10 1b __ BPL $0e07 ; (nforml.s5 + 0)
.s36:
0dec : 38 __ __ SEC
0ded : a9 00 __ LDA #$00
0def : e5 11 __ SBC P4 ; (v + 0)
0df1 : 85 11 __ STA P4 ; (v + 0)
0df3 : a9 00 __ LDA #$00
0df5 : e5 12 __ SBC P5 ; (v + 1)
0df7 : 85 12 __ STA P5 ; (v + 1)
0df9 : a9 00 __ LDA #$00
0dfb : e5 13 __ SBC P6 ; (v + 2)
0dfd : 85 13 __ STA P6 ; (v + 2)
0dff : a9 00 __ LDA #$00
0e01 : e5 14 __ SBC P7 ; (v + 3)
0e03 : 85 14 __ STA P7 ; (v + 3)
0e05 : e6 43 __ INC T4 + 0 
.s5:
0e07 : a9 10 __ LDA #$10
0e09 : 85 44 __ STA T5 + 0 
0e0b : a5 14 __ LDA P7 ; (v + 3)
0e0d : d0 0c __ BNE $0e1b ; (nforml.l43 + 0)
.s31:
0e0f : a5 13 __ LDA P6 ; (v + 2)
0e11 : d0 08 __ BNE $0e1b ; (nforml.l43 + 0)
.s32:
0e13 : a5 12 __ LDA P5 ; (v + 1)
0e15 : d0 04 __ BNE $0e1b ; (nforml.l43 + 0)
.s33:
0e17 : c5 11 __ CMP P4 ; (v + 0)
0e19 : b0 13 __ BCS $0e2e ; (nforml.s6 + 0)
.l43:
0e1b : a5 11 __ LDA P4 ; (v + 0)
0e1d : 85 1b __ STA ACCU + 0 
0e1f : a5 12 __ LDA P5 ; (v + 1)
0e21 : 85 1c __ STA ACCU + 1 
0e23 : a5 13 __ LDA P6 ; (v + 2)
0e25 : 85 1d __ STA ACCU + 2 
0e27 : a5 14 __ LDA P7 ; (v + 3)
0e29 : 85 1e __ STA ACCU + 3 
0e2b : 4c ea 0e JMP $0eea ; (nforml.l28 + 0)
.s6:
0e2e : a9 ff __ LDA #$ff
0e30 : a0 02 __ LDY #$02
0e32 : d1 0d __ CMP (P0),y ; (si + 0)
0e34 : d0 04 __ BNE $0e3a ; (nforml.s27 + 0)
.s7:
0e36 : a9 0f __ LDA #$0f
0e38 : d0 05 __ BNE $0e3f ; (nforml.s41 + 0)
.s27:
0e3a : 38 __ __ SEC
0e3b : a9 10 __ LDA #$10
0e3d : f1 0d __ SBC (P0),y ; (si + 0)
.s41:
0e3f : a8 __ __ TAY
0e40 : c4 44 __ CPY T5 + 0 
0e42 : b0 0d __ BCS $0e51 ; (nforml.s8 + 0)
.s26:
0e44 : a9 30 __ LDA #$30
.l42:
0e46 : a6 44 __ LDX T5 + 0 
0e48 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e4b : c6 44 __ DEC T5 + 0 
0e4d : c4 44 __ CPY T5 + 0 
0e4f : 90 f5 __ BCC $0e46 ; (nforml.l42 + 0)
.s8:
0e51 : a0 07 __ LDY #$07
0e53 : b1 0d __ LDA (P0),y ; (si + 0)
0e55 : f0 1d __ BEQ $0e74 ; (nforml.s9 + 0)
.s24:
0e57 : a9 10 __ LDA #$10
0e59 : a0 04 __ LDY #$04
0e5b : d1 0d __ CMP (P0),y ; (si + 0)
0e5d : d0 15 __ BNE $0e74 ; (nforml.s9 + 0)
.s25:
0e5f : 88 __ __ DEY
0e60 : b1 0d __ LDA (P0),y ; (si + 0)
0e62 : a8 __ __ TAY
0e63 : a9 30 __ LDA #$30
0e65 : a6 44 __ LDX T5 + 0 
0e67 : 9d e8 9f STA $9fe8,x ; (buff[0] + 48)
0e6a : 98 __ __ TYA
0e6b : 69 16 __ ADC #$16
0e6d : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e70 : ca __ __ DEX
0e71 : ca __ __ DEX
0e72 : 86 44 __ STX T5 + 0 
.s9:
0e74 : a9 00 __ LDA #$00
0e76 : 85 1b __ STA ACCU + 0 
0e78 : a5 43 __ LDA T4 + 0 
0e7a : f0 0c __ BEQ $0e88 ; (nforml.s10 + 0)
.s23:
0e7c : a9 2d __ LDA #$2d
.s22:
0e7e : a6 44 __ LDX T5 + 0 
0e80 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e83 : c6 44 __ DEC T5 + 0 
0e85 : 4c 92 0e JMP $0e92 ; (nforml.s11 + 0)
.s10:
0e88 : a0 05 __ LDY #$05
0e8a : b1 0d __ LDA (P0),y ; (si + 0)
0e8c : f0 04 __ BEQ $0e92 ; (nforml.s11 + 0)
.s21:
0e8e : a9 2b __ LDA #$2b
0e90 : d0 ec __ BNE $0e7e ; (nforml.s22 + 0)
.s11:
0e92 : a0 06 __ LDY #$06
0e94 : a6 44 __ LDX T5 + 0 
0e96 : b1 0d __ LDA (P0),y ; (si + 0)
0e98 : d0 29 __ BNE $0ec3 ; (nforml.s17 + 0)
.l12:
0e9a : 8a __ __ TXA
0e9b : 18 __ __ CLC
0e9c : a0 01 __ LDY #$01
0e9e : 71 0d __ ADC (P0),y ; (si + 0)
0ea0 : b0 04 __ BCS $0ea6 ; (nforml.s15 + 0)
.s16:
0ea2 : c9 11 __ CMP #$11
0ea4 : 90 0a __ BCC $0eb0 ; (nforml.s13 + 0)
.s15:
0ea6 : a0 00 __ LDY #$00
0ea8 : b1 0d __ LDA (P0),y ; (si + 0)
0eaa : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0ead : ca __ __ DEX
0eae : b0 ea __ BCS $0e9a ; (nforml.l12 + 0)
.s13:
0eb0 : e0 10 __ CPX #$10
0eb2 : b0 0e __ BCS $0ec2 ; (nforml.s3 + 0)
.s14:
0eb4 : 88 __ __ DEY
.l39:
0eb5 : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0eb8 : 91 0f __ STA (P2),y ; (str + 0)
0eba : c8 __ __ INY
0ebb : e8 __ __ INX
0ebc : e0 10 __ CPX #$10
0ebe : 90 f5 __ BCC $0eb5 ; (nforml.l39 + 0)
.s40:
0ec0 : 84 1b __ STY ACCU + 0 
.s3:
0ec2 : 60 __ __ RTS
.s17:
0ec3 : e0 10 __ CPX #$10
0ec5 : b0 1a __ BCS $0ee1 ; (nforml.l18 + 0)
.s20:
0ec7 : a0 00 __ LDY #$00
.l37:
0ec9 : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0ecc : 91 0f __ STA (P2),y ; (str + 0)
0ece : c8 __ __ INY
0ecf : e8 __ __ INX
0ed0 : e0 10 __ CPX #$10
0ed2 : 90 f5 __ BCC $0ec9 ; (nforml.l37 + 0)
.s38:
0ed4 : 84 1b __ STY ACCU + 0 
0ed6 : b0 09 __ BCS $0ee1 ; (nforml.l18 + 0)
.s19:
0ed8 : 88 __ __ DEY
0ed9 : b1 0d __ LDA (P0),y ; (si + 0)
0edb : a4 1b __ LDY ACCU + 0 
0edd : 91 0f __ STA (P2),y ; (str + 0)
0edf : e6 1b __ INC ACCU + 0 
.l18:
0ee1 : a5 1b __ LDA ACCU + 0 
0ee3 : a0 01 __ LDY #$01
0ee5 : d1 0d __ CMP (P0),y ; (si + 0)
0ee7 : 90 ef __ BCC $0ed8 ; (nforml.s19 + 0)
0ee9 : 60 __ __ RTS
.l28:
0eea : a0 04 __ LDY #$04
0eec : b1 0d __ LDA (P0),y ; (si + 0)
0eee : 85 03 __ STA WORK + 0 
0ef0 : a9 00 __ LDA #$00
0ef2 : 85 04 __ STA WORK + 1 
0ef4 : 85 05 __ STA WORK + 2 
0ef6 : 85 06 __ STA WORK + 3 
0ef8 : 20 8f 18 JSR $188f ; (divmod32 + 0)
0efb : a5 07 __ LDA WORK + 4 
0efd : c9 0a __ CMP #$0a
0eff : b0 04 __ BCS $0f05 ; (nforml.s34 + 0)
.s29:
0f01 : a9 30 __ LDA #$30
0f03 : 90 06 __ BCC $0f0b ; (nforml.s30 + 0)
.s34:
0f05 : a0 03 __ LDY #$03
0f07 : b1 0d __ LDA (P0),y ; (si + 0)
0f09 : e9 0a __ SBC #$0a
.s30:
0f0b : 18 __ __ CLC
0f0c : 65 07 __ ADC WORK + 4 
0f0e : a6 44 __ LDX T5 + 0 
0f10 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0f13 : a5 1b __ LDA ACCU + 0 
0f15 : 85 11 __ STA P4 ; (v + 0)
0f17 : a5 1c __ LDA ACCU + 1 
0f19 : 85 12 __ STA P5 ; (v + 1)
0f1b : a5 1d __ LDA ACCU + 2 
0f1d : 85 13 __ STA P6 ; (v + 2)
0f1f : a5 1e __ LDA ACCU + 3 
0f21 : 85 14 __ STA P7 ; (v + 3)
0f23 : c6 44 __ DEC T5 + 0 
0f25 : aa __ __ TAX
0f26 : d0 c2 __ BNE $0eea ; (nforml.l28 + 0)
0f28 : 4c 0f 0e JMP $0e0f ; (nforml.s31 + 0)
--------------------------------------------------------------------
nformf: ; nformf(const struct sinfo*,u8*,float,u8)->u8
; 199, "C:/oscar64/include/stdio.c"
.s1:
0f2b : a5 53 __ LDA T10 + 0 
0f2d : 8d f1 9f STA $9ff1 ; (nformf@stack + 0)
0f30 : a5 54 __ LDA T11 + 0 
0f32 : 8d f2 9f STA $9ff2 ; (nformf@stack + 1)
.s4:
0f35 : a5 11 __ LDA P4 ; (f + 0)
0f37 : 85 43 __ STA T0 + 0 
0f39 : a5 12 __ LDA P5 ; (f + 1)
0f3b : 85 44 __ STA T0 + 1 
0f3d : a5 14 __ LDA P7 ; (f + 3)
0f3f : 29 7f __ AND #$7f
0f41 : 05 13 __ ORA P6 ; (f + 2)
0f43 : 05 12 __ ORA P5 ; (f + 1)
0f45 : a6 13 __ LDX P6 ; (f + 2)
0f47 : 86 45 __ STX T0 + 2 
0f49 : 05 11 __ ORA P4 ; (f + 0)
0f4b : f0 14 __ BEQ $0f61 ; (nformf.s5 + 0)
.s89:
0f4d : 24 14 __ BIT P7 ; (f + 3)
0f4f : 10 10 __ BPL $0f61 ; (nformf.s5 + 0)
.s88:
0f51 : a9 2d __ LDA #$2d
0f53 : a0 00 __ LDY #$00
0f55 : 91 0f __ STA (P2),y ; (str + 0)
0f57 : a5 14 __ LDA P7 ; (f + 3)
0f59 : 49 80 __ EOR #$80
0f5b : 85 14 __ STA P7 ; (f + 3)
.s87:
0f5d : a9 01 __ LDA #$01
0f5f : d0 0e __ BNE $0f6f ; (nformf.s6 + 0)
.s5:
0f61 : a0 05 __ LDY #$05
0f63 : b1 0d __ LDA (P0),y ; (si + 0)
0f65 : f0 08 __ BEQ $0f6f ; (nformf.s6 + 0)
.s86:
0f67 : a9 2b __ LDA #$2b
0f69 : a0 00 __ LDY #$00
0f6b : 91 0f __ STA (P2),y ; (str + 0)
0f6d : a9 01 __ LDA #$01
.s6:
0f6f : 85 52 __ STA T9 + 0 
0f71 : 8a __ __ TXA
0f72 : 0a __ __ ASL
0f73 : a5 14 __ LDA P7 ; (f + 3)
0f75 : 2a __ __ ROL
0f76 : c9 ff __ CMP #$ff
0f78 : d0 29 __ BNE $0fa3 ; (nformf.s7 + 0)
.s85:
0f7a : a0 03 __ LDY #$03
0f7c : b1 0d __ LDA (P0),y ; (si + 0)
0f7e : 69 07 __ ADC #$07
0f80 : a4 52 __ LDY T9 + 0 
0f82 : 91 0f __ STA (P2),y ; (str + 0)
0f84 : 18 __ __ CLC
0f85 : a0 03 __ LDY #$03
0f87 : b1 0d __ LDA (P0),y ; (si + 0)
0f89 : 69 0d __ ADC #$0d
0f8b : a4 52 __ LDY T9 + 0 
0f8d : c8 __ __ INY
0f8e : 91 0f __ STA (P2),y ; (str + 0)
0f90 : 18 __ __ CLC
0f91 : a0 03 __ LDY #$03
0f93 : b1 0d __ LDA (P0),y ; (si + 0)
0f95 : 69 05 __ ADC #$05
0f97 : a4 52 __ LDY T9 + 0 
0f99 : c8 __ __ INY
0f9a : c8 __ __ INY
0f9b : 91 0f __ STA (P2),y ; (str + 0)
0f9d : c8 __ __ INY
0f9e : 84 52 __ STY T9 + 0 
0fa0 : 4c 75 12 JMP $1275 ; (nformf.s27 + 0)
.s7:
0fa3 : a0 02 __ LDY #$02
0fa5 : b1 0d __ LDA (P0),y ; (si + 0)
0fa7 : a6 14 __ LDX P7 ; (f + 3)
0fa9 : 86 46 __ STX T0 + 3 
0fab : c9 ff __ CMP #$ff
0fad : d0 02 __ BNE $0fb1 ; (nformf.s84 + 0)
.s8:
0faf : a9 06 __ LDA #$06
.s84:
0fb1 : 85 50 __ STA T7 + 0 
0fb3 : a9 00 __ LDA #$00
0fb5 : 85 4d __ STA T5 + 0 
0fb7 : 85 4e __ STA T5 + 1 
0fb9 : 8a __ __ TXA
0fba : 29 7f __ AND #$7f
0fbc : 05 13 __ ORA P6 ; (f + 2)
0fbe : 05 12 __ ORA P5 ; (f + 1)
0fc0 : 05 11 __ ORA P4 ; (f + 0)
0fc2 : d0 03 __ BNE $0fc7 ; (nformf.s63 + 0)
0fc4 : 4c 99 10 JMP $1099 ; (nformf.s9 + 0)
.s63:
0fc7 : 8a __ __ TXA
0fc8 : 30 51 __ BMI $101b ; (nformf.l72 + 0)
.s81:
0fca : c9 44 __ CMP #$44
0fcc : d0 06 __ BNE $0fd4 ; (nformf.l83 + 0)
.s82:
0fce : a5 13 __ LDA P6 ; (f + 2)
0fd0 : c9 7a __ CMP #$7a
0fd2 : f0 02 __ BEQ $0fd6 ; (nformf.l78 + 0)
.l83:
0fd4 : 90 35 __ BCC $100b ; (nformf.s64 + 0)
.l78:
0fd6 : 18 __ __ CLC
0fd7 : a5 4d __ LDA T5 + 0 
0fd9 : 69 03 __ ADC #$03
0fdb : 85 4d __ STA T5 + 0 
0fdd : 90 02 __ BCC $0fe1 ; (nformf.s103 + 0)
.s102:
0fdf : e6 4e __ INC T5 + 1 
.s103:
0fe1 : a9 7a __ LDA #$7a
0fe3 : 85 05 __ STA WORK + 2 
0fe5 : a9 44 __ LDA #$44
0fe7 : 85 06 __ STA WORK + 3 
0fe9 : 20 89 19 JSR $1989 ; (freg@proxy + 0)
0fec : 20 af 15 JSR $15af ; (crt_fdiv + 0)
0fef : a5 1b __ LDA ACCU + 0 
0ff1 : 85 43 __ STA T0 + 0 
0ff3 : a5 1c __ LDA ACCU + 1 
0ff5 : 85 44 __ STA T0 + 1 
0ff7 : a6 1d __ LDX ACCU + 2 
0ff9 : 86 45 __ STX T0 + 2 
0ffb : a5 1e __ LDA ACCU + 3 
0ffd : 85 46 __ STA T0 + 3 
0fff : 30 0a __ BMI $100b ; (nformf.s64 + 0)
.s79:
1001 : c9 44 __ CMP #$44
1003 : d0 cf __ BNE $0fd4 ; (nformf.l83 + 0)
.s80:
1005 : e0 7a __ CPX #$7a
1007 : f0 cd __ BEQ $0fd6 ; (nformf.l78 + 0)
1009 : d0 c9 __ BNE $0fd4 ; (nformf.l83 + 0)
.s64:
100b : a5 46 __ LDA T0 + 3 
100d : 30 0c __ BMI $101b ; (nformf.l72 + 0)
.s76:
100f : c9 3f __ CMP #$3f
1011 : d0 06 __ BNE $1019 ; (nformf.s75 + 0)
.s77:
1013 : a5 45 __ LDA T0 + 2 
1015 : c9 80 __ CMP #$80
1017 : f0 40 __ BEQ $1059 ; (nformf.s65 + 0)
.s75:
1019 : b0 3e __ BCS $1059 ; (nformf.s65 + 0)
.l72:
101b : 38 __ __ SEC
101c : a5 4d __ LDA T5 + 0 
101e : e9 03 __ SBC #$03
1020 : 85 4d __ STA T5 + 0 
1022 : b0 02 __ BCS $1026 ; (nformf.s98 + 0)
.s97:
1024 : c6 4e __ DEC T5 + 1 
.s98:
1026 : a9 00 __ LDA #$00
1028 : 85 1b __ STA ACCU + 0 
102a : 85 1c __ STA ACCU + 1 
102c : a9 7a __ LDA #$7a
102e : 85 1d __ STA ACCU + 2 
1030 : a9 44 __ LDA #$44
1032 : 85 1e __ STA ACCU + 3 
1034 : a2 43 __ LDX #$43
1036 : 20 b9 13 JSR $13b9 ; (freg + 4)
1039 : 20 e7 14 JSR $14e7 ; (crt_fmul + 0)
103c : a5 1b __ LDA ACCU + 0 
103e : 85 43 __ STA T0 + 0 
1040 : a5 1c __ LDA ACCU + 1 
1042 : 85 44 __ STA T0 + 1 
1044 : a6 1d __ LDX ACCU + 2 
1046 : 86 45 __ STX T0 + 2 
1048 : a5 1e __ LDA ACCU + 3 
104a : 85 46 __ STA T0 + 3 
104c : 30 cd __ BMI $101b ; (nformf.l72 + 0)
.s73:
104e : c9 3f __ CMP #$3f
1050 : 90 c9 __ BCC $101b ; (nformf.l72 + 0)
.s106:
1052 : d0 05 __ BNE $1059 ; (nformf.s65 + 0)
.s74:
1054 : e0 80 __ CPX #$80
1056 : 4c 19 10 JMP $1019 ; (nformf.s75 + 0)
.s65:
1059 : a5 46 __ LDA T0 + 3 
105b : 30 3c __ BMI $1099 ; (nformf.s9 + 0)
.s69:
105d : c9 41 __ CMP #$41
105f : d0 06 __ BNE $1067 ; (nformf.l71 + 0)
.s70:
1061 : a5 45 __ LDA T0 + 2 
1063 : c9 20 __ CMP #$20
1065 : f0 02 __ BEQ $1069 ; (nformf.l66 + 0)
.l71:
1067 : 90 30 __ BCC $1099 ; (nformf.s9 + 0)
.l66:
1069 : e6 4d __ INC T5 + 0 
106b : d0 02 __ BNE $106f ; (nformf.s101 + 0)
.s100:
106d : e6 4e __ INC T5 + 1 
.s101:
106f : a9 20 __ LDA #$20
1071 : 85 05 __ STA WORK + 2 
1073 : a9 41 __ LDA #$41
1075 : 85 06 __ STA WORK + 3 
1077 : 20 89 19 JSR $1989 ; (freg@proxy + 0)
107a : 20 af 15 JSR $15af ; (crt_fdiv + 0)
107d : a5 1b __ LDA ACCU + 0 
107f : 85 43 __ STA T0 + 0 
1081 : a5 1c __ LDA ACCU + 1 
1083 : 85 44 __ STA T0 + 1 
1085 : a6 1d __ LDX ACCU + 2 
1087 : 86 45 __ STX T0 + 2 
1089 : a5 1e __ LDA ACCU + 3 
108b : 85 46 __ STA T0 + 3 
108d : 30 0a __ BMI $1099 ; (nformf.s9 + 0)
.s67:
108f : c9 41 __ CMP #$41
1091 : d0 d4 __ BNE $1067 ; (nformf.l71 + 0)
.s68:
1093 : e0 20 __ CPX #$20
1095 : f0 d2 __ BEQ $1069 ; (nformf.l66 + 0)
1097 : d0 ce __ BNE $1067 ; (nformf.l71 + 0)
.s9:
1099 : a5 15 __ LDA P8 ; (type + 0)
109b : c9 65 __ CMP #$65
109d : d0 04 __ BNE $10a3 ; (nformf.s11 + 0)
.s10:
109f : a9 01 __ LDA #$01
10a1 : d0 02 __ BNE $10a5 ; (nformf.s12 + 0)
.s11:
10a3 : a9 00 __ LDA #$00
.s12:
10a5 : 85 53 __ STA T10 + 0 
10a7 : a6 50 __ LDX T7 + 0 
10a9 : e8 __ __ INX
10aa : 86 4f __ STX T6 + 0 
10ac : a5 15 __ LDA P8 ; (type + 0)
10ae : c9 67 __ CMP #$67
10b0 : d0 13 __ BNE $10c5 ; (nformf.s13 + 0)
.s59:
10b2 : a5 4e __ LDA T5 + 1 
10b4 : 30 08 __ BMI $10be ; (nformf.s60 + 0)
.s62:
10b6 : d0 06 __ BNE $10be ; (nformf.s60 + 0)
.s61:
10b8 : a5 4d __ LDA T5 + 0 
10ba : c9 04 __ CMP #$04
10bc : 90 07 __ BCC $10c5 ; (nformf.s13 + 0)
.s60:
10be : a9 01 __ LDA #$01
10c0 : 85 53 __ STA T10 + 0 
10c2 : 4c 02 13 JMP $1302 ; (nformf.s51 + 0)
.s13:
10c5 : a5 53 __ LDA T10 + 0 
10c7 : d0 f9 __ BNE $10c2 ; (nformf.s60 + 4)
.s14:
10c9 : 24 4e __ BIT T5 + 1 
10cb : 10 35 __ BPL $1102 ; (nformf.s15 + 0)
.s50:
10cd : a5 43 __ LDA T0 + 0 
10cf : 85 1b __ STA ACCU + 0 
10d1 : a5 44 __ LDA T0 + 1 
10d3 : 85 1c __ STA ACCU + 1 
10d5 : a5 45 __ LDA T0 + 2 
10d7 : 85 1d __ STA ACCU + 2 
10d9 : a5 46 __ LDA T0 + 3 
10db : 85 1e __ STA ACCU + 3 
.l90:
10dd : 20 76 19 JSR $1976 ; (freg@proxy + 0)
10e0 : 20 af 15 JSR $15af ; (crt_fdiv + 0)
10e3 : 18 __ __ CLC
10e4 : a5 4d __ LDA T5 + 0 
10e6 : 69 01 __ ADC #$01
10e8 : 85 4d __ STA T5 + 0 
10ea : a5 4e __ LDA T5 + 1 
10ec : 69 00 __ ADC #$00
10ee : 85 4e __ STA T5 + 1 
10f0 : 30 eb __ BMI $10dd ; (nformf.l90 + 0)
.s91:
10f2 : a5 1e __ LDA ACCU + 3 
10f4 : 85 46 __ STA T0 + 3 
10f6 : a5 1d __ LDA ACCU + 2 
10f8 : 85 45 __ STA T0 + 2 
10fa : a5 1c __ LDA ACCU + 1 
10fc : 85 44 __ STA T0 + 1 
10fe : a5 1b __ LDA ACCU + 0 
1100 : 85 43 __ STA T0 + 0 
.s15:
1102 : 18 __ __ CLC
1103 : a5 50 __ LDA T7 + 0 
1105 : 65 4d __ ADC T5 + 0 
1107 : 18 __ __ CLC
1108 : 69 01 __ ADC #$01
110a : 85 4f __ STA T6 + 0 
110c : c9 07 __ CMP #$07
110e : 90 14 __ BCC $1124 ; (nformf.s49 + 0)
.s16:
1110 : ad d0 19 LDA $19d0 ; (fround5[0] + 24)
1113 : 85 47 __ STA T1 + 0 
1115 : ad d1 19 LDA $19d1 ; (fround5[0] + 25)
1118 : 85 48 __ STA T1 + 1 
111a : ad d2 19 LDA $19d2 ; (fround5[0] + 26)
111d : 85 49 __ STA T1 + 2 
111f : ad d3 19 LDA $19d3 ; (fround5[0] + 27)
1122 : b0 15 __ BCS $1139 ; (nformf.s17 + 0)
.s49:
1124 : 0a __ __ ASL
1125 : 0a __ __ ASL
1126 : aa __ __ TAX
1127 : bd b4 19 LDA $19b4,x ; (freg@proxy + 16)
112a : 85 47 __ STA T1 + 0 
112c : bd b5 19 LDA $19b5,x ; (freg@proxy + 17)
112f : 85 48 __ STA T1 + 1 
1131 : bd b6 19 LDA $19b6,x ; (freg@proxy + 18)
1134 : 85 49 __ STA T1 + 2 
1136 : bd b7 19 LDA $19b7,x ; (spentry + 0)
.s17:
1139 : 85 4a __ STA T1 + 3 
113b : a2 47 __ LDX #$47
113d : 20 a4 19 JSR $19a4 ; (freg@proxy + 0)
1140 : 20 00 14 JSR $1400 ; (faddsub + 6)
1143 : a5 1c __ LDA ACCU + 1 
1145 : 85 12 __ STA P5 ; (f + 1)
1147 : a5 1d __ LDA ACCU + 2 
1149 : 85 13 __ STA P6 ; (f + 2)
114b : a6 1b __ LDX ACCU + 0 
114d : a5 1e __ LDA ACCU + 3 
114f : 85 14 __ STA P7 ; (f + 3)
1151 : 30 22 __ BMI $1175 ; (nformf.s18 + 0)
.s46:
1153 : c9 41 __ CMP #$41
1155 : d0 06 __ BNE $115d ; (nformf.s48 + 0)
.s47:
1157 : a5 13 __ LDA P6 ; (f + 2)
1159 : c9 20 __ CMP #$20
115b : f0 02 __ BEQ $115f ; (nformf.s45 + 0)
.s48:
115d : 90 16 __ BCC $1175 ; (nformf.s18 + 0)
.s45:
115f : 20 76 19 JSR $1976 ; (freg@proxy + 0)
1162 : 20 af 15 JSR $15af ; (crt_fdiv + 0)
1165 : a5 1c __ LDA ACCU + 1 
1167 : 85 12 __ STA P5 ; (f + 1)
1169 : a5 1d __ LDA ACCU + 2 
116b : 85 13 __ STA P6 ; (f + 2)
116d : a5 1e __ LDA ACCU + 3 
116f : 85 14 __ STA P7 ; (f + 3)
1171 : c6 50 __ DEC T7 + 0 
1173 : a6 1b __ LDX ACCU + 0 
.s18:
1175 : 38 __ __ SEC
1176 : a5 4f __ LDA T6 + 0 
1178 : e5 50 __ SBC T7 + 0 
117a : 85 4b __ STA T4 + 0 
117c : a5 4f __ LDA T6 + 0 
117e : c9 15 __ CMP #$15
1180 : 90 04 __ BCC $1186 ; (nformf.s19 + 0)
.s44:
1182 : a9 14 __ LDA #$14
1184 : 85 4f __ STA T6 + 0 
.s19:
1186 : a5 4b __ LDA T4 + 0 
1188 : d0 08 __ BNE $1192 ; (nformf.s21 + 0)
.s20:
118a : a9 30 __ LDA #$30
118c : a4 52 __ LDY T9 + 0 
118e : 91 0f __ STA (P2),y ; (str + 0)
1190 : e6 52 __ INC T9 + 0 
.s21:
1192 : a9 00 __ LDA #$00
1194 : 85 54 __ STA T11 + 0 
1196 : c5 4b __ CMP T4 + 0 
1198 : f0 61 __ BEQ $11fb ; (nformf.l43 + 0)
.s23:
119a : c9 07 __ CMP #$07
119c : 90 04 __ BCC $11a2 ; (nformf.s24 + 0)
.l42:
119e : a9 30 __ LDA #$30
11a0 : b0 47 __ BCS $11e9 ; (nformf.l25 + 0)
.s24:
11a2 : 86 1b __ STX ACCU + 0 
11a4 : 86 43 __ STX T0 + 0 
11a6 : a5 12 __ LDA P5 ; (f + 1)
11a8 : 85 1c __ STA ACCU + 1 
11aa : 85 44 __ STA T0 + 1 
11ac : a5 13 __ LDA P6 ; (f + 2)
11ae : 85 1d __ STA ACCU + 2 
11b0 : 85 45 __ STA T0 + 2 
11b2 : a5 14 __ LDA P7 ; (f + 3)
11b4 : 85 1e __ STA ACCU + 3 
11b6 : 85 46 __ STA T0 + 3 
11b8 : 20 76 17 JSR $1776 ; (f32_to_i16 + 0)
11bb : a5 1b __ LDA ACCU + 0 
11bd : 85 51 __ STA T8 + 0 
11bf : 20 c2 17 JSR $17c2 ; (sint16_to_float + 0)
11c2 : a2 43 __ LDX #$43
11c4 : 20 b9 13 JSR $13b9 ; (freg + 4)
11c7 : a5 1e __ LDA ACCU + 3 
11c9 : 49 80 __ EOR #$80
11cb : 85 1e __ STA ACCU + 3 
11cd : 20 00 14 JSR $1400 ; (faddsub + 6)
11d0 : 20 76 19 JSR $1976 ; (freg@proxy + 0)
11d3 : 20 e7 14 JSR $14e7 ; (crt_fmul + 0)
11d6 : a5 1c __ LDA ACCU + 1 
11d8 : 85 12 __ STA P5 ; (f + 1)
11da : a5 1d __ LDA ACCU + 2 
11dc : 85 13 __ STA P6 ; (f + 2)
11de : a5 1e __ LDA ACCU + 3 
11e0 : 85 14 __ STA P7 ; (f + 3)
11e2 : 18 __ __ CLC
11e3 : a5 51 __ LDA T8 + 0 
11e5 : 69 30 __ ADC #$30
11e7 : a6 1b __ LDX ACCU + 0 
.l25:
11e9 : a4 52 __ LDY T9 + 0 
11eb : 91 0f __ STA (P2),y ; (str + 0)
11ed : e6 52 __ INC T9 + 0 
11ef : e6 54 __ INC T11 + 0 
11f1 : a5 54 __ LDA T11 + 0 
11f3 : c5 4f __ CMP T6 + 0 
11f5 : b0 14 __ BCS $120b ; (nformf.s26 + 0)
.s22:
11f7 : c5 4b __ CMP T4 + 0 
11f9 : d0 9f __ BNE $119a ; (nformf.s23 + 0)
.l43:
11fb : a9 2e __ LDA #$2e
11fd : a4 52 __ LDY T9 + 0 
11ff : 91 0f __ STA (P2),y ; (str + 0)
1201 : a5 54 __ LDA T11 + 0 
1203 : c9 07 __ CMP #$07
1205 : e6 52 __ INC T9 + 0 
1207 : 90 99 __ BCC $11a2 ; (nformf.s24 + 0)
1209 : b0 93 __ BCS $119e ; (nformf.l42 + 0)
.s26:
120b : a5 53 __ LDA T10 + 0 
120d : f0 66 __ BEQ $1275 ; (nformf.s27 + 0)
.s38:
120f : a0 03 __ LDY #$03
1211 : b1 0d __ LDA (P0),y ; (si + 0)
1213 : 69 03 __ ADC #$03
1215 : a4 52 __ LDY T9 + 0 
1217 : 91 0f __ STA (P2),y ; (str + 0)
1219 : c8 __ __ INY
121a : 84 52 __ STY T9 + 0 
121c : 24 4e __ BIT T5 + 1 
121e : 30 06 __ BMI $1226 ; (nformf.s41 + 0)
.s39:
1220 : a9 2b __ LDA #$2b
1222 : 91 0f __ STA (P2),y ; (str + 0)
1224 : d0 11 __ BNE $1237 ; (nformf.s40 + 0)
.s41:
1226 : a9 2d __ LDA #$2d
1228 : 91 0f __ STA (P2),y ; (str + 0)
122a : 38 __ __ SEC
122b : a9 00 __ LDA #$00
122d : e5 4d __ SBC T5 + 0 
122f : 85 4d __ STA T5 + 0 
1231 : a9 00 __ LDA #$00
1233 : e5 4e __ SBC T5 + 1 
1235 : 85 4e __ STA T5 + 1 
.s40:
1237 : a5 4d __ LDA T5 + 0 
1239 : 85 1b __ STA ACCU + 0 
123b : a5 4e __ LDA T5 + 1 
123d : 85 1c __ STA ACCU + 1 
123f : e6 52 __ INC T9 + 0 
1241 : a9 0a __ LDA #$0a
1243 : 85 03 __ STA WORK + 0 
1245 : a9 00 __ LDA #$00
1247 : 85 04 __ STA WORK + 1 
1249 : 20 5d 16 JSR $165d ; (divs16 + 0)
124c : 18 __ __ CLC
124d : a5 1b __ LDA ACCU + 0 
124f : 69 30 __ ADC #$30
1251 : a4 52 __ LDY T9 + 0 
1253 : 91 0f __ STA (P2),y ; (str + 0)
1255 : a5 4d __ LDA T5 + 0 
1257 : 85 1b __ STA ACCU + 0 
1259 : a5 4e __ LDA T5 + 1 
125b : 85 1c __ STA ACCU + 1 
125d : e6 52 __ INC T9 + 0 
125f : a9 0a __ LDA #$0a
1261 : 85 03 __ STA WORK + 0 
1263 : a9 00 __ LDA #$00
1265 : 85 04 __ STA WORK + 1 
1267 : 20 1f 17 JSR $171f ; (mods16 + 0)
126a : 18 __ __ CLC
126b : a5 05 __ LDA WORK + 2 
126d : 69 30 __ ADC #$30
126f : a4 52 __ LDY T9 + 0 
1271 : 91 0f __ STA (P2),y ; (str + 0)
1273 : e6 52 __ INC T9 + 0 
.s27:
1275 : a5 52 __ LDA T9 + 0 
1277 : a0 01 __ LDY #$01
1279 : d1 0d __ CMP (P0),y ; (si + 0)
127b : b0 69 __ BCS $12e6 ; (nformf.s3 + 0)
.s28:
127d : a0 06 __ LDY #$06
127f : b1 0d __ LDA (P0),y ; (si + 0)
1281 : f0 04 __ BEQ $1287 ; (nformf.s29 + 0)
.s92:
1283 : a6 52 __ LDX T9 + 0 
1285 : 90 6c __ BCC $12f3 ; (nformf.l36 + 0)
.s29:
1287 : a5 52 __ LDA T9 + 0 
1289 : f0 3c __ BEQ $12c7 ; (nformf.s30 + 0)
.s35:
128b : e9 00 __ SBC #$00
128d : a2 00 __ LDX #$00
128f : b0 01 __ BCS $1292 ; (nformf.s105 + 0)
.s104:
1291 : ca __ __ DEX
.s105:
1292 : 18 __ __ CLC
1293 : 65 0f __ ADC P2 ; (str + 0)
1295 : 85 43 __ STA T0 + 0 
1297 : 8a __ __ TXA
1298 : 65 10 __ ADC P3 ; (str + 1)
129a : 85 44 __ STA T0 + 1 
129c : a9 01 __ LDA #$01
129e : 85 47 __ STA T1 + 0 
12a0 : a6 10 __ LDX P3 ; (str + 1)
12a2 : 38 __ __ SEC
.l93:
12a3 : a0 01 __ LDY #$01
12a5 : b1 0d __ LDA (P0),y ; (si + 0)
12a7 : e5 47 __ SBC T1 + 0 
12a9 : 85 4b __ STA T4 + 0 
12ab : 8a __ __ TXA
12ac : 69 ff __ ADC #$ff
12ae : 85 4c __ STA T4 + 1 
12b0 : 88 __ __ DEY
12b1 : b1 43 __ LDA (T0 + 0),y 
12b3 : a4 0f __ LDY P2 ; (str + 0)
12b5 : 91 4b __ STA (T4 + 0),y 
12b7 : a5 43 __ LDA T0 + 0 
12b9 : d0 02 __ BNE $12bd ; (nformf.s96 + 0)
.s95:
12bb : c6 44 __ DEC T0 + 1 
.s96:
12bd : c6 43 __ DEC T0 + 0 
12bf : e6 47 __ INC T1 + 0 
12c1 : a5 52 __ LDA T9 + 0 
12c3 : c5 47 __ CMP T1 + 0 
12c5 : b0 dc __ BCS $12a3 ; (nformf.l93 + 0)
.s30:
12c7 : a9 00 __ LDA #$00
12c9 : 85 4b __ STA T4 + 0 
12cb : 90 08 __ BCC $12d5 ; (nformf.l31 + 0)
.s33:
12cd : a9 20 __ LDA #$20
12cf : a4 4b __ LDY T4 + 0 
12d1 : 91 0f __ STA (P2),y ; (str + 0)
12d3 : e6 4b __ INC T4 + 0 
.l31:
12d5 : a0 01 __ LDY #$01
12d7 : b1 0d __ LDA (P0),y ; (si + 0)
12d9 : 38 __ __ SEC
12da : e5 52 __ SBC T9 + 0 
12dc : 90 ef __ BCC $12cd ; (nformf.s33 + 0)
.s34:
12de : c5 4b __ CMP T4 + 0 
12e0 : 90 02 __ BCC $12e4 ; (nformf.s32 + 0)
.s94:
12e2 : d0 e9 __ BNE $12cd ; (nformf.s33 + 0)
.s32:
12e4 : b1 0d __ LDA (P0),y ; (si + 0)
.s3:
12e6 : 85 1b __ STA ACCU + 0 
12e8 : ad f1 9f LDA $9ff1 ; (nformf@stack + 0)
12eb : 85 53 __ STA T10 + 0 
12ed : ad f2 9f LDA $9ff2 ; (nformf@stack + 1)
12f0 : 85 54 __ STA T11 + 0 
12f2 : 60 __ __ RTS
.l36:
12f3 : 8a __ __ TXA
12f4 : a0 01 __ LDY #$01
12f6 : d1 0d __ CMP (P0),y ; (si + 0)
12f8 : b0 ea __ BCS $12e4 ; (nformf.s32 + 0)
.s37:
12fa : a8 __ __ TAY
12fb : a9 20 __ LDA #$20
12fd : 91 0f __ STA (P2),y ; (str + 0)
12ff : e8 __ __ INX
1300 : 90 f1 __ BCC $12f3 ; (nformf.l36 + 0)
.s51:
1302 : a5 4f __ LDA T6 + 0 
1304 : c9 07 __ CMP #$07
1306 : 90 14 __ BCC $131c ; (nformf.s58 + 0)
.s52:
1308 : ad d0 19 LDA $19d0 ; (fround5[0] + 24)
130b : 85 47 __ STA T1 + 0 
130d : ad d1 19 LDA $19d1 ; (fround5[0] + 25)
1310 : 85 48 __ STA T1 + 1 
1312 : ad d2 19 LDA $19d2 ; (fround5[0] + 26)
1315 : 85 49 __ STA T1 + 2 
1317 : ad d3 19 LDA $19d3 ; (fround5[0] + 27)
131a : b0 15 __ BCS $1331 ; (nformf.s53 + 0)
.s58:
131c : 0a __ __ ASL
131d : 0a __ __ ASL
131e : aa __ __ TAX
131f : bd b4 19 LDA $19b4,x ; (freg@proxy + 16)
1322 : 85 47 __ STA T1 + 0 
1324 : bd b5 19 LDA $19b5,x ; (freg@proxy + 17)
1327 : 85 48 __ STA T1 + 1 
1329 : bd b6 19 LDA $19b6,x ; (freg@proxy + 18)
132c : 85 49 __ STA T1 + 2 
132e : bd b7 19 LDA $19b7,x ; (spentry + 0)
.s53:
1331 : 85 4a __ STA T1 + 3 
1333 : a2 47 __ LDX #$47
1335 : 20 a4 19 JSR $19a4 ; (freg@proxy + 0)
1338 : 20 00 14 JSR $1400 ; (faddsub + 6)
133b : a5 1c __ LDA ACCU + 1 
133d : 85 12 __ STA P5 ; (f + 1)
133f : a5 1d __ LDA ACCU + 2 
1341 : 85 13 __ STA P6 ; (f + 2)
1343 : a6 1b __ LDX ACCU + 0 
1345 : a5 1e __ LDA ACCU + 3 
1347 : 85 14 __ STA P7 ; (f + 3)
1349 : 10 03 __ BPL $134e ; (nformf.s55 + 0)
134b : 4c 75 11 JMP $1175 ; (nformf.s18 + 0)
.s55:
134e : c9 41 __ CMP #$41
1350 : d0 06 __ BNE $1358 ; (nformf.s57 + 0)
.s56:
1352 : a5 13 __ LDA P6 ; (f + 2)
1354 : c9 20 __ CMP #$20
1356 : f0 02 __ BEQ $135a ; (nformf.s54 + 0)
.s57:
1358 : 90 f1 __ BCC $134b ; (nformf.s53 + 26)
.s54:
135a : 20 76 19 JSR $1976 ; (freg@proxy + 0)
135d : 20 af 15 JSR $15af ; (crt_fdiv + 0)
1360 : a5 1c __ LDA ACCU + 1 
1362 : 85 12 __ STA P5 ; (f + 1)
1364 : a5 1d __ LDA ACCU + 2 
1366 : 85 13 __ STA P6 ; (f + 2)
1368 : a5 1e __ LDA ACCU + 3 
136a : 85 14 __ STA P7 ; (f + 3)
136c : a6 1b __ LDX ACCU + 0 
136e : e6 4d __ INC T5 + 0 
1370 : d0 d9 __ BNE $134b ; (nformf.s53 + 26)
.s99:
1372 : e6 4e __ INC T5 + 1 
1374 : 4c 75 11 JMP $1175 ; (nformf.s18 + 0)
--------------------------------------------------------------------
1377 : __ __ __ BYT 25 64 3a 20 25 64 0a 00                         : %d: %d..
--------------------------------------------------------------------
mul32by8: ; mul32by8
137f : a0 00 __ LDY #$00
1381 : 84 07 __ STY WORK + 4 
1383 : 84 08 __ STY WORK + 5 
1385 : 84 09 __ STY WORK + 6 
1387 : 4a __ __ LSR
1388 : b0 0d __ BCS $1397 ; (mul32by8 + 24)
138a : f0 26 __ BEQ $13b2 ; (mul32by8 + 51)
138c : 06 1b __ ASL ACCU + 0 
138e : 26 1c __ ROL ACCU + 1 
1390 : 26 1d __ ROL ACCU + 2 
1392 : 26 1e __ ROL ACCU + 3 
1394 : 4a __ __ LSR
1395 : 90 f5 __ BCC $138c ; (mul32by8 + 13)
1397 : aa __ __ TAX
1398 : 18 __ __ CLC
1399 : a5 07 __ LDA WORK + 4 
139b : 65 1b __ ADC ACCU + 0 
139d : 85 07 __ STA WORK + 4 
139f : a5 08 __ LDA WORK + 5 
13a1 : 65 1c __ ADC ACCU + 1 
13a3 : 85 08 __ STA WORK + 5 
13a5 : a5 09 __ LDA WORK + 6 
13a7 : 65 1d __ ADC ACCU + 2 
13a9 : 85 09 __ STA WORK + 6 
13ab : 98 __ __ TYA
13ac : 65 1e __ ADC ACCU + 3 
13ae : a8 __ __ TAY
13af : 8a __ __ TXA
13b0 : d0 da __ BNE $138c ; (mul32by8 + 13)
13b2 : 84 0a __ STY WORK + 7 
13b4 : 60 __ __ RTS
--------------------------------------------------------------------
freg: ; freg
13b5 : b1 19 __ LDA (IP + 0),y 
13b7 : c8 __ __ INY
13b8 : aa __ __ TAX
13b9 : b5 00 __ LDA $00,x 
13bb : 85 03 __ STA WORK + 0 
13bd : b5 01 __ LDA $01,x 
13bf : 85 04 __ STA WORK + 1 
13c1 : b5 02 __ LDA $02,x 
13c3 : 85 05 __ STA WORK + 2 
13c5 : b5 03 __ LDA WORK + 0,x 
13c7 : 85 06 __ STA WORK + 3 
13c9 : a5 05 __ LDA WORK + 2 
13cb : 0a __ __ ASL
13cc : a5 06 __ LDA WORK + 3 
13ce : 2a __ __ ROL
13cf : 85 08 __ STA WORK + 5 
13d1 : f0 06 __ BEQ $13d9 ; (freg + 36)
13d3 : a5 05 __ LDA WORK + 2 
13d5 : 09 80 __ ORA #$80
13d7 : 85 05 __ STA WORK + 2 
13d9 : a5 1d __ LDA ACCU + 2 
13db : 0a __ __ ASL
13dc : a5 1e __ LDA ACCU + 3 
13de : 2a __ __ ROL
13df : 85 07 __ STA WORK + 4 
13e1 : f0 06 __ BEQ $13e9 ; (freg + 52)
13e3 : a5 1d __ LDA ACCU + 2 
13e5 : 09 80 __ ORA #$80
13e7 : 85 1d __ STA ACCU + 2 
13e9 : 60 __ __ RTS
13ea : 06 1e __ ASL ACCU + 3 
13ec : a5 07 __ LDA WORK + 4 
13ee : 6a __ __ ROR
13ef : 85 1e __ STA ACCU + 3 
13f1 : b0 06 __ BCS $13f9 ; (freg + 68)
13f3 : a5 1d __ LDA ACCU + 2 
13f5 : 29 7f __ AND #$7f
13f7 : 85 1d __ STA ACCU + 2 
13f9 : 60 __ __ RTS
--------------------------------------------------------------------
faddsub: ; faddsub
13fa : a5 06 __ LDA WORK + 3 
13fc : 49 80 __ EOR #$80
13fe : 85 06 __ STA WORK + 3 
1400 : a9 ff __ LDA #$ff
1402 : c5 07 __ CMP WORK + 4 
1404 : f0 04 __ BEQ $140a ; (faddsub + 16)
1406 : c5 08 __ CMP WORK + 5 
1408 : d0 11 __ BNE $141b ; (faddsub + 33)
140a : a5 1e __ LDA ACCU + 3 
140c : 09 7f __ ORA #$7f
140e : 85 1e __ STA ACCU + 3 
1410 : a9 80 __ LDA #$80
1412 : 85 1d __ STA ACCU + 2 
1414 : a9 00 __ LDA #$00
1416 : 85 1b __ STA ACCU + 0 
1418 : 85 1c __ STA ACCU + 1 
141a : 60 __ __ RTS
141b : 38 __ __ SEC
141c : a5 07 __ LDA WORK + 4 
141e : e5 08 __ SBC WORK + 5 
1420 : f0 38 __ BEQ $145a ; (faddsub + 96)
1422 : aa __ __ TAX
1423 : b0 25 __ BCS $144a ; (faddsub + 80)
1425 : e0 e9 __ CPX #$e9
1427 : b0 0e __ BCS $1437 ; (faddsub + 61)
1429 : a5 08 __ LDA WORK + 5 
142b : 85 07 __ STA WORK + 4 
142d : a9 00 __ LDA #$00
142f : 85 1b __ STA ACCU + 0 
1431 : 85 1c __ STA ACCU + 1 
1433 : 85 1d __ STA ACCU + 2 
1435 : f0 23 __ BEQ $145a ; (faddsub + 96)
1437 : a5 1d __ LDA ACCU + 2 
1439 : 4a __ __ LSR
143a : 66 1c __ ROR ACCU + 1 
143c : 66 1b __ ROR ACCU + 0 
143e : e8 __ __ INX
143f : d0 f8 __ BNE $1439 ; (faddsub + 63)
1441 : 85 1d __ STA ACCU + 2 
1443 : a5 08 __ LDA WORK + 5 
1445 : 85 07 __ STA WORK + 4 
1447 : 4c 5a 14 JMP $145a ; (faddsub + 96)
144a : e0 18 __ CPX #$18
144c : b0 33 __ BCS $1481 ; (faddsub + 135)
144e : a5 05 __ LDA WORK + 2 
1450 : 4a __ __ LSR
1451 : 66 04 __ ROR WORK + 1 
1453 : 66 03 __ ROR WORK + 0 
1455 : ca __ __ DEX
1456 : d0 f8 __ BNE $1450 ; (faddsub + 86)
1458 : 85 05 __ STA WORK + 2 
145a : a5 1e __ LDA ACCU + 3 
145c : 29 80 __ AND #$80
145e : 85 1e __ STA ACCU + 3 
1460 : 45 06 __ EOR WORK + 3 
1462 : 30 31 __ BMI $1495 ; (faddsub + 155)
1464 : 18 __ __ CLC
1465 : a5 1b __ LDA ACCU + 0 
1467 : 65 03 __ ADC WORK + 0 
1469 : 85 1b __ STA ACCU + 0 
146b : a5 1c __ LDA ACCU + 1 
146d : 65 04 __ ADC WORK + 1 
146f : 85 1c __ STA ACCU + 1 
1471 : a5 1d __ LDA ACCU + 2 
1473 : 65 05 __ ADC WORK + 2 
1475 : 85 1d __ STA ACCU + 2 
1477 : 90 08 __ BCC $1481 ; (faddsub + 135)
1479 : 66 1d __ ROR ACCU + 2 
147b : 66 1c __ ROR ACCU + 1 
147d : 66 1b __ ROR ACCU + 0 
147f : e6 07 __ INC WORK + 4 
1481 : a5 07 __ LDA WORK + 4 
1483 : c9 ff __ CMP #$ff
1485 : f0 83 __ BEQ $140a ; (faddsub + 16)
1487 : 4a __ __ LSR
1488 : 05 1e __ ORA ACCU + 3 
148a : 85 1e __ STA ACCU + 3 
148c : b0 06 __ BCS $1494 ; (faddsub + 154)
148e : a5 1d __ LDA ACCU + 2 
1490 : 29 7f __ AND #$7f
1492 : 85 1d __ STA ACCU + 2 
1494 : 60 __ __ RTS
1495 : 38 __ __ SEC
1496 : a5 1b __ LDA ACCU + 0 
1498 : e5 03 __ SBC WORK + 0 
149a : 85 1b __ STA ACCU + 0 
149c : a5 1c __ LDA ACCU + 1 
149e : e5 04 __ SBC WORK + 1 
14a0 : 85 1c __ STA ACCU + 1 
14a2 : a5 1d __ LDA ACCU + 2 
14a4 : e5 05 __ SBC WORK + 2 
14a6 : 85 1d __ STA ACCU + 2 
14a8 : b0 19 __ BCS $14c3 ; (faddsub + 201)
14aa : 38 __ __ SEC
14ab : a9 00 __ LDA #$00
14ad : e5 1b __ SBC ACCU + 0 
14af : 85 1b __ STA ACCU + 0 
14b1 : a9 00 __ LDA #$00
14b3 : e5 1c __ SBC ACCU + 1 
14b5 : 85 1c __ STA ACCU + 1 
14b7 : a9 00 __ LDA #$00
14b9 : e5 1d __ SBC ACCU + 2 
14bb : 85 1d __ STA ACCU + 2 
14bd : a5 1e __ LDA ACCU + 3 
14bf : 49 80 __ EOR #$80
14c1 : 85 1e __ STA ACCU + 3 
14c3 : a5 1d __ LDA ACCU + 2 
14c5 : 30 ba __ BMI $1481 ; (faddsub + 135)
14c7 : 05 1c __ ORA ACCU + 1 
14c9 : 05 1b __ ORA ACCU + 0 
14cb : f0 0f __ BEQ $14dc ; (faddsub + 226)
14cd : c6 07 __ DEC WORK + 4 
14cf : f0 0b __ BEQ $14dc ; (faddsub + 226)
14d1 : 06 1b __ ASL ACCU + 0 
14d3 : 26 1c __ ROL ACCU + 1 
14d5 : 26 1d __ ROL ACCU + 2 
14d7 : 10 f4 __ BPL $14cd ; (faddsub + 211)
14d9 : 4c 81 14 JMP $1481 ; (faddsub + 135)
14dc : a9 00 __ LDA #$00
14de : 85 1b __ STA ACCU + 0 
14e0 : 85 1c __ STA ACCU + 1 
14e2 : 85 1d __ STA ACCU + 2 
14e4 : 85 1e __ STA ACCU + 3 
14e6 : 60 __ __ RTS
--------------------------------------------------------------------
crt_fmul: ; crt_fmul
14e7 : a5 1b __ LDA ACCU + 0 
14e9 : 05 1c __ ORA ACCU + 1 
14eb : 05 1d __ ORA ACCU + 2 
14ed : f0 0e __ BEQ $14fd ; (crt_fmul + 22)
14ef : a5 03 __ LDA WORK + 0 
14f1 : 05 04 __ ORA WORK + 1 
14f3 : 05 05 __ ORA WORK + 2 
14f5 : d0 09 __ BNE $1500 ; (crt_fmul + 25)
14f7 : 85 1b __ STA ACCU + 0 
14f9 : 85 1c __ STA ACCU + 1 
14fb : 85 1d __ STA ACCU + 2 
14fd : 85 1e __ STA ACCU + 3 
14ff : 60 __ __ RTS
1500 : a5 1e __ LDA ACCU + 3 
1502 : 45 06 __ EOR WORK + 3 
1504 : 29 80 __ AND #$80
1506 : 85 1e __ STA ACCU + 3 
1508 : a9 ff __ LDA #$ff
150a : c5 07 __ CMP WORK + 4 
150c : f0 42 __ BEQ $1550 ; (crt_fmul + 105)
150e : c5 08 __ CMP WORK + 5 
1510 : f0 3e __ BEQ $1550 ; (crt_fmul + 105)
1512 : a9 00 __ LDA #$00
1514 : 85 09 __ STA WORK + 6 
1516 : 85 0a __ STA WORK + 7 
1518 : 85 0b __ STA WORK + 8 
151a : a4 1b __ LDY ACCU + 0 
151c : a5 03 __ LDA WORK + 0 
151e : d0 06 __ BNE $1526 ; (crt_fmul + 63)
1520 : a5 04 __ LDA WORK + 1 
1522 : f0 0a __ BEQ $152e ; (crt_fmul + 71)
1524 : d0 05 __ BNE $152b ; (crt_fmul + 68)
1526 : 20 81 15 JSR $1581 ; (crt_fmul8 + 0)
1529 : a5 04 __ LDA WORK + 1 
152b : 20 81 15 JSR $1581 ; (crt_fmul8 + 0)
152e : a5 05 __ LDA WORK + 2 
1530 : 20 81 15 JSR $1581 ; (crt_fmul8 + 0)
1533 : 38 __ __ SEC
1534 : a5 0b __ LDA WORK + 8 
1536 : 30 06 __ BMI $153e ; (crt_fmul + 87)
1538 : 06 09 __ ASL WORK + 6 
153a : 26 0a __ ROL WORK + 7 
153c : 2a __ __ ROL
153d : 18 __ __ CLC
153e : 29 7f __ AND #$7f
1540 : 85 0b __ STA WORK + 8 
1542 : a5 07 __ LDA WORK + 4 
1544 : 65 08 __ ADC WORK + 5 
1546 : 90 19 __ BCC $1561 ; (crt_fmul + 122)
1548 : e9 7f __ SBC #$7f
154a : b0 04 __ BCS $1550 ; (crt_fmul + 105)
154c : c9 ff __ CMP #$ff
154e : d0 15 __ BNE $1565 ; (crt_fmul + 126)
1550 : a5 1e __ LDA ACCU + 3 
1552 : 09 7f __ ORA #$7f
1554 : 85 1e __ STA ACCU + 3 
1556 : a9 80 __ LDA #$80
1558 : 85 1d __ STA ACCU + 2 
155a : a9 00 __ LDA #$00
155c : 85 1b __ STA ACCU + 0 
155e : 85 1c __ STA ACCU + 1 
1560 : 60 __ __ RTS
1561 : e9 7e __ SBC #$7e
1563 : 90 15 __ BCC $157a ; (crt_fmul + 147)
1565 : 4a __ __ LSR
1566 : 05 1e __ ORA ACCU + 3 
1568 : 85 1e __ STA ACCU + 3 
156a : a9 00 __ LDA #$00
156c : 6a __ __ ROR
156d : 05 0b __ ORA WORK + 8 
156f : 85 1d __ STA ACCU + 2 
1571 : a5 0a __ LDA WORK + 7 
1573 : 85 1c __ STA ACCU + 1 
1575 : a5 09 __ LDA WORK + 6 
1577 : 85 1b __ STA ACCU + 0 
1579 : 60 __ __ RTS
157a : a9 00 __ LDA #$00
157c : 85 1e __ STA ACCU + 3 
157e : f0 d8 __ BEQ $1558 ; (crt_fmul + 113)
1580 : 60 __ __ RTS
--------------------------------------------------------------------
crt_fmul8: ; crt_fmul8
1581 : 38 __ __ SEC
1582 : 6a __ __ ROR
1583 : 90 1e __ BCC $15a3 ; (crt_fmul8 + 34)
1585 : aa __ __ TAX
1586 : 18 __ __ CLC
1587 : 98 __ __ TYA
1588 : 65 09 __ ADC WORK + 6 
158a : 85 09 __ STA WORK + 6 
158c : a5 0a __ LDA WORK + 7 
158e : 65 1c __ ADC ACCU + 1 
1590 : 85 0a __ STA WORK + 7 
1592 : a5 0b __ LDA WORK + 8 
1594 : 65 1d __ ADC ACCU + 2 
1596 : 6a __ __ ROR
1597 : 85 0b __ STA WORK + 8 
1599 : 8a __ __ TXA
159a : 66 0a __ ROR WORK + 7 
159c : 66 09 __ ROR WORK + 6 
159e : 4a __ __ LSR
159f : f0 0d __ BEQ $15ae ; (crt_fmul8 + 45)
15a1 : b0 e2 __ BCS $1585 ; (crt_fmul8 + 4)
15a3 : 66 0b __ ROR WORK + 8 
15a5 : 66 0a __ ROR WORK + 7 
15a7 : 66 09 __ ROR WORK + 6 
15a9 : 4a __ __ LSR
15aa : 90 f7 __ BCC $15a3 ; (crt_fmul8 + 34)
15ac : d0 d7 __ BNE $1585 ; (crt_fmul8 + 4)
15ae : 60 __ __ RTS
--------------------------------------------------------------------
crt_fdiv: ; crt_fdiv
15af : a5 1b __ LDA ACCU + 0 
15b1 : 05 1c __ ORA ACCU + 1 
15b3 : 05 1d __ ORA ACCU + 2 
15b5 : d0 03 __ BNE $15ba ; (crt_fdiv + 11)
15b7 : 85 1e __ STA ACCU + 3 
15b9 : 60 __ __ RTS
15ba : a5 1e __ LDA ACCU + 3 
15bc : 45 06 __ EOR WORK + 3 
15be : 29 80 __ AND #$80
15c0 : 85 1e __ STA ACCU + 3 
15c2 : a5 08 __ LDA WORK + 5 
15c4 : f0 62 __ BEQ $1628 ; (crt_fdiv + 121)
15c6 : a5 07 __ LDA WORK + 4 
15c8 : c9 ff __ CMP #$ff
15ca : f0 5c __ BEQ $1628 ; (crt_fdiv + 121)
15cc : a9 00 __ LDA #$00
15ce : 85 09 __ STA WORK + 6 
15d0 : 85 0a __ STA WORK + 7 
15d2 : 85 0b __ STA WORK + 8 
15d4 : a2 18 __ LDX #$18
15d6 : a5 1b __ LDA ACCU + 0 
15d8 : c5 03 __ CMP WORK + 0 
15da : a5 1c __ LDA ACCU + 1 
15dc : e5 04 __ SBC WORK + 1 
15de : a5 1d __ LDA ACCU + 2 
15e0 : e5 05 __ SBC WORK + 2 
15e2 : 90 13 __ BCC $15f7 ; (crt_fdiv + 72)
15e4 : a5 1b __ LDA ACCU + 0 
15e6 : e5 03 __ SBC WORK + 0 
15e8 : 85 1b __ STA ACCU + 0 
15ea : a5 1c __ LDA ACCU + 1 
15ec : e5 04 __ SBC WORK + 1 
15ee : 85 1c __ STA ACCU + 1 
15f0 : a5 1d __ LDA ACCU + 2 
15f2 : e5 05 __ SBC WORK + 2 
15f4 : 85 1d __ STA ACCU + 2 
15f6 : 38 __ __ SEC
15f7 : 26 09 __ ROL WORK + 6 
15f9 : 26 0a __ ROL WORK + 7 
15fb : 26 0b __ ROL WORK + 8 
15fd : ca __ __ DEX
15fe : f0 0a __ BEQ $160a ; (crt_fdiv + 91)
1600 : 06 1b __ ASL ACCU + 0 
1602 : 26 1c __ ROL ACCU + 1 
1604 : 26 1d __ ROL ACCU + 2 
1606 : b0 dc __ BCS $15e4 ; (crt_fdiv + 53)
1608 : 90 cc __ BCC $15d6 ; (crt_fdiv + 39)
160a : 38 __ __ SEC
160b : a5 0b __ LDA WORK + 8 
160d : 30 06 __ BMI $1615 ; (crt_fdiv + 102)
160f : 06 09 __ ASL WORK + 6 
1611 : 26 0a __ ROL WORK + 7 
1613 : 2a __ __ ROL
1614 : 18 __ __ CLC
1615 : 29 7f __ AND #$7f
1617 : 85 0b __ STA WORK + 8 
1619 : a5 07 __ LDA WORK + 4 
161b : e5 08 __ SBC WORK + 5 
161d : 90 1a __ BCC $1639 ; (crt_fdiv + 138)
161f : 18 __ __ CLC
1620 : 69 7f __ ADC #$7f
1622 : b0 04 __ BCS $1628 ; (crt_fdiv + 121)
1624 : c9 ff __ CMP #$ff
1626 : d0 15 __ BNE $163d ; (crt_fdiv + 142)
1628 : a5 1e __ LDA ACCU + 3 
162a : 09 7f __ ORA #$7f
162c : 85 1e __ STA ACCU + 3 
162e : a9 80 __ LDA #$80
1630 : 85 1d __ STA ACCU + 2 
1632 : a9 00 __ LDA #$00
1634 : 85 1c __ STA ACCU + 1 
1636 : 85 1b __ STA ACCU + 0 
1638 : 60 __ __ RTS
1639 : 69 7f __ ADC #$7f
163b : 90 15 __ BCC $1652 ; (crt_fdiv + 163)
163d : 4a __ __ LSR
163e : 05 1e __ ORA ACCU + 3 
1640 : 85 1e __ STA ACCU + 3 
1642 : a9 00 __ LDA #$00
1644 : 6a __ __ ROR
1645 : 05 0b __ ORA WORK + 8 
1647 : 85 1d __ STA ACCU + 2 
1649 : a5 0a __ LDA WORK + 7 
164b : 85 1c __ STA ACCU + 1 
164d : a5 09 __ LDA WORK + 6 
164f : 85 1b __ STA ACCU + 0 
1651 : 60 __ __ RTS
1652 : a9 00 __ LDA #$00
1654 : 85 1e __ STA ACCU + 3 
1656 : 85 1d __ STA ACCU + 2 
1658 : 85 1c __ STA ACCU + 1 
165a : 85 1b __ STA ACCU + 0 
165c : 60 __ __ RTS
--------------------------------------------------------------------
divs16: ; divs16
165d : 24 1c __ BIT ACCU + 1 
165f : 10 0d __ BPL $166e ; (divs16 + 17)
1661 : 20 78 16 JSR $1678 ; (negaccu + 0)
1664 : 24 04 __ BIT WORK + 1 
1666 : 10 0d __ BPL $1675 ; (divs16 + 24)
1668 : 20 86 16 JSR $1686 ; (negtmp + 0)
166b : 4c 94 16 JMP $1694 ; (divmod + 0)
166e : 24 04 __ BIT WORK + 1 
1670 : 10 f9 __ BPL $166b ; (divs16 + 14)
1672 : 20 86 16 JSR $1686 ; (negtmp + 0)
1675 : 20 94 16 JSR $1694 ; (divmod + 0)
--------------------------------------------------------------------
negaccu: ; negaccu
1678 : 38 __ __ SEC
1679 : a9 00 __ LDA #$00
167b : e5 1b __ SBC ACCU + 0 
167d : 85 1b __ STA ACCU + 0 
167f : a9 00 __ LDA #$00
1681 : e5 1c __ SBC ACCU + 1 
1683 : 85 1c __ STA ACCU + 1 
1685 : 60 __ __ RTS
--------------------------------------------------------------------
negtmp: ; negtmp
1686 : 38 __ __ SEC
1687 : a9 00 __ LDA #$00
1689 : e5 03 __ SBC WORK + 0 
168b : 85 03 __ STA WORK + 0 
168d : a9 00 __ LDA #$00
168f : e5 04 __ SBC WORK + 1 
1691 : 85 04 __ STA WORK + 1 
1693 : 60 __ __ RTS
--------------------------------------------------------------------
divmod: ; divmod
1694 : a5 1c __ LDA ACCU + 1 
1696 : d0 3b __ BNE $16d3 ; (divmod + 63)
1698 : a5 04 __ LDA WORK + 1 
169a : d0 1e __ BNE $16ba ; (divmod + 38)
169c : 85 06 __ STA WORK + 3 
169e : a2 04 __ LDX #$04
16a0 : 06 1b __ ASL ACCU + 0 
16a2 : 2a __ __ ROL
16a3 : c5 03 __ CMP WORK + 0 
16a5 : 90 02 __ BCC $16a9 ; (divmod + 21)
16a7 : e5 03 __ SBC WORK + 0 
16a9 : 26 1b __ ROL ACCU + 0 
16ab : 2a __ __ ROL
16ac : c5 03 __ CMP WORK + 0 
16ae : 90 02 __ BCC $16b2 ; (divmod + 30)
16b0 : e5 03 __ SBC WORK + 0 
16b2 : 26 1b __ ROL ACCU + 0 
16b4 : ca __ __ DEX
16b5 : d0 eb __ BNE $16a2 ; (divmod + 14)
16b7 : 85 05 __ STA WORK + 2 
16b9 : 60 __ __ RTS
16ba : a5 1b __ LDA ACCU + 0 
16bc : 85 05 __ STA WORK + 2 
16be : a5 1c __ LDA ACCU + 1 
16c0 : 85 06 __ STA WORK + 3 
16c2 : a9 00 __ LDA #$00
16c4 : 85 1b __ STA ACCU + 0 
16c6 : 85 1c __ STA ACCU + 1 
16c8 : 60 __ __ RTS
16c9 : 85 03 __ STA WORK + 0 
16cb : a9 00 __ LDA #$00
16cd : 85 04 __ STA WORK + 1 
16cf : a5 1c __ LDA ACCU + 1 
16d1 : f0 c9 __ BEQ $169c ; (divmod + 8)
16d3 : a5 04 __ LDA WORK + 1 
16d5 : d0 1f __ BNE $16f6 ; (divmod + 98)
16d7 : a5 03 __ LDA WORK + 0 
16d9 : 30 1b __ BMI $16f6 ; (divmod + 98)
16db : a9 00 __ LDA #$00
16dd : 85 06 __ STA WORK + 3 
16df : a2 10 __ LDX #$10
16e1 : 06 1b __ ASL ACCU + 0 
16e3 : 26 1c __ ROL ACCU + 1 
16e5 : 2a __ __ ROL
16e6 : c5 03 __ CMP WORK + 0 
16e8 : 90 02 __ BCC $16ec ; (divmod + 88)
16ea : e5 03 __ SBC WORK + 0 
16ec : 26 1b __ ROL ACCU + 0 
16ee : 26 1c __ ROL ACCU + 1 
16f0 : ca __ __ DEX
16f1 : d0 f2 __ BNE $16e5 ; (divmod + 81)
16f3 : 85 05 __ STA WORK + 2 
16f5 : 60 __ __ RTS
16f6 : a9 00 __ LDA #$00
16f8 : 85 05 __ STA WORK + 2 
16fa : 85 06 __ STA WORK + 3 
16fc : a0 10 __ LDY #$10
16fe : 18 __ __ CLC
16ff : 26 1b __ ROL ACCU + 0 
1701 : 26 1c __ ROL ACCU + 1 
1703 : 26 05 __ ROL WORK + 2 
1705 : 26 06 __ ROL WORK + 3 
1707 : 38 __ __ SEC
1708 : a5 05 __ LDA WORK + 2 
170a : e5 03 __ SBC WORK + 0 
170c : aa __ __ TAX
170d : a5 06 __ LDA WORK + 3 
170f : e5 04 __ SBC WORK + 1 
1711 : 90 04 __ BCC $1717 ; (divmod + 131)
1713 : 86 05 __ STX WORK + 2 
1715 : 85 06 __ STA WORK + 3 
1717 : 88 __ __ DEY
1718 : d0 e5 __ BNE $16ff ; (divmod + 107)
171a : 26 1b __ ROL ACCU + 0 
171c : 26 1c __ ROL ACCU + 1 
171e : 60 __ __ RTS
--------------------------------------------------------------------
mods16: ; mods16
171f : 24 1c __ BIT ACCU + 1 
1721 : 10 10 __ BPL $1733 ; (mods16 + 20)
1723 : 20 78 16 JSR $1678 ; (negaccu + 0)
1726 : 24 04 __ BIT WORK + 1 
1728 : 10 03 __ BPL $172d ; (mods16 + 14)
172a : 20 86 16 JSR $1686 ; (negtmp + 0)
172d : 20 94 16 JSR $1694 ; (divmod + 0)
1730 : 4c 68 17 JMP $1768 ; (negtmpb + 0)
1733 : 24 04 __ BIT WORK + 1 
1735 : 10 03 __ BPL $173a ; (mods16 + 27)
1737 : 20 86 16 JSR $1686 ; (negtmp + 0)
173a : 4c 94 16 JMP $1694 ; (divmod + 0)
173d : 60 __ __ RTS
--------------------------------------------------------------------
negtmpb: ; negtmpb
1768 : 38 __ __ SEC
1769 : a9 00 __ LDA #$00
176b : e5 05 __ SBC WORK + 2 
176d : 85 05 __ STA WORK + 2 
176f : a9 00 __ LDA #$00
1771 : e5 06 __ SBC WORK + 3 
1773 : 85 06 __ STA WORK + 3 
1775 : 60 __ __ RTS
--------------------------------------------------------------------
f32_to_i16: ; f32_to_i16
1776 : 20 d9 13 JSR $13d9 ; (freg + 36)
1779 : a5 07 __ LDA WORK + 4 
177b : c9 7f __ CMP #$7f
177d : b0 07 __ BCS $1786 ; (f32_to_i16 + 16)
177f : a9 00 __ LDA #$00
1781 : 85 1b __ STA ACCU + 0 
1783 : 85 1c __ STA ACCU + 1 
1785 : 60 __ __ RTS
1786 : e9 8e __ SBC #$8e
1788 : 90 16 __ BCC $17a0 ; (f32_to_i16 + 42)
178a : 24 1e __ BIT ACCU + 3 
178c : 30 09 __ BMI $1797 ; (f32_to_i16 + 33)
178e : a9 ff __ LDA #$ff
1790 : 85 1b __ STA ACCU + 0 
1792 : a9 7f __ LDA #$7f
1794 : 85 1c __ STA ACCU + 1 
1796 : 60 __ __ RTS
1797 : a9 00 __ LDA #$00
1799 : 85 1b __ STA ACCU + 0 
179b : a9 80 __ LDA #$80
179d : 85 1c __ STA ACCU + 1 
179f : 60 __ __ RTS
17a0 : aa __ __ TAX
17a1 : a5 1c __ LDA ACCU + 1 
17a3 : 46 1d __ LSR ACCU + 2 
17a5 : 6a __ __ ROR
17a6 : e8 __ __ INX
17a7 : d0 fa __ BNE $17a3 ; (f32_to_i16 + 45)
17a9 : 24 1e __ BIT ACCU + 3 
17ab : 10 0e __ BPL $17bb ; (f32_to_i16 + 69)
17ad : 38 __ __ SEC
17ae : 49 ff __ EOR #$ff
17b0 : 69 00 __ ADC #$00
17b2 : 85 1b __ STA ACCU + 0 
17b4 : a9 00 __ LDA #$00
17b6 : e5 1d __ SBC ACCU + 2 
17b8 : 85 1c __ STA ACCU + 1 
17ba : 60 __ __ RTS
17bb : 85 1b __ STA ACCU + 0 
17bd : a5 1d __ LDA ACCU + 2 
17bf : 85 1c __ STA ACCU + 1 
17c1 : 60 __ __ RTS
--------------------------------------------------------------------
sint16_to_float: ; sint16_to_float
17c2 : 24 1c __ BIT ACCU + 1 
17c4 : 30 03 __ BMI $17c9 ; (sint16_to_float + 7)
17c6 : 4c e0 17 JMP $17e0 ; (uint16_to_float + 0)
17c9 : 38 __ __ SEC
17ca : a9 00 __ LDA #$00
17cc : e5 1b __ SBC ACCU + 0 
17ce : 85 1b __ STA ACCU + 0 
17d0 : a9 00 __ LDA #$00
17d2 : e5 1c __ SBC ACCU + 1 
17d4 : 85 1c __ STA ACCU + 1 
17d6 : 20 e0 17 JSR $17e0 ; (uint16_to_float + 0)
17d9 : a5 1e __ LDA ACCU + 3 
17db : 09 80 __ ORA #$80
17dd : 85 1e __ STA ACCU + 3 
17df : 60 __ __ RTS
--------------------------------------------------------------------
uint16_to_float: ; uint16_to_float
17e0 : a5 1b __ LDA ACCU + 0 
17e2 : 05 1c __ ORA ACCU + 1 
17e4 : d0 05 __ BNE $17eb ; (uint16_to_float + 11)
17e6 : 85 1d __ STA ACCU + 2 
17e8 : 85 1e __ STA ACCU + 3 
17ea : 60 __ __ RTS
17eb : a2 8e __ LDX #$8e
17ed : a5 1c __ LDA ACCU + 1 
17ef : 30 06 __ BMI $17f7 ; (uint16_to_float + 23)
17f1 : ca __ __ DEX
17f2 : 06 1b __ ASL ACCU + 0 
17f4 : 2a __ __ ROL
17f5 : 10 fa __ BPL $17f1 ; (uint16_to_float + 17)
17f7 : 0a __ __ ASL
17f8 : 85 1d __ STA ACCU + 2 
17fa : a5 1b __ LDA ACCU + 0 
17fc : 85 1c __ STA ACCU + 1 
17fe : 8a __ __ TXA
17ff : 4a __ __ LSR
1800 : 85 1e __ STA ACCU + 3 
1802 : a9 00 __ LDA #$00
1804 : 85 1b __ STA ACCU + 0 
1806 : 66 1d __ ROR ACCU + 2 
1808 : 60 __ __ RTS
--------------------------------------------------------------------
mul32: ; mul32
1809 : a5 04 __ LDA WORK + 1 
180b : 05 05 __ ORA WORK + 2 
180d : 05 06 __ ORA WORK + 3 
180f : d0 05 __ BNE $1816 ; (mul32 + 13)
1811 : a5 03 __ LDA WORK + 0 
1813 : 4c 7f 13 JMP $137f ; (mul32by8 + 0)
1816 : a0 00 __ LDY #$00
1818 : 84 07 __ STY WORK + 4 
181a : 84 08 __ STY WORK + 5 
181c : 98 __ __ TYA
181d : 38 __ __ SEC
181e : 66 03 __ ROR WORK + 0 
1820 : 90 15 __ BCC $1837 ; (mul32 + 46)
1822 : aa __ __ TAX
1823 : 18 __ __ CLC
1824 : a5 07 __ LDA WORK + 4 
1826 : 65 1b __ ADC ACCU + 0 
1828 : 85 07 __ STA WORK + 4 
182a : a5 08 __ LDA WORK + 5 
182c : 65 1c __ ADC ACCU + 1 
182e : 85 08 __ STA WORK + 5 
1830 : 98 __ __ TYA
1831 : 65 1d __ ADC ACCU + 2 
1833 : a8 __ __ TAY
1834 : 8a __ __ TXA
1835 : 65 1e __ ADC ACCU + 3 
1837 : 46 04 __ LSR WORK + 1 
1839 : 90 0f __ BCC $184a ; (mul32 + 65)
183b : aa __ __ TAX
183c : 18 __ __ CLC
183d : a5 08 __ LDA WORK + 5 
183f : 65 1b __ ADC ACCU + 0 
1841 : 85 08 __ STA WORK + 5 
1843 : 98 __ __ TYA
1844 : 65 1c __ ADC ACCU + 1 
1846 : a8 __ __ TAY
1847 : 8a __ __ TXA
1848 : 65 1d __ ADC ACCU + 2 
184a : 46 05 __ LSR WORK + 2 
184c : 90 09 __ BCC $1857 ; (mul32 + 78)
184e : aa __ __ TAX
184f : 18 __ __ CLC
1850 : 98 __ __ TYA
1851 : 65 1b __ ADC ACCU + 0 
1853 : a8 __ __ TAY
1854 : 8a __ __ TXA
1855 : 65 1c __ ADC ACCU + 1 
1857 : 46 06 __ LSR WORK + 3 
1859 : 90 03 __ BCC $185e ; (mul32 + 85)
185b : 18 __ __ CLC
185c : 65 1b __ ADC ACCU + 0 
185e : 06 1b __ ASL ACCU + 0 
1860 : 26 1c __ ROL ACCU + 1 
1862 : 26 1d __ ROL ACCU + 2 
1864 : 26 1e __ ROL ACCU + 3 
1866 : 46 03 __ LSR WORK + 0 
1868 : 90 cd __ BCC $1837 ; (mul32 + 46)
186a : d0 b6 __ BNE $1822 ; (mul32 + 25)
186c : 84 09 __ STY WORK + 6 
186e : 85 0a __ STA WORK + 7 
1870 : 60 __ __ RTS
--------------------------------------------------------------------
divmod32: ; divmod32
188f : a9 00 __ LDA #$00
1891 : 85 07 __ STA WORK + 4 
1893 : 85 08 __ STA WORK + 5 
1895 : 85 09 __ STA WORK + 6 
1897 : 85 0a __ STA WORK + 7 
1899 : a5 05 __ LDA WORK + 2 
189b : 05 06 __ ORA WORK + 3 
189d : f0 4b __ BEQ $18ea ; (divmod32 + 91)
189f : a0 10 __ LDY #$10
18a1 : a5 1e __ LDA ACCU + 3 
18a3 : 85 08 __ STA WORK + 5 
18a5 : a5 1d __ LDA ACCU + 2 
18a7 : 85 07 __ STA WORK + 4 
18a9 : a9 00 __ LDA #$00
18ab : 85 1d __ STA ACCU + 2 
18ad : 85 1e __ STA ACCU + 3 
18af : 18 __ __ CLC
18b0 : 26 1b __ ROL ACCU + 0 
18b2 : 26 1c __ ROL ACCU + 1 
18b4 : 26 07 __ ROL WORK + 4 
18b6 : 26 08 __ ROL WORK + 5 
18b8 : 26 09 __ ROL WORK + 6 
18ba : 26 0a __ ROL WORK + 7 
18bc : a5 07 __ LDA WORK + 4 
18be : c5 03 __ CMP WORK + 0 
18c0 : a5 08 __ LDA WORK + 5 
18c2 : e5 04 __ SBC WORK + 1 
18c4 : a5 09 __ LDA WORK + 6 
18c6 : e5 05 __ SBC WORK + 2 
18c8 : aa __ __ TAX
18c9 : a5 0a __ LDA WORK + 7 
18cb : e5 06 __ SBC WORK + 3 
18cd : 90 11 __ BCC $18e0 ; (divmod32 + 81)
18cf : 86 09 __ STX WORK + 6 
18d1 : 85 0a __ STA WORK + 7 
18d3 : a5 07 __ LDA WORK + 4 
18d5 : e5 03 __ SBC WORK + 0 
18d7 : 85 07 __ STA WORK + 4 
18d9 : a5 08 __ LDA WORK + 5 
18db : e5 04 __ SBC WORK + 1 
18dd : 85 08 __ STA WORK + 5 
18df : 38 __ __ SEC
18e0 : 88 __ __ DEY
18e1 : d0 cd __ BNE $18b0 ; (divmod32 + 33)
18e3 : 26 1b __ ROL ACCU + 0 
18e5 : 26 1c __ ROL ACCU + 1 
18e7 : a4 02 __ LDY $02 
18e9 : 60 __ __ RTS
18ea : a5 1d __ LDA ACCU + 2 
18ec : 05 1e __ ORA ACCU + 3 
18ee : d0 0c __ BNE $18fc ; (divmod32 + 109)
18f0 : 20 94 16 JSR $1694 ; (divmod + 0)
18f3 : a5 05 __ LDA WORK + 2 
18f5 : 85 07 __ STA WORK + 4 
18f7 : a5 06 __ LDA WORK + 3 
18f9 : 85 08 __ STA WORK + 5 
18fb : 60 __ __ RTS
18fc : a0 20 __ LDY #$20
18fe : a5 04 __ LDA WORK + 1 
1900 : d0 27 __ BNE $1929 ; (divmod32 + 154)
1902 : 18 __ __ CLC
1903 : 26 1b __ ROL ACCU + 0 
1905 : 26 1c __ ROL ACCU + 1 
1907 : 26 1d __ ROL ACCU + 2 
1909 : 26 1e __ ROL ACCU + 3 
190b : 2a __ __ ROL
190c : 90 05 __ BCC $1913 ; (divmod32 + 132)
190e : e5 03 __ SBC WORK + 0 
1910 : 38 __ __ SEC
1911 : b0 06 __ BCS $1919 ; (divmod32 + 138)
1913 : c5 03 __ CMP WORK + 0 
1915 : 90 02 __ BCC $1919 ; (divmod32 + 138)
1917 : e5 03 __ SBC WORK + 0 
1919 : 88 __ __ DEY
191a : d0 e7 __ BNE $1903 ; (divmod32 + 116)
191c : 85 07 __ STA WORK + 4 
191e : 26 1b __ ROL ACCU + 0 
1920 : 26 1c __ ROL ACCU + 1 
1922 : 26 1d __ ROL ACCU + 2 
1924 : 26 1e __ ROL ACCU + 3 
1926 : a4 02 __ LDY $02 
1928 : 60 __ __ RTS
1929 : a5 1e __ LDA ACCU + 3 
192b : d0 10 __ BNE $193d ; (divmod32 + 174)
192d : a6 1d __ LDX ACCU + 2 
192f : 86 1e __ STX ACCU + 3 
1931 : a6 1c __ LDX ACCU + 1 
1933 : 86 1d __ STX ACCU + 2 
1935 : a6 1b __ LDX ACCU + 0 
1937 : 86 1c __ STX ACCU + 1 
1939 : 85 1b __ STA ACCU + 0 
193b : a0 18 __ LDY #$18
193d : 18 __ __ CLC
193e : 26 1b __ ROL ACCU + 0 
1940 : 26 1c __ ROL ACCU + 1 
1942 : 26 1d __ ROL ACCU + 2 
1944 : 26 1e __ ROL ACCU + 3 
1946 : 26 07 __ ROL WORK + 4 
1948 : 26 08 __ ROL WORK + 5 
194a : 90 0c __ BCC $1958 ; (divmod32 + 201)
194c : a5 07 __ LDA WORK + 4 
194e : e5 03 __ SBC WORK + 0 
1950 : aa __ __ TAX
1951 : a5 08 __ LDA WORK + 5 
1953 : e5 04 __ SBC WORK + 1 
1955 : 38 __ __ SEC
1956 : b0 0c __ BCS $1964 ; (divmod32 + 213)
1958 : 38 __ __ SEC
1959 : a5 07 __ LDA WORK + 4 
195b : e5 03 __ SBC WORK + 0 
195d : aa __ __ TAX
195e : a5 08 __ LDA WORK + 5 
1960 : e5 04 __ SBC WORK + 1 
1962 : 90 04 __ BCC $1968 ; (divmod32 + 217)
1964 : 86 07 __ STX WORK + 4 
1966 : 85 08 __ STA WORK + 5 
1968 : 88 __ __ DEY
1969 : d0 d3 __ BNE $193e ; (divmod32 + 175)
196b : 26 1b __ ROL ACCU + 0 
196d : 26 1c __ ROL ACCU + 1 
196f : 26 1d __ ROL ACCU + 2 
1971 : 26 1e __ ROL ACCU + 3 
1973 : a4 02 __ LDY $02 
1975 : 60 __ __ RTS
--------------------------------------------------------------------
freg@proxy: ; freg@proxy
1976 : a9 00 __ LDA #$00
1978 : 85 03 __ STA WORK + 0 
197a : a9 00 __ LDA #$00
197c : 85 04 __ STA WORK + 1 
197e : a9 20 __ LDA #$20
1980 : 85 05 __ STA WORK + 2 
1982 : a9 41 __ LDA #$41
1984 : 85 06 __ STA WORK + 3 
1986 : 4c c9 13 JMP $13c9 ; (freg + 20)
--------------------------------------------------------------------
freg@proxy: ; freg@proxy
1989 : a9 00 __ LDA #$00
198b : 85 03 __ STA WORK + 0 
198d : a9 00 __ LDA #$00
198f : 85 04 __ STA WORK + 1 
1991 : a5 43 __ LDA $43 
1993 : 85 1b __ STA ACCU + 0 
1995 : a5 44 __ LDA $44 
1997 : 85 1c __ STA ACCU + 1 
1999 : a5 45 __ LDA $45 
199b : 85 1d __ STA ACCU + 2 
199d : a5 46 __ LDA $46 
199f : 85 1e __ STA ACCU + 3 
19a1 : 4c c9 13 JMP $13c9 ; (freg + 20)
--------------------------------------------------------------------
freg@proxy: ; freg@proxy
19a4 : a5 43 __ LDA $43 
19a6 : 85 1b __ STA ACCU + 0 
19a8 : a5 44 __ LDA $44 
19aa : 85 1c __ STA ACCU + 1 
19ac : a5 45 __ LDA $45 
19ae : 85 1d __ STA ACCU + 2 
19b0 : a5 46 __ LDA $46 
19b2 : 85 1e __ STA ACCU + 3 
19b4 : 4c b9 13 JMP $13b9 ; (freg + 4)
--------------------------------------------------------------------
spentry:
19b7 : __ __ __ BYT 00                                              : .
--------------------------------------------------------------------
fround5:
19b8 : __ __ __ BYT 00 00 00 3f cd cc 4c 3d 0a d7 a3 3b 6f 12 03 3a : ...?..L=...;o..:
19c8 : __ __ __ BYT 17 b7 51 38 ac c5 a7 36 bd 37 06 35             : ..Q8...6.7.5
--------------------------------------------------------------------
seed:
19d4 : __ __ __ BYT 6f 00 00 00                                     : o...
