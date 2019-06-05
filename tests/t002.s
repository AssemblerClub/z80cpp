;;
;; TEST: INDIRECT LOADING THROUGH HL
;;    It tests how to load some data and modify memory
;; using HL as indirect pointer.
;;
.area _DATA
.area _CODE
LD H, #0x00 ;; 0000 26 00
LD L, #0x10 ;; 0002 2E 10
LD B, #0x3E ;; 0004 06 3E
LD C, #0x11 ;; 0006 0E 11
LD D, #0x06 ;; 0008 16 06
LD (HL), B  ;; 000A 70
LD L, C     ;; 000B 69
LD (HL), C  ;; 000C 71
LD L, #0x12 ;; 000D 2E 12
LD (HL), D  ;; 000F 72
.ds 4       ;; 0010 00 00 00 00 
HALT        ;; 0014 76

;; OUTPUT
;; A=11  BC=0011  DE=0600  HL=0012
;; (0x0010) = 3E 11 06 00 76