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
    void setWidth(const size_t &width) 
        { this->width = width; }
    
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

inline shared_ptr<VariableToken> makeNumberToken(const Date &now) {
    return newInstance<NumberToken>(3);
}

inline shared_ptr<VariableToken> makeYearToken(const Date &now) {
    return newInstance<DateToken>(4, now.getYear());
}

inline shared_ptr<VariableToken> makeMonthToken(const Date &now) {
    return newInstance<DateToken>(2, now.getMonth());
}

inline shared_ptr<VariableToken> makeDayToken(const Date &now) {
    return newInstance<DateToken>(2, now.getDay());
}

inline shared_ptr<VariableToken> makeHourToken(const Date &now) {
    return newInstance<DateToken>(2, now.getHour());
}

inline shared_ptr<VariableToken> makeMinuteToken(const Date &now) {
    return newInstance<DateToken>(2, now.getMinute());
}

inline shared_ptr<VariableToken> makeSecondToken(const Date &now) {
    return newInstance<DateToken>(2, now.getSecond());
}

using MakeTokenProc = function<shared_ptr<VariableToken>(Date)>;

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
                throw describe(__FILE__, "(", __LINE__, "): " , "'}'��������܂���B");
            if (closePos == pos) 
                throw describe(__FILE__, "(", __LINE__, "): " , "�ϐ����`���Ă��������B");
            int type = pattern.at(pos);
            if (getMakeTokenProcs()->count(type) == 0) 
                throw describe(__FILE__, "(", __LINE__, "): " , "'", type, "'�Ƃ�����ނ̕ϐ��͂���܂���B");
            pos++;
            auto t = getMakeTokenProcs()->at(type)(now);
            if (pos < closePos) 
                t->setWidth(s2ul(pattern.substr(pos, closePos - pos)));
            tokens->push_back(t);
            pos = closePos + 1;
        }
    }
    appendLiteralIfNotEmpty();
    return tokens;
}

#endif
