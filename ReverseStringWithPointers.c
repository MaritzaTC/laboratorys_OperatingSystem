# include <stdio.h>
#include <ctype.h> 

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
// convert all non-alphabetic characters 
void alphaString(char *str){
    char *read = str;
    char *write = str;

    while(*read){
        if(isalpha(*read)){
            *write = *read;
            write++;
        }else if (*read == ' '){
            *write = '_';
            write++;
        }
        read++;
    }
    *write = '\0'; // Null-terminate the new string
}




void vowels(char *str){
    char vowels[] = "aeiouAEIOU";
    int count = 0;
    for(int i = 0; str[i] != '\0'; i++) {
        for (int j = 0; vowels[j] != '\0'; j++) {
            if (str[i] == vowels[j]) { 
                count++;  
                printf("Number of : %c %d,", str[i], count);
            }
        }
    }
    printf("\nNumber of vowels: %d\n", count);
}

void consonants (char *str){
    char consonants[] = "bcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZ";
    int count = 0;
    for(int i = 0; str[i] != '\0'; i++) {
        for (int j = 0; consonants[j] != '\0'; j++) {
            if (str[i] == consonants[j]) { 
                count++; 
            }
        }
    }
    printf("\nNumber of consonants: %d", count);
}	



int main() {
    char str[] = "Hel2lo1, World!";
    printf("Original string: %s\n", str);   
    reverse(str);
    printf("Reversed string: %s\n", str);
    alphaString(str);
    printf("Alpha string: %s\n", str);
    vowels(str);
    consonants(str);
    return 0;
}