# include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
# include <string.h>

void reverse(char *str){
    char *start = str;
    char *end = str;
    // Move end pointer to the last character
    while (*end != '\0') {
        end++;
    }
    end--; // Move back to the last character
    // Swap characters until start and end pointers meet
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

int isAlphabetic(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

// convert all non-alphabetic characters 
void modifyString(char *str){
    while(*str != '\0') {
        if (!isAlphabetic(*str)) {
            *str = ' ';
        }
        str++;
    }
}
void replaceSpacesWithUnderscore(char *str) {
    while (*str != '\0') {
        if (*str == ' ') {
            *str = '_';
        }
        str++;
    }
}

void countVowelsAndConsonants(char *str){
    int vowelsCount[5] = {0}; // a, e, i, o, u
    int consonantsCount = 0; // b, c, d, f, g, h, j, k, l, m, n, p, q, r, s, t, v, w, x, y
    char *ptr = str;

    while (*ptr != '\0') {
        char c = tolower(*ptr);
        if (c >= 'a' && c <= 'z') {
            switch (c) {
                case 'a': vowelsCount[0]++; break;
                case 'e': vowelsCount[1]++; break;
                case 'i': vowelsCount[2]++; break;
                case 'o': vowelsCount[3]++; break;
                case 'u': vowelsCount[4]++; break;
                default:
                    if (isAlphabetic(c)) {
                        consonantsCount++;  
                    }
                    break;
            }
        }
        ptr++;
    }
    printf("Number of vowels: %d\n", vowelsCount[0] + vowelsCount[1] + vowelsCount[2] + vowelsCount[3] + vowelsCount[4]);
    printf("Number of 'a': %d, number of 'e': %d, number of 'i': %d, number of 'o': %d, number of 'u': %d\n",
           vowelsCount[0], vowelsCount[1], vowelsCount[2], vowelsCount[3], vowelsCount[4]);
    printf("Number of consonants: %d\n", consonantsCount);
}

int main() {
    char *str;
    int max_length = 101;
    str = (char *)malloc(max_length * sizeof(char)); // Allocate memory for the string
    if (str == NULL) {
        fprintf(stderr, "Error: Memory allocation failed!\n");
        return 1; 
    }
    printf("Enter a string (maximum length 100 characters) : ");
    if (fgets(str, max_length, stdin) == NULL) {
        fprintf(stderr, "Error: Failed to read input!\n");
        free(str);
        return 1;
    }
    str[strcspn(str, "\n")] = '\0';
    modifyString(str); // Convert non-alphabetic characters to spaces
    reverse(str);
    printf("Reversed string: %s\n", str);
    countVowelsAndConsonants(str);
    replaceSpacesWithUnderscore(str); // Replace spaces with underscores
    printf("Modified string: %s\n", str);
    free(str); // Free the allocated memory
    return 0;
}

