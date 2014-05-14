#include <iostream>

// The lexer returns tokens [0-255] if it is an unknown character, otherwise one
// of these for known ops.
enum Token {
	tok_eof = -1,			// EOF
	tok_def = -2,			// Function definition ("def")
	tok_extern = -3,		// External prototype definition ("extern")
	tok_identifier = -4,	// String identifier 
	tok_number = -5,		// Int/Double number
};

static std::string identifier;		// Filled in if tok_identifier
static double number;				// Filled in if tok_number

static int gettoken() {
	static int currChar = ' ';

	// Skip any whitespace.
	while (isspace(currChar)) currChar = getchar();

	// matches regex [a-zA-Z][a-zA-Z0-9]*
	if (isalpha(currChar))
	{ 
		identifier = currChar;
		//add all following characters to the identifier
		while (isalnum((currChar = getchar()))) identifier += currChar;

		//return tok_def if it is a definition, else if it is an extern return tok_extern
		if (identifier == "def") return tok_def;
		if (identifier == "extern") return tok_extern;

		//doesn't match anything, so just return the idenfitier token
		return tok_identifier;
	}

	// matches regex [0-9.]+
	if (isdigit(currChar) || currChar == '.') {   
		std::string temp;
		do {
			temp += currChar;
			currChar = getchar(); //get next char
		} while (isdigit(currChar) || currChar == '.');

		//set the raw value of temp to the number
		number = strtod(temp.c_str(), 0);

		//return tok_number (found a number)
		return tok_number;
	}

	if (currChar == '#') {
		// Comment until end of line.
		do currChar = getchar();
		while (currChar != EOF && currChar != '\n' && currChar != '\r');

		if (currChar != EOF)
			return gettoken();
	}
	// Check for end of file.  Don't eat the EOF.
	if (currChar == EOF) return tok_eof;

	// Otherwise, just return the character as its ascii value.
	int x = currChar;
	currChar = getchar();
	return x;
}