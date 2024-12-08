#pragma once
#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <sstream>

#define logger sl::Logger::instance()

namespace sl {
	class Logger {
	public:
		// Get the single instance of Logger
		static Logger& instance() {
			static Logger instance;
			return instance;
		}

		// Delete copy constructor and assignment operator to prevent copying
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;

		// Overload operator<< for logging
		template <typename T>
		Logger& operator<<(const T& value) {
			std::lock_guard<std::mutex> lock(mutex_); // Thread-safe
			logStream_ << value; // Write to buffer
			return *this;
		}

		// Special function to flush the log stream
		Logger& operator<<(std::ostream& (*manip)(std::ostream&)) {
			std::lock_guard<std::mutex> lock(mutex_); // Thread-safe
			logFile_ << logStream_.str(); // Write buffered log to file
			logStream_.str("");           // Clear the buffer
			logStream_.clear();
			logFile_ << manip; // Support for manipulators like std::endl
			return *this;
		}

	private:
		std::ofstream logFile_;
		std::ostringstream logStream_; // Buffer for thread-safe stream operations
		std::mutex mutex_;

		// Private constructor
		Logger() {
			logFile_.open("log.txt", std::ios::trunc); // Open file in append mode
			if (!logFile_.is_open()) {
				throw std::runtime_error("Failed to open log file.");
			}
		}

		// Private destructor
		~Logger() {
			if (logFile_.is_open()) {
				logFile_.close();
			}
		}
	};
}
