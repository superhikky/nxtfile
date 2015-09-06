#include "help.h"
#include "token.h"
#include <iostream>
#include <string>

using namespace std;

constexpr size_t DEFAULT_BEGIN_NUMBER = 1;

const string USAGE = 
"nxtfileは既存のファイルと重複しないような番号付きファイル名を作ります。\n"
"使い方: ./nxtfile pattern [beginNumber=1]\n"
"  patternには作りたいファイル名のパターンを指定します。\n"
"  例えば'../log/a_{y}{m}{d}_{n3}.log'のように書きます。\n"
"  パターンには変数を埋め込むことができます。\n"
"  書式は'{<変数の種類><桁数(省略可)>}'です。\n"
"  パターンに含まれるすべての変数は適切な文字列に置き換えます。\n"
"  値は右寄せし、空いた桁は'0'で埋めます。\n"
"  例えば'{n3}'で番号が12なら、文字列は'012'になります。\n"
"  '{'自体を含めたい場合は'{{'と二つ並べて書きます。\n"
"  パターンには番号変数が一つだけ含まれていなければなりません。\n"
"ファイル名の重複を避ける\n"
"  beginNumberから1ずつ番号を増やしながらファイル名を作ります。\n"
"  同じ名前のファイルがすでに存在していれば次の番号に進みます。\n"
"  既存のファイルと重複しない最初のファイル名を出力します。\n"
"変数の種類の一覧\n"
"  n 重複しない最初の番号。桁数を省略なら" DEFAULT_NUMBER_WIDTH "。beginNumber...\n"
"  y 現在の年。桁数を省略なら" DEFAULT_YEAR_WIDTH "。2015...\n"
"  m 現在の月。桁数を省略なら" DEFAULT_MONTH_WIDTH "。1..12\n"
"  d 現在の日。桁数を省略なら" DEFAULT_DAY_WIDTH "。1..31\n"
"  h 現在の時。桁数を省略なら" DEFAULT_MINUTE_WIDTH "。0..23\n"
"  M 現在の分。桁数を省略なら" DEFAULT_MINUTE_WIDTH "。0..59\n"
"  s 現在の秒。桁数を省略なら" DEFAULT_SECOND_WIDTH "。0..59\n"
;

int main(int argc, char **argv) {
    int result = 0;
    try {
        if (argc < 2 || 
            string(argv[1]) == "-h" || 
            string(argv[1]) == "/?") 
            throw USAGE;
        string pattern = argv[1];
        size_t beginNumber = DEFAULT_BEGIN_NUMBER;
        if (argc >= 3) 
            beginNumber = s2ul(argv[2]);
        auto tokens = tokenize(pattern, beginNumber);
        NumberToken *numberToken = nullptr;
        for (auto token : (*tokens)) {
            if (auto *nt = dynamic_cast<NumberToken *>(token.get())) {
                if (numberToken) 
                    throw describe(__FILE__, "(", __LINE__, "): " , "番号変数を二つ以上指定してはいけません。");
                numberToken = nt;
            }
        }
        if (!numberToken) 
            throw describe(__FILE__, "(", __LINE__, "): " , "番号変数が見つかりません。");
        for (size_t n = beginNumber;; n++) {
            numberToken->setNumber(n);
            string file = "";
            for (auto token : (*tokens)) 
                file += token->toString();
            if (!fileExist(file)) {
                cout << file;
                break;
            }
        }
    } catch (const string &message) {
        cerr << message << endl;
        result = 1;
    }
    return result;
}
