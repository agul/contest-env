#ifdef USE_JNGEN
#include <jngen/jngen.h>
#endif

#include "/Users/agul/dev/olimp/test/tasks/EMultiplication4.cpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <ctime>

enum class Format : uint8_t {
	DEFAULT = 0,
	BOLD = 1,
	ITALIC = 3,
	UNDERLINE = 4,

	BLACK = 30,
	RED = 31,
	GREEN = 32,
	YELLOW = 33,
	BLUE = 34,
	MAGENTA = 35,
	CYAN = 36,
	WHITE = 37
};

std::ostream& operator <<(std::ostream& os, const Format color) {
	return os << "\033[" << static_cast<uint32_t>(color) << "m";
}

namespace jhelper {

struct Test {
	std::string input;
	std::string output;
	bool active;
	bool has_output;
};

bool check_is_equal(std::string expected_output, std::string actual_output) {
	while (!expected_output.empty() && isspace(*--expected_output.end()))
		expected_output.erase(--expected_output.end());
	while (!actual_output.empty() && isspace(*--actual_output.end()))
		actual_output.erase(--actual_output.end());
	return expected_output == actual_output;
}

struct TestRunResult {
	double elapsed_time;
	bool is_ok;
};

TestRunResult run_test(const Test& test) {
	std::cout << "Input: \n" << test.input << std::endl;
	if (test.has_output) {
		std::cout << "Expected output: \n" << test.output << std::endl;
	} else {
		std::cout << "Expected output: \n" << "N/A" << std::endl;
	}

	std::stringstream in(test.input);
	std::ostringstream out;
	std::clock_t start = std::clock();

	EMultiplication4 solver;
	if (EMultiplication4::kMultiTest) {
		size_t tests_count;
		in >> tests_count;
		for (size_t test_id : inclusiveRange<size_t>(1, tests_count)) {
			if (EMultiplication4::kWriteCaseNumber) {
				out << "Case #" << test_id << ": ";
			}
			solver.solve(in, out);
		}
	} else {
		solver.solve(in, out);
	}

	std::clock_t finish = std::clock();
	const double current_time = double(finish - start) / CLOCKS_PER_SEC;
	std::cout << "Actual output: \n" << out.str() << std::endl;

	bool test_result = false;
	if (test.has_output && !EMultiplication4::kUseCustomChecker) {
		test_result = jhelper::check_is_equal(test.output, out.str());
	} else {
		test_result = solver.check(std::istringstream{test.input}, std::istringstream{test.output}, std::istringstream{out.str()});
	}
	std::cout << "Result: "
			<< (test_result ? Format::CYAN : Format::RED)
			<< (test_result ? "OK" : "Wrong answer")
			<< Format::DEFAULT << std::endl;
	std::cout << "Time: " << current_time << " s." << std::endl;
	std::cout << std::endl;

	return {current_time, test_result};
}

}  // namespace jhelper

int main() {

	srand(time(nullptr));
	std::cout << std::fixed;

	std::vector<jhelper::Test> tests = {
		{"4 2\n1 2 -3 -4\n", "12\n", true, true},{"4 3\n-1 -2 -3 -4\n", "1000000001\n", true, true},{"2 1\n-1 1000000000\n", "1000000000\n", true, true},{"10 10\n1000000000 100000000 10000000 1000000 100000 10000 1000 100 10 1\n", "999983200\n", true, true},{"16 8\n-7 0 4 -2 -8 4 9 -4 1 5 -7 5 6 -6 -9 -4", "4762800", true, true},{"5 4\n-10 -6 4 -3 10", "2400", true, true},{"2 2\n-2 4", "999999999", true, true},{"4 4\n2 4 1 -1", "999999999", true, true},{"1 1\n-8", "999999999", true, true},{"1 1\n8", "8", true, true},{"2 1\n-8 -7", "1000000000", true, true},{"2 2\n-8 -7", "56", true, true},{"2 1\n8 7", "8", true, true},{"2 2\n8 7", "56", true, true},{"5 0\n1 1 1 1 1", "1", true, true},{"3 3\n1 2 -4", "999999999", true, true},{"3 3\n1 -2 -3", "6", true, true},
	};

	double max_run_time = 0.0;
	bool all_ok = true;
	size_t test_id = 0;
	size_t skipped_count = 0;

	std::vector<size_t> failed_tests;

	for (const jhelper::Test& test : tests) {
		std::cout << Format::BOLD << Format::MAGENTA << "Test #" << test_id << Format::DEFAULT << std::endl;

		if (!test.active) {
			std::cout << Format::YELLOW << "SKIPPED\n" << Format::DEFAULT << std::endl;
			++skipped_count;
			++test_id;
			continue;
		}

		const jhelper::TestRunResult test_run_result = jhelper::run_test(test);
		max_run_time = std::max(max_run_time, test_run_result.elapsed_time);
		all_ok &= test_run_result.is_ok;
		if (!test_run_result.is_ok) {
			failed_tests.emplace_back(test_id);
			if (EMultiplication4::kStopAfterFirstFail) {
				break;
			}
		}

		++test_id;
	}

	for (size_t i : range(EMultiplication4::kGeneratedTestsCount)) {
		std::cout << Format::BOLD << Format::MAGENTA << "Test #" << test_id << Format::DEFAULT << std::endl;

		std::ostringstream generated_test;
		EMultiplication4::generate_test(generated_test);
		auto test = jhelper::Test{generated_test.str(), "", true, false};

		const jhelper::TestRunResult test_run_result = jhelper::run_test(test);
		max_run_time = std::max(max_run_time, test_run_result.elapsed_time);
		all_ok &= test_run_result.is_ok;
		if (!test_run_result.is_ok) {
			failed_tests.emplace_back(test_id);
			if (EMultiplication4::kStopAfterFirstFail) {
				break;
			}
		}

		++test_id;
	}

	if (all_ok) {
		auto tests_count_to_str = [](const size_t tests_count) {
			return std::to_string(tests_count) + " test" + (tests_count > 1 ? "s" : "");
		};
		std::cout << Format::BOLD << Format::GREEN << tests_count_to_str(test_id - skipped_count) << " OK" << Format::DEFAULT << std::endl;
		std::cout << Format::YELLOW << tests_count_to_str(skipped_count) << " skipped" << Format::DEFAULT << std::endl;
	} else {
		std::cout << Format::BOLD << Format::RED << "Some cases failed:" << Format::DEFAULT << std::endl;
		for (const size_t test_id : failed_tests) {
			std::cout << Format::RED << "Test #" << test_id << Format::DEFAULT << std::endl;
		}
	}

	std::cout << "Maximal time: " << max_run_time << "s." << std::endl;
	return 0;
}
