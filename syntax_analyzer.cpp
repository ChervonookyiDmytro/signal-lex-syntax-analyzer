#include "structeres.h"
#include <memory>

struct TreeNode {
	std::string label;
	std::vector<std::unique_ptr<TreeNode>> children;

	TreeNode(const std::string& lbl) : label(lbl) {}
};

struct ParseResult {
	std::unique_ptr<TreeNode> node;
	bool error;
};

int i;

std::unique_ptr<TreeNode> SP();
std::unique_ptr<TreeNode> P();
ParseResult BLK();
ParseResult VD();
ParseResult DL();
ParseResult D();
ParseResult SL();
ParseResult S();
ParseResult CS();
ParseResult ICS();
ParseResult CE();
ParseResult E();
ParseResult AP();

static void print_tree(const TreeNode* node, int depth = 0) {
	for (int j = 0; j < depth; ++j) {
		std::cout << ". ";
	}
	std::cout << node->label << "\n";
	for (const auto& child : node->children) {
		if(child.get() != nullptr)
		print_tree(child.get(), depth + 1);
	}
}

static void error_wrapper(const std::string& message) {
	if (i >= lexems.size()) {
		errors.push_back("Parser: Error at end of input: " + message);
	} else {
		errors.push_back("Parser: Error (line " + std::to_string(lexems[i].line) + " column " + std::to_string(lexems[i].column) + "): " + message);
	}
}

void analyze_syntax() {
	print_tree(SP().get());
}

// <signal-program> -- > <program>
std::unique_ptr<TreeNode> SP() {
	auto node = std::make_unique<TreeNode>("<signal-program>");
	node->children.push_back(std::move(P()));

	return node;
}


// <program> --> PROGRAM <procedure-identifier> ; <block>.
std::unique_ptr<TreeNode> P() {
	auto node = std::make_unique<TreeNode>("<program>");
	if (i < lexems.size() && lexems[i].id == 401) { // PROGRAM
		node->children.push_back(std::make_unique<TreeNode>("PROGRAM"));
		++i;
		if (i < lexems.size() && lexems[i].id >= 1001) { // <procedure-identifier>
			auto procedure_identifier_node = std::make_unique<TreeNode>("<procedure-identifier>");
			auto identifier_node = std::make_unique<TreeNode>("<identifier>");
			identifier_node->children.push_back(std::make_unique<TreeNode>(lexems[i].lexem));
			procedure_identifier_node->children.push_back(std::move(identifier_node));
			node->children.push_back(std::move(procedure_identifier_node));

			++i;
			if (i < lexems.size() && lexems[i].id == 59) { // ;
				node->children.push_back(std::make_unique<TreeNode>(";"));
				++i;

				auto node_check = BLK();
				node->children.push_back(std::move(node_check.node));
				if (node_check.error) {
					return node;
				}

				if (i < lexems.size() && lexems[i].id == 46) { // .
					node->children.push_back(std::make_unique<TreeNode>("."));
				} else {
					error_wrapper("separator '.' expected");
				}
			} else {
				error_wrapper("separator';' expected");
			}
		} else {
			error_wrapper("<procedure-identifier> expected");
		}

	} else {
		error_wrapper("keyword 'PROGRAM' expected");
	}

	return node;
}

// <block> --> <variable-declarations> BEGIN <statements - list> END
ParseResult BLK() {
	auto node = std::make_unique<TreeNode>("<block>");
	bool error = false;

	auto node_check = VD();
	node->children.push_back(std::move(node_check.node));
	if (node_check.error) {
		return { std::move(node), node_check.error };
	}

	if (i < lexems.size() && lexems[i].id == 402) { // BEGIN
		node->children.push_back(std::make_unique<TreeNode>("BEGIN"));
		++i;

		auto node_check = SL();
		node->children.push_back(std::move(node_check.node));
		if (node_check.error) {
			return { std::move(node), node_check.error };
		}

		if (i < lexems.size() && lexems[i].id == 403) { // END
			node->children.push_back(std::make_unique<TreeNode>("END"));
			++i;
		} else {
			error_wrapper("keyword 'END' expected");
			error = true;
		}
	} else {
		error_wrapper("keyword 'BEGIN' expected");
		error = true;
	}
	return { std::move(node), error };
}

