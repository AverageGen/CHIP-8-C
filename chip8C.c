#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <SDL2/SDL.h>

#define CYCLES_PER_FRAME 10
#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32
#define SCALE 10
#define FONT_START 0X050
#define ROM_START 0X200


uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};



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




int load_rom(struct Chip_8 *chip8, const char  *filename ){

    FILE *rom = fopen(filename, "rb");


    if(rom == NULL){
        printf("Erro ao abrir rom\n");
        return 0;
    }

    size_t bytes_read =  fread(&chip8->memory[ROM_START],
        1, 
        sizeof(chip8->memory) - ROM_START,
        rom
    );

    fclose(rom);

    printf("rom carregada\n");

    return 1;

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
    case 0xD: {
        uint8_t x_index = (opcode & 0x0F00) >> 8;
        uint8_t y_index = (opcode & 0x00F0) >> 4;
        uint8_t height = opcode & 0x000F;

        uint8_t x_pos = chip8->V[x_index] % 64;
        uint8_t y_pos = chip8->V[y_index] % 32;

        chip8->V[0xF] = 0;

        for(int row = 0; row < height; row++){
            uint8_t sprite_byte = chip8->memory[chip8->I + row];

            for(int col = 0; col < 8; col++){
                uint8_t sprite_pixel = (sprite_byte >> (7 - col)) & 0x1;

                if(sprite_pixel){
                    int screen_x = (x_pos + col) % 64;
                    int screen_y = (y_pos + row) % 32;

                    if(chip8->display[screen_x][screen_y] == 1){
                        chip8->V[0xF] = 1;
                    }

                    chip8->display[screen_x][screen_y] ^= 1;
                }
            }
        }

    break;
    }

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
            uint8_t x = (opcode & 0x0F00) >> 8;

            int keypressed = -1;

            for(int i = 0; i<16; i++){
                if(chip8->keypad[i]){
                    keypressed = i;
                    break;
                }
            }

            if(keypressed == -1){
                chip8->PC-=2;
            }else{
                chip8->V[x] = keypressed;
            }

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
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t digit = chip8->V[x] & 0x0F;


            chip8->I = FONT_START + (digit * 5);

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



void render_display(SDL_Renderer *renderer, struct Chip_8 *chip8 ){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


    for(int y = 0; y < 32; y++ ){
        for( int x = 0; x< 64; x++){
            if(chip8->display[x][y]){
                SDL_Rect rect = {
                    x * SCALE,
                    y * SCALE,
                    SCALE,
                    SCALE
                };  
                SDL_RenderFillRect(renderer, &rect);
            }

            



        }
    }


    SDL_RenderPresent(renderer);
    
}




int main(int argc, char *argv[]) {
    struct Chip_8 chip8;
    memset(&chip8, 0, sizeof(chip8));

    chip8.PC = ROM_START;

    if(!load_rom(&chip8, "Pong.ch8")){
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "chip-8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        CHIP8_WIDTH * SCALE,
        CHIP8_HEIGHT * SCALE,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );




    for(int i = 0; i < 80; i++){
        chip8.memory[FONT_START + i] = fontset[i];
    }


    int running = 1;


    while(running){
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT){
                running = 0;
            }

            if(event.type == SDL_KEYDOWN){
                switch (event.key.keysym.sym)
                {
                case SDLK_1: chip8.keypad[0x1] = 1; break;
                case SDLK_2: chip8.keypad[0x2] = 1; break;
                case SDLK_3: chip8.keypad[0x3] = 1; break;
                case SDLK_4: chip8.keypad[0xC] = 1; break;

                case SDLK_q: chip8.keypad[0x4] = 1; break;
                case SDLK_w: chip8.keypad[0x5] = 1; break;
                case SDLK_e: chip8.keypad[0x6] = 1; break;
                case SDLK_r: chip8.keypad[0xD] = 1; break;

                case SDLK_a: chip8.keypad[0x7] = 1; break;
                case SDLK_s: chip8.keypad[0x8] = 1; break;
                case SDLK_d: chip8.keypad[0x9] = 1; break;
                case SDLK_f: chip8.keypad[0xE] = 1; break;

                case SDLK_z: chip8.keypad[0xA] = 1; break;
                case SDLK_x: chip8.keypad[0x0] = 1; break;
                case SDLK_c: chip8.keypad[0xB] = 1; break;
                case SDLK_v: chip8.keypad[0xF] = 1; break;
                }

            }


            if(event.type == SDL_KEYUP){
                switch (event.key.keysym.sym)
                {
                case SDLK_1: chip8.keypad[0x1] = 0; break;
                case SDLK_2: chip8.keypad[0x2] = 0; break;
                case SDLK_3: chip8.keypad[0x3] = 0; break;
                case SDLK_4: chip8.keypad[0xC] = 0; break;

                case SDLK_q: chip8.keypad[0x4] = 0; break;
                case SDLK_w: chip8.keypad[0x5] = 0; break;
                case SDLK_e: chip8.keypad[0x6] = 0; break;
                case SDLK_r: chip8.keypad[0xD] = 0; break;

                case SDLK_a: chip8.keypad[0x7] = 0; break;
                case SDLK_s: chip8.keypad[0x8] = 0; break;
                case SDLK_d: chip8.keypad[0x9] = 0; break;
                case SDLK_f: chip8.keypad[0xE] = 0; break;

                case SDLK_z: chip8.keypad[0xA] = 0; break;
                case SDLK_x: chip8.keypad[0x0] = 0; break;
                case SDLK_c: chip8.keypad[0xB] = 0; break;
                case SDLK_v: chip8.keypad[0xF] = 0; break;
                }

            }




        }
        
        for (int i = 0; i < CYCLES_PER_FRAME; i++){
            uint16_t opcode = (chip8.memory[chip8.PC]<<8 | chip8.memory[chip8.PC+1]);
            chip8.PC+=2;

            fetch_and_decode(opcode, &chip8);
        }


        if(chip8.DelayTimer > 0){
            chip8.DelayTimer--;
        }

        if(chip8.SoundTimer > 0){
            chip8.SoundTimer--;
        }
        



        render_display(renderer, &chip8);

        SDL_Delay(16);


    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();





    return 0;


}
