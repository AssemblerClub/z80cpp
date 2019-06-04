;;
;; TEST: JP (HL)
;;    
;;
.area _DATA
.area _CODE
LD  A, #0x11      ;; 0000 3E 11
LD HL, #fw_1      ;; 0002 21 12 00
JP (HL)           ;; 0005 E9
LD  A, #0x22      ;; 0006 3E 22
back_1:
LD DE, #0xddee    ;; 0008 11 EE DD
LD HL, #fw_2      ;; 000B 21 20 00
JP (HL)           ;; 000E E9 
LD DE, #0x8765    ;; 000F 11 65 87
fw_1:
LD BC, #0xbbcc    ;; 0012 01 CC BB
LD HL, #back_1    ;; 0015 21 08 00
JP (HL)           ;; 0018 E9
LD BC, #0x4567    ;; 0019 01 67 45
LD HL, #fw_2+3    ;; 001C 21 23 00
JP (HL)           ;; 001F E9
fw_2:
LD HL, #0xf1f2    ;; 0020 21 F2 F1
JP .              ;; 0023 C3 23 00

;; OUTPUT
;; A=0x11, BC=0xBBCC, DE=0xDDEE, HL=0xf1f2