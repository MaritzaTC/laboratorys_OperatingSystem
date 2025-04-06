package main

import (
	"bufio"
	"fmt"
	"os"
	"unicode"
)

// Removes non-alphabetic (except space) and non-ASCII characters
func cleanASCIIString(s string) string {
	var cleaned []byte
	for i := 0; i < len(s); i++ {
		ch := s[i]
		if ch <= 127 && (unicode.IsLetter(rune(ch)) || ch == ' ') {
			cleaned = append(cleaned, ch)
		}
	}
	return string(cleaned)
}

// Reverses a byte slice using pointers
func reverseBytes(b []byte) {
	start := 0
	end := len(b) - 1
	for start < end {
		b[start], b[end] = b[end], b[start]
		start++
		end--
	}
}

// Counts total vowels, individual vowels, and consonants
func countVowelsAndConsonants(b []byte) (int, map[byte]int, int) {
	vowels := map[byte]bool{
		'a': true, 'e': true, 'i': true, 'o': true, 'u': true,
		'A': true, 'E': true, 'I': true, 'O': true, 'U': true,
	}
	vowelMap := make(map[byte]int)
	vowelCount := 0
	consonantCount := 0

	for i := 0; i < len(b); i++ {
		ch := b[i]
		if unicode.IsLetter(rune(ch)) {
			if vowels[ch] {
				vowelCount++
				vowelMap[byte(unicode.ToLower(rune(ch)))]++
			} else {
				consonantCount++
			}
		}
	}
	return vowelCount, vowelMap, consonantCount
}

// Replaces spaces with underscores using pointers
func replaceSpacesWithUnderscore(b []byte) {
	for i := 0; i < len(b); i++ {
		if b[i] == ' ' {
			b[i] = '_'
		}
	}
}

func main() {
	// Read user input
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Enter a string (maximum 100 characters): ")
	input, _ := reader.ReadString('\n')

	// Remove non-alphabetic and non-ASCII characters
	cleaned := cleanASCIIString(input)

	// Convert to byte slice
	bytes := []byte(cleaned)

	// Reverse using pointers
	reverseBytes(bytes)

	fmt.Println("\n--- Results ---")
	fmt.Println("Reversed string:", string(bytes))

	// Count vowels and consonants
	vowelCount, vowelMap, consonantCount := countVowelsAndConsonants(bytes)

	// Replace spaces with underscores
	replaceSpacesWithUnderscore(bytes)

	fmt.Println("Number of vowels:", vowelCount)
	for v, c := range vowelMap {
		fmt.Printf("Number of \"%c\": %d\n", v, c)
	}
	fmt.Println("Number of consonants:", consonantCount)
	fmt.Println("Modified string:", string(bytes))
}
