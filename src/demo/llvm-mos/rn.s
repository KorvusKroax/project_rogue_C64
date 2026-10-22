; uint16_t RN_asm(void)
; seed = seed * 11109 + 13849 (mod 2^32), vissza: seed >> 16
; llvm-mos hívási konvenció: 16 bites visszatérési érték A = alsó, X = felső bájt

        .section .bss.rn_work,"aw",@nobits
res:    .zero 4                 ; munkaterület: részeredmény
mul:    .zero 2                 ; munkaterület: szorzó

        .section .text.RN_asm,"ax",@progbits
        .global RN_asm
RN_asm:
        lda #0                  ; res = 0
        sta res
        sta res+1
        sta res+2
        sta res+3
        lda #$65                ; mul = 11109 = $2B65
        sta mul
        lda #$2B
        sta mul+1

        ldy #14                 ; $2B65 legfelső 1-es bitje a 13., 14 kör elég
loop:
        lsr mul+1               ; a szorzó legalsó bitje a carry-be
        ror mul
        bcc skip
        clc                     ; res += seed
        lda res
        adc seed
        sta res
        lda res+1
        adc seed+1
        sta res+1
        lda res+2
        adc seed+2
        sta res+2
        lda res+3
        adc seed+3
        sta res+3
skip:
        asl seed                ; szorzandó << 1 (a seed-et a végén úgyis felülírjuk)
        rol seed+1
        rol seed+2
        rol seed+3
        dey
        bne loop

        clc                     ; seed = res + 13849 ($3619)
        lda res
        adc #$19
        sta seed
        lda res+1
        adc #$36
        sta seed+1
        lda res+2
        adc #0
        sta seed+2
        tay                     ; az eredmény alsó bájtja félretéve
        lda res+3
        adc #0
        sta seed+3
        tax                     ; X = felső bájt
        tya                     ; A = alsó bájt
        rts
