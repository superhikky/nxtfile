#ifndef HELP_H
#define HELP_H

#include <cstdlib>
#include <ctime>
#include <memory>
#include <sstream>
#include <string>
#include <sys/stat.h>

using namespace std;

template <typename Type, typename ...Arguments> 
shared_ptr<Type> newInstance(Arguments&&... arguments) {
    return shared_ptr<Type>(new Type(static_cast<Arguments&&>(arguments)...));
}

inline void describeTo(std::stringstream &ss) {}

template <typename Type, typename ...Arguments> 
void describeTo(stringstream &ss, const Type &value, Arguments&&... arguments) {
    ss << value;
    describeTo(ss, arguments...);
}

template <typename ...Arguments> 
string describe(Arguments&&... arguments) {
    stringstream ss;
    describeTo(ss, arguments...);
    return ss.str();
}

inline bool fileExist(const string &name) {
    struct stat buffer;
    return stat(name.c_str(), &buffer) == 0;
}

inline unsigned long s2ul(const string &s) {
    char *end;
    unsigned long ul = strtoul(s.c_str(), &end, 0);
    if (s.empty() || *end) 
        throw describe(__FILE__, "(", __LINE__, "): " , "'", s , "'‚ð•„†–³‚µ®”‚É•ÏŠ·‚Å‚«‚Ü‚¹‚ñB");
    return ul;
}

class Date {
protected:
    struct tm *localTime;
public:
    Date() {
        time_t time_ = time(NULL);
        this->localTime = localtime(&time_);
    }
    
    size_t getYear() const 
        { return this->localTime->tm_year + 1900; }
    size_t getMonth() const 
        { return this->localTime->tm_mon + 1; }
    size_t getDay() const 
        { return this->localTime->tm_mday; }
    size_t getHour() const 
        { return this->localTime->tm_hour; }
    size_t getMinute() const 
        { return this->localTime->tm_min; }
    size_t getSecond() const 
        { return this->localTime->tm_sec; }
};

#endif
