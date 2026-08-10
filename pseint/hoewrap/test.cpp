#include <iostream>
#include "hoewrap.h"

int main() {
	std::string input = 
		"# Example\n"
		"\n"
		"*  **Hello** *World*\n"
		"![this should be an image](foo.png)\n"
		"2^N\n"
		;
	std::cout << markdown2html(input.c_str(),input.size()) << std::endl;
}
