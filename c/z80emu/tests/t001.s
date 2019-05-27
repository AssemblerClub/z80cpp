.area _DATA
.area _CODE
LD A, #0x11
LD B, A
LD A, #0x22
LD C, A
LD H, #0x33
LD L, #0x44
LD A, #0x00
LD H, A
LD L, A
LD (HL), #0x55
