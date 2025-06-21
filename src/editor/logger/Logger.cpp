#include "Logger.h"

// Define static member variables
int Logger::m_logFileDescriptor = -1;
const char* Logger::m_logFilePath = "/var/log/mizan_editor.log";
bool Logger::m_logToFile = false;