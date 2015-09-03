#include "help.h"
#include "token.h"
#include <iostream>
#include <string>

using namespace std;

constexpr size_t DEFAULT_BEGIN_NUMBER = 1;

const string USAGE = 
"nxtfile�͊����̃t�@�C���Əd�����Ȃ��悤�Ȕԍ��t���t�@�C���������܂��B\n"
"�g����: ./nxtfile pattern [beginNumber=1]\n"
"  pattern�ɂ͍�肽���t�@�C�����̃p�^�[�����w�肵�܂��B\n"
"  �Ⴆ��'../log/a_{y}{m}{d}_{n3}.log'�̂悤�ɏ����܂��B\n"
"  �p�^�[���ɂ͕ϐ��𖄂ߍ��ނ��Ƃ��ł��܂��B\n"
"  ������'{<�ϐ��̎��><����(�ȗ���)>}'�ł��B\n"
"  �p�^�[���Ɋ܂܂�邷�ׂĂ̕ϐ��͓K�؂ȕ�����ɒu�������܂��B\n"
"  �l�͉E�񂹂��A�󂢂�����'0'�Ŗ��߂܂��B\n"
"  �Ⴆ��'{n3}'�Ŕԍ���12�Ȃ�A�������'012'�ɂȂ�܂��B\n"
"  '{'���̂��܂߂����ꍇ��'{{'�Ɠ���ׂď����܂��B\n"
"  �p�^�[���ɂ͔ԍ��ϐ���������܂܂�Ă��Ȃ���΂Ȃ�܂���B\n"
"�t�@�C�����̏d���������\n"
"  beginNumber����1���ԍ��𑝂₵�Ȃ���t�@�C���������܂��B\n"
"  �������O�̃t�@�C�������łɑ��݂��Ă���Ύ��̔ԍ��ɐi�݂܂��B\n"
"  �����̃t�@�C���Əd�����Ȃ��ŏ��̃t�@�C�������o�͂��܂��B\n"
"�ϐ��̎�ނ̈ꗗ\n"
"  n �d�����Ȃ��ŏ��̔ԍ��B�������ȗ��Ȃ�3�BbeginNumber...\n"
"  y ���݂̔N�B�������ȗ��Ȃ�4�B2015...\n"
"  m ���݂̌��B�������ȗ��Ȃ�2�B1..12\n"
"  d ���݂̓��B�������ȗ��Ȃ�2�B1..31\n"
"  h ���݂̎��B�������ȗ��Ȃ�2�B0..23\n"
"  M ���݂̕��B�������ȗ��Ȃ�2�B0..59\n"
"  s ���݂̕b�B�������ȗ��Ȃ�2�B0..59\n"
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
                    throw describe(__FILE__, "(", __LINE__, "): " , "�ԍ��ϐ����ȏ�w�肵�Ă͂����܂���B");
                numberToken = nt;
            }
        }
        if (!numberToken) 
            throw describe(__FILE__, "(", __LINE__, "): " , "�ԍ��ϐ���������܂���B");
        for (size_t n = beginNumber;; n++) {
            numberToken->setNumber(n);
            string file = "";
            for (auto token : (*tokens)) {
                file += token->toString();
            }
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
