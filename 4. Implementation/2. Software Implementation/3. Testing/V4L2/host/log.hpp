#pragma once

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

namespace log {

	struct Logger {

	private:

		std::string prefix;
		std::ostream& stream;

	public:

		friend std::ostream& operator<<(const log::Logger& logger, const std::string& str) {
			
			logger.stream << logger.prefix << str;
			return logger.stream;
		}

		friend std::ostream& operator<<(const log::Logger& logger, const char* str) {
			
			logger.stream << logger.prefix << str;
			return logger.stream;
		}

		friend std::ostream& operator<<(const log::Logger& logger, int32_t num) {
			
			logger.stream << logger.prefix << to_string(num);
			return logger.stream;
		}

		friend std::ostream& operator<<(const log::Logger& logger, uint32_t num) {
			
			logger.stream << logger.prefix << to_string(num);
			return logger.stream;
		}

		Logger(std::string prefix, std::ostream& stream) : prefix(prefix), stream(stream) {

		}

	};

	const Logger   error("  [ERROR]: ", std::cerr);
	const Logger	info("   [INFO]: ", std::cout);
	const Logger warning("[WARNING]: ", std::cout);
	const Logger      ok("     [OK]: ", std::cout);
	const Logger details("\t[DETAILS]: ", std::cout);
	const Logger generic("",  std::cout);
};