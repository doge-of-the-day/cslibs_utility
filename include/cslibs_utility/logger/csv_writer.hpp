#ifndef CSLIBS_UTILITY_CSV_WRITER_HPP
#define CSLIBS_UTILITY_CSV_WRITER_HPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <queue>

namespace cslibs_utility {
namespace logger {
template<typename ... Types>
class CSVWriter {
public:
    using Ptr = std::shared_ptr<CSVWriter<Types ...>>;

    static constexpr std::size_t size = sizeof ... (Types);
    using Header = std::array<std::string, size>;

    inline void write(const Types ... ts)
    {
        std::string string_built = buildString(ts...);
        std::unique_lock<std::mutex> q_lock(q_mutex_);
        q_.push(string_built);
        notify_log_.notify_one();
    }

    CSVWriter(const Header &header,
              const std::string &path) :
        header_(header),
        has_header_(true),
        path_(path),
        stop_(false)
    {
        running_ = true;
        worker_thread_ = std::thread([this]{loop();});
    }

    CSVWriter(const std::string &path) :
        has_header_(false),
        path_(path),
        stop_(false)
    {
        running_ = true;
        worker_thread_ = std::thread([this]{loop();});
    }


    virtual ~CSVWriter()
    {
        if(running_) {
            stop_ = true;
            notify_log_.notify_one();
            if(worker_thread_.joinable())
                worker_thread_.join();
        }
    }

private:
    std::ofstream           out_;
    Header                  header_;
    bool                    has_header_;
    std::string             path_;
    long                    start_time_;

    std::thread             worker_thread_;
    std::mutex              q_mutex_;
    std::queue<std::string> q_;
    std::mutex              notify_mutex_;
    std::condition_variable notify_log_;

    std::atomic_bool        running_;
    std::atomic_bool        stop_;

    void loop()
    {
        out_.open(path_);
        if(!out_.is_open()) {
            std::cerr << "[CSVWriter]: Could not open path '" << path_ << "'!\n";
            return;
        }

        if(has_header_) {
            for(std::size_t i = 0 ; i < size - 1 ; ++i) {
                out_ << header_[i];
            }
            out_ << header_[size - 1];
            out_ << "\n";
        }

        auto dumpQ = [this] () {
            while(!q_.empty()) {
                std::unique_lock<std::mutex> q_lock(q_mutex_);
                auto f = q_.front();
                q_.pop();
                q_lock.unlock();

                out_ << f << "\n";
            }
        };

        std::unique_lock<std::mutex> notify_lock(notify_mutex_);
        while(!stop_) {
            notify_log_.wait(notify_lock);
            dumpQ();
        }
        dumpQ();

        out_.flush();
        if(out_.is_open())
            out_.close();

        running_ = false;
    }

    template<typename WT, typename ... WTypes>
    inline std::string buildString(const WT &t, WTypes ... ts) const
    {
        return std::to_string(t) + "," + buildString(ts...);
    }

    template<typename T>
    inline std::string buildString(const T &t) const
    {
        return std::to_string(t);
    }

};
}
}

#endif // CSLIBS_UTILITY_CSV_WRITER_HPP
