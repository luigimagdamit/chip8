#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

struct CPU {
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t i;

    uint16_t stack[16];

    uint8_t delay_timer;
    uint8_t sound_timer;

    uint16_t pc;

};
typedef struct {
    uint8_t opcode;
    uint8_t x;
    uint8_t y;
    uint8_t kk;
    uint16_t nnn;

} Chip8Instruction;

uint16_t encodeInstruction(Chip8Instruction instruction) {
    uint16_t encodedInstruction = 0;

    encodedInstruction |= (instruction.opcode & 0xFF) << 12;
    encodedInstruction |= (instruction.x & 0xF) << 8;
    encodedInstruction |= (instruction.y & 0xF) << 4;
    encodedInstruction |= (instruction.kk & 0xFF);

    return encodedInstruction;
}
void loadsSpritesIntoMemory(uint8_t memory[]) {
    // each element F, 0 is 4 bits, two f0 is 8, becoming a bit
    // multiply value by 16^position

    const uint8_t sprites[] = {
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
    int length = sizeof(sprites) / sizeof(sprites[0]);
    for(int i = 0; i < length; i++) {
        memory[i] = sprites[i];
    }
}

// input is 8 bit array
void loadProgramIntoMemory(uint8_t program[], int length, struct CPU *cpu) {
    for(int loc = 0; loc < length; loc++) {
        cpu->memory[0x200 + loc] = program[loc];
    }
}
int main(){
    struct CPU cpu;
    loadsSpritesIntoMemory(cpu.memory);

   FILE *file = fopen("chip8_instructions.txt", "r");
   if(file == NULL) {
    perror("Error opening file");
    return 1;
   }

   uint16_t memory[4096] = {0};
   Chip8Instruction instruction;
   int memoryIndex = 0;

   while(fscanf(file, "%hhx %hhx %hhx %hhx %hx",
    &instruction.opcode, &instruction.x, &instruction.y, &instruction.kk, &instruction.nnn) == 5) {
        uint16_t encodedInstruction = encodeInstruction(instruction);
        printf("0x%X\n", encodedInstruction);
        if(memoryIndex < 4096) {
            memory[memoryIndex] = encodedInstruction;
            memoryIndex++;
        } else {
            fprintf(stderr, "Memory overflow. Cannot load more instructions\n");
            break;
        }
    }
    fclose(file);


    return 0;
}