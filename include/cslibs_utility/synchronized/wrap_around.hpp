#ifndef CSLIBS_UTILITY_WRAP_AROUND_HPP
#define CSLIBS_UTILITY_WRAP_AROUND_HPP

#include <mutex>
#include <assert.h>

namespace cslibs_utility {
namespace synchronized {
template<typename data_t>
class WrapAround {
public:
    using mutex_t = std::mutex;

    inline WrapAround() :
        data_(nullptr)
    {
    }

    inline explicit WrapAround(data_t  *data,
                               mutex_t *mutex) :
        data_(data),
        mutex_(mutex)
    {
        assert(mutex != nullptr);
        assert(data  != nullptr);
        if(data_) {
            mutex_->lock();
        }
    }

    inline WrapAround(const WrapAround &other) = default;
    inline WrapAround(WrapAround &&other) = default;
    inline WrapAround& operator = (const WrapAround &other) = default;
    inline WrapAround& operator = (WrapAround &&other) = default;

    virtual inline ~WrapAround()
    {
        if(data_) {
            mutex_->unlock();
        }
    }

    inline bool empty() const
    {
        return data_ == nullptr;
    }

    inline const data_t& data() const
    {
        return *data_;
    }

    inline data_t& data()
    {
        return *data_;
    }

    inline operator data_t* ()
    {
        return data_;
    }

    inline operator data_t const *() const
    {
        return data_;
    }

    inline data_t * operator -> ()
    {
        return data_;
    }

    inline data_t const * operator -> () const
    {
        return data_;
    }

private:
    data_t  *data_;
    mutex_t *mutex_;
};
}
}

#endif // CSLIBS_UTILITY_WRAP_AROUND_HPP
