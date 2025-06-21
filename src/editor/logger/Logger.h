#ifndef LOGGER_H
#define LOGGER_H

#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

/**
 * Logger is a simple logging utility that provides methods to log messages
 * Logger supports different log levels and can be used to log messages to the console and files.
 */

class Logger {
public:
    enum class LogLevel {
        Info,
        Warning,
        Error
    };

    static void log(const char* message, LogLevel level = LogLevel::Info) {
        switch (level) {
            case LogLevel::Info:
                printf("\033[0;32m[INFO] %s\033[0m\n", message); // Green text
                break;
            case LogLevel::Warning:
                printf("\033[0;33m[WARNING] %s\033[0m\n", message); // Yellow text
                break;
            case LogLevel::Error:
                printf("\033[0;31m[ERROR] %s\033[0m\n", message); // Red text
                break;
        }
    }

    static void logToFile(const char* message, LogLevel level = LogLevel::Info) {
        if (m_logToFile && m_logFileDescriptor != -1) {
            const char* levelStr = "";
            switch (level) {
                case LogLevel::Info: levelStr = "[INFO] "; break;
                case LogLevel::Warning: levelStr = "[WARNING] "; break;
                case LogLevel::Error: levelStr = "[ERROR] "; break;
            }
            dprintf(m_logFileDescriptor, "%s%s\n", levelStr, message);
        }
    }

    static void enableFileLogging(const char* filePath) {
        m_logFilePath = filePath;
        m_logFileDescriptor = open(m_logFilePath, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (m_logFileDescriptor == -1) {
            perror("Failed to open log file");
            m_logToFile = false;
        } else {
            m_logToFile = true;
        }
    }

    static void disableFileLogging() {
        if (m_logFileDescriptor != -1) {
            close(m_logFileDescriptor);
            m_logFileDescriptor = -1;
        }
        m_logToFile = false;
    }

    static bool isFileLoggingEnabled() {
        return m_logToFile;
    }

    static const char* getLogFilePath() {
        return m_logFilePath;
    }

    static void setLogFilePath(const char* filePath) {
        m_logFilePath = filePath;
    }

private:
    static int m_logFileDescriptor; // File descriptor for logging to a file, if needed
    static const char* m_logFilePath; // Default log file path
    static bool m_logToFile; // Flag to indicate if logging to a file is enabled
};

#endif // LOGGER_H