#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

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

uint8_t generateRandom() {
    srand(time(NULL));
    uint8_t res = rand();
    return res * 0xFF; 

}
void push(struct CPU *cpu, uint16_t add) {
    cpu->stack[cpu->stack_size] = add;
    cpu->stack_size+=1;
}
uint16_t pop(struct CPU *cpu) {
    uint16_t temp;
    temp = cpu->stack[cpu->stack_size - 1];
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
        uint8_t sum = (cpu->V[x] + cpu->V[y]);


        uint8_t random = generateRandom();
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
                cpu->pc = opcode & 0xFFF;
                printf("\nPC CHANGE to nnn: %03X\n", cpu->pc);
                break; 
            case 0x2000:
                printf("CALL addr");
                printf("\nnnn:%03X", opcode & 0xFFF);
                break; 
            case 0x3000:
                printf("V%X (%X) EQUALS ? %02X - %04X \n", x, cpu->V[x], opcode & 0xFF, opcode); 
                if (cpu->V[x] == (opcode & 0xFF)) {
                    printf("true");
                    cpu->pc += 2;
                }
                break; 
            case 0x4000:
                printf("V%X (%X) NOT EQUALS ? %02X - %04X \n", x, cpu->V[x], opcode & 0xFF, opcode); 
                if (cpu->V[x] != (opcode & 0xFF)) {
                    printf("true");
                    cpu->pc += 2;
                }
                break; 
            case 0x5000:
                printf("SE Vx Vy");
                printf("V%X = V%X?", x, y);
                if (cpu->V[x] == cpu->V[y]) {
                    printf("true");
                    cpu->pc += 2;
                }
                break; 
            case 0x6000:
                printf("SET V%X to %02X\n", x, opcode & 0xFF);
                printf("0x600");
                cpu->V[x] = (opcode & 0xFF);
                break; 
            case 0x7000:
                printf("0x700");
                cpu->V[x] += (opcode & 0xFF);
                break; 
            case 0x8000:
                printf("0x800o");
                switch(opcode & 0xF) {
                    case 0x0:
                        printf("\nSET V%X = V%X", x, y);
                        cpu->V[x] = cpu->V[y];
                        break;
                    case 0x1:
                        cpu->V[x] |= cpu->V[y];
                        break;
                    case 0x2:
                        cpu->V[x] &= cpu->V[y];
                        break;
                    case 0x3:
                        cpu->V[x] ^= cpu->V[y];
                        break;
                    case 0x4:
                        cpu->V[0xF] = 0;

                        if(sum > 0xFF) {
                            cpu->V[0xF] = 1;
                        }
                        cpu->V[x] = sum;
                        break;
                    case 0x5:
                        cpu->V[0xF] = 0;
                        if(cpu->V[x] > cpu->V[y]) {
                            cpu->V[0xF] = 1;
                        }
                        
                        cpu->V[x] -= cpu->V[y];
                        break;
                    case 0x6:
                        cpu->V[0xF] = cpu->V[x] & 0x1;
                        cpu->V[x] >>= 1;
                        break;
                    case 0x7:
                        cpu->V[0xF] = 0;
                        
                        if(cpu->V[y] > cpu->V[x]) {
                            cpu->V[0xF] = 1;
                        }
                        cpu->V[x] = cpu->V[y] - cpu->V[x];
                        break;
                    case 0xE:
                        cpu->V[0xF] = (cpu->V[x] & 0x80);
                        cpu->V[x] <<= 1;
                        break;
                }
                break; 
        case 0x9000:
            printf("\n9000: V[%X] ?= V[%X]", x, y);
            //if(cpu->V[x] !== cpu->V[y]) {
            //    cpu->pc = cpu->pc + 2;
            //}
            if(cpu->V[x] != cpu->V[y]) {
                printf("CASE NOTED");
                cpu->pc += 2;
            }
            break;
        
        case 0xA000:
            cpu->i = (opcode & 0xFFF);
            break;
        case 0xB000:
            cpu->pc = (opcode & 0xFFF) + cpu->V[0];
            break;
        case 0xC000:
            printf("\nRANDOM NUMBER: %02X\n", random);
            cpu->V[x] = random & (opcode & 0xFF);
            printf("\nVx: %02X\n", cpu->V[x]);
            break;
        case 0xD000:
            break;
        case 0xE000:
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
        printf("\n|---MEM POS: %d - CONTENT: %02X--|\n", i, cpu->memory[i]);
    }
}
void printStack(struct CPU *cpu) {
    for(int i = 0; i < 16; i++) {
        printf("\n|---%04X---|\n", cpu->stack[i]);
    }
    printf("\n--END OF STACK--\n");
}
void printRegisters(struct CPU *cpu) {
    for(int i = 0; i < 16; i++) {
        printf("\n---V[%d]: %02X---\n", i, cpu->V[i]);
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
    execute(&cpu, 100);
    push(&cpu, 0xFFFF);
    push(&cpu, 0xFFFE);
    cpu.V[2] = 0x03;
    printf("\n%X\n", cpu.pc);
    readOpcode(&cpu, 0xC210);
    printf("\n%X\n", cpu.pc);
    return 0;
}
