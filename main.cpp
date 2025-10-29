#include <iostream>
#include <string>
#include <stdexcept>
#include <map>
using namespace std;

enum class TokenType
{
    number,
    plus,
    minus,
    mul,
    div,
    mod,
    assign,
    identifier,
    lparen,
    rparen,
    end_of_file
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
        while (pos < src.size())
        {
            char current = src[pos];
            if (isspace(current))
            {
                pos++;
                continue;
            }

            if (isalpha(current))
            {
                string id;
                while (pos < src.size() && isalnum(src[pos]))
                    id.push_back(src[pos++]);
                return {TokenType::identifier, id};
            }

            if (isdigit(current) || current == '.')
            {
                string num;
                bool hasDot = false;
                while (pos < src.size() && (isdigit(src[pos]) || src[pos] == '.'))
                {
                    if (src[pos] == '.')
                    {
                        if (hasDot)
                            break;
                        hasDot = true;
                    }
                    num.push_back(src[pos++]);
                }
                return {TokenType::number, num};
            }

            pos++;
            switch (current)
            {
            case '+':
                return {TokenType::plus, "+"};
            case '-':
                return {TokenType::minus, "-"};
            case '*':
                return {TokenType::mul, "*"};
            case '/':
                return {TokenType::div, "/"};
            case '%':
                return {TokenType::mod, "%"};
            case '(':
                return {TokenType::lparen, "("};
            case ')':
                return {TokenType::rparen, ")"};
            case '=':
                return {TokenType::assign, "="};
            default:
                cout << "Unexpected character: " << current << "\n";
            }
        }
        return {TokenType::end_of_file, ""};
    }
};

enum class NodeType
{
    number,
    binop,
    assign,
    variable
};

class BaseNode
{
protected:
    NodeType nodetype;

public:
    BaseNode(NodeType type) : nodetype(type) {}
    virtual ~BaseNode() = default;
    NodeType getType() const { return nodetype; }
};

class NumNode : public BaseNode
{
    double value;

public:
    NumNode(double v) : BaseNode(NodeType::number), value(v) {}
    double getValue() const { return value; }
};

class VarNode : public BaseNode
{
    string name;

public:
    VarNode(const string &n) : BaseNode(NodeType::variable), name(n) {}
    string getName() const { return name; }
};

class BinOpNode : public BaseNode
{
    BaseNode *left;
    Token op;
    BaseNode *right;

public:
    BinOpNode(BaseNode *l, Token o, BaseNode *r): BaseNode(NodeType::binop), left(l), op(o), right(r) {}
    BaseNode *getLeft() const { return left; }
    BaseNode *getRight() const { return right; }
    Token getOp() const { return op; }
    ~BinOpNode() override
    {
        delete left;
        delete right;
    }
};

class AssignNode : public BaseNode
{
    string varName;
    BaseNode *expr;

public:
    AssignNode(const string &n, BaseNode *e): BaseNode(NodeType::assign), varName(n), expr(e) {}
    string getVarName() const { return varName; }
    BaseNode *getExpr() const { return expr; }
    ~AssignNode() override { delete expr; }
};

class Parser
{
private:
    Lexer &lexer;
    Token token;

    void eat(TokenType expected)
    {
        if (token.type == expected)
            token = lexer.getNextToken();
        else
            throw runtime_error("Unexpected token");
    }

    BaseNode *factor()
    {
        if (token.type == TokenType::number)
        {
            double val = stod(token.value);
            eat(TokenType::number);
            return new NumNode(val);
        }
        else if (token.type == TokenType::identifier)
        {
            string name = token.value;
            eat(TokenType::identifier);
            return new VarNode(name);
        }
        else if (token.type == TokenType::lparen)
        {
            eat(TokenType::lparen);
            BaseNode *node = expr();
            eat(TokenType::rparen);
            return node;
        }
        throw runtime_error("Invalid factor");
    }

    BaseNode *term()
    {
        BaseNode *node = factor();
        while (token.type == TokenType::mul || token.type == TokenType::div || token.type == TokenType::mod)
        {
            Token oper = token;
            if (oper.type == TokenType::mul)
                eat(TokenType::mul);
            else if (oper.type == TokenType::div)
                eat(TokenType::div);
            else
                eat(TokenType::mod);
            node = new BinOpNode(node, oper, factor());
        }
        return node;
    }

    BaseNode *expr()
    {
        BaseNode *node = term();
        while (token.type == TokenType::plus || token.type == TokenType::minus)
        {
            Token oper = token;
            if (oper.type == TokenType::plus)
                eat(TokenType::plus);
            else
                eat(TokenType::minus);
            node = new BinOpNode(node, oper, term());
        }
        return node;
    }

public:
    Parser(Lexer &lex) : lexer(lex) { token = lexer.getNextToken(); }

    BaseNode *statement()
    {
        if (token.type == TokenType::identifier)
        {
            string varName = token.value;
            eat(TokenType::identifier);
            if (token.type == TokenType::assign)
            {
                eat(TokenType::assign);
                BaseNode *rhs = expr();
                return new AssignNode(varName, rhs);
            }
            throw runtime_error("Expected '=' after variable");
        }
        return expr();
    }

    BaseNode *parse() { return statement(); }
};
class Value {
public:
    double val;
    Value(double v = 0) : val(v) {}

    Value operator+(const Value& other) const { return Value(val + other.val); }
    Value operator-(const Value& other) const { return Value(val - other.val); }
    Value operator*(const Value& other) const { return Value(val * other.val); }
    Value operator/(const Value& other) const {
        if (other.val == 0) throw runtime_error("Division by zero");
        return Value(val / other.val);
    }

    Value mod(const Value& other) const {
        if (other.val == 0) throw runtime_error("Modulo by zero");
        if (fabs(val - round(val)) > 1e-9 || fabs(other.val - round(other.val)) > 1e-9)
            throw runtime_error("Modulus only supported for integers");
        return Value(fmod(val, other.val));
    }

    void display() const {
        if (fabs(val - round(val)) < 1e-9)
            cout << (int)round(val);
        else
            cout << val;
    }
};

class Interpreter {
public:
    Value visit(BaseNode* node) {
        if (node->getType() == NodeType::number) {
            NumNode* n = static_cast<NumNode*>(node);
            return Value(n->getValue());
        } else if (node->getType() == NodeType::binop) {
            BinOpNode* b = static_cast<BinOpNode*>(node);
            Value left = visit(b->getLeft());
            Value right = visit(b->getRight());
            switch (b->getOp().type) {
                case TokenType::plus:  return left + right;
                case TokenType::minus: return left - right;
                case TokenType::mul:   return left * right;
                case TokenType::div:   return left / right;
                case TokenType::mod:   return left.mod(right);
                default: return Value(0);
            }
        }
        return Value(0);
    }
};

int main() {
    string s;
    cout << "Enter expression: ";
    getline(cin, s);

    Lexer lexer(s);
    Parser parser(lexer);
    BaseNode* tree = parser.parse();

    Interpreter interp;
    try {
        Value result = interp.visit(tree);
        cout << "Result = ";
        result.display();
        cout << endl;
    } catch (exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    delete tree;
    return 0;
}