// <variable-declarations> --> VAR <declarations-list> | <empty>
ParseResult VD() {
	auto node = std::make_unique<TreeNode>("<variable-declarations>");
	bool error = false;
	if (i < lexems.size() && lexems[i].id == 404) { // VAR
		node->children.push_back(std::make_unique<TreeNode>("VAR"));
		++i;

		auto node_check = DL();
		node->children.push_back(std::move(node_check.node));
		if (node_check.error) {
			return { std::move(node), node_check.error };
		}

	} else if (i < lexems.size() && lexems[i].id == 402) { //  <empty>
		node->children.push_back(std::make_unique<TreeNode>("<empty>"));
	} else {
		error_wrapper("keyword 'VAR' expected");
		error = true;
	}
	return { std::move(node), error };
}

// <declarations-list> --> <declaration> <declarationslist> | <empty>
ParseResult DL() {
	auto node = std::make_unique<TreeNode>("<declarations-list>");
	bool error = false;
	if (i < lexems.size() && lexems[i].id == 402) { // BEGIN
		node->children.push_back(std::make_unique<TreeNode>("<empty>"));
	} else {

		auto node_check = D();
		node->children.push_back(std::move(node_check.node));
		if (node_check.error) {
			return { std::move(node), node_check.error };
		}


		node_check = DL();
		node->children.push_back(std::move(node_check.node));
		if (node_check.error) {
			return { std::move(node), node_check.error };
		}

	}
	return { std::move(node), error };
}

// <declaration> --><variable-identifier>:<attribute> ;
ParseResult D() {
	auto node = std::make_unique<TreeNode>("<declaration>");
	bool error = false;
	if (i < lexems.size() && lexems[i].id >= 1001) { // <variable-identifier>

		auto variable_identifier_node = std::make_unique<TreeNode>("<variable-identifier>");
		auto identifier_node = std::make_unique<TreeNode>("<identifier>");
		identifier_node->children.push_back(std::make_unique<TreeNode>(lexems[i].lexem));
		variable_identifier_node->children.push_back(std::move(identifier_node));
		node->children.push_back(std::move(variable_identifier_node));
		++i;
		if (i < lexems.size() && lexems[i].id == 58) { // :
			node->children.push_back(std::make_unique<TreeNode>(":"));
			++i;
			if (i < lexems.size() && (lexems[i].id == 405 || lexems[i].id == 406)) { // <attribute>
				auto attribute_node = std::make_unique<TreeNode>("<attribute>");
				attribute_node->children.push_back(std::make_unique<TreeNode>(lexems[i].lexem));
				node->children.push_back(std::move(attribute_node));
				++i;
				if (i < lexems.size() && lexems[i].id == 59) { // ;
					node->children.push_back(std::make_unique<TreeNode>(";"));
					++i;
				} else {
					error_wrapper("separator ';' expected");
					error = true;
				}
			} else {
				error_wrapper("attribute expected");
				error = true;
			}
		} else {
			error_wrapper("separator ':' expected");
			error = true;
		}
	} else {
		error_wrapper("<variable-identifier> expected");
		error = true;
	}
	return { std::move(node), error };
}

// <statements-list> --> <statement> <statements-list> | <empty>
ParseResult SL() {
	auto node = std::make_unique<TreeNode>("<statements-list>");
	bool error = false;
	if (i < lexems.size() && lexems[i].id == 408) { // IF

		auto node_check = S();
		node->children.push_back(std::move(node_check.node));
		if (node_check.error) {
			return { std::move(node), node_check.error };
		}

		while (i < lexems.size() && lexems[i].id == 408) { // IF

			auto node_check = S();
			node->children.push_back(std::move(node_check.node));
			if (node_check.error) {
				return { std::move(node), node_check.error };
			}
		}

		auto empty_list = std::make_unique<TreeNode>("<statements-list>");
		empty_list->children.push_back(std::make_unique<TreeNode>("<empty>"));
		node->children.push_back(std::move(empty_list));

	} else {
		node->children.push_back(std::make_unique<TreeNode>("<empty>"));
	}

	return { std::move(node), error };
}

// <statement> --> <condition-statement> ENDIF ;
ParseResult S() {
	auto node = std::make_unique<TreeNode>("<statement>");
	bool error = false;

	auto node_check = CS();
	node->children.push_back(std::move(node_check.node));
	if (node_check.error) {
		return { std::move(node), node_check.error };
	}

	if (i < lexems.size() && lexems[i].id == 407) { // ENDIF
		node->children.push_back(std::make_unique<TreeNode>("ENDIF"));
		++i;
		if (i < lexems.size() && lexems[i].id == 59) { // ;
			node->children.push_back(std::make_unique<TreeNode>(";"));
			++i;
		} else {
			error_wrapper("separator ';' expected");
			error = true;
		}
	} else {
		error_wrapper("keyword 'ENDIF' expected");
		error = true;
	}

	return { std::move(node), error };
}

