package main

import (
	"bufio"
	"fmt"
	"os"
)

// Removes non-alphabetic (except space) and non-ASCII characters using pointer
func cleanASCIIString(s string) string {
	var cleaned []byte
	for i := 0; i < len(s); i++ {
		ch := s[i]
		if ch <= 127 && ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == ' ') {
			cleaned = append(cleaned, ch)
		}
	}
	return string(cleaned)
}

// Reverses a byte slice using a pointer
func reverseBytes(b *[]byte) {
	start := 0
	end := len(*b) - 1
	for start < end {
		(*b)[start], (*b)[end] = (*b)[end], (*b)[start]
		start++
		end--
	}
}

// Counts vowels and consonants using pointer
func countVowelsAndConsonants(b *[]byte) (int, map[byte]int, int) {
	vowels := map[byte]bool{
		'a': true, 'e': true, 'i': true, 'o': true, 'u': true,
		'A': true, 'E': true, 'I': true, 'O': true, 'U': true,
	}
	vowelMap := make(map[byte]int)
	vowelCount := 0
	consonantCount := 0

	for i := 0; i < len(*b); i++ {
		ch := (*b)[i]
		if (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') {
			if vowels[ch] {
				vowelCount++
				// Guardamos siempre en minúsculas
				if ch >= 'A' && ch <= 'Z' {
					ch = ch + 32
				}
				vowelMap[ch]++
			} else {
				consonantCount++
			}
		}
	}
	return vowelCount, vowelMap, consonantCount
}

// Replaces spaces with underscores using a pointer
func replaceSpacesWithUnderscore(b *[]byte) {
	for i := 0; i < len(*b); i++ {
		if (*b)[i] == ' ' {
			(*b)[i] = '_'
		}
	}
}

func main() {
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Enter a string (maximum 100 characters): ")
	input, _ := reader.ReadString('\n')

	// Clean input (remove non-alphabetic, keep spaces)
	cleaned := cleanASCIIString(input)

	// Convert to byte slice and use pointer
	bytes := []byte(cleaned)

	// Reverse using pointer
	reverseBytes(&bytes)

	fmt.Println("\n--- Results ---")
	fmt.Println("Reversed string:", string(bytes))

	// Count vowels and consonants
	vowelCount, vowelMap, consonantCount := countVowelsAndConsonants(&bytes)

	// Replace spaces with underscores
	replaceSpacesWithUnderscore(&bytes)

	fmt.Println("Number of vowels:", vowelCount)
	for v, c := range vowelMap {
		fmt.Printf("Number of \"%c\": %d\n", v, c)
	}
	fmt.Println("Number of consonants:", consonantCount)
	fmt.Println("Modified string:", string(bytes))
}
