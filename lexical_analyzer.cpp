#include "lexical_analyzer.h"
#include "structeres.h"
#include <fstream>

void PrintHeaderOfTable() {
	std::cout << " -------------------------------\n";
	std::cout << " |" << std::setw(4) << "Line" << "|" << std::setw(7) << "Column" << "|" <<
		std::setw(5) << "Id" << "|" << std::setw(10) << "Lexem" << "|\n";
	std::cout << " -------------------------------\n";
}

void PrintTableOfLexems() {
	for (auto& it : lexems) {
		it.printLexem();
	}
}

void PrintErrors() {
	for (auto& error : errors) {
		std::cout << error << '\n';
	}
}

int lexer_analize(std::ifstream& file) {
	char c;
	int ascii_symbol;
	int ln = 1; // Line
	int col{}; // Column
	bool is_next_read = false;
	bool error_was = false;
	Lexem token;



	while (!file.eof()) {

		if (is_next_read == true) {
			is_next_read = false;
		} else {
			file.get(c);
			++col;
		}

		if (file.eof()) {
			break;
		}

		ascii_symbol = c;
		switch (ascii[ascii_symbol]) {
		case 0: // пробільні символи
			while (!file.eof() && ascii[ascii_symbol] == 0) {
				if (c == '\n') {
					++ln;
					col = 0;
				} else if (c == '\t') {
					col = col - (col - 1) % 4 + 4;
				}

				file.get(c);
				++col;
				ascii_symbol = c;
			}
			is_next_read = true;
			break;


		case 1: // константа
			error_was = false;

			token.column = col;
			token.line = ln;
			token.lexem += c;

			file.get(c);
			++col;
			ascii_symbol = c;
			while (ascii[ascii_symbol] == 1) {
				token.lexem += c;
				file.get(c);
				ascii_symbol = c;
				++col;
			}

			if (ascii[ascii_symbol] != 0 && c != ';' && c != '=' && c != '.') {
				error_was = true;
				errors.push_back("Lexer: Error (line " + std::to_string(ln) + " column " + std::to_string(token.column) + ") incorrect constant ");
				while (ascii[ascii_symbol] != 0 && c != ';' && c != '=') {
					token.lexem += c;
					file.get(c);
					ascii_symbol = c;
					++col;
				}
				errors.back() += token.lexem;
			}

			if (!error_was) {
				if (auto it = constants.find(token.lexem); it == constants.end()) {
					constants.insert({ token.lexem, Lexem::const_count });
					token.id = Lexem::const_count;
					++Lexem::const_count;
				} else {
					token.id = it->second;
				}
				lexems.push_back(token);
			}

			token.lexem.clear();
			is_next_read = true;
			break;

		case 2: // ідентифікатори та ключові слова
			error_was = false;
			token.column = col;
			token.line = ln;
			token.lexem += c;

			file.get(c);
			++col;
			ascii_symbol = c;

			while (true) {
				if (ascii[ascii_symbol] == 2 || ascii[ascii_symbol] == 1) {
					token.lexem += c;
				} else if (ascii[ascii_symbol] == 6) {
					error_was = true;
					errors.push_back("Lexer: Error (line " + std::to_string(ln) + " column " + std::to_string(token.column) + ") incorrect lexem ");

					// починаємо збереження помилкової частини:
					token.lexem += c;
					file.get(c);
					++col;
					ascii_symbol = c;

					while (c != ':' && c != '=' && c != ';' && ascii[ascii_symbol] != 0 && !file.eof()) {
						token.lexem += c;
						file.get(c);
						++col;
						ascii_symbol = c;
					}

					errors.back() += token.lexem;
					break;
				} else {
					break;
				}

				file.get(c);
				++col;
				ascii_symbol = c;

				if (file.eof())
					break;
			}

			if (!error_was) {
				if (auto it = keywords.find(token.lexem); it == keywords.end()) {
					if (auto it = identifiers.find(token.lexem); it == identifiers.end()) {
						identifiers.insert({ token.lexem, Lexem::id_count });
						token.id = Lexem::id_count;
						++Lexem::id_count;
					} else {
						token.id = identifiers[token.lexem];
					}
				} else {
					token.id = keywords[token.lexem];
				}
				lexems.push_back(token);
			}

			is_next_read = true;
			token.lexem.clear();
			break;

		case 3: // Односимвольні роздільники
			token.column = col;
			token.line = ln;
			token.lexem += c;
			token.id = (*separators.find(token.lexem)).second;
			lexems.push_back(token);
			token.lexem.clear();
			break;

		case 4: // Багатосимвольні роздільники
			break;

		case 5: // Коментарі
			file.get(c);
			ascii_symbol = c;
			++col;

			if (!file.eof()) {
				if (c == '*') {
					bool isFindStar = false;
					bool isEOC = false;
					bool isEOF = false;

					while (true) {
						file.get(c);
						ascii_symbol = c;

						if (file.eof()) {
							isEOF = true;
							break;
						}

						if (c == '\n') {
							++ln;
							col = 1;
							continue;
						} else if (c == '\t') {
							col = col - (col - 1) % 4 + 4;
						} else {
							++col;
						}

						if (c == '*') {
							isFindStar = true;
						} else if (c == ')' && isFindStar) {
							isEOC = true;
							break;
						} else {
							isFindStar = false;
						}
					}

					if (isEOC) {} else if (isEOF) {
						errors.push_back("Lexer: Error (line " + std::to_string(ln) + ", column " + std::to_string(col) + "): unexpected EOF while reading comment");
					} else {
						errors.push_back("Lexer: Error (line " + std::to_string(ln) + ", column " + std::to_string(col) + "): unterminated comment");
					}
				} else {
					errors.push_back("Lexer: Error (line " + std::to_string(ln) + ", column " + std::to_string(col) + "): expected '*' after '(' to start comment");
				}
			} else {
				errors.push_back("Lexer: Error (line " + std::to_string(ln) + ", column " + std::to_string(col) + "): unexpected EOF while reading comment");
			}

			break;

		case 6: // Недопустимі символи
			token.column = col;
			token.line = ln;
			token.lexem += c;

			errors.push_back("Lexer: Error (line " + std::to_string(ln) + ", column " + std::to_string(col) + "): illegal symbol '" + token.lexem + "'");

			token.lexem.clear();
			break;
		}
	}

	PrintHeaderOfTable();
	PrintTableOfLexems();
	PrintErrors();

	return 0;
}