// <condition-statement> --> <incomplete-conditionstatement><alternative-part>
ParseResult CS() {
	auto node = std::make_unique<TreeNode>("<condition-statement>");
	bool error = false;

	auto node_check = ICS();
	node->children.push_back(std::move(node_check.node));
	if (node_check.error) {
		return { std::move(node), node_check.error };
	}

	node_check = AP();
	node->children.push_back(std::move(node_check.node));
	if (node_check.error) {
		return { std::move(node), node_check.error };
	}

	return { std::move(node), error };
}

// <incomplete-condition-statement> --> IF <conditional - expression> THEN <statements-list>
ParseResult ICS() {
	auto node = std::make_unique<TreeNode>("<incomplete-condition-statement>");
	bool error = false;

	if (i < lexems.size() && lexems[i].id == 408) { // IF
		node->children.push_back(std::make_unique<TreeNode>("IF"));
		++i;
		auto node_check = CE();
		node->children.push_back(std::move(node_check.node));
		if (node_check.error) {
			return { std::move(node), node_check.error };
		}

		if (i < lexems.size() && lexems[i].id == 409) { // THEN
			node->children.push_back(std::make_unique<TreeNode>("THEN"));
			++i;

			auto node_check = SL();
			node->children.push_back(std::move(node_check.node));
			if (node_check.error) {
				return { std::move(node), node_check.error };
			}
		} else {
			error_wrapper("keyword 'THEN' expected");
			error = true;
		}
	} else {
		error_wrapper("keyword 'IF' expected");
		error = true;
	}

	return { std::move(node), error };
}

// <conditional-expression> -- > <expression> = <expression>
ParseResult CE() {
	auto node = std::make_unique<TreeNode>("<conditional-expression>");
	bool error = false;

	auto node_check = E();
	node->children.push_back(std::move(node_check.node));
	if (node_check.error) {
		return { std::move(node), node_check.error };
	}

	if (i < lexems.size() && lexems[i].id == 61) { // =
		node->children.push_back(std::make_unique<TreeNode>("="));
		++i;
	} else {
		error = true;
		error_wrapper("separator '=' expected");
		return { std::move(node), error };
	}

	node_check = E();
	node->children.push_back(std::move(node_check.node));
	if (node_check.error) {
		return { std::move(node), node_check.error };
	}

	return { std::move(node), error };
}

ParseResult E() {
	std::unique_ptr<TreeNode> node = nullptr;
	bool error = false;
	if (i < lexems.size() && lexems[i].id >= 501) { // <expression>
		node = std::make_unique<TreeNode>("<expression>");

		if (i < lexems.size() && lexems[i].id >= 1001) { // <variable-identifier>
			auto identifier_type_node = std::make_unique<TreeNode>("<variable-identifier>");
			identifier_type_node->children.push_back(std::make_unique<TreeNode>(lexems[i].lexem));
			node->children.push_back(std::move(identifier_type_node));
		} else if (i < lexems.size() && lexems[i].id >= 501) { // <unsigned-integer>
			auto identifier_type_node = std::make_unique<TreeNode>("<unsigned-identifier>");
			identifier_type_node->children.push_back(std::make_unique<TreeNode>(lexems[i].lexem));
			node->children.push_back(std::move(identifier_type_node));
		}
		++i;
	} else {
		error = true;
		error_wrapper("<expression> expected");
	}
	return { std::move(node), error };
}

// <alternative-part> --> ELSE <statements-list> | <empty>
ParseResult AP() {
	auto node = std::make_unique<TreeNode>("<alternative-part>");
	bool error = false;
	if (i < lexems.size() && lexems[i].id != 407) { // !ENDIF
		if (i < lexems.size() && lexems[i].id == 410) { // ELSE
			node->children.push_back(std::make_unique<TreeNode>("ELSE"));
			++i;
			node->children.push_back(std::move(SL().node));
		}
	} else {
		node->children.push_back(std::make_unique<TreeNode>("<empty>"));
	}

	return { std::move(node), error };
}