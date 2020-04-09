// Good morning! Here's your coding interview problem for today.
// This problem was asked by Facebook.
// Given a string consisting of parentheses, single digits, and positive and
// negative signs, convert the string into a mathematical expression to obtain
// the answer. Don't use eval or a similar built-in parser. For example, given
// '-1 + (2 + 3)', you should return 4.

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

enum class TokType { Number, Op };
struct Tok {
  TokType type;
  unsigned value;
  char op = 0;

  bool is_op(char op) const { return type == TokType::Op && this->op == op; }
};

string to_string(Tok t) {
  stringstream ss;
  ss << "Tok(" << (t.type == TokType::Number ? "Number" : "Op") << ", ";
  if (t.type == TokType::Number) {
    ss << t.value;
  } else {
    ss << t.op;
  }
  ss << ")";
  return ss.str();
}

bool is_digit(char c) { return c >= '0' && c <= '9'; }

struct TokenizerError : public std::runtime_error {
  TokenizerError(const string what) : std::runtime_error(what.c_str()) {}
};
struct ParseError : public std::runtime_error {
  ParseError(const string what) : std::runtime_error(what.c_str()) {}
};

vector<Tok> tokenize(string exp) {
  vector<Tok> results;
  for (size_t i = 0; i < exp.size(); ++i) {
    if (is_digit(exp[i])) {
      unsigned number = 0;
      size_t k;
      for (k = i; k < exp.size() && is_digit(exp[k]); ++k) {
        number = number * 10 + (exp[k] - '0');
      }
      results.emplace_back(Tok{TokType::Number, number, -1});
      if (k < exp.size()) {
        // we encountered op
        i = k - 1;
      } else {
        i = k;
      }
    } else if (exp[i] == '-' || exp[i] == ')' || exp[i] == '(' ||
               exp[i] == '+' || exp[i] == '*' || exp[i] == '/') {
      results.emplace_back(Tok{TokType::Op, (unsigned)-1, exp[i]});
    } else if (exp[i] == ' ') {
      // just skip
    } else {
      throw TokenizerError("Syntax error at: " + to_string(i));
    }
  }
  return results;
}

struct TokenStream {
  TokenStream(vector<Tok> toks)
      : toks(move(toks)), curr_it(this->toks.begin()) {}
  vector<Tok> toks;
  vector<Tok>::const_iterator curr_it;
  TokenStream &next() {
    assert(this->curr_it != this->toks.end());
    this->curr_it++;
    return *this;
  }
  const bool end() const { return this->curr_it == this->toks.end(); }
  const Tok &curr() const { return *this->curr_it; }
};

// Grammar:
// EXP: TERM [[+-] TERM]*;
// TERM: NUMBER [[*/] NUMBER];
// NUMBER: [0-9]+ | '(' EXP ')'
int exp(TokenStream &toks);
int term(TokenStream &toks);
int number(TokenStream &toks);

int exp(TokenStream &toks) {
  int result = term(toks);
  while (!toks.end()) {
    if (toks.curr().type == TokType::Op) {
      if (toks.curr().op == '+') {
        result = result + term(toks.next());
      } else if (toks.curr().op == '-') {
        result = result - term(toks.next());
      } else {
        break;
      }
    }
  }
  return result;
}

int term(TokenStream &toks) {
  // term is number of multiplication/division of numbers.
  int left_num = number(toks);
  while (toks.curr().type == TokType::Op) {
    if (toks.curr().op == '*') {
      left_num = left_num * number(toks.next());
    } else if (toks.curr().op == '/') {
      const int rhs = number(toks.next());
      if (rhs == 0) {
        throw ParseError("Div by zero");
      }
      left_num = left_num / rhs;
    } else {
      break;
    }
  }
  return left_num;
}

// todo: number is not number anymore but it is factor now.
// and factor has following grammar:
// number || '(' exp ')'
int number(TokenStream &toks) {
  if (toks.curr().is_op('(')) {
    auto val = exp(toks.next());
    if (!toks.curr().is_op(')')) {
      throw ParseError("Expected closing bracket, got some shit: " +
                       to_string(toks.curr()));
    }
    toks.next(); // consume token
    return val;
  }
  int sign = 1;
  while (toks.curr().type == TokType::Op) {
    if (toks.curr().op == '-') {
      sign = sign * -1;
    } else if (toks.curr().op == '+') {
      // do nothing
    } else {
      throw ParseError("Unexpected operator before number");
    }
    toks.next();
  }
  if (toks.curr().type != TokType::Number) {
    throw ParseError("Expectd number");
  }
  const int v = toks.curr().value;
  toks.next();
  return sign * v;
}

int evaluate(vector<Tok> toks) {
  TokenStream stream(toks);
  return exp(stream);
}

void play() {
  while (true) {
    string line;
    cin >> line;
    try {
      for (auto t : tokenize(line)) {
        cout << to_string(t) << "\n";
      }
    } catch (TokenizerError &e) {
      cerr << "ERROR: Tokenize: " << e.what() << "\n";
    } catch (ParseError &e) {
      cerr << "ERROR: Parse: " << e.what() << "\n";
    }
  }
}

void tests() {
  auto test = [](string exp, int expected) {
    try {
      auto actual = evaluate(tokenize(exp));
      if (actual != expected) {
        cout << "FAILED: " << exp << " (Expeted: " << expected
             << ", Actual: " << actual << ")\n";
      } else {
        cout << "Passed: " << exp << "\n";
      }
    } catch (ParseError &e) {
      cout << "FAILED: " << exp << " (Expeted: " << expected
           << ", Actual: Error: " << e.what() << ")\n";
    } catch (TokenizerError &e) {
      cout << "FAILED: " << exp << " (Expeted: " << expected
           << ", Actual: Error: " << e.what() << ")\n";
    }
  };

  auto test_err = [](string exp, string err_expected) {
    try {
      auto actual = evaluate(tokenize(exp));
      cout << "FAILED: " << exp << " (Expected error, got result: " << actual
           << ")\n";
    } catch (std::exception &e) {
      if (e.what() != err_expected) {
        cout << "FAILED: " << exp << " (Expected error '" << err_expected
             << "' instead got: '" << e.what() << "')\n";
      } else {
        cout << "Passed: " << exp << "\n";
      }
    }
  };

  test("1", 1);
  test("-1", -1);
  test("--1", 1);
  test("-+1", -1);
  test("0", 0);
  test("1 + 2", 3);
  test("1+2", 3);
  test("2 - 1", 1);
  test("1 + 2 + 3", 6);
  test("1 + 2 + 3 * 4 - 1", 14);
  test("1 + 2 + -3 * 4 / 2 - 1", -4);
  test("3 * (2 + 1)", 9);
  test("3 * (2 + 1*-1)", 3);
  test("3 * (2 * (1 + 4))", 30);
  test_err("2 / 0", "Div by zero");
  test("-1 + (2 + 3)", 4);
}

int main() { tests(); }
