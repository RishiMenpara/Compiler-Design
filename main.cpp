#include <iostream>
#include <string>
using namespace std;

enum class TokenType {
    NUMBER, PLUS, MINUS, MUL, DIV, END_OF_FILE
};

struct Token {
    TokenType type;
    string value;
};

class Lexer {
private:
    string src;
    int pos;
public:
    Lexer(const string& source) : src(source), pos(0) {}

    string getSource() const { return src; }

    Token getNextToken() {
        while (pos < (int)src.size()) {
            char current = src[pos];
            if (isspace(current)) { pos++; continue; }

            if (isdigit(current)) {
                string num;
                while (pos < (int)src.size() && isdigit(src[pos])) {
                    num.push_back(src[pos++]);
                }
                return {TokenType::NUMBER, num};
            }

            if (current == '+') { pos++; return {TokenType::PLUS, "+"}; }
            if (current == '-') { pos++; return {TokenType::MINUS, "-"}; }
            if (current == '*') { pos++; return {TokenType::MUL, "*"}; }
            if (current == '/') { pos++; return {TokenType::DIV, "/"}; }

            pos++;
        }
        return {TokenType::END_OF_FILE, ""};
void showSource(const Lexer &l)
{
    cout << "Lexer source: " << l.getSource() << endl;
}
class AST {
public:
    virtual ~AST() = default;
};

class NumberNode : public AST {
public:
    int value;
    NumberNode(int v = 0) : value(v) {}
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
        return new NumberNode();
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
    cout << "Enter an expression: ";
    getline(cin, s);

    Lexer lexer(s);
    showSource(lexer);

    Parser parser(lexer);
    AST *tree = parser.parse();

    Interpreter interp;
    int result = interp.visit(tree);
    cout << "Result = " << result << endl;

    delete tree;
    return 0;
}