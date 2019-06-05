;;
;; BASIC LD-16 bits test
;;    It test loading main 16 bits registers with data
;;
.area _DATA
.area _CODE
LD BC, #0xBBCC  ;; 0000 01 CC BB
LD DE, #0xDDEE  ;; 0003 11 EE DD 
LD HL, #0x1122  ;; 0006 21 22 11 
LD SP, #0x3344  ;; 0009 31 44 33
HALT            ;; 000C 76

;; OUTPUT
;; BC = 0xBBCC, DE = 0xDDEE, HL = 0x1122, SP = 0x3344