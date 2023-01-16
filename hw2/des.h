#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define LB32_MASK 0x00000001
#define LB64_MASK 0x0000000000000001
#define L64_MASK 0x00000000ffffffff
#define H64_MASK 0xffffffff00000000

// Initial Permutation Table
static char IP[] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7};

// Inverse Initial Permutation Table
static char PI[] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25};

// Expansion table
static char E[] = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1};

// Post S-Box permutation
static char P[] = {
    16, 7, 20, 21,
    29, 12, 28, 17,
    1, 15, 23, 26,
    5, 18, 31, 10,
    2, 8, 24, 14,
    32, 27, 3, 9,
    19, 13, 30, 6,
    22, 11, 4, 25};

// The S-Box tables
static char S[8][64] = {{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
                         0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
                         4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
                         15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13},
                        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
                         3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
                         0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
                         13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9},
                        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
                         13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
                         13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
                         1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12},
                        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
                         13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
                         10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
                         3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14},
                        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
                         14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
                         4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
                         11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3},
                        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
                         10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
                         9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
                         4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13},
                        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
                         13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
                         1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
                         6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12},
                        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
                         1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
                         7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
                         2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}};

// Permuted Choice 1 Table
static char PC1[] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,

    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4};

// Permuted Choice 2 Table
static char PC2[] = {
    14, 17, 11, 24, 1, 5,
    3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32};

// Iteration Shift Array
static char iteration_shift[] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

/*
 * Function to generate 16 subkeys
 */
void *key_schedule(uint64_t key, uint64_t *sub_key)
{
    // uint64_t *sub_key = (uint64_t *)malloc(16 * sizeof(uint64_t));
    // uint64_t key = 0x133457799BBCDFF1;
    uint64_t temp = 0;
    uint32_t C = 0;
    uint32_t D = 0;
    int i, j;
    char row, column;

    /* Permuted Choice 1 */
    for (i = 0; i < 56; i++)
    {
        temp <<= 1;
        temp |= (key >> (64 - PC1[i])) & 1;
    }
    key = temp;

    /* Split into C0 and D0 */
    C = (key >> 28) & 0xFFFFFFF;
    D = key & 0xFFFFFFF;

    /* Generate 16 subkeys */
    for (i = 0; i < 16; i++)
    {
        /* Rotate C and D */
        for (j = 0; j < iteration_shift[i]; j++)
        {
            C = ((C << 1) & 0xFFFFFFF) | ((C >> 27) & 1);
            D = ((D << 1) & 0xFFFFFFF) | ((D >> 27) & 1);
        }

        /* Combine C and D */
        temp = ((uint64_t)C << 28) | D;

        /* Permuted Choice 2 */
        sub_key[i] = 0;
        for (j = 0; j < 48; j++)
        {
            sub_key[i] <<= 1;
            sub_key[i] |= (temp >> (56 - PC2[j])) & 1;
        }
    }
    // return sub_key;
}

/*
 * Initial Permutation
 */
uint64_t do_initial_permutation(uint64_t input)
{
    uint64_t output = 0;
    for (int i = 0; i < 64; i++)
    {
        output <<= 1;
        output |= (input >> (64 - IP[i])) & LB64_MASK;
    }
    return output;
}
/*
 * Inverse Initial Permutation
 */
uint64_t do_inverse_permutation(uint64_t input)
{
    uint64_t output = 0;
    for (int i = 0; i < 64; i++)
    {
        output <<= 1;
        output |= (input >> (64 - PI[i])) & LB64_MASK;
    }
    return output;
}
/*
 * Expansion function
 */
uint64_t do_expansion(uint32_t input)
{
    uint64_t output = 0;
    for (int i = 0; i < 48; i++)
    {
        output <<= 1;
        output |= (uint64_t)(input >> (32 - E[i])) & LB32_MASK;
    }
    return output;
}
/*
 * S-Box Substitution function
 */
