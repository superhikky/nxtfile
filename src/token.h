#ifndef TOKEN_H
#define TOKEN_H

#include "help.h"
#include <functional>
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define DEFAULT_NUMBER_WIDTH "3"
#define DEFAULT_YEAR_WIDTH   "4"
#define DEFAULT_MONTH_WIDTH  "2"
#define DEFAULT_DAY_WIDTH    "2"
#define DEFAULT_HOUR_WIDTH   "2"
#define DEFAULT_MINUTE_WIDTH "2"
#define DEFAULT_SECOND_WIDTH "2"

inline const map<int, string> *getDefaultWidths() {
    static map<int, string> DEFAULT_WIDTHS = {
        {'n', DEFAULT_NUMBER_WIDTH}, 
        {'y', DEFAULT_YEAR_WIDTH}, 
        {'m', DEFAULT_MONTH_WIDTH}, 
        {'d', DEFAULT_DAY_WIDTH}, 
        {'h', DEFAULT_HOUR_WIDTH}, 
        {'M', DEFAULT_MINUTE_WIDTH}, 
        {'s', DEFAULT_SECOND_WIDTH}, 
    };
    return &DEFAULT_WIDTHS;
};

class Token {
public:
    virtual string toString() = 0;
};

class LiteralToken : public Token {
protected:
    string literal;
public:
    LiteralToken(const string &literal) : literal(literal) {}
    virtual string toString() override 
        { return this->literal; }
};

class VariableToken : public Token {
protected:
    size_t width;
    size_t number;
    
    VariableToken(const size_t &width) : width(width) {}
public:
    virtual string toString() override {
        stringstream ss;
        ss << setw(this->width) << setfill('0') << this->number;
        return ss.str();
    }
};

class NumberToken : public VariableToken {
public:
    NumberToken(const size_t &width) : 
        VariableToken(width) {}
    void setNumber(const size_t &number) 
        { this->number = number; }
};

class DateToken : public VariableToken {
public:
    DateToken(const size_t &width, const size_t &number) : 
        VariableToken(width) 
        { this->number = number; }
};

inline shared_ptr<VariableToken> makeNumberToken(const size_t &width, const Date &now) {
    return newInstance<NumberToken>(width);
}

inline shared_ptr<VariableToken> makeYearToken(const size_t &width, const Date &now) {
    return newInstance<DateToken>(width, now.getYear());
}

inline shared_ptr<VariableToken> makeMonthToken(const size_t &width, const Date &now) {
    return newInstance<DateToken>(width, now.getMonth());
}

inline shared_ptr<VariableToken> makeDayToken(const size_t &width, const Date &now) {
    return newInstance<DateToken>(width, now.getDay());
}

inline shared_ptr<VariableToken> makeHourToken(const size_t &width, const Date &now) {
    return newInstance<DateToken>(width, now.getHour());
}

inline shared_ptr<VariableToken> makeMinuteToken(const size_t &width, const Date &now) {
    return newInstance<DateToken>(width, now.getMinute());
}

inline shared_ptr<VariableToken> makeSecondToken(const size_t &width, const Date &now) {
    return newInstance<DateToken>(width, now.getSecond());
}

using MakeTokenProc = function<shared_ptr<VariableToken>(size_t, Date)>;
inline const map<int, MakeTokenProc> *getMakeTokenProcs() {
    static const map<int, MakeTokenProc> MAKE_TOKEN_PROCS = {
        {'n', &makeNumberToken}, 
        {'y', &makeYearToken}, 
        {'m', &makeMonthToken}, 
        {'d', &makeDayToken}, 
        {'h', &makeHourToken}, 
        {'M', &makeMinuteToken}, 
        {'s', &makeSecondToken}, 
    };
    return &MAKE_TOKEN_PROCS;
}

inline shared_ptr<vector<shared_ptr<Token>>> tokenize(
    const string &pattern, 
    const size_t &beginNumber) 
{
    auto tokens = newInstance<vector<shared_ptr<Token>>>();
    string literal = "";
    auto appendLiteralIfNotEmpty = [&]() {
        if (!literal.empty()) {
            tokens->push_back(newInstance<LiteralToken>(literal));
            literal = "";
        }
    };
    Date now;
    for (size_t pos = 0; pos < pattern.length();) {
        int cur = pattern.at(pos);
        int nxt = '\0';
        if (pos + 1 < pattern.length()) 
            nxt = pattern.at(pos + 1);
        if (cur != '{') {
            literal += cur;
            pos++;
        } else if (nxt == '{') {
            literal += cur;
            pos += 2;
        } else {
            appendLiteralIfNotEmpty();
            pos++;
            size_t closePos = pattern.find_first_of("}", pos);
            if (closePos == string::npos) 
                throw describe(__FILE__, "(", __LINE__, "): " , "'}'が見つかりません。");
            if (closePos == pos) 
                throw describe(__FILE__, "(", __LINE__, "): " , "変数を定義してください。");
            int type = pattern.at(pos);
            if (getMakeTokenProcs()->count(type) == 0) 
                throw describe(__FILE__, "(", __LINE__, "): " , "'", type, "'という種類の変数はありません。");
            pos++;
            string w = getDefaultWidths()->at(type);
            if (pos < closePos) 
                w = pattern.substr(pos, closePos - pos);
            auto t = getMakeTokenProcs()->at(type)(s2ul(w), now);
            tokens->push_back(t);
            pos = closePos + 1;
        }
    }
    appendLiteralIfNotEmpty();
    return tokens;
}

#endif
