#include <cctype>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

namespace {
	//AST for all expression nodes
	class ExprAST {
	public:
		//only needs a destructor; other methods will be supplied
		//by inherited ASTs
		virtual ~ExprAST() {}
	};

	//AST for numeric values
	class NumberExprAST : public ExprAST {
	public:
		NumberExprAST(double val) {}
	};

	//AST for variables
	class VariableExprAST : public ExprAST {
		std::string name;
	public:
		VariableExprAST(const std::string &n) : name(n) {}
	};

	//AST for a binary operator (e.g. +, -)
	class BinaryExprAST : public ExprAST {
	public:
		BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs) {}
	};

	//Represents a function prototype (e.g. int prototype(int a, int b) in c++)
	class PrototypeAST {
		std::string name;
		std::vector<std::string> arguments;
	public:
		PrototypeAST(const std::string &n, const std::vector<std::string> &args)
			: name(n), arguments(args) {}

	};

	//Represents an actual function
	class FunctionAST {
	public:
		//needs a prototype as well as a body
		FunctionAST(PrototypeAST *proto, ExprAST *body) {}
	};

	//AST for function calls
	class CallExprAST : public ExprAST {
		std::string callee;
		std::vector<ExprAST*> arguments;
	public:
		CallExprAST(const std::string &c, std::vector<ExprAST*> &args)
			: callee(c), arguments(args) {}
	};
}