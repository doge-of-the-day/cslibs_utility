#ifndef CSLIBS_UTILITY_WRAP_AROUND_HPP
#define CSLIBS_UTILITY_WRAP_AROUND_HPP

#include <mutex>

namespace cslibs_utility {
namespace synchronized {
template<typename data_t, std::mutex data_t::*mutex>
class WrapAround {
public:
    inline WrapAround() :
        data_(nullptr)
    {
    }

    inline WrapAround(data_t *container) :
        data_(container)
    {
        if(data_)
            (data_->*mutex).lock();
    }

    virtual inline ~WrapAround()
    {
        if(data_)
          (data_->*mutex).unlock();
    }

    inline bool empty() const
    {
        return data_ == nullptr;
    }

    inline const data_t& data() const
    {
        return data_->data_;
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
    data_t *data_;
};
}
}

#endif // CSLIBS_UTILITY_WRAP_AROUND_HPP
