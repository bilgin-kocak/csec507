#include <stdio.h>
#include <string.h>

char* hex2bin(const char* s) {
    char mp[16][5] = {
        "0000",
        "0001",
        "0010",
        "0011",
        "0100",
        "0101",
        "0110",
        "0111",
        "1000",
        "1001",
        "1010",
        "1011",
        "1100",
        "1101",
        "1110",
        "1111",
    };
    static char bin[512] = {0};
    for (int i = 0; i < strlen(s); i++) {
        if (s[i] >= '0' && s[i] <= '9') {
            strcat(bin, mp[s[i] - '0']);
        } else if (s[i] >= 'A' && s[i] <= 'F') {
            strcat(bin, mp[s[i] - 'A' + 10]);
        }
    }
    return bin;
}

char* bin2hex(const char* s) {
    char mp[16][5] = {
        "0000",
        "0001",
        "0010",
        "0011",
        "0100",
        "0101",
        "0110",
        "0111",
        "1000",
        "1001",
        "1010",
        "1011",
        "1100",
        "1101",
        "1110",
        "1111",
    };
    char hex_chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    static char hex[512] = {0};
    for (int i = 0; i < strlen(s); i += 4) {
        char ch[5] = {0};
        strncpy(ch, s + i, 4);
        for (int j = 0; j < 16; j++) {
            if (strcmp(ch, mp[j]) == 0) {
                hex[strlen(hex)] = hex_chars[j];
                break;
            }
        }
    }
    return hex;
}

int main() {
   // printf() displays the string inside quotation
   printf("Hello, World!");
   char* result = hex2bin("ABCD");
   printf("%s\n", result);
   char* result = bin2hex("1010");
   printf("%s\n", result);
   return 0;
}
