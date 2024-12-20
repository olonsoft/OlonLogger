#pragma once

#include <Arduino.h>
#include <Print.h>
#include <StreamString.h>
#include <stdarg.h>  // For va_list, va_start, va_end

#define OLON_LOG_VERSION "1.1.0"

#define OLON_LOG_BUFFER_SIZE 256

// Define log levels
#define OLON_LOG_LEVEL_NONE  0
#define OLON_LOG_LEVEL_ERROR 1
#define OLON_LOG_LEVEL_WARN  2
#define OLON_LOG_LEVEL_INFO  3
#define OLON_LOG_LEVEL_DEBUG 4

// ANSI escape codes for colors
#define OLON_LOG_COLOR_RESET  "\033[0m"
#define OLON_LOG_COLOR_RED    "\033[31m"  // Error (Red)
#define OLON_LOG_COLOR_YELLOW "\033[33m"  // Warning (Yellow)
#define OLON_LOG_COLOR_GREEN  "\033[32m"  // Info (Green)
#define OLON_LOG_COLOR_BLUE   "\033[34m"  // Debug (Blue)

namespace Olon {

class Logger {
 public:
  Logger()
      : _level(OLON_LOG_LEVEL_DEBUG) {
  }

  void setLevel(uint8_t level) {
    _level = level;
  }

  uint8_t getLevel() {
    return _level;
  }

  void setOutput(Print* printer) {
    _outputs.push_back(printer);
  }

  template <typename... Args>
  void debug(const char* tag, const char* format, Args... args) {
    log(OLON_LOG_LEVEL_DEBUG, tag, format, args...);
  }

  template <typename... Args>
  void info(const char* tag, const char* format, Args... args) {
    log(OLON_LOG_LEVEL_INFO, tag, format, args...);
  }

  template <typename... Args>
  void warn(const char* tag, const char* format, Args... args) {
    log(OLON_LOG_LEVEL_WARN, tag, format, args...);
  }

  template <typename... Args>
  void error(const char* tag, const char* format, Args... args) {
    log(OLON_LOG_LEVEL_ERROR, tag, format, args...);
  }

  template <typename... Args>
  void log(uint8_t level, const char* tag, const char* format, Args... args) {
    if (level > _level)
      return;

    // check if level is within range
    if (level > OLON_LOG_LEVEL_DEBUG) return;

    StreamString buffer;
    buffer.reserve(OLON_LOG_BUFFER_SIZE);

    // Add color based on the log level
    buffer.print(_colors[level]);

    buffer.printf("%6lu [%c] [%s] ",
                  millis(),
                  _codes[level],
                  tag);
    buffer.printf(format, args...);

    // Reset color after the message
    buffer.print(OLON_LOG_COLOR_RESET);

    buffer.print("\r\n");

    for (auto& output : _outputs) {
      output->print(buffer);
    }
  }

 private:
  const char* _codes     = " EWID";
  const char* _colors[5] = {
      OLON_LOG_COLOR_RESET,   // None
      OLON_LOG_COLOR_RED,     // Error
      OLON_LOG_COLOR_YELLOW,  // Warning
      OLON_LOG_COLOR_GREEN,   // Info
      OLON_LOG_COLOR_BLUE     // Debug
  };

  uint8_t             _level;
  std::vector<Print*> _outputs;
};

}  // namespace Olon