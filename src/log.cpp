#include "log.hpp"

#include <exception>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <print>
#include <sstream>
#include <streambuf>
#include <thread>

#include "util/preferences.hpp"
#include "util/time.hpp"

using namespace game::util;

namespace game
{
  class StderrToLoggerBuf final : public std::streambuf
  {
    Logger* logger{};
    std::string buffer;
    std::mutex mutex;

  public:
    explicit StderrToLoggerBuf(Logger* target) : logger(target) {}

  protected:
    int overflow(int ch) override
    {
      if (ch == traits_type::eof()) return traits_type::not_eof(ch);
      std::lock_guard<std::mutex> lock(mutex);
      if (ch == '\n')
      {
        if (logger && !buffer.empty())
        {
          logger->error(buffer);
          buffer.clear();
        }
      }
      else
      {
        buffer.push_back(static_cast<char>(ch));
      }
      return ch;
    }

    int sync() override
    {
      std::lock_guard<std::mutex> lock(mutex);
      if (logger && !buffer.empty())
      {
        logger->error(buffer);
        buffer.clear();
      }
      return 0;
    }
  };

  std::streambuf* old_stderr_buf = nullptr;

  void Logger::write_raw(const std::string& message)
  {
    std::println("{}", message);
    if (file.is_open()) file << message << '\n' << std::flush;
  }

  void Logger::write(const Level level, const std::string& message)
  {
    std::string formatted = std::format("{} {} {}", LEVEL_STRINGS[level], time::get("(%d-%B-%Y %I:%M:%S)"), message);
    write_raw(formatted);
  }

  void Logger::info(const std::string& message) { write(LEVEL_INFO, message); }
  void Logger::warning(const std::string& message) { write(LEVEL_WARNING, message); }
  void Logger::error(const std::string& message) { write(LEVEL_ERROR, message); }
  void Logger::fatal(const std::string& message) { write(LEVEL_FATAL, message); }
  void Logger::open(const std::filesystem::path& path) { file.open(path, std::ios::out | std::ios::app); }

  std::filesystem::path Logger::path() { return preferences::path() / "log.txt"; }

  Logger::Logger()
  {
    open(path());
    static StderrToLoggerBuf stderr_buf(this);
    old_stderr_buf = std::cerr.rdbuf(&stderr_buf);
    std::cerr.setf(std::ios::unitbuf);
    std::set_terminate(
        []
        {
          try
          {
            if (auto eptr = std::current_exception()) std::rethrow_exception(eptr);
          }
          catch (const std::exception& ex)
          {
            logger.fatal(std::string("Unhandled exception: ") + ex.what());
          }
          catch (...)
          {
            logger.fatal("Unhandled exception: <unknown>");
          }
          std::abort();
        });
  }
}

game::Logger logger;
