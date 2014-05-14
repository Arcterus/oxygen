#include "parser.cpp";
int main()
{
		//standard binary operators.
		precedence['<'] = 1;
		precedence['+'] = 2;
		precedence['-'] = 2;
		precedence['*'] = 4;  

		//find first token
		fprintf(stdout, "oxygen> ");
		getNextToken();
		run();
		return 0;
}