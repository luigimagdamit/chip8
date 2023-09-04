#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define START_ADDRESS 0x200

struct CPU {
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t i;

    uint16_t stack[16];
    int stack_size;
    uint8_t delay_timer;
    uint8_t sound_timer;

    uint16_t pc;

};

void push(struct CPU *cpu, uint16_t add) {
    cpu->stack[cpu->stack_size] = add;
    cpu->stack_size+=1;
}
uint16_t pop(struct CPU *cpu) {
    uint16_t temp;
    temp = cpu->stack[cpu->stack_size - 1];
    printf("TEMP: %X", temp);
    cpu->stack[cpu->stack_size - 1] = 0;

    cpu->stack_size -= 1;
    return temp;
}
int loadsSpritesIntoMemory(uint8_t memory[]) {
    // each element F, 0 is 4 bits, two f0 is 8, becoming a bit
    // multiply value by 16^position
    int index = 0;
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
        index++;
        memory[i] = sprites[i];
    }
    return index;
}
size_t readHexCodesFromFile(const char *filename, uint16_t *hexValues, size_t maxValues) {
    FILE *file;
    size_t numHexValues = 0;


    file = fopen(filename, "rb");

    if(file == NULL) {
        perror("Error opening file");
    }

    while(numHexValues < maxValues && fread(&hexValues[numHexValues], sizeof(uint16_t), 1, file) == 1) {
        numHexValues++;
    }
    fclose(file);

    return numHexValues;
}

void splitHexCode(uint16_t hexCode, uint8_t *byte1, uint8_t *byte2) {
    *byte1 = (uint8_t)(hexCode & 0xFF); 
    *byte2 = (uint8_t)((hexCode >> 8) & 0xFF);
}
// input is 8 bit array

void loadHexValues(uint16_t hexValues[], struct CPU *cpu, size_t numHexValues) {
    uint8_t b1;
    uint8_t b2;

    int memoryIndex = 0;
    for(size_t i = 0; i < numHexValues; i++) {
        printf("Hex Code %zu: 0x%04X\n", i + 1, hexValues[i]);
        splitHexCode(hexValues[i], &b1, &b2);
        printf("%02X %02X \n", b1, b2);
        // lets store byte 2 first
        cpu->memory[START_ADDRESS + memoryIndex] = b2;
        cpu->memory[START_ADDRESS + memoryIndex + 1] = b1;
        memoryIndex+= 2;
    }
}
void loadFile(char *filename, struct CPU *cpu) {

    size_t numHexValues;
    uint16_t hexValues[100];

    numHexValues = readHexCodesFromFile(filename, hexValues, 100);
    
    if (numHexValues == 0) {
        printf("No hex codes read from the file.\n");
    } else {
        printf("Hex Codes read: %zu\n", numHexValues);
        loadHexValues(hexValues, cpu, numHexValues);
    }
} 

void readOpcode(struct CPU *cpu, uint16_t opcode) {
        uint8_t x = (opcode & 0xF00) >> 8;
        uint8_t y = (opcode & 0x00F0) >> 4;
        printf("\n\n--%04X--\n", opcode);
        printf("\nInstruction %d \nValue X: %02X\n Value Y: %02X\n OPCODE: %04X\n", 0, x, y, opcode & 0xF000);

        switch(opcode & 0xF000) {
            case 0x0000:
                printf("0x000");
                switch(opcode) {
                    case 0x00E0:
                    printf("CLEAR DISPLAY");
                        break;
                    case 0x00EE:
                        printf("POP STACK - STORE IN PC - CURRENT PC = %X\n", cpu->pc);
                        cpu->pc = pop(cpu);
                        printf("\nCURRENT PC: %X", cpu->pc);
                        break;
                }
                break;
            case 0x1000:
                printf("\nJP addr");
                printf("\nnnn %03X", opcode & 0xFFF);
                //cpu->pc = opcode & 0xFFF;
                //printf("PC CHANGE: %03X", cpu->pc);
                break; 
            case 0x2000:
                printf("CALL addr");
                printf("\nnnn:%03X", opcode & 0xFFF);
                break; 
            case 0x3000:
                printf("V%X EQUALS ? %02X - %04X \n", x, opcode & 0xFF, opcode); 
                //if (cpu->V[x] == (opcode & 0xFF)) {
                //    cpu->pc += 2;
                //}
                break; 
            case 0x4000:
                printf("SNE - SKIP INSTRUCTION IF NOT EQUAL");
                printf("V%X EQUALS ? %02X - %04X \n", x, opcode & 0xFF, opcode); 
                break; 
            case 0x5000:
                printf("SE Vx Vy");
                printf("V%X = V%X?", x, y);
                break; 
            case 0x6000:
                printf("SET V%X to %02X\n", x, opcode & 0xFF);
                printf("0x600");
                break; 
            case 0x7000:
                printf("0x700");
                break; 
            case 0x8000:
                printf("0x800o");
                switch(opcode & 0xF) {
                    case 0x0:
                        printf("\nSET V%X = V%X", x, y);
                        break;
                    case 0x1:
                        break;
                    case 0x2:
                        break;
                    case 0x3:
                        break;
                    case 0x4:
                        break;
                    case 0x5:
                        break;
                    case 0x6:
                        break;
                    case 0x7:
                        break;
                    case 0xE:
                        break;
                }
                break; 
        };
        cpu->pc += 2;
}
void execute(struct CPU *cpu, int limit) {
    for(int i = 0; i < limit; i++) {
        uint16_t opcode = cpu->memory[cpu->pc] << 8 | cpu->memory[cpu->pc + 1];
        readOpcode(cpu, opcode);
    }
}
void printMemory(struct CPU *cpu, int limit) {
    for(int i = 0; i < limit; i++) {
        printf("%d %02X \n", i, cpu->memory[i]);
    }
}
void initialize(struct CPU *cpu) {
    cpu->pc = START_ADDRESS;
    cpu->stack_size = 0;
    loadsSpritesIntoMemory(cpu->memory);
}
int main(){
    struct CPU cpu;
    initialize(&cpu);
    loadFile("IBM Logo.ch8", &cpu);
  //  execute(&cpu, 100);
    readOpcode(&cpu, 0x00EE);
    return 0;
}
