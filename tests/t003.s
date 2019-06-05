;;
;; PERFORMANCE TEST
;;    Some instructions repeated 200 times to fill-up memory
;; in order to let the user execute up to 16000 t-states and
;; test how fast the emulator goes
;;
.area _DATA
.area _CODE
.rept 200
      LD H, #0x0A ;; 0000 26 0A
      LD L, #0x10 ;; 0002 2E 10
      LD B, #0x3E ;; 0004 06 3E
      LD C, #0x11 ;; 0006 0E 11
      LD D, #0x06 ;; 0008 16 06
      LD (HL), B  ;; 000A 70
      LD L, C     ;; 000B 69
      LD (HL), C  ;; 000C 71
      LD L, #0x12 ;; 000D 2E 12
      LD (HL), D  ;; 000F 72
.endm
HALT  ;; 0C80 76

;; OUTPUT
;; A=0x11, HL=0x0A12, BC=0x3E11, DE=0x0600 