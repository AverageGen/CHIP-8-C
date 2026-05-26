#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

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
    uint8_t keypad[16];

};





int main() {






}


void fetch_and_decode(uint16_t opcode, struct Chip_8 *chip8){

    switch ((opcode & 0xF000) >> 12)
    {

    case 0x0: {

        if((opcode & 0x000F) == 0x0){
            memset(chip8->display, 0, sizeof(chip8->display));
        }else{
            chip8->StackPointer--;
            chip8->PC = chip8->stack[chip8->StackPointer];
        }

        break;
    }

    case 0x1: {

        chip8->PC = (opcode & 0x0FFF);

        break;
    }

    case 0x2: {

        chip8->stack[chip8->StackPointer] = chip8->PC;
        chip8->StackPointer++;

        chip8->PC = opcode & 0x0FFF;

        break;
    }

    case 0x3: {

        if((opcode & 0x00FF) == chip8->V[(opcode & 0x0F00) >> 8]){
            chip8->PC += 2;
        }

        break;
    }

    case 0x4: {

        if((opcode & 0x00FF) != chip8->V[(opcode & 0x0F00) >> 8]){
            chip8->PC += 2;
        }

        break;
    }

    case 0x5: {

        if(chip8->V[(opcode & 0x0F00) >> 8] ==
           chip8->V[(opcode & 0x00F0) >> 4]){

            chip8->PC += 2;
        }

        break;
    }

    case 0x6: {

        chip8->V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;

        break;
    }

    case 0x7: {

        chip8->V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;

        break;
    }

    case 0x8: {

        uint8_t last_bit = opcode & 0x000F;

        switch(last_bit){

            case 0x0: {

                chip8->V[(opcode & 0x0F00) >> 8] =
                chip8->V[(opcode & 0x00F0) >> 4];

                break;
            }

            case 0x1: {

                chip8->V[(opcode & 0x0F00) >> 8] =
                chip8->V[(opcode & 0x0F00) >> 8] |
                chip8->V[(opcode & 0x00F0) >> 4];

                break;
            }

            case 0x2: {

                chip8->V[(opcode & 0x0F00) >> 8] =
                chip8->V[(opcode & 0x0F00) >> 8] &
                chip8->V[(opcode & 0x00F0) >> 4];

                break;
            }

            case 0x3: {

                chip8->V[(opcode & 0x0F00) >> 8] =
                chip8->V[(opcode & 0x0F00) >> 8] ^
                chip8->V[(opcode & 0x00F0) >> 4];

                break;
            }

            case 0x4: {

                uint16_t sum =
                    chip8->V[(opcode & 0x0F00) >> 8] +
                    chip8->V[(opcode & 0x00F0) >> 4];

                if(sum > 255){
                    chip8->V[0xF] = 1;
                }else{
                    chip8->V[0xF] = 0;
                }

                chip8->V[(opcode & 0x0F00) >> 8] = sum;

                break;
            }

            case 0x5: {

                if(chip8->V[(opcode & 0x0F00)>>8] >= chip8->V[(opcode & 0x00F0)>>4]){
                    chip8->V[0xF] = 1;
                }else{
                    chip8->V[0xF] = 0;
                }

                chip8->V[(opcode & 0x0F00)>>8] =  chip8->V[(opcode & 0x0F00)>>8] - chip8->V[(opcode & 0x00F0)>>4];




                break;
            }

            case 0x6: {

                chip8->V[0xF] =
                    chip8->V[(opcode & 0x00F0) >> 4] & 0x1;

                chip8->V[(opcode & 0x0F00) >> 8] =
                    chip8->V[(opcode & 0x00F0) >> 4] >> 1;

                break;
            }

            case 0x7: {



                if(chip8->V[(opcode & 0x00F0)>>4] >= chip8->V[(opcode & 0x0F00) >> 8]){
                    chip8->V[0xF] = 1;
                }else{
                    chip8->V[0xF] = 0;
                }

                chip8->V[(opcode & 0x0F00) >> 8] = chip8->V[(opcode & 0x00F0)>>4] - chip8->V[(opcode & 0x0F00) >> 8];

                break;
            }

            case 0xE: {

                chip8->V[0xF] =
                    (chip8->V[(opcode & 0x00F0) >> 4] & 0x80) >> 7;

                chip8->V[(opcode & 0x0F00) >> 8] =
                    chip8->V[(opcode & 0x00F0) >> 4] << 1;

                break;
            }
        }

        break;
    }

    case 0x9: {

        if(chip8->V[(opcode & 0x0F00) >> 8] !=
           chip8->V[(opcode & 0x00F0) >> 4]){

            chip8->PC += 2;
        }

        break;
    }

    case 0xA: {

        chip8->I = opcode & 0x0FFF;

        break;
    }

    case 0xB: {

        chip8->PC =
            (opcode & 0x0FFF) + chip8->V[0x0];

        break;
    }
    case 0xC: {
        chip8->V[(opcode & 0x0F00)>>8] = (rand() % 256) & (opcode & 0x00FF);



        break;

    }
    case 0xD:{


    }

    break;

    case 0xE:{
        uint8_t last_byte = opcode & 0x00FF;

        switch (last_byte)
        {
        case 0x9E: {
            if(chip8->keypad[chip8->V[(opcode & 0x0F00)>>8]]){
                chip8->PC+=2;
            }
        }

            break;

        case 0xA1:{
            if(!chip8->keypad[chip8->V[(opcode & 0x0F00)>>8]]){
                chip8->PC+=2;
            }
        }

        break;
        
        default:
            break;
        }

    }
    
    break;

    case 0xF: {
        uint8_t last_byte = opcode & 0x00FF;

        switch (last_byte)
        {
        case 0x07:{
            chip8->V[(opcode & 0x0F00)>>8] = chip8->DelayTimer;

        }
            
            break;

        case 0x0A:{
            
        }

        break;

        case 0x15:{
            chip8->DelayTimer = chip8->V[(opcode & 0x0F00)>>8];

        }

        break;

        case 0x18: {
            chip8->SoundTimer = chip8->V[(opcode & 0x0F00)>>8];


        }
        break;

        case 0x1E: {
            chip8->I += chip8->V[(opcode & 0x0F00)>>8];
        }
        break;

        case 0x29:{

        }
        break;

        case 0x33:{
            uint8_t valor = chip8->V[(opcode & 0x0F00)>>8];

            chip8->memory[chip8->I] = valor / 100;
            chip8->memory[chip8->I+1] = (valor / 10) % 10;
            chip8->memory[chip8->I+2] = valor % 10;

            break;

        }
        case 0x55:{
            uint8_t x = (opcode & 0x0F00) >> 8;

            for(int i = 0; i<=x;i++){
                chip8->memory[chip8->I+i] = chip8->V[i];
            }

            chip8->I += x+1;



            break;
        }
        case 0x65:{           
            uint8_t x = (opcode & 0x0F00) >> 8;

            for(int i = 0; i <= x; i++){
                chip8->V[i] = chip8->memory[chip8->I + i];
            }

            chip8->I += x + 1;

            break;

        }

        default:
            break;
        }



        break;

    }





    default: {

        break;
    }

    }
}