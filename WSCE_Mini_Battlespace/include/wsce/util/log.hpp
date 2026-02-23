#pragma once
#include <mutex>
#include <string>

namespace wsce::log {

enum class Level { Debug, Info, Warn, Error };

// Thread-safe, lightweight logger.
void write(Level lvl, const std::string& msg);

inline void debug(const std::string& m) { write(Level::Debug, m); }
inline void info (const std::string& m) { write(Level::Info,  m); }
inline void warn (const std::string& m) { write(Level::Warn,  m); }
inline void error(const std::string& m) { write(Level::Error, m); }

} // namespace wsce::log