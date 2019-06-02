;;
;; PERFORMANCE TEST
;;    Some instructions repeated 200 times to fill-up memory
;; in order to let the user execute up to 16000 t-states and
;; test how fast the emulator goes
;;
.area _DATA
.area _CODE
.rept 200
      LD H, #0x0A
      LD L, #0x10
      LD B, #0x3E
      LD C, #0x11
      LD D, #0x06
      LD (HL), B
      LD L, C
      LD (HL), C
      LD L, #0x12
      LD (HL), D
.endm
HALT