
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "des.h"
#include <time.h>

int main(int argc, const char *argv[])
{
    // Question 1
    uint64_t input = 0x4e6f772069732074;
    uint64_t key = 0x0123456789abcdef;
    uint64_t result = input;

    printf("Question 1 \n");
    // Encrypt
    result = des(input, key, 'e', 'y');
    printf("E: %016llx\n", result);

    // Decrypt
    result = des(result, key, 'd', 'n');
    printf("D: %016llx\n", result);

    // Question 2
    printf("\n******************************\n");
    printf("Question 2 \n");

    char plain_text[] = "Bilgin Kocak";
    // Ascii to Decimal Conversion
    // 66 105 108 103 105 110 32 75 111 99 97 107
    // Asci to Hex Conversion
    // 42 69 6c 67 69 6e 20 4b 6f 63 61 6b
    uint64_t inputs[2] = {0x42696c67696e204b, 0x6f63616b80000000}; // {Bilgin K, ocak}
    key = 0x42696c67696e204b;
    uint64_t outputs[2] = {0};

    // Number of blocks
    uint8_t block_number = sizeof(inputs) / sizeof(inputs[0]);

    cbc_des(inputs, outputs, key, block_number);

    printf("CBC DES Encryption\n");
    for (int i = 0; i < block_number; i++)
    {
        printf("%016llx\n", outputs[i]);
    }

    // Question 3
    printf("\n******************************\n");
    printf("Question 3 \n");

    clock_t start, end;
    double cpu_time_used;

    uint64_t input_3 = 0x4e6f772069732074;
    uint64_t key_3 = 0x0123456789abcdef;
    uint64_t result_3 = input_3;

    start = clock();
    for (int i = 0; i < pow(2, 23); i++)
    {
        result_3 = des(result_3, key_3, 'e', 'n');
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("DES Encryption tooks %f seconds on a single core of Intel(R) Core(TM) i7-8750H CPU @ 2.20GHz  2.20 GHz \n ", cpu_time_used);

    exit(0);
}
