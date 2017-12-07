#ifndef CSLIBS_UTILITY_CSV_LOGGER_HPP
#define CSLIBS_UTILITY_CSV_LOGGER_HPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <queue>

#include <cslibs_utility/logger/csv_writer.hpp>

namespace cslibs_utility {
namespace logger {
template<typename ... Types>
class CSVLogger {
public:
    using Ptr = std::shared_ptr<CSVLogger<Types ...>>;

    static constexpr std::size_t size = sizeof ... (Types);
    using header_t = std::array<std::string, size>;

    inline void log(const Types ... ts)
    {
        writer_->write(getTime(), ts...);
    }

    inline CSVLogger(const header_t &header,
                     const std::string &path = "")
    {
        typename CSVWriter<std::string, Types ...>::header_t head;
        head[0] = "time";
        for(std::size_t i = 0 ; i < size ; ++i) {
            head[i+1] = header[i];
        }
        writer_.reset(new CSVWriter<std::string, Types ...>(head, path == "" ? "/tmp/" + getTime() + ".log" : path));
    }

private:
    typename CSVWriter<std::string, Types ...>::Ptr writer_;

    inline void getTime(long &seconds,
                        long &milliseconds)
    {
        auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
        milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        seconds = milliseconds / 1000;
        milliseconds = milliseconds % 1000;
    }

    inline std::string getTime()
    {
        long s, ms;
        getTime(s, ms);

        const std::string ms_off = ms >= 100 ? "" : (ms >= 10 ? "0" : "00");
        const std::string time = std::to_string(s) + "." + ms_off + std::to_string(ms);
        return time;
    }
};
}
}

#endif // CSLIBS_UTILITY_CSV_LOGGER_HPP
