#include <stdio.h>
#include <stdint.h>

struct Chip_8{
    
    uint8_t registers[16];
    uint16_t I;
    uint16_t PC;
    uint8_t SP;
    uint8_t DelayTimer;
    uint8_t SoundTimer;
    uint16_t stack[16];
    uint8_t memory[4096];
    uint8_t display[64][32];

};





int main() {






}