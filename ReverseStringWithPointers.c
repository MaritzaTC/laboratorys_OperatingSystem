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
        }
        read++;
    }
    *write = '\0'; // Null-terminate the new string
}

int main() {
    char str[] = "Hel2lo1, World!";
    printf("Original string: %s\n", str);   
    reverse(str);
    printf("Reversed string: %s\n", str);
    alphaString(str);
    printf("Alpha string: %s\n", str);

    return 0;
}