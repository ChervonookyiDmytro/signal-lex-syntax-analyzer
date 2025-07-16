#include "structeres.h"
#include "lexical_analyzer.h"
#include "syntax_analyzer.h"
#include <fstream>

static void test(std::string&& file_name) {
    std::ifstream file(file_name);
    if (!file) {
        std::cerr << "File \"" << file_name << "\" was not opened!";
        return;
    }
    std::cout << "\n\n-------- " << file_name << " --------\n";
    lexer_analize(file);

    std::cout << "\n\n";
    analyze_syntax();
    std::cout << '\n';
    for (auto& error : errors) {
        std::cout << error << '\n';
    }

    file.close();
    errors.clear();
    identifiers.clear();
    constants.clear();
    lexems.clear();
    Lexem::const_count = 501;
    Lexem::id_count = 1001;
    i = 0;
}


int main() {
    test("true_test_1.txt");
    test("true_test_2.txt");
    test("true_test_3.txt");
    test("true_test_4.txt");
    test("true_test_5.txt");

    test("false_test_1.txt");
    test("false_test_2.txt");
    test("false_test_3.txt");
    test("false_test_4.txt");
    test("false_test_5.txt");
}

