#include <stdio.h>
#include <stdint.h>
#include <string.h>

struct Chip_8{
    
    uint8_t V[16];
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
    case 0x1:
        chip8->PC = (opcode & 0x0FFF);
        break;
    case 0x2:
        chip8->stack[chip8->StackPointer] = chip8->PC;
        chip8->StackPointer++;

        chip8->PC = opcode & 0x0FFF;
        break;
    
    case 0x3:
        if((opcode & 0x00FF) == chip8->V[(opcode & 0x0F00) >> 8]){
            chip8->PC+=2;
        }
        break;
    
    case 0x4:
        if((opcode & 0x00FF) != chip8->V[(opcode & 0x0F00) >> 8]){
            chip8->PC+=2;
        }
        break;

    case 0x5:
        if((chip8->V[(opcode & 0x0F00) >> 8] ) == (chip8->V[(opcode & 0x00F0) >> 4] )){
            chip8->PC+=2;
        }
        break;
    case 0x6:
        chip8->V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;

        break;

    case 0x7:
        chip8->V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;

        break;
    case 0x8:
        
        uint8_t last_bit = opcode & 0x000F;

        switch(last_bit){
            case 0x0:
                chip8->V[(opcode & 0x0F00) >> 8] = chip8->V[(opcode & 0x00F0) >> 4];

                break;

            case 0x1:
                chip8->V[(opcode & 0x0F00)>>8] = chip8->V[(opcode & 0x0F00) >> 8] | chip8->V[(opcode & 0x00F0)>>4];
                break;

            case 0x2:
                chip8->V[(opcode & 0x0F00)>>8] = chip8->V[(opcode & 0x0F00)>>8] & chip8->V[(opcode & 0x00F0)>>4];
                break;
            case 0x3:
                chip8->V[(opcode & 0x0F00)>>8] = chip8->V[(opcode & 0x0F00)>>8] ^ chip8->V[(opcode & 0x00F0)>>4];
                break;
            case 0x4:
                uint16_t sum = chip8->V[(opcode & 0x0F00)>>8] + chip8->V[(opcode & 0x00F0)>>4];

                if(sum > 255){
                    chip8->V[0xF] = 1;

                }else{
                    chip8->V[0xF]=0;
                }

                 chip8->V[(opcode & 0x0F00)>>8] = sum;

                break;
            
            case 0x5:
                uint16_t sub = chip8->V[(opcode & 0x00F0)>>4] - chip8->V[(opcode & 0x0F00)>>8];

                if(sub > 0){
                    chip8->V[0xF] = 0;
                }else{
                    chip8->V[0xF] = 1;
                }

                break;
            






        }




















    
    default:
        break;
    }




}