uint32_t do_sbox_substitution(uint64_t input)
{
    uint32_t output = 0;
    char row, column;
    for (int i = 0; i < 8; i++)
    {
        row = (char)((input & (0x0000840000000000 >> 6 * i)) >> 42 - 6 * i);
        row = (row >> 4) | row & 0x01;
        column = (char)((input & (0x0000780000000000 >> 6 * i)) >> 43 - 6 * i);
        output <<= 4;
        output |= (uint32_t)(S[i][16 * row + column] & 0x0f);
    }
    return output;
}

/*
 * After Permutation function
 * input: 32 bit message
 * output: 32 bit message
 */
uint64_t do_after_sbox_permutation(uint64_t input)
{
    uint64_t output = 0;
    for (int i = 0; i < 32; i++)
    {
        output <<= 1;
        output |= (input >> (32 - P[i])) & LB32_MASK;
    }
    return output;
}

/*
 * The DES function
 * input: 64 bit message
 * key: 64 bit key for encryption/decryption
 * mode: 'e' = encryption; 'd' = decryption
 * round_output: 'y' = print round output; 'n' = no print
 */
uint64_t des(uint64_t input, uint64_t key, char mode, char round_output)
{
    int i, j;

    /* 32 bits */
    uint32_t L = 0;
    uint32_t R = 0;
    uint32_t s_output = 0;
    uint32_t after_sbox_permutation = 0;
    uint32_t temp = 0;

    /* 48 bits */
    uint64_t sub_key[16] = {0};
    uint64_t s_input = 0;

    /* 64 bits */
    uint64_t initial_permutation = 0;
    uint64_t inv_initial_permutation = 0;
    uint64_t pre_output = 0;

    /* initial permutation */
    initial_permutation = do_initial_permutation(input);

    L = (uint32_t)(initial_permutation >> 32) & L64_MASK;
    R = (uint32_t)initial_permutation & L64_MASK;

    key_schedule(key, sub_key);

    // Round function
    for (i = 0; i < 16; i++)
    {

        /* Expansion Function 32 bits to 48 bits */
        s_input = do_expansion(R);

        /*
         * Encryption/Decryption
         * XORing expanded Ri with Ki
         */
        if (mode == 'd')
        {
            // decryption
            s_input = s_input ^ sub_key[15 - i];
        }
        else
        {
            // encryption
            s_input = s_input ^ sub_key[i];
        }

        /* S-Box Substitution */
        uint32_t s_output = do_sbox_substitution(s_input);

        after_sbox_permutation = do_after_sbox_permutation(s_output);

        /* Swap*/
        temp = R;
        R = L ^ after_sbox_permutation;
        L = temp;

        if (round_output == 'y')
        {
            printf("R: %08x L: %08x K: %012llx \n", L, R, sub_key[i]);
        }
        // pre_output = (((uint64_t)R) << 32) | (uint64_t)L;
        // printf("r: %i : %016llx : %x\n", i, pre_output, sub_key[i]);
    }

    // Concatenate Left and Right halves
    pre_output = (((uint64_t)R) << 32) | (uint64_t)L;

    /* inverse initial permutation */
    inv_initial_permutation = do_inverse_permutation(pre_output);

    return inv_initial_permutation;
}
/*
 * Random 64 bit number generator
 */
uint64_t rand_uint64_slow(void)
{
    uint64_t r = 0;
    for (int i = 0; i < 64; i++)
    {
        r = r * 2 + rand() % 2;
    }
    return r;
};
/*
 * CBC DES
 */
void cbc_des(uint64_t *inputs, uint64_t *outputs, uint64_t key, uint8_t block_number)
{
    uint64_t iv = rand_uint64_slow();
    uint64_t output = iv;

    // First block
    output = des(inputs[0] ^ output, key, 'e', 'n');
    outputs[0] = output;
    // Rest of the blocks
    for (int i = 1; i < block_number; i++)
    {
        output = des(inputs[i] ^ output, key, 'e', 'n');
        outputs[i] = output;
    }
};