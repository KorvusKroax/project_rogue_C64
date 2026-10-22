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
080e : 8e fa 13 STX $13fa ; (spentry + 0)
0811 : a2 1a __ LDX #$1a
0813 : a0 52 __ LDY #$52
0815 : a9 00 __ LDA #$00
0817 : 85 19 __ STA IP + 0 
0819 : 86 1a __ STX IP + 1 
081b : e0 1a __ CPX #$1a
081d : f0 0b __ BEQ $082a ; (startup + 41)
081f : 91 19 __ STA (IP + 0),y 
0821 : c8 __ __ INY
0822 : d0 fb __ BNE $081f ; (startup + 30)
0824 : e8 __ __ INX
0825 : d0 f2 __ BNE $0819 ; (startup + 24)
0827 : 91 19 __ STA (IP + 0),y 
0829 : c8 __ __ INY
082a : c0 52 __ CPY #$52
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
;  16, "C:/Users/korvus/Projects/C64/project_rogue_C64/src/demo/main.c"
.s1:
0880 : a5 53 __ LDA T1 + 0 
0882 : 8d 9f 9f STA $9f9f ; (main@stack + 0)
.s4:
0885 : a9 93 __ LDA #$93
0887 : 20 d2 ff JSR $ffd2 
088a : a9 00 __ LDA #$00
088c : 85 53 __ STA T1 + 0 
.l5:
088e : a9 7f __ LDA #$7f
0890 : 8d fa 9f STA $9ffa ; (sstack + 0)
0893 : a9 13 __ LDA #$13
0895 : 8d fb 9f STA $9ffb ; (sstack + 1)
0898 : a5 53 __ LDA T1 + 0 
089a : 8d fc 9f STA $9ffc ; (sstack + 2)
089d : a9 00 __ LDA #$00
089f : 8d fd 9f STA $9ffd ; (sstack + 3)
08a2 : 20 87 13 JSR $1387 ; (RN_asm.s4 + 0)
08a5 : a9 64 __ LDA #$64
08a7 : 20 b0 17 JSR $17b0 ; (divmod + 53)
08aa : a5 05 __ LDA WORK + 2 
08ac : 8d fe 9f STA $9ffe ; (sstack + 4)
08af : a9 00 __ LDA #$00
08b1 : 8d ff 9f STA $9fff ; (sstack + 5)
08b4 : 20 37 09 JSR $0937 ; (printf.s1 + 0)
08b7 : e6 53 __ INC T1 + 0 
08b9 : a5 53 __ LDA T1 + 0 
08bb : c9 0a __ CMP #$0a
08bd : 90 cf __ BCC $088e ; (main.l5 + 0)
.s6:
08bf : a9 43 __ LDA #$43
08c1 : a2 00 __ LDX #$00
.l7:
08c3 : 9d 84 04 STA $0484,x 
08c6 : 9d 24 05 STA $0524,x 
08c9 : e8 __ __ INX
08ca : e0 0a __ CPX #$0a
08cc : d0 f5 __ BNE $08c3 ; (main.l7 + 0)
.s8:
08ce : a9 42 __ LDA #$42
08d0 : 8d ac 04 STA $04ac 
08d3 : 8d b5 04 STA $04b5 
08d6 : 8d d4 04 STA $04d4 
08d9 : 8d dd 04 STA $04dd 
08dc : 8d fc 04 STA $04fc 
08df : 8d 05 05 STA $0505 
08e2 : a9 ef __ LDA #$ef
08e4 : 8d fa 9f STA $9ffa ; (sstack + 0)
08e7 : a9 13 __ LDA #$13
08e9 : 8d fb 9f STA $9ffb ; (sstack + 1)
08ec : 20 37 09 JSR $0937 ; (printf.s1 + 0)
08ef : 20 23 14 JSR $1423 ; (ram_test.s4 + 0)
08f2 : a5 1b __ LDA ACCU + 0 
08f4 : 8d fc 9f STA $9ffc ; (sstack + 2)
08f7 : a5 1c __ LDA ACCU + 1 
08f9 : 8d fd 9f STA $9ffd ; (sstack + 3)
08fc : a9 00 __ LDA #$00
08fe : 8d fa 9f STA $9ffa ; (sstack + 0)
0901 : a9 14 __ LDA #$14
0903 : 8d fb 9f STA $9ffb ; (sstack + 1)
0906 : 20 37 09 JSR $0937 ; (printf.s1 + 0)
0909 : a9 36 __ LDA #$36
090b : 85 01 __ STA $01 
090d : 20 23 14 JSR $1423 ; (ram_test.s4 + 0)
0910 : a5 1b __ LDA ACCU + 0 
0912 : 8d fc 9f STA $9ffc ; (sstack + 2)
0915 : a5 1c __ LDA ACCU + 1 
0917 : 8d fd 9f STA $9ffd ; (sstack + 3)
091a : a9 79 __ LDA #$79
091c : 8d fa 9f STA $9ffa ; (sstack + 0)
091f : a9 14 __ LDA #$14
0921 : 8d fb 9f STA $9ffb ; (sstack + 1)
0924 : 20 37 09 JSR $0937 ; (printf.s1 + 0)
0927 : a9 37 __ LDA #$37
0929 : 85 01 __ STA $01 
092b : a9 00 __ LDA #$00
092d : 85 1b __ STA ACCU + 0 
092f : 85 1c __ STA ACCU + 1 
.s3:
0931 : ad 9f 9f LDA $9f9f ; (main@stack + 0)
0934 : 85 53 __ STA T1 + 0 
0936 : 60 __ __ RTS
--------------------------------------------------------------------
printf: ; printf(const u8*)->void
;  18, "C:/oscar64/include/stdio.h"
.s1:
0937 : a2 08 __ LDX #$08
0939 : b5 53 __ LDA T1 + 0,x 
093b : 9d a3 9f STA $9fa3,x ; (printf@stack + 0)
093e : ca __ __ DEX
093f : 10 f8 __ BPL $0939 ; (printf.s1 + 2)
.s4:
0941 : ad fa 9f LDA $9ffa ; (sstack + 0)
0944 : 85 58 __ STA T5 + 0 
0946 : a9 fc __ LDA #$fc
0948 : 85 56 __ STA T3 + 0 
094a : a9 9f __ LDA #$9f
094c : 85 57 __ STA T3 + 1 
094e : a9 00 __ LDA #$00
0950 : 85 5b __ STA T7 + 0 
0952 : ad fb 9f LDA $9ffb ; (sstack + 1)
0955 : 85 59 __ STA T5 + 1 
.l5:
0957 : a0 00 __ LDY #$00
0959 : b1 58 __ LDA (T5 + 0),y 
095b : d0 15 __ BNE $0972 ; (printf.s8 + 0)
.s6:
095d : a6 5b __ LDX T7 + 0 
095f : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0962 : f0 03 __ BEQ $0967 ; (printf.s3 + 0)
.s7:
0964 : 20 96 0c JSR $0c96 ; (puts@proxy + 0)
.s3:
0967 : a2 08 __ LDX #$08
0969 : bd a3 9f LDA $9fa3,x ; (printf@stack + 0)
096c : 95 53 __ STA T1 + 0,x 
096e : ca __ __ DEX
096f : 10 f8 __ BPL $0969 ; (printf.s3 + 2)
0971 : 60 __ __ RTS
.s8:
0972 : c9 25 __ CMP #$25
0974 : f0 28 __ BEQ $099e ; (printf.s12 + 0)
.s9:
0976 : a6 5b __ LDX T7 + 0 
0978 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
097b : e6 58 __ INC T5 + 0 
097d : d0 02 __ BNE $0981 ; (printf.s110 + 0)
.s109:
097f : e6 59 __ INC T5 + 1 
.s110:
0981 : e8 __ __ INX
0982 : 86 5b __ STX T7 + 0 
0984 : e0 28 __ CPX #$28
0986 : 90 cf __ BCC $0957 ; (printf.l5 + 0)
.s10:
0988 : a9 b8 __ LDA #$b8
098a : 85 0d __ STA P0 
098c : a9 9f __ LDA #$9f
098e : 85 0e __ STA P1 
0990 : 98 __ __ TYA
0991 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
.s11:
0994 : 20 9e 0c JSR $0c9e ; (puts.s4 + 0)
0997 : a9 00 __ LDA #$00
.s92:
0999 : 85 5b __ STA T7 + 0 
099b : 4c 57 09 JMP $0957 ; (printf.l5 + 0)
.s12:
099e : a5 5b __ LDA T7 + 0 
09a0 : f0 0c __ BEQ $09ae ; (printf.s13 + 0)
.s85:
09a2 : aa __ __ TAX
09a3 : 98 __ __ TYA
09a4 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
09a7 : 20 96 0c JSR $0c96 ; (puts@proxy + 0)
09aa : a9 00 __ LDA #$00
09ac : 85 5b __ STA T7 + 0 
.s13:
09ae : 8d b5 9f STA $9fb5 ; (si.sign + 0)
09b1 : 8d b6 9f STA $9fb6 ; (si.left + 0)
09b4 : 8d b7 9f STA $9fb7 ; (si.prefix + 0)
09b7 : a0 01 __ LDY #$01
09b9 : b1 58 __ LDA (T5 + 0),y 
09bb : a2 20 __ LDX #$20
09bd : 8e b0 9f STX $9fb0 ; (si.fill + 0)
09c0 : a2 00 __ LDX #$00
09c2 : 8e b1 9f STX $9fb1 ; (si.width + 0)
09c5 : ca __ __ DEX
09c6 : 8e b2 9f STX $9fb2 ; (si.precision + 0)
09c9 : a2 0a __ LDX #$0a
09cb : 8e b4 9f STX $9fb4 ; (si.base + 0)
09ce : aa __ __ TAX
09cf : a9 02 __ LDA #$02
09d1 : d0 07 __ BNE $09da ; (printf.l14 + 0)
.s81:
09d3 : a0 00 __ LDY #$00
09d5 : b1 58 __ LDA (T5 + 0),y 
09d7 : aa __ __ TAX
09d8 : a9 01 __ LDA #$01
.l14:
09da : 18 __ __ CLC
09db : 65 58 __ ADC T5 + 0 
09dd : 85 58 __ STA T5 + 0 
09df : 90 02 __ BCC $09e3 ; (printf.s96 + 0)
.s95:
09e1 : e6 59 __ INC T5 + 1 
.s96:
09e3 : e0 2b __ CPX #$2b
09e5 : d0 07 __ BNE $09ee ; (printf.s15 + 0)
.s84:
09e7 : a9 01 __ LDA #$01
09e9 : 8d b5 9f STA $9fb5 ; (si.sign + 0)
09ec : d0 e5 __ BNE $09d3 ; (printf.s81 + 0)
.s15:
09ee : 8a __ __ TXA
09ef : e0 30 __ CPX #$30
09f1 : d0 05 __ BNE $09f8 ; (printf.s16 + 0)
.s83:
09f3 : 8d b0 9f STA $9fb0 ; (si.fill + 0)
09f6 : f0 db __ BEQ $09d3 ; (printf.s81 + 0)
.s16:
09f8 : c9 23 __ CMP #$23
09fa : d0 07 __ BNE $0a03 ; (printf.s17 + 0)
.s82:
09fc : a9 01 __ LDA #$01
09fe : 8d b7 9f STA $9fb7 ; (si.prefix + 0)
0a01 : d0 d0 __ BNE $09d3 ; (printf.s81 + 0)
.s17:
0a03 : c9 2d __ CMP #$2d
0a05 : d0 07 __ BNE $0a0e ; (printf.s18 + 0)
.s80:
0a07 : a9 01 __ LDA #$01
0a09 : 8d b6 9f STA $9fb6 ; (si.left + 0)
0a0c : d0 c5 __ BNE $09d3 ; (printf.s81 + 0)
.s18:
0a0e : 85 55 __ STA T2 + 0 
0a10 : c9 30 __ CMP #$30
0a12 : 90 3a __ BCC $0a4e ; (printf.s19 + 0)
.s75:
0a14 : e0 3a __ CPX #$3a
0a16 : b0 66 __ BCS $0a7e ; (printf.s20 + 0)
.s76:
0a18 : a9 00 __ LDA #$00
0a1a : e0 3a __ CPX #$3a
0a1c : b0 2b __ BCS $0a49 ; (printf.s78 + 0)
.s111:
0a1e : 85 43 __ STA T0 + 0 
0a20 : a4 58 __ LDY T5 + 0 
0a22 : 85 58 __ STA T5 + 0 
.l79:
0a24 : a5 43 __ LDA T0 + 0 
0a26 : 0a __ __ ASL
0a27 : 0a __ __ ASL
0a28 : 18 __ __ CLC
0a29 : 65 43 __ ADC T0 + 0 
0a2b : 0a __ __ ASL
0a2c : 18 __ __ CLC
0a2d : 65 55 __ ADC T2 + 0 
0a2f : 38 __ __ SEC
0a30 : e9 30 __ SBC #$30
0a32 : 85 43 __ STA T0 + 0 
0a34 : b1 58 __ LDA (T5 + 0),y 
0a36 : 85 55 __ STA T2 + 0 
0a38 : c8 __ __ INY
0a39 : d0 02 __ BNE $0a3d ; (printf.s108 + 0)
.s107:
0a3b : e6 59 __ INC T5 + 1 
.s108:
0a3d : c9 30 __ CMP #$30
0a3f : 90 04 __ BCC $0a45 ; (printf.s112 + 0)
.s77:
0a41 : c9 3a __ CMP #$3a
0a43 : 90 df __ BCC $0a24 ; (printf.l79 + 0)
.s112:
0a45 : 84 58 __ STY T5 + 0 
0a47 : a5 43 __ LDA T0 + 0 
.s78:
0a49 : 8d b1 9f STA $9fb1 ; (si.width + 0)
0a4c : a5 55 __ LDA T2 + 0 
.s19:
0a4e : c9 2e __ CMP #$2e
0a50 : d0 2c __ BNE $0a7e ; (printf.s20 + 0)
.s70:
0a52 : a9 00 __ LDA #$00
0a54 : a8 __ __ TAY
0a55 : f0 0e __ BEQ $0a65 ; (printf.l71 + 0)
.s74:
0a57 : a5 43 __ LDA T0 + 0 
0a59 : 0a __ __ ASL
0a5a : 0a __ __ ASL
0a5b : 18 __ __ CLC
0a5c : 65 43 __ ADC T0 + 0 
0a5e : 0a __ __ ASL
0a5f : 18 __ __ CLC
0a60 : 65 55 __ ADC T2 + 0 
0a62 : 38 __ __ SEC
0a63 : e9 30 __ SBC #$30
.l71:
0a65 : 85 43 __ STA T0 + 0 
0a67 : b1 58 __ LDA (T5 + 0),y 
0a69 : 85 55 __ STA T2 + 0 
0a6b : e6 58 __ INC T5 + 0 
0a6d : d0 02 __ BNE $0a71 ; (printf.s98 + 0)
.s97:
0a6f : e6 59 __ INC T5 + 1 
.s98:
0a71 : c9 30 __ CMP #$30
0a73 : 90 04 __ BCC $0a79 ; (printf.s72 + 0)
.s73:
0a75 : c9 3a __ CMP #$3a
0a77 : 90 de __ BCC $0a57 ; (printf.s74 + 0)
.s72:
0a79 : a6 43 __ LDX T0 + 0 
0a7b : 8e b2 9f STX $9fb2 ; (si.precision + 0)
.s20:
0a7e : c9 64 __ CMP #$64
0a80 : f0 0c __ BEQ $0a8e ; (printf.s69 + 0)
.s21:
0a82 : c9 44 __ CMP #$44
0a84 : f0 08 __ BEQ $0a8e ; (printf.s69 + 0)
.s22:
0a86 : c9 69 __ CMP #$69
0a88 : f0 04 __ BEQ $0a8e ; (printf.s69 + 0)
.s23:
0a8a : c9 49 __ CMP #$49
0a8c : d0 0f __ BNE $0a9d ; (printf.s24 + 0)
.s69:
0a8e : a9 01 __ LDA #$01
.s68:
0a90 : 85 13 __ STA P6 
0a92 : a0 00 __ LDY #$00
0a94 : b1 56 __ LDA (T3 + 0),y 
0a96 : aa __ __ TAX
0a97 : c8 __ __ INY
0a98 : b1 56 __ LDA (T3 + 0),y 
0a9a : 4c 78 0c JMP $0c78 ; (printf.s66 + 0)
.s24:
0a9d : c9 75 __ CMP #$75
0a9f : f0 04 __ BEQ $0aa5 ; (printf.s67 + 0)
.s25:
0aa1 : c9 55 __ CMP #$55
0aa3 : d0 04 __ BNE $0aa9 ; (printf.s26 + 0)
.s67:
0aa5 : a9 00 __ LDA #$00
0aa7 : f0 e7 __ BEQ $0a90 ; (printf.s68 + 0)
.s26:
0aa9 : c9 78 __ CMP #$78
0aab : f0 04 __ BEQ $0ab1 ; (printf.s65 + 0)
.s27:
0aad : c9 58 __ CMP #$58
0aaf : d0 1d __ BNE $0ace ; (printf.s28 + 0)
.s65:
0ab1 : a0 00 __ LDY #$00
0ab3 : 84 13 __ STY P6 
0ab5 : a9 10 __ LDA #$10
0ab7 : 8d b4 9f STA $9fb4 ; (si.base + 0)
0aba : b1 56 __ LDA (T3 + 0),y 
0abc : aa __ __ TAX
0abd : c8 __ __ INY
0abe : b1 56 __ LDA (T3 + 0),y 
0ac0 : a8 __ __ TAY
0ac1 : a5 55 __ LDA T2 + 0 
0ac3 : 29 e0 __ AND #$e0
0ac5 : 09 01 __ ORA #$01
0ac7 : 8d b3 9f STA $9fb3 ; (si.cha + 0)
0aca : 98 __ __ TYA
0acb : 4c 78 0c JMP $0c78 ; (printf.s66 + 0)
.s28:
0ace : c9 6c __ CMP #$6c
0ad0 : d0 03 __ BNE $0ad5 ; (printf.s29 + 0)
0ad2 : 4c fd 0b JMP $0bfd ; (printf.s53 + 0)
.s29:
0ad5 : c9 4c __ CMP #$4c
0ad7 : f0 f9 __ BEQ $0ad2 ; (printf.s28 + 4)
.s30:
0ad9 : c9 66 __ CMP #$66
0adb : f0 14 __ BEQ $0af1 ; (printf.s52 + 0)
.s31:
0add : c9 67 __ CMP #$67
0adf : f0 10 __ BEQ $0af1 ; (printf.s52 + 0)
.s32:
0ae1 : c9 65 __ CMP #$65
0ae3 : f0 0c __ BEQ $0af1 ; (printf.s52 + 0)
.s33:
0ae5 : c9 46 __ CMP #$46
0ae7 : f0 08 __ BEQ $0af1 ; (printf.s52 + 0)
.s34:
0ae9 : c9 47 __ CMP #$47
0aeb : f0 04 __ BEQ $0af1 ; (printf.s52 + 0)
.s35:
0aed : c9 45 __ CMP #$45
0aef : d0 44 __ BNE $0b35 ; (printf.s36 + 0)
.s52:
0af1 : a9 b8 __ LDA #$b8
0af3 : 85 0f __ STA P2 
0af5 : a9 9f __ LDA #$9f
0af7 : 85 10 __ STA P3 
0af9 : a0 00 __ LDY #$00
0afb : b1 56 __ LDA (T3 + 0),y 
0afd : 85 11 __ STA P4 
0aff : c8 __ __ INY
0b00 : b1 56 __ LDA (T3 + 0),y 
0b02 : 85 12 __ STA P5 
0b04 : c8 __ __ INY
0b05 : b1 56 __ LDA (T3 + 0),y 
0b07 : 85 13 __ STA P6 
0b09 : c8 __ __ INY
0b0a : b1 56 __ LDA (T3 + 0),y 
0b0c : 85 14 __ STA P7 
0b0e : a5 55 __ LDA T2 + 0 
0b10 : 29 e0 __ AND #$e0
0b12 : 09 01 __ ORA #$01
0b14 : 8d b3 9f STA $9fb3 ; (si.cha + 0)
0b17 : a9 b0 __ LDA #$b0
0b19 : 85 0d __ STA P0 
0b1b : a9 9f __ LDA #$9f
0b1d : 85 0e __ STA P1 
0b1f : a5 55 __ LDA T2 + 0 
0b21 : ed b3 9f SBC $9fb3 ; (si.cha + 0)
0b24 : 18 __ __ CLC
0b25 : 69 61 __ ADC #$61
0b27 : 85 15 __ STA P8 
0b29 : 20 33 0f JSR $0f33 ; (nformf.s1 + 0)
0b2c : a5 1b __ LDA ACCU + 0 ; (fmt + 2)
0b2e : 85 5b __ STA T7 + 0 
0b30 : a9 04 __ LDA #$04
0b32 : 4c f1 0b JMP $0bf1 ; (printf.s90 + 0)
.s36:
0b35 : c9 73 __ CMP #$73
0b37 : f0 2d __ BEQ $0b66 ; (printf.s44 + 0)
.s37:
0b39 : c9 53 __ CMP #$53
0b3b : f0 29 __ BEQ $0b66 ; (printf.s44 + 0)
.s38:
0b3d : c9 63 __ CMP #$63
0b3f : f0 12 __ BEQ $0b53 ; (printf.s43 + 0)
.s39:
0b41 : c9 43 __ CMP #$43
0b43 : f0 0e __ BEQ $0b53 ; (printf.s43 + 0)
.s40:
0b45 : aa __ __ TAX
0b46 : d0 03 __ BNE $0b4b ; (printf.s41 + 0)
0b48 : 4c 57 09 JMP $0957 ; (printf.l5 + 0)
.s41:
0b4b : 8d b8 9f STA $9fb8 ; (buff[0] + 0)
.s42:
0b4e : a9 01 __ LDA #$01
0b50 : 4c 99 09 JMP $0999 ; (printf.s92 + 0)
.s43:
0b53 : a0 00 __ LDY #$00
0b55 : b1 56 __ LDA (T3 + 0),y 
0b57 : 8d b8 9f STA $9fb8 ; (buff[0] + 0)
0b5a : a5 56 __ LDA T3 + 0 
0b5c : 69 01 __ ADC #$01
0b5e : 85 56 __ STA T3 + 0 
0b60 : 90 ec __ BCC $0b4e ; (printf.s42 + 0)
.s106:
0b62 : e6 57 __ INC T3 + 1 
0b64 : b0 e8 __ BCS $0b4e ; (printf.s42 + 0)
.s44:
0b66 : a0 00 __ LDY #$00
0b68 : 84 5a __ STY T6 + 0 
0b6a : b1 56 __ LDA (T3 + 0),y 
0b6c : 85 53 __ STA T1 + 0 
0b6e : c8 __ __ INY
0b6f : b1 56 __ LDA (T3 + 0),y 
0b71 : 85 54 __ STA T1 + 1 
0b73 : a5 56 __ LDA T3 + 0 
0b75 : 69 01 __ ADC #$01
0b77 : 85 56 __ STA T3 + 0 
0b79 : 90 02 __ BCC $0b7d ; (printf.s105 + 0)
.s104:
0b7b : e6 57 __ INC T3 + 1 
.s105:
0b7d : ad b1 9f LDA $9fb1 ; (si.width + 0)
0b80 : f0 0d __ BEQ $0b8f ; (printf.s45 + 0)
.s93:
0b82 : a0 00 __ LDY #$00
0b84 : b1 53 __ LDA (T1 + 0),y 
0b86 : f0 05 __ BEQ $0b8d ; (printf.s94 + 0)
.l51:
0b88 : c8 __ __ INY
0b89 : b1 53 __ LDA (T1 + 0),y 
0b8b : d0 fb __ BNE $0b88 ; (printf.l51 + 0)
.s94:
0b8d : 84 5a __ STY T6 + 0 
.s45:
0b8f : ad b6 9f LDA $9fb6 ; (si.left + 0)
0b92 : 85 55 __ STA T2 + 0 
0b94 : d0 07 __ BNE $0b9d ; (printf.s47 + 0)
.s46:
0b96 : a4 5a __ LDY T6 + 0 
0b98 : cc b1 9f CPY $9fb1 ; (si.width + 0)
0b9b : 90 2a __ BCC $0bc7 ; (printf.s50 + 0)
.s47:
0b9d : a5 53 __ LDA T1 + 0 
0b9f : 85 0d __ STA P0 
0ba1 : a5 54 __ LDA T1 + 1 
0ba3 : 85 0e __ STA P1 
0ba5 : 20 9e 0c JSR $0c9e ; (puts.s4 + 0)
0ba8 : a5 55 __ LDA T2 + 0 
0baa : f0 9c __ BEQ $0b48 ; (printf.s40 + 3)
.s48:
0bac : a4 5a __ LDY T6 + 0 
0bae : cc b1 9f CPY $9fb1 ; (si.width + 0)
0bb1 : b0 95 __ BCS $0b48 ; (printf.s40 + 3)
.s49:
0bb3 : ad b0 9f LDA $9fb0 ; (si.fill + 0)
0bb6 : a2 00 __ LDX #$00
.l88:
0bb8 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0bbb : e8 __ __ INX
0bbc : c8 __ __ INY
0bbd : cc b1 9f CPY $9fb1 ; (si.width + 0)
0bc0 : 90 f6 __ BCC $0bb8 ; (printf.l88 + 0)
.s86:
0bc2 : 86 5b __ STX T7 + 0 
0bc4 : 4c 57 09 JMP $0957 ; (printf.l5 + 0)
.s50:
0bc7 : ad b0 9f LDA $9fb0 ; (si.fill + 0)
0bca : a2 00 __ LDX #$00
.l89:
0bcc : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0bcf : e8 __ __ INX
0bd0 : c8 __ __ INY
0bd1 : cc b1 9f CPY $9fb1 ; (si.width + 0)
0bd4 : 90 f6 __ BCC $0bcc ; (printf.l89 + 0)
.s87:
0bd6 : a9 b8 __ LDA #$b8
0bd8 : 85 0d __ STA P0 
0bda : a9 9f __ LDA #$9f
0bdc : 85 0e __ STA P1 
0bde : a9 00 __ LDA #$00
0be0 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0be3 : 20 9e 0c JSR $0c9e ; (puts.s4 + 0)
0be6 : a5 53 __ LDA T1 + 0 
0be8 : 85 0d __ STA P0 
0bea : a5 54 __ LDA T1 + 1 
0bec : 85 0e __ STA P1 
0bee : 4c 94 09 JMP $0994 ; (printf.s11 + 0)
.s90:
0bf1 : 18 __ __ CLC
0bf2 : 65 56 __ ADC T3 + 0 
0bf4 : 85 56 __ STA T3 + 0 
0bf6 : 90 cc __ BCC $0bc4 ; (printf.s86 + 2)
.s99:
0bf8 : e6 57 __ INC T3 + 1 
0bfa : 4c 57 09 JMP $0957 ; (printf.l5 + 0)
.s53:
0bfd : a0 00 __ LDY #$00
0bff : b1 56 __ LDA (T3 + 0),y 
0c01 : 85 11 __ STA P4 
0c03 : c8 __ __ INY
0c04 : b1 56 __ LDA (T3 + 0),y 
0c06 : 85 12 __ STA P5 
0c08 : c8 __ __ INY
0c09 : b1 56 __ LDA (T3 + 0),y 
0c0b : 85 13 __ STA P6 
0c0d : c8 __ __ INY
0c0e : b1 56 __ LDA (T3 + 0),y 
0c10 : 85 14 __ STA P7 
0c12 : a5 56 __ LDA T3 + 0 
0c14 : 69 03 __ ADC #$03
0c16 : 85 56 __ STA T3 + 0 
0c18 : 90 02 __ BCC $0c1c ; (printf.s101 + 0)
.s100:
0c1a : e6 57 __ INC T3 + 1 
.s101:
0c1c : a0 00 __ LDY #$00
0c1e : b1 58 __ LDA (T5 + 0),y 
0c20 : aa __ __ TAX
0c21 : e6 58 __ INC T5 + 0 
0c23 : d0 02 __ BNE $0c27 ; (printf.s103 + 0)
.s102:
0c25 : e6 59 __ INC T5 + 1 
.s103:
0c27 : e0 64 __ CPX #$64
0c29 : f0 0c __ BEQ $0c37 ; (printf.s64 + 0)
.s54:
0c2b : e0 44 __ CPX #$44
0c2d : f0 08 __ BEQ $0c37 ; (printf.s64 + 0)
.s55:
0c2f : e0 69 __ CPX #$69
0c31 : f0 04 __ BEQ $0c37 ; (printf.s64 + 0)
.s56:
0c33 : e0 49 __ CPX #$49
0c35 : d0 1c __ BNE $0c53 ; (printf.s57 + 0)
.s64:
0c37 : a9 01 __ LDA #$01
.s91:
0c39 : 85 15 __ STA P8 
.s62:
0c3b : a9 b8 __ LDA #$b8
0c3d : 85 0f __ STA P2 
0c3f : a9 9f __ LDA #$9f
0c41 : 85 0e __ STA P1 
0c43 : a9 9f __ LDA #$9f
0c45 : 85 10 __ STA P3 
0c47 : a9 b0 __ LDA #$b0
0c49 : 85 0d __ STA P0 
0c4b : 20 e8 0d JSR $0de8 ; (nforml.s4 + 0)
0c4e : a5 1b __ LDA ACCU + 0 ; (fmt + 2)
0c50 : 4c 99 09 JMP $0999 ; (printf.s92 + 0)
.s57:
0c53 : e0 75 __ CPX #$75
0c55 : f0 04 __ BEQ $0c5b ; (printf.s63 + 0)
.s58:
0c57 : e0 55 __ CPX #$55
0c59 : d0 03 __ BNE $0c5e ; (printf.s59 + 0)
.s63:
0c5b : 98 __ __ TYA
0c5c : f0 db __ BEQ $0c39 ; (printf.s91 + 0)
.s59:
0c5e : e0 78 __ CPX #$78
0c60 : f0 04 __ BEQ $0c66 ; (printf.s61 + 0)
.s60:
0c62 : e0 58 __ CPX #$58
0c64 : d0 94 __ BNE $0bfa ; (printf.s99 + 2)
.s61:
0c66 : 84 15 __ STY P8 
0c68 : a9 10 __ LDA #$10
0c6a : 8d b4 9f STA $9fb4 ; (si.base + 0)
0c6d : 8a __ __ TXA
0c6e : 29 e0 __ AND #$e0
0c70 : 09 01 __ ORA #$01
0c72 : 8d b3 9f STA $9fb3 ; (si.cha + 0)
0c75 : 4c 3b 0c JMP $0c3b ; (printf.s62 + 0)
.s66:
0c78 : 86 11 __ STX P4 
0c7a : 85 12 __ STA P5 
0c7c : a9 b8 __ LDA #$b8
0c7e : 85 0f __ STA P2 
0c80 : a9 9f __ LDA #$9f
0c82 : 85 0e __ STA P1 
0c84 : a9 9f __ LDA #$9f
0c86 : 85 10 __ STA P3 
0c88 : a9 b0 __ LDA #$b0
0c8a : 85 0d __ STA P0 
0c8c : 20 d3 0c JSR $0cd3 ; (nformi.s4 + 0)
0c8f : 85 5b __ STA T7 + 0 
0c91 : a9 02 __ LDA #$02
0c93 : 4c f1 0b JMP $0bf1 ; (printf.s90 + 0)
--------------------------------------------------------------------
puts@proxy: ; puts@proxy
0c96 : a9 b8 __ LDA #$b8
0c98 : 85 0d __ STA P0 
0c9a : a9 9f __ LDA #$9f
0c9c : 85 0e __ STA P1 
--------------------------------------------------------------------
puts: ; puts(const u8*)->void
;  12, "C:/oscar64/include/stdio.h"
.s4:
0c9e : a0 00 __ LDY #$00
.l5:
0ca0 : b1 0d __ LDA (P0),y ; (str + 0)
0ca2 : aa __ __ TAX
0ca3 : 18 __ __ CLC
0ca4 : a5 0d __ LDA P0 ; (str + 0)
0ca6 : 69 01 __ ADC #$01
0ca8 : 85 0d __ STA P0 ; (str + 0)
0caa : 8a __ __ TXA
0cab : d0 01 __ BNE $0cae ; (puts.s6 + 0)
.s3:
0cad : 60 __ __ RTS
.s6:
0cae : 90 02 __ BCC $0cb2 ; (puts.s13 + 0)
.s12:
0cb0 : e6 0e __ INC P1 ; (str + 1)
.s13:
0cb2 : c9 0a __ CMP #$0a
0cb4 : d0 08 __ BNE $0cbe ; (puts.s7 + 0)
.s10:
0cb6 : a9 0d __ LDA #$0d
.s11:
0cb8 : 20 d2 ff JSR $ffd2 
0cbb : 4c a0 0c JMP $0ca0 ; (puts.l5 + 0)
.s7:
0cbe : c9 09 __ CMP #$09
0cc0 : d0 f6 __ BNE $0cb8 ; (puts.s11 + 0)
.s8:
0cc2 : a5 d3 __ LDA $d3 
0cc4 : 29 03 __ AND #$03
0cc6 : aa __ __ TAX
.l9:
0cc7 : a9 20 __ LDA #$20
0cc9 : 20 d2 ff JSR $ffd2 
0ccc : e8 __ __ INX
0ccd : e0 04 __ CPX #$04
0ccf : 90 f6 __ BCC $0cc7 ; (puts.l9 + 0)
0cd1 : b0 cd __ BCS $0ca0 ; (puts.l5 + 0)
--------------------------------------------------------------------
nformi: ; nformi(const struct sinfo*,u8*,i16,bool)->u8
;  79, "C:/oscar64/include/stdio.c"
.s4:
0cd3 : a9 00 __ LDA #$00
0cd5 : 85 43 __ STA T5 + 0 
0cd7 : a0 04 __ LDY #$04
0cd9 : b1 0d __ LDA (P0),y ; (si + 0)
0cdb : 85 44 __ STA T6 + 0 
0cdd : a5 13 __ LDA P6 ; (s + 0)
0cdf : f0 13 __ BEQ $0cf4 ; (nformi.s5 + 0)
.s34:
0ce1 : 24 12 __ BIT P5 ; (v + 1)
0ce3 : 10 0f __ BPL $0cf4 ; (nformi.s5 + 0)
.s35:
0ce5 : 38 __ __ SEC
0ce6 : a9 00 __ LDA #$00
0ce8 : e5 11 __ SBC P4 ; (v + 0)
0cea : 85 11 __ STA P4 ; (v + 0)
0cec : a9 00 __ LDA #$00
0cee : e5 12 __ SBC P5 ; (v + 1)
0cf0 : 85 12 __ STA P5 ; (v + 1)
0cf2 : e6 43 __ INC T5 + 0 
.s5:
0cf4 : a9 10 __ LDA #$10
0cf6 : 85 45 __ STA T7 + 0 
0cf8 : a5 11 __ LDA P4 ; (v + 0)
0cfa : 05 12 __ ORA P5 ; (v + 1)
0cfc : f0 2d __ BEQ $0d2b ; (nformi.s6 + 0)
.s29:
0cfe : a5 11 __ LDA P4 ; (v + 0)
0d00 : 85 1b __ STA ACCU + 0 
0d02 : a5 12 __ LDA P5 ; (v + 1)
0d04 : 85 1c __ STA ACCU + 1 
.l30:
0d06 : a5 44 __ LDA T6 + 0 
0d08 : 20 b0 17 JSR $17b0 ; (divmod + 53)
0d0b : a5 05 __ LDA WORK + 2 
0d0d : c9 0a __ CMP #$0a
0d0f : b0 04 __ BCS $0d15 ; (nformi.s33 + 0)
.s31:
0d11 : a9 30 __ LDA #$30
0d13 : 90 06 __ BCC $0d1b ; (nformi.s32 + 0)
.s33:
0d15 : a0 03 __ LDY #$03
0d17 : b1 0d __ LDA (P0),y ; (si + 0)
0d19 : e9 0a __ SBC #$0a
.s32:
0d1b : 18 __ __ CLC
0d1c : 65 05 __ ADC WORK + 2 
0d1e : a6 45 __ LDX T7 + 0 
0d20 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d23 : c6 45 __ DEC T7 + 0 
0d25 : a5 1b __ LDA ACCU + 0 
0d27 : 05 1c __ ORA ACCU + 1 
0d29 : d0 db __ BNE $0d06 ; (nformi.l30 + 0)
.s6:
0d2b : a9 ff __ LDA #$ff
0d2d : a0 02 __ LDY #$02
0d2f : d1 0d __ CMP (P0),y ; (si + 0)
0d31 : d0 04 __ BNE $0d37 ; (nformi.s28 + 0)
.s7:
0d33 : a9 0f __ LDA #$0f
0d35 : d0 05 __ BNE $0d3c ; (nformi.s40 + 0)
.s28:
0d37 : 38 __ __ SEC
0d38 : a9 10 __ LDA #$10
0d3a : f1 0d __ SBC (P0),y ; (si + 0)
.s40:
0d3c : a8 __ __ TAY
0d3d : c4 45 __ CPY T7 + 0 
0d3f : b0 0d __ BCS $0d4e ; (nformi.s8 + 0)
.s27:
0d41 : a9 30 __ LDA #$30
.l41:
0d43 : a6 45 __ LDX T7 + 0 
0d45 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d48 : c6 45 __ DEC T7 + 0 
0d4a : c4 45 __ CPY T7 + 0 
0d4c : 90 f5 __ BCC $0d43 ; (nformi.l41 + 0)
.s8:
0d4e : a0 07 __ LDY #$07
0d50 : b1 0d __ LDA (P0),y ; (si + 0)
0d52 : f0 1c __ BEQ $0d70 ; (nformi.s9 + 0)
.s25:
0d54 : a5 44 __ LDA T6 + 0 
0d56 : c9 10 __ CMP #$10
0d58 : d0 16 __ BNE $0d70 ; (nformi.s9 + 0)
.s26:
0d5a : a0 03 __ LDY #$03
0d5c : b1 0d __ LDA (P0),y ; (si + 0)
0d5e : a8 __ __ TAY
0d5f : a9 30 __ LDA #$30
0d61 : a6 45 __ LDX T7 + 0 
0d63 : 9d e8 9f STA $9fe8,x ; (buff[0] + 48)
0d66 : 98 __ __ TYA
0d67 : 69 16 __ ADC #$16
0d69 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d6c : ca __ __ DEX
0d6d : ca __ __ DEX
0d6e : 86 45 __ STX T7 + 0 
.s9:
0d70 : a9 00 __ LDA #$00
0d72 : 85 1b __ STA ACCU + 0 
0d74 : a5 43 __ LDA T5 + 0 
0d76 : f0 0c __ BEQ $0d84 ; (nformi.s10 + 0)
.s24:
0d78 : a9 2d __ LDA #$2d
.s23:
0d7a : a6 45 __ LDX T7 + 0 
0d7c : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d7f : c6 45 __ DEC T7 + 0 
0d81 : 4c 8e 0d JMP $0d8e ; (nformi.s11 + 0)
.s10:
0d84 : a0 05 __ LDY #$05
0d86 : b1 0d __ LDA (P0),y ; (si + 0)
0d88 : f0 04 __ BEQ $0d8e ; (nformi.s11 + 0)
.s22:
0d8a : a9 2b __ LDA #$2b
0d8c : d0 ec __ BNE $0d7a ; (nformi.s23 + 0)
.s11:
0d8e : a6 45 __ LDX T7 + 0 
0d90 : a0 06 __ LDY #$06
0d92 : b1 0d __ LDA (P0),y ; (si + 0)
0d94 : d0 2b __ BNE $0dc1 ; (nformi.s18 + 0)
.l12:
0d96 : 8a __ __ TXA
0d97 : 18 __ __ CLC
0d98 : a0 01 __ LDY #$01
0d9a : 71 0d __ ADC (P0),y ; (si + 0)
0d9c : b0 04 __ BCS $0da2 ; (nformi.s16 + 0)
.s17:
0d9e : c9 11 __ CMP #$11
0da0 : 90 0a __ BCC $0dac ; (nformi.s13 + 0)
.s16:
0da2 : a0 00 __ LDY #$00
0da4 : b1 0d __ LDA (P0),y ; (si + 0)
0da6 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0da9 : ca __ __ DEX
0daa : b0 ea __ BCS $0d96 ; (nformi.l12 + 0)
.s13:
0dac : e0 10 __ CPX #$10
0dae : b0 0e __ BCS $0dbe ; (nformi.s14 + 0)
.s15:
0db0 : 88 __ __ DEY
.l38:
0db1 : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0db4 : 91 0f __ STA (P2),y ; (str + 0)
0db6 : c8 __ __ INY
0db7 : e8 __ __ INX
0db8 : e0 10 __ CPX #$10
0dba : 90 f5 __ BCC $0db1 ; (nformi.l38 + 0)
.s39:
0dbc : 84 1b __ STY ACCU + 0 
.s14:
0dbe : a5 1b __ LDA ACCU + 0 
.s3:
0dc0 : 60 __ __ RTS
.s18:
0dc1 : e0 10 __ CPX #$10
0dc3 : b0 1a __ BCS $0ddf ; (nformi.l19 + 0)
.s21:
0dc5 : a0 00 __ LDY #$00
.l36:
0dc7 : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0dca : 91 0f __ STA (P2),y ; (str + 0)
0dcc : c8 __ __ INY
0dcd : e8 __ __ INX
0dce : e0 10 __ CPX #$10
0dd0 : 90 f5 __ BCC $0dc7 ; (nformi.l36 + 0)
.s37:
0dd2 : 84 1b __ STY ACCU + 0 
0dd4 : b0 09 __ BCS $0ddf ; (nformi.l19 + 0)
.s20:
0dd6 : 88 __ __ DEY
0dd7 : b1 0d __ LDA (P0),y ; (si + 0)
0dd9 : a4 1b __ LDY ACCU + 0 
0ddb : 91 0f __ STA (P2),y ; (str + 0)
0ddd : e6 1b __ INC ACCU + 0 
.l19:
0ddf : a5 1b __ LDA ACCU + 0 
0de1 : a0 01 __ LDY #$01
0de3 : d1 0d __ CMP (P0),y ; (si + 0)
0de5 : 90 ef __ BCC $0dd6 ; (nformi.s20 + 0)
0de7 : 60 __ __ RTS
--------------------------------------------------------------------
nforml: ; nforml(const struct sinfo*,u8*,i32,bool)->u8
; 137, "C:/oscar64/include/stdio.c"
.s4:
0de8 : a9 00 __ LDA #$00
0dea : 85 43 __ STA T4 + 0 
0dec : a5 15 __ LDA P8 ; (s + 0)
0dee : f0 1f __ BEQ $0e0f ; (nforml.s5 + 0)
.s35:
0df0 : 24 14 __ BIT P7 ; (v + 3)
0df2 : 10 1b __ BPL $0e0f ; (nforml.s5 + 0)
.s36:
0df4 : 38 __ __ SEC
0df5 : a9 00 __ LDA #$00
0df7 : e5 11 __ SBC P4 ; (v + 0)
0df9 : 85 11 __ STA P4 ; (v + 0)
0dfb : a9 00 __ LDA #$00
0dfd : e5 12 __ SBC P5 ; (v + 1)
0dff : 85 12 __ STA P5 ; (v + 1)
0e01 : a9 00 __ LDA #$00
0e03 : e5 13 __ SBC P6 ; (v + 2)
0e05 : 85 13 __ STA P6 ; (v + 2)
0e07 : a9 00 __ LDA #$00
0e09 : e5 14 __ SBC P7 ; (v + 3)
0e0b : 85 14 __ STA P7 ; (v + 3)
0e0d : e6 43 __ INC T4 + 0 
.s5:
0e0f : a9 10 __ LDA #$10
0e11 : 85 44 __ STA T5 + 0 
0e13 : a5 14 __ LDA P7 ; (v + 3)
0e15 : d0 0c __ BNE $0e23 ; (nforml.l43 + 0)
.s31:
0e17 : a5 13 __ LDA P6 ; (v + 2)
0e19 : d0 08 __ BNE $0e23 ; (nforml.l43 + 0)
.s32:
0e1b : a5 12 __ LDA P5 ; (v + 1)
0e1d : d0 04 __ BNE $0e23 ; (nforml.l43 + 0)
.s33:
0e1f : c5 11 __ CMP P4 ; (v + 0)
0e21 : b0 13 __ BCS $0e36 ; (nforml.s6 + 0)
.l43:
0e23 : a5 11 __ LDA P4 ; (v + 0)
0e25 : 85 1b __ STA ACCU + 0 
0e27 : a5 12 __ LDA P5 ; (v + 1)
0e29 : 85 1c __ STA ACCU + 1 
0e2b : a5 13 __ LDA P6 ; (v + 2)
0e2d : 85 1d __ STA ACCU + 2 
0e2f : a5 14 __ LDA P7 ; (v + 3)
0e31 : 85 1e __ STA ACCU + 3 
0e33 : 4c f2 0e JMP $0ef2 ; (nforml.l28 + 0)
.s6:
0e36 : a9 ff __ LDA #$ff
0e38 : a0 02 __ LDY #$02
0e3a : d1 0d __ CMP (P0),y ; (si + 0)
0e3c : d0 04 __ BNE $0e42 ; (nforml.s27 + 0)
.s7:
0e3e : a9 0f __ LDA #$0f
0e40 : d0 05 __ BNE $0e47 ; (nforml.s41 + 0)
.s27:
0e42 : 38 __ __ SEC
0e43 : a9 10 __ LDA #$10
0e45 : f1 0d __ SBC (P0),y ; (si + 0)
.s41:
0e47 : a8 __ __ TAY
0e48 : c4 44 __ CPY T5 + 0 
0e4a : b0 0d __ BCS $0e59 ; (nforml.s8 + 0)
.s26:
0e4c : a9 30 __ LDA #$30
.l42:
0e4e : a6 44 __ LDX T5 + 0 
0e50 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e53 : c6 44 __ DEC T5 + 0 
0e55 : c4 44 __ CPY T5 + 0 
0e57 : 90 f5 __ BCC $0e4e ; (nforml.l42 + 0)
.s8:
0e59 : a0 07 __ LDY #$07
0e5b : b1 0d __ LDA (P0),y ; (si + 0)
0e5d : f0 1d __ BEQ $0e7c ; (nforml.s9 + 0)
.s24:
0e5f : a9 10 __ LDA #$10
0e61 : a0 04 __ LDY #$04
0e63 : d1 0d __ CMP (P0),y ; (si + 0)
0e65 : d0 15 __ BNE $0e7c ; (nforml.s9 + 0)
.s25:
0e67 : 88 __ __ DEY
0e68 : b1 0d __ LDA (P0),y ; (si + 0)
0e6a : a8 __ __ TAY
0e6b : a9 30 __ LDA #$30
0e6d : a6 44 __ LDX T5 + 0 
0e6f : 9d e8 9f STA $9fe8,x ; (buff[0] + 48)
0e72 : 98 __ __ TYA
0e73 : 69 16 __ ADC #$16
0e75 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e78 : ca __ __ DEX
0e79 : ca __ __ DEX
0e7a : 86 44 __ STX T5 + 0 
.s9:
0e7c : a9 00 __ LDA #$00
0e7e : 85 1b __ STA ACCU + 0 
0e80 : a5 43 __ LDA T4 + 0 
0e82 : f0 0c __ BEQ $0e90 ; (nforml.s10 + 0)
.s23:
0e84 : a9 2d __ LDA #$2d
.s22:
0e86 : a6 44 __ LDX T5 + 0 
0e88 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e8b : c6 44 __ DEC T5 + 0 
0e8d : 4c 9a 0e JMP $0e9a ; (nforml.s11 + 0)
.s10:
0e90 : a0 05 __ LDY #$05
0e92 : b1 0d __ LDA (P0),y ; (si + 0)
0e94 : f0 04 __ BEQ $0e9a ; (nforml.s11 + 0)
.s21:
0e96 : a9 2b __ LDA #$2b
0e98 : d0 ec __ BNE $0e86 ; (nforml.s22 + 0)
.s11:
0e9a : a0 06 __ LDY #$06
0e9c : a6 44 __ LDX T5 + 0 
0e9e : b1 0d __ LDA (P0),y ; (si + 0)
0ea0 : d0 29 __ BNE $0ecb ; (nforml.s17 + 0)
.l12:
0ea2 : 8a __ __ TXA
0ea3 : 18 __ __ CLC
0ea4 : a0 01 __ LDY #$01
0ea6 : 71 0d __ ADC (P0),y ; (si + 0)
0ea8 : b0 04 __ BCS $0eae ; (nforml.s15 + 0)
.s16:
0eaa : c9 11 __ CMP #$11
0eac : 90 0a __ BCC $0eb8 ; (nforml.s13 + 0)
.s15:
0eae : a0 00 __ LDY #$00
0eb0 : b1 0d __ LDA (P0),y ; (si + 0)
0eb2 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0eb5 : ca __ __ DEX
0eb6 : b0 ea __ BCS $0ea2 ; (nforml.l12 + 0)
.s13:
0eb8 : e0 10 __ CPX #$10
0eba : b0 0e __ BCS $0eca ; (nforml.s3 + 0)
.s14:
0ebc : 88 __ __ DEY
.l39:
0ebd : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0ec0 : 91 0f __ STA (P2),y ; (str + 0)
0ec2 : c8 __ __ INY
0ec3 : e8 __ __ INX
0ec4 : e0 10 __ CPX #$10
0ec6 : 90 f5 __ BCC $0ebd ; (nforml.l39 + 0)
.s40:
0ec8 : 84 1b __ STY ACCU + 0 
.s3:
0eca : 60 __ __ RTS
.s17:
0ecb : e0 10 __ CPX #$10
0ecd : b0 1a __ BCS $0ee9 ; (nforml.l18 + 0)
.s20:
0ecf : a0 00 __ LDY #$00
.l37:
0ed1 : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0ed4 : 91 0f __ STA (P2),y ; (str + 0)
0ed6 : c8 __ __ INY
0ed7 : e8 __ __ INX
0ed8 : e0 10 __ CPX #$10
0eda : 90 f5 __ BCC $0ed1 ; (nforml.l37 + 0)
.s38:
0edc : 84 1b __ STY ACCU + 0 
0ede : b0 09 __ BCS $0ee9 ; (nforml.l18 + 0)
.s19:
0ee0 : 88 __ __ DEY
0ee1 : b1 0d __ LDA (P0),y ; (si + 0)
0ee3 : a4 1b __ LDY ACCU + 0 
0ee5 : 91 0f __ STA (P2),y ; (str + 0)
0ee7 : e6 1b __ INC ACCU + 0 
.l18:
0ee9 : a5 1b __ LDA ACCU + 0 
0eeb : a0 01 __ LDY #$01
0eed : d1 0d __ CMP (P0),y ; (si + 0)
0eef : 90 ef __ BCC $0ee0 ; (nforml.s19 + 0)
0ef1 : 60 __ __ RTS
.l28:
0ef2 : a0 04 __ LDY #$04
0ef4 : b1 0d __ LDA (P0),y ; (si + 0)
0ef6 : 85 03 __ STA WORK + 0 
0ef8 : a9 00 __ LDA #$00
0efa : 85 04 __ STA WORK + 1 
0efc : 85 05 __ STA WORK + 2 
0efe : 85 06 __ STA WORK + 3 
0f00 : 20 0e 19 JSR $190e ; (divmod32 + 0)
0f03 : a5 07 __ LDA WORK + 4 
0f05 : c9 0a __ CMP #$0a
0f07 : b0 04 __ BCS $0f0d ; (nforml.s34 + 0)
.s29:
0f09 : a9 30 __ LDA #$30
0f0b : 90 06 __ BCC $0f13 ; (nforml.s30 + 0)
.s34:
0f0d : a0 03 __ LDY #$03
0f0f : b1 0d __ LDA (P0),y ; (si + 0)
0f11 : e9 0a __ SBC #$0a
.s30:
0f13 : 18 __ __ CLC
0f14 : 65 07 __ ADC WORK + 4 
0f16 : a6 44 __ LDX T5 + 0 
0f18 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0f1b : a5 1b __ LDA ACCU + 0 
0f1d : 85 11 __ STA P4 ; (v + 0)
0f1f : a5 1c __ LDA ACCU + 1 
0f21 : 85 12 __ STA P5 ; (v + 1)
0f23 : a5 1d __ LDA ACCU + 2 
0f25 : 85 13 __ STA P6 ; (v + 2)
0f27 : a5 1e __ LDA ACCU + 3 
0f29 : 85 14 __ STA P7 ; (v + 3)
0f2b : c6 44 __ DEC T5 + 0 
0f2d : aa __ __ TAX
0f2e : d0 c2 __ BNE $0ef2 ; (nforml.l28 + 0)
0f30 : 4c 17 0e JMP $0e17 ; (nforml.s31 + 0)
--------------------------------------------------------------------
nformf: ; nformf(const struct sinfo*,u8*,float,u8)->u8
; 199, "C:/oscar64/include/stdio.c"
.s1:
0f33 : a5 53 __ LDA T10 + 0 
0f35 : 8d f1 9f STA $9ff1 ; (nformf@stack + 0)
0f38 : a5 54 __ LDA T11 + 0 
0f3a : 8d f2 9f STA $9ff2 ; (nformf@stack + 1)
.s4:
0f3d : a5 11 __ LDA P4 ; (f + 0)
0f3f : 85 43 __ STA T0 + 0 
0f41 : a5 12 __ LDA P5 ; (f + 1)
0f43 : 85 44 __ STA T0 + 1 
0f45 : a5 14 __ LDA P7 ; (f + 3)
0f47 : 29 7f __ AND #$7f
0f49 : 05 13 __ ORA P6 ; (f + 2)
0f4b : 05 12 __ ORA P5 ; (f + 1)
0f4d : a6 13 __ LDX P6 ; (f + 2)
0f4f : 86 45 __ STX T0 + 2 
0f51 : 05 11 __ ORA P4 ; (f + 0)
0f53 : f0 14 __ BEQ $0f69 ; (nformf.s5 + 0)
.s89:
0f55 : 24 14 __ BIT P7 ; (f + 3)
0f57 : 10 10 __ BPL $0f69 ; (nformf.s5 + 0)
.s88:
0f59 : a9 2d __ LDA #$2d
0f5b : a0 00 __ LDY #$00
0f5d : 91 0f __ STA (P2),y ; (str + 0)
0f5f : a5 14 __ LDA P7 ; (f + 3)
0f61 : 49 80 __ EOR #$80
0f63 : 85 14 __ STA P7 ; (f + 3)
.s87:
0f65 : a9 01 __ LDA #$01
0f67 : d0 0e __ BNE $0f77 ; (nformf.s6 + 0)
.s5:
0f69 : a0 05 __ LDY #$05
0f6b : b1 0d __ LDA (P0),y ; (si + 0)
0f6d : f0 08 __ BEQ $0f77 ; (nformf.s6 + 0)
.s86:
0f6f : a9 2b __ LDA #$2b
0f71 : a0 00 __ LDY #$00
0f73 : 91 0f __ STA (P2),y ; (str + 0)
0f75 : a9 01 __ LDA #$01
.s6:
0f77 : 85 52 __ STA T9 + 0 
0f79 : 8a __ __ TXA
0f7a : 0a __ __ ASL
0f7b : a5 14 __ LDA P7 ; (f + 3)
0f7d : 2a __ __ ROL
0f7e : c9 ff __ CMP #$ff
0f80 : d0 29 __ BNE $0fab ; (nformf.s7 + 0)
.s85:
0f82 : a0 03 __ LDY #$03
0f84 : b1 0d __ LDA (P0),y ; (si + 0)
0f86 : 69 07 __ ADC #$07
0f88 : a4 52 __ LDY T9 + 0 
0f8a : 91 0f __ STA (P2),y ; (str + 0)
0f8c : 18 __ __ CLC
0f8d : a0 03 __ LDY #$03
0f8f : b1 0d __ LDA (P0),y ; (si + 0)
0f91 : 69 0d __ ADC #$0d
0f93 : a4 52 __ LDY T9 + 0 
0f95 : c8 __ __ INY
0f96 : 91 0f __ STA (P2),y ; (str + 0)
0f98 : 18 __ __ CLC
0f99 : a0 03 __ LDY #$03
0f9b : b1 0d __ LDA (P0),y ; (si + 0)
0f9d : 69 05 __ ADC #$05
0f9f : a4 52 __ LDY T9 + 0 
0fa1 : c8 __ __ INY
0fa2 : c8 __ __ INY
0fa3 : 91 0f __ STA (P2),y ; (str + 0)
0fa5 : c8 __ __ INY
0fa6 : 84 52 __ STY T9 + 0 
0fa8 : 4c 7d 12 JMP $127d ; (nformf.s27 + 0)
.s7:
0fab : a0 02 __ LDY #$02
0fad : b1 0d __ LDA (P0),y ; (si + 0)
0faf : a6 14 __ LDX P7 ; (f + 3)
0fb1 : 86 46 __ STX T0 + 3 
0fb3 : c9 ff __ CMP #$ff
0fb5 : d0 02 __ BNE $0fb9 ; (nformf.s84 + 0)
.s8:
0fb7 : a9 06 __ LDA #$06
.s84:
0fb9 : 85 50 __ STA T7 + 0 
0fbb : a9 00 __ LDA #$00
0fbd : 85 4d __ STA T5 + 0 
0fbf : 85 4e __ STA T5 + 1 
0fc1 : 8a __ __ TXA
0fc2 : 29 7f __ AND #$7f
0fc4 : 05 13 __ ORA P6 ; (f + 2)
0fc6 : 05 12 __ ORA P5 ; (f + 1)
0fc8 : 05 11 __ ORA P4 ; (f + 0)
0fca : d0 03 __ BNE $0fcf ; (nformf.s63 + 0)
0fcc : 4c a1 10 JMP $10a1 ; (nformf.s9 + 0)
.s63:
0fcf : 8a __ __ TXA
0fd0 : 30 51 __ BMI $1023 ; (nformf.l72 + 0)
.s81:
0fd2 : c9 44 __ CMP #$44
0fd4 : d0 06 __ BNE $0fdc ; (nformf.l83 + 0)
.s82:
0fd6 : a5 13 __ LDA P6 ; (f + 2)
0fd8 : c9 7a __ CMP #$7a
0fda : f0 02 __ BEQ $0fde ; (nformf.l78 + 0)
.l83:
0fdc : 90 35 __ BCC $1013 ; (nformf.s64 + 0)
.l78:
0fde : 18 __ __ CLC
0fdf : a5 4d __ LDA T5 + 0 
0fe1 : 69 03 __ ADC #$03
0fe3 : 85 4d __ STA T5 + 0 
0fe5 : 90 02 __ BCC $0fe9 ; (nformf.s103 + 0)
.s102:
0fe7 : e6 4e __ INC T5 + 1 
.s103:
0fe9 : a9 7a __ LDA #$7a
0feb : 85 05 __ STA WORK + 2 
0fed : a9 44 __ LDA #$44
0fef : 85 06 __ STA WORK + 3 
0ff1 : 20 08 1a JSR $1a08 ; (freg@proxy + 0)
0ff4 : 20 96 16 JSR $1696 ; (crt_fdiv + 0)
0ff7 : a5 1b __ LDA ACCU + 0 
0ff9 : 85 43 __ STA T0 + 0 
0ffb : a5 1c __ LDA ACCU + 1 
0ffd : 85 44 __ STA T0 + 1 
0fff : a6 1d __ LDX ACCU + 2 
1001 : 86 45 __ STX T0 + 2 
1003 : a5 1e __ LDA ACCU + 3 
1005 : 85 46 __ STA T0 + 3 
1007 : 30 0a __ BMI $1013 ; (nformf.s64 + 0)
.s79:
1009 : c9 44 __ CMP #$44
100b : d0 cf __ BNE $0fdc ; (nformf.l83 + 0)
.s80:
100d : e0 7a __ CPX #$7a
100f : f0 cd __ BEQ $0fde ; (nformf.l78 + 0)
1011 : d0 c9 __ BNE $0fdc ; (nformf.l83 + 0)
.s64:
1013 : a5 46 __ LDA T0 + 3 
1015 : 30 0c __ BMI $1023 ; (nformf.l72 + 0)
.s76:
1017 : c9 3f __ CMP #$3f
1019 : d0 06 __ BNE $1021 ; (nformf.s75 + 0)
.s77:
101b : a5 45 __ LDA T0 + 2 
101d : c9 80 __ CMP #$80
101f : f0 40 __ BEQ $1061 ; (nformf.s65 + 0)
.s75:
1021 : b0 3e __ BCS $1061 ; (nformf.s65 + 0)
.l72:
1023 : 38 __ __ SEC
1024 : a5 4d __ LDA T5 + 0 
1026 : e9 03 __ SBC #$03
1028 : 85 4d __ STA T5 + 0 
102a : b0 02 __ BCS $102e ; (nformf.s98 + 0)
.s97:
102c : c6 4e __ DEC T5 + 1 
.s98:
102e : a9 00 __ LDA #$00
1030 : 85 1b __ STA ACCU + 0 
1032 : 85 1c __ STA ACCU + 1 
1034 : a9 7a __ LDA #$7a
1036 : 85 1d __ STA ACCU + 2 
1038 : a9 44 __ LDA #$44
103a : 85 1e __ STA ACCU + 3 
103c : a2 43 __ LDX #$43
103e : 20 a0 14 JSR $14a0 ; (freg + 4)
1041 : 20 ce 15 JSR $15ce ; (crt_fmul + 0)
1044 : a5 1b __ LDA ACCU + 0 
1046 : 85 43 __ STA T0 + 0 
1048 : a5 1c __ LDA ACCU + 1 
104a : 85 44 __ STA T0 + 1 
104c : a6 1d __ LDX ACCU + 2 
104e : 86 45 __ STX T0 + 2 
1050 : a5 1e __ LDA ACCU + 3 
1052 : 85 46 __ STA T0 + 3 
1054 : 30 cd __ BMI $1023 ; (nformf.l72 + 0)
.s73:
1056 : c9 3f __ CMP #$3f
1058 : 90 c9 __ BCC $1023 ; (nformf.l72 + 0)
.s106:
105a : d0 05 __ BNE $1061 ; (nformf.s65 + 0)
.s74:
105c : e0 80 __ CPX #$80
105e : 4c 21 10 JMP $1021 ; (nformf.s75 + 0)
.s65:
1061 : a5 46 __ LDA T0 + 3 
1063 : 30 3c __ BMI $10a1 ; (nformf.s9 + 0)
.s69:
1065 : c9 41 __ CMP #$41
1067 : d0 06 __ BNE $106f ; (nformf.l71 + 0)
.s70:
1069 : a5 45 __ LDA T0 + 2 
106b : c9 20 __ CMP #$20
106d : f0 02 __ BEQ $1071 ; (nformf.l66 + 0)
.l71:
106f : 90 30 __ BCC $10a1 ; (nformf.s9 + 0)
.l66:
1071 : e6 4d __ INC T5 + 0 
1073 : d0 02 __ BNE $1077 ; (nformf.s101 + 0)
.s100:
1075 : e6 4e __ INC T5 + 1 
.s101:
1077 : a9 20 __ LDA #$20
1079 : 85 05 __ STA WORK + 2 
107b : a9 41 __ LDA #$41
107d : 85 06 __ STA WORK + 3 
107f : 20 08 1a JSR $1a08 ; (freg@proxy + 0)
1082 : 20 96 16 JSR $1696 ; (crt_fdiv + 0)
1085 : a5 1b __ LDA ACCU + 0 
1087 : 85 43 __ STA T0 + 0 
1089 : a5 1c __ LDA ACCU + 1 
108b : 85 44 __ STA T0 + 1 
108d : a6 1d __ LDX ACCU + 2 
108f : 86 45 __ STX T0 + 2 
1091 : a5 1e __ LDA ACCU + 3 
1093 : 85 46 __ STA T0 + 3 
1095 : 30 0a __ BMI $10a1 ; (nformf.s9 + 0)
.s67:
1097 : c9 41 __ CMP #$41
1099 : d0 d4 __ BNE $106f ; (nformf.l71 + 0)
.s68:
109b : e0 20 __ CPX #$20
109d : f0 d2 __ BEQ $1071 ; (nformf.l66 + 0)
109f : d0 ce __ BNE $106f ; (nformf.l71 + 0)
.s9:
10a1 : a5 15 __ LDA P8 ; (type + 0)
10a3 : c9 65 __ CMP #$65
10a5 : d0 04 __ BNE $10ab ; (nformf.s11 + 0)
.s10:
10a7 : a9 01 __ LDA #$01
10a9 : d0 02 __ BNE $10ad ; (nformf.s12 + 0)
.s11:
10ab : a9 00 __ LDA #$00
.s12:
10ad : 85 53 __ STA T10 + 0 
10af : a6 50 __ LDX T7 + 0 
10b1 : e8 __ __ INX
10b2 : 86 4f __ STX T6 + 0 
10b4 : a5 15 __ LDA P8 ; (type + 0)
10b6 : c9 67 __ CMP #$67
10b8 : d0 13 __ BNE $10cd ; (nformf.s13 + 0)
.s59:
10ba : a5 4e __ LDA T5 + 1 
10bc : 30 08 __ BMI $10c6 ; (nformf.s60 + 0)
.s62:
10be : d0 06 __ BNE $10c6 ; (nformf.s60 + 0)
.s61:
10c0 : a5 4d __ LDA T5 + 0 
10c2 : c9 04 __ CMP #$04
10c4 : 90 07 __ BCC $10cd ; (nformf.s13 + 0)
.s60:
10c6 : a9 01 __ LDA #$01
10c8 : 85 53 __ STA T10 + 0 
10ca : 4c 0a 13 JMP $130a ; (nformf.s51 + 0)
.s13:
10cd : a5 53 __ LDA T10 + 0 
10cf : d0 f9 __ BNE $10ca ; (nformf.s60 + 4)
.s14:
10d1 : 24 4e __ BIT T5 + 1 
10d3 : 10 35 __ BPL $110a ; (nformf.s15 + 0)
.s50:
10d5 : a5 43 __ LDA T0 + 0 
10d7 : 85 1b __ STA ACCU + 0 
10d9 : a5 44 __ LDA T0 + 1 
10db : 85 1c __ STA ACCU + 1 
10dd : a5 45 __ LDA T0 + 2 
10df : 85 1d __ STA ACCU + 2 
10e1 : a5 46 __ LDA T0 + 3 
10e3 : 85 1e __ STA ACCU + 3 
.l90:
10e5 : 20 f5 19 JSR $19f5 ; (freg@proxy + 0)
10e8 : 20 96 16 JSR $1696 ; (crt_fdiv + 0)
10eb : 18 __ __ CLC
10ec : a5 4d __ LDA T5 + 0 
10ee : 69 01 __ ADC #$01
10f0 : 85 4d __ STA T5 + 0 
10f2 : a5 4e __ LDA T5 + 1 
10f4 : 69 00 __ ADC #$00
10f6 : 85 4e __ STA T5 + 1 
10f8 : 30 eb __ BMI $10e5 ; (nformf.l90 + 0)
.s91:
10fa : a5 1e __ LDA ACCU + 3 
10fc : 85 46 __ STA T0 + 3 
10fe : a5 1d __ LDA ACCU + 2 
1100 : 85 45 __ STA T0 + 2 
1102 : a5 1c __ LDA ACCU + 1 
1104 : 85 44 __ STA T0 + 1 
1106 : a5 1b __ LDA ACCU + 0 
1108 : 85 43 __ STA T0 + 0 
.s15:
110a : 18 __ __ CLC
110b : a5 50 __ LDA T7 + 0 
110d : 65 4d __ ADC T5 + 0 
110f : 18 __ __ CLC
1110 : 69 01 __ ADC #$01
1112 : 85 4f __ STA T6 + 0 
1114 : c9 07 __ CMP #$07
1116 : 90 14 __ BCC $112c ; (nformf.s49 + 0)
.s16:
1118 : ad 4e 1a LDA $1a4e ; (fround5[0] + 24)
111b : 85 47 __ STA T1 + 0 
111d : ad 4f 1a LDA $1a4f ; (fround5[0] + 25)
1120 : 85 48 __ STA T1 + 1 
1122 : ad 50 1a LDA $1a50 ; (fround5[0] + 26)
1125 : 85 49 __ STA T1 + 2 
1127 : ad 51 1a LDA $1a51 ; (fround5[0] + 27)
112a : b0 15 __ BCS $1141 ; (nformf.s17 + 0)
.s49:
112c : 0a __ __ ASL
112d : 0a __ __ ASL
112e : aa __ __ TAX
112f : bd 32 1a LDA $1a32,x ; (freg@proxy + 15)
1132 : 85 47 __ STA T1 + 0 
1134 : bd 33 1a LDA $1a33,x ; (freg@proxy + 16)
1137 : 85 48 __ STA T1 + 1 
1139 : bd 34 1a LDA $1a34,x ; (freg@proxy + 17)
113c : 85 49 __ STA T1 + 2 
113e : bd 35 1a LDA $1a35,x ; (freg@proxy + 18)
.s17:
1141 : 85 4a __ STA T1 + 3 
1143 : a2 47 __ LDX #$47
1145 : 20 23 1a JSR $1a23 ; (freg@proxy + 0)
1148 : 20 e7 14 JSR $14e7 ; (faddsub + 6)
114b : a5 1c __ LDA ACCU + 1 
114d : 85 12 __ STA P5 ; (f + 1)
114f : a5 1d __ LDA ACCU + 2 
1151 : 85 13 __ STA P6 ; (f + 2)
1153 : a6 1b __ LDX ACCU + 0 
1155 : a5 1e __ LDA ACCU + 3 
1157 : 85 14 __ STA P7 ; (f + 3)
1159 : 30 22 __ BMI $117d ; (nformf.s18 + 0)
.s46:
115b : c9 41 __ CMP #$41
115d : d0 06 __ BNE $1165 ; (nformf.s48 + 0)
.s47:
115f : a5 13 __ LDA P6 ; (f + 2)
1161 : c9 20 __ CMP #$20
1163 : f0 02 __ BEQ $1167 ; (nformf.s45 + 0)
.s48:
1165 : 90 16 __ BCC $117d ; (nformf.s18 + 0)
.s45:
1167 : 20 f5 19 JSR $19f5 ; (freg@proxy + 0)
116a : 20 96 16 JSR $1696 ; (crt_fdiv + 0)
116d : a5 1c __ LDA ACCU + 1 
116f : 85 12 __ STA P5 ; (f + 1)
1171 : a5 1d __ LDA ACCU + 2 
1173 : 85 13 __ STA P6 ; (f + 2)
1175 : a5 1e __ LDA ACCU + 3 
1177 : 85 14 __ STA P7 ; (f + 3)
1179 : c6 50 __ DEC T7 + 0 
117b : a6 1b __ LDX ACCU + 0 
.s18:
117d : 38 __ __ SEC
117e : a5 4f __ LDA T6 + 0 
1180 : e5 50 __ SBC T7 + 0 
1182 : 85 4b __ STA T4 + 0 
1184 : a5 4f __ LDA T6 + 0 
1186 : c9 15 __ CMP #$15
1188 : 90 04 __ BCC $118e ; (nformf.s19 + 0)
.s44:
118a : a9 14 __ LDA #$14
118c : 85 4f __ STA T6 + 0 
.s19:
118e : a5 4b __ LDA T4 + 0 
1190 : d0 08 __ BNE $119a ; (nformf.s21 + 0)
.s20:
1192 : a9 30 __ LDA #$30
1194 : a4 52 __ LDY T9 + 0 
1196 : 91 0f __ STA (P2),y ; (str + 0)
1198 : e6 52 __ INC T9 + 0 
.s21:
119a : a9 00 __ LDA #$00
119c : 85 54 __ STA T11 + 0 
119e : c5 4b __ CMP T4 + 0 
11a0 : f0 61 __ BEQ $1203 ; (nformf.l43 + 0)
.s23:
11a2 : c9 07 __ CMP #$07
11a4 : 90 04 __ BCC $11aa ; (nformf.s24 + 0)
.l42:
11a6 : a9 30 __ LDA #$30
11a8 : b0 47 __ BCS $11f1 ; (nformf.l25 + 0)
.s24:
11aa : 86 1b __ STX ACCU + 0 
11ac : 86 43 __ STX T0 + 0 
11ae : a5 12 __ LDA P5 ; (f + 1)
11b0 : 85 1c __ STA ACCU + 1 
11b2 : 85 44 __ STA T0 + 1 
11b4 : a5 13 __ LDA P6 ; (f + 2)
11b6 : 85 1d __ STA ACCU + 2 
11b8 : 85 45 __ STA T0 + 2 
11ba : a5 14 __ LDA P7 ; (f + 3)
11bc : 85 1e __ STA ACCU + 3 
11be : 85 46 __ STA T0 + 3 
11c0 : 20 5d 18 JSR $185d ; (f32_to_i16 + 0)
11c3 : a5 1b __ LDA ACCU + 0 
11c5 : 85 51 __ STA T8 + 0 
11c7 : 20 a9 18 JSR $18a9 ; (sint16_to_float + 0)
11ca : a2 43 __ LDX #$43
11cc : 20 a0 14 JSR $14a0 ; (freg + 4)
11cf : a5 1e __ LDA ACCU + 3 
11d1 : 49 80 __ EOR #$80
11d3 : 85 1e __ STA ACCU + 3 
11d5 : 20 e7 14 JSR $14e7 ; (faddsub + 6)
11d8 : 20 f5 19 JSR $19f5 ; (freg@proxy + 0)
11db : 20 ce 15 JSR $15ce ; (crt_fmul + 0)
11de : a5 1c __ LDA ACCU + 1 
11e0 : 85 12 __ STA P5 ; (f + 1)
11e2 : a5 1d __ LDA ACCU + 2 
11e4 : 85 13 __ STA P6 ; (f + 2)
11e6 : a5 1e __ LDA ACCU + 3 
11e8 : 85 14 __ STA P7 ; (f + 3)
11ea : 18 __ __ CLC
11eb : a5 51 __ LDA T8 + 0 
11ed : 69 30 __ ADC #$30
11ef : a6 1b __ LDX ACCU + 0 
.l25:
11f1 : a4 52 __ LDY T9 + 0 
11f3 : 91 0f __ STA (P2),y ; (str + 0)
11f5 : e6 52 __ INC T9 + 0 
11f7 : e6 54 __ INC T11 + 0 
11f9 : a5 54 __ LDA T11 + 0 
11fb : c5 4f __ CMP T6 + 0 
11fd : b0 14 __ BCS $1213 ; (nformf.s26 + 0)
.s22:
11ff : c5 4b __ CMP T4 + 0 
1201 : d0 9f __ BNE $11a2 ; (nformf.s23 + 0)
.l43:
1203 : a9 2e __ LDA #$2e
1205 : a4 52 __ LDY T9 + 0 
1207 : 91 0f __ STA (P2),y ; (str + 0)
1209 : a5 54 __ LDA T11 + 0 
120b : c9 07 __ CMP #$07
120d : e6 52 __ INC T9 + 0 
120f : 90 99 __ BCC $11aa ; (nformf.s24 + 0)
1211 : b0 93 __ BCS $11a6 ; (nformf.l42 + 0)
.s26:
1213 : a5 53 __ LDA T10 + 0 
1215 : f0 66 __ BEQ $127d ; (nformf.s27 + 0)
.s38:
1217 : a0 03 __ LDY #$03
1219 : b1 0d __ LDA (P0),y ; (si + 0)
121b : 69 03 __ ADC #$03
121d : a4 52 __ LDY T9 + 0 
121f : 91 0f __ STA (P2),y ; (str + 0)
1221 : c8 __ __ INY
1222 : 84 52 __ STY T9 + 0 
1224 : 24 4e __ BIT T5 + 1 
1226 : 30 06 __ BMI $122e ; (nformf.s41 + 0)
.s39:
1228 : a9 2b __ LDA #$2b
122a : 91 0f __ STA (P2),y ; (str + 0)
122c : d0 11 __ BNE $123f ; (nformf.s40 + 0)
.s41:
122e : a9 2d __ LDA #$2d
1230 : 91 0f __ STA (P2),y ; (str + 0)
1232 : 38 __ __ SEC
1233 : a9 00 __ LDA #$00
1235 : e5 4d __ SBC T5 + 0 
1237 : 85 4d __ STA T5 + 0 
1239 : a9 00 __ LDA #$00
123b : e5 4e __ SBC T5 + 1 
123d : 85 4e __ STA T5 + 1 
.s40:
123f : a5 4d __ LDA T5 + 0 
1241 : 85 1b __ STA ACCU + 0 
1243 : a5 4e __ LDA T5 + 1 
1245 : 85 1c __ STA ACCU + 1 
1247 : e6 52 __ INC T9 + 0 
1249 : a9 0a __ LDA #$0a
124b : 85 03 __ STA WORK + 0 
124d : a9 00 __ LDA #$00
124f : 85 04 __ STA WORK + 1 
1251 : 20 44 17 JSR $1744 ; (divs16 + 0)
1254 : 18 __ __ CLC
1255 : a5 1b __ LDA ACCU + 0 
1257 : 69 30 __ ADC #$30
1259 : a4 52 __ LDY T9 + 0 
125b : 91 0f __ STA (P2),y ; (str + 0)
125d : a5 4d __ LDA T5 + 0 
125f : 85 1b __ STA ACCU + 0 
1261 : a5 4e __ LDA T5 + 1 
1263 : 85 1c __ STA ACCU + 1 
1265 : e6 52 __ INC T9 + 0 
1267 : a9 0a __ LDA #$0a
1269 : 85 03 __ STA WORK + 0 
126b : a9 00 __ LDA #$00
126d : 85 04 __ STA WORK + 1 
126f : 20 06 18 JSR $1806 ; (mods16 + 0)
1272 : 18 __ __ CLC
1273 : a5 05 __ LDA WORK + 2 
1275 : 69 30 __ ADC #$30
1277 : a4 52 __ LDY T9 + 0 
1279 : 91 0f __ STA (P2),y ; (str + 0)
127b : e6 52 __ INC T9 + 0 
.s27:
127d : a5 52 __ LDA T9 + 0 
127f : a0 01 __ LDY #$01
1281 : d1 0d __ CMP (P0),y ; (si + 0)
1283 : b0 69 __ BCS $12ee ; (nformf.s3 + 0)
.s28:
1285 : a0 06 __ LDY #$06
1287 : b1 0d __ LDA (P0),y ; (si + 0)
1289 : f0 04 __ BEQ $128f ; (nformf.s29 + 0)
.s92:
128b : a6 52 __ LDX T9 + 0 
128d : 90 6c __ BCC $12fb ; (nformf.l36 + 0)
.s29:
128f : a5 52 __ LDA T9 + 0 
1291 : f0 3c __ BEQ $12cf ; (nformf.s30 + 0)
.s35:
1293 : e9 00 __ SBC #$00
1295 : a2 00 __ LDX #$00
1297 : b0 01 __ BCS $129a ; (nformf.s105 + 0)
.s104:
1299 : ca __ __ DEX
.s105:
129a : 18 __ __ CLC
129b : 65 0f __ ADC P2 ; (str + 0)
129d : 85 43 __ STA T0 + 0 
129f : 8a __ __ TXA
12a0 : 65 10 __ ADC P3 ; (str + 1)
12a2 : 85 44 __ STA T0 + 1 
12a4 : a9 01 __ LDA #$01
12a6 : 85 47 __ STA T1 + 0 
12a8 : a6 10 __ LDX P3 ; (str + 1)
12aa : 38 __ __ SEC
.l93:
12ab : a0 01 __ LDY #$01
12ad : b1 0d __ LDA (P0),y ; (si + 0)
12af : e5 47 __ SBC T1 + 0 
12b1 : 85 4b __ STA T4 + 0 
12b3 : 8a __ __ TXA
12b4 : 69 ff __ ADC #$ff
12b6 : 85 4c __ STA T4 + 1 
12b8 : 88 __ __ DEY
12b9 : b1 43 __ LDA (T0 + 0),y 
12bb : a4 0f __ LDY P2 ; (str + 0)
12bd : 91 4b __ STA (T4 + 0),y 
12bf : a5 43 __ LDA T0 + 0 
12c1 : d0 02 __ BNE $12c5 ; (nformf.s96 + 0)
.s95:
12c3 : c6 44 __ DEC T0 + 1 
.s96:
12c5 : c6 43 __ DEC T0 + 0 
12c7 : e6 47 __ INC T1 + 0 
12c9 : a5 52 __ LDA T9 + 0 
12cb : c5 47 __ CMP T1 + 0 
12cd : b0 dc __ BCS $12ab ; (nformf.l93 + 0)
.s30:
12cf : a9 00 __ LDA #$00
12d1 : 85 4b __ STA T4 + 0 
12d3 : 90 08 __ BCC $12dd ; (nformf.l31 + 0)
.s33:
12d5 : a9 20 __ LDA #$20
12d7 : a4 4b __ LDY T4 + 0 
12d9 : 91 0f __ STA (P2),y ; (str + 0)
12db : e6 4b __ INC T4 + 0 
.l31:
12dd : a0 01 __ LDY #$01
12df : b1 0d __ LDA (P0),y ; (si + 0)
12e1 : 38 __ __ SEC
12e2 : e5 52 __ SBC T9 + 0 
12e4 : 90 ef __ BCC $12d5 ; (nformf.s33 + 0)
.s34:
12e6 : c5 4b __ CMP T4 + 0 
12e8 : 90 02 __ BCC $12ec ; (nformf.s32 + 0)
.s94:
12ea : d0 e9 __ BNE $12d5 ; (nformf.s33 + 0)
.s32:
12ec : b1 0d __ LDA (P0),y ; (si + 0)
.s3:
12ee : 85 1b __ STA ACCU + 0 
12f0 : ad f1 9f LDA $9ff1 ; (nformf@stack + 0)
12f3 : 85 53 __ STA T10 + 0 
12f5 : ad f2 9f LDA $9ff2 ; (nformf@stack + 1)
12f8 : 85 54 __ STA T11 + 0 
12fa : 60 __ __ RTS
.l36:
12fb : 8a __ __ TXA
12fc : a0 01 __ LDY #$01
12fe : d1 0d __ CMP (P0),y ; (si + 0)
1300 : b0 ea __ BCS $12ec ; (nformf.s32 + 0)
.s37:
1302 : a8 __ __ TAY
1303 : a9 20 __ LDA #$20
1305 : 91 0f __ STA (P2),y ; (str + 0)
1307 : e8 __ __ INX
1308 : 90 f1 __ BCC $12fb ; (nformf.l36 + 0)
.s51:
130a : a5 4f __ LDA T6 + 0 
130c : c9 07 __ CMP #$07
130e : 90 14 __ BCC $1324 ; (nformf.s58 + 0)
.s52:
1310 : ad 4e 1a LDA $1a4e ; (fround5[0] + 24)
1313 : 85 47 __ STA T1 + 0 
1315 : ad 4f 1a LDA $1a4f ; (fround5[0] + 25)
1318 : 85 48 __ STA T1 + 1 
131a : ad 50 1a LDA $1a50 ; (fround5[0] + 26)
131d : 85 49 __ STA T1 + 2 
131f : ad 51 1a LDA $1a51 ; (fround5[0] + 27)
1322 : b0 15 __ BCS $1339 ; (nformf.s53 + 0)
.s58:
1324 : 0a __ __ ASL
1325 : 0a __ __ ASL
1326 : aa __ __ TAX
1327 : bd 32 1a LDA $1a32,x ; (freg@proxy + 15)
132a : 85 47 __ STA T1 + 0 
132c : bd 33 1a LDA $1a33,x ; (freg@proxy + 16)
132f : 85 48 __ STA T1 + 1 
1331 : bd 34 1a LDA $1a34,x ; (freg@proxy + 17)
1334 : 85 49 __ STA T1 + 2 
1336 : bd 35 1a LDA $1a35,x ; (freg@proxy + 18)
.s53:
1339 : 85 4a __ STA T1 + 3 
133b : a2 47 __ LDX #$47
133d : 20 23 1a JSR $1a23 ; (freg@proxy + 0)
1340 : 20 e7 14 JSR $14e7 ; (faddsub + 6)
1343 : a5 1c __ LDA ACCU + 1 
1345 : 85 12 __ STA P5 ; (f + 1)
1347 : a5 1d __ LDA ACCU + 2 
1349 : 85 13 __ STA P6 ; (f + 2)
134b : a6 1b __ LDX ACCU + 0 
134d : a5 1e __ LDA ACCU + 3 
134f : 85 14 __ STA P7 ; (f + 3)
1351 : 10 03 __ BPL $1356 ; (nformf.s55 + 0)
1353 : 4c 7d 11 JMP $117d ; (nformf.s18 + 0)
.s55:
1356 : c9 41 __ CMP #$41
1358 : d0 06 __ BNE $1360 ; (nformf.s57 + 0)
.s56:
135a : a5 13 __ LDA P6 ; (f + 2)
135c : c9 20 __ CMP #$20
135e : f0 02 __ BEQ $1362 ; (nformf.s54 + 0)
.s57:
1360 : 90 f1 __ BCC $1353 ; (nformf.s53 + 26)
.s54:
1362 : 20 f5 19 JSR $19f5 ; (freg@proxy + 0)
1365 : 20 96 16 JSR $1696 ; (crt_fdiv + 0)
1368 : a5 1c __ LDA ACCU + 1 
136a : 85 12 __ STA P5 ; (f + 1)
136c : a5 1d __ LDA ACCU + 2 
136e : 85 13 __ STA P6 ; (f + 2)
1370 : a5 1e __ LDA ACCU + 3 
1372 : 85 14 __ STA P7 ; (f + 3)
1374 : a6 1b __ LDX ACCU + 0 
1376 : e6 4d __ INC T5 + 0 
1378 : d0 d9 __ BNE $1353 ; (nformf.s53 + 26)
.s99:
137a : e6 4e __ INC T5 + 1 
137c : 4c 7d 11 JMP $117d ; (nformf.s18 + 0)
--------------------------------------------------------------------
137f : __ __ __ BYT 25 64 3a 20 25 64 0a 00                         : %d: %d..
--------------------------------------------------------------------
RN_asm: ; RN_asm()->u16
;  11, "C:/Users/korvus/Projects/C64/project_rogue_C64/src/demo/platform.h"
.s4:
1387 : a9 00 __ LDA #$00
1389 : 85 03 __ STA WORK + 0 
138b : 85 04 __ STA WORK + 1 
138d : 85 05 __ STA WORK + 2 
138f : 85 06 __ STA WORK + 3 
1391 : a9 65 __ LDA #$65
1393 : 85 07 __ STA WORK + 4 
1395 : a9 2b __ LDA #$2b
1397 : 85 08 __ STA WORK + 5 
1399 : a0 0e __ LDY #$0e
139b : 46 08 __ LSR WORK + 5 
139d : 66 07 __ ROR WORK + 4 
139f : 90 1d __ BCC $13be ; (RN_asm.s4 + 55)
13a1 : 18 __ __ CLC
13a2 : a5 03 __ LDA WORK + 0 
13a4 : 6d fb 13 ADC $13fb ; (seed + 0)
13a7 : 85 03 __ STA WORK + 0 
13a9 : a5 04 __ LDA WORK + 1 
13ab : 6d fc 13 ADC $13fc ; (seed + 1)
13ae : 85 04 __ STA WORK + 1 
13b0 : a5 05 __ LDA WORK + 2 
13b2 : 6d fd 13 ADC $13fd ; (seed + 2)
13b5 : 85 05 __ STA WORK + 2 
13b7 : a5 06 __ LDA WORK + 3 
13b9 : 6d fe 13 ADC $13fe ; (seed + 3)
13bc : 85 06 __ STA WORK + 3 
13be : 0e fb 13 ASL $13fb ; (seed + 0)
13c1 : 2e fc 13 ROL $13fc ; (seed + 1)
13c4 : 2e fd 13 ROL $13fd ; (seed + 2)
13c7 : 2e fe 13 ROL $13fe ; (seed + 3)
13ca : 88 __ __ DEY
13cb : d0 ce __ BNE $139b ; (RN_asm.s4 + 20)
13cd : 18 __ __ CLC
13ce : a5 03 __ LDA WORK + 0 
13d0 : 69 19 __ ADC #$19
13d2 : 8d fb 13 STA $13fb ; (seed + 0)
13d5 : a5 04 __ LDA WORK + 1 
13d7 : 69 36 __ ADC #$36
13d9 : 8d fc 13 STA $13fc ; (seed + 1)
13dc : a5 05 __ LDA WORK + 2 
13de : 69 00 __ ADC #$00
13e0 : 8d fd 13 STA $13fd ; (seed + 2)
13e3 : 85 1b __ STA ACCU + 0 
13e5 : a5 06 __ LDA WORK + 3 
13e7 : 69 00 __ ADC #$00
13e9 : 8d fe 13 STA $13fe ; (seed + 3)
13ec : 85 1c __ STA ACCU + 1 
.s3:
13ee : 60 __ __ RTS
--------------------------------------------------------------------
13ef : __ __ __ BYT 52 41 4d 20 54 45 53 54 3a 0a 00                : RAM TEST:..
--------------------------------------------------------------------
spentry:
13fa : __ __ __ BYT 00                                              : .
--------------------------------------------------------------------
seed:
13fb : __ __ __ BYT 6f 00 00 00                                     : o...
--------------------------------------------------------------------
1400 : __ __ __ BYT 20 42 41 53 49 43 20 4b 49 4b 41 50 43 53 4f 4c :  BASIC KIKAPCSOL
1410 : __ __ __ BYT 41 53 20 45 4c 4f 54 54 3a 20 25 75 20 48 49 42 : AS ELOTT: %u HIB
1420 : __ __ __ BYT 41 0a 00                                        : A..
--------------------------------------------------------------------
ram_test: ; ram_test()->u16
;   7, "C:/Users/korvus/Projects/C64/project_rogue_C64/src/demo/main.c"
.s4:
1423 : a9 00 __ LDA #$00
1425 : 85 44 __ STA T1 + 1 
1427 : 85 1b __ STA ACCU + 0 
1429 : 85 1c __ STA ACCU + 1 
142b : 85 45 __ STA T2 + 0 
142d : a8 __ __ TAY
142e : a9 a0 __ LDA #$a0
1430 : 85 46 __ STA T2 + 1 
1432 : a2 03 __ LDX #$03
.l10:
1434 : 8a __ __ TXA
1435 : 91 45 __ STA (T2 + 0),y 
1437 : c8 __ __ INY
1438 : d0 02 __ BNE $143c ; (ram_test.s13 + 0)
.s12:
143a : e6 46 __ INC T2 + 1 
.s13:
143c : 18 __ __ CLC
143d : 69 07 __ ADC #$07
143f : aa __ __ TAX
1440 : a5 44 __ LDA T1 + 1 
1442 : 69 00 __ ADC #$00
1444 : 85 44 __ STA T1 + 1 
1446 : c9 e0 __ CMP #$e0
1448 : d0 ea __ BNE $1434 ; (ram_test.l10 + 0)
.s11:
144a : e0 03 __ CPX #$03
144c : d0 e6 __ BNE $1434 ; (ram_test.l10 + 0)
.s5:
144e : a9 a0 __ LDA #$a0
1450 : 85 46 __ STA T2 + 1 
1452 : a0 00 __ LDY #$00
1454 : 84 44 __ STY T1 + 1 
.l6:
1456 : 8a __ __ TXA
1457 : d1 45 __ CMP (T2 + 0),y 
1459 : f0 06 __ BEQ $1461 ; (ram_test.s7 + 0)
.s9:
145b : e6 1b __ INC ACCU + 0 
145d : d0 02 __ BNE $1461 ; (ram_test.s7 + 0)
.s16:
145f : e6 1c __ INC ACCU + 1 
.s7:
1461 : c8 __ __ INY
1462 : d0 02 __ BNE $1466 ; (ram_test.s15 + 0)
.s14:
1464 : e6 46 __ INC T2 + 1 
.s15:
1466 : 18 __ __ CLC
1467 : 69 07 __ ADC #$07
1469 : aa __ __ TAX
146a : a5 44 __ LDA T1 + 1 
146c : 69 00 __ ADC #$00
146e : 85 44 __ STA T1 + 1 
1470 : c9 e0 __ CMP #$e0
1472 : d0 e2 __ BNE $1456 ; (ram_test.l6 + 0)
.s8:
1474 : e0 03 __ CPX #$03
1476 : d0 de __ BNE $1456 ; (ram_test.l6 + 0)
.s3:
1478 : 60 __ __ RTS
--------------------------------------------------------------------
1479 : __ __ __ BYT 20 42 41 53 49 43 20 4b 49 4b 41 50 43 53 4f 4c :  BASIC KIKAPCSOL
1489 : __ __ __ BYT 41 53 20 55 54 41 4e 3a 20 20 25 75 20 48 49 42 : AS UTAN:  %u HIB
1499 : __ __ __ BYT 41 0a 00                                        : A..
--------------------------------------------------------------------
freg: ; freg
149c : b1 19 __ LDA (IP + 0),y 
149e : c8 __ __ INY
149f : aa __ __ TAX
14a0 : b5 00 __ LDA $00,x 
14a2 : 85 03 __ STA WORK + 0 
14a4 : b5 01 __ LDA $01,x 
14a6 : 85 04 __ STA WORK + 1 
14a8 : b5 02 __ LDA $02,x 
14aa : 85 05 __ STA WORK + 2 
14ac : b5 03 __ LDA WORK + 0,x 
14ae : 85 06 __ STA WORK + 3 
14b0 : a5 05 __ LDA WORK + 2 
14b2 : 0a __ __ ASL
14b3 : a5 06 __ LDA WORK + 3 
14b5 : 2a __ __ ROL
14b6 : 85 08 __ STA WORK + 5 
14b8 : f0 06 __ BEQ $14c0 ; (freg + 36)
14ba : a5 05 __ LDA WORK + 2 
14bc : 09 80 __ ORA #$80
14be : 85 05 __ STA WORK + 2 
14c0 : a5 1d __ LDA ACCU + 2 
14c2 : 0a __ __ ASL
14c3 : a5 1e __ LDA ACCU + 3 
14c5 : 2a __ __ ROL
14c6 : 85 07 __ STA WORK + 4 
14c8 : f0 06 __ BEQ $14d0 ; (freg + 52)
14ca : a5 1d __ LDA ACCU + 2 
14cc : 09 80 __ ORA #$80
14ce : 85 1d __ STA ACCU + 2 
14d0 : 60 __ __ RTS
14d1 : 06 1e __ ASL ACCU + 3 
14d3 : a5 07 __ LDA WORK + 4 
14d5 : 6a __ __ ROR
14d6 : 85 1e __ STA ACCU + 3 
14d8 : b0 06 __ BCS $14e0 ; (freg + 68)
14da : a5 1d __ LDA ACCU + 2 
14dc : 29 7f __ AND #$7f
14de : 85 1d __ STA ACCU + 2 
14e0 : 60 __ __ RTS
--------------------------------------------------------------------
faddsub: ; faddsub
14e1 : a5 06 __ LDA WORK + 3 
14e3 : 49 80 __ EOR #$80
14e5 : 85 06 __ STA WORK + 3 
14e7 : a9 ff __ LDA #$ff
14e9 : c5 07 __ CMP WORK + 4 
14eb : f0 04 __ BEQ $14f1 ; (faddsub + 16)
14ed : c5 08 __ CMP WORK + 5 
14ef : d0 11 __ BNE $1502 ; (faddsub + 33)
14f1 : a5 1e __ LDA ACCU + 3 
14f3 : 09 7f __ ORA #$7f
14f5 : 85 1e __ STA ACCU + 3 
14f7 : a9 80 __ LDA #$80
14f9 : 85 1d __ STA ACCU + 2 
14fb : a9 00 __ LDA #$00
14fd : 85 1b __ STA ACCU + 0 
14ff : 85 1c __ STA ACCU + 1 
1501 : 60 __ __ RTS
1502 : 38 __ __ SEC
1503 : a5 07 __ LDA WORK + 4 
1505 : e5 08 __ SBC WORK + 5 
1507 : f0 38 __ BEQ $1541 ; (faddsub + 96)
1509 : aa __ __ TAX
150a : b0 25 __ BCS $1531 ; (faddsub + 80)
150c : e0 e9 __ CPX #$e9
150e : b0 0e __ BCS $151e ; (faddsub + 61)
1510 : a5 08 __ LDA WORK + 5 
1512 : 85 07 __ STA WORK + 4 
1514 : a9 00 __ LDA #$00
1516 : 85 1b __ STA ACCU + 0 
1518 : 85 1c __ STA ACCU + 1 
151a : 85 1d __ STA ACCU + 2 
151c : f0 23 __ BEQ $1541 ; (faddsub + 96)
151e : a5 1d __ LDA ACCU + 2 
1520 : 4a __ __ LSR
1521 : 66 1c __ ROR ACCU + 1 
1523 : 66 1b __ ROR ACCU + 0 
1525 : e8 __ __ INX
1526 : d0 f8 __ BNE $1520 ; (faddsub + 63)
1528 : 85 1d __ STA ACCU + 2 
152a : a5 08 __ LDA WORK + 5 
152c : 85 07 __ STA WORK + 4 
152e : 4c 41 15 JMP $1541 ; (faddsub + 96)
1531 : e0 18 __ CPX #$18
1533 : b0 33 __ BCS $1568 ; (faddsub + 135)
1535 : a5 05 __ LDA WORK + 2 
1537 : 4a __ __ LSR
1538 : 66 04 __ ROR WORK + 1 
153a : 66 03 __ ROR WORK + 0 
153c : ca __ __ DEX
153d : d0 f8 __ BNE $1537 ; (faddsub + 86)
153f : 85 05 __ STA WORK + 2 
1541 : a5 1e __ LDA ACCU + 3 
1543 : 29 80 __ AND #$80
1545 : 85 1e __ STA ACCU + 3 
1547 : 45 06 __ EOR WORK + 3 
1549 : 30 31 __ BMI $157c ; (faddsub + 155)
154b : 18 __ __ CLC
154c : a5 1b __ LDA ACCU + 0 
154e : 65 03 __ ADC WORK + 0 
1550 : 85 1b __ STA ACCU + 0 
1552 : a5 1c __ LDA ACCU + 1 
1554 : 65 04 __ ADC WORK + 1 
1556 : 85 1c __ STA ACCU + 1 
1558 : a5 1d __ LDA ACCU + 2 
155a : 65 05 __ ADC WORK + 2 
155c : 85 1d __ STA ACCU + 2 
155e : 90 08 __ BCC $1568 ; (faddsub + 135)
1560 : 66 1d __ ROR ACCU + 2 
1562 : 66 1c __ ROR ACCU + 1 
1564 : 66 1b __ ROR ACCU + 0 
1566 : e6 07 __ INC WORK + 4 
1568 : a5 07 __ LDA WORK + 4 
156a : c9 ff __ CMP #$ff
156c : f0 83 __ BEQ $14f1 ; (faddsub + 16)
156e : 4a __ __ LSR
156f : 05 1e __ ORA ACCU + 3 
1571 : 85 1e __ STA ACCU + 3 
1573 : b0 06 __ BCS $157b ; (faddsub + 154)
1575 : a5 1d __ LDA ACCU + 2 
1577 : 29 7f __ AND #$7f
1579 : 85 1d __ STA ACCU + 2 
157b : 60 __ __ RTS
157c : 38 __ __ SEC
157d : a5 1b __ LDA ACCU + 0 
157f : e5 03 __ SBC WORK + 0 
1581 : 85 1b __ STA ACCU + 0 
1583 : a5 1c __ LDA ACCU + 1 
1585 : e5 04 __ SBC WORK + 1 
1587 : 85 1c __ STA ACCU + 1 
1589 : a5 1d __ LDA ACCU + 2 
158b : e5 05 __ SBC WORK + 2 
158d : 85 1d __ STA ACCU + 2 
158f : b0 19 __ BCS $15aa ; (faddsub + 201)
1591 : 38 __ __ SEC
1592 : a9 00 __ LDA #$00
1594 : e5 1b __ SBC ACCU + 0 
1596 : 85 1b __ STA ACCU + 0 
1598 : a9 00 __ LDA #$00
159a : e5 1c __ SBC ACCU + 1 
159c : 85 1c __ STA ACCU + 1 
159e : a9 00 __ LDA #$00
15a0 : e5 1d __ SBC ACCU + 2 
15a2 : 85 1d __ STA ACCU + 2 
15a4 : a5 1e __ LDA ACCU + 3 
15a6 : 49 80 __ EOR #$80
15a8 : 85 1e __ STA ACCU + 3 
15aa : a5 1d __ LDA ACCU + 2 
15ac : 30 ba __ BMI $1568 ; (faddsub + 135)
15ae : 05 1c __ ORA ACCU + 1 
15b0 : 05 1b __ ORA ACCU + 0 
15b2 : f0 0f __ BEQ $15c3 ; (faddsub + 226)
15b4 : c6 07 __ DEC WORK + 4 
15b6 : f0 0b __ BEQ $15c3 ; (faddsub + 226)
15b8 : 06 1b __ ASL ACCU + 0 
15ba : 26 1c __ ROL ACCU + 1 
15bc : 26 1d __ ROL ACCU + 2 
15be : 10 f4 __ BPL $15b4 ; (faddsub + 211)
15c0 : 4c 68 15 JMP $1568 ; (faddsub + 135)
15c3 : a9 00 __ LDA #$00
15c5 : 85 1b __ STA ACCU + 0 
15c7 : 85 1c __ STA ACCU + 1 
15c9 : 85 1d __ STA ACCU + 2 
15cb : 85 1e __ STA ACCU + 3 
15cd : 60 __ __ RTS
--------------------------------------------------------------------
crt_fmul: ; crt_fmul
15ce : a5 1b __ LDA ACCU + 0 
15d0 : 05 1c __ ORA ACCU + 1 
15d2 : 05 1d __ ORA ACCU + 2 
15d4 : f0 0e __ BEQ $15e4 ; (crt_fmul + 22)
15d6 : a5 03 __ LDA WORK + 0 
15d8 : 05 04 __ ORA WORK + 1 
15da : 05 05 __ ORA WORK + 2 
15dc : d0 09 __ BNE $15e7 ; (crt_fmul + 25)
15de : 85 1b __ STA ACCU + 0 
15e0 : 85 1c __ STA ACCU + 1 
15e2 : 85 1d __ STA ACCU + 2 
15e4 : 85 1e __ STA ACCU + 3 
15e6 : 60 __ __ RTS
15e7 : a5 1e __ LDA ACCU + 3 
15e9 : 45 06 __ EOR WORK + 3 
15eb : 29 80 __ AND #$80
15ed : 85 1e __ STA ACCU + 3 
15ef : a9 ff __ LDA #$ff
15f1 : c5 07 __ CMP WORK + 4 
15f3 : f0 42 __ BEQ $1637 ; (crt_fmul + 105)
15f5 : c5 08 __ CMP WORK + 5 
15f7 : f0 3e __ BEQ $1637 ; (crt_fmul + 105)
15f9 : a9 00 __ LDA #$00
15fb : 85 09 __ STA WORK + 6 
15fd : 85 0a __ STA WORK + 7 
15ff : 85 0b __ STA WORK + 8 
1601 : a4 1b __ LDY ACCU + 0 
1603 : a5 03 __ LDA WORK + 0 
1605 : d0 06 __ BNE $160d ; (crt_fmul + 63)
1607 : a5 04 __ LDA WORK + 1 
1609 : f0 0a __ BEQ $1615 ; (crt_fmul + 71)
160b : d0 05 __ BNE $1612 ; (crt_fmul + 68)
160d : 20 68 16 JSR $1668 ; (crt_fmul8 + 0)
1610 : a5 04 __ LDA WORK + 1 
1612 : 20 68 16 JSR $1668 ; (crt_fmul8 + 0)
1615 : a5 05 __ LDA WORK + 2 
1617 : 20 68 16 JSR $1668 ; (crt_fmul8 + 0)
161a : 38 __ __ SEC
161b : a5 0b __ LDA WORK + 8 
161d : 30 06 __ BMI $1625 ; (crt_fmul + 87)
161f : 06 09 __ ASL WORK + 6 
1621 : 26 0a __ ROL WORK + 7 
1623 : 2a __ __ ROL
1624 : 18 __ __ CLC
1625 : 29 7f __ AND #$7f
1627 : 85 0b __ STA WORK + 8 
1629 : a5 07 __ LDA WORK + 4 
162b : 65 08 __ ADC WORK + 5 
162d : 90 19 __ BCC $1648 ; (crt_fmul + 122)
162f : e9 7f __ SBC #$7f
1631 : b0 04 __ BCS $1637 ; (crt_fmul + 105)
1633 : c9 ff __ CMP #$ff
1635 : d0 15 __ BNE $164c ; (crt_fmul + 126)
1637 : a5 1e __ LDA ACCU + 3 
1639 : 09 7f __ ORA #$7f
163b : 85 1e __ STA ACCU + 3 
163d : a9 80 __ LDA #$80
163f : 85 1d __ STA ACCU + 2 
1641 : a9 00 __ LDA #$00
1643 : 85 1b __ STA ACCU + 0 
1645 : 85 1c __ STA ACCU + 1 
1647 : 60 __ __ RTS
1648 : e9 7e __ SBC #$7e
164a : 90 15 __ BCC $1661 ; (crt_fmul + 147)
164c : 4a __ __ LSR
164d : 05 1e __ ORA ACCU + 3 
164f : 85 1e __ STA ACCU + 3 
1651 : a9 00 __ LDA #$00
1653 : 6a __ __ ROR
1654 : 05 0b __ ORA WORK + 8 
1656 : 85 1d __ STA ACCU + 2 
1658 : a5 0a __ LDA WORK + 7 
165a : 85 1c __ STA ACCU + 1 
165c : a5 09 __ LDA WORK + 6 
165e : 85 1b __ STA ACCU + 0 
1660 : 60 __ __ RTS
1661 : a9 00 __ LDA #$00
1663 : 85 1e __ STA ACCU + 3 
1665 : f0 d8 __ BEQ $163f ; (crt_fmul + 113)
1667 : 60 __ __ RTS
--------------------------------------------------------------------
crt_fmul8: ; crt_fmul8
1668 : 38 __ __ SEC
1669 : 6a __ __ ROR
166a : 90 1e __ BCC $168a ; (crt_fmul8 + 34)
166c : aa __ __ TAX
166d : 18 __ __ CLC
166e : 98 __ __ TYA
166f : 65 09 __ ADC WORK + 6 
1671 : 85 09 __ STA WORK + 6 
1673 : a5 0a __ LDA WORK + 7 
1675 : 65 1c __ ADC ACCU + 1 
1677 : 85 0a __ STA WORK + 7 
1679 : a5 0b __ LDA WORK + 8 
167b : 65 1d __ ADC ACCU + 2 
167d : 6a __ __ ROR
167e : 85 0b __ STA WORK + 8 
1680 : 8a __ __ TXA
1681 : 66 0a __ ROR WORK + 7 
1683 : 66 09 __ ROR WORK + 6 
1685 : 4a __ __ LSR
1686 : f0 0d __ BEQ $1695 ; (crt_fmul8 + 45)
1688 : b0 e2 __ BCS $166c ; (crt_fmul8 + 4)
168a : 66 0b __ ROR WORK + 8 
168c : 66 0a __ ROR WORK + 7 
168e : 66 09 __ ROR WORK + 6 
1690 : 4a __ __ LSR
1691 : 90 f7 __ BCC $168a ; (crt_fmul8 + 34)
1693 : d0 d7 __ BNE $166c ; (crt_fmul8 + 4)
1695 : 60 __ __ RTS
--------------------------------------------------------------------
crt_fdiv: ; crt_fdiv
1696 : a5 1b __ LDA ACCU + 0 
1698 : 05 1c __ ORA ACCU + 1 
169a : 05 1d __ ORA ACCU + 2 
169c : d0 03 __ BNE $16a1 ; (crt_fdiv + 11)
169e : 85 1e __ STA ACCU + 3 
16a0 : 60 __ __ RTS
16a1 : a5 1e __ LDA ACCU + 3 
16a3 : 45 06 __ EOR WORK + 3 
16a5 : 29 80 __ AND #$80
16a7 : 85 1e __ STA ACCU + 3 
16a9 : a5 08 __ LDA WORK + 5 
16ab : f0 62 __ BEQ $170f ; (crt_fdiv + 121)
16ad : a5 07 __ LDA WORK + 4 
16af : c9 ff __ CMP #$ff
16b1 : f0 5c __ BEQ $170f ; (crt_fdiv + 121)
16b3 : a9 00 __ LDA #$00
16b5 : 85 09 __ STA WORK + 6 
16b7 : 85 0a __ STA WORK + 7 
16b9 : 85 0b __ STA WORK + 8 
16bb : a2 18 __ LDX #$18
16bd : a5 1b __ LDA ACCU + 0 
16bf : c5 03 __ CMP WORK + 0 
16c1 : a5 1c __ LDA ACCU + 1 
16c3 : e5 04 __ SBC WORK + 1 
16c5 : a5 1d __ LDA ACCU + 2 
16c7 : e5 05 __ SBC WORK + 2 
16c9 : 90 13 __ BCC $16de ; (crt_fdiv + 72)
16cb : a5 1b __ LDA ACCU + 0 
16cd : e5 03 __ SBC WORK + 0 
16cf : 85 1b __ STA ACCU + 0 
16d1 : a5 1c __ LDA ACCU + 1 
16d3 : e5 04 __ SBC WORK + 1 
16d5 : 85 1c __ STA ACCU + 1 
16d7 : a5 1d __ LDA ACCU + 2 
16d9 : e5 05 __ SBC WORK + 2 
16db : 85 1d __ STA ACCU + 2 
16dd : 38 __ __ SEC
16de : 26 09 __ ROL WORK + 6 
16e0 : 26 0a __ ROL WORK + 7 
16e2 : 26 0b __ ROL WORK + 8 
16e4 : ca __ __ DEX
16e5 : f0 0a __ BEQ $16f1 ; (crt_fdiv + 91)
16e7 : 06 1b __ ASL ACCU + 0 
16e9 : 26 1c __ ROL ACCU + 1 
16eb : 26 1d __ ROL ACCU + 2 
16ed : b0 dc __ BCS $16cb ; (crt_fdiv + 53)
16ef : 90 cc __ BCC $16bd ; (crt_fdiv + 39)
16f1 : 38 __ __ SEC
16f2 : a5 0b __ LDA WORK + 8 
16f4 : 30 06 __ BMI $16fc ; (crt_fdiv + 102)
16f6 : 06 09 __ ASL WORK + 6 
16f8 : 26 0a __ ROL WORK + 7 
16fa : 2a __ __ ROL
16fb : 18 __ __ CLC
16fc : 29 7f __ AND #$7f
16fe : 85 0b __ STA WORK + 8 
1700 : a5 07 __ LDA WORK + 4 
1702 : e5 08 __ SBC WORK + 5 
1704 : 90 1a __ BCC $1720 ; (crt_fdiv + 138)
1706 : 18 __ __ CLC
1707 : 69 7f __ ADC #$7f
1709 : b0 04 __ BCS $170f ; (crt_fdiv + 121)
170b : c9 ff __ CMP #$ff
170d : d0 15 __ BNE $1724 ; (crt_fdiv + 142)
170f : a5 1e __ LDA ACCU + 3 
1711 : 09 7f __ ORA #$7f
1713 : 85 1e __ STA ACCU + 3 
1715 : a9 80 __ LDA #$80
1717 : 85 1d __ STA ACCU + 2 
1719 : a9 00 __ LDA #$00
171b : 85 1c __ STA ACCU + 1 
171d : 85 1b __ STA ACCU + 0 
171f : 60 __ __ RTS
1720 : 69 7f __ ADC #$7f
1722 : 90 15 __ BCC $1739 ; (crt_fdiv + 163)
1724 : 4a __ __ LSR
1725 : 05 1e __ ORA ACCU + 3 
1727 : 85 1e __ STA ACCU + 3 
1729 : a9 00 __ LDA #$00
172b : 6a __ __ ROR
172c : 05 0b __ ORA WORK + 8 
172e : 85 1d __ STA ACCU + 2 
1730 : a5 0a __ LDA WORK + 7 
1732 : 85 1c __ STA ACCU + 1 
1734 : a5 09 __ LDA WORK + 6 
1736 : 85 1b __ STA ACCU + 0 
1738 : 60 __ __ RTS
1739 : a9 00 __ LDA #$00
173b : 85 1e __ STA ACCU + 3 
173d : 85 1d __ STA ACCU + 2 
173f : 85 1c __ STA ACCU + 1 
1741 : 85 1b __ STA ACCU + 0 
1743 : 60 __ __ RTS
--------------------------------------------------------------------
divs16: ; divs16
1744 : 24 1c __ BIT ACCU + 1 
1746 : 10 0d __ BPL $1755 ; (divs16 + 17)
1748 : 20 5f 17 JSR $175f ; (negaccu + 0)
174b : 24 04 __ BIT WORK + 1 
174d : 10 0d __ BPL $175c ; (divs16 + 24)
174f : 20 6d 17 JSR $176d ; (negtmp + 0)
1752 : 4c 7b 17 JMP $177b ; (divmod + 0)
1755 : 24 04 __ BIT WORK + 1 
1757 : 10 f9 __ BPL $1752 ; (divs16 + 14)
1759 : 20 6d 17 JSR $176d ; (negtmp + 0)
175c : 20 7b 17 JSR $177b ; (divmod + 0)
--------------------------------------------------------------------
negaccu: ; negaccu
175f : 38 __ __ SEC
1760 : a9 00 __ LDA #$00
1762 : e5 1b __ SBC ACCU + 0 
1764 : 85 1b __ STA ACCU + 0 
1766 : a9 00 __ LDA #$00
1768 : e5 1c __ SBC ACCU + 1 
176a : 85 1c __ STA ACCU + 1 
176c : 60 __ __ RTS
--------------------------------------------------------------------
negtmp: ; negtmp
176d : 38 __ __ SEC
176e : a9 00 __ LDA #$00
1770 : e5 03 __ SBC WORK + 0 
1772 : 85 03 __ STA WORK + 0 
1774 : a9 00 __ LDA #$00
1776 : e5 04 __ SBC WORK + 1 
1778 : 85 04 __ STA WORK + 1 
177a : 60 __ __ RTS
--------------------------------------------------------------------
divmod: ; divmod
177b : a5 1c __ LDA ACCU + 1 
177d : d0 3b __ BNE $17ba ; (divmod + 63)
177f : a5 04 __ LDA WORK + 1 
1781 : d0 1e __ BNE $17a1 ; (divmod + 38)
1783 : 85 06 __ STA WORK + 3 
1785 : a2 04 __ LDX #$04
1787 : 06 1b __ ASL ACCU + 0 
1789 : 2a __ __ ROL
178a : c5 03 __ CMP WORK + 0 
178c : 90 02 __ BCC $1790 ; (divmod + 21)
178e : e5 03 __ SBC WORK + 0 
1790 : 26 1b __ ROL ACCU + 0 
1792 : 2a __ __ ROL
1793 : c5 03 __ CMP WORK + 0 
1795 : 90 02 __ BCC $1799 ; (divmod + 30)
1797 : e5 03 __ SBC WORK + 0 
1799 : 26 1b __ ROL ACCU + 0 
179b : ca __ __ DEX
179c : d0 eb __ BNE $1789 ; (divmod + 14)
179e : 85 05 __ STA WORK + 2 
17a0 : 60 __ __ RTS
17a1 : a5 1b __ LDA ACCU + 0 
17a3 : 85 05 __ STA WORK + 2 
17a5 : a5 1c __ LDA ACCU + 1 
17a7 : 85 06 __ STA WORK + 3 
17a9 : a9 00 __ LDA #$00
17ab : 85 1b __ STA ACCU + 0 
17ad : 85 1c __ STA ACCU + 1 
17af : 60 __ __ RTS
17b0 : 85 03 __ STA WORK + 0 
17b2 : a9 00 __ LDA #$00
17b4 : 85 04 __ STA WORK + 1 
17b6 : a5 1c __ LDA ACCU + 1 
17b8 : f0 c9 __ BEQ $1783 ; (divmod + 8)
17ba : a5 04 __ LDA WORK + 1 
17bc : d0 1f __ BNE $17dd ; (divmod + 98)
17be : a5 03 __ LDA WORK + 0 
17c0 : 30 1b __ BMI $17dd ; (divmod + 98)
17c2 : a9 00 __ LDA #$00
17c4 : 85 06 __ STA WORK + 3 
17c6 : a2 10 __ LDX #$10
17c8 : 06 1b __ ASL ACCU + 0 
17ca : 26 1c __ ROL ACCU + 1 
17cc : 2a __ __ ROL
17cd : c5 03 __ CMP WORK + 0 
17cf : 90 02 __ BCC $17d3 ; (divmod + 88)
17d1 : e5 03 __ SBC WORK + 0 
17d3 : 26 1b __ ROL ACCU + 0 
17d5 : 26 1c __ ROL ACCU + 1 
17d7 : ca __ __ DEX
17d8 : d0 f2 __ BNE $17cc ; (divmod + 81)
17da : 85 05 __ STA WORK + 2 
17dc : 60 __ __ RTS
17dd : a9 00 __ LDA #$00
17df : 85 05 __ STA WORK + 2 
17e1 : 85 06 __ STA WORK + 3 
17e3 : a0 10 __ LDY #$10
17e5 : 18 __ __ CLC
17e6 : 26 1b __ ROL ACCU + 0 
17e8 : 26 1c __ ROL ACCU + 1 
17ea : 26 05 __ ROL WORK + 2 
17ec : 26 06 __ ROL WORK + 3 
17ee : 38 __ __ SEC
17ef : a5 05 __ LDA WORK + 2 
17f1 : e5 03 __ SBC WORK + 0 
17f3 : aa __ __ TAX
17f4 : a5 06 __ LDA WORK + 3 
17f6 : e5 04 __ SBC WORK + 1 
17f8 : 90 04 __ BCC $17fe ; (divmod + 131)
17fa : 86 05 __ STX WORK + 2 
17fc : 85 06 __ STA WORK + 3 
17fe : 88 __ __ DEY
17ff : d0 e5 __ BNE $17e6 ; (divmod + 107)
1801 : 26 1b __ ROL ACCU + 0 
1803 : 26 1c __ ROL ACCU + 1 
1805 : 60 __ __ RTS
--------------------------------------------------------------------
mods16: ; mods16
1806 : 24 1c __ BIT ACCU + 1 
1808 : 10 10 __ BPL $181a ; (mods16 + 20)
180a : 20 5f 17 JSR $175f ; (negaccu + 0)
180d : 24 04 __ BIT WORK + 1 
180f : 10 03 __ BPL $1814 ; (mods16 + 14)
1811 : 20 6d 17 JSR $176d ; (negtmp + 0)
1814 : 20 7b 17 JSR $177b ; (divmod + 0)
1817 : 4c 4f 18 JMP $184f ; (negtmpb + 0)
181a : 24 04 __ BIT WORK + 1 
181c : 10 03 __ BPL $1821 ; (mods16 + 27)
181e : 20 6d 17 JSR $176d ; (negtmp + 0)
1821 : 4c 7b 17 JMP $177b ; (divmod + 0)
1824 : 60 __ __ RTS
--------------------------------------------------------------------
negtmpb: ; negtmpb
184f : 38 __ __ SEC
1850 : a9 00 __ LDA #$00
1852 : e5 05 __ SBC WORK + 2 
1854 : 85 05 __ STA WORK + 2 
1856 : a9 00 __ LDA #$00
1858 : e5 06 __ SBC WORK + 3 
185a : 85 06 __ STA WORK + 3 
185c : 60 __ __ RTS
--------------------------------------------------------------------
f32_to_i16: ; f32_to_i16
185d : 20 c0 14 JSR $14c0 ; (freg + 36)
1860 : a5 07 __ LDA WORK + 4 
1862 : c9 7f __ CMP #$7f
1864 : b0 07 __ BCS $186d ; (f32_to_i16 + 16)
1866 : a9 00 __ LDA #$00
1868 : 85 1b __ STA ACCU + 0 
186a : 85 1c __ STA ACCU + 1 
186c : 60 __ __ RTS
186d : e9 8e __ SBC #$8e
186f : 90 16 __ BCC $1887 ; (f32_to_i16 + 42)
1871 : 24 1e __ BIT ACCU + 3 
1873 : 30 09 __ BMI $187e ; (f32_to_i16 + 33)
1875 : a9 ff __ LDA #$ff
1877 : 85 1b __ STA ACCU + 0 
1879 : a9 7f __ LDA #$7f
187b : 85 1c __ STA ACCU + 1 
187d : 60 __ __ RTS
187e : a9 00 __ LDA #$00
1880 : 85 1b __ STA ACCU + 0 
1882 : a9 80 __ LDA #$80
1884 : 85 1c __ STA ACCU + 1 
1886 : 60 __ __ RTS
1887 : aa __ __ TAX
1888 : a5 1c __ LDA ACCU + 1 
188a : 46 1d __ LSR ACCU + 2 
188c : 6a __ __ ROR
188d : e8 __ __ INX
188e : d0 fa __ BNE $188a ; (f32_to_i16 + 45)
1890 : 24 1e __ BIT ACCU + 3 
1892 : 10 0e __ BPL $18a2 ; (f32_to_i16 + 69)
1894 : 38 __ __ SEC
1895 : 49 ff __ EOR #$ff
1897 : 69 00 __ ADC #$00
1899 : 85 1b __ STA ACCU + 0 
189b : a9 00 __ LDA #$00
189d : e5 1d __ SBC ACCU + 2 
189f : 85 1c __ STA ACCU + 1 
18a1 : 60 __ __ RTS
18a2 : 85 1b __ STA ACCU + 0 
18a4 : a5 1d __ LDA ACCU + 2 
18a6 : 85 1c __ STA ACCU + 1 
18a8 : 60 __ __ RTS
--------------------------------------------------------------------
sint16_to_float: ; sint16_to_float
18a9 : 24 1c __ BIT ACCU + 1 
18ab : 30 03 __ BMI $18b0 ; (sint16_to_float + 7)
18ad : 4c c7 18 JMP $18c7 ; (uint16_to_float + 0)
18b0 : 38 __ __ SEC
18b1 : a9 00 __ LDA #$00
18b3 : e5 1b __ SBC ACCU + 0 
18b5 : 85 1b __ STA ACCU + 0 
18b7 : a9 00 __ LDA #$00
18b9 : e5 1c __ SBC ACCU + 1 
18bb : 85 1c __ STA ACCU + 1 
18bd : 20 c7 18 JSR $18c7 ; (uint16_to_float + 0)
18c0 : a5 1e __ LDA ACCU + 3 
18c2 : 09 80 __ ORA #$80
18c4 : 85 1e __ STA ACCU + 3 
18c6 : 60 __ __ RTS
--------------------------------------------------------------------
uint16_to_float: ; uint16_to_float
18c7 : a5 1b __ LDA ACCU + 0 
18c9 : 05 1c __ ORA ACCU + 1 
18cb : d0 05 __ BNE $18d2 ; (uint16_to_float + 11)
18cd : 85 1d __ STA ACCU + 2 
18cf : 85 1e __ STA ACCU + 3 
18d1 : 60 __ __ RTS
18d2 : a2 8e __ LDX #$8e
18d4 : a5 1c __ LDA ACCU + 1 
18d6 : 30 06 __ BMI $18de ; (uint16_to_float + 23)
18d8 : ca __ __ DEX
18d9 : 06 1b __ ASL ACCU + 0 
18db : 2a __ __ ROL
18dc : 10 fa __ BPL $18d8 ; (uint16_to_float + 17)
18de : 0a __ __ ASL
18df : 85 1d __ STA ACCU + 2 
18e1 : a5 1b __ LDA ACCU + 0 
18e3 : 85 1c __ STA ACCU + 1 
18e5 : 8a __ __ TXA
18e6 : 4a __ __ LSR
18e7 : 85 1e __ STA ACCU + 3 
18e9 : a9 00 __ LDA #$00
18eb : 85 1b __ STA ACCU + 0 
18ed : 66 1d __ ROR ACCU + 2 
18ef : 60 __ __ RTS
--------------------------------------------------------------------
divmod32: ; divmod32
190e : a9 00 __ LDA #$00
1910 : 85 07 __ STA WORK + 4 
1912 : 85 08 __ STA WORK + 5 
1914 : 85 09 __ STA WORK + 6 
1916 : 85 0a __ STA WORK + 7 
1918 : a5 05 __ LDA WORK + 2 
191a : 05 06 __ ORA WORK + 3 
191c : f0 4b __ BEQ $1969 ; (divmod32 + 91)
191e : a0 10 __ LDY #$10
1920 : a5 1e __ LDA ACCU + 3 
1922 : 85 08 __ STA WORK + 5 
1924 : a5 1d __ LDA ACCU + 2 
1926 : 85 07 __ STA WORK + 4 
1928 : a9 00 __ LDA #$00
192a : 85 1d __ STA ACCU + 2 
192c : 85 1e __ STA ACCU + 3 
192e : 18 __ __ CLC
192f : 26 1b __ ROL ACCU + 0 
1931 : 26 1c __ ROL ACCU + 1 
1933 : 26 07 __ ROL WORK + 4 
1935 : 26 08 __ ROL WORK + 5 
1937 : 26 09 __ ROL WORK + 6 
1939 : 26 0a __ ROL WORK + 7 
193b : a5 07 __ LDA WORK + 4 
193d : c5 03 __ CMP WORK + 0 
193f : a5 08 __ LDA WORK + 5 
1941 : e5 04 __ SBC WORK + 1 
1943 : a5 09 __ LDA WORK + 6 
1945 : e5 05 __ SBC WORK + 2 
1947 : aa __ __ TAX
1948 : a5 0a __ LDA WORK + 7 
194a : e5 06 __ SBC WORK + 3 
194c : 90 11 __ BCC $195f ; (divmod32 + 81)
194e : 86 09 __ STX WORK + 6 
1950 : 85 0a __ STA WORK + 7 
1952 : a5 07 __ LDA WORK + 4 
1954 : e5 03 __ SBC WORK + 0 
1956 : 85 07 __ STA WORK + 4 
1958 : a5 08 __ LDA WORK + 5 
195a : e5 04 __ SBC WORK + 1 
195c : 85 08 __ STA WORK + 5 
195e : 38 __ __ SEC
195f : 88 __ __ DEY
1960 : d0 cd __ BNE $192f ; (divmod32 + 33)
1962 : 26 1b __ ROL ACCU + 0 
1964 : 26 1c __ ROL ACCU + 1 
1966 : a4 02 __ LDY $02 
1968 : 60 __ __ RTS
1969 : a5 1d __ LDA ACCU + 2 
196b : 05 1e __ ORA ACCU + 3 
196d : d0 0c __ BNE $197b ; (divmod32 + 109)
196f : 20 7b 17 JSR $177b ; (divmod + 0)
1972 : a5 05 __ LDA WORK + 2 
1974 : 85 07 __ STA WORK + 4 
1976 : a5 06 __ LDA WORK + 3 
1978 : 85 08 __ STA WORK + 5 
197a : 60 __ __ RTS
197b : a0 20 __ LDY #$20
197d : a5 04 __ LDA WORK + 1 
197f : d0 27 __ BNE $19a8 ; (divmod32 + 154)
1981 : 18 __ __ CLC
1982 : 26 1b __ ROL ACCU + 0 
1984 : 26 1c __ ROL ACCU + 1 
1986 : 26 1d __ ROL ACCU + 2 
1988 : 26 1e __ ROL ACCU + 3 
198a : 2a __ __ ROL
198b : 90 05 __ BCC $1992 ; (divmod32 + 132)
198d : e5 03 __ SBC WORK + 0 
198f : 38 __ __ SEC
1990 : b0 06 __ BCS $1998 ; (divmod32 + 138)
1992 : c5 03 __ CMP WORK + 0 
1994 : 90 02 __ BCC $1998 ; (divmod32 + 138)
1996 : e5 03 __ SBC WORK + 0 
1998 : 88 __ __ DEY
1999 : d0 e7 __ BNE $1982 ; (divmod32 + 116)
199b : 85 07 __ STA WORK + 4 
199d : 26 1b __ ROL ACCU + 0 
199f : 26 1c __ ROL ACCU + 1 
19a1 : 26 1d __ ROL ACCU + 2 
19a3 : 26 1e __ ROL ACCU + 3 
19a5 : a4 02 __ LDY $02 
19a7 : 60 __ __ RTS
19a8 : a5 1e __ LDA ACCU + 3 
19aa : d0 10 __ BNE $19bc ; (divmod32 + 174)
19ac : a6 1d __ LDX ACCU + 2 
19ae : 86 1e __ STX ACCU + 3 
19b0 : a6 1c __ LDX ACCU + 1 
19b2 : 86 1d __ STX ACCU + 2 
19b4 : a6 1b __ LDX ACCU + 0 
19b6 : 86 1c __ STX ACCU + 1 
19b8 : 85 1b __ STA ACCU + 0 
19ba : a0 18 __ LDY #$18
19bc : 18 __ __ CLC
19bd : 26 1b __ ROL ACCU + 0 
19bf : 26 1c __ ROL ACCU + 1 
19c1 : 26 1d __ ROL ACCU + 2 
19c3 : 26 1e __ ROL ACCU + 3 
19c5 : 26 07 __ ROL WORK + 4 
19c7 : 26 08 __ ROL WORK + 5 
19c9 : 90 0c __ BCC $19d7 ; (divmod32 + 201)
19cb : a5 07 __ LDA WORK + 4 
19cd : e5 03 __ SBC WORK + 0 
19cf : aa __ __ TAX
19d0 : a5 08 __ LDA WORK + 5 
19d2 : e5 04 __ SBC WORK + 1 
19d4 : 38 __ __ SEC
19d5 : b0 0c __ BCS $19e3 ; (divmod32 + 213)
19d7 : 38 __ __ SEC
19d8 : a5 07 __ LDA WORK + 4 
19da : e5 03 __ SBC WORK + 0 
19dc : aa __ __ TAX
19dd : a5 08 __ LDA WORK + 5 
19df : e5 04 __ SBC WORK + 1 
19e1 : 90 04 __ BCC $19e7 ; (divmod32 + 217)
19e3 : 86 07 __ STX WORK + 4 
19e5 : 85 08 __ STA WORK + 5 
19e7 : 88 __ __ DEY
19e8 : d0 d3 __ BNE $19bd ; (divmod32 + 175)
19ea : 26 1b __ ROL ACCU + 0 
19ec : 26 1c __ ROL ACCU + 1 
19ee : 26 1d __ ROL ACCU + 2 
19f0 : 26 1e __ ROL ACCU + 3 
19f2 : a4 02 __ LDY $02 
19f4 : 60 __ __ RTS
--------------------------------------------------------------------
freg@proxy: ; freg@proxy
19f5 : a9 00 __ LDA #$00
19f7 : 85 03 __ STA WORK + 0 
19f9 : a9 00 __ LDA #$00
19fb : 85 04 __ STA WORK + 1 
19fd : a9 20 __ LDA #$20
19ff : 85 05 __ STA WORK + 2 
1a01 : a9 41 __ LDA #$41
1a03 : 85 06 __ STA WORK + 3 
1a05 : 4c b0 14 JMP $14b0 ; (freg + 20)
--------------------------------------------------------------------
freg@proxy: ; freg@proxy
1a08 : a9 00 __ LDA #$00
1a0a : 85 03 __ STA WORK + 0 
1a0c : a9 00 __ LDA #$00
1a0e : 85 04 __ STA WORK + 1 
1a10 : a5 43 __ LDA $43 
1a12 : 85 1b __ STA ACCU + 0 
1a14 : a5 44 __ LDA $44 
1a16 : 85 1c __ STA ACCU + 1 
1a18 : a5 45 __ LDA $45 
1a1a : 85 1d __ STA ACCU + 2 
1a1c : a5 46 __ LDA $46 
1a1e : 85 1e __ STA ACCU + 3 
1a20 : 4c b0 14 JMP $14b0 ; (freg + 20)
--------------------------------------------------------------------
freg@proxy: ; freg@proxy
1a23 : a5 43 __ LDA $43 
1a25 : 85 1b __ STA ACCU + 0 
1a27 : a5 44 __ LDA $44 
1a29 : 85 1c __ STA ACCU + 1 
1a2b : a5 45 __ LDA $45 
1a2d : 85 1d __ STA ACCU + 2 
1a2f : a5 46 __ LDA $46 
1a31 : 85 1e __ STA ACCU + 3 
1a33 : 4c a0 14 JMP $14a0 ; (freg + 4)
--------------------------------------------------------------------
fround5:
1a36 : __ __ __ BYT 00 00 00 3f cd cc 4c 3d 0a d7 a3 3b 6f 12 03 3a : ...?..L=...;o..:
1a46 : __ __ __ BYT 17 b7 51 38 ac c5 a7 36 bd 37 06 35             : ..Q8...6.7.5
