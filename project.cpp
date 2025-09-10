#include <iostream>
#include <string>
using namespace std;

enum class TokenType
{
    NUMBER,
    PLUS,
    MINUS,
    MUL,
    DIV,
    END_OF_FILE
};

struct Token
{
    TokenType type;
    string value;
};

class Lexer
{
private:
    string src;
    int pos;

public:
    Lexer(const string &source) : src(source), pos(0) {}
    Token getNextToken()
    {
        return {TokenType::END_OF_FILE, ""};
    }
};

class AST
{
public:
    virtual ~AST() = default;
};

class NumberNode : public AST
{
public:
    int value;
    NumberNode(int v) : value(v) {}
};

class Parser
{
private:
    Lexer &lexer;
    Token current;

public:
    Parser(Lexer &lex) : lexer(lex)
    {
        current = lexer.getNextToken();
    }
    AST *parse()
    {
        return nullptr;
    }
};

class Interpreter
{
public:
    int visit(AST *node)
    {
        if (node == nullptr)
            return 0;
        if (auto num = dynamic_cast<NumberNode *>(node))
        {
            return num->value;
        }
        return 0;
    }
};
int main()
{
    string s;
    cout << "Enter an expression";
    getline(cin, s);
    Lexer lexer(s);
    Parser parser(lexer);
    AST *tree = parser.parse();

    Interpreter interp;
    int result = interp.visit(tree);

    return 0;
}