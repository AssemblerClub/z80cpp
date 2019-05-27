.area _DATA
.area _CODE
LD H, #0x00
LD L, #0x10
LD B, #0x3E
LD C, #0x11
LD D, #0x06
LD (HL), B
LD L, C
LD (HL), C
LD L, #0x12
LD (HL), D