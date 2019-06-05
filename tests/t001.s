;;
;; TEST: BASIC 8-bit REGISTER LOADING
;;    It loads 8-bit registers with data
;;
.area _DATA
.area _CODE
LD A, #0x11  ;; 0000 3E 11
LD B, A      ;; 0002 47
LD A, #0x22  ;; 0003 3E 22
LD C, A      ;; 0005 4F
LD H, #0x33  ;; 0006 26 33
LD L, #0x44  ;; 0008 2E 44
LD A, #0x00  ;; 000A 3E 00
LD H, A      ;; 000C 67
LD L, A      ;; 000D 6F
LD D, #0x55  ;; 000E 16 55
LD E, #0x66  ;; 0010 1E 66
LD A, D      ;; 0012 7A
HALT         ;; 0013 76

;; OUTPUT
;; A=0x55, BC=0x1122, DE=0x5566, HL=0x0000
