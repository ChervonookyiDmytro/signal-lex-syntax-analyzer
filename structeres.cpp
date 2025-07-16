#include "structeres.h"

// 0 - пробіли/пропуск
// 1 - цифри
// 2 - літерaи
// 3 - односимвольні роздільники
// 5 - дужки/початок коментаря
// 6 - недопустимі

int ascii[] = {
6,6,6,6,6,6,6,6, // 0 - 7
0,0,0,0,0,0, // 8 - 13
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6, // 14 - 31 
0, // 32
6,6, // 33 - 34
6, // 35 
6,6,6,6, // 36 - 39 
5,6, // 40 - 41
6, // 42 (*)
6, // 43 
6, // 44
6, // 45
3, // 46 
6, // 47
1,1,1,1,1,1,1,1,1,1, // 48 - 57
3, // 58
3, // 59
6, // 60
3, // 61 (=)
6,6,6, // 62 - 64
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 65 - 90
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6 // 91 - 127
};

std::unordered_map<std::string, int> keywords = { {"PROGRAM", 401}, {"BEGIN", 402}, {"END", 403}, {"VAR", 404}, {"INTEGER", 405}, {"FLOAT", 406}, {"ENDIF", 407}, {"IF", 408}, {"THEN", 409}, {"ELSE", 410} };
std::unordered_map<std::string, int> separators = { {".", 46}, {":", 58}, {";",59}, {"=",61} };

std::map<std::string, int> identifiers;
std::map<std::string, int> constants;
std::vector<std::string> errors;


void Lexem::printLexem() const{
	std::cout << " |" << std::setw(4) << line << "|" << std::setw(7) << column <<
		"|" << std::setw(5) << id << "|" << std::setw(10) << lexem << "|" << std::endl;
}

int Lexem::const_count = 501;
int Lexem::id_count = 1001;

std::vector<Lexem> lexems;
