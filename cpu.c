#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define START_ADDRESS 0x200

struct CPU {
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t i;

    uint16_t stack[16];

    uint8_t delay_timer;
    uint8_t sound_timer;

    uint16_t pc;

};

void LoadROM(struct CPU *cpu, const char *filename) {
    FILE *file = fopen(filename, "rb");

    if(file != NULL) {
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);

        u_int8_t *buffer = (uint8_t*)malloc(size);

        if (buffer != NULL) {
            fread(buffer, 1, size, file);
            fclose(file);

            for(long i = 0; i < size; i++) {
                cpu->memory[START_ADDRESS+i] = buffer[i];
            }
            free(buffer);
        } else {
            perror("Memory allocation failed");
            fclose(file);
        }
    } else {
        perror("Failed to open file");
    }
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
        // lets store byte 1 first
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
void printMemory(struct CPU *cpu) {
    for(int i = 0; i < 548; i++) {
        printf("%d %02X \n", i, cpu->memory[i]);
    }
}
int main(){
    struct CPU cpu;
    loadsSpritesIntoMemory(cpu.memory);
    loadFile("test_opcode.ch8", &cpu);
    printMemory(&cpu);
    return 0;
}