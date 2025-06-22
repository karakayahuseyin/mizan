#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

/**
 * Logger is a modern C++17 logging utility that provides methods to log messages
 * Logger supports different log levels and can be used to log messages to the console and files.
 */
class Logger {
public:
    enum class LogLevel {
        Info,
        Warning,
        Error,
        Debug
    };

    // Modern string-based logging methods
    static void log(std::string_view message, LogLevel level = LogLevel::Info);
    static void log(const std::string& message, LogLevel level = LogLevel::Info);
    
    // Template method for formatted logging
    template<typename... Args>
    static void logf(LogLevel level, std::string_view format, Args&&... args);
    
    // Convenience methods for different log levels
    static void info(std::string_view message) { log(message, LogLevel::Info); }
    static void warning(std::string_view message) { log(message, LogLevel::Warning); }
    static void error(std::string_view message) { log(message, LogLevel::Error); }
    static void debug(std::string_view message) { log(message, LogLevel::Debug); }
    
    // File logging with modern filesystem API
    static void logToFile(std::string_view message, LogLevel level = LogLevel::Info);
    static bool enableFileLogging(const std::filesystem::path& filePath);
    static void disableFileLogging();
    
    // Configuration methods
    static bool isFileLoggingEnabled() noexcept { return m_fileLoggingEnabled; }
    static const std::filesystem::path& getLogFilePath() noexcept { return m_logFilePath; }
    static void setLogFilePath(const std::filesystem::path& filePath);
    static void setMinLogLevel(LogLevel level) noexcept { m_minLogLevel = level; }
    static LogLevel getMinLogLevel() noexcept { return m_minLogLevel; }
    
    // Utility methods
    static std::string levelToString(LogLevel level);
    static std::string getCurrentTimestamp();

private:
    static std::ofstream m_logFileStream;
    static std::filesystem::path m_logFilePath;
    static bool m_fileLoggingEnabled;
    static LogLevel m_minLogLevel;
    
    // Helper methods
    static std::string formatMessage(std::string_view message, LogLevel level);
    static void writeToConsole(const std::string& formattedMessage, LogLevel level);
    static void writeToFile(const std::string& formattedMessage);
    static bool shouldLog(LogLevel level) noexcept;
};

// Template implementation
template<typename... Args>
void Logger::logf(LogLevel level, std::string_view format, Args&&... args) {
    if (!shouldLog(level)) return;
    
    // Simple string formatting (C++20 would have std::format)
    std::ostringstream oss;
    ((oss << args << " "), ...); // C++17 fold expression
    std::string message = std::string(format) + ": " + oss.str();
    log(message, level);
}

#endif // LOGGER_H