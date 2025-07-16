#pragma once
#include <map>
#include <unordered_map>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>

extern int ascii[];
extern std::unordered_map<std::string, int> keywords;
extern std::unordered_map<std::string, int> separators;
extern std::map<std::string, int> identifiers;
extern std::map<std::string, int> constants;
extern std::vector<std::string> errors;

struct Lexem {
	int line = 1;
	int column{};
	int id{};
	std::string lexem;

	static int const_count;
	static int id_count;

	void printLexem() const;
};

extern std::vector<Lexem> lexems;
