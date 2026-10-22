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
080e : 8e 8f 19 STX $198f ; (spentry + 0)
0811 : a2 19 __ LDX #$19
0813 : a0 b0 __ LDY #$b0
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
082a : c0 b0 __ CPY #$b0
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
;  18, "C:/Users/korvus/Projects/C64/project_rogue_C64/src/demo.c"
.s1:
0880 : a5 53 __ LDA T1 + 0 
0882 : 8d 9f 9f STA $9f9f ; (main@stack + 0)
.s4:
0885 : a9 00 __ LDA #$00
0887 : 85 53 __ STA T1 + 0 
.l5:
0889 : a9 4f __ LDA #$4f
088b : 8d fa 9f STA $9ffa ; (sstack + 0)
088e : a9 13 __ LDA #$13
0890 : 8d fb 9f STA $9ffb ; (sstack + 1)
0893 : a5 53 __ LDA T1 + 0 
0895 : 8d fc 9f STA $9ffc ; (sstack + 2)
0898 : ad ac 19 LDA $19ac ; (seed + 0)
089b : 85 1b __ STA ACCU + 0 
089d : ad ad 19 LDA $19ad ; (seed + 1)
08a0 : 85 1c __ STA ACCU + 1 
08a2 : ad ae 19 LDA $19ae ; (seed + 2)
08a5 : 85 1d __ STA ACCU + 2 
08a7 : ad af 19 LDA $19af ; (seed + 3)
08aa : 85 1e __ STA ACCU + 3 
08ac : a9 00 __ LDA #$00
08ae : 8d fd 9f STA $9ffd ; (sstack + 3)
08b1 : 85 05 __ STA WORK + 2 
08b3 : 85 06 __ STA WORK + 3 
08b5 : a9 65 __ LDA #$65
08b7 : 85 03 __ STA WORK + 0 
08b9 : a9 2b __ LDA #$2b
08bb : 85 04 __ STA WORK + 1 
08bd : 20 e1 17 JSR $17e1 ; (mul32 + 0)
08c0 : 18 __ __ CLC
08c1 : a5 07 __ LDA WORK + 4 
08c3 : 69 19 __ ADC #$19
08c5 : 8d ac 19 STA $19ac ; (seed + 0)
08c8 : a5 08 __ LDA WORK + 5 
08ca : 69 36 __ ADC #$36
08cc : 8d ad 19 STA $19ad ; (seed + 1)
08cf : a5 09 __ LDA WORK + 6 
08d1 : 69 00 __ ADC #$00
08d3 : 85 1b __ STA ACCU + 0 
08d5 : 8d ae 19 STA $19ae ; (seed + 2)
08d8 : a5 0a __ LDA WORK + 7 
08da : 69 00 __ ADC #$00
08dc : 85 1c __ STA ACCU + 1 
08de : 8d af 19 STA $19af ; (seed + 3)
08e1 : a9 64 __ LDA #$64
08e3 : 20 a1 16 JSR $16a1 ; (divmod + 53)
08e6 : a5 05 __ LDA WORK + 2 
08e8 : 8d fe 9f STA $9ffe ; (sstack + 4)
08eb : a9 00 __ LDA #$00
08ed : 8d ff 9f STA $9fff ; (sstack + 5)
08f0 : 20 07 09 JSR $0907 ; (printf.s1 + 0)
08f3 : e6 53 __ INC T1 + 0 
08f5 : a5 53 __ LDA T1 + 0 
08f7 : c9 0a __ CMP #$0a
08f9 : 90 8e __ BCC $0889 ; (main.l5 + 0)
.s6:
08fb : a9 00 __ LDA #$00
08fd : 85 1b __ STA ACCU + 0 
08ff : 85 1c __ STA ACCU + 1 
.s3:
0901 : ad 9f 9f LDA $9f9f ; (main@stack + 0)
0904 : 85 53 __ STA T1 + 0 
0906 : 60 __ __ RTS
--------------------------------------------------------------------
printf: ; printf(const u8*)->void
;  18, "C:/oscar64/include/stdio.h"
.s1:
0907 : a2 08 __ LDX #$08
0909 : b5 53 __ LDA T1 + 0,x 
090b : 9d a3 9f STA $9fa3,x ; (printf@stack + 0)
090e : ca __ __ DEX
090f : 10 f8 __ BPL $0909 ; (printf.s1 + 2)
.s4:
0911 : ad fa 9f LDA $9ffa ; (sstack + 0)
0914 : 85 58 __ STA T5 + 0 
0916 : a9 fc __ LDA #$fc
0918 : 85 56 __ STA T3 + 0 
091a : a9 9f __ LDA #$9f
091c : 85 57 __ STA T3 + 1 
091e : a9 00 __ LDA #$00
0920 : 85 5b __ STA T7 + 0 
0922 : ad fb 9f LDA $9ffb ; (sstack + 1)
0925 : 85 59 __ STA T5 + 1 
.l5:
0927 : a0 00 __ LDY #$00
0929 : b1 58 __ LDA (T5 + 0),y 
092b : d0 15 __ BNE $0942 ; (printf.s8 + 0)
.s6:
092d : a6 5b __ LDX T7 + 0 
092f : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0932 : f0 03 __ BEQ $0937 ; (printf.s3 + 0)
.s7:
0934 : 20 66 0c JSR $0c66 ; (puts@proxy + 0)
.s3:
0937 : a2 08 __ LDX #$08
0939 : bd a3 9f LDA $9fa3,x ; (printf@stack + 0)
093c : 95 53 __ STA T1 + 0,x 
093e : ca __ __ DEX
093f : 10 f8 __ BPL $0939 ; (printf.s3 + 2)
0941 : 60 __ __ RTS
.s8:
0942 : c9 25 __ CMP #$25
0944 : f0 28 __ BEQ $096e ; (printf.s12 + 0)
.s9:
0946 : a6 5b __ LDX T7 + 0 
0948 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
094b : e6 58 __ INC T5 + 0 
094d : d0 02 __ BNE $0951 ; (printf.s110 + 0)
.s109:
094f : e6 59 __ INC T5 + 1 
.s110:
0951 : e8 __ __ INX
0952 : 86 5b __ STX T7 + 0 
0954 : e0 28 __ CPX #$28
0956 : 90 cf __ BCC $0927 ; (printf.l5 + 0)
.s10:
0958 : a9 b8 __ LDA #$b8
095a : 85 0d __ STA P0 
095c : a9 9f __ LDA #$9f
095e : 85 0e __ STA P1 
0960 : 98 __ __ TYA
0961 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
.s11:
0964 : 20 6e 0c JSR $0c6e ; (puts.s4 + 0)
0967 : a9 00 __ LDA #$00
.s92:
0969 : 85 5b __ STA T7 + 0 
096b : 4c 27 09 JMP $0927 ; (printf.l5 + 0)
.s12:
096e : a5 5b __ LDA T7 + 0 
0970 : f0 0c __ BEQ $097e ; (printf.s13 + 0)
.s85:
0972 : aa __ __ TAX
0973 : 98 __ __ TYA
0974 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0977 : 20 66 0c JSR $0c66 ; (puts@proxy + 0)
097a : a9 00 __ LDA #$00
097c : 85 5b __ STA T7 + 0 
.s13:
097e : 8d b5 9f STA $9fb5 ; (si.sign + 0)
0981 : 8d b6 9f STA $9fb6 ; (si.left + 0)
0984 : 8d b7 9f STA $9fb7 ; (si.prefix + 0)
0987 : a0 01 __ LDY #$01
0989 : b1 58 __ LDA (T5 + 0),y 
098b : a2 20 __ LDX #$20
098d : 8e b0 9f STX $9fb0 ; (si.fill + 0)
0990 : a2 00 __ LDX #$00
0992 : 8e b1 9f STX $9fb1 ; (si.width + 0)
0995 : ca __ __ DEX
0996 : 8e b2 9f STX $9fb2 ; (si.precision + 0)
0999 : a2 0a __ LDX #$0a
099b : 8e b4 9f STX $9fb4 ; (si.base + 0)
099e : aa __ __ TAX
099f : a9 02 __ LDA #$02
09a1 : d0 07 __ BNE $09aa ; (printf.l14 + 0)
.s81:
09a3 : a0 00 __ LDY #$00
09a5 : b1 58 __ LDA (T5 + 0),y 
09a7 : aa __ __ TAX
09a8 : a9 01 __ LDA #$01
.l14:
09aa : 18 __ __ CLC
09ab : 65 58 __ ADC T5 + 0 
09ad : 85 58 __ STA T5 + 0 
09af : 90 02 __ BCC $09b3 ; (printf.s96 + 0)
.s95:
09b1 : e6 59 __ INC T5 + 1 
.s96:
09b3 : e0 2b __ CPX #$2b
09b5 : d0 07 __ BNE $09be ; (printf.s15 + 0)
.s84:
09b7 : a9 01 __ LDA #$01
09b9 : 8d b5 9f STA $9fb5 ; (si.sign + 0)
09bc : d0 e5 __ BNE $09a3 ; (printf.s81 + 0)
.s15:
09be : 8a __ __ TXA
09bf : e0 30 __ CPX #$30
09c1 : d0 05 __ BNE $09c8 ; (printf.s16 + 0)
.s83:
09c3 : 8d b0 9f STA $9fb0 ; (si.fill + 0)
09c6 : f0 db __ BEQ $09a3 ; (printf.s81 + 0)
.s16:
09c8 : c9 23 __ CMP #$23
09ca : d0 07 __ BNE $09d3 ; (printf.s17 + 0)
.s82:
09cc : a9 01 __ LDA #$01
09ce : 8d b7 9f STA $9fb7 ; (si.prefix + 0)
09d1 : d0 d0 __ BNE $09a3 ; (printf.s81 + 0)
.s17:
09d3 : c9 2d __ CMP #$2d
09d5 : d0 07 __ BNE $09de ; (printf.s18 + 0)
.s80:
09d7 : a9 01 __ LDA #$01
09d9 : 8d b6 9f STA $9fb6 ; (si.left + 0)
09dc : d0 c5 __ BNE $09a3 ; (printf.s81 + 0)
.s18:
09de : 85 55 __ STA T2 + 0 
09e0 : c9 30 __ CMP #$30
09e2 : 90 3a __ BCC $0a1e ; (printf.s19 + 0)
.s75:
09e4 : e0 3a __ CPX #$3a
09e6 : b0 66 __ BCS $0a4e ; (printf.s20 + 0)
.s76:
09e8 : a9 00 __ LDA #$00
09ea : e0 3a __ CPX #$3a
09ec : b0 2b __ BCS $0a19 ; (printf.s78 + 0)
.s111:
09ee : 85 43 __ STA T0 + 0 
09f0 : a4 58 __ LDY T5 + 0 
09f2 : 85 58 __ STA T5 + 0 
.l79:
09f4 : a5 43 __ LDA T0 + 0 
09f6 : 0a __ __ ASL
09f7 : 0a __ __ ASL
09f8 : 18 __ __ CLC
09f9 : 65 43 __ ADC T0 + 0 
09fb : 0a __ __ ASL
09fc : 18 __ __ CLC
09fd : 65 55 __ ADC T2 + 0 
09ff : 38 __ __ SEC
0a00 : e9 30 __ SBC #$30
0a02 : 85 43 __ STA T0 + 0 
0a04 : b1 58 __ LDA (T5 + 0),y 
0a06 : 85 55 __ STA T2 + 0 
0a08 : c8 __ __ INY
0a09 : d0 02 __ BNE $0a0d ; (printf.s108 + 0)
.s107:
0a0b : e6 59 __ INC T5 + 1 
.s108:
0a0d : c9 30 __ CMP #$30
0a0f : 90 04 __ BCC $0a15 ; (printf.s112 + 0)
.s77:
0a11 : c9 3a __ CMP #$3a
0a13 : 90 df __ BCC $09f4 ; (printf.l79 + 0)
.s112:
0a15 : 84 58 __ STY T5 + 0 
0a17 : a5 43 __ LDA T0 + 0 
.s78:
0a19 : 8d b1 9f STA $9fb1 ; (si.width + 0)
0a1c : a5 55 __ LDA T2 + 0 
.s19:
0a1e : c9 2e __ CMP #$2e
0a20 : d0 2c __ BNE $0a4e ; (printf.s20 + 0)
.s70:
0a22 : a9 00 __ LDA #$00
0a24 : a8 __ __ TAY
0a25 : f0 0e __ BEQ $0a35 ; (printf.l71 + 0)
.s74:
0a27 : a5 43 __ LDA T0 + 0 
0a29 : 0a __ __ ASL
0a2a : 0a __ __ ASL
0a2b : 18 __ __ CLC
0a2c : 65 43 __ ADC T0 + 0 
0a2e : 0a __ __ ASL
0a2f : 18 __ __ CLC
0a30 : 65 55 __ ADC T2 + 0 
0a32 : 38 __ __ SEC
0a33 : e9 30 __ SBC #$30
.l71:
0a35 : 85 43 __ STA T0 + 0 
0a37 : b1 58 __ LDA (T5 + 0),y 
0a39 : 85 55 __ STA T2 + 0 
0a3b : e6 58 __ INC T5 + 0 
0a3d : d0 02 __ BNE $0a41 ; (printf.s98 + 0)
.s97:
0a3f : e6 59 __ INC T5 + 1 
.s98:
0a41 : c9 30 __ CMP #$30
0a43 : 90 04 __ BCC $0a49 ; (printf.s72 + 0)
.s73:
0a45 : c9 3a __ CMP #$3a
0a47 : 90 de __ BCC $0a27 ; (printf.s74 + 0)
.s72:
0a49 : a6 43 __ LDX T0 + 0 
0a4b : 8e b2 9f STX $9fb2 ; (si.precision + 0)
.s20:
0a4e : c9 64 __ CMP #$64
0a50 : f0 0c __ BEQ $0a5e ; (printf.s69 + 0)
.s21:
0a52 : c9 44 __ CMP #$44
0a54 : f0 08 __ BEQ $0a5e ; (printf.s69 + 0)
.s22:
0a56 : c9 69 __ CMP #$69
0a58 : f0 04 __ BEQ $0a5e ; (printf.s69 + 0)
.s23:
0a5a : c9 49 __ CMP #$49
0a5c : d0 0f __ BNE $0a6d ; (printf.s24 + 0)
.s69:
0a5e : a9 01 __ LDA #$01
.s68:
0a60 : 85 13 __ STA P6 
0a62 : a0 00 __ LDY #$00
0a64 : b1 56 __ LDA (T3 + 0),y 
0a66 : aa __ __ TAX
0a67 : c8 __ __ INY
0a68 : b1 56 __ LDA (T3 + 0),y 
0a6a : 4c 48 0c JMP $0c48 ; (printf.s66 + 0)
.s24:
0a6d : c9 75 __ CMP #$75
0a6f : f0 04 __ BEQ $0a75 ; (printf.s67 + 0)
.s25:
0a71 : c9 55 __ CMP #$55
0a73 : d0 04 __ BNE $0a79 ; (printf.s26 + 0)
.s67:
0a75 : a9 00 __ LDA #$00
0a77 : f0 e7 __ BEQ $0a60 ; (printf.s68 + 0)
.s26:
0a79 : c9 78 __ CMP #$78
0a7b : f0 04 __ BEQ $0a81 ; (printf.s65 + 0)
.s27:
0a7d : c9 58 __ CMP #$58
0a7f : d0 1d __ BNE $0a9e ; (printf.s28 + 0)
.s65:
0a81 : a0 00 __ LDY #$00
0a83 : 84 13 __ STY P6 
0a85 : a9 10 __ LDA #$10
0a87 : 8d b4 9f STA $9fb4 ; (si.base + 0)
0a8a : b1 56 __ LDA (T3 + 0),y 
0a8c : aa __ __ TAX
0a8d : c8 __ __ INY
0a8e : b1 56 __ LDA (T3 + 0),y 
0a90 : a8 __ __ TAY
0a91 : a5 55 __ LDA T2 + 0 
0a93 : 29 e0 __ AND #$e0
0a95 : 09 01 __ ORA #$01
0a97 : 8d b3 9f STA $9fb3 ; (si.cha + 0)
0a9a : 98 __ __ TYA
0a9b : 4c 48 0c JMP $0c48 ; (printf.s66 + 0)
.s28:
0a9e : c9 6c __ CMP #$6c
0aa0 : d0 03 __ BNE $0aa5 ; (printf.s29 + 0)
0aa2 : 4c cd 0b JMP $0bcd ; (printf.s53 + 0)
.s29:
0aa5 : c9 4c __ CMP #$4c
0aa7 : f0 f9 __ BEQ $0aa2 ; (printf.s28 + 4)
.s30:
0aa9 : c9 66 __ CMP #$66
0aab : f0 14 __ BEQ $0ac1 ; (printf.s52 + 0)
.s31:
0aad : c9 67 __ CMP #$67
0aaf : f0 10 __ BEQ $0ac1 ; (printf.s52 + 0)
.s32:
0ab1 : c9 65 __ CMP #$65
0ab3 : f0 0c __ BEQ $0ac1 ; (printf.s52 + 0)
.s33:
0ab5 : c9 46 __ CMP #$46
0ab7 : f0 08 __ BEQ $0ac1 ; (printf.s52 + 0)
.s34:
0ab9 : c9 47 __ CMP #$47
0abb : f0 04 __ BEQ $0ac1 ; (printf.s52 + 0)
.s35:
0abd : c9 45 __ CMP #$45
0abf : d0 44 __ BNE $0b05 ; (printf.s36 + 0)
.s52:
0ac1 : a9 b8 __ LDA #$b8
0ac3 : 85 0f __ STA P2 
0ac5 : a9 9f __ LDA #$9f
0ac7 : 85 10 __ STA P3 
0ac9 : a0 00 __ LDY #$00
0acb : b1 56 __ LDA (T3 + 0),y 
0acd : 85 11 __ STA P4 
0acf : c8 __ __ INY
0ad0 : b1 56 __ LDA (T3 + 0),y 
0ad2 : 85 12 __ STA P5 
0ad4 : c8 __ __ INY
0ad5 : b1 56 __ LDA (T3 + 0),y 
0ad7 : 85 13 __ STA P6 
0ad9 : c8 __ __ INY
0ada : b1 56 __ LDA (T3 + 0),y 
0adc : 85 14 __ STA P7 
0ade : a5 55 __ LDA T2 + 0 
0ae0 : 29 e0 __ AND #$e0
0ae2 : 09 01 __ ORA #$01
0ae4 : 8d b3 9f STA $9fb3 ; (si.cha + 0)
0ae7 : a9 b0 __ LDA #$b0
0ae9 : 85 0d __ STA P0 
0aeb : a9 9f __ LDA #$9f
0aed : 85 0e __ STA P1 
0aef : a5 55 __ LDA T2 + 0 
0af1 : ed b3 9f SBC $9fb3 ; (si.cha + 0)
0af4 : 18 __ __ CLC
0af5 : 69 61 __ ADC #$61
0af7 : 85 15 __ STA P8 
0af9 : 20 03 0f JSR $0f03 ; (nformf.s1 + 0)
0afc : a5 1b __ LDA ACCU + 0 ; (fmt + 2)
0afe : 85 5b __ STA T7 + 0 
0b00 : a9 04 __ LDA #$04
0b02 : 4c c1 0b JMP $0bc1 ; (printf.s90 + 0)
.s36:
0b05 : c9 73 __ CMP #$73
0b07 : f0 2d __ BEQ $0b36 ; (printf.s44 + 0)
.s37:
0b09 : c9 53 __ CMP #$53
0b0b : f0 29 __ BEQ $0b36 ; (printf.s44 + 0)
.s38:
0b0d : c9 63 __ CMP #$63
0b0f : f0 12 __ BEQ $0b23 ; (printf.s43 + 0)
.s39:
0b11 : c9 43 __ CMP #$43
0b13 : f0 0e __ BEQ $0b23 ; (printf.s43 + 0)
.s40:
0b15 : aa __ __ TAX
0b16 : d0 03 __ BNE $0b1b ; (printf.s41 + 0)
0b18 : 4c 27 09 JMP $0927 ; (printf.l5 + 0)
.s41:
0b1b : 8d b8 9f STA $9fb8 ; (buff[0] + 0)
.s42:
0b1e : a9 01 __ LDA #$01
0b20 : 4c 69 09 JMP $0969 ; (printf.s92 + 0)
.s43:
0b23 : a0 00 __ LDY #$00
0b25 : b1 56 __ LDA (T3 + 0),y 
0b27 : 8d b8 9f STA $9fb8 ; (buff[0] + 0)
0b2a : a5 56 __ LDA T3 + 0 
0b2c : 69 01 __ ADC #$01
0b2e : 85 56 __ STA T3 + 0 
0b30 : 90 ec __ BCC $0b1e ; (printf.s42 + 0)
.s106:
0b32 : e6 57 __ INC T3 + 1 
0b34 : b0 e8 __ BCS $0b1e ; (printf.s42 + 0)
.s44:
0b36 : a0 00 __ LDY #$00
0b38 : 84 5a __ STY T6 + 0 
0b3a : b1 56 __ LDA (T3 + 0),y 
0b3c : 85 53 __ STA T1 + 0 
0b3e : c8 __ __ INY
0b3f : b1 56 __ LDA (T3 + 0),y 
0b41 : 85 54 __ STA T1 + 1 
0b43 : a5 56 __ LDA T3 + 0 
0b45 : 69 01 __ ADC #$01
0b47 : 85 56 __ STA T3 + 0 
0b49 : 90 02 __ BCC $0b4d ; (printf.s105 + 0)
.s104:
0b4b : e6 57 __ INC T3 + 1 
.s105:
0b4d : ad b1 9f LDA $9fb1 ; (si.width + 0)
0b50 : f0 0d __ BEQ $0b5f ; (printf.s45 + 0)
.s93:
0b52 : a0 00 __ LDY #$00
0b54 : b1 53 __ LDA (T1 + 0),y 
0b56 : f0 05 __ BEQ $0b5d ; (printf.s94 + 0)
.l51:
0b58 : c8 __ __ INY
0b59 : b1 53 __ LDA (T1 + 0),y 
0b5b : d0 fb __ BNE $0b58 ; (printf.l51 + 0)
.s94:
0b5d : 84 5a __ STY T6 + 0 
.s45:
0b5f : ad b6 9f LDA $9fb6 ; (si.left + 0)
0b62 : 85 55 __ STA T2 + 0 
0b64 : d0 07 __ BNE $0b6d ; (printf.s47 + 0)
.s46:
0b66 : a4 5a __ LDY T6 + 0 
0b68 : cc b1 9f CPY $9fb1 ; (si.width + 0)
0b6b : 90 2a __ BCC $0b97 ; (printf.s50 + 0)
.s47:
0b6d : a5 53 __ LDA T1 + 0 
0b6f : 85 0d __ STA P0 
0b71 : a5 54 __ LDA T1 + 1 
0b73 : 85 0e __ STA P1 
0b75 : 20 6e 0c JSR $0c6e ; (puts.s4 + 0)
0b78 : a5 55 __ LDA T2 + 0 
0b7a : f0 9c __ BEQ $0b18 ; (printf.s40 + 3)
.s48:
0b7c : a4 5a __ LDY T6 + 0 
0b7e : cc b1 9f CPY $9fb1 ; (si.width + 0)
0b81 : b0 95 __ BCS $0b18 ; (printf.s40 + 3)
.s49:
0b83 : ad b0 9f LDA $9fb0 ; (si.fill + 0)
0b86 : a2 00 __ LDX #$00
.l88:
0b88 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0b8b : e8 __ __ INX
0b8c : c8 __ __ INY
0b8d : cc b1 9f CPY $9fb1 ; (si.width + 0)
0b90 : 90 f6 __ BCC $0b88 ; (printf.l88 + 0)
.s86:
0b92 : 86 5b __ STX T7 + 0 
0b94 : 4c 27 09 JMP $0927 ; (printf.l5 + 0)
.s50:
0b97 : ad b0 9f LDA $9fb0 ; (si.fill + 0)
0b9a : a2 00 __ LDX #$00
.l89:
0b9c : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0b9f : e8 __ __ INX
0ba0 : c8 __ __ INY
0ba1 : cc b1 9f CPY $9fb1 ; (si.width + 0)
0ba4 : 90 f6 __ BCC $0b9c ; (printf.l89 + 0)
.s87:
0ba6 : a9 b8 __ LDA #$b8
0ba8 : 85 0d __ STA P0 
0baa : a9 9f __ LDA #$9f
0bac : 85 0e __ STA P1 
0bae : a9 00 __ LDA #$00
0bb0 : 9d b8 9f STA $9fb8,x ; (buff[0] + 0)
0bb3 : 20 6e 0c JSR $0c6e ; (puts.s4 + 0)
0bb6 : a5 53 __ LDA T1 + 0 
0bb8 : 85 0d __ STA P0 
0bba : a5 54 __ LDA T1 + 1 
0bbc : 85 0e __ STA P1 
0bbe : 4c 64 09 JMP $0964 ; (printf.s11 + 0)
.s90:
0bc1 : 18 __ __ CLC
0bc2 : 65 56 __ ADC T3 + 0 
0bc4 : 85 56 __ STA T3 + 0 
0bc6 : 90 cc __ BCC $0b94 ; (printf.s86 + 2)
.s99:
0bc8 : e6 57 __ INC T3 + 1 
0bca : 4c 27 09 JMP $0927 ; (printf.l5 + 0)
.s53:
0bcd : a0 00 __ LDY #$00
0bcf : b1 56 __ LDA (T3 + 0),y 
0bd1 : 85 11 __ STA P4 
0bd3 : c8 __ __ INY
0bd4 : b1 56 __ LDA (T3 + 0),y 
0bd6 : 85 12 __ STA P5 
0bd8 : c8 __ __ INY
0bd9 : b1 56 __ LDA (T3 + 0),y 
0bdb : 85 13 __ STA P6 
0bdd : c8 __ __ INY
0bde : b1 56 __ LDA (T3 + 0),y 
0be0 : 85 14 __ STA P7 
0be2 : a5 56 __ LDA T3 + 0 
0be4 : 69 03 __ ADC #$03
0be6 : 85 56 __ STA T3 + 0 
0be8 : 90 02 __ BCC $0bec ; (printf.s101 + 0)
.s100:
0bea : e6 57 __ INC T3 + 1 
.s101:
0bec : a0 00 __ LDY #$00
0bee : b1 58 __ LDA (T5 + 0),y 
0bf0 : aa __ __ TAX
0bf1 : e6 58 __ INC T5 + 0 
0bf3 : d0 02 __ BNE $0bf7 ; (printf.s103 + 0)
.s102:
0bf5 : e6 59 __ INC T5 + 1 
.s103:
0bf7 : e0 64 __ CPX #$64
0bf9 : f0 0c __ BEQ $0c07 ; (printf.s64 + 0)
.s54:
0bfb : e0 44 __ CPX #$44
0bfd : f0 08 __ BEQ $0c07 ; (printf.s64 + 0)
.s55:
0bff : e0 69 __ CPX #$69
0c01 : f0 04 __ BEQ $0c07 ; (printf.s64 + 0)
.s56:
0c03 : e0 49 __ CPX #$49
0c05 : d0 1c __ BNE $0c23 ; (printf.s57 + 0)
.s64:
0c07 : a9 01 __ LDA #$01
.s91:
0c09 : 85 15 __ STA P8 
.s62:
0c0b : a9 b8 __ LDA #$b8
0c0d : 85 0f __ STA P2 
0c0f : a9 9f __ LDA #$9f
0c11 : 85 0e __ STA P1 
0c13 : a9 9f __ LDA #$9f
0c15 : 85 10 __ STA P3 
0c17 : a9 b0 __ LDA #$b0
0c19 : 85 0d __ STA P0 
0c1b : 20 b8 0d JSR $0db8 ; (nforml.s4 + 0)
0c1e : a5 1b __ LDA ACCU + 0 ; (fmt + 2)
0c20 : 4c 69 09 JMP $0969 ; (printf.s92 + 0)
.s57:
0c23 : e0 75 __ CPX #$75
0c25 : f0 04 __ BEQ $0c2b ; (printf.s63 + 0)
.s58:
0c27 : e0 55 __ CPX #$55
0c29 : d0 03 __ BNE $0c2e ; (printf.s59 + 0)
.s63:
0c2b : 98 __ __ TYA
0c2c : f0 db __ BEQ $0c09 ; (printf.s91 + 0)
.s59:
0c2e : e0 78 __ CPX #$78
0c30 : f0 04 __ BEQ $0c36 ; (printf.s61 + 0)
.s60:
0c32 : e0 58 __ CPX #$58
0c34 : d0 94 __ BNE $0bca ; (printf.s99 + 2)
.s61:
0c36 : 84 15 __ STY P8 
0c38 : a9 10 __ LDA #$10
0c3a : 8d b4 9f STA $9fb4 ; (si.base + 0)
0c3d : 8a __ __ TXA
0c3e : 29 e0 __ AND #$e0
0c40 : 09 01 __ ORA #$01
0c42 : 8d b3 9f STA $9fb3 ; (si.cha + 0)
0c45 : 4c 0b 0c JMP $0c0b ; (printf.s62 + 0)
.s66:
0c48 : 86 11 __ STX P4 
0c4a : 85 12 __ STA P5 
0c4c : a9 b8 __ LDA #$b8
0c4e : 85 0f __ STA P2 
0c50 : a9 9f __ LDA #$9f
0c52 : 85 0e __ STA P1 
0c54 : a9 9f __ LDA #$9f
0c56 : 85 10 __ STA P3 
0c58 : a9 b0 __ LDA #$b0
0c5a : 85 0d __ STA P0 
0c5c : 20 a3 0c JSR $0ca3 ; (nformi.s4 + 0)
0c5f : 85 5b __ STA T7 + 0 
0c61 : a9 02 __ LDA #$02
0c63 : 4c c1 0b JMP $0bc1 ; (printf.s90 + 0)
--------------------------------------------------------------------
puts@proxy: ; puts@proxy
0c66 : a9 b8 __ LDA #$b8
0c68 : 85 0d __ STA P0 
0c6a : a9 9f __ LDA #$9f
0c6c : 85 0e __ STA P1 
--------------------------------------------------------------------
puts: ; puts(const u8*)->void
;  12, "C:/oscar64/include/stdio.h"
.s4:
0c6e : a0 00 __ LDY #$00
.l5:
0c70 : b1 0d __ LDA (P0),y ; (str + 0)
0c72 : aa __ __ TAX
0c73 : 18 __ __ CLC
0c74 : a5 0d __ LDA P0 ; (str + 0)
0c76 : 69 01 __ ADC #$01
0c78 : 85 0d __ STA P0 ; (str + 0)
0c7a : 8a __ __ TXA
0c7b : d0 01 __ BNE $0c7e ; (puts.s6 + 0)
.s3:
0c7d : 60 __ __ RTS
.s6:
0c7e : 90 02 __ BCC $0c82 ; (puts.s13 + 0)
.s12:
0c80 : e6 0e __ INC P1 ; (str + 1)
.s13:
0c82 : c9 0a __ CMP #$0a
0c84 : d0 08 __ BNE $0c8e ; (puts.s7 + 0)
.s10:
0c86 : a9 0d __ LDA #$0d
.s11:
0c88 : 20 d2 ff JSR $ffd2 
0c8b : 4c 70 0c JMP $0c70 ; (puts.l5 + 0)
.s7:
0c8e : c9 09 __ CMP #$09
0c90 : d0 f6 __ BNE $0c88 ; (puts.s11 + 0)
.s8:
0c92 : a5 d3 __ LDA $d3 
0c94 : 29 03 __ AND #$03
0c96 : aa __ __ TAX
.l9:
0c97 : a9 20 __ LDA #$20
0c99 : 20 d2 ff JSR $ffd2 
0c9c : e8 __ __ INX
0c9d : e0 04 __ CPX #$04
0c9f : 90 f6 __ BCC $0c97 ; (puts.l9 + 0)
0ca1 : b0 cd __ BCS $0c70 ; (puts.l5 + 0)
--------------------------------------------------------------------
nformi: ; nformi(const struct sinfo*,u8*,i16,bool)->u8
;  79, "C:/oscar64/include/stdio.c"
.s4:
0ca3 : a9 00 __ LDA #$00
0ca5 : 85 43 __ STA T5 + 0 
0ca7 : a0 04 __ LDY #$04
0ca9 : b1 0d __ LDA (P0),y ; (si + 0)
0cab : 85 44 __ STA T6 + 0 
0cad : a5 13 __ LDA P6 ; (s + 0)
0caf : f0 13 __ BEQ $0cc4 ; (nformi.s5 + 0)
.s34:
0cb1 : 24 12 __ BIT P5 ; (v + 1)
0cb3 : 10 0f __ BPL $0cc4 ; (nformi.s5 + 0)
.s35:
0cb5 : 38 __ __ SEC
0cb6 : a9 00 __ LDA #$00
0cb8 : e5 11 __ SBC P4 ; (v + 0)
0cba : 85 11 __ STA P4 ; (v + 0)
0cbc : a9 00 __ LDA #$00
0cbe : e5 12 __ SBC P5 ; (v + 1)
0cc0 : 85 12 __ STA P5 ; (v + 1)
0cc2 : e6 43 __ INC T5 + 0 
.s5:
0cc4 : a9 10 __ LDA #$10
0cc6 : 85 45 __ STA T7 + 0 
0cc8 : a5 11 __ LDA P4 ; (v + 0)
0cca : 05 12 __ ORA P5 ; (v + 1)
0ccc : f0 2d __ BEQ $0cfb ; (nformi.s6 + 0)
.s29:
0cce : a5 11 __ LDA P4 ; (v + 0)
0cd0 : 85 1b __ STA ACCU + 0 
0cd2 : a5 12 __ LDA P5 ; (v + 1)
0cd4 : 85 1c __ STA ACCU + 1 
.l30:
0cd6 : a5 44 __ LDA T6 + 0 
0cd8 : 20 a1 16 JSR $16a1 ; (divmod + 53)
0cdb : a5 05 __ LDA WORK + 2 
0cdd : c9 0a __ CMP #$0a
0cdf : b0 04 __ BCS $0ce5 ; (nformi.s33 + 0)
.s31:
0ce1 : a9 30 __ LDA #$30
0ce3 : 90 06 __ BCC $0ceb ; (nformi.s32 + 0)
.s33:
0ce5 : a0 03 __ LDY #$03
0ce7 : b1 0d __ LDA (P0),y ; (si + 0)
0ce9 : e9 0a __ SBC #$0a
.s32:
0ceb : 18 __ __ CLC
0cec : 65 05 __ ADC WORK + 2 
0cee : a6 45 __ LDX T7 + 0 
0cf0 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0cf3 : c6 45 __ DEC T7 + 0 
0cf5 : a5 1b __ LDA ACCU + 0 
0cf7 : 05 1c __ ORA ACCU + 1 
0cf9 : d0 db __ BNE $0cd6 ; (nformi.l30 + 0)
.s6:
0cfb : a9 ff __ LDA #$ff
0cfd : a0 02 __ LDY #$02
0cff : d1 0d __ CMP (P0),y ; (si + 0)
0d01 : d0 04 __ BNE $0d07 ; (nformi.s28 + 0)
.s7:
0d03 : a9 0f __ LDA #$0f
0d05 : d0 05 __ BNE $0d0c ; (nformi.s40 + 0)
.s28:
0d07 : 38 __ __ SEC
0d08 : a9 10 __ LDA #$10
0d0a : f1 0d __ SBC (P0),y ; (si + 0)
.s40:
0d0c : a8 __ __ TAY
0d0d : c4 45 __ CPY T7 + 0 
0d0f : b0 0d __ BCS $0d1e ; (nformi.s8 + 0)
.s27:
0d11 : a9 30 __ LDA #$30
.l41:
0d13 : a6 45 __ LDX T7 + 0 
0d15 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d18 : c6 45 __ DEC T7 + 0 
0d1a : c4 45 __ CPY T7 + 0 
0d1c : 90 f5 __ BCC $0d13 ; (nformi.l41 + 0)
.s8:
0d1e : a0 07 __ LDY #$07
0d20 : b1 0d __ LDA (P0),y ; (si + 0)
0d22 : f0 1c __ BEQ $0d40 ; (nformi.s9 + 0)
.s25:
0d24 : a5 44 __ LDA T6 + 0 
0d26 : c9 10 __ CMP #$10
0d28 : d0 16 __ BNE $0d40 ; (nformi.s9 + 0)
.s26:
0d2a : a0 03 __ LDY #$03
0d2c : b1 0d __ LDA (P0),y ; (si + 0)
0d2e : a8 __ __ TAY
0d2f : a9 30 __ LDA #$30
0d31 : a6 45 __ LDX T7 + 0 
0d33 : 9d e8 9f STA $9fe8,x ; (buff[0] + 48)
0d36 : 98 __ __ TYA
0d37 : 69 16 __ ADC #$16
0d39 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d3c : ca __ __ DEX
0d3d : ca __ __ DEX
0d3e : 86 45 __ STX T7 + 0 
.s9:
0d40 : a9 00 __ LDA #$00
0d42 : 85 1b __ STA ACCU + 0 
0d44 : a5 43 __ LDA T5 + 0 
0d46 : f0 0c __ BEQ $0d54 ; (nformi.s10 + 0)
.s24:
0d48 : a9 2d __ LDA #$2d
.s23:
0d4a : a6 45 __ LDX T7 + 0 
0d4c : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d4f : c6 45 __ DEC T7 + 0 
0d51 : 4c 5e 0d JMP $0d5e ; (nformi.s11 + 0)
.s10:
0d54 : a0 05 __ LDY #$05
0d56 : b1 0d __ LDA (P0),y ; (si + 0)
0d58 : f0 04 __ BEQ $0d5e ; (nformi.s11 + 0)
.s22:
0d5a : a9 2b __ LDA #$2b
0d5c : d0 ec __ BNE $0d4a ; (nformi.s23 + 0)
.s11:
0d5e : a6 45 __ LDX T7 + 0 
0d60 : a0 06 __ LDY #$06
0d62 : b1 0d __ LDA (P0),y ; (si + 0)
0d64 : d0 2b __ BNE $0d91 ; (nformi.s18 + 0)
.l12:
0d66 : 8a __ __ TXA
0d67 : 18 __ __ CLC
0d68 : a0 01 __ LDY #$01
0d6a : 71 0d __ ADC (P0),y ; (si + 0)
0d6c : b0 04 __ BCS $0d72 ; (nformi.s16 + 0)
.s17:
0d6e : c9 11 __ CMP #$11
0d70 : 90 0a __ BCC $0d7c ; (nformi.s13 + 0)
.s16:
0d72 : a0 00 __ LDY #$00
0d74 : b1 0d __ LDA (P0),y ; (si + 0)
0d76 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0d79 : ca __ __ DEX
0d7a : b0 ea __ BCS $0d66 ; (nformi.l12 + 0)
.s13:
0d7c : e0 10 __ CPX #$10
0d7e : b0 0e __ BCS $0d8e ; (nformi.s14 + 0)
.s15:
0d80 : 88 __ __ DEY
.l38:
0d81 : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0d84 : 91 0f __ STA (P2),y ; (str + 0)
0d86 : c8 __ __ INY
0d87 : e8 __ __ INX
0d88 : e0 10 __ CPX #$10
0d8a : 90 f5 __ BCC $0d81 ; (nformi.l38 + 0)
.s39:
0d8c : 84 1b __ STY ACCU + 0 
.s14:
0d8e : a5 1b __ LDA ACCU + 0 
.s3:
0d90 : 60 __ __ RTS
.s18:
0d91 : e0 10 __ CPX #$10
0d93 : b0 1a __ BCS $0daf ; (nformi.l19 + 0)
.s21:
0d95 : a0 00 __ LDY #$00
.l36:
0d97 : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0d9a : 91 0f __ STA (P2),y ; (str + 0)
0d9c : c8 __ __ INY
0d9d : e8 __ __ INX
0d9e : e0 10 __ CPX #$10
0da0 : 90 f5 __ BCC $0d97 ; (nformi.l36 + 0)
.s37:
0da2 : 84 1b __ STY ACCU + 0 
0da4 : b0 09 __ BCS $0daf ; (nformi.l19 + 0)
.s20:
0da6 : 88 __ __ DEY
0da7 : b1 0d __ LDA (P0),y ; (si + 0)
0da9 : a4 1b __ LDY ACCU + 0 
0dab : 91 0f __ STA (P2),y ; (str + 0)
0dad : e6 1b __ INC ACCU + 0 
.l19:
0daf : a5 1b __ LDA ACCU + 0 
0db1 : a0 01 __ LDY #$01
0db3 : d1 0d __ CMP (P0),y ; (si + 0)
0db5 : 90 ef __ BCC $0da6 ; (nformi.s20 + 0)
0db7 : 60 __ __ RTS
--------------------------------------------------------------------
nforml: ; nforml(const struct sinfo*,u8*,i32,bool)->u8
; 137, "C:/oscar64/include/stdio.c"
.s4:
0db8 : a9 00 __ LDA #$00
0dba : 85 43 __ STA T4 + 0 
0dbc : a5 15 __ LDA P8 ; (s + 0)
0dbe : f0 1f __ BEQ $0ddf ; (nforml.s5 + 0)
.s35:
0dc0 : 24 14 __ BIT P7 ; (v + 3)
0dc2 : 10 1b __ BPL $0ddf ; (nforml.s5 + 0)
.s36:
0dc4 : 38 __ __ SEC
0dc5 : a9 00 __ LDA #$00
0dc7 : e5 11 __ SBC P4 ; (v + 0)
0dc9 : 85 11 __ STA P4 ; (v + 0)
0dcb : a9 00 __ LDA #$00
0dcd : e5 12 __ SBC P5 ; (v + 1)
0dcf : 85 12 __ STA P5 ; (v + 1)
0dd1 : a9 00 __ LDA #$00
0dd3 : e5 13 __ SBC P6 ; (v + 2)
0dd5 : 85 13 __ STA P6 ; (v + 2)
0dd7 : a9 00 __ LDA #$00
0dd9 : e5 14 __ SBC P7 ; (v + 3)
0ddb : 85 14 __ STA P7 ; (v + 3)
0ddd : e6 43 __ INC T4 + 0 
.s5:
0ddf : a9 10 __ LDA #$10
0de1 : 85 44 __ STA T5 + 0 
0de3 : a5 14 __ LDA P7 ; (v + 3)
0de5 : d0 0c __ BNE $0df3 ; (nforml.l43 + 0)
.s31:
0de7 : a5 13 __ LDA P6 ; (v + 2)
0de9 : d0 08 __ BNE $0df3 ; (nforml.l43 + 0)
.s32:
0deb : a5 12 __ LDA P5 ; (v + 1)
0ded : d0 04 __ BNE $0df3 ; (nforml.l43 + 0)
.s33:
0def : c5 11 __ CMP P4 ; (v + 0)
0df1 : b0 13 __ BCS $0e06 ; (nforml.s6 + 0)
.l43:
0df3 : a5 11 __ LDA P4 ; (v + 0)
0df5 : 85 1b __ STA ACCU + 0 
0df7 : a5 12 __ LDA P5 ; (v + 1)
0df9 : 85 1c __ STA ACCU + 1 
0dfb : a5 13 __ LDA P6 ; (v + 2)
0dfd : 85 1d __ STA ACCU + 2 
0dff : a5 14 __ LDA P7 ; (v + 3)
0e01 : 85 1e __ STA ACCU + 3 
0e03 : 4c c2 0e JMP $0ec2 ; (nforml.l28 + 0)
.s6:
0e06 : a9 ff __ LDA #$ff
0e08 : a0 02 __ LDY #$02
0e0a : d1 0d __ CMP (P0),y ; (si + 0)
0e0c : d0 04 __ BNE $0e12 ; (nforml.s27 + 0)
.s7:
0e0e : a9 0f __ LDA #$0f
0e10 : d0 05 __ BNE $0e17 ; (nforml.s41 + 0)
.s27:
0e12 : 38 __ __ SEC
0e13 : a9 10 __ LDA #$10
0e15 : f1 0d __ SBC (P0),y ; (si + 0)
.s41:
0e17 : a8 __ __ TAY
0e18 : c4 44 __ CPY T5 + 0 
0e1a : b0 0d __ BCS $0e29 ; (nforml.s8 + 0)
.s26:
0e1c : a9 30 __ LDA #$30
.l42:
0e1e : a6 44 __ LDX T5 + 0 
0e20 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e23 : c6 44 __ DEC T5 + 0 
0e25 : c4 44 __ CPY T5 + 0 
0e27 : 90 f5 __ BCC $0e1e ; (nforml.l42 + 0)
.s8:
0e29 : a0 07 __ LDY #$07
0e2b : b1 0d __ LDA (P0),y ; (si + 0)
0e2d : f0 1d __ BEQ $0e4c ; (nforml.s9 + 0)
.s24:
0e2f : a9 10 __ LDA #$10
0e31 : a0 04 __ LDY #$04
0e33 : d1 0d __ CMP (P0),y ; (si + 0)
0e35 : d0 15 __ BNE $0e4c ; (nforml.s9 + 0)
.s25:
0e37 : 88 __ __ DEY
0e38 : b1 0d __ LDA (P0),y ; (si + 0)
0e3a : a8 __ __ TAY
0e3b : a9 30 __ LDA #$30
0e3d : a6 44 __ LDX T5 + 0 
0e3f : 9d e8 9f STA $9fe8,x ; (buff[0] + 48)
0e42 : 98 __ __ TYA
0e43 : 69 16 __ ADC #$16
0e45 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e48 : ca __ __ DEX
0e49 : ca __ __ DEX
0e4a : 86 44 __ STX T5 + 0 
.s9:
0e4c : a9 00 __ LDA #$00
0e4e : 85 1b __ STA ACCU + 0 
0e50 : a5 43 __ LDA T4 + 0 
0e52 : f0 0c __ BEQ $0e60 ; (nforml.s10 + 0)
.s23:
0e54 : a9 2d __ LDA #$2d
.s22:
0e56 : a6 44 __ LDX T5 + 0 
0e58 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e5b : c6 44 __ DEC T5 + 0 
0e5d : 4c 6a 0e JMP $0e6a ; (nforml.s11 + 0)
.s10:
0e60 : a0 05 __ LDY #$05
0e62 : b1 0d __ LDA (P0),y ; (si + 0)
0e64 : f0 04 __ BEQ $0e6a ; (nforml.s11 + 0)
.s21:
0e66 : a9 2b __ LDA #$2b
0e68 : d0 ec __ BNE $0e56 ; (nforml.s22 + 0)
.s11:
0e6a : a0 06 __ LDY #$06
0e6c : a6 44 __ LDX T5 + 0 
0e6e : b1 0d __ LDA (P0),y ; (si + 0)
0e70 : d0 29 __ BNE $0e9b ; (nforml.s17 + 0)
.l12:
0e72 : 8a __ __ TXA
0e73 : 18 __ __ CLC
0e74 : a0 01 __ LDY #$01
0e76 : 71 0d __ ADC (P0),y ; (si + 0)
0e78 : b0 04 __ BCS $0e7e ; (nforml.s15 + 0)
.s16:
0e7a : c9 11 __ CMP #$11
0e7c : 90 0a __ BCC $0e88 ; (nforml.s13 + 0)
.s15:
0e7e : a0 00 __ LDY #$00
0e80 : b1 0d __ LDA (P0),y ; (si + 0)
0e82 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0e85 : ca __ __ DEX
0e86 : b0 ea __ BCS $0e72 ; (nforml.l12 + 0)
.s13:
0e88 : e0 10 __ CPX #$10
0e8a : b0 0e __ BCS $0e9a ; (nforml.s3 + 0)
.s14:
0e8c : 88 __ __ DEY
.l39:
0e8d : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0e90 : 91 0f __ STA (P2),y ; (str + 0)
0e92 : c8 __ __ INY
0e93 : e8 __ __ INX
0e94 : e0 10 __ CPX #$10
0e96 : 90 f5 __ BCC $0e8d ; (nforml.l39 + 0)
.s40:
0e98 : 84 1b __ STY ACCU + 0 
.s3:
0e9a : 60 __ __ RTS
.s17:
0e9b : e0 10 __ CPX #$10
0e9d : b0 1a __ BCS $0eb9 ; (nforml.l18 + 0)
.s20:
0e9f : a0 00 __ LDY #$00
.l37:
0ea1 : bd ea 9f LDA $9fea,x ; (buffer[0] + 0)
0ea4 : 91 0f __ STA (P2),y ; (str + 0)
0ea6 : c8 __ __ INY
0ea7 : e8 __ __ INX
0ea8 : e0 10 __ CPX #$10
0eaa : 90 f5 __ BCC $0ea1 ; (nforml.l37 + 0)
.s38:
0eac : 84 1b __ STY ACCU + 0 
0eae : b0 09 __ BCS $0eb9 ; (nforml.l18 + 0)
.s19:
0eb0 : 88 __ __ DEY
0eb1 : b1 0d __ LDA (P0),y ; (si + 0)
0eb3 : a4 1b __ LDY ACCU + 0 
0eb5 : 91 0f __ STA (P2),y ; (str + 0)
0eb7 : e6 1b __ INC ACCU + 0 
.l18:
0eb9 : a5 1b __ LDA ACCU + 0 
0ebb : a0 01 __ LDY #$01
0ebd : d1 0d __ CMP (P0),y ; (si + 0)
0ebf : 90 ef __ BCC $0eb0 ; (nforml.s19 + 0)
0ec1 : 60 __ __ RTS
.l28:
0ec2 : a0 04 __ LDY #$04
0ec4 : b1 0d __ LDA (P0),y ; (si + 0)
0ec6 : 85 03 __ STA WORK + 0 
0ec8 : a9 00 __ LDA #$00
0eca : 85 04 __ STA WORK + 1 
0ecc : 85 05 __ STA WORK + 2 
0ece : 85 06 __ STA WORK + 3 
0ed0 : 20 67 18 JSR $1867 ; (divmod32 + 0)
0ed3 : a5 07 __ LDA WORK + 4 
0ed5 : c9 0a __ CMP #$0a
0ed7 : b0 04 __ BCS $0edd ; (nforml.s34 + 0)
.s29:
0ed9 : a9 30 __ LDA #$30
0edb : 90 06 __ BCC $0ee3 ; (nforml.s30 + 0)
.s34:
0edd : a0 03 __ LDY #$03
0edf : b1 0d __ LDA (P0),y ; (si + 0)
0ee1 : e9 0a __ SBC #$0a
.s30:
0ee3 : 18 __ __ CLC
0ee4 : 65 07 __ ADC WORK + 4 
0ee6 : a6 44 __ LDX T5 + 0 
0ee8 : 9d e9 9f STA $9fe9,x ; (buff[0] + 49)
0eeb : a5 1b __ LDA ACCU + 0 
0eed : 85 11 __ STA P4 ; (v + 0)
0eef : a5 1c __ LDA ACCU + 1 
0ef1 : 85 12 __ STA P5 ; (v + 1)
0ef3 : a5 1d __ LDA ACCU + 2 
0ef5 : 85 13 __ STA P6 ; (v + 2)
0ef7 : a5 1e __ LDA ACCU + 3 
0ef9 : 85 14 __ STA P7 ; (v + 3)
0efb : c6 44 __ DEC T5 + 0 
0efd : aa __ __ TAX
0efe : d0 c2 __ BNE $0ec2 ; (nforml.l28 + 0)
0f00 : 4c e7 0d JMP $0de7 ; (nforml.s31 + 0)
--------------------------------------------------------------------
nformf: ; nformf(const struct sinfo*,u8*,float,u8)->u8
; 199, "C:/oscar64/include/stdio.c"
.s1:
0f03 : a5 53 __ LDA T10 + 0 
0f05 : 8d f1 9f STA $9ff1 ; (nformf@stack + 0)
0f08 : a5 54 __ LDA T11 + 0 
0f0a : 8d f2 9f STA $9ff2 ; (nformf@stack + 1)
.s4:
0f0d : a5 11 __ LDA P4 ; (f + 0)
0f0f : 85 43 __ STA T0 + 0 
0f11 : a5 12 __ LDA P5 ; (f + 1)
0f13 : 85 44 __ STA T0 + 1 
0f15 : a5 14 __ LDA P7 ; (f + 3)
0f17 : 29 7f __ AND #$7f
0f19 : 05 13 __ ORA P6 ; (f + 2)
0f1b : 05 12 __ ORA P5 ; (f + 1)
0f1d : a6 13 __ LDX P6 ; (f + 2)
0f1f : 86 45 __ STX T0 + 2 
0f21 : 05 11 __ ORA P4 ; (f + 0)
0f23 : f0 14 __ BEQ $0f39 ; (nformf.s5 + 0)
.s89:
0f25 : 24 14 __ BIT P7 ; (f + 3)
0f27 : 10 10 __ BPL $0f39 ; (nformf.s5 + 0)
.s88:
0f29 : a9 2d __ LDA #$2d
0f2b : a0 00 __ LDY #$00
0f2d : 91 0f __ STA (P2),y ; (str + 0)
0f2f : a5 14 __ LDA P7 ; (f + 3)
0f31 : 49 80 __ EOR #$80
0f33 : 85 14 __ STA P7 ; (f + 3)
.s87:
0f35 : a9 01 __ LDA #$01
0f37 : d0 0e __ BNE $0f47 ; (nformf.s6 + 0)
.s5:
0f39 : a0 05 __ LDY #$05
0f3b : b1 0d __ LDA (P0),y ; (si + 0)
0f3d : f0 08 __ BEQ $0f47 ; (nformf.s6 + 0)
.s86:
0f3f : a9 2b __ LDA #$2b
0f41 : a0 00 __ LDY #$00
0f43 : 91 0f __ STA (P2),y ; (str + 0)
0f45 : a9 01 __ LDA #$01
.s6:
0f47 : 85 52 __ STA T9 + 0 
0f49 : 8a __ __ TXA
0f4a : 0a __ __ ASL
0f4b : a5 14 __ LDA P7 ; (f + 3)
0f4d : 2a __ __ ROL
0f4e : c9 ff __ CMP #$ff
0f50 : d0 29 __ BNE $0f7b ; (nformf.s7 + 0)
.s85:
0f52 : a0 03 __ LDY #$03
0f54 : b1 0d __ LDA (P0),y ; (si + 0)
0f56 : 69 07 __ ADC #$07
0f58 : a4 52 __ LDY T9 + 0 
0f5a : 91 0f __ STA (P2),y ; (str + 0)
0f5c : 18 __ __ CLC
0f5d : a0 03 __ LDY #$03
0f5f : b1 0d __ LDA (P0),y ; (si + 0)
0f61 : 69 0d __ ADC #$0d
0f63 : a4 52 __ LDY T9 + 0 
0f65 : c8 __ __ INY
0f66 : 91 0f __ STA (P2),y ; (str + 0)
0f68 : 18 __ __ CLC
0f69 : a0 03 __ LDY #$03
0f6b : b1 0d __ LDA (P0),y ; (si + 0)
0f6d : 69 05 __ ADC #$05
0f6f : a4 52 __ LDY T9 + 0 
0f71 : c8 __ __ INY
0f72 : c8 __ __ INY
0f73 : 91 0f __ STA (P2),y ; (str + 0)
0f75 : c8 __ __ INY
0f76 : 84 52 __ STY T9 + 0 
0f78 : 4c 4d 12 JMP $124d ; (nformf.s27 + 0)
.s7:
0f7b : a0 02 __ LDY #$02
0f7d : b1 0d __ LDA (P0),y ; (si + 0)
0f7f : a6 14 __ LDX P7 ; (f + 3)
0f81 : 86 46 __ STX T0 + 3 
0f83 : c9 ff __ CMP #$ff
0f85 : d0 02 __ BNE $0f89 ; (nformf.s84 + 0)
.s8:
0f87 : a9 06 __ LDA #$06
.s84:
0f89 : 85 50 __ STA T7 + 0 
0f8b : a9 00 __ LDA #$00
0f8d : 85 4d __ STA T5 + 0 
0f8f : 85 4e __ STA T5 + 1 
0f91 : 8a __ __ TXA
0f92 : 29 7f __ AND #$7f
0f94 : 05 13 __ ORA P6 ; (f + 2)
0f96 : 05 12 __ ORA P5 ; (f + 1)
0f98 : 05 11 __ ORA P4 ; (f + 0)
0f9a : d0 03 __ BNE $0f9f ; (nformf.s63 + 0)
0f9c : 4c 71 10 JMP $1071 ; (nformf.s9 + 0)
.s63:
0f9f : 8a __ __ TXA
0fa0 : 30 51 __ BMI $0ff3 ; (nformf.l72 + 0)
.s81:
0fa2 : c9 44 __ CMP #$44
0fa4 : d0 06 __ BNE $0fac ; (nformf.l83 + 0)
.s82:
0fa6 : a5 13 __ LDA P6 ; (f + 2)
0fa8 : c9 7a __ CMP #$7a
0faa : f0 02 __ BEQ $0fae ; (nformf.l78 + 0)
.l83:
0fac : 90 35 __ BCC $0fe3 ; (nformf.s64 + 0)
.l78:
0fae : 18 __ __ CLC
0faf : a5 4d __ LDA T5 + 0 
0fb1 : 69 03 __ ADC #$03
0fb3 : 85 4d __ STA T5 + 0 
0fb5 : 90 02 __ BCC $0fb9 ; (nformf.s103 + 0)
.s102:
0fb7 : e6 4e __ INC T5 + 1 
.s103:
0fb9 : a9 7a __ LDA #$7a
0fbb : 85 05 __ STA WORK + 2 
0fbd : a9 44 __ LDA #$44
0fbf : 85 06 __ STA WORK + 3 
0fc1 : 20 61 19 JSR $1961 ; (freg@proxy + 0)
0fc4 : 20 87 15 JSR $1587 ; (crt_fdiv + 0)
0fc7 : a5 1b __ LDA ACCU + 0 
0fc9 : 85 43 __ STA T0 + 0 
0fcb : a5 1c __ LDA ACCU + 1 
0fcd : 85 44 __ STA T0 + 1 
0fcf : a6 1d __ LDX ACCU + 2 
0fd1 : 86 45 __ STX T0 + 2 
0fd3 : a5 1e __ LDA ACCU + 3 
0fd5 : 85 46 __ STA T0 + 3 
0fd7 : 30 0a __ BMI $0fe3 ; (nformf.s64 + 0)
.s79:
0fd9 : c9 44 __ CMP #$44
0fdb : d0 cf __ BNE $0fac ; (nformf.l83 + 0)
.s80:
0fdd : e0 7a __ CPX #$7a
0fdf : f0 cd __ BEQ $0fae ; (nformf.l78 + 0)
0fe1 : d0 c9 __ BNE $0fac ; (nformf.l83 + 0)
.s64:
0fe3 : a5 46 __ LDA T0 + 3 
0fe5 : 30 0c __ BMI $0ff3 ; (nformf.l72 + 0)
.s76:
0fe7 : c9 3f __ CMP #$3f
0fe9 : d0 06 __ BNE $0ff1 ; (nformf.s75 + 0)
.s77:
0feb : a5 45 __ LDA T0 + 2 
0fed : c9 80 __ CMP #$80
0fef : f0 40 __ BEQ $1031 ; (nformf.s65 + 0)
.s75:
0ff1 : b0 3e __ BCS $1031 ; (nformf.s65 + 0)
.l72:
0ff3 : 38 __ __ SEC
0ff4 : a5 4d __ LDA T5 + 0 
0ff6 : e9 03 __ SBC #$03
0ff8 : 85 4d __ STA T5 + 0 
0ffa : b0 02 __ BCS $0ffe ; (nformf.s98 + 0)
.s97:
0ffc : c6 4e __ DEC T5 + 1 
.s98:
0ffe : a9 00 __ LDA #$00
1000 : 85 1b __ STA ACCU + 0 
1002 : 85 1c __ STA ACCU + 1 
1004 : a9 7a __ LDA #$7a
1006 : 85 1d __ STA ACCU + 2 
1008 : a9 44 __ LDA #$44
100a : 85 1e __ STA ACCU + 3 
100c : a2 43 __ LDX #$43
100e : 20 91 13 JSR $1391 ; (freg + 4)
1011 : 20 bf 14 JSR $14bf ; (crt_fmul + 0)
1014 : a5 1b __ LDA ACCU + 0 
1016 : 85 43 __ STA T0 + 0 
1018 : a5 1c __ LDA ACCU + 1 
101a : 85 44 __ STA T0 + 1 
101c : a6 1d __ LDX ACCU + 2 
101e : 86 45 __ STX T0 + 2 
1020 : a5 1e __ LDA ACCU + 3 
1022 : 85 46 __ STA T0 + 3 
1024 : 30 cd __ BMI $0ff3 ; (nformf.l72 + 0)
.s73:
1026 : c9 3f __ CMP #$3f
1028 : 90 c9 __ BCC $0ff3 ; (nformf.l72 + 0)
.s106:
102a : d0 05 __ BNE $1031 ; (nformf.s65 + 0)
.s74:
102c : e0 80 __ CPX #$80
102e : 4c f1 0f JMP $0ff1 ; (nformf.s75 + 0)
.s65:
1031 : a5 46 __ LDA T0 + 3 
1033 : 30 3c __ BMI $1071 ; (nformf.s9 + 0)
.s69:
1035 : c9 41 __ CMP #$41
1037 : d0 06 __ BNE $103f ; (nformf.l71 + 0)
.s70:
1039 : a5 45 __ LDA T0 + 2 
103b : c9 20 __ CMP #$20
103d : f0 02 __ BEQ $1041 ; (nformf.l66 + 0)
.l71:
103f : 90 30 __ BCC $1071 ; (nformf.s9 + 0)
.l66:
1041 : e6 4d __ INC T5 + 0 
1043 : d0 02 __ BNE $1047 ; (nformf.s101 + 0)
.s100:
1045 : e6 4e __ INC T5 + 1 
.s101:
1047 : a9 20 __ LDA #$20
1049 : 85 05 __ STA WORK + 2 
104b : a9 41 __ LDA #$41
104d : 85 06 __ STA WORK + 3 
104f : 20 61 19 JSR $1961 ; (freg@proxy + 0)
1052 : 20 87 15 JSR $1587 ; (crt_fdiv + 0)
1055 : a5 1b __ LDA ACCU + 0 
1057 : 85 43 __ STA T0 + 0 
1059 : a5 1c __ LDA ACCU + 1 
105b : 85 44 __ STA T0 + 1 
105d : a6 1d __ LDX ACCU + 2 
105f : 86 45 __ STX T0 + 2 
1061 : a5 1e __ LDA ACCU + 3 
1063 : 85 46 __ STA T0 + 3 
1065 : 30 0a __ BMI $1071 ; (nformf.s9 + 0)
.s67:
1067 : c9 41 __ CMP #$41
1069 : d0 d4 __ BNE $103f ; (nformf.l71 + 0)
.s68:
106b : e0 20 __ CPX #$20
106d : f0 d2 __ BEQ $1041 ; (nformf.l66 + 0)
106f : d0 ce __ BNE $103f ; (nformf.l71 + 0)
.s9:
1071 : a5 15 __ LDA P8 ; (type + 0)
1073 : c9 65 __ CMP #$65
1075 : d0 04 __ BNE $107b ; (nformf.s11 + 0)
.s10:
1077 : a9 01 __ LDA #$01
1079 : d0 02 __ BNE $107d ; (nformf.s12 + 0)
.s11:
107b : a9 00 __ LDA #$00
.s12:
107d : 85 53 __ STA T10 + 0 
107f : a6 50 __ LDX T7 + 0 
1081 : e8 __ __ INX
1082 : 86 4f __ STX T6 + 0 
1084 : a5 15 __ LDA P8 ; (type + 0)
1086 : c9 67 __ CMP #$67
1088 : d0 13 __ BNE $109d ; (nformf.s13 + 0)
.s59:
108a : a5 4e __ LDA T5 + 1 
108c : 30 08 __ BMI $1096 ; (nformf.s60 + 0)
.s62:
108e : d0 06 __ BNE $1096 ; (nformf.s60 + 0)
.s61:
1090 : a5 4d __ LDA T5 + 0 
1092 : c9 04 __ CMP #$04
1094 : 90 07 __ BCC $109d ; (nformf.s13 + 0)
.s60:
1096 : a9 01 __ LDA #$01
1098 : 85 53 __ STA T10 + 0 
109a : 4c da 12 JMP $12da ; (nformf.s51 + 0)
.s13:
109d : a5 53 __ LDA T10 + 0 
109f : d0 f9 __ BNE $109a ; (nformf.s60 + 4)
.s14:
10a1 : 24 4e __ BIT T5 + 1 
10a3 : 10 35 __ BPL $10da ; (nformf.s15 + 0)
.s50:
10a5 : a5 43 __ LDA T0 + 0 
10a7 : 85 1b __ STA ACCU + 0 
10a9 : a5 44 __ LDA T0 + 1 
10ab : 85 1c __ STA ACCU + 1 
10ad : a5 45 __ LDA T0 + 2 
10af : 85 1d __ STA ACCU + 2 
10b1 : a5 46 __ LDA T0 + 3 
10b3 : 85 1e __ STA ACCU + 3 
.l90:
10b5 : 20 4e 19 JSR $194e ; (freg@proxy + 0)
10b8 : 20 87 15 JSR $1587 ; (crt_fdiv + 0)
10bb : 18 __ __ CLC
10bc : a5 4d __ LDA T5 + 0 
10be : 69 01 __ ADC #$01
10c0 : 85 4d __ STA T5 + 0 
10c2 : a5 4e __ LDA T5 + 1 
10c4 : 69 00 __ ADC #$00
10c6 : 85 4e __ STA T5 + 1 
10c8 : 30 eb __ BMI $10b5 ; (nformf.l90 + 0)
.s91:
10ca : a5 1e __ LDA ACCU + 3 
10cc : 85 46 __ STA T0 + 3 
10ce : a5 1d __ LDA ACCU + 2 
10d0 : 85 45 __ STA T0 + 2 
10d2 : a5 1c __ LDA ACCU + 1 
10d4 : 85 44 __ STA T0 + 1 
10d6 : a5 1b __ LDA ACCU + 0 
10d8 : 85 43 __ STA T0 + 0 
.s15:
10da : 18 __ __ CLC
10db : a5 50 __ LDA T7 + 0 
10dd : 65 4d __ ADC T5 + 0 
10df : 18 __ __ CLC
10e0 : 69 01 __ ADC #$01
10e2 : 85 4f __ STA T6 + 0 
10e4 : c9 07 __ CMP #$07
10e6 : 90 14 __ BCC $10fc ; (nformf.s49 + 0)
.s16:
10e8 : ad a8 19 LDA $19a8 ; (fround5[0] + 24)
10eb : 85 47 __ STA T1 + 0 
10ed : ad a9 19 LDA $19a9 ; (fround5[0] + 25)
10f0 : 85 48 __ STA T1 + 1 
10f2 : ad aa 19 LDA $19aa ; (fround5[0] + 26)
10f5 : 85 49 __ STA T1 + 2 
10f7 : ad ab 19 LDA $19ab ; (fround5[0] + 27)
10fa : b0 15 __ BCS $1111 ; (nformf.s17 + 0)
.s49:
10fc : 0a __ __ ASL
10fd : 0a __ __ ASL
10fe : aa __ __ TAX
10ff : bd 8c 19 LDA $198c,x ; (freg@proxy + 16)
1102 : 85 47 __ STA T1 + 0 
1104 : bd 8d 19 LDA $198d,x ; (freg@proxy + 17)
1107 : 85 48 __ STA T1 + 1 
1109 : bd 8e 19 LDA $198e,x ; (freg@proxy + 18)
110c : 85 49 __ STA T1 + 2 
110e : bd 8f 19 LDA $198f,x ; (spentry + 0)
.s17:
1111 : 85 4a __ STA T1 + 3 
1113 : a2 47 __ LDX #$47
1115 : 20 7c 19 JSR $197c ; (freg@proxy + 0)
1118 : 20 d8 13 JSR $13d8 ; (faddsub + 6)
111b : a5 1c __ LDA ACCU + 1 
111d : 85 12 __ STA P5 ; (f + 1)
111f : a5 1d __ LDA ACCU + 2 
1121 : 85 13 __ STA P6 ; (f + 2)
1123 : a6 1b __ LDX ACCU + 0 
1125 : a5 1e __ LDA ACCU + 3 
1127 : 85 14 __ STA P7 ; (f + 3)
1129 : 30 22 __ BMI $114d ; (nformf.s18 + 0)
.s46:
112b : c9 41 __ CMP #$41
112d : d0 06 __ BNE $1135 ; (nformf.s48 + 0)
.s47:
112f : a5 13 __ LDA P6 ; (f + 2)
1131 : c9 20 __ CMP #$20
1133 : f0 02 __ BEQ $1137 ; (nformf.s45 + 0)
.s48:
1135 : 90 16 __ BCC $114d ; (nformf.s18 + 0)
.s45:
1137 : 20 4e 19 JSR $194e ; (freg@proxy + 0)
113a : 20 87 15 JSR $1587 ; (crt_fdiv + 0)
113d : a5 1c __ LDA ACCU + 1 
113f : 85 12 __ STA P5 ; (f + 1)
1141 : a5 1d __ LDA ACCU + 2 
1143 : 85 13 __ STA P6 ; (f + 2)
1145 : a5 1e __ LDA ACCU + 3 
1147 : 85 14 __ STA P7 ; (f + 3)
1149 : c6 50 __ DEC T7 + 0 
114b : a6 1b __ LDX ACCU + 0 
.s18:
114d : 38 __ __ SEC
114e : a5 4f __ LDA T6 + 0 
1150 : e5 50 __ SBC T7 + 0 
1152 : 85 4b __ STA T4 + 0 
1154 : a5 4f __ LDA T6 + 0 
1156 : c9 15 __ CMP #$15
1158 : 90 04 __ BCC $115e ; (nformf.s19 + 0)
.s44:
115a : a9 14 __ LDA #$14
115c : 85 4f __ STA T6 + 0 
.s19:
115e : a5 4b __ LDA T4 + 0 
1160 : d0 08 __ BNE $116a ; (nformf.s21 + 0)
.s20:
1162 : a9 30 __ LDA #$30
1164 : a4 52 __ LDY T9 + 0 
1166 : 91 0f __ STA (P2),y ; (str + 0)
1168 : e6 52 __ INC T9 + 0 
.s21:
116a : a9 00 __ LDA #$00
116c : 85 54 __ STA T11 + 0 
116e : c5 4b __ CMP T4 + 0 
1170 : f0 61 __ BEQ $11d3 ; (nformf.l43 + 0)
.s23:
1172 : c9 07 __ CMP #$07
1174 : 90 04 __ BCC $117a ; (nformf.s24 + 0)
.l42:
1176 : a9 30 __ LDA #$30
1178 : b0 47 __ BCS $11c1 ; (nformf.l25 + 0)
.s24:
117a : 86 1b __ STX ACCU + 0 
117c : 86 43 __ STX T0 + 0 
117e : a5 12 __ LDA P5 ; (f + 1)
1180 : 85 1c __ STA ACCU + 1 
1182 : 85 44 __ STA T0 + 1 
1184 : a5 13 __ LDA P6 ; (f + 2)
1186 : 85 1d __ STA ACCU + 2 
1188 : 85 45 __ STA T0 + 2 
118a : a5 14 __ LDA P7 ; (f + 3)
118c : 85 1e __ STA ACCU + 3 
118e : 85 46 __ STA T0 + 3 
1190 : 20 4e 17 JSR $174e ; (f32_to_i16 + 0)
1193 : a5 1b __ LDA ACCU + 0 
1195 : 85 51 __ STA T8 + 0 
1197 : 20 9a 17 JSR $179a ; (sint16_to_float + 0)
119a : a2 43 __ LDX #$43
119c : 20 91 13 JSR $1391 ; (freg + 4)
119f : a5 1e __ LDA ACCU + 3 
11a1 : 49 80 __ EOR #$80
11a3 : 85 1e __ STA ACCU + 3 
11a5 : 20 d8 13 JSR $13d8 ; (faddsub + 6)
11a8 : 20 4e 19 JSR $194e ; (freg@proxy + 0)
11ab : 20 bf 14 JSR $14bf ; (crt_fmul + 0)
11ae : a5 1c __ LDA ACCU + 1 
11b0 : 85 12 __ STA P5 ; (f + 1)
11b2 : a5 1d __ LDA ACCU + 2 
11b4 : 85 13 __ STA P6 ; (f + 2)
11b6 : a5 1e __ LDA ACCU + 3 
11b8 : 85 14 __ STA P7 ; (f + 3)
11ba : 18 __ __ CLC
11bb : a5 51 __ LDA T8 + 0 
11bd : 69 30 __ ADC #$30
11bf : a6 1b __ LDX ACCU + 0 
.l25:
11c1 : a4 52 __ LDY T9 + 0 
11c3 : 91 0f __ STA (P2),y ; (str + 0)
11c5 : e6 52 __ INC T9 + 0 
11c7 : e6 54 __ INC T11 + 0 
11c9 : a5 54 __ LDA T11 + 0 
11cb : c5 4f __ CMP T6 + 0 
11cd : b0 14 __ BCS $11e3 ; (nformf.s26 + 0)
.s22:
11cf : c5 4b __ CMP T4 + 0 
11d1 : d0 9f __ BNE $1172 ; (nformf.s23 + 0)
.l43:
11d3 : a9 2e __ LDA #$2e
11d5 : a4 52 __ LDY T9 + 0 
11d7 : 91 0f __ STA (P2),y ; (str + 0)
11d9 : a5 54 __ LDA T11 + 0 
11db : c9 07 __ CMP #$07
11dd : e6 52 __ INC T9 + 0 
11df : 90 99 __ BCC $117a ; (nformf.s24 + 0)
11e1 : b0 93 __ BCS $1176 ; (nformf.l42 + 0)
.s26:
11e3 : a5 53 __ LDA T10 + 0 
11e5 : f0 66 __ BEQ $124d ; (nformf.s27 + 0)
.s38:
11e7 : a0 03 __ LDY #$03
11e9 : b1 0d __ LDA (P0),y ; (si + 0)
11eb : 69 03 __ ADC #$03
11ed : a4 52 __ LDY T9 + 0 
11ef : 91 0f __ STA (P2),y ; (str + 0)
11f1 : c8 __ __ INY
11f2 : 84 52 __ STY T9 + 0 
11f4 : 24 4e __ BIT T5 + 1 
11f6 : 30 06 __ BMI $11fe ; (nformf.s41 + 0)
.s39:
11f8 : a9 2b __ LDA #$2b
11fa : 91 0f __ STA (P2),y ; (str + 0)
11fc : d0 11 __ BNE $120f ; (nformf.s40 + 0)
.s41:
11fe : a9 2d __ LDA #$2d
1200 : 91 0f __ STA (P2),y ; (str + 0)
1202 : 38 __ __ SEC
1203 : a9 00 __ LDA #$00
1205 : e5 4d __ SBC T5 + 0 
1207 : 85 4d __ STA T5 + 0 
1209 : a9 00 __ LDA #$00
120b : e5 4e __ SBC T5 + 1 
120d : 85 4e __ STA T5 + 1 
.s40:
120f : a5 4d __ LDA T5 + 0 
1211 : 85 1b __ STA ACCU + 0 
1213 : a5 4e __ LDA T5 + 1 
1215 : 85 1c __ STA ACCU + 1 
1217 : e6 52 __ INC T9 + 0 
1219 : a9 0a __ LDA #$0a
121b : 85 03 __ STA WORK + 0 
121d : a9 00 __ LDA #$00
121f : 85 04 __ STA WORK + 1 
1221 : 20 35 16 JSR $1635 ; (divs16 + 0)
1224 : 18 __ __ CLC
1225 : a5 1b __ LDA ACCU + 0 
1227 : 69 30 __ ADC #$30
1229 : a4 52 __ LDY T9 + 0 
122b : 91 0f __ STA (P2),y ; (str + 0)
122d : a5 4d __ LDA T5 + 0 
122f : 85 1b __ STA ACCU + 0 
1231 : a5 4e __ LDA T5 + 1 
1233 : 85 1c __ STA ACCU + 1 
1235 : e6 52 __ INC T9 + 0 
1237 : a9 0a __ LDA #$0a
1239 : 85 03 __ STA WORK + 0 
123b : a9 00 __ LDA #$00
123d : 85 04 __ STA WORK + 1 
123f : 20 f7 16 JSR $16f7 ; (mods16 + 0)
1242 : 18 __ __ CLC
1243 : a5 05 __ LDA WORK + 2 
1245 : 69 30 __ ADC #$30
1247 : a4 52 __ LDY T9 + 0 
1249 : 91 0f __ STA (P2),y ; (str + 0)
124b : e6 52 __ INC T9 + 0 
.s27:
124d : a5 52 __ LDA T9 + 0 
124f : a0 01 __ LDY #$01
1251 : d1 0d __ CMP (P0),y ; (si + 0)
1253 : b0 69 __ BCS $12be ; (nformf.s3 + 0)
.s28:
1255 : a0 06 __ LDY #$06
1257 : b1 0d __ LDA (P0),y ; (si + 0)
1259 : f0 04 __ BEQ $125f ; (nformf.s29 + 0)
.s92:
125b : a6 52 __ LDX T9 + 0 
125d : 90 6c __ BCC $12cb ; (nformf.l36 + 0)
.s29:
125f : a5 52 __ LDA T9 + 0 
1261 : f0 3c __ BEQ $129f ; (nformf.s30 + 0)
.s35:
1263 : e9 00 __ SBC #$00
1265 : a2 00 __ LDX #$00
1267 : b0 01 __ BCS $126a ; (nformf.s105 + 0)
.s104:
1269 : ca __ __ DEX
.s105:
126a : 18 __ __ CLC
126b : 65 0f __ ADC P2 ; (str + 0)
126d : 85 43 __ STA T0 + 0 
126f : 8a __ __ TXA
1270 : 65 10 __ ADC P3 ; (str + 1)
1272 : 85 44 __ STA T0 + 1 
1274 : a9 01 __ LDA #$01
1276 : 85 47 __ STA T1 + 0 
1278 : a6 10 __ LDX P3 ; (str + 1)
127a : 38 __ __ SEC
.l93:
127b : a0 01 __ LDY #$01
127d : b1 0d __ LDA (P0),y ; (si + 0)
127f : e5 47 __ SBC T1 + 0 
1281 : 85 4b __ STA T4 + 0 
1283 : 8a __ __ TXA
1284 : 69 ff __ ADC #$ff
1286 : 85 4c __ STA T4 + 1 
1288 : 88 __ __ DEY
1289 : b1 43 __ LDA (T0 + 0),y 
128b : a4 0f __ LDY P2 ; (str + 0)
128d : 91 4b __ STA (T4 + 0),y 
128f : a5 43 __ LDA T0 + 0 
1291 : d0 02 __ BNE $1295 ; (nformf.s96 + 0)
.s95:
1293 : c6 44 __ DEC T0 + 1 
.s96:
1295 : c6 43 __ DEC T0 + 0 
1297 : e6 47 __ INC T1 + 0 
1299 : a5 52 __ LDA T9 + 0 
129b : c5 47 __ CMP T1 + 0 
129d : b0 dc __ BCS $127b ; (nformf.l93 + 0)
.s30:
129f : a9 00 __ LDA #$00
12a1 : 85 4b __ STA T4 + 0 
12a3 : 90 08 __ BCC $12ad ; (nformf.l31 + 0)
.s33:
12a5 : a9 20 __ LDA #$20
12a7 : a4 4b __ LDY T4 + 0 
12a9 : 91 0f __ STA (P2),y ; (str + 0)
12ab : e6 4b __ INC T4 + 0 
.l31:
12ad : a0 01 __ LDY #$01
12af : b1 0d __ LDA (P0),y ; (si + 0)
12b1 : 38 __ __ SEC
12b2 : e5 52 __ SBC T9 + 0 
12b4 : 90 ef __ BCC $12a5 ; (nformf.s33 + 0)
.s34:
12b6 : c5 4b __ CMP T4 + 0 
12b8 : 90 02 __ BCC $12bc ; (nformf.s32 + 0)
.s94:
12ba : d0 e9 __ BNE $12a5 ; (nformf.s33 + 0)
.s32:
12bc : b1 0d __ LDA (P0),y ; (si + 0)
.s3:
12be : 85 1b __ STA ACCU + 0 
12c0 : ad f1 9f LDA $9ff1 ; (nformf@stack + 0)
12c3 : 85 53 __ STA T10 + 0 
12c5 : ad f2 9f LDA $9ff2 ; (nformf@stack + 1)
12c8 : 85 54 __ STA T11 + 0 
12ca : 60 __ __ RTS
.l36:
12cb : 8a __ __ TXA
12cc : a0 01 __ LDY #$01
12ce : d1 0d __ CMP (P0),y ; (si + 0)
12d0 : b0 ea __ BCS $12bc ; (nformf.s32 + 0)
.s37:
12d2 : a8 __ __ TAY
12d3 : a9 20 __ LDA #$20
12d5 : 91 0f __ STA (P2),y ; (str + 0)
12d7 : e8 __ __ INX
12d8 : 90 f1 __ BCC $12cb ; (nformf.l36 + 0)
.s51:
12da : a5 4f __ LDA T6 + 0 
12dc : c9 07 __ CMP #$07
12de : 90 14 __ BCC $12f4 ; (nformf.s58 + 0)
.s52:
12e0 : ad a8 19 LDA $19a8 ; (fround5[0] + 24)
12e3 : 85 47 __ STA T1 + 0 
12e5 : ad a9 19 LDA $19a9 ; (fround5[0] + 25)
12e8 : 85 48 __ STA T1 + 1 
12ea : ad aa 19 LDA $19aa ; (fround5[0] + 26)
12ed : 85 49 __ STA T1 + 2 
12ef : ad ab 19 LDA $19ab ; (fround5[0] + 27)
12f2 : b0 15 __ BCS $1309 ; (nformf.s53 + 0)
.s58:
12f4 : 0a __ __ ASL
12f5 : 0a __ __ ASL
12f6 : aa __ __ TAX
12f7 : bd 8c 19 LDA $198c,x ; (freg@proxy + 16)
12fa : 85 47 __ STA T1 + 0 
12fc : bd 8d 19 LDA $198d,x ; (freg@proxy + 17)
12ff : 85 48 __ STA T1 + 1 
1301 : bd 8e 19 LDA $198e,x ; (freg@proxy + 18)
1304 : 85 49 __ STA T1 + 2 
1306 : bd 8f 19 LDA $198f,x ; (spentry + 0)
.s53:
1309 : 85 4a __ STA T1 + 3 
130b : a2 47 __ LDX #$47
130d : 20 7c 19 JSR $197c ; (freg@proxy + 0)
1310 : 20 d8 13 JSR $13d8 ; (faddsub + 6)
1313 : a5 1c __ LDA ACCU + 1 
1315 : 85 12 __ STA P5 ; (f + 1)
1317 : a5 1d __ LDA ACCU + 2 
1319 : 85 13 __ STA P6 ; (f + 2)
131b : a6 1b __ LDX ACCU + 0 
131d : a5 1e __ LDA ACCU + 3 
131f : 85 14 __ STA P7 ; (f + 3)
1321 : 10 03 __ BPL $1326 ; (nformf.s55 + 0)
1323 : 4c 4d 11 JMP $114d ; (nformf.s18 + 0)
.s55:
1326 : c9 41 __ CMP #$41
1328 : d0 06 __ BNE $1330 ; (nformf.s57 + 0)
.s56:
132a : a5 13 __ LDA P6 ; (f + 2)
132c : c9 20 __ CMP #$20
132e : f0 02 __ BEQ $1332 ; (nformf.s54 + 0)
.s57:
1330 : 90 f1 __ BCC $1323 ; (nformf.s53 + 26)
.s54:
1332 : 20 4e 19 JSR $194e ; (freg@proxy + 0)
1335 : 20 87 15 JSR $1587 ; (crt_fdiv + 0)
1338 : a5 1c __ LDA ACCU + 1 
133a : 85 12 __ STA P5 ; (f + 1)
133c : a5 1d __ LDA ACCU + 2 
133e : 85 13 __ STA P6 ; (f + 2)
1340 : a5 1e __ LDA ACCU + 3 
1342 : 85 14 __ STA P7 ; (f + 3)
1344 : a6 1b __ LDX ACCU + 0 
1346 : e6 4d __ INC T5 + 0 
1348 : d0 d9 __ BNE $1323 ; (nformf.s53 + 26)
.s99:
134a : e6 4e __ INC T5 + 1 
134c : 4c 4d 11 JMP $114d ; (nformf.s18 + 0)
--------------------------------------------------------------------
134f : __ __ __ BYT 25 64 3a 20 25 64 0a 00                         : %d: %d..
--------------------------------------------------------------------
mul32by8: ; mul32by8
1357 : a0 00 __ LDY #$00
1359 : 84 07 __ STY WORK + 4 
135b : 84 08 __ STY WORK + 5 
135d : 84 09 __ STY WORK + 6 
135f : 4a __ __ LSR
1360 : b0 0d __ BCS $136f ; (mul32by8 + 24)
1362 : f0 26 __ BEQ $138a ; (mul32by8 + 51)
1364 : 06 1b __ ASL ACCU + 0 
1366 : 26 1c __ ROL ACCU + 1 
1368 : 26 1d __ ROL ACCU + 2 
136a : 26 1e __ ROL ACCU + 3 
136c : 4a __ __ LSR
136d : 90 f5 __ BCC $1364 ; (mul32by8 + 13)
136f : aa __ __ TAX
1370 : 18 __ __ CLC
1371 : a5 07 __ LDA WORK + 4 
1373 : 65 1b __ ADC ACCU + 0 
1375 : 85 07 __ STA WORK + 4 
1377 : a5 08 __ LDA WORK + 5 
1379 : 65 1c __ ADC ACCU + 1 
137b : 85 08 __ STA WORK + 5 
137d : a5 09 __ LDA WORK + 6 
137f : 65 1d __ ADC ACCU + 2 
1381 : 85 09 __ STA WORK + 6 
1383 : 98 __ __ TYA
1384 : 65 1e __ ADC ACCU + 3 
1386 : a8 __ __ TAY
1387 : 8a __ __ TXA
1388 : d0 da __ BNE $1364 ; (mul32by8 + 13)
138a : 84 0a __ STY WORK + 7 
138c : 60 __ __ RTS
--------------------------------------------------------------------
freg: ; freg
138d : b1 19 __ LDA (IP + 0),y 
138f : c8 __ __ INY
1390 : aa __ __ TAX
1391 : b5 00 __ LDA $00,x 
1393 : 85 03 __ STA WORK + 0 
1395 : b5 01 __ LDA $01,x 
1397 : 85 04 __ STA WORK + 1 
1399 : b5 02 __ LDA $02,x 
139b : 85 05 __ STA WORK + 2 
139d : b5 03 __ LDA WORK + 0,x 
139f : 85 06 __ STA WORK + 3 
13a1 : a5 05 __ LDA WORK + 2 
13a3 : 0a __ __ ASL
13a4 : a5 06 __ LDA WORK + 3 
13a6 : 2a __ __ ROL
13a7 : 85 08 __ STA WORK + 5 
13a9 : f0 06 __ BEQ $13b1 ; (freg + 36)
13ab : a5 05 __ LDA WORK + 2 
13ad : 09 80 __ ORA #$80
13af : 85 05 __ STA WORK + 2 
13b1 : a5 1d __ LDA ACCU + 2 
13b3 : 0a __ __ ASL
13b4 : a5 1e __ LDA ACCU + 3 
13b6 : 2a __ __ ROL
13b7 : 85 07 __ STA WORK + 4 
13b9 : f0 06 __ BEQ $13c1 ; (freg + 52)
13bb : a5 1d __ LDA ACCU + 2 
13bd : 09 80 __ ORA #$80
13bf : 85 1d __ STA ACCU + 2 
13c1 : 60 __ __ RTS
13c2 : 06 1e __ ASL ACCU + 3 
13c4 : a5 07 __ LDA WORK + 4 
13c6 : 6a __ __ ROR
13c7 : 85 1e __ STA ACCU + 3 
13c9 : b0 06 __ BCS $13d1 ; (freg + 68)
13cb : a5 1d __ LDA ACCU + 2 
13cd : 29 7f __ AND #$7f
13cf : 85 1d __ STA ACCU + 2 
13d1 : 60 __ __ RTS
--------------------------------------------------------------------
faddsub: ; faddsub
13d2 : a5 06 __ LDA WORK + 3 
13d4 : 49 80 __ EOR #$80
13d6 : 85 06 __ STA WORK + 3 
13d8 : a9 ff __ LDA #$ff
13da : c5 07 __ CMP WORK + 4 
13dc : f0 04 __ BEQ $13e2 ; (faddsub + 16)
13de : c5 08 __ CMP WORK + 5 
13e0 : d0 11 __ BNE $13f3 ; (faddsub + 33)
13e2 : a5 1e __ LDA ACCU + 3 
13e4 : 09 7f __ ORA #$7f
13e6 : 85 1e __ STA ACCU + 3 
13e8 : a9 80 __ LDA #$80
13ea : 85 1d __ STA ACCU + 2 
13ec : a9 00 __ LDA #$00
13ee : 85 1b __ STA ACCU + 0 
13f0 : 85 1c __ STA ACCU + 1 
13f2 : 60 __ __ RTS
13f3 : 38 __ __ SEC
13f4 : a5 07 __ LDA WORK + 4 
13f6 : e5 08 __ SBC WORK + 5 
13f8 : f0 38 __ BEQ $1432 ; (faddsub + 96)
13fa : aa __ __ TAX
13fb : b0 25 __ BCS $1422 ; (faddsub + 80)
13fd : e0 e9 __ CPX #$e9
13ff : b0 0e __ BCS $140f ; (faddsub + 61)
1401 : a5 08 __ LDA WORK + 5 
1403 : 85 07 __ STA WORK + 4 
1405 : a9 00 __ LDA #$00
1407 : 85 1b __ STA ACCU + 0 
1409 : 85 1c __ STA ACCU + 1 
140b : 85 1d __ STA ACCU + 2 
140d : f0 23 __ BEQ $1432 ; (faddsub + 96)
140f : a5 1d __ LDA ACCU + 2 
1411 : 4a __ __ LSR
1412 : 66 1c __ ROR ACCU + 1 
1414 : 66 1b __ ROR ACCU + 0 
1416 : e8 __ __ INX
1417 : d0 f8 __ BNE $1411 ; (faddsub + 63)
1419 : 85 1d __ STA ACCU + 2 
141b : a5 08 __ LDA WORK + 5 
141d : 85 07 __ STA WORK + 4 
141f : 4c 32 14 JMP $1432 ; (faddsub + 96)
1422 : e0 18 __ CPX #$18
1424 : b0 33 __ BCS $1459 ; (faddsub + 135)
1426 : a5 05 __ LDA WORK + 2 
1428 : 4a __ __ LSR
1429 : 66 04 __ ROR WORK + 1 
142b : 66 03 __ ROR WORK + 0 
142d : ca __ __ DEX
142e : d0 f8 __ BNE $1428 ; (faddsub + 86)
1430 : 85 05 __ STA WORK + 2 
1432 : a5 1e __ LDA ACCU + 3 
1434 : 29 80 __ AND #$80
1436 : 85 1e __ STA ACCU + 3 
1438 : 45 06 __ EOR WORK + 3 
143a : 30 31 __ BMI $146d ; (faddsub + 155)
143c : 18 __ __ CLC
143d : a5 1b __ LDA ACCU + 0 
143f : 65 03 __ ADC WORK + 0 
1441 : 85 1b __ STA ACCU + 0 
1443 : a5 1c __ LDA ACCU + 1 
1445 : 65 04 __ ADC WORK + 1 
1447 : 85 1c __ STA ACCU + 1 
1449 : a5 1d __ LDA ACCU + 2 
144b : 65 05 __ ADC WORK + 2 
144d : 85 1d __ STA ACCU + 2 
144f : 90 08 __ BCC $1459 ; (faddsub + 135)
1451 : 66 1d __ ROR ACCU + 2 
1453 : 66 1c __ ROR ACCU + 1 
1455 : 66 1b __ ROR ACCU + 0 
1457 : e6 07 __ INC WORK + 4 
1459 : a5 07 __ LDA WORK + 4 
145b : c9 ff __ CMP #$ff
145d : f0 83 __ BEQ $13e2 ; (faddsub + 16)
145f : 4a __ __ LSR
1460 : 05 1e __ ORA ACCU + 3 
1462 : 85 1e __ STA ACCU + 3 
1464 : b0 06 __ BCS $146c ; (faddsub + 154)
1466 : a5 1d __ LDA ACCU + 2 
1468 : 29 7f __ AND #$7f
146a : 85 1d __ STA ACCU + 2 
146c : 60 __ __ RTS
146d : 38 __ __ SEC
146e : a5 1b __ LDA ACCU + 0 
1470 : e5 03 __ SBC WORK + 0 
1472 : 85 1b __ STA ACCU + 0 
1474 : a5 1c __ LDA ACCU + 1 
1476 : e5 04 __ SBC WORK + 1 
1478 : 85 1c __ STA ACCU + 1 
147a : a5 1d __ LDA ACCU + 2 
147c : e5 05 __ SBC WORK + 2 
147e : 85 1d __ STA ACCU + 2 
1480 : b0 19 __ BCS $149b ; (faddsub + 201)
1482 : 38 __ __ SEC
1483 : a9 00 __ LDA #$00
1485 : e5 1b __ SBC ACCU + 0 
1487 : 85 1b __ STA ACCU + 0 
1489 : a9 00 __ LDA #$00
148b : e5 1c __ SBC ACCU + 1 
148d : 85 1c __ STA ACCU + 1 
148f : a9 00 __ LDA #$00
1491 : e5 1d __ SBC ACCU + 2 
1493 : 85 1d __ STA ACCU + 2 
1495 : a5 1e __ LDA ACCU + 3 
1497 : 49 80 __ EOR #$80
1499 : 85 1e __ STA ACCU + 3 
149b : a5 1d __ LDA ACCU + 2 
149d : 30 ba __ BMI $1459 ; (faddsub + 135)
149f : 05 1c __ ORA ACCU + 1 
14a1 : 05 1b __ ORA ACCU + 0 
14a3 : f0 0f __ BEQ $14b4 ; (faddsub + 226)
14a5 : c6 07 __ DEC WORK + 4 
14a7 : f0 0b __ BEQ $14b4 ; (faddsub + 226)
14a9 : 06 1b __ ASL ACCU + 0 
14ab : 26 1c __ ROL ACCU + 1 
14ad : 26 1d __ ROL ACCU + 2 
14af : 10 f4 __ BPL $14a5 ; (faddsub + 211)
14b1 : 4c 59 14 JMP $1459 ; (faddsub + 135)
14b4 : a9 00 __ LDA #$00
14b6 : 85 1b __ STA ACCU + 0 
14b8 : 85 1c __ STA ACCU + 1 
14ba : 85 1d __ STA ACCU + 2 
14bc : 85 1e __ STA ACCU + 3 
14be : 60 __ __ RTS
--------------------------------------------------------------------
crt_fmul: ; crt_fmul
14bf : a5 1b __ LDA ACCU + 0 
14c1 : 05 1c __ ORA ACCU + 1 
14c3 : 05 1d __ ORA ACCU + 2 
14c5 : f0 0e __ BEQ $14d5 ; (crt_fmul + 22)
14c7 : a5 03 __ LDA WORK + 0 
14c9 : 05 04 __ ORA WORK + 1 
14cb : 05 05 __ ORA WORK + 2 
14cd : d0 09 __ BNE $14d8 ; (crt_fmul + 25)
14cf : 85 1b __ STA ACCU + 0 
14d1 : 85 1c __ STA ACCU + 1 
14d3 : 85 1d __ STA ACCU + 2 
14d5 : 85 1e __ STA ACCU + 3 
14d7 : 60 __ __ RTS
14d8 : a5 1e __ LDA ACCU + 3 
14da : 45 06 __ EOR WORK + 3 
14dc : 29 80 __ AND #$80
14de : 85 1e __ STA ACCU + 3 
14e0 : a9 ff __ LDA #$ff
14e2 : c5 07 __ CMP WORK + 4 
14e4 : f0 42 __ BEQ $1528 ; (crt_fmul + 105)
14e6 : c5 08 __ CMP WORK + 5 
14e8 : f0 3e __ BEQ $1528 ; (crt_fmul + 105)
14ea : a9 00 __ LDA #$00
14ec : 85 09 __ STA WORK + 6 
14ee : 85 0a __ STA WORK + 7 
14f0 : 85 0b __ STA WORK + 8 
14f2 : a4 1b __ LDY ACCU + 0 
14f4 : a5 03 __ LDA WORK + 0 
14f6 : d0 06 __ BNE $14fe ; (crt_fmul + 63)
14f8 : a5 04 __ LDA WORK + 1 
14fa : f0 0a __ BEQ $1506 ; (crt_fmul + 71)
14fc : d0 05 __ BNE $1503 ; (crt_fmul + 68)
14fe : 20 59 15 JSR $1559 ; (crt_fmul8 + 0)
1501 : a5 04 __ LDA WORK + 1 
1503 : 20 59 15 JSR $1559 ; (crt_fmul8 + 0)
1506 : a5 05 __ LDA WORK + 2 
1508 : 20 59 15 JSR $1559 ; (crt_fmul8 + 0)
150b : 38 __ __ SEC
150c : a5 0b __ LDA WORK + 8 
150e : 30 06 __ BMI $1516 ; (crt_fmul + 87)
1510 : 06 09 __ ASL WORK + 6 
1512 : 26 0a __ ROL WORK + 7 
1514 : 2a __ __ ROL
1515 : 18 __ __ CLC
1516 : 29 7f __ AND #$7f
1518 : 85 0b __ STA WORK + 8 
151a : a5 07 __ LDA WORK + 4 
151c : 65 08 __ ADC WORK + 5 
151e : 90 19 __ BCC $1539 ; (crt_fmul + 122)
1520 : e9 7f __ SBC #$7f
1522 : b0 04 __ BCS $1528 ; (crt_fmul + 105)
1524 : c9 ff __ CMP #$ff
1526 : d0 15 __ BNE $153d ; (crt_fmul + 126)
1528 : a5 1e __ LDA ACCU + 3 
152a : 09 7f __ ORA #$7f
152c : 85 1e __ STA ACCU + 3 
152e : a9 80 __ LDA #$80
1530 : 85 1d __ STA ACCU + 2 
1532 : a9 00 __ LDA #$00
1534 : 85 1b __ STA ACCU + 0 
1536 : 85 1c __ STA ACCU + 1 
1538 : 60 __ __ RTS
1539 : e9 7e __ SBC #$7e
153b : 90 15 __ BCC $1552 ; (crt_fmul + 147)
153d : 4a __ __ LSR
153e : 05 1e __ ORA ACCU + 3 
1540 : 85 1e __ STA ACCU + 3 
1542 : a9 00 __ LDA #$00
1544 : 6a __ __ ROR
1545 : 05 0b __ ORA WORK + 8 
1547 : 85 1d __ STA ACCU + 2 
1549 : a5 0a __ LDA WORK + 7 
154b : 85 1c __ STA ACCU + 1 
154d : a5 09 __ LDA WORK + 6 
154f : 85 1b __ STA ACCU + 0 
1551 : 60 __ __ RTS
1552 : a9 00 __ LDA #$00
1554 : 85 1e __ STA ACCU + 3 
1556 : f0 d8 __ BEQ $1530 ; (crt_fmul + 113)
1558 : 60 __ __ RTS
--------------------------------------------------------------------
crt_fmul8: ; crt_fmul8
1559 : 38 __ __ SEC
155a : 6a __ __ ROR
155b : 90 1e __ BCC $157b ; (crt_fmul8 + 34)
155d : aa __ __ TAX
155e : 18 __ __ CLC
155f : 98 __ __ TYA
1560 : 65 09 __ ADC WORK + 6 
1562 : 85 09 __ STA WORK + 6 
1564 : a5 0a __ LDA WORK + 7 
1566 : 65 1c __ ADC ACCU + 1 
1568 : 85 0a __ STA WORK + 7 
156a : a5 0b __ LDA WORK + 8 
156c : 65 1d __ ADC ACCU + 2 
156e : 6a __ __ ROR
156f : 85 0b __ STA WORK + 8 
1571 : 8a __ __ TXA
1572 : 66 0a __ ROR WORK + 7 
1574 : 66 09 __ ROR WORK + 6 
1576 : 4a __ __ LSR
1577 : f0 0d __ BEQ $1586 ; (crt_fmul8 + 45)
1579 : b0 e2 __ BCS $155d ; (crt_fmul8 + 4)
157b : 66 0b __ ROR WORK + 8 
157d : 66 0a __ ROR WORK + 7 
157f : 66 09 __ ROR WORK + 6 
1581 : 4a __ __ LSR
1582 : 90 f7 __ BCC $157b ; (crt_fmul8 + 34)
1584 : d0 d7 __ BNE $155d ; (crt_fmul8 + 4)
1586 : 60 __ __ RTS
--------------------------------------------------------------------
crt_fdiv: ; crt_fdiv
1587 : a5 1b __ LDA ACCU + 0 
1589 : 05 1c __ ORA ACCU + 1 
158b : 05 1d __ ORA ACCU + 2 
158d : d0 03 __ BNE $1592 ; (crt_fdiv + 11)
158f : 85 1e __ STA ACCU + 3 
1591 : 60 __ __ RTS
1592 : a5 1e __ LDA ACCU + 3 
1594 : 45 06 __ EOR WORK + 3 
1596 : 29 80 __ AND #$80
1598 : 85 1e __ STA ACCU + 3 
159a : a5 08 __ LDA WORK + 5 
159c : f0 62 __ BEQ $1600 ; (crt_fdiv + 121)
159e : a5 07 __ LDA WORK + 4 
15a0 : c9 ff __ CMP #$ff
15a2 : f0 5c __ BEQ $1600 ; (crt_fdiv + 121)
15a4 : a9 00 __ LDA #$00
15a6 : 85 09 __ STA WORK + 6 
15a8 : 85 0a __ STA WORK + 7 
15aa : 85 0b __ STA WORK + 8 
15ac : a2 18 __ LDX #$18
15ae : a5 1b __ LDA ACCU + 0 
15b0 : c5 03 __ CMP WORK + 0 
15b2 : a5 1c __ LDA ACCU + 1 
15b4 : e5 04 __ SBC WORK + 1 
15b6 : a5 1d __ LDA ACCU + 2 
15b8 : e5 05 __ SBC WORK + 2 
15ba : 90 13 __ BCC $15cf ; (crt_fdiv + 72)
15bc : a5 1b __ LDA ACCU + 0 
15be : e5 03 __ SBC WORK + 0 
15c0 : 85 1b __ STA ACCU + 0 
15c2 : a5 1c __ LDA ACCU + 1 
15c4 : e5 04 __ SBC WORK + 1 
15c6 : 85 1c __ STA ACCU + 1 
15c8 : a5 1d __ LDA ACCU + 2 
15ca : e5 05 __ SBC WORK + 2 
15cc : 85 1d __ STA ACCU + 2 
15ce : 38 __ __ SEC
15cf : 26 09 __ ROL WORK + 6 
15d1 : 26 0a __ ROL WORK + 7 
15d3 : 26 0b __ ROL WORK + 8 
15d5 : ca __ __ DEX
15d6 : f0 0a __ BEQ $15e2 ; (crt_fdiv + 91)
15d8 : 06 1b __ ASL ACCU + 0 
15da : 26 1c __ ROL ACCU + 1 
15dc : 26 1d __ ROL ACCU + 2 
15de : b0 dc __ BCS $15bc ; (crt_fdiv + 53)
15e0 : 90 cc __ BCC $15ae ; (crt_fdiv + 39)
15e2 : 38 __ __ SEC
15e3 : a5 0b __ LDA WORK + 8 
15e5 : 30 06 __ BMI $15ed ; (crt_fdiv + 102)
15e7 : 06 09 __ ASL WORK + 6 
15e9 : 26 0a __ ROL WORK + 7 
15eb : 2a __ __ ROL
15ec : 18 __ __ CLC
15ed : 29 7f __ AND #$7f
15ef : 85 0b __ STA WORK + 8 
15f1 : a5 07 __ LDA WORK + 4 
15f3 : e5 08 __ SBC WORK + 5 
15f5 : 90 1a __ BCC $1611 ; (crt_fdiv + 138)
15f7 : 18 __ __ CLC
15f8 : 69 7f __ ADC #$7f
15fa : b0 04 __ BCS $1600 ; (crt_fdiv + 121)
15fc : c9 ff __ CMP #$ff
15fe : d0 15 __ BNE $1615 ; (crt_fdiv + 142)
1600 : a5 1e __ LDA ACCU + 3 
1602 : 09 7f __ ORA #$7f
1604 : 85 1e __ STA ACCU + 3 
1606 : a9 80 __ LDA #$80
1608 : 85 1d __ STA ACCU + 2 
160a : a9 00 __ LDA #$00
160c : 85 1c __ STA ACCU + 1 
160e : 85 1b __ STA ACCU + 0 
1610 : 60 __ __ RTS
1611 : 69 7f __ ADC #$7f
1613 : 90 15 __ BCC $162a ; (crt_fdiv + 163)
1615 : 4a __ __ LSR
1616 : 05 1e __ ORA ACCU + 3 
1618 : 85 1e __ STA ACCU + 3 
161a : a9 00 __ LDA #$00
161c : 6a __ __ ROR
161d : 05 0b __ ORA WORK + 8 
161f : 85 1d __ STA ACCU + 2 
1621 : a5 0a __ LDA WORK + 7 
1623 : 85 1c __ STA ACCU + 1 
1625 : a5 09 __ LDA WORK + 6 
1627 : 85 1b __ STA ACCU + 0 
1629 : 60 __ __ RTS
162a : a9 00 __ LDA #$00
162c : 85 1e __ STA ACCU + 3 
162e : 85 1d __ STA ACCU + 2 
1630 : 85 1c __ STA ACCU + 1 
1632 : 85 1b __ STA ACCU + 0 
1634 : 60 __ __ RTS
--------------------------------------------------------------------
divs16: ; divs16
1635 : 24 1c __ BIT ACCU + 1 
1637 : 10 0d __ BPL $1646 ; (divs16 + 17)
1639 : 20 50 16 JSR $1650 ; (negaccu + 0)
163c : 24 04 __ BIT WORK + 1 
163e : 10 0d __ BPL $164d ; (divs16 + 24)
1640 : 20 5e 16 JSR $165e ; (negtmp + 0)
1643 : 4c 6c 16 JMP $166c ; (divmod + 0)
1646 : 24 04 __ BIT WORK + 1 
1648 : 10 f9 __ BPL $1643 ; (divs16 + 14)
164a : 20 5e 16 JSR $165e ; (negtmp + 0)
164d : 20 6c 16 JSR $166c ; (divmod + 0)
--------------------------------------------------------------------
negaccu: ; negaccu
1650 : 38 __ __ SEC
1651 : a9 00 __ LDA #$00
1653 : e5 1b __ SBC ACCU + 0 
1655 : 85 1b __ STA ACCU + 0 
1657 : a9 00 __ LDA #$00
1659 : e5 1c __ SBC ACCU + 1 
165b : 85 1c __ STA ACCU + 1 
165d : 60 __ __ RTS
--------------------------------------------------------------------
negtmp: ; negtmp
165e : 38 __ __ SEC
165f : a9 00 __ LDA #$00
1661 : e5 03 __ SBC WORK + 0 
1663 : 85 03 __ STA WORK + 0 
1665 : a9 00 __ LDA #$00
1667 : e5 04 __ SBC WORK + 1 
1669 : 85 04 __ STA WORK + 1 
166b : 60 __ __ RTS
--------------------------------------------------------------------
divmod: ; divmod
166c : a5 1c __ LDA ACCU + 1 
166e : d0 3b __ BNE $16ab ; (divmod + 63)
1670 : a5 04 __ LDA WORK + 1 
1672 : d0 1e __ BNE $1692 ; (divmod + 38)
1674 : 85 06 __ STA WORK + 3 
1676 : a2 04 __ LDX #$04
1678 : 06 1b __ ASL ACCU + 0 
167a : 2a __ __ ROL
167b : c5 03 __ CMP WORK + 0 
167d : 90 02 __ BCC $1681 ; (divmod + 21)
167f : e5 03 __ SBC WORK + 0 
1681 : 26 1b __ ROL ACCU + 0 
1683 : 2a __ __ ROL
1684 : c5 03 __ CMP WORK + 0 
1686 : 90 02 __ BCC $168a ; (divmod + 30)
1688 : e5 03 __ SBC WORK + 0 
168a : 26 1b __ ROL ACCU + 0 
168c : ca __ __ DEX
168d : d0 eb __ BNE $167a ; (divmod + 14)
168f : 85 05 __ STA WORK + 2 
1691 : 60 __ __ RTS
1692 : a5 1b __ LDA ACCU + 0 
1694 : 85 05 __ STA WORK + 2 
1696 : a5 1c __ LDA ACCU + 1 
1698 : 85 06 __ STA WORK + 3 
169a : a9 00 __ LDA #$00
169c : 85 1b __ STA ACCU + 0 
169e : 85 1c __ STA ACCU + 1 
16a0 : 60 __ __ RTS
16a1 : 85 03 __ STA WORK + 0 
16a3 : a9 00 __ LDA #$00
16a5 : 85 04 __ STA WORK + 1 
16a7 : a5 1c __ LDA ACCU + 1 
16a9 : f0 c9 __ BEQ $1674 ; (divmod + 8)
16ab : a5 04 __ LDA WORK + 1 
16ad : d0 1f __ BNE $16ce ; (divmod + 98)
16af : a5 03 __ LDA WORK + 0 
16b1 : 30 1b __ BMI $16ce ; (divmod + 98)
16b3 : a9 00 __ LDA #$00
16b5 : 85 06 __ STA WORK + 3 
16b7 : a2 10 __ LDX #$10
16b9 : 06 1b __ ASL ACCU + 0 
16bb : 26 1c __ ROL ACCU + 1 
16bd : 2a __ __ ROL
16be : c5 03 __ CMP WORK + 0 
16c0 : 90 02 __ BCC $16c4 ; (divmod + 88)
16c2 : e5 03 __ SBC WORK + 0 
16c4 : 26 1b __ ROL ACCU + 0 
16c6 : 26 1c __ ROL ACCU + 1 
16c8 : ca __ __ DEX
16c9 : d0 f2 __ BNE $16bd ; (divmod + 81)
16cb : 85 05 __ STA WORK + 2 
16cd : 60 __ __ RTS
16ce : a9 00 __ LDA #$00
16d0 : 85 05 __ STA WORK + 2 
16d2 : 85 06 __ STA WORK + 3 
16d4 : a0 10 __ LDY #$10
16d6 : 18 __ __ CLC
16d7 : 26 1b __ ROL ACCU + 0 
16d9 : 26 1c __ ROL ACCU + 1 
16db : 26 05 __ ROL WORK + 2 
16dd : 26 06 __ ROL WORK + 3 
16df : 38 __ __ SEC
16e0 : a5 05 __ LDA WORK + 2 
16e2 : e5 03 __ SBC WORK + 0 
16e4 : aa __ __ TAX
16e5 : a5 06 __ LDA WORK + 3 
16e7 : e5 04 __ SBC WORK + 1 
16e9 : 90 04 __ BCC $16ef ; (divmod + 131)
16eb : 86 05 __ STX WORK + 2 
16ed : 85 06 __ STA WORK + 3 
16ef : 88 __ __ DEY
16f0 : d0 e5 __ BNE $16d7 ; (divmod + 107)
16f2 : 26 1b __ ROL ACCU + 0 
16f4 : 26 1c __ ROL ACCU + 1 
16f6 : 60 __ __ RTS
--------------------------------------------------------------------
mods16: ; mods16
16f7 : 24 1c __ BIT ACCU + 1 
16f9 : 10 10 __ BPL $170b ; (mods16 + 20)
16fb : 20 50 16 JSR $1650 ; (negaccu + 0)
16fe : 24 04 __ BIT WORK + 1 
1700 : 10 03 __ BPL $1705 ; (mods16 + 14)
1702 : 20 5e 16 JSR $165e ; (negtmp + 0)
1705 : 20 6c 16 JSR $166c ; (divmod + 0)
1708 : 4c 40 17 JMP $1740 ; (negtmpb + 0)
170b : 24 04 __ BIT WORK + 1 
170d : 10 03 __ BPL $1712 ; (mods16 + 27)
170f : 20 5e 16 JSR $165e ; (negtmp + 0)
1712 : 4c 6c 16 JMP $166c ; (divmod + 0)
1715 : 60 __ __ RTS
--------------------------------------------------------------------
negtmpb: ; negtmpb
1740 : 38 __ __ SEC
1741 : a9 00 __ LDA #$00
1743 : e5 05 __ SBC WORK + 2 
1745 : 85 05 __ STA WORK + 2 
1747 : a9 00 __ LDA #$00
1749 : e5 06 __ SBC WORK + 3 
174b : 85 06 __ STA WORK + 3 
174d : 60 __ __ RTS
--------------------------------------------------------------------
f32_to_i16: ; f32_to_i16
174e : 20 b1 13 JSR $13b1 ; (freg + 36)
1751 : a5 07 __ LDA WORK + 4 
1753 : c9 7f __ CMP #$7f
1755 : b0 07 __ BCS $175e ; (f32_to_i16 + 16)
1757 : a9 00 __ LDA #$00
1759 : 85 1b __ STA ACCU + 0 
175b : 85 1c __ STA ACCU + 1 
175d : 60 __ __ RTS
175e : e9 8e __ SBC #$8e
1760 : 90 16 __ BCC $1778 ; (f32_to_i16 + 42)
1762 : 24 1e __ BIT ACCU + 3 
1764 : 30 09 __ BMI $176f ; (f32_to_i16 + 33)
1766 : a9 ff __ LDA #$ff
1768 : 85 1b __ STA ACCU + 0 
176a : a9 7f __ LDA #$7f
176c : 85 1c __ STA ACCU + 1 
176e : 60 __ __ RTS
176f : a9 00 __ LDA #$00
1771 : 85 1b __ STA ACCU + 0 
1773 : a9 80 __ LDA #$80
1775 : 85 1c __ STA ACCU + 1 
1777 : 60 __ __ RTS
1778 : aa __ __ TAX
1779 : a5 1c __ LDA ACCU + 1 
177b : 46 1d __ LSR ACCU + 2 
177d : 6a __ __ ROR
177e : e8 __ __ INX
177f : d0 fa __ BNE $177b ; (f32_to_i16 + 45)
1781 : 24 1e __ BIT ACCU + 3 
1783 : 10 0e __ BPL $1793 ; (f32_to_i16 + 69)
1785 : 38 __ __ SEC
1786 : 49 ff __ EOR #$ff
1788 : 69 00 __ ADC #$00
178a : 85 1b __ STA ACCU + 0 
178c : a9 00 __ LDA #$00
178e : e5 1d __ SBC ACCU + 2 
1790 : 85 1c __ STA ACCU + 1 
1792 : 60 __ __ RTS
1793 : 85 1b __ STA ACCU + 0 
1795 : a5 1d __ LDA ACCU + 2 
1797 : 85 1c __ STA ACCU + 1 
1799 : 60 __ __ RTS
--------------------------------------------------------------------
sint16_to_float: ; sint16_to_float
179a : 24 1c __ BIT ACCU + 1 
179c : 30 03 __ BMI $17a1 ; (sint16_to_float + 7)
179e : 4c b8 17 JMP $17b8 ; (uint16_to_float + 0)
17a1 : 38 __ __ SEC
17a2 : a9 00 __ LDA #$00
17a4 : e5 1b __ SBC ACCU + 0 
17a6 : 85 1b __ STA ACCU + 0 
17a8 : a9 00 __ LDA #$00
17aa : e5 1c __ SBC ACCU + 1 
17ac : 85 1c __ STA ACCU + 1 
17ae : 20 b8 17 JSR $17b8 ; (uint16_to_float + 0)
17b1 : a5 1e __ LDA ACCU + 3 
17b3 : 09 80 __ ORA #$80
17b5 : 85 1e __ STA ACCU + 3 
17b7 : 60 __ __ RTS
--------------------------------------------------------------------
uint16_to_float: ; uint16_to_float
17b8 : a5 1b __ LDA ACCU + 0 
17ba : 05 1c __ ORA ACCU + 1 
17bc : d0 05 __ BNE $17c3 ; (uint16_to_float + 11)
17be : 85 1d __ STA ACCU + 2 
17c0 : 85 1e __ STA ACCU + 3 
17c2 : 60 __ __ RTS
17c3 : a2 8e __ LDX #$8e
17c5 : a5 1c __ LDA ACCU + 1 
17c7 : 30 06 __ BMI $17cf ; (uint16_to_float + 23)
17c9 : ca __ __ DEX
17ca : 06 1b __ ASL ACCU + 0 
17cc : 2a __ __ ROL
17cd : 10 fa __ BPL $17c9 ; (uint16_to_float + 17)
17cf : 0a __ __ ASL
17d0 : 85 1d __ STA ACCU + 2 
17d2 : a5 1b __ LDA ACCU + 0 
17d4 : 85 1c __ STA ACCU + 1 
17d6 : 8a __ __ TXA
17d7 : 4a __ __ LSR
17d8 : 85 1e __ STA ACCU + 3 
17da : a9 00 __ LDA #$00
17dc : 85 1b __ STA ACCU + 0 
17de : 66 1d __ ROR ACCU + 2 
17e0 : 60 __ __ RTS
--------------------------------------------------------------------
mul32: ; mul32
17e1 : a5 04 __ LDA WORK + 1 
17e3 : 05 05 __ ORA WORK + 2 
17e5 : 05 06 __ ORA WORK + 3 
17e7 : d0 05 __ BNE $17ee ; (mul32 + 13)
17e9 : a5 03 __ LDA WORK + 0 
17eb : 4c 57 13 JMP $1357 ; (mul32by8 + 0)
17ee : a0 00 __ LDY #$00
17f0 : 84 07 __ STY WORK + 4 
17f2 : 84 08 __ STY WORK + 5 
17f4 : 98 __ __ TYA
17f5 : 38 __ __ SEC
17f6 : 66 03 __ ROR WORK + 0 
17f8 : 90 15 __ BCC $180f ; (mul32 + 46)
17fa : aa __ __ TAX
17fb : 18 __ __ CLC
17fc : a5 07 __ LDA WORK + 4 
17fe : 65 1b __ ADC ACCU + 0 
1800 : 85 07 __ STA WORK + 4 
1802 : a5 08 __ LDA WORK + 5 
1804 : 65 1c __ ADC ACCU + 1 
1806 : 85 08 __ STA WORK + 5 
1808 : 98 __ __ TYA
1809 : 65 1d __ ADC ACCU + 2 
180b : a8 __ __ TAY
180c : 8a __ __ TXA
180d : 65 1e __ ADC ACCU + 3 
180f : 46 04 __ LSR WORK + 1 
1811 : 90 0f __ BCC $1822 ; (mul32 + 65)
1813 : aa __ __ TAX
1814 : 18 __ __ CLC
1815 : a5 08 __ LDA WORK + 5 
1817 : 65 1b __ ADC ACCU + 0 
1819 : 85 08 __ STA WORK + 5 
181b : 98 __ __ TYA
181c : 65 1c __ ADC ACCU + 1 
181e : a8 __ __ TAY
181f : 8a __ __ TXA
1820 : 65 1d __ ADC ACCU + 2 
1822 : 46 05 __ LSR WORK + 2 
1824 : 90 09 __ BCC $182f ; (mul32 + 78)
1826 : aa __ __ TAX
1827 : 18 __ __ CLC
1828 : 98 __ __ TYA
1829 : 65 1b __ ADC ACCU + 0 
182b : a8 __ __ TAY
182c : 8a __ __ TXA
182d : 65 1c __ ADC ACCU + 1 
182f : 46 06 __ LSR WORK + 3 
1831 : 90 03 __ BCC $1836 ; (mul32 + 85)
1833 : 18 __ __ CLC
1834 : 65 1b __ ADC ACCU + 0 
1836 : 06 1b __ ASL ACCU + 0 
1838 : 26 1c __ ROL ACCU + 1 
183a : 26 1d __ ROL ACCU + 2 
183c : 26 1e __ ROL ACCU + 3 
183e : 46 03 __ LSR WORK + 0 
1840 : 90 cd __ BCC $180f ; (mul32 + 46)
1842 : d0 b6 __ BNE $17fa ; (mul32 + 25)
1844 : 84 09 __ STY WORK + 6 
1846 : 85 0a __ STA WORK + 7 
1848 : 60 __ __ RTS
--------------------------------------------------------------------
divmod32: ; divmod32
1867 : a9 00 __ LDA #$00
1869 : 85 07 __ STA WORK + 4 
186b : 85 08 __ STA WORK + 5 
186d : 85 09 __ STA WORK + 6 
186f : 85 0a __ STA WORK + 7 
1871 : a5 05 __ LDA WORK + 2 
1873 : 05 06 __ ORA WORK + 3 
1875 : f0 4b __ BEQ $18c2 ; (divmod32 + 91)
1877 : a0 10 __ LDY #$10
1879 : a5 1e __ LDA ACCU + 3 
187b : 85 08 __ STA WORK + 5 
187d : a5 1d __ LDA ACCU + 2 
187f : 85 07 __ STA WORK + 4 
1881 : a9 00 __ LDA #$00
1883 : 85 1d __ STA ACCU + 2 
1885 : 85 1e __ STA ACCU + 3 
1887 : 18 __ __ CLC
1888 : 26 1b __ ROL ACCU + 0 
188a : 26 1c __ ROL ACCU + 1 
188c : 26 07 __ ROL WORK + 4 
188e : 26 08 __ ROL WORK + 5 
1890 : 26 09 __ ROL WORK + 6 
1892 : 26 0a __ ROL WORK + 7 
1894 : a5 07 __ LDA WORK + 4 
1896 : c5 03 __ CMP WORK + 0 
1898 : a5 08 __ LDA WORK + 5 
189a : e5 04 __ SBC WORK + 1 
189c : a5 09 __ LDA WORK + 6 
189e : e5 05 __ SBC WORK + 2 
18a0 : aa __ __ TAX
18a1 : a5 0a __ LDA WORK + 7 
18a3 : e5 06 __ SBC WORK + 3 
18a5 : 90 11 __ BCC $18b8 ; (divmod32 + 81)
18a7 : 86 09 __ STX WORK + 6 
18a9 : 85 0a __ STA WORK + 7 
18ab : a5 07 __ LDA WORK + 4 
18ad : e5 03 __ SBC WORK + 0 
18af : 85 07 __ STA WORK + 4 
18b1 : a5 08 __ LDA WORK + 5 
18b3 : e5 04 __ SBC WORK + 1 
18b5 : 85 08 __ STA WORK + 5 
18b7 : 38 __ __ SEC
18b8 : 88 __ __ DEY
18b9 : d0 cd __ BNE $1888 ; (divmod32 + 33)
18bb : 26 1b __ ROL ACCU + 0 
18bd : 26 1c __ ROL ACCU + 1 
18bf : a4 02 __ LDY $02 
18c1 : 60 __ __ RTS
18c2 : a5 1d __ LDA ACCU + 2 
18c4 : 05 1e __ ORA ACCU + 3 
18c6 : d0 0c __ BNE $18d4 ; (divmod32 + 109)
18c8 : 20 6c 16 JSR $166c ; (divmod + 0)
18cb : a5 05 __ LDA WORK + 2 
18cd : 85 07 __ STA WORK + 4 
18cf : a5 06 __ LDA WORK + 3 
18d1 : 85 08 __ STA WORK + 5 
18d3 : 60 __ __ RTS
18d4 : a0 20 __ LDY #$20
18d6 : a5 04 __ LDA WORK + 1 
18d8 : d0 27 __ BNE $1901 ; (divmod32 + 154)
18da : 18 __ __ CLC
18db : 26 1b __ ROL ACCU + 0 
18dd : 26 1c __ ROL ACCU + 1 
18df : 26 1d __ ROL ACCU + 2 
18e1 : 26 1e __ ROL ACCU + 3 
18e3 : 2a __ __ ROL
18e4 : 90 05 __ BCC $18eb ; (divmod32 + 132)
18e6 : e5 03 __ SBC WORK + 0 
18e8 : 38 __ __ SEC
18e9 : b0 06 __ BCS $18f1 ; (divmod32 + 138)
18eb : c5 03 __ CMP WORK + 0 
18ed : 90 02 __ BCC $18f1 ; (divmod32 + 138)
18ef : e5 03 __ SBC WORK + 0 
18f1 : 88 __ __ DEY
18f2 : d0 e7 __ BNE $18db ; (divmod32 + 116)
18f4 : 85 07 __ STA WORK + 4 
18f6 : 26 1b __ ROL ACCU + 0 
18f8 : 26 1c __ ROL ACCU + 1 
18fa : 26 1d __ ROL ACCU + 2 
18fc : 26 1e __ ROL ACCU + 3 
18fe : a4 02 __ LDY $02 
1900 : 60 __ __ RTS
1901 : a5 1e __ LDA ACCU + 3 
1903 : d0 10 __ BNE $1915 ; (divmod32 + 174)
1905 : a6 1d __ LDX ACCU + 2 
1907 : 86 1e __ STX ACCU + 3 
1909 : a6 1c __ LDX ACCU + 1 
190b : 86 1d __ STX ACCU + 2 
190d : a6 1b __ LDX ACCU + 0 
190f : 86 1c __ STX ACCU + 1 
1911 : 85 1b __ STA ACCU + 0 
1913 : a0 18 __ LDY #$18
1915 : 18 __ __ CLC
1916 : 26 1b __ ROL ACCU + 0 
1918 : 26 1c __ ROL ACCU + 1 
191a : 26 1d __ ROL ACCU + 2 
191c : 26 1e __ ROL ACCU + 3 
191e : 26 07 __ ROL WORK + 4 
1920 : 26 08 __ ROL WORK + 5 
1922 : 90 0c __ BCC $1930 ; (divmod32 + 201)
1924 : a5 07 __ LDA WORK + 4 
1926 : e5 03 __ SBC WORK + 0 
1928 : aa __ __ TAX
1929 : a5 08 __ LDA WORK + 5 
192b : e5 04 __ SBC WORK + 1 
192d : 38 __ __ SEC
192e : b0 0c __ BCS $193c ; (divmod32 + 213)
1930 : 38 __ __ SEC
1931 : a5 07 __ LDA WORK + 4 
1933 : e5 03 __ SBC WORK + 0 
1935 : aa __ __ TAX
1936 : a5 08 __ LDA WORK + 5 
1938 : e5 04 __ SBC WORK + 1 
193a : 90 04 __ BCC $1940 ; (divmod32 + 217)
193c : 86 07 __ STX WORK + 4 
193e : 85 08 __ STA WORK + 5 
1940 : 88 __ __ DEY
1941 : d0 d3 __ BNE $1916 ; (divmod32 + 175)
1943 : 26 1b __ ROL ACCU + 0 
1945 : 26 1c __ ROL ACCU + 1 
1947 : 26 1d __ ROL ACCU + 2 
1949 : 26 1e __ ROL ACCU + 3 
194b : a4 02 __ LDY $02 
194d : 60 __ __ RTS
--------------------------------------------------------------------
freg@proxy: ; freg@proxy
194e : a9 00 __ LDA #$00
1950 : 85 03 __ STA WORK + 0 
1952 : a9 00 __ LDA #$00
1954 : 85 04 __ STA WORK + 1 
1956 : a9 20 __ LDA #$20
1958 : 85 05 __ STA WORK + 2 
195a : a9 41 __ LDA #$41
195c : 85 06 __ STA WORK + 3 
195e : 4c a1 13 JMP $13a1 ; (freg + 20)
--------------------------------------------------------------------
freg@proxy: ; freg@proxy
1961 : a9 00 __ LDA #$00
1963 : 85 03 __ STA WORK + 0 
1965 : a9 00 __ LDA #$00
1967 : 85 04 __ STA WORK + 1 
1969 : a5 43 __ LDA $43 
196b : 85 1b __ STA ACCU + 0 
196d : a5 44 __ LDA $44 
196f : 85 1c __ STA ACCU + 1 
1971 : a5 45 __ LDA $45 
1973 : 85 1d __ STA ACCU + 2 
1975 : a5 46 __ LDA $46 
1977 : 85 1e __ STA ACCU + 3 
1979 : 4c a1 13 JMP $13a1 ; (freg + 20)
--------------------------------------------------------------------
freg@proxy: ; freg@proxy
197c : a5 43 __ LDA $43 
197e : 85 1b __ STA ACCU + 0 
1980 : a5 44 __ LDA $44 
1982 : 85 1c __ STA ACCU + 1 
1984 : a5 45 __ LDA $45 
1986 : 85 1d __ STA ACCU + 2 
1988 : a5 46 __ LDA $46 
198a : 85 1e __ STA ACCU + 3 
198c : 4c 91 13 JMP $1391 ; (freg + 4)
--------------------------------------------------------------------
spentry:
198f : __ __ __ BYT 00                                              : .
--------------------------------------------------------------------
fround5:
1990 : __ __ __ BYT 00 00 00 3f cd cc 4c 3d 0a d7 a3 3b 6f 12 03 3a : ...?..L=...;o..:
19a0 : __ __ __ BYT 17 b7 51 38 ac c5 a7 36 bd 37 06 35             : ..Q8...6.7.5
--------------------------------------------------------------------
seed:
19ac : __ __ __ BYT 6f 00 00 00                                     : o...
