#include "Logger.h"

// Define static member variables
std::ofstream Logger::m_logFileStream;
std::filesystem::path Logger::m_logFilePath = "/var/log/mizan_editor.log";
bool Logger::m_fileLoggingEnabled = false;
Logger::LogLevel Logger::m_minLogLevel = Logger::LogLevel::Info;

void Logger::log(std::string_view message, LogLevel level) {
    if (!shouldLog(level)) return;
    
    std::string formattedMessage = formatMessage(message, level);
    
    writeToConsole(formattedMessage, level);
    
    if (m_fileLoggingEnabled) {
        writeToFile(formattedMessage);
    }
}

void Logger::log(const std::string& message, LogLevel level) {
    log(std::string_view(message), level);
}

void Logger::logToFile(std::string_view message, LogLevel level) {
    if (!m_fileLoggingEnabled || !shouldLog(level)) return;
    
    std::string formattedMessage = formatMessage(message, level);
    writeToFile(formattedMessage);
}

bool Logger::enableFileLogging(const std::filesystem::path& filePath) {
    try {
        // Ensure directory exists
        if (filePath.has_parent_path()) {
            std::filesystem::create_directories(filePath.parent_path());
        }
        
        m_logFilePath = filePath;
        m_logFileStream.open(m_logFilePath, std::ios::app);
        
        if (m_logFileStream.is_open()) {
            m_fileLoggingEnabled = true;
            
            // Log startup message
            std::string startupMsg = "=== Logging session started at " + getCurrentTimestamp() + " ===";
            writeToFile(startupMsg);
            
            return true;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to enable file logging: " << e.what() << std::endl;
    }
    
    m_fileLoggingEnabled = false;
    return false;
}

void Logger::disableFileLogging() {
    if (m_logFileStream.is_open()) {
        std::string shutdownMsg = "=== Logging session ended at " + getCurrentTimestamp() + " ===";
        writeToFile(shutdownMsg);
        m_logFileStream.close();
    }
    m_fileLoggingEnabled = false;
}

void Logger::setLogFilePath(const std::filesystem::path& filePath) {
    bool wasEnabled = m_fileLoggingEnabled;
    if (wasEnabled) {
        disableFileLogging();
    }
    
    m_logFilePath = filePath;
    
    if (wasEnabled) {
        enableFileLogging(m_logFilePath);
    }
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Debug:   return "DEBUG";
        default:                return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

std::string Logger::formatMessage(std::string_view message, LogLevel level) {
    std::ostringstream oss;
    oss << "[" << getCurrentTimestamp() << "] "
        << "[" << levelToString(level) << "] "
        << message;
    return oss.str();
}

void Logger::writeToConsole(const std::string& formattedMessage, LogLevel level) {
    switch (level) {
        case LogLevel::Info:
            std::cout << "\033[0;32m" << formattedMessage << "\033[0m" << std::endl;
            break;
        case LogLevel::Warning:
            std::cout << "\033[0;33m" << formattedMessage << "\033[0m" << std::endl;
            break;
        case LogLevel::Error:
            std::cerr << "\033[0;31m" << formattedMessage << "\033[0m" << std::endl;
            break;
        case LogLevel::Debug:
            std::cout << "\033[0;36m" << formattedMessage << "\033[0m" << std::endl;
            break;
    }
}

void Logger::writeToFile(const std::string& formattedMessage) {
    if (m_logFileStream.is_open()) {
        m_logFileStream << formattedMessage << std::endl;
        m_logFileStream.flush(); // Ensure immediate write
    }
}

bool Logger::shouldLog(LogLevel level) noexcept {
    return level >= m_minLogLevel;
}