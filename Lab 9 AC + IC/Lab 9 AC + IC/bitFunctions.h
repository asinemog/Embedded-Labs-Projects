
#define bitSet(reg, ind) (reg |= 1 << ind)

#define bitClear(reg, ind) (reg &= ~(1 << ind) )


//e.g. PIND = 0b00010100, 
// reg = PIND = 0b00010100
// ind = PIND4 = 4
// PIND >> PIND4 & 1 = 0b00010100 >> 4 & 1 = 0b00000001 & 0b00000001 = 1/true
// 
#define bitCheck(reg, ind) (reg >> ind & 1)


#define bitToggle(reg, ind) (reg ^= 1 << ind)

