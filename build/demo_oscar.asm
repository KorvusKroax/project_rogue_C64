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
080e : 8e 40 19 STX $1940 ; (spentry + 0)
0811 : a2 19 __ LDX #$19
0813 : a0 61 __ LDY #$61
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
082a : c0 61 __ CPY #$61
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
;   6, "C:/Users/korvus/Projects/C64/project_rogue_C64/src/demo/main.c"
.s1:
0880 : a5 53 __ LDA T1 + 0 
0882 : 8d 9f 9f STA $9f9f ; (main@stack + 0)
.s4:
0885 : a9 93 __ LDA #$93
0887 : 20 d2 ff JSR $ffd2 
088a : a9 00 __ LDA #$00
088c : 85 53 __ STA T1 + 0 
.l5:
088e : a9 36 __ LDA #$36
0890 : 8d fa 9f STA $9ffa ; (sstack + 0)
0893 : a9 13 __ LDA #$13
0895 : 8d fb 9f STA $9ffb ; (sstack + 1)
0898 : a5 53 __ LDA T1 + 0 
089a : 8d fc 9f STA $9ffc ; (sstack + 2)
089d : a9 00 __ LDA #$00
089f : 8d fd 9f STA $9ffd ; (sstack + 3)
08a2 : 20 3e 13 JSR $133e ; (RN_asm.s4 + 0)
08a5 : a9 64 __ LDA #$64
08a7 : 20 ba 16 JSR $16ba ; (divmod + 53)
08aa : a5 05 __ LDA WORK + 2 
08ac : 8d fe 9f STA $9ffe ; (sstack + 4)
08af : a9 00 __ LDA #$00
08b1 : 8d ff 9f STA $9fff ; (sstack + 5)
08b4 : 20 ee 08 JSR $08ee ; (printf.s1 + 0)
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
08e2 : a9 00 __ LDA #$00
08e4 : 85 1b __ STA ACCU + 0 
08e6 : 85 1c __ STA ACCU + 1 
.s3:
08e8 : ad 9f 9f LDA $9f9f ; (main@stack + 0)
08eb : 85 53 __ STA T1 + 0 
08ed : 60 __ __ RTS
--------------------------------------------------------------------
printf: ; printf(const u8*)->void
;  18, "C:/oscar64/include/stdio.h"
.s1:
08ee : a2 08 __ LDX #$08
08f0 : b5 53 __ LDA T1 + 0,x 
08f2 : 9d a3 9f STA $9fa3,x ; (printf@stack + 0)
08f5 : ca __ __ DEX
08f6 : 10 f8 __ BPL $08f0 ; (printf.s1 + 2)
.s4:
08f8 : ad fa 9f LDA $9ffa ; (sstack + 0)
08fb : 85 58 __ STA T5 + 0 
08fd : a9 fc __ LDA #$fc
08ff : 85 56 __ STA T3 + 0 
0901 : a9 9f __ LDA #$9f
0903 : 85 57 __ STA T3 + 1 
0905 : a9 00 __ LDA #$00
0907 : 85 5b __ STA T7 + 0 
0909 : ad fb 9f LDA $9ffb ; (sstack + 1)
090c : 85 59 __ STA T5 + 1 
.l5:
090e : a0 00 __ LDY #$00
0910 : b1 58 __ LDA (T5 + 0),y 
0912 : d0 15 __ BNE $0929 ; (printf.s8 + 0)
.s6:
0914 : a6 5b __ LDX T7 + 0 
0916 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0919 : f0 03 __ BEQ $091e ; (printf.s3 + 0)
.s7:
091b : 20 4d 0c JSR $0c4d ; (puts@proxy + 0)
.s3:
091e : a2 08 __ LDX #$08
0920 : bd a3 9f LDA $9fa3,x ; (printf@stack + 0)
0923 : 95 53 __ STA T1 + 0,x 
0925 : ca __ __ DEX
0926 : 10 f8 __ BPL $0920 ; (printf.s3 + 2)
0928 : 60 __ __ RTS
.s8:
0929 : c9 25 __ CMP #$25
092b : f0 28 __ BEQ $0955 ; (printf.s12 + 0)
.s9:
092d : a6 5b __ LDX T7 + 0 
092f : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0932 : e6 58 __ INC T5 + 0 
0934 : d0 02 __ BNE $0938 ; (printf.s110 + 0)
.s109:
0936 : e6 59 __ INC T5 + 1 
.s110:
0938 : e8 __ __ INX
0939 : 86 5b __ STX T7 + 0 
093b : e0 28 __ CPX #$28
093d : 90 cf __ BCC $090e ; (printf.l5 + 0)
.s10:
093f : a9 b8 __ LDA #$b8
0941 : 85 0d __ STA P0 
0943 : a9 9f __ LDA #$9f
0945 : 85 0e __ STA P1 
0947 : 98 __ __ TYA
0948 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
.s11:
094b : 20 55 0c JSR $0c55 ; (puts.s4 + 0)
094e : a9 00 __ LDA #$00
.s92:
0950 : 85 5b __ STA T7 + 0 
0952 : 4c 0e 09 JMP $090e ; (printf.l5 + 0)
.s12:
0955 : a5 5b __ LDA T7 + 0 
0957 : f0 0c __ BEQ $0965 ; (printf.s13 + 0)
.s85:
0959 : aa __ __ TAX
095a : 98 __ __ TYA
095b : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
095e : 20 4d 0c JSR $0c4d ; (puts@proxy + 0)
0961 : a9 00 __ LDA #$00
0963 : 85 5b __ STA T7 + 0 
.s13:
0965 : 8d b5 9f STA $9fb5 ; (si.sign + 0)
0968 : 8d b6 9f STA $9fb6 ; (si.left + 0)
096b : 8d b7 9f STA $9fb7 ; (si.prefix + 0)
096e : a0 01 __ LDY #$01
0970 : b1 58 __ LDA (T5 + 0),y 
0972 : a2 20 __ LDX #$20
0974 : 8e b0 9f STX $9fb0 ; (si.fill + 0)
0977 : a2 00 __ LDX #$00
0979 : 8e b1 9f STX $9fb1 ; (si.width + 0)
097c : ca __ __ DEX
097d : 8e b2 9f STX $9fb2 ; (si.precision + 0)
0980 : a2 0a __ LDX #$0a
0982 : 8e b4 9f STX $9fb4 ; (si.base + 0)
0985 : aa __ __ TAX
0986 : a9 02 __ LDA #$02
0988 : d0 07 __ BNE $0991 ; (printf.l14 + 0)
.s81:
098a : a0 00 __ LDY #$00
098c : b1 58 __ LDA (T5 + 0),y 
098e : aa __ __ TAX
098f : a9 01 __ LDA #$01
.l14:
0991 : 18 __ __ CLC
0992 : 65 58 __ ADC T5 + 0 
0994 : 85 58 __ STA T5 + 0 
0996 : 90 02 __ BCC $099a ; (printf.s96 + 0)
.s95:
0998 : e6 59 __ INC T5 + 1 
.s96:
099a : e0 2b __ CPX #$2b
099c : d0 07 __ BNE $09a5 ; (printf.s15 + 0)
.s84:
099e : a9 01 __ LDA #$01
09a0 : 8d b5 9f STA $9fb5 ; (si.sign + 0)
09a3 : d0 e5 __ BNE $098a ; (printf.s81 + 0)
.s15:
09a5 : 8a __ __ TXA
09a6 : e0 30 __ CPX #$30
09a8 : d0 05 __ BNE $09af ; (printf.s16 + 0)
.s83:
09aa : 8d b0 9f STA $9fb0 ; (si.fill + 0)
09ad : f0 db __ BEQ $098a ; (printf.s81 + 0)
.s16:
09af : c9 23 __ CMP #$23
09b1 : d0 07 __ BNE $09ba ; (printf.s17 + 0)
.s82:
09b3 : a9 01 __ LDA #$01
09b5 : 8d b7 9f STA $9fb7 ; (si.prefix + 0)
09b8 : d0 d0 __ BNE $098a ; (printf.s81 + 0)
.s17:
09ba : c9 2d __ CMP #$2d
09bc : d0 07 __ BNE $09c5 ; (printf.s18 + 0)
.s80:
09be : a9 01 __ LDA #$01
09c0 : 8d b6 9f STA $9fb6 ; (si.left + 0)
09c3 : d0 c5 __ BNE $098a ; (printf.s81 + 0)
.s18:
09c5 : 85 55 __ STA T2 + 0 
09c7 : c9 30 __ CMP #$30
09c9 : 90 3a __ BCC $0a05 ; (printf.s19 + 0)
.s75:
09cb : e0 3a __ CPX #$3a
09cd : b0 66 __ BCS $0a35 ; (printf.s20 + 0)
.s76:
09cf : a9 00 __ LDA #$00
09d1 : e0 3a __ CPX #$3a
09d3 : b0 2b __ BCS $0a00 ; (printf.s78 + 0)
.s111:
09d5 : 85 43 __ STA T0 + 0 
09d7 : a4 58 __ LDY T5 + 0 
09d9 : 85 58 __ STA T5 + 0 
.l79:
09db : a5 43 __ LDA T0 + 0 
09dd : 0a __ __ ASL
09de : 0a __ __ ASL
09df : 18 __ __ CLC
09e0 : 65 43 __ ADC T0 + 0 
09e2 : 0a __ __ ASL
09e3 : 18 __ __ CLC
09e4 : 65 55 __ ADC T2 + 0 
09e6 : 38 __ __ SEC
09e7 : e9 30 __ SBC #$30
09e9 : 85 43 __ STA T0 + 0 
09eb : b1 58 __ LDA (T5 + 0),y 
09ed : 85 55 __ STA T2 + 0 
09ef : c8 __ __ INY
09f0 : d0 02 __ BNE $09f4 ; (printf.s108 + 0)
.s107:
09f2 : e6 59 __ INC T5 + 1 
.s108:
09f4 : c9 30 __ CMP #$30
09f6 : 90 04 __ BCC $09fc ; (printf.s112 + 0)
.s77:
09f8 : c9 3a __ CMP #$3a
09fa : 90 df __ BCC $09db ; (printf.l79 + 0)
.s112:
09fc : 84 58 __ STY T5 + 0 
09fe : a5 43 __ LDA T0 + 0 
.s78:
0a00 : 8d b1 9f STA $9fb1 ; (si.width + 0)
0a03 : a5 55 __ LDA T2 + 0 
.s19:
0a05 : c9 2e __ CMP #$2e
0a07 : d0 2c __ BNE $0a35 ; (printf.s20 + 0)
.s70:
0a09 : a9 00 __ LDA #$00
0a0b : a8 __ __ TAY
0a0c : f0 0e __ BEQ $0a1c ; (printf.l71 + 0)
.s74:
0a0e : a5 43 __ LDA T0 + 0 
0a10 : 0a __ __ ASL
0a11 : 0a __ __ ASL
0a12 : 18 __ __ CLC
0a13 : 65 43 __ ADC T0 + 0 
0a15 : 0a __ __ ASL
0a16 : 18 __ __ CLC
0a17 : 65 55 __ ADC T2 + 0 
0a19 : 38 __ __ SEC
0a1a : e9 30 __ SBC #$30
.l71:
0a1c : 85 43 __ STA T0 + 0 
0a1e : b1 58 __ LDA (T5 + 0),y 
0a20 : 85 55 __ STA T2 + 0 
0a22 : e6 58 __ INC T5 + 0 
0a24 : d0 02 __ BNE $0a28 ; (printf.s98 + 0)
.s97:
0a26 : e6 59 __ INC T5 + 1 
.s98:
0a28 : c9 30 __ CMP #$30
0a2a : 90 04 __ BCC $0a30 ; (printf.s72 + 0)
.s73:
0a2c : c9 3a __ CMP #$3a
0a2e : 90 de __ BCC $0a0e ; (printf.s74 + 0)
.s72:
0a30 : a6 43 __ LDX T0 + 0 
0a32 : 8e b2 9f STX $9fb2 ; (si.precision + 0)
.s20:
0a35 : c9 64 __ CMP #$64
0a37 : f0 0c __ BEQ $0a45 ; (printf.s69 + 0)
.s21:
0a39 : c9 44 __ CMP #$44
0a3b : f0 08 __ BEQ $0a45 ; (printf.s69 + 0)
.s22:
0a3d : c9 69 __ CMP #$69
0a3f : f0 04 __ BEQ $0a45 ; (printf.s69 + 0)
.s23:
0a41 : c9 49 __ CMP #$49
0a43 : d0 0f __ BNE $0a54 ; (printf.s24 + 0)
.s69:
0a45 : a9 01 __ LDA #$01
.s68:
0a47 : 85 13 __ STA P6 
0a49 : a0 00 __ LDY #$00
0a4b : b1 56 __ LDA (T3 + 0),y 
0a4d : aa __ __ TAX
0a4e : c8 __ __ INY
0a4f : b1 56 __ LDA (T3 + 0),y 
0a51 : 4c 2f 0c JMP $0c2f ; (printf.s66 + 0)
.s24:
0a54 : c9 75 __ CMP #$75
0a56 : f0 04 __ BEQ $0a5c ; (printf.s67 + 0)
.s25:
0a58 : c9 55 __ CMP #$55
0a5a : d0 04 __ BNE $0a60 ; (printf.s26 + 0)
.s67:
0a5c : a9 00 __ LDA #$00
0a5e : f0 e7 __ BEQ $0a47 ; (printf.s68 + 0)
.s26:
0a60 : c9 78 __ CMP #$78
0a62 : f0 04 __ BEQ $0a68 ; (printf.s65 + 0)
.s27:
0a64 : c9 58 __ CMP #$58
0a66 : d0 1d __ BNE $0a85 ; (printf.s28 + 0)
.s65:
0a68 : a0 00 __ LDY #$00
0a6a : 84 13 __ STY P6 
0a6c : a9 10 __ LDA #$10
0a6e : 8d b4 9f STA $9fb4 ; (si.base + 0)
0a71 : b1 56 __ LDA (T3 + 0),y 
0a73 : aa __ __ TAX
0a74 : c8 __ __ INY
0a75 : b1 56 __ LDA (T3 + 0),y 
0a77 : a8 __ __ TAY
0a78 : a5 55 __ LDA T2 + 0 
0a7a : 29 e0 __ AND #$e0
0a7c : 09 01 __ ORA #$01
0a7e : 8d b3 9f STA $9fb3 ; (si.cha + 0)
0a81 : 98 __ __ TYA
0a82 : 4c 2f 0c JMP $0c2f ; (printf.s66 + 0)
.s28:
0a85 : c9 6c __ CMP #$6c
0a87 : d0 03 __ BNE $0a8c ; (printf.s29 + 0)
0a89 : 4c b4 0b JMP $0bb4 ; (printf.s53 + 0)
.s29:
0a8c : c9 4c __ CMP #$4c
0a8e : f0 f9 __ BEQ $0a89 ; (printf.s28 + 4)
.s30:
0a90 : c9 66 __ CMP #$66
0a92 : f0 14 __ BEQ $0aa8 ; (printf.s52 + 0)
.s31:
0a94 : c9 67 __ CMP #$67
0a96 : f0 10 __ BEQ $0aa8 ; (printf.s52 + 0)
.s32:
0a98 : c9 65 __ CMP #$65
0a9a : f0 0c __ BEQ $0aa8 ; (printf.s52 + 0)
.s33:
0a9c : c9 46 __ CMP #$46
0a9e : f0 08 __ BEQ $0aa8 ; (printf.s52 + 0)
.s34:
0aa0 : c9 47 __ CMP #$47
0aa2 : f0 04 __ BEQ $0aa8 ; (printf.s52 + 0)
.s35:
0aa4 : c9 45 __ CMP #$45
0aa6 : d0 44 __ BNE $0aec ; (printf.s36 + 0)
.s52:
0aa8 : a9 b8 __ LDA #$b8
0aaa : 85 0f __ STA P2 
0aac : a9 9f __ LDA #$9f
0aae : 85 10 __ STA P3 
0ab0 : a0 00 __ LDY #$00
0ab2 : b1 56 __ LDA (T3 + 0),y 
0ab4 : 85 11 __ STA P4 
0ab6 : c8 __ __ INY
0ab7 : b1 56 __ LDA (T3 + 0),y 
0ab9 : 85 12 __ STA P5 
0abb : c8 __ __ INY
0abc : b1 56 __ LDA (T3 + 0),y 
0abe : 85 13 __ STA P6 
0ac0 : c8 __ __ INY
0ac1 : b1 56 __ LDA (T3 + 0),y 
0ac3 : 85 14 __ STA P7 
0ac5 : a5 55 __ LDA T2 + 0 
0ac7 : 29 e0 __ AND #$e0
0ac9 : 09 01 __ ORA #$01
0acb : 8d b3 9f STA $9fb3 ; (si.cha + 0)
0ace : a9 b0 __ LDA #$b0
0ad0 : 85 0d __ STA P0 
0ad2 : a9 9f __ LDA #$9f
0ad4 : 85 0e __ STA P1 
0ad6 : a5 55 __ LDA T2 + 0 
0ad8 : ed b3 9f SBC $9fb3 ; (si.cha + 0)
0adb : 18 __ __ CLC
0adc : 69 61 __ ADC #$61
0ade : 85 15 __ STA P8 
0ae0 : 20 ea 0e JSR $0eea ; (nformf.s1 + 0)
0ae3 : a5 1b __ LDA ACCU + 0 ; (fmt + 2)
0ae5 : 85 5b __ STA T7 + 0 
0ae7 : a9 04 __ LDA #$04
0ae9 : 4c a8 0b JMP $0ba8 ; (printf.s90 + 0)
.s36:
0aec : c9 73 __ CMP #$73
0aee : f0 2d __ BEQ $0b1d ; (printf.s44 + 0)
.s37:
0af0 : c9 53 __ CMP #$53
0af2 : f0 29 __ BEQ $0b1d ; (printf.s44 + 0)
.s38:
0af4 : c9 63 __ CMP #$63
0af6 : f0 12 __ BEQ $0b0a ; (printf.s43 + 0)
.s39:
0af8 : c9 43 __ CMP #$43
0afa : f0 0e __ BEQ $0b0a ; (printf.s43 + 0)
.s40:
0afc : aa __ __ TAX
0afd : d0 03 __ BNE $0b02 ; (printf.s41 + 0)
0aff : 4c 0e 09 JMP $090e ; (printf.l5 + 0)
.s41:
0b02 : 8d b8 9f STA $9fb8 ; (buff[0] + 0)
.s42:
0b05 : a9 01 __ LDA #$01
0b07 : 4c 50 09 JMP $0950 ; (printf.s92 + 0)
.s43:
0b0a : a0 00 __ LDY #$00
0b0c : b1 56 __ LDA (T3 + 0),y 
0b0e : 8d b8 9f STA $9fb8 ; (buff[0] + 0)
0b11 : a5 56 __ LDA T3 + 0 
0b13 : 69 01 __ ADC #$01
0b15 : 85 56 __ STA T3 + 0 
0b17 : 90 ec __ BCC $0b05 ; (printf.s42 + 0)
.s106:
0b19 : e6 57 __ INC T3 + 1 
0b1b : b0 e8 __ BCS $0b05 ; (printf.s42 + 0)
.s44:
0b1d : a0 00 __ LDY #$00
0b1f : 84 5a __ STY T6 + 0 
0b21 : b1 56 __ LDA (T3 + 0),y 
0b23 : 85 53 __ STA T1 + 0 
0b25 : c8 __ __ INY
0b26 : b1 56 __ LDA (T3 + 0),y 
0b28 : 85 54 __ STA T1 + 1 
0b2a : a5 56 __ LDA T3 + 0 
0b2c : 69 01 __ ADC #$01
0b2e : 85 56 __ STA T3 + 0 
0b30 : 90 02 __ BCC $0b34 ; (printf.s105 + 0)
.s104:
0b32 : e6 57 __ INC T3 + 1 
.s105:
0b34 : ad b1 9f LDA $9fb1 ; (si.width + 0)
0b37 : f0 0d __ BEQ $0b46 ; (printf.s45 + 0)
.s93:
0b39 : a0 00 __ LDY #$00
0b3b : b1 53 __ LDA (T1 + 0),y 
0b3d : f0 05 __ BEQ $0b44 ; (printf.s94 + 0)
.l51:
0b3f : c8 __ __ INY
0b40 : b1 53 __ LDA (T1 + 0),y 
0b42 : d0 fb __ BNE $0b3f ; (printf.l51 + 0)
.s94:
0b44 : 84 5a __ STY T6 + 0 
.s45:
0b46 : ad b6 9f LDA $9fb6 ; (si.left + 0)
0b49 : 85 55 __ STA T2 + 0 
0b4b : d0 07 __ BNE $0b54 ; (printf.s47 + 0)
.s46:
0b4d : a4 5a __ LDY T6 + 0 
0b4f : cc b1 9f CPY $9fb1 ; (si.width + 0)
0b52 : 90 2a __ BCC $0b7e ; (printf.s50 + 0)
.s47:
0b54 : a5 53 __ LDA T1 + 0 
0b56 : 85 0d __ STA P0 
0b58 : a5 54 __ LDA T1 + 1 
0b5a : 85 0e __ STA P1 
0b5c : 20 55 0c JSR $0c55 ; (puts.s4 + 0)
0b5f : a5 55 __ LDA T2 + 0 
0b61 : f0 9c __ BEQ $0aff ; (printf.s40 + 3)
.s48:
0b63 : a4 5a __ LDY T6 + 0 
0b65 : cc b1 9f CPY $9fb1 ; (si.width + 0)
0b68 : b0 95 __ BCS $0aff ; (printf.s40 + 3)
.s49:
0b6a : ad b0 9f LDA $9fb0 ; (si.fill + 0)
0b6d : a2 00 __ LDX #$00
.l88:
0b6f : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0b72 : e8 __ __ INX
0b73 : c8 __ __ INY
0b74 : cc b1 9f CPY $9fb1 ; (si.width + 0)
0b77 : 90 f6 __ BCC $0b6f ; (printf.l88 + 0)
.s86:
0b79 : 86 5b __ STX T7 + 0 
0b7b : 4c 0e 09 JMP $090e ; (printf.l5 + 0)
.s50:
0b7e : ad b0 9f LDA $9fb0 ; (si.fill + 0)
0b81 : a2 00 __ LDX #$00
.l89:
0b83 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0b86 : e8 __ __ INX
0b87 : c8 __ __ INY
0b88 : cc b1 9f CPY $9fb1 ; (si.width + 0)
0b8b : 90 f6 __ BCC $0b83 ; (printf.l89 + 0)
.s87:
0b8d : a9 b8 __ LDA #$b8
0b8f : 85 0d __ STA P0 
0b91 : a9 9f __ LDA #$9f
0b93 : 85 0e __ STA P1 
0b95 : a9 00 __ LDA #$00
0b97 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0b9a : 20 55 0c JSR $0c55 ; (puts.s4 + 0)
0b9d : a5 53 __ LDA T1 + 0 
0b9f : 85 0d __ STA P0 
0ba1 : a5 54 __ LDA T1 + 1 
0ba3 : 85 0e __ STA P1 
0ba5 : 4c 4b 09 JMP $094b ; (printf.s11 + 0)
.s90:
0ba8 : 18 __ __ CLC
0ba9 : 65 56 __ ADC T3 + 0 
0bab : 85 56 __ STA T3 + 0 
0bad : 90 cc __ BCC $0b7b ; (printf.s86 + 2)
.s99:
0baf : e6 57 __ INC T3 + 1 
0bb1 : 4c 0e 09 JMP $090e ; (printf.l5 + 0)
.s53:
0bb4 : a0 00 __ LDY #$00
0bb6 : b1 56 __ LDA (T3 + 0),y 
0bb8 : 85 11 __ STA P4 
0bba : c8 __ __ INY
0bbb : b1 56 __ LDA (T3 + 0),y 
0bbd : 85 12 __ STA P5 
0bbf : c8 __ __ INY
0bc0 : b1 56 __ LDA (T3 + 0),y 
0bc2 : 85 13 __ STA P6 
0bc4 : c8 __ __ INY
0bc5 : b1 56 __ LDA (T3 + 0),y 
0bc7 : 85 14 __ STA P7 
0bc9 : a5 56 __ LDA T3 + 0 
0bcb : 69 03 __ ADC #$03
0bcd : 85 56 __ STA T3 + 0 
0bcf : 90 02 __ BCC $0bd3 ; (printf.s101 + 0)
.s100:
0bd1 : e6 57 __ INC T3 + 1 
.s101:
0bd3 : a0 00 __ LDY #$00
0bd5 : b1 58 __ LDA (T5 + 0),y 
0bd7 : aa __ __ TAX
0bd8 : e6 58 __ INC T5 + 0 
0bda : d0 02 __ BNE $0bde ; (printf.s103 + 0)
.s102:
0bdc : e6 59 __ INC T5 + 1 
.s103:
0bde : e0 64 __ CPX #$64
0be0 : f0 0c __ BEQ $0bee ; (printf.s64 + 0)
.s54:
0be2 : e0 44 __ CPX #$44
0be4 : f0 08 __ BEQ $0bee ; (printf.s64 + 0)
.s55:
0be6 : e0 69 __ CPX #$69
0be8 : f0 04 __ BEQ $0bee ; (printf.s64 + 0)
.s56:
0bea : e0 49 __ CPX #$49
0bec : d0 1c __ BNE $0c0a ; (printf.s57 + 0)
.s64:
0bee : a9 01 __ LDA #$01
.s91:
0bf0 : 85 15 __ STA P8 
.s62:
0bf2 : a9 b8 __ LDA #$b8
0bf4 : 85 0f __ STA P2 
0bf6 : a9 9f __ LDA #$9f
0bf8 : 85 0e __ STA P1 
0bfa : a9 9f __ LDA #$9f
0bfc : 85 10 __ STA P3 
0bfe : a9 b0 __ LDA #$b0
0c00 : 85 0d __ STA P0 
0c02 : 20 9f 0d JSR $0d9f ; (nforml.s4 + 0)
0c05 : a5 1b __ LDA ACCU + 0 ; (fmt + 2)
0c07 : 4c 50 09 JMP $0950 ; (printf.s92 + 0)
.s57:
0c0a : e0 75 __ CPX #$75
0c0c : f0 04 __ BEQ $0c12 ; (printf.s63 + 0)
.s58:
0c0e : e0 55 __ CPX #$55
0c10 : d0 03 __ BNE $0c15 ; (printf.s59 + 0)
.s63:
0c12 : 98 __ __ TYA
0c13 : f0 db __ BEQ $0bf0 ; (printf.s91 + 0)
.s59:
0c15 : e0 78 __ CPX #$78
0c17 : f0 04 __ BEQ $0c1d ; (printf.s61 + 0)
.s60:
0c19 : e0 58 __ CPX #$58
0c1b : d0 94 __ BNE $0bb1 ; (printf.s99 + 2)
.s61:
0c1d : 84 15 __ STY P8 
0c1f : a9 10 __ LDA #$10
0c21 : 8d b4 9f STA $9fb4 ; (si.base + 0)
0c24 : 8a __ __ TXA
0c25 : 29 e0 __ AND #$e0
0c27 : 09 01 __ ORA #$01
0c29 : 8d b3 9f STA $9fb3 ; (si.cha + 0)
0c2c : 4c f2 0b JMP $0bf2 ; (printf.s62 + 0)
.s66:
0c2f : 86 11 __ STX P4 
0c31 : 85 12 __ STA P5 
0c33 : a9 b8 __ LDA #$b8
0c35 : 85 0f __ STA P2 
0c37 : a9 9f __ LDA #$9f
0c39 : 85 0e __ STA P1 
0c3b : a9 9f __ LDA #$9f
0c3d : 85 10 __ STA P3 
0c3f : a9 b0 __ LDA #$b0
0c41 : 85 0d __ STA P0 
0c43 : 20 8a 0c JSR $0c8a ; (nformi.s4 + 0)
0c46 : 85 5b __ STA T7 + 0 
0c48 : a9 02 __ LDA #$02
0c4a : 4c a8 0b JMP $0ba8 ; (printf.s90 + 0)
--------------------------------------------------------------------
puts@proxy: ; puts@proxy
0c4d : a9 b8 __ LDA #$b8
0c4f : 85 0d __ STA P0 
0c51 : a9 9f __ LDA #$9f
0c53 : 85 0e __ STA P1 
--------------------------------------------------------------------
puts: ; puts(const u8*)->void
;  12, "C:/oscar64/include/stdio.h"
.s4:
0c55 : a0 00 __ LDY #$00
.l5:
0c57 : b1 0d __ LDA (P0),y ; (str + 0)
0c59 : aa __ __ TAX
0c5a : 18 __ __ CLC
0c5b : a5 0d __ LDA P0 ; (str + 0)
0c5d : 69 01 __ ADC #$01
0c5f : 85 0d __ STA P0 ; (str + 0)
0c61 : 8a __ __ TXA
0c62 : d0 01 __ BNE $0c65 ; (puts.s6 + 0)
.s3:
0c64 : 60 __ __ RTS
.s6:
0c65 : 90 02 __ BCC $0c69 ; (puts.s13 + 0)
.s12:
0c67 : e6 0e __ INC P1 ; (str + 1)
.s13:
0c69 : c9 0a __ CMP #$0a
0c6b : d0 08 __ BNE $0c75 ; (puts.s7 + 0)
.s10:
0c6d : a9 0d __ LDA #$0d
.s11:
0c6f : 20 d2 ff JSR $ffd2 
0c72 : 4c 57 0c JMP $0c57 ; (puts.l5 + 0)
.s7:
0c75 : c9 09 __ CMP #$09
0c77 : d0 f6 __ BNE $0c6f ; (puts.s11 + 0)
.s8:
0c79 : a5 d3 __ LDA $d3 
0c7b : 29 03 __ AND #$03
0c7d : aa __ __ TAX
.l9:
0c7e : a9 20 __ LDA #$20
0c80 : 20 d2 ff JSR $ffd2 
0c83 : e8 __ __ INX
0c84 : e0 04 __ CPX #$04
0c86 : 90 f6 __ BCC $0c7e ; (puts.l9 + 0)
0c88 : b0 cd __ BCS $0c57 ; (puts.l5 + 0)
--------------------------------------------------------------------
nformi: ; nformi(const struct sinfo*,u8*,i16,bool)->u8
;  79, "C:/oscar64/include/stdio.c"
.s4:
0c8a : a9 00 __ LDA #$00
0c8c : 85 43 __ STA T5 + 0 
0c8e : a0 04 __ LDY #$04
0c90 : b1 0d __ LDA (P0),y ; (si + 0)
0c92 : 85 44 __ STA T6 + 0 
0c94 : a5 13 __ LDA P6 ; (s + 0)
0c96 : f0 13 __ BEQ $0cab ; (nformi.s5 + 0)
.s34:
0c98 : 24 12 __ BIT P5 ; (v + 1)
0c9a : 10 0f __ BPL $0cab ; (nformi.s5 + 0)
.s35:
0c9c : 38 __ __ SEC
0c9d : a9 00 __ LDA #$00
0c9f : e5 11 __ SBC P4 ; (v + 0)
0ca1 : 85 11 __ STA P4 ; (v + 0)
0ca3 : a9 00 __ LDA #$00
0ca5 : e5 12 __ SBC P5 ; (v + 1)
0ca7 : 85 12 __ STA P5 ; (v + 1)
0ca9 : e6 43 __ INC T5 + 0 
.s5:
0cab : a9 10 __ LDA #$10
0cad : 85 45 __ STA T7 + 0 
0caf : a5 11 __ LDA P4 ; (v + 0)
0cb1 : 05 12 __ ORA P5 ; (v + 1)
0cb3 : f0 2d __ BEQ $0ce2 ; (nformi.s6 + 0)
.s29:
0cb5 : a5 11 __ LDA P4 ; (v + 0)
0cb7 : 85 1b __ STA ACCU + 0 
0cb9 : a5 12 __ LDA P5 ; (v + 1)
0cbb : 85 1c __ STA ACCU + 1 
.l30:
0cbd : a5 44 __ LDA T6 + 0 
0cbf : 20 ba 16 JSR $16ba ; (divmod + 53)
0cc2 : a5 05 __ LDA WORK + 2 
0cc4 : c9 0a __ CMP #$0a
0cc6 : b0 04 __ BCS $0ccc ; (nformi.s33 + 0)
.s31:
0cc8 : a9 30 __ LDA #$30
0cca : 90 06 __ BCC $0cd2 ; (nformi.s32 + 0)
.s33:
0ccc : a0 03 __ LDY #$03
0cce : b1 0d __ LDA (P0),y ; (si + 0)
0cd0 : e9 0a __ SBC #$0a
.s32:
0cd2 : 18 __ __ CLC
0cd3 : 65 05 __ ADC WORK + 2 
0cd5 : a6 45 __ LDX T7 + 0 
0cd7 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0cda : c6 45 __ DEC T7 + 0 
0cdc : a5 1b __ LDA ACCU + 0 
0cde : 05 1c __ ORA ACCU + 1 
0ce0 : d0 db __ BNE $0cbd ; (nformi.l30 + 0)
.s6:
0ce2 : a9 ff __ LDA #$ff
0ce4 : a0 02 __ LDY #$02
0ce6 : d1 0d __ CMP (P0),y ; (si + 0)
0ce8 : d0 04 __ BNE $0cee ; (nformi.s28 + 0)
.s7:
0cea : a9 0f __ LDA #$0f
0cec : d0 05 __ BNE $0cf3 ; (nformi.s40 + 0)
.s28:
0cee : 38 __ __ SEC
0cef : a9 10 __ LDA #$10
0cf1 : f1 0d __ SBC (P0),y ; (si + 0)
.s40:
0cf3 : a8 __ __ TAY
0cf4 : c4 45 __ CPY T7 + 0 
0cf6 : b0 0d __ BCS $0d05 ; (nformi.s8 + 0)
.s27:
0cf8 : a9 30 __ LDA #$30
.l41:
0cfa : a6 45 __ LDX T7 + 0 
0cfc : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0cff : c6 45 __ DEC T7 + 0 
0d01 : c4 45 __ CPY T7 + 0 
0d03 : 90 f5 __ BCC $0cfa ; (nformi.l41 + 0)
.s8:
0d05 : a0 07 __ LDY #$07
0d07 : b1 0d __ LDA (P0),y ; (si + 0)
0d09 : f0 1c __ BEQ $0d27 ; (nformi.s9 + 0)
.s25:
0d0b : a5 44 __ LDA T6 + 0 
0d0d : c9 10 __ CMP #$10
0d0f : d0 16 __ BNE $0d27 ; (nformi.s9 + 0)
.s26:
0d11 : a0 03 __ LDY #$03
0d13 : b1 0d __ LDA (P0),y ; (si + 0)
0d15 : a8 __ __ TAY
0d16 : a9 30 __ LDA #$30
0d18 : a6 45 __ LDX T7 + 0 
0d1a : 9d e8 9f STA $9fe8,x ; (buff[0] + 48)
0d1d : 98 __ __ TYA
0d1e : 69 16 __ ADC #$16
0d20 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d23 : ca __ __ DEX
0d24 : ca __ __ DEX
0d25 : 86 45 __ STX T7 + 0 
.s9:
0d27 : a9 00 __ LDA #$00
0d29 : 85 1b __ STA ACCU + 0 
0d2b : a5 43 __ LDA T5 + 0 
0d2d : f0 0c __ BEQ $0d3b ; (nformi.s10 + 0)
.s24:
0d2f : a9 2d __ LDA #$2d
.s23:
0d31 : a6 45 __ LDX T7 + 0 
0d33 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d36 : c6 45 __ DEC T7 + 0 
0d38 : 4c 45 0d JMP $0d45 ; (nformi.s11 + 0)
.s10:
0d3b : a0 05 __ LDY #$05
0d3d : b1 0d __ LDA (P0),y ; (si + 0)
0d3f : f0 04 __ BEQ $0d45 ; (nformi.s11 + 0)
.s22:
0d41 : a9 2b __ LDA #$2b
0d43 : d0 ec __ BNE $0d31 ; (nformi.s23 + 0)
.s11:
0d45 : a6 45 __ LDX T7 + 0 
0d47 : a0 06 __ LDY #$06
0d49 : b1 0d __ LDA (P0),y ; (si + 0)
0d4b : d0 2b __ BNE $0d78 ; (nformi.s18 + 0)
.l12:
0d4d : 8a __ __ TXA
0d4e : 18 __ __ CLC
0d4f : a0 01 __ LDY #$01
0d51 : 71 0d __ ADC (P0),y ; (si + 0)
0d53 : b0 04 __ BCS $0d59 ; (nformi.s16 + 0)
.s17:
0d55 : c9 11 __ CMP #$11
0d57 : 90 0a __ BCC $0d63 ; (nformi.s13 + 0)
.s16:
0d59 : a0 00 __ LDY #$00
0d5b : b1 0d __ LDA (P0),y ; (si + 0)
0d5d : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d60 : ca __ __ DEX
0d61 : b0 ea __ BCS $0d4d ; (nformi.l12 + 0)
.s13:
0d63 : e0 10 __ CPX #$10
0d65 : b0 0e __ BCS $0d75 ; (nformi.s14 + 0)
.s15:
0d67 : 88 __ __ DEY
.l38:
0d68 : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0d6b : 91 0f __ STA (P2),y ; (str + 0)
0d6d : c8 __ __ INY
0d6e : e8 __ __ INX
0d6f : e0 10 __ CPX #$10
0d71 : 90 f5 __ BCC $0d68 ; (nformi.l38 + 0)
.s39:
0d73 : 84 1b __ STY ACCU + 0 
.s14:
0d75 : a5 1b __ LDA ACCU + 0 
.s3:
0d77 : 60 __ __ RTS
.s18:
0d78 : e0 10 __ CPX #$10
0d7a : b0 1a __ BCS $0d96 ; (nformi.l19 + 0)
.s21:
0d7c : a0 00 __ LDY #$00
.l36:
0d7e : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0d81 : 91 0f __ STA (P2),y ; (str + 0)
0d83 : c8 __ __ INY
0d84 : e8 __ __ INX
0d85 : e0 10 __ CPX #$10
0d87 : 90 f5 __ BCC $0d7e ; (nformi.l36 + 0)
.s37:
0d89 : 84 1b __ STY ACCU + 0 
0d8b : b0 09 __ BCS $0d96 ; (nformi.l19 + 0)
.s20:
0d8d : 88 __ __ DEY
0d8e : b1 0d __ LDA (P0),y ; (si + 0)
0d90 : a4 1b __ LDY ACCU + 0 
0d92 : 91 0f __ STA (P2),y ; (str + 0)
0d94 : e6 1b __ INC ACCU + 0 
.l19:
0d96 : a5 1b __ LDA ACCU + 0 
0d98 : a0 01 __ LDY #$01
0d9a : d1 0d __ CMP (P0),y ; (si + 0)
0d9c : 90 ef __ BCC $0d8d ; (nformi.s20 + 0)
0d9e : 60 __ __ RTS
--------------------------------------------------------------------
nforml: ; nforml(const struct sinfo*,u8*,i32,bool)->u8
; 137, "C:/oscar64/include/stdio.c"
.s4:
0d9f : a9 00 __ LDA #$00
0da1 : 85 43 __ STA T4 + 0 
0da3 : a5 15 __ LDA P8 ; (s + 0)
0da5 : f0 1f __ BEQ $0dc6 ; (nforml.s5 + 0)
.s35:
0da7 : 24 14 __ BIT P7 ; (v + 3)
0da9 : 10 1b __ BPL $0dc6 ; (nforml.s5 + 0)
.s36:
0dab : 38 __ __ SEC
0dac : a9 00 __ LDA #$00
0dae : e5 11 __ SBC P4 ; (v + 0)
0db0 : 85 11 __ STA P4 ; (v + 0)
0db2 : a9 00 __ LDA #$00
0db4 : e5 12 __ SBC P5 ; (v + 1)
0db6 : 85 12 __ STA P5 ; (v + 1)
0db8 : a9 00 __ LDA #$00
0dba : e5 13 __ SBC P6 ; (v + 2)
0dbc : 85 13 __ STA P6 ; (v + 2)
0dbe : a9 00 __ LDA #$00
0dc0 : e5 14 __ SBC P7 ; (v + 3)
0dc2 : 85 14 __ STA P7 ; (v + 3)
0dc4 : e6 43 __ INC T4 + 0 
.s5:
0dc6 : a9 10 __ LDA #$10
0dc8 : 85 44 __ STA T5 + 0 
0dca : a5 14 __ LDA P7 ; (v + 3)
0dcc : d0 0c __ BNE $0dda ; (nforml.l43 + 0)
.s31:
0dce : a5 13 __ LDA P6 ; (v + 2)
0dd0 : d0 08 __ BNE $0dda ; (nforml.l43 + 0)
.s32:
0dd2 : a5 12 __ LDA P5 ; (v + 1)
0dd4 : d0 04 __ BNE $0dda ; (nforml.l43 + 0)
.s33:
0dd6 : c5 11 __ CMP P4 ; (v + 0)
0dd8 : b0 13 __ BCS $0ded ; (nforml.s6 + 0)
.l43:
0dda : a5 11 __ LDA P4 ; (v + 0)
0ddc : 85 1b __ STA ACCU + 0 
0dde : a5 12 __ LDA P5 ; (v + 1)
0de0 : 85 1c __ STA ACCU + 1 
0de2 : a5 13 __ LDA P6 ; (v + 2)
0de4 : 85 1d __ STA ACCU + 2 
0de6 : a5 14 __ LDA P7 ; (v + 3)
0de8 : 85 1e __ STA ACCU + 3 
0dea : 4c a9 0e JMP $0ea9 ; (nforml.l28 + 0)
.s6:
0ded : a9 ff __ LDA #$ff
0def : a0 02 __ LDY #$02
0df1 : d1 0d __ CMP (P0),y ; (si + 0)
0df3 : d0 04 __ BNE $0df9 ; (nforml.s27 + 0)
.s7:
0df5 : a9 0f __ LDA #$0f
0df7 : d0 05 __ BNE $0dfe ; (nforml.s41 + 0)
.s27:
0df9 : 38 __ __ SEC
0dfa : a9 10 __ LDA #$10
0dfc : f1 0d __ SBC (P0),y ; (si + 0)
.s41:
0dfe : a8 __ __ TAY
0dff : c4 44 __ CPY T5 + 0 
0e01 : b0 0d __ BCS $0e10 ; (nforml.s8 + 0)
.s26:
0e03 : a9 30 __ LDA #$30
.l42:
0e05 : a6 44 __ LDX T5 + 0 
0e07 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e0a : c6 44 __ DEC T5 + 0 
0e0c : c4 44 __ CPY T5 + 0 
0e0e : 90 f5 __ BCC $0e05 ; (nforml.l42 + 0)
.s8:
0e10 : a0 07 __ LDY #$07
0e12 : b1 0d __ LDA (P0),y ; (si + 0)
0e14 : f0 1d __ BEQ $0e33 ; (nforml.s9 + 0)
.s24:
0e16 : a9 10 __ LDA #$10
0e18 : a0 04 __ LDY #$04
0e1a : d1 0d __ CMP (P0),y ; (si + 0)
0e1c : d0 15 __ BNE $0e33 ; (nforml.s9 + 0)
.s25:
0e1e : 88 __ __ DEY
0e1f : b1 0d __ LDA (P0),y ; (si + 0)
0e21 : a8 __ __ TAY
0e22 : a9 30 __ LDA #$30
0e24 : a6 44 __ LDX T5 + 0 
0e26 : 9d e8 9f STA $9fe8,x ; (buff[0] + 48)
0e29 : 98 __ __ TYA
0e2a : 69 16 __ ADC #$16
0e2c : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e2f : ca __ __ DEX
0e30 : ca __ __ DEX
0e31 : 86 44 __ STX T5 + 0 
.s9:
0e33 : a9 00 __ LDA #$00
0e35 : 85 1b __ STA ACCU + 0 
0e37 : a5 43 __ LDA T4 + 0 
0e39 : f0 0c __ BEQ $0e47 ; (nforml.s10 + 0)
.s23:
0e3b : a9 2d __ LDA #$2d
.s22:
0e3d : a6 44 __ LDX T5 + 0 
0e3f : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e42 : c6 44 __ DEC T5 + 0 
0e44 : 4c 51 0e JMP $0e51 ; (nforml.s11 + 0)
.s10:
0e47 : a0 05 __ LDY #$05
0e49 : b1 0d __ LDA (P0),y ; (si + 0)
0e4b : f0 04 __ BEQ $0e51 ; (nforml.s11 + 0)
.s21:
0e4d : a9 2b __ LDA #$2b
0e4f : d0 ec __ BNE $0e3d ; (nforml.s22 + 0)
.s11:
0e51 : a0 06 __ LDY #$06
0e53 : a6 44 __ LDX T5 + 0 
0e55 : b1 0d __ LDA (P0),y ; (si + 0)
0e57 : d0 29 __ BNE $0e82 ; (nforml.s17 + 0)
.l12:
0e59 : 8a __ __ TXA
0e5a : 18 __ __ CLC
0e5b : a0 01 __ LDY #$01
0e5d : 71 0d __ ADC (P0),y ; (si + 0)
0e5f : b0 04 __ BCS $0e65 ; (nforml.s15 + 0)
.s16:
0e61 : c9 11 __ CMP #$11
0e63 : 90 0a __ BCC $0e6f ; (nforml.s13 + 0)
.s15:
0e65 : a0 00 __ LDY #$00
0e67 : b1 0d __ LDA (P0),y ; (si + 0)
0e69 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e6c : ca __ __ DEX
0e6d : b0 ea __ BCS $0e59 ; (nforml.l12 + 0)
.s13:
0e6f : e0 10 __ CPX #$10
0e71 : b0 0e __ BCS $0e81 ; (nforml.s3 + 0)
.s14:
0e73 : 88 __ __ DEY
.l39:
0e74 : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0e77 : 91 0f __ STA (P2),y ; (str + 0)
0e79 : c8 __ __ INY
0e7a : e8 __ __ INX
0e7b : e0 10 __ CPX #$10
0e7d : 90 f5 __ BCC $0e74 ; (nforml.l39 + 0)
.s40:
0e7f : 84 1b __ STY ACCU + 0 
.s3:
0e81 : 60 __ __ RTS
.s17:
0e82 : e0 10 __ CPX #$10
0e84 : b0 1a __ BCS $0ea0 ; (nforml.l18 + 0)
.s20:
0e86 : a0 00 __ LDY #$00
.l37:
0e88 : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0e8b : 91 0f __ STA (P2),y ; (str + 0)
0e8d : c8 __ __ INY
0e8e : e8 __ __ INX
0e8f : e0 10 __ CPX #$10
0e91 : 90 f5 __ BCC $0e88 ; (nforml.l37 + 0)
.s38:
0e93 : 84 1b __ STY ACCU + 0 
0e95 : b0 09 __ BCS $0ea0 ; (nforml.l18 + 0)
.s19:
0e97 : 88 __ __ DEY
0e98 : b1 0d __ LDA (P0),y ; (si + 0)
0e9a : a4 1b __ LDY ACCU + 0 
0e9c : 91 0f __ STA (P2),y ; (str + 0)
0e9e : e6 1b __ INC ACCU + 0 
.l18:
0ea0 : a5 1b __ LDA ACCU + 0 
0ea2 : a0 01 __ LDY #$01
0ea4 : d1 0d __ CMP (P0),y ; (si + 0)
0ea6 : 90 ef __ BCC $0e97 ; (nforml.s19 + 0)
0ea8 : 60 __ __ RTS
.l28:
0ea9 : a0 04 __ LDY #$04
0eab : b1 0d __ LDA (P0),y ; (si + 0)
0ead : 85 03 __ STA WORK + 0 
0eaf : a9 00 __ LDA #$00
0eb1 : 85 04 __ STA WORK + 1 
0eb3 : 85 05 __ STA WORK + 2 
0eb5 : 85 06 __ STA WORK + 3 
0eb7 : 20 18 18 JSR $1818 ; (divmod32 + 0)
0eba : a5 07 __ LDA WORK + 4 
0ebc : c9 0a __ CMP #$0a
0ebe : b0 04 __ BCS $0ec4 ; (nforml.s34 + 0)
.s29:
0ec0 : a9 30 __ LDA #$30
0ec2 : 90 06 __ BCC $0eca ; (nforml.s30 + 0)
.s34:
0ec4 : a0 03 __ LDY #$03
0ec6 : b1 0d __ LDA (P0),y ; (si + 0)
0ec8 : e9 0a __ SBC #$0a
.s30:
0eca : 18 __ __ CLC
0ecb : 65 07 __ ADC WORK + 4 
0ecd : a6 44 __ LDX T5 + 0 
0ecf : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0ed2 : a5 1b __ LDA ACCU + 0 
0ed4 : 85 11 __ STA P4 ; (v + 0)
0ed6 : a5 1c __ LDA ACCU + 1 
0ed8 : 85 12 __ STA P5 ; (v + 1)
0eda : a5 1d __ LDA ACCU + 2 
0edc : 85 13 __ STA P6 ; (v + 2)
0ede : a5 1e __ LDA ACCU + 3 
0ee0 : 85 14 __ STA P7 ; (v + 3)
0ee2 : c6 44 __ DEC T5 + 0 
0ee4 : aa __ __ TAX
0ee5 : d0 c2 __ BNE $0ea9 ; (nforml.l28 + 0)
0ee7 : 4c ce 0d JMP $0dce ; (nforml.s31 + 0)
--------------------------------------------------------------------
nformf: ; nformf(const struct sinfo*,u8*,float,u8)->u8
; 199, "C:/oscar64/include/stdio.c"
.s1:
0eea : a5 53 __ LDA T10 + 0 
0eec : 8d f1 9f STA $9ff1 ; (nformf@stack + 0)
0eef : a5 54 __ LDA T11 + 0 
0ef1 : 8d f2 9f STA $9ff2 ; (nformf@stack + 1)
.s4:
0ef4 : a5 11 __ LDA P4 ; (f + 0)
0ef6 : 85 43 __ STA T0 + 0 
0ef8 : a5 12 __ LDA P5 ; (f + 1)
0efa : 85 44 __ STA T0 + 1 
0efc : a5 14 __ LDA P7 ; (f + 3)
0efe : 29 7f __ AND #$7f
0f00 : 05 13 __ ORA P6 ; (f + 2)
0f02 : 05 12 __ ORA P5 ; (f + 1)
0f04 : a6 13 __ LDX P6 ; (f + 2)
0f06 : 86 45 __ STX T0 + 2 
0f08 : 05 11 __ ORA P4 ; (f + 0)
0f0a : f0 14 __ BEQ $0f20 ; (nformf.s5 + 0)
.s89:
0f0c : 24 14 __ BIT P7 ; (f + 3)
0f0e : 10 10 __ BPL $0f20 ; (nformf.s5 + 0)
.s88:
0f10 : a9 2d __ LDA #$2d
0f12 : a0 00 __ LDY #$00
0f14 : 91 0f __ STA (P2),y ; (str + 0)
0f16 : a5 14 __ LDA P7 ; (f + 3)
0f18 : 49 80 __ EOR #$80
0f1a : 85 14 __ STA P7 ; (f + 3)
.s87:
0f1c : a9 01 __ LDA #$01
0f1e : d0 0e __ BNE $0f2e ; (nformf.s6 + 0)
.s5:
0f20 : a0 05 __ LDY #$05
0f22 : b1 0d __ LDA (P0),y ; (si + 0)
0f24 : f0 08 __ BEQ $0f2e ; (nformf.s6 + 0)
.s86:
0f26 : a9 2b __ LDA #$2b
0f28 : a0 00 __ LDY #$00
0f2a : 91 0f __ STA (P2),y ; (str + 0)
0f2c : a9 01 __ LDA #$01
.s6:
0f2e : 85 52 __ STA T9 + 0 
0f30 : 8a __ __ TXA
0f31 : 0a __ __ ASL
0f32 : a5 14 __ LDA P7 ; (f + 3)
0f34 : 2a __ __ ROL
0f35 : c9 ff __ CMP #$ff
0f37 : d0 29 __ BNE $0f62 ; (nformf.s7 + 0)
.s85:
0f39 : a0 03 __ LDY #$03
0f3b : b1 0d __ LDA (P0),y ; (si + 0)
0f3d : 69 07 __ ADC #$07
0f3f : a4 52 __ LDY T9 + 0 
0f41 : 91 0f __ STA (P2),y ; (str + 0)
0f43 : 18 __ __ CLC
0f44 : a0 03 __ LDY #$03
0f46 : b1 0d __ LDA (P0),y ; (si + 0)
0f48 : 69 0d __ ADC #$0d
0f4a : a4 52 __ LDY T9 + 0 
0f4c : c8 __ __ INY
0f4d : 91 0f __ STA (P2),y ; (str + 0)
0f4f : 18 __ __ CLC
0f50 : a0 03 __ LDY #$03
0f52 : b1 0d __ LDA (P0),y ; (si + 0)
0f54 : 69 05 __ ADC #$05
0f56 : a4 52 __ LDY T9 + 0 
0f58 : c8 __ __ INY
0f59 : c8 __ __ INY
0f5a : 91 0f __ STA (P2),y ; (str + 0)
0f5c : c8 __ __ INY
0f5d : 84 52 __ STY T9 + 0 
0f5f : 4c 34 12 JMP $1234 ; (nformf.s27 + 0)
.s7:
0f62 : a0 02 __ LDY #$02
0f64 : b1 0d __ LDA (P0),y ; (si + 0)
0f66 : a6 14 __ LDX P7 ; (f + 3)
0f68 : 86 46 __ STX T0 + 3 
0f6a : c9 ff __ CMP #$ff
0f6c : d0 02 __ BNE $0f70 ; (nformf.s84 + 0)
.s8:
0f6e : a9 06 __ LDA #$06
.s84:
0f70 : 85 50 __ STA T7 + 0 
0f72 : a9 00 __ LDA #$00
0f74 : 85 4d __ STA T5 + 0 
0f76 : 85 4e __ STA T5 + 1 
0f78 : 8a __ __ TXA
0f79 : 29 7f __ AND #$7f
0f7b : 05 13 __ ORA P6 ; (f + 2)
0f7d : 05 12 __ ORA P5 ; (f + 1)
0f7f : 05 11 __ ORA P4 ; (f + 0)
0f81 : d0 03 __ BNE $0f86 ; (nformf.s63 + 0)
0f83 : 4c 58 10 JMP $1058 ; (nformf.s9 + 0)
.s63:
0f86 : 8a __ __ TXA
0f87 : 30 51 __ BMI $0fda ; (nformf.l72 + 0)
.s81:
0f89 : c9 44 __ CMP #$44
0f8b : d0 06 __ BNE $0f93 ; (nformf.l83 + 0)
.s82:
0f8d : a5 13 __ LDA P6 ; (f + 2)
0f8f : c9 7a __ CMP #$7a
0f91 : f0 02 __ BEQ $0f95 ; (nformf.l78 + 0)
.l83:
0f93 : 90 35 __ BCC $0fca ; (nformf.s64 + 0)
.l78:
0f95 : 18 __ __ CLC
0f96 : a5 4d __ LDA T5 + 0 
0f98 : 69 03 __ ADC #$03
0f9a : 85 4d __ STA T5 + 0 
0f9c : 90 02 __ BCC $0fa0 ; (nformf.s103 + 0)
.s102:
0f9e : e6 4e __ INC T5 + 1 
.s103:
0fa0 : a9 7a __ LDA #$7a
0fa2 : 85 05 __ STA WORK + 2 
0fa4 : a9 44 __ LDA #$44
0fa6 : 85 06 __ STA WORK + 3 
0fa8 : 20 12 19 JSR $1912 ; (freg@proxy + 0)
0fab : 20 a0 15 JSR $15a0 ; (crt_fdiv + 0)
0fae : a5 1b __ LDA ACCU + 0 
0fb0 : 85 43 __ STA T0 + 0 
0fb2 : a5 1c __ LDA ACCU + 1 
0fb4 : 85 44 __ STA T0 + 1 
0fb6 : a6 1d __ LDX ACCU + 2 
0fb8 : 86 45 __ STX T0 + 2 
0fba : a5 1e __ LDA ACCU + 3 
0fbc : 85 46 __ STA T0 + 3 
0fbe : 30 0a __ BMI $0fca ; (nformf.s64 + 0)
.s79:
0fc0 : c9 44 __ CMP #$44
0fc2 : d0 cf __ BNE $0f93 ; (nformf.l83 + 0)
.s80:
0fc4 : e0 7a __ CPX #$7a
0fc6 : f0 cd __ BEQ $0f95 ; (nformf.l78 + 0)
0fc8 : d0 c9 __ BNE $0f93 ; (nformf.l83 + 0)
.s64:
0fca : a5 46 __ LDA T0 + 3 
0fcc : 30 0c __ BMI $0fda ; (nformf.l72 + 0)
.s76:
0fce : c9 3f __ CMP #$3f
0fd0 : d0 06 __ BNE $0fd8 ; (nformf.s75 + 0)
.s77:
0fd2 : a5 45 __ LDA T0 + 2 
0fd4 : c9 80 __ CMP #$80
0fd6 : f0 40 __ BEQ $1018 ; (nformf.s65 + 0)
.s75:
0fd8 : b0 3e __ BCS $1018 ; (nformf.s65 + 0)
.l72:
0fda : 38 __ __ SEC
0fdb : a5 4d __ LDA T5 + 0 
0fdd : e9 03 __ SBC #$03
0fdf : 85 4d __ STA T5 + 0 
0fe1 : b0 02 __ BCS $0fe5 ; (nformf.s98 + 0)
.s97:
0fe3 : c6 4e __ DEC T5 + 1 
.s98:
0fe5 : a9 00 __ LDA #$00
0fe7 : 85 1b __ STA ACCU + 0 
0fe9 : 85 1c __ STA ACCU + 1 
0feb : a9 7a __ LDA #$7a
0fed : 85 1d __ STA ACCU + 2 
0fef : a9 44 __ LDA #$44
0ff1 : 85 1e __ STA ACCU + 3 
0ff3 : a2 43 __ LDX #$43
0ff5 : 20 aa 13 JSR $13aa ; (freg + 4)
0ff8 : 20 d8 14 JSR $14d8 ; (crt_fmul + 0)
0ffb : a5 1b __ LDA ACCU + 0 
0ffd : 85 43 __ STA T0 + 0 
0fff : a5 1c __ LDA ACCU + 1 
1001 : 85 44 __ STA T0 + 1 
1003 : a6 1d __ LDX ACCU + 2 
1005 : 86 45 __ STX T0 + 2 
1007 : a5 1e __ LDA ACCU + 3 
1009 : 85 46 __ STA T0 + 3 
100b : 30 cd __ BMI $0fda ; (nformf.l72 + 0)
.s73:
100d : c9 3f __ CMP #$3f
100f : 90 c9 __ BCC $0fda ; (nformf.l72 + 0)
.s106:
1011 : d0 05 __ BNE $1018 ; (nformf.s65 + 0)
.s74:
1013 : e0 80 __ CPX #$80
1015 : 4c d8 0f JMP $0fd8 ; (nformf.s75 + 0)
.s65:
1018 : a5 46 __ LDA T0 + 3 
101a : 30 3c __ BMI $1058 ; (nformf.s9 + 0)
.s69:
101c : c9 41 __ CMP #$41
101e : d0 06 __ BNE $1026 ; (nformf.l71 + 0)
.s70:
1020 : a5 45 __ LDA T0 + 2 
1022 : c9 20 __ CMP #$20
1024 : f0 02 __ BEQ $1028 ; (nformf.l66 + 0)
.l71:
1026 : 90 30 __ BCC $1058 ; (nformf.s9 + 0)
.l66:
1028 : e6 4d __ INC T5 + 0 
102a : d0 02 __ BNE $102e ; (nformf.s101 + 0)
.s100:
102c : e6 4e __ INC T5 + 1 
.s101:
102e : a9 20 __ LDA #$20
1030 : 85 05 __ STA WORK + 2 
1032 : a9 41 __ LDA #$41
1034 : 85 06 __ STA WORK + 3 
1036 : 20 12 19 JSR $1912 ; (freg@proxy + 0)
1039 : 20 a0 15 JSR $15a0 ; (crt_fdiv + 0)
103c : a5 1b __ LDA ACCU + 0 
103e : 85 43 __ STA T0 + 0 
1040 : a5 1c __ LDA ACCU + 1 
1042 : 85 44 __ STA T0 + 1 
1044 : a6 1d __ LDX ACCU + 2 
1046 : 86 45 __ STX T0 + 2 
1048 : a5 1e __ LDA ACCU + 3 
104a : 85 46 __ STA T0 + 3 
104c : 30 0a __ BMI $1058 ; (nformf.s9 + 0)
.s67:
104e : c9 41 __ CMP #$41
1050 : d0 d4 __ BNE $1026 ; (nformf.l71 + 0)
.s68:
1052 : e0 20 __ CPX #$20
1054 : f0 d2 __ BEQ $1028 ; (nformf.l66 + 0)
1056 : d0 ce __ BNE $1026 ; (nformf.l71 + 0)
.s9:
1058 : a5 15 __ LDA P8 ; (type + 0)
105a : c9 65 __ CMP #$65
105c : d0 04 __ BNE $1062 ; (nformf.s11 + 0)
.s10:
105e : a9 01 __ LDA #$01
1060 : d0 02 __ BNE $1064 ; (nformf.s12 + 0)
.s11:
1062 : a9 00 __ LDA #$00
.s12:
1064 : 85 53 __ STA T10 + 0 
1066 : a6 50 __ LDX T7 + 0 
1068 : e8 __ __ INX
1069 : 86 4f __ STX T6 + 0 
106b : a5 15 __ LDA P8 ; (type + 0)
106d : c9 67 __ CMP #$67
106f : d0 13 __ BNE $1084 ; (nformf.s13 + 0)
.s59:
1071 : a5 4e __ LDA T5 + 1 
1073 : 30 08 __ BMI $107d ; (nformf.s60 + 0)
.s62:
1075 : d0 06 __ BNE $107d ; (nformf.s60 + 0)
.s61:
1077 : a5 4d __ LDA T5 + 0 
1079 : c9 04 __ CMP #$04
107b : 90 07 __ BCC $1084 ; (nformf.s13 + 0)
.s60:
107d : a9 01 __ LDA #$01
107f : 85 53 __ STA T10 + 0 
1081 : 4c c1 12 JMP $12c1 ; (nformf.s51 + 0)
.s13:
1084 : a5 53 __ LDA T10 + 0 
1086 : d0 f9 __ BNE $1081 ; (nformf.s60 + 4)
.s14:
1088 : 24 4e __ BIT T5 + 1 
108a : 10 35 __ BPL $10c1 ; (nformf.s15 + 0)
.s50:
108c : a5 43 __ LDA T0 + 0 
108e : 85 1b __ STA ACCU + 0 
1090 : a5 44 __ LDA T0 + 1 
1092 : 85 1c __ STA ACCU + 1 
1094 : a5 45 __ LDA T0 + 2 
1096 : 85 1d __ STA ACCU + 2 
1098 : a5 46 __ LDA T0 + 3 
109a : 85 1e __ STA ACCU + 3 
.l90:
109c : 20 ff 18 JSR $18ff ; (freg@proxy + 0)
109f : 20 a0 15 JSR $15a0 ; (crt_fdiv + 0)
10a2 : 18 __ __ CLC
10a3 : a5 4d __ LDA T5 + 0 
10a5 : 69 01 __ ADC #$01
10a7 : 85 4d __ STA T5 + 0 
10a9 : a5 4e __ LDA T5 + 1 
10ab : 69 00 __ ADC #$00
10ad : 85 4e __ STA T5 + 1 
10af : 30 eb __ BMI $109c ; (nformf.l90 + 0)
.s91:
10b1 : a5 1e __ LDA ACCU + 3 
10b3 : 85 46 __ STA T0 + 3 
10b5 : a5 1d __ LDA ACCU + 2 
10b7 : 85 45 __ STA T0 + 2 
10b9 : a5 1c __ LDA ACCU + 1 
10bb : 85 44 __ STA T0 + 1 
10bd : a5 1b __ LDA ACCU + 0 
10bf : 85 43 __ STA T0 + 0 
.s15:
10c1 : 18 __ __ CLC
10c2 : a5 50 __ LDA T7 + 0 
10c4 : 65 4d __ ADC T5 + 0 
10c6 : 18 __ __ CLC
10c7 : 69 01 __ ADC #$01
10c9 : 85 4f __ STA T6 + 0 
10cb : c9 07 __ CMP #$07
10cd : 90 14 __ BCC $10e3 ; (nformf.s49 + 0)
.s16:
10cf : ad 59 19 LDA $1959 ; (fround5[0] + 24)
10d2 : 85 47 __ STA T1 + 0 
10d4 : ad 5a 19 LDA $195a ; (fround5[0] + 25)
10d7 : 85 48 __ STA T1 + 1 
10d9 : ad 5b 19 LDA $195b ; (fround5[0] + 26)
10dc : 85 49 __ STA T1 + 2 
10de : ad 5c 19 LDA $195c ; (fround5[0] + 27)
10e1 : b0 15 __ BCS $10f8 ; (nformf.s17 + 0)
.s49:
10e3 : 0a __ __ ASL
10e4 : 0a __ __ ASL
10e5 : aa __ __ TAX
10e6 : bd 3d 19 LDA $193d,x ; (freg@proxy + 16)
10e9 : 85 47 __ STA T1 + 0 
10eb : bd 3e 19 LDA $193e,x ; (freg@proxy + 17)
10ee : 85 48 __ STA T1 + 1 
10f0 : bd 3f 19 LDA $193f,x ; (freg@proxy + 18)
10f3 : 85 49 __ STA T1 + 2 
10f5 : bd 40 19 LDA $1940,x ; (spentry + 0)
.s17:
10f8 : 85 4a __ STA T1 + 3 
10fa : a2 47 __ LDX #$47
10fc : 20 2d 19 JSR $192d ; (freg@proxy + 0)
10ff : 20 f1 13 JSR $13f1 ; (faddsub + 6)
1102 : a5 1c __ LDA ACCU + 1 
1104 : 85 12 __ STA P5 ; (f + 1)
1106 : a5 1d __ LDA ACCU + 2 
1108 : 85 13 __ STA P6 ; (f + 2)
110a : a6 1b __ LDX ACCU + 0 
110c : a5 1e __ LDA ACCU + 3 
110e : 85 14 __ STA P7 ; (f + 3)
1110 : 30 22 __ BMI $1134 ; (nformf.s18 + 0)
.s46:
1112 : c9 41 __ CMP #$41
1114 : d0 06 __ BNE $111c ; (nformf.s48 + 0)
.s47:
1116 : a5 13 __ LDA P6 ; (f + 2)
1118 : c9 20 __ CMP #$20
111a : f0 02 __ BEQ $111e ; (nformf.s45 + 0)
.s48:
111c : 90 16 __ BCC $1134 ; (nformf.s18 + 0)
.s45:
111e : 20 ff 18 JSR $18ff ; (freg@proxy + 0)
1121 : 20 a0 15 JSR $15a0 ; (crt_fdiv + 0)
1124 : a5 1c __ LDA ACCU + 1 
1126 : 85 12 __ STA P5 ; (f + 1)
1128 : a5 1d __ LDA ACCU + 2 
112a : 85 13 __ STA P6 ; (f + 2)
112c : a5 1e __ LDA ACCU + 3 
112e : 85 14 __ STA P7 ; (f + 3)
1130 : c6 50 __ DEC T7 + 0 
1132 : a6 1b __ LDX ACCU + 0 
.s18:
1134 : 38 __ __ SEC
1135 : a5 4f __ LDA T6 + 0 
1137 : e5 50 __ SBC T7 + 0 
1139 : 85 4b __ STA T4 + 0 
113b : a5 4f __ LDA T6 + 0 
113d : c9 15 __ CMP #$15
113f : 90 04 __ BCC $1145 ; (nformf.s19 + 0)
.s44:
1141 : a9 14 __ LDA #$14
1143 : 85 4f __ STA T6 + 0 
.s19:
1145 : a5 4b __ LDA T4 + 0 
1147 : d0 08 __ BNE $1151 ; (nformf.s21 + 0)
.s20:
1149 : a9 30 __ LDA #$30
114b : a4 52 __ LDY T9 + 0 
114d : 91 0f __ STA (P2),y ; (str + 0)
114f : e6 52 __ INC T9 + 0 
.s21:
1151 : a9 00 __ LDA #$00
1153 : 85 54 __ STA T11 + 0 
1155 : c5 4b __ CMP T4 + 0 
1157 : f0 61 __ BEQ $11ba ; (nformf.l43 + 0)
.s23:
1159 : c9 07 __ CMP #$07
115b : 90 04 __ BCC $1161 ; (nformf.s24 + 0)
.l42:
115d : a9 30 __ LDA #$30
115f : b0 47 __ BCS $11a8 ; (nformf.l25 + 0)
.s24:
1161 : 86 1b __ STX ACCU + 0 
1163 : 86 43 __ STX T0 + 0 
1165 : a5 12 __ LDA P5 ; (f + 1)
1167 : 85 1c __ STA ACCU + 1 
1169 : 85 44 __ STA T0 + 1 
116b : a5 13 __ LDA P6 ; (f + 2)
116d : 85 1d __ STA ACCU + 2 
116f : 85 45 __ STA T0 + 2 
1171 : a5 14 __ LDA P7 ; (f + 3)
1173 : 85 1e __ STA ACCU + 3 
1175 : 85 46 __ STA T0 + 3 
1177 : 20 67 17 JSR $1767 ; (f32_to_i16 + 0)
117a : a5 1b __ LDA ACCU + 0 
117c : 85 51 __ STA T8 + 0 
117e : 20 b3 17 JSR $17b3 ; (sint16_to_float + 0)
1181 : a2 43 __ LDX #$43
1183 : 20 aa 13 JSR $13aa ; (freg + 4)
1186 : a5 1e __ LDA ACCU + 3 
1188 : 49 80 __ EOR #$80
118a : 85 1e __ STA ACCU + 3 
118c : 20 f1 13 JSR $13f1 ; (faddsub + 6)
118f : 20 ff 18 JSR $18ff ; (freg@proxy + 0)
1192 : 20 d8 14 JSR $14d8 ; (crt_fmul + 0)
1195 : a5 1c __ LDA ACCU + 1 
1197 : 85 12 __ STA P5 ; (f + 1)
1199 : a5 1d __ LDA ACCU + 2 
119b : 85 13 __ STA P6 ; (f + 2)
119d : a5 1e __ LDA ACCU + 3 
119f : 85 14 __ STA P7 ; (f + 3)
11a1 : 18 __ __ CLC
11a2 : a5 51 __ LDA T8 + 0 
11a4 : 69 30 __ ADC #$30
11a6 : a6 1b __ LDX ACCU + 0 
.l25:
11a8 : a4 52 __ LDY T9 + 0 
11aa : 91 0f __ STA (P2),y ; (str + 0)
11ac : e6 52 __ INC T9 + 0 
11ae : e6 54 __ INC T11 + 0 
11b0 : a5 54 __ LDA T11 + 0 
11b2 : c5 4f __ CMP T6 + 0 
11b4 : b0 14 __ BCS $11ca ; (nformf.s26 + 0)
.s22:
11b6 : c5 4b __ CMP T4 + 0 
11b8 : d0 9f __ BNE $1159 ; (nformf.s23 + 0)
.l43:
11ba : a9 2e __ LDA #$2e
11bc : a4 52 __ LDY T9 + 0 
11be : 91 0f __ STA (P2),y ; (str + 0)
11c0 : a5 54 __ LDA T11 + 0 
11c2 : c9 07 __ CMP #$07
11c4 : e6 52 __ INC T9 + 0 
11c6 : 90 99 __ BCC $1161 ; (nformf.s24 + 0)
11c8 : b0 93 __ BCS $115d ; (nformf.l42 + 0)
.s26:
11ca : a5 53 __ LDA T10 + 0 
11cc : f0 66 __ BEQ $1234 ; (nformf.s27 + 0)
.s38:
11ce : a0 03 __ LDY #$03
11d0 : b1 0d __ LDA (P0),y ; (si + 0)
11d2 : 69 03 __ ADC #$03
11d4 : a4 52 __ LDY T9 + 0 
11d6 : 91 0f __ STA (P2),y ; (str + 0)
11d8 : c8 __ __ INY
11d9 : 84 52 __ STY T9 + 0 
11db : 24 4e __ BIT T5 + 1 
11dd : 30 06 __ BMI $11e5 ; (nformf.s41 + 0)
.s39:
11df : a9 2b __ LDA #$2b
11e1 : 91 0f __ STA (P2),y ; (str + 0)
11e3 : d0 11 __ BNE $11f6 ; (nformf.s40 + 0)
.s41:
11e5 : a9 2d __ LDA #$2d
11e7 : 91 0f __ STA (P2),y ; (str + 0)
11e9 : 38 __ __ SEC
11ea : a9 00 __ LDA #$00
11ec : e5 4d __ SBC T5 + 0 
11ee : 85 4d __ STA T5 + 0 
11f0 : a9 00 __ LDA #$00
11f2 : e5 4e __ SBC T5 + 1 
11f4 : 85 4e __ STA T5 + 1 
.s40:
11f6 : a5 4d __ LDA T5 + 0 
11f8 : 85 1b __ STA ACCU + 0 
11fa : a5 4e __ LDA T5 + 1 
11fc : 85 1c __ STA ACCU + 1 
11fe : e6 52 __ INC T9 + 0 
1200 : a9 0a __ LDA #$0a
1202 : 85 03 __ STA WORK + 0 
1204 : a9 00 __ LDA #$00
1206 : 85 04 __ STA WORK + 1 
1208 : 20 4e 16 JSR $164e ; (divs16 + 0)
120b : 18 __ __ CLC
120c : a5 1b __ LDA ACCU + 0 
120e : 69 30 __ ADC #$30
1210 : a4 52 __ LDY T9 + 0 
1212 : 91 0f __ STA (P2),y ; (str + 0)
1214 : a5 4d __ LDA T5 + 0 
1216 : 85 1b __ STA ACCU + 0 
1218 : a5 4e __ LDA T5 + 1 
121a : 85 1c __ STA ACCU + 1 
121c : e6 52 __ INC T9 + 0 
121e : a9 0a __ LDA #$0a
1220 : 85 03 __ STA WORK + 0 
1222 : a9 00 __ LDA #$00
1224 : 85 04 __ STA WORK + 1 
1226 : 20 10 17 JSR $1710 ; (mods16 + 0)
1229 : 18 __ __ CLC
122a : a5 05 __ LDA WORK + 2 
122c : 69 30 __ ADC #$30
122e : a4 52 __ LDY T9 + 0 
1230 : 91 0f __ STA (P2),y ; (str + 0)
1232 : e6 52 __ INC T9 + 0 
.s27:
1234 : a5 52 __ LDA T9 + 0 
1236 : a0 01 __ LDY #$01
1238 : d1 0d __ CMP (P0),y ; (si + 0)
123a : b0 69 __ BCS $12a5 ; (nformf.s3 + 0)
.s28:
123c : a0 06 __ LDY #$06
123e : b1 0d __ LDA (P0),y ; (si + 0)
1240 : f0 04 __ BEQ $1246 ; (nformf.s29 + 0)
.s92:
1242 : a6 52 __ LDX T9 + 0 
1244 : 90 6c __ BCC $12b2 ; (nformf.l36 + 0)
.s29:
1246 : a5 52 __ LDA T9 + 0 
1248 : f0 3c __ BEQ $1286 ; (nformf.s30 + 0)
.s35:
124a : e9 00 __ SBC #$00
124c : a2 00 __ LDX #$00
124e : b0 01 __ BCS $1251 ; (nformf.s105 + 0)
.s104:
1250 : ca __ __ DEX
.s105:
1251 : 18 __ __ CLC
1252 : 65 0f __ ADC P2 ; (str + 0)
1254 : 85 43 __ STA T0 + 0 
1256 : 8a __ __ TXA
1257 : 65 10 __ ADC P3 ; (str + 1)
1259 : 85 44 __ STA T0 + 1 
125b : a9 01 __ LDA #$01
125d : 85 47 __ STA T1 + 0 
125f : a6 10 __ LDX P3 ; (str + 1)
1261 : 38 __ __ SEC
.l93:
1262 : a0 01 __ LDY #$01
1264 : b1 0d __ LDA (P0),y ; (si + 0)
1266 : e5 47 __ SBC T1 + 0 
1268 : 85 4b __ STA T4 + 0 
126a : 8a __ __ TXA
126b : 69 ff __ ADC #$ff
126d : 85 4c __ STA T4 + 1 
126f : 88 __ __ DEY
1270 : b1 43 __ LDA (T0 + 0),y 
1272 : a4 0f __ LDY P2 ; (str + 0)
1274 : 91 4b __ STA (T4 + 0),y 
1276 : a5 43 __ LDA T0 + 0 
1278 : d0 02 __ BNE $127c ; (nformf.s96 + 0)
.s95:
127a : c6 44 __ DEC T0 + 1 
.s96:
127c : c6 43 __ DEC T0 + 0 
127e : e6 47 __ INC T1 + 0 
1280 : a5 52 __ LDA T9 + 0 
1282 : c5 47 __ CMP T1 + 0 
1284 : b0 dc __ BCS $1262 ; (nformf.l93 + 0)
.s30:
1286 : a9 00 __ LDA #$00
1288 : 85 4b __ STA T4 + 0 
128a : 90 08 __ BCC $1294 ; (nformf.l31 + 0)
.s33:
128c : a9 20 __ LDA #$20
128e : a4 4b __ LDY T4 + 0 
1290 : 91 0f __ STA (P2),y ; (str + 0)
1292 : e6 4b __ INC T4 + 0 
.l31:
1294 : a0 01 __ LDY #$01
1296 : b1 0d __ LDA (P0),y ; (si + 0)
1298 : 38 __ __ SEC
1299 : e5 52 __ SBC T9 + 0 
129b : 90 ef __ BCC $128c ; (nformf.s33 + 0)
.s34:
129d : c5 4b __ CMP T4 + 0 
129f : 90 02 __ BCC $12a3 ; (nformf.s32 + 0)
.s94:
12a1 : d0 e9 __ BNE $128c ; (nformf.s33 + 0)
.s32:
12a3 : b1 0d __ LDA (P0),y ; (si + 0)
.s3:
12a5 : 85 1b __ STA ACCU + 0 
12a7 : ad f1 9f LDA $9ff1 ; (nformf@stack + 0)
12aa : 85 53 __ STA T10 + 0 
12ac : ad f2 9f LDA $9ff2 ; (nformf@stack + 1)
12af : 85 54 __ STA T11 + 0 
12b1 : 60 __ __ RTS
.l36:
12b2 : 8a __ __ TXA
12b3 : a0 01 __ LDY #$01
12b5 : d1 0d __ CMP (P0),y ; (si + 0)
12b7 : b0 ea __ BCS $12a3 ; (nformf.s32 + 0)
.s37:
12b9 : a8 __ __ TAY
12ba : a9 20 __ LDA #$20
12bc : 91 0f __ STA (P2),y ; (str + 0)
12be : e8 __ __ INX
12bf : 90 f1 __ BCC $12b2 ; (nformf.l36 + 0)
.s51:
12c1 : a5 4f __ LDA T6 + 0 
12c3 : c9 07 __ CMP #$07
12c5 : 90 14 __ BCC $12db ; (nformf.s58 + 0)
.s52:
12c7 : ad 59 19 LDA $1959 ; (fround5[0] + 24)
12ca : 85 47 __ STA T1 + 0 
12cc : ad 5a 19 LDA $195a ; (fround5[0] + 25)
12cf : 85 48 __ STA T1 + 1 
12d1 : ad 5b 19 LDA $195b ; (fround5[0] + 26)
12d4 : 85 49 __ STA T1 + 2 
12d6 : ad 5c 19 LDA $195c ; (fround5[0] + 27)
12d9 : b0 15 __ BCS $12f0 ; (nformf.s53 + 0)
.s58:
12db : 0a __ __ ASL
12dc : 0a __ __ ASL
12dd : aa __ __ TAX
12de : bd 3d 19 LDA $193d,x ; (freg@proxy + 16)
12e1 : 85 47 __ STA T1 + 0 
12e3 : bd 3e 19 LDA $193e,x ; (freg@proxy + 17)
12e6 : 85 48 __ STA T1 + 1 
12e8 : bd 3f 19 LDA $193f,x ; (freg@proxy + 18)
12eb : 85 49 __ STA T1 + 2 
12ed : bd 40 19 LDA $1940,x ; (spentry + 0)
.s53:
12f0 : 85 4a __ STA T1 + 3 
12f2 : a2 47 __ LDX #$47
12f4 : 20 2d 19 JSR $192d ; (freg@proxy + 0)
12f7 : 20 f1 13 JSR $13f1 ; (faddsub + 6)
12fa : a5 1c __ LDA ACCU + 1 
12fc : 85 12 __ STA P5 ; (f + 1)
12fe : a5 1d __ LDA ACCU + 2 
1300 : 85 13 __ STA P6 ; (f + 2)
1302 : a6 1b __ LDX ACCU + 0 
1304 : a5 1e __ LDA ACCU + 3 
1306 : 85 14 __ STA P7 ; (f + 3)
1308 : 10 03 __ BPL $130d ; (nformf.s55 + 0)
130a : 4c 34 11 JMP $1134 ; (nformf.s18 + 0)
.s55:
130d : c9 41 __ CMP #$41
130f : d0 06 __ BNE $1317 ; (nformf.s57 + 0)
.s56:
1311 : a5 13 __ LDA P6 ; (f + 2)
1313 : c9 20 __ CMP #$20
1315 : f0 02 __ BEQ $1319 ; (nformf.s54 + 0)
.s57:
1317 : 90 f1 __ BCC $130a ; (nformf.s53 + 26)
.s54:
1319 : 20 ff 18 JSR $18ff ; (freg@proxy + 0)
131c : 20 a0 15 JSR $15a0 ; (crt_fdiv + 0)
131f : a5 1c __ LDA ACCU + 1 
1321 : 85 12 __ STA P5 ; (f + 1)
1323 : a5 1d __ LDA ACCU + 2 
1325 : 85 13 __ STA P6 ; (f + 2)
1327 : a5 1e __ LDA ACCU + 3 
1329 : 85 14 __ STA P7 ; (f + 3)
132b : a6 1b __ LDX ACCU + 0 
132d : e6 4d __ INC T5 + 0 
132f : d0 d9 __ BNE $130a ; (nformf.s53 + 26)
.s99:
1331 : e6 4e __ INC T5 + 1 
1333 : 4c 34 11 JMP $1134 ; (nformf.s18 + 0)
--------------------------------------------------------------------
1336 : __ __ __ BYT 25 64 3a 20 25 64 0a 00                         : %d: %d..
--------------------------------------------------------------------
RN_asm: ; RN_asm()->u16
;  11, "C:/Users/korvus/Projects/C64/project_rogue_C64/src/demo/platform.h"
.s4:
133e : a9 00 __ LDA #$00
1340 : 85 03 __ STA WORK + 0 
1342 : 85 04 __ STA WORK + 1 
1344 : 85 05 __ STA WORK + 2 
1346 : 85 06 __ STA WORK + 3 
1348 : a9 65 __ LDA #$65
134a : 85 07 __ STA WORK + 4 
134c : a9 2b __ LDA #$2b
134e : 85 08 __ STA WORK + 5 
1350 : a0 0e __ LDY #$0e
1352 : 46 08 __ LSR WORK + 5 
1354 : 66 07 __ ROR WORK + 4 
1356 : 90 1d __ BCC $1375 ; (RN_asm.s4 + 55)
1358 : 18 __ __ CLC
1359 : a5 03 __ LDA WORK + 0 
135b : 6d 5d 19 ADC $195d ; (seed + 0)
135e : 85 03 __ STA WORK + 0 
1360 : a5 04 __ LDA WORK + 1 
1362 : 6d 5e 19 ADC $195e ; (seed + 1)
1365 : 85 04 __ STA WORK + 1 
1367 : a5 05 __ LDA WORK + 2 
1369 : 6d 5f 19 ADC $195f ; (seed + 2)
136c : 85 05 __ STA WORK + 2 
136e : a5 06 __ LDA WORK + 3 
1370 : 6d 60 19 ADC $1960 ; (seed + 3)
1373 : 85 06 __ STA WORK + 3 
1375 : 0e 5d 19 ASL $195d ; (seed + 0)
1378 : 2e 5e 19 ROL $195e ; (seed + 1)
137b : 2e 5f 19 ROL $195f ; (seed + 2)
137e : 2e 60 19 ROL $1960 ; (seed + 3)
1381 : 88 __ __ DEY
1382 : d0 ce __ BNE $1352 ; (RN_asm.s4 + 20)
1384 : 18 __ __ CLC
1385 : a5 03 __ LDA WORK + 0 
1387 : 69 19 __ ADC #$19
1389 : 8d 5d 19 STA $195d ; (seed + 0)
138c : a5 04 __ LDA WORK + 1 
138e : 69 36 __ ADC #$36
1390 : 8d 5e 19 STA $195e ; (seed + 1)
1393 : a5 05 __ LDA WORK + 2 
1395 : 69 00 __ ADC #$00
1397 : 8d 5f 19 STA $195f ; (seed + 2)
139a : 85 1b __ STA ACCU + 0 
139c : a5 06 __ LDA WORK + 3 
139e : 69 00 __ ADC #$00
13a0 : 8d 60 19 STA $1960 ; (seed + 3)
13a3 : 85 1c __ STA ACCU + 1 
.s3:
13a5 : 60 __ __ RTS
--------------------------------------------------------------------
freg: ; freg
13a6 : b1 19 __ LDA (IP + 0),y 
13a8 : c8 __ __ INY
13a9 : aa __ __ TAX
13aa : b5 00 __ LDA $00,x 
13ac : 85 03 __ STA WORK + 0 
13ae : b5 01 __ LDA $01,x 
13b0 : 85 04 __ STA WORK + 1 
13b2 : b5 02 __ LDA $02,x 
13b4 : 85 05 __ STA WORK + 2 
13b6 : b5 03 __ LDA WORK + 0,x 
13b8 : 85 06 __ STA WORK + 3 
13ba : a5 05 __ LDA WORK + 2 
13bc : 0a __ __ ASL
13bd : a5 06 __ LDA WORK + 3 
13bf : 2a __ __ ROL
13c0 : 85 08 __ STA WORK + 5 
13c2 : f0 06 __ BEQ $13ca ; (freg + 36)
13c4 : a5 05 __ LDA WORK + 2 
13c6 : 09 80 __ ORA #$80
13c8 : 85 05 __ STA WORK + 2 
13ca : a5 1d __ LDA ACCU + 2 
13cc : 0a __ __ ASL
13cd : a5 1e __ LDA ACCU + 3 
13cf : 2a __ __ ROL
13d0 : 85 07 __ STA WORK + 4 
13d2 : f0 06 __ BEQ $13da ; (freg + 52)
13d4 : a5 1d __ LDA ACCU + 2 
13d6 : 09 80 __ ORA #$80
13d8 : 85 1d __ STA ACCU + 2 
13da : 60 __ __ RTS
13db : 06 1e __ ASL ACCU + 3 
13dd : a5 07 __ LDA WORK + 4 
13df : 6a __ __ ROR
13e0 : 85 1e __ STA ACCU + 3 
13e2 : b0 06 __ BCS $13ea ; (freg + 68)
13e4 : a5 1d __ LDA ACCU + 2 
13e6 : 29 7f __ AND #$7f
13e8 : 85 1d __ STA ACCU + 2 
13ea : 60 __ __ RTS
--------------------------------------------------------------------
faddsub: ; faddsub
13eb : a5 06 __ LDA WORK + 3 
13ed : 49 80 __ EOR #$80
13ef : 85 06 __ STA WORK + 3 
13f1 : a9 ff __ LDA #$ff
13f3 : c5 07 __ CMP WORK + 4 
13f5 : f0 04 __ BEQ $13fb ; (faddsub + 16)
13f7 : c5 08 __ CMP WORK + 5 
13f9 : d0 11 __ BNE $140c ; (faddsub + 33)
13fb : a5 1e __ LDA ACCU + 3 
13fd : 09 7f __ ORA #$7f
13ff : 85 1e __ STA ACCU + 3 
1401 : a9 80 __ LDA #$80
1403 : 85 1d __ STA ACCU + 2 
1405 : a9 00 __ LDA #$00
1407 : 85 1b __ STA ACCU + 0 
1409 : 85 1c __ STA ACCU + 1 
140b : 60 __ __ RTS
140c : 38 __ __ SEC
140d : a5 07 __ LDA WORK + 4 
140f : e5 08 __ SBC WORK + 5 
1411 : f0 38 __ BEQ $144b ; (faddsub + 96)
1413 : aa __ __ TAX
1414 : b0 25 __ BCS $143b ; (faddsub + 80)
1416 : e0 e9 __ CPX #$e9
1418 : b0 0e __ BCS $1428 ; (faddsub + 61)
141a : a5 08 __ LDA WORK + 5 
141c : 85 07 __ STA WORK + 4 
141e : a9 00 __ LDA #$00
1420 : 85 1b __ STA ACCU + 0 
1422 : 85 1c __ STA ACCU + 1 
1424 : 85 1d __ STA ACCU + 2 
1426 : f0 23 __ BEQ $144b ; (faddsub + 96)
1428 : a5 1d __ LDA ACCU + 2 
142a : 4a __ __ LSR
142b : 66 1c __ ROR ACCU + 1 
142d : 66 1b __ ROR ACCU + 0 
142f : e8 __ __ INX
1430 : d0 f8 __ BNE $142a ; (faddsub + 63)
1432 : 85 1d __ STA ACCU + 2 
1434 : a5 08 __ LDA WORK + 5 
1436 : 85 07 __ STA WORK + 4 
1438 : 4c 4b 14 JMP $144b ; (faddsub + 96)
143b : e0 18 __ CPX #$18
143d : b0 33 __ BCS $1472 ; (faddsub + 135)
143f : a5 05 __ LDA WORK + 2 
1441 : 4a __ __ LSR
1442 : 66 04 __ ROR WORK + 1 
1444 : 66 03 __ ROR WORK + 0 
1446 : ca __ __ DEX
1447 : d0 f8 __ BNE $1441 ; (faddsub + 86)
1449 : 85 05 __ STA WORK + 2 
144b : a5 1e __ LDA ACCU + 3 
144d : 29 80 __ AND #$80
144f : 85 1e __ STA ACCU + 3 
1451 : 45 06 __ EOR WORK + 3 
1453 : 30 31 __ BMI $1486 ; (faddsub + 155)
1455 : 18 __ __ CLC
1456 : a5 1b __ LDA ACCU + 0 
1458 : 65 03 __ ADC WORK + 0 
145a : 85 1b __ STA ACCU + 0 
145c : a5 1c __ LDA ACCU + 1 
145e : 65 04 __ ADC WORK + 1 
1460 : 85 1c __ STA ACCU + 1 
1462 : a5 1d __ LDA ACCU + 2 
1464 : 65 05 __ ADC WORK + 2 
1466 : 85 1d __ STA ACCU + 2 
1468 : 90 08 __ BCC $1472 ; (faddsub + 135)
146a : 66 1d __ ROR ACCU + 2 
146c : 66 1c __ ROR ACCU + 1 
146e : 66 1b __ ROR ACCU + 0 
1470 : e6 07 __ INC WORK + 4 
1472 : a5 07 __ LDA WORK + 4 
1474 : c9 ff __ CMP #$ff
1476 : f0 83 __ BEQ $13fb ; (faddsub + 16)
1478 : 4a __ __ LSR
1479 : 05 1e __ ORA ACCU + 3 
147b : 85 1e __ STA ACCU + 3 
147d : b0 06 __ BCS $1485 ; (faddsub + 154)
147f : a5 1d __ LDA ACCU + 2 
1481 : 29 7f __ AND #$7f
1483 : 85 1d __ STA ACCU + 2 
1485 : 60 __ __ RTS
1486 : 38 __ __ SEC
1487 : a5 1b __ LDA ACCU + 0 
1489 : e5 03 __ SBC WORK + 0 
148b : 85 1b __ STA ACCU + 0 
148d : a5 1c __ LDA ACCU + 1 
148f : e5 04 __ SBC WORK + 1 
1491 : 85 1c __ STA ACCU + 1 
1493 : a5 1d __ LDA ACCU + 2 
1495 : e5 05 __ SBC WORK + 2 
1497 : 85 1d __ STA ACCU + 2 
1499 : b0 19 __ BCS $14b4 ; (faddsub + 201)
149b : 38 __ __ SEC
149c : a9 00 __ LDA #$00
149e : e5 1b __ SBC ACCU + 0 
14a0 : 85 1b __ STA ACCU + 0 
14a2 : a9 00 __ LDA #$00
14a4 : e5 1c __ SBC ACCU + 1 
14a6 : 85 1c __ STA ACCU + 1 
14a8 : a9 00 __ LDA #$00
14aa : e5 1d __ SBC ACCU + 2 
14ac : 85 1d __ STA ACCU + 2 
14ae : a5 1e __ LDA ACCU + 3 
14b0 : 49 80 __ EOR #$80
14b2 : 85 1e __ STA ACCU + 3 
14b4 : a5 1d __ LDA ACCU + 2 
14b6 : 30 ba __ BMI $1472 ; (faddsub + 135)
14b8 : 05 1c __ ORA ACCU + 1 
14ba : 05 1b __ ORA ACCU + 0 
14bc : f0 0f __ BEQ $14cd ; (faddsub + 226)
14be : c6 07 __ DEC WORK + 4 
14c0 : f0 0b __ BEQ $14cd ; (faddsub + 226)
14c2 : 06 1b __ ASL ACCU + 0 
14c4 : 26 1c __ ROL ACCU + 1 
14c6 : 26 1d __ ROL ACCU + 2 
14c8 : 10 f4 __ BPL $14be ; (faddsub + 211)
14ca : 4c 72 14 JMP $1472 ; (faddsub + 135)
14cd : a9 00 __ LDA #$00
14cf : 85 1b __ STA ACCU + 0 
14d1 : 85 1c __ STA ACCU + 1 
14d3 : 85 1d __ STA ACCU + 2 
14d5 : 85 1e __ STA ACCU + 3 
14d7 : 60 __ __ RTS
--------------------------------------------------------------------
crt_fmul: ; crt_fmul
14d8 : a5 1b __ LDA ACCU + 0 
14da : 05 1c __ ORA ACCU + 1 
14dc : 05 1d __ ORA ACCU + 2 
14de : f0 0e __ BEQ $14ee ; (crt_fmul + 22)
14e0 : a5 03 __ LDA WORK + 0 
14e2 : 05 04 __ ORA WORK + 1 
14e4 : 05 05 __ ORA WORK + 2 
14e6 : d0 09 __ BNE $14f1 ; (crt_fmul + 25)
14e8 : 85 1b __ STA ACCU + 0 
14ea : 85 1c __ STA ACCU + 1 
14ec : 85 1d __ STA ACCU + 2 
14ee : 85 1e __ STA ACCU + 3 
14f0 : 60 __ __ RTS
14f1 : a5 1e __ LDA ACCU + 3 
14f3 : 45 06 __ EOR WORK + 3 
14f5 : 29 80 __ AND #$80
14f7 : 85 1e __ STA ACCU + 3 
14f9 : a9 ff __ LDA #$ff
14fb : c5 07 __ CMP WORK + 4 
14fd : f0 42 __ BEQ $1541 ; (crt_fmul + 105)
14ff : c5 08 __ CMP WORK + 5 
1501 : f0 3e __ BEQ $1541 ; (crt_fmul + 105)
1503 : a9 00 __ LDA #$00
1505 : 85 09 __ STA WORK + 6 
1507 : 85 0a __ STA WORK + 7 
1509 : 85 0b __ STA WORK + 8 
150b : a4 1b __ LDY ACCU + 0 
150d : a5 03 __ LDA WORK + 0 
150f : d0 06 __ BNE $1517 ; (crt_fmul + 63)
1511 : a5 04 __ LDA WORK + 1 
1513 : f0 0a __ BEQ $151f ; (crt_fmul + 71)
1515 : d0 05 __ BNE $151c ; (crt_fmul + 68)
1517 : 20 72 15 JSR $1572 ; (crt_fmul8 + 0)
151a : a5 04 __ LDA WORK + 1 
151c : 20 72 15 JSR $1572 ; (crt_fmul8 + 0)
151f : a5 05 __ LDA WORK + 2 
1521 : 20 72 15 JSR $1572 ; (crt_fmul8 + 0)
1524 : 38 __ __ SEC
1525 : a5 0b __ LDA WORK + 8 
1527 : 30 06 __ BMI $152f ; (crt_fmul + 87)
1529 : 06 09 __ ASL WORK + 6 
152b : 26 0a __ ROL WORK + 7 
152d : 2a __ __ ROL
152e : 18 __ __ CLC
152f : 29 7f __ AND #$7f
1531 : 85 0b __ STA WORK + 8 
1533 : a5 07 __ LDA WORK + 4 
1535 : 65 08 __ ADC WORK + 5 
1537 : 90 19 __ BCC $1552 ; (crt_fmul + 122)
1539 : e9 7f __ SBC #$7f
153b : b0 04 __ BCS $1541 ; (crt_fmul + 105)
153d : c9 ff __ CMP #$ff
153f : d0 15 __ BNE $1556 ; (crt_fmul + 126)
1541 : a5 1e __ LDA ACCU + 3 
1543 : 09 7f __ ORA #$7f
1545 : 85 1e __ STA ACCU + 3 
1547 : a9 80 __ LDA #$80
1549 : 85 1d __ STA ACCU + 2 
154b : a9 00 __ LDA #$00
154d : 85 1b __ STA ACCU + 0 
154f : 85 1c __ STA ACCU + 1 
1551 : 60 __ __ RTS
1552 : e9 7e __ SBC #$7e
1554 : 90 15 __ BCC $156b ; (crt_fmul + 147)
1556 : 4a __ __ LSR
1557 : 05 1e __ ORA ACCU + 3 
1559 : 85 1e __ STA ACCU + 3 
155b : a9 00 __ LDA #$00
155d : 6a __ __ ROR
155e : 05 0b __ ORA WORK + 8 
1560 : 85 1d __ STA ACCU + 2 
1562 : a5 0a __ LDA WORK + 7 
1564 : 85 1c __ STA ACCU + 1 
1566 : a5 09 __ LDA WORK + 6 
1568 : 85 1b __ STA ACCU + 0 
156a : 60 __ __ RTS
156b : a9 00 __ LDA #$00
156d : 85 1e __ STA ACCU + 3 
156f : f0 d8 __ BEQ $1549 ; (crt_fmul + 113)
1571 : 60 __ __ RTS
--------------------------------------------------------------------
crt_fmul8: ; crt_fmul8
1572 : 38 __ __ SEC
1573 : 6a __ __ ROR
1574 : 90 1e __ BCC $1594 ; (crt_fmul8 + 34)
1576 : aa __ __ TAX
1577 : 18 __ __ CLC
1578 : 98 __ __ TYA
1579 : 65 09 __ ADC WORK + 6 
157b : 85 09 __ STA WORK + 6 
157d : a5 0a __ LDA WORK + 7 
157f : 65 1c __ ADC ACCU + 1 
1581 : 85 0a __ STA WORK + 7 
1583 : a5 0b __ LDA WORK + 8 
1585 : 65 1d __ ADC ACCU + 2 
1587 : 6a __ __ ROR
1588 : 85 0b __ STA WORK + 8 
158a : 8a __ __ TXA
158b : 66 0a __ ROR WORK + 7 
158d : 66 09 __ ROR WORK + 6 
158f : 4a __ __ LSR
1590 : f0 0d __ BEQ $159f ; (crt_fmul8 + 45)
1592 : b0 e2 __ BCS $1576 ; (crt_fmul8 + 4)
1594 : 66 0b __ ROR WORK + 8 
1596 : 66 0a __ ROR WORK + 7 
1598 : 66 09 __ ROR WORK + 6 
159a : 4a __ __ LSR
159b : 90 f7 __ BCC $1594 ; (crt_fmul8 + 34)
159d : d0 d7 __ BNE $1576 ; (crt_fmul8 + 4)
159f : 60 __ __ RTS
--------------------------------------------------------------------
crt_fdiv: ; crt_fdiv
15a0 : a5 1b __ LDA ACCU + 0 
15a2 : 05 1c __ ORA ACCU + 1 
15a4 : 05 1d __ ORA ACCU + 2 
15a6 : d0 03 __ BNE $15ab ; (crt_fdiv + 11)
15a8 : 85 1e __ STA ACCU + 3 
15aa : 60 __ __ RTS
15ab : a5 1e __ LDA ACCU + 3 
15ad : 45 06 __ EOR WORK + 3 
15af : 29 80 __ AND #$80
15b1 : 85 1e __ STA ACCU + 3 
15b3 : a5 08 __ LDA WORK + 5 
15b5 : f0 62 __ BEQ $1619 ; (crt_fdiv + 121)
15b7 : a5 07 __ LDA WORK + 4 
15b9 : c9 ff __ CMP #$ff
15bb : f0 5c __ BEQ $1619 ; (crt_fdiv + 121)
15bd : a9 00 __ LDA #$00
15bf : 85 09 __ STA WORK + 6 
15c1 : 85 0a __ STA WORK + 7 
15c3 : 85 0b __ STA WORK + 8 
15c5 : a2 18 __ LDX #$18
15c7 : a5 1b __ LDA ACCU + 0 
15c9 : c5 03 __ CMP WORK + 0 
15cb : a5 1c __ LDA ACCU + 1 
15cd : e5 04 __ SBC WORK + 1 
15cf : a5 1d __ LDA ACCU + 2 
15d1 : e5 05 __ SBC WORK + 2 
15d3 : 90 13 __ BCC $15e8 ; (crt_fdiv + 72)
15d5 : a5 1b __ LDA ACCU + 0 
15d7 : e5 03 __ SBC WORK + 0 
15d9 : 85 1b __ STA ACCU + 0 
15db : a5 1c __ LDA ACCU + 1 
15dd : e5 04 __ SBC WORK + 1 
15df : 85 1c __ STA ACCU + 1 
15e1 : a5 1d __ LDA ACCU + 2 
15e3 : e5 05 __ SBC WORK + 2 
15e5 : 85 1d __ STA ACCU + 2 
15e7 : 38 __ __ SEC
15e8 : 26 09 __ ROL WORK + 6 
15ea : 26 0a __ ROL WORK + 7 
15ec : 26 0b __ ROL WORK + 8 
15ee : ca __ __ DEX
15ef : f0 0a __ BEQ $15fb ; (crt_fdiv + 91)
15f1 : 06 1b __ ASL ACCU + 0 
15f3 : 26 1c __ ROL ACCU + 1 
15f5 : 26 1d __ ROL ACCU + 2 
15f7 : b0 dc __ BCS $15d5 ; (crt_fdiv + 53)
15f9 : 90 cc __ BCC $15c7 ; (crt_fdiv + 39)
15fb : 38 __ __ SEC
15fc : a5 0b __ LDA WORK + 8 
15fe : 30 06 __ BMI $1606 ; (crt_fdiv + 102)
1600 : 06 09 __ ASL WORK + 6 
1602 : 26 0a __ ROL WORK + 7 
1604 : 2a __ __ ROL
1605 : 18 __ __ CLC
1606 : 29 7f __ AND #$7f
1608 : 85 0b __ STA WORK + 8 
160a : a5 07 __ LDA WORK + 4 
160c : e5 08 __ SBC WORK + 5 
160e : 90 1a __ BCC $162a ; (crt_fdiv + 138)
1610 : 18 __ __ CLC
1611 : 69 7f __ ADC #$7f
1613 : b0 04 __ BCS $1619 ; (crt_fdiv + 121)
1615 : c9 ff __ CMP #$ff
1617 : d0 15 __ BNE $162e ; (crt_fdiv + 142)
1619 : a5 1e __ LDA ACCU + 3 
161b : 09 7f __ ORA #$7f
161d : 85 1e __ STA ACCU + 3 
161f : a9 80 __ LDA #$80
1621 : 85 1d __ STA ACCU + 2 
1623 : a9 00 __ LDA #$00
1625 : 85 1c __ STA ACCU + 1 
1627 : 85 1b __ STA ACCU + 0 
1629 : 60 __ __ RTS
162a : 69 7f __ ADC #$7f
162c : 90 15 __ BCC $1643 ; (crt_fdiv + 163)
162e : 4a __ __ LSR
162f : 05 1e __ ORA ACCU + 3 
1631 : 85 1e __ STA ACCU + 3 
1633 : a9 00 __ LDA #$00
1635 : 6a __ __ ROR
1636 : 05 0b __ ORA WORK + 8 
1638 : 85 1d __ STA ACCU + 2 
163a : a5 0a __ LDA WORK + 7 
163c : 85 1c __ STA ACCU + 1 
163e : a5 09 __ LDA WORK + 6 
1640 : 85 1b __ STA ACCU + 0 
1642 : 60 __ __ RTS
1643 : a9 00 __ LDA #$00
1645 : 85 1e __ STA ACCU + 3 
1647 : 85 1d __ STA ACCU + 2 
1649 : 85 1c __ STA ACCU + 1 
164b : 85 1b __ STA ACCU + 0 
164d : 60 __ __ RTS
--------------------------------------------------------------------
divs16: ; divs16
164e : 24 1c __ BIT ACCU + 1 
1650 : 10 0d __ BPL $165f ; (divs16 + 17)
1652 : 20 69 16 JSR $1669 ; (negaccu + 0)
1655 : 24 04 __ BIT WORK + 1 
1657 : 10 0d __ BPL $1666 ; (divs16 + 24)
1659 : 20 77 16 JSR $1677 ; (negtmp + 0)
165c : 4c 85 16 JMP $1685 ; (divmod + 0)
165f : 24 04 __ BIT WORK + 1 
1661 : 10 f9 __ BPL $165c ; (divs16 + 14)
1663 : 20 77 16 JSR $1677 ; (negtmp + 0)
1666 : 20 85 16 JSR $1685 ; (divmod + 0)
--------------------------------------------------------------------
negaccu: ; negaccu
1669 : 38 __ __ SEC
166a : a9 00 __ LDA #$00
166c : e5 1b __ SBC ACCU + 0 
166e : 85 1b __ STA ACCU + 0 
1670 : a9 00 __ LDA #$00
1672 : e5 1c __ SBC ACCU + 1 
1674 : 85 1c __ STA ACCU + 1 
1676 : 60 __ __ RTS
--------------------------------------------------------------------
negtmp: ; negtmp
1677 : 38 __ __ SEC
1678 : a9 00 __ LDA #$00
167a : e5 03 __ SBC WORK + 0 
167c : 85 03 __ STA WORK + 0 
167e : a9 00 __ LDA #$00
1680 : e5 04 __ SBC WORK + 1 
1682 : 85 04 __ STA WORK + 1 
1684 : 60 __ __ RTS
--------------------------------------------------------------------
divmod: ; divmod
1685 : a5 1c __ LDA ACCU + 1 
1687 : d0 3b __ BNE $16c4 ; (divmod + 63)
1689 : a5 04 __ LDA WORK + 1 
168b : d0 1e __ BNE $16ab ; (divmod + 38)
168d : 85 06 __ STA WORK + 3 
168f : a2 04 __ LDX #$04
1691 : 06 1b __ ASL ACCU + 0 
1693 : 2a __ __ ROL
1694 : c5 03 __ CMP WORK + 0 
1696 : 90 02 __ BCC $169a ; (divmod + 21)
1698 : e5 03 __ SBC WORK + 0 
169a : 26 1b __ ROL ACCU + 0 
169c : 2a __ __ ROL
169d : c5 03 __ CMP WORK + 0 
169f : 90 02 __ BCC $16a3 ; (divmod + 30)
16a1 : e5 03 __ SBC WORK + 0 
16a3 : 26 1b __ ROL ACCU + 0 
16a5 : ca __ __ DEX
16a6 : d0 eb __ BNE $1693 ; (divmod + 14)
16a8 : 85 05 __ STA WORK + 2 
16aa : 60 __ __ RTS
16ab : a5 1b __ LDA ACCU + 0 
16ad : 85 05 __ STA WORK + 2 
16af : a5 1c __ LDA ACCU + 1 
16b1 : 85 06 __ STA WORK + 3 
16b3 : a9 00 __ LDA #$00
16b5 : 85 1b __ STA ACCU + 0 
16b7 : 85 1c __ STA ACCU + 1 
16b9 : 60 __ __ RTS
16ba : 85 03 __ STA WORK + 0 
16bc : a9 00 __ LDA #$00
16be : 85 04 __ STA WORK + 1 
16c0 : a5 1c __ LDA ACCU + 1 
16c2 : f0 c9 __ BEQ $168d ; (divmod + 8)
16c4 : a5 04 __ LDA WORK + 1 
16c6 : d0 1f __ BNE $16e7 ; (divmod + 98)
16c8 : a5 03 __ LDA WORK + 0 
16ca : 30 1b __ BMI $16e7 ; (divmod + 98)
16cc : a9 00 __ LDA #$00
16ce : 85 06 __ STA WORK + 3 
16d0 : a2 10 __ LDX #$10
16d2 : 06 1b __ ASL ACCU + 0 
16d4 : 26 1c __ ROL ACCU + 1 
16d6 : 2a __ __ ROL
16d7 : c5 03 __ CMP WORK + 0 
16d9 : 90 02 __ BCC $16dd ; (divmod + 88)
16db : e5 03 __ SBC WORK + 0 
16dd : 26 1b __ ROL ACCU + 0 
16df : 26 1c __ ROL ACCU + 1 
16e1 : ca __ __ DEX
16e2 : d0 f2 __ BNE $16d6 ; (divmod + 81)
16e4 : 85 05 __ STA WORK + 2 
16e6 : 60 __ __ RTS
16e7 : a9 00 __ LDA #$00
16e9 : 85 05 __ STA WORK + 2 
16eb : 85 06 __ STA WORK + 3 
16ed : a0 10 __ LDY #$10
16ef : 18 __ __ CLC
16f0 : 26 1b __ ROL ACCU + 0 
16f2 : 26 1c __ ROL ACCU + 1 
16f4 : 26 05 __ ROL WORK + 2 
16f6 : 26 06 __ ROL WORK + 3 
16f8 : 38 __ __ SEC
16f9 : a5 05 __ LDA WORK + 2 
16fb : e5 03 __ SBC WORK + 0 
16fd : aa __ __ TAX
16fe : a5 06 __ LDA WORK + 3 
1700 : e5 04 __ SBC WORK + 1 
1702 : 90 04 __ BCC $1708 ; (divmod + 131)
1704 : 86 05 __ STX WORK + 2 
1706 : 85 06 __ STA WORK + 3 
1708 : 88 __ __ DEY
1709 : d0 e5 __ BNE $16f0 ; (divmod + 107)
170b : 26 1b __ ROL ACCU + 0 
170d : 26 1c __ ROL ACCU + 1 
170f : 60 __ __ RTS
--------------------------------------------------------------------
mods16: ; mods16
1710 : 24 1c __ BIT ACCU + 1 
1712 : 10 10 __ BPL $1724 ; (mods16 + 20)
1714 : 20 69 16 JSR $1669 ; (negaccu + 0)
1717 : 24 04 __ BIT WORK + 1 
1719 : 10 03 __ BPL $171e ; (mods16 + 14)
171b : 20 77 16 JSR $1677 ; (negtmp + 0)
171e : 20 85 16 JSR $1685 ; (divmod + 0)
1721 : 4c 59 17 JMP $1759 ; (negtmpb + 0)
1724 : 24 04 __ BIT WORK + 1 
1726 : 10 03 __ BPL $172b ; (mods16 + 27)
1728 : 20 77 16 JSR $1677 ; (negtmp + 0)
172b : 4c 85 16 JMP $1685 ; (divmod + 0)
172e : 60 __ __ RTS
--------------------------------------------------------------------
negtmpb: ; negtmpb
1759 : 38 __ __ SEC
175a : a9 00 __ LDA #$00
175c : e5 05 __ SBC WORK + 2 
175e : 85 05 __ STA WORK + 2 
1760 : a9 00 __ LDA #$00
1762 : e5 06 __ SBC WORK + 3 
1764 : 85 06 __ STA WORK + 3 
1766 : 60 __ __ RTS
--------------------------------------------------------------------
f32_to_i16: ; f32_to_i16
1767 : 20 ca 13 JSR $13ca ; (freg + 36)
176a : a5 07 __ LDA WORK + 4 
176c : c9 7f __ CMP #$7f
176e : b0 07 __ BCS $1777 ; (f32_to_i16 + 16)
1770 : a9 00 __ LDA #$00
1772 : 85 1b __ STA ACCU + 0 
1774 : 85 1c __ STA ACCU + 1 
1776 : 60 __ __ RTS
1777 : e9 8e __ SBC #$8e
1779 : 90 16 __ BCC $1791 ; (f32_to_i16 + 42)
177b : 24 1e __ BIT ACCU + 3 
177d : 30 09 __ BMI $1788 ; (f32_to_i16 + 33)
177f : a9 ff __ LDA #$ff
1781 : 85 1b __ STA ACCU + 0 
1783 : a9 7f __ LDA #$7f
1785 : 85 1c __ STA ACCU + 1 
1787 : 60 __ __ RTS
1788 : a9 00 __ LDA #$00
178a : 85 1b __ STA ACCU + 0 
178c : a9 80 __ LDA #$80
178e : 85 1c __ STA ACCU + 1 
1790 : 60 __ __ RTS
1791 : aa __ __ TAX
1792 : a5 1c __ LDA ACCU + 1 
1794 : 46 1d __ LSR ACCU + 2 
1796 : 6a __ __ ROR
1797 : e8 __ __ INX
1798 : d0 fa __ BNE $1794 ; (f32_to_i16 + 45)
179a : 24 1e __ BIT ACCU + 3 
179c : 10 0e __ BPL $17ac ; (f32_to_i16 + 69)
179e : 38 __ __ SEC
179f : 49 ff __ EOR #$ff
17a1 : 69 00 __ ADC #$00
17a3 : 85 1b __ STA ACCU + 0 
17a5 : a9 00 __ LDA #$00
17a7 : e5 1d __ SBC ACCU + 2 
17a9 : 85 1c __ STA ACCU + 1 
17ab : 60 __ __ RTS
17ac : 85 1b __ STA ACCU + 0 
17ae : a5 1d __ LDA ACCU + 2 
17b0 : 85 1c __ STA ACCU + 1 
17b2 : 60 __ __ RTS
--------------------------------------------------------------------
sint16_to_float: ; sint16_to_float
17b3 : 24 1c __ BIT ACCU + 1 
17b5 : 30 03 __ BMI $17ba ; (sint16_to_float + 7)
17b7 : 4c d1 17 JMP $17d1 ; (uint16_to_float + 0)
17ba : 38 __ __ SEC
17bb : a9 00 __ LDA #$00
17bd : e5 1b __ SBC ACCU + 0 
17bf : 85 1b __ STA ACCU + 0 
17c1 : a9 00 __ LDA #$00
17c3 : e5 1c __ SBC ACCU + 1 
17c5 : 85 1c __ STA ACCU + 1 
17c7 : 20 d1 17 JSR $17d1 ; (uint16_to_float + 0)
17ca : a5 1e __ LDA ACCU + 3 
17cc : 09 80 __ ORA #$80
17ce : 85 1e __ STA ACCU + 3 
17d0 : 60 __ __ RTS
--------------------------------------------------------------------
uint16_to_float: ; uint16_to_float
17d1 : a5 1b __ LDA ACCU + 0 
17d3 : 05 1c __ ORA ACCU + 1 
17d5 : d0 05 __ BNE $17dc ; (uint16_to_float + 11)
17d7 : 85 1d __ STA ACCU + 2 
17d9 : 85 1e __ STA ACCU + 3 
17db : 60 __ __ RTS
17dc : a2 8e __ LDX #$8e
17de : a5 1c __ LDA ACCU + 1 
17e0 : 30 06 __ BMI $17e8 ; (uint16_to_float + 23)
17e2 : ca __ __ DEX
17e3 : 06 1b __ ASL ACCU + 0 
17e5 : 2a __ __ ROL
17e6 : 10 fa __ BPL $17e2 ; (uint16_to_float + 17)
17e8 : 0a __ __ ASL
17e9 : 85 1d __ STA ACCU + 2 
17eb : a5 1b __ LDA ACCU + 0 
17ed : 85 1c __ STA ACCU + 1 
17ef : 8a __ __ TXA
17f0 : 4a __ __ LSR
17f1 : 85 1e __ STA ACCU + 3 
17f3 : a9 00 __ LDA #$00
17f5 : 85 1b __ STA ACCU + 0 
17f7 : 66 1d __ ROR ACCU + 2 
17f9 : 60 __ __ RTS
--------------------------------------------------------------------
divmod32: ; divmod32
1818 : a9 00 __ LDA #$00
181a : 85 07 __ STA WORK + 4 
181c : 85 08 __ STA WORK + 5 
181e : 85 09 __ STA WORK + 6 
1820 : 85 0a __ STA WORK + 7 
1822 : a5 05 __ LDA WORK + 2 
1824 : 05 06 __ ORA WORK + 3 
1826 : f0 4b __ BEQ $1873 ; (divmod32 + 91)
1828 : a0 10 __ LDY #$10
182a : a5 1e __ LDA ACCU + 3 
182c : 85 08 __ STA WORK + 5 
182e : a5 1d __ LDA ACCU + 2 
1830 : 85 07 __ STA WORK + 4 
1832 : a9 00 __ LDA #$00
1834 : 85 1d __ STA ACCU + 2 
1836 : 85 1e __ STA ACCU + 3 
1838 : 18 __ __ CLC
1839 : 26 1b __ ROL ACCU + 0 
183b : 26 1c __ ROL ACCU + 1 
183d : 26 07 __ ROL WORK + 4 
183f : 26 08 __ ROL WORK + 5 
1841 : 26 09 __ ROL WORK + 6 
1843 : 26 0a __ ROL WORK + 7 
1845 : a5 07 __ LDA WORK + 4 
1847 : c5 03 __ CMP WORK + 0 
1849 : a5 08 __ LDA WORK + 5 
184b : e5 04 __ SBC WORK + 1 
184d : a5 09 __ LDA WORK + 6 
184f : e5 05 __ SBC WORK + 2 
1851 : aa __ __ TAX
1852 : a5 0a __ LDA WORK + 7 
1854 : e5 06 __ SBC WORK + 3 
1856 : 90 11 __ BCC $1869 ; (divmod32 + 81)
1858 : 86 09 __ STX WORK + 6 
185a : 85 0a __ STA WORK + 7 
185c : a5 07 __ LDA WORK + 4 
185e : e5 03 __ SBC WORK + 0 
1860 : 85 07 __ STA WORK + 4 
1862 : a5 08 __ LDA WORK + 5 
1864 : e5 04 __ SBC WORK + 1 
1866 : 85 08 __ STA WORK + 5 
1868 : 38 __ __ SEC
1869 : 88 __ __ DEY
186a : d0 cd __ BNE $1839 ; (divmod32 + 33)
186c : 26 1b __ ROL ACCU + 0 
186e : 26 1c __ ROL ACCU + 1 
1870 : a4 02 __ LDY $02 
1872 : 60 __ __ RTS
1873 : a5 1d __ LDA ACCU + 2 
1875 : 05 1e __ ORA ACCU + 3 
1877 : d0 0c __ BNE $1885 ; (divmod32 + 109)
1879 : 20 85 16 JSR $1685 ; (divmod + 0)
187c : a5 05 __ LDA WORK + 2 
187e : 85 07 __ STA WORK + 4 
1880 : a5 06 __ LDA WORK + 3 
1882 : 85 08 __ STA WORK + 5 
1884 : 60 __ __ RTS
1885 : a0 20 __ LDY #$20
1887 : a5 04 __ LDA WORK + 1 
1889 : d0 27 __ BNE $18b2 ; (divmod32 + 154)
188b : 18 __ __ CLC
188c : 26 1b __ ROL ACCU + 0 
188e : 26 1c __ ROL ACCU + 1 
1890 : 26 1d __ ROL ACCU + 2 
1892 : 26 1e __ ROL ACCU + 3 
1894 : 2a __ __ ROL
1895 : 90 05 __ BCC $189c ; (divmod32 + 132)
1897 : e5 03 __ SBC WORK + 0 
1899 : 38 __ __ SEC
189a : b0 06 __ BCS $18a2 ; (divmod32 + 138)
189c : c5 03 __ CMP WORK + 0 
189e : 90 02 __ BCC $18a2 ; (divmod32 + 138)
18a0 : e5 03 __ SBC WORK + 0 
18a2 : 88 __ __ DEY
18a3 : d0 e7 __ BNE $188c ; (divmod32 + 116)
18a5 : 85 07 __ STA WORK + 4 
18a7 : 26 1b __ ROL ACCU + 0 
18a9 : 26 1c __ ROL ACCU + 1 
18ab : 26 1d __ ROL ACCU + 2 
18ad : 26 1e __ ROL ACCU + 3 
18af : a4 02 __ LDY $02 
18b1 : 60 __ __ RTS
18b2 : a5 1e __ LDA ACCU + 3 
18b4 : d0 10 __ BNE $18c6 ; (divmod32 + 174)
18b6 : a6 1d __ LDX ACCU + 2 
18b8 : 86 1e __ STX ACCU + 3 
18ba : a6 1c __ LDX ACCU + 1 
18bc : 86 1d __ STX ACCU + 2 
18be : a6 1b __ LDX ACCU + 0 
18c0 : 86 1c __ STX ACCU + 1 
18c2 : 85 1b __ STA ACCU + 0 
18c4 : a0 18 __ LDY #$18
18c6 : 18 __ __ CLC
18c7 : 26 1b __ ROL ACCU + 0 
18c9 : 26 1c __ ROL ACCU + 1 
18cb : 26 1d __ ROL ACCU + 2 
18cd : 26 1e __ ROL ACCU + 3 
18cf : 26 07 __ ROL WORK + 4 
18d1 : 26 08 __ ROL WORK + 5 
18d3 : 90 0c __ BCC $18e1 ; (divmod32 + 201)
18d5 : a5 07 __ LDA WORK + 4 
18d7 : e5 03 __ SBC WORK + 0 
18d9 : aa __ __ TAX
18da : a5 08 __ LDA WORK + 5 
18dc : e5 04 __ SBC WORK + 1 
18de : 38 __ __ SEC
18df : b0 0c __ BCS $18ed ; (divmod32 + 213)
18e1 : 38 __ __ SEC
18e2 : a5 07 __ LDA WORK + 4 
18e4 : e5 03 __ SBC WORK + 0 
18e6 : aa __ __ TAX
18e7 : a5 08 __ LDA WORK + 5 
18e9 : e5 04 __ SBC WORK + 1 
18eb : 90 04 __ BCC $18f1 ; (divmod32 + 217)
18ed : 86 07 __ STX WORK + 4 
18ef : 85 08 __ STA WORK + 5 
18f1 : 88 __ __ DEY
18f2 : d0 d3 __ BNE $18c7 ; (divmod32 + 175)
18f4 : 26 1b __ ROL ACCU + 0 
18f6 : 26 1c __ ROL ACCU + 1 
18f8 : 26 1d __ ROL ACCU + 2 
18fa : 26 1e __ ROL ACCU + 3 
18fc : a4 02 __ LDY $02 
18fe : 60 __ __ RTS
--------------------------------------------------------------------
freg@proxy: ; freg@proxy
18ff : a9 00 __ LDA #$00
1901 : 85 03 __ STA WORK + 0 
1903 : a9 00 __ LDA #$00
1905 : 85 04 __ STA WORK + 1 
1907 : a9 20 __ LDA #$20
1909 : 85 05 __ STA WORK + 2 
190b : a9 41 __ LDA #$41
190d : 85 06 __ STA WORK + 3 
190f : 4c ba 13 JMP $13ba ; (freg + 20)
--------------------------------------------------------------------
freg@proxy: ; freg@proxy
1912 : a9 00 __ LDA #$00
1914 : 85 03 __ STA WORK + 0 
1916 : a9 00 __ LDA #$00
1918 : 85 04 __ STA WORK + 1 
191a : a5 43 __ LDA $43 
191c : 85 1b __ STA ACCU + 0 
191e : a5 44 __ LDA $44 
1920 : 85 1c __ STA ACCU + 1 
1922 : a5 45 __ LDA $45 
1924 : 85 1d __ STA ACCU + 2 
1926 : a5 46 __ LDA $46 
1928 : 85 1e __ STA ACCU + 3 
192a : 4c ba 13 JMP $13ba ; (freg + 20)
--------------------------------------------------------------------
freg@proxy: ; freg@proxy
192d : a5 43 __ LDA $43 
192f : 85 1b __ STA ACCU + 0 
1931 : a5 44 __ LDA $44 
1933 : 85 1c __ STA ACCU + 1 
1935 : a5 45 __ LDA $45 
1937 : 85 1d __ STA ACCU + 2 
1939 : a5 46 __ LDA $46 
193b : 85 1e __ STA ACCU + 3 
193d : 4c aa 13 JMP $13aa ; (freg + 4)
--------------------------------------------------------------------
spentry:
1940 : __ __ __ BYT 00                                              : .
--------------------------------------------------------------------
fround5:
1941 : __ __ __ BYT 00 00 00 3f cd cc 4c 3d 0a d7 a3 3b 6f 12 03 3a : ...?..L=...;o..:
1951 : __ __ __ BYT 17 b7 51 38 ac c5 a7 36 bd 37 06 35             : ..Q8...6.7.5
--------------------------------------------------------------------
seed:
195d : __ __ __ BYT 6f 00 00 00                                     : o...
