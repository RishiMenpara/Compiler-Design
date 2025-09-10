#include <iostream>
#include <string>
using namespace std;

enum class TokenType {
    NUMBER, PLUS, MINUS, MUL, DIV,END_OF_FILE
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
    Lexer(const string &source) : src(source), pos(0) {}
    Token getNextToken() {
        return {TokenType::END_OF_FILE, ""};
    }
};