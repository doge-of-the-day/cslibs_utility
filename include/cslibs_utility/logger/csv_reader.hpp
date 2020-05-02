#ifndef CSLIBS_UTILITY_CSV_READER_HPP
#define CSLIBS_UTILITY_CSV_READER_HPP

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <sstream>
#include <thread>
#include <tuple>

namespace cslibs_utility {
namespace logger {
template <typename T>
inline T fromString(const std::string &str) {
  T data;
  std::stringstream ss{str};
  ss >> data;
  return data;
}

inline void splitLine(const std::string &line, std::vector<std::string> &tokens,
                      const char delimiter = ',') {
  std::stringstream ss(line);
  while (std::getline(ss, tokens.emplace_back(), delimiter)) {
  }
}

template <typename... Types>
class CSVReader {
 public:
  using Ptr = std::unique_ptr<CSVReader<Types...>>;

  static constexpr std::size_t size = sizeof...(Types);
  using header_t = std::array<std::string, size>;
  using entry_t = std::tuple<Types...>;
  using data_t = std::vector<entry_t>;

  inline explicit CSVReader(const std::string &path,
                            const bool has_header = false)
      : in_{path} {}
  inline virtual ~CSVReader() = default;

  inline bool hasHeader() const { return header_.has_value(); }

  inline header_t const &getHeader() const { return header_.value(); }

  inline data_t const &getData() const { return data_; }

 private:
  std::ifstream in_;
  std::optional<header_t> header_;
  data_t data_;

  void read(const bool has_header) {
    if (in_.is_open()) {
      std::string line;
      if (has_header) {
        if (std::getline(in_, line)) {
          header_ = header_t();
          std::vector<std::string> header_tokens;
          splitLine(line, header_tokens);
          for (std::size_t i = 0; i < size; ++i) {
            header_.value()[i] = header_tokens[i];
          }
        } else {
          std::cerr << "[CSVReader]: Could not read header." << std::endl;
        }
      }

      while (std::getline(in_, line)) {
        std::vector<std::string> tokens;
        splitLine(line, tokens);
        auto it = tokens.begin();

        entry_t entry;
        auto iterate = [&it](){const auto value = *it; ++it; return value;};

        std::apply(
            [&iterate](auto &&... args) {
              ((args = fromString<decltype(args)>(iterate())), ...);
            },
            entry);
      }
    }
  }

  template <typename WT, typename... WTypes>
  inline std::string buildString(const WT &t, WTypes... ts) const {
    return toString(t) + "," + buildString(ts...);
  }

  template <typename T>
  inline std::string buildString(const T &t) const {
    return toString(t);
  }
};
}  // namespace logger
}  // namespace cslibs_utility

#endif  // CSLIBS_UTILITY_CSV_READER_HPP
