#include <stdio.h>
#include <stdint.h>
#include <string.h>

struct Chip_8{
    
    uint8_t registers[16];
    uint16_t I;
    uint16_t PC;
    uint8_t StackPointer;
    uint8_t DelayTimer;
    uint8_t SoundTimer;
    uint16_t stack[16];
    uint8_t memory[4096];
    uint8_t display[64][32];

};





int main() {






}


void fetch_and_decode(uint16_t opcode, struct Chip_8 *chip8){

    switch ((opcode & 0xF00) >> 12)
    {
    case 0x0:
        if((opcode & 0x000F) == 0x0){
            memset(chip8->display, 0, sizeof(chip8->display));
        }else{
            chip8->StackPointer--;
            chip8->PC = chip8->stack[chip8->StackPointer];
        }
        break;
    
    default:
        break;
    }




}



