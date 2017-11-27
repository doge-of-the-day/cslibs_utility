#ifndef CSLIBS_UTILITY_FEEDBACK_MEMBER_ITERATOR_HPP
#define CSLIBS_UTILITY_FEEDBACK_MEMBER_ITERATOR_HPP

#include <cslibs_utility/common/delegate.hpp>

namespace cslibs_utility {
namespace iterators {
/**
 * @brief The MemberIterator class is used to make a certain member of a data
 *        class available, whereas the full data entry can only be accessed by
 *        const reference.
 */
template<typename data_t, typename T, T data_t::*Member>
class FeedbackMemberIterator : public std::iterator<std::random_access_iterator_tag, T>
{
    using parent    = std::iterator<std::random_access_iterator_tag, T>;
    using iterator  = typename parent::iterator;
    using reference = typename parent::reference;

    using notify_update = delegate<void(const T &)>;

    data_t            *data_;     /// data container content
    notify_update    update_;   /// on update callback

public:
    /**
     * @brief MemberIterator constructor.
     * @param begin     - point to the data in memory
     * @param update    - on update callback
     */
    explicit FeedbackMemberIterator(data_t         *begin,
                            notify_update update) :
        data_(begin),
        update_(update)
    {
    }

    /**
     * @brief operator ++ increments the position within the iterable data and executes the
     *        update callback for the previously accessed data.
     *        It is assumed that the data is iterated till the (n+1)th element though the
     *        n-th element is the last one meant to be accessed.
     * @return the new iterator state
     */
    inline iterator& operator++()
    {
        update_(data_->*Member);
        ++data_;
        return *this;
    }

    /**
     * @brief operator == compares two member iterators.
     * @param _other    - another member iterator
     * @return whether bother itaterators are equal or not
     */
    inline bool operator ==(const FeedbackMemberIterator<data_t, T, Member> &_other) const
    {
        return data_ == _other.data_;
    }

    /**
     * @brief operator != check for inequality of two iterators, meaning that they currently
     *        point to two different positions within the data structure.
     * @param _other - another data iterator
     * @return
     */
    inline bool operator !=(const FeedbackMemberIterator<data_t, T, Member> &_other) const
    {
        return !(*this == _other);
    }

    /**
     * @brief operator * dereferences the member to be accessible.
     * @return reference to the member
     */
    inline reference operator *() const
    {
        return (data_->*Member);
    }

    /**
     * @brief getData returns a const reference to the full data entry not allowing
     *        to change anything. Read only access is the result of that.
     * @return
     */
    inline const data_t& getData() const
    {
        return *data_;
    }
};

/**
 * @brief The MemberDecorator class is used to retrieve member iterators for begin and
 *        end of the data structure, as well as for the type setup.
 */
template<typename data_t, typename container_t, typename T, T data_t::*Member>
class FeedbackMemberDecorator {
public:
    using iterator_t      = FeedbackMemberIterator<data_t, T, Member>;
    using notify_update   = delegate<void(const T&)>;
    using notify_touch    = delegate<void()>;
    using notify_finished = delegate<void()>;

    FeedbackMemberDecorator(container_t                &data,
                    notify_touch                touch,
                    notify_update               update,
                    notify_finished             finished) :
        data_(data),
        untouched_(true),
        touch_(touch),
        update_(update),
        finished_(finished)
    {
    }


    FeedbackMemberDecorator(container_t                &data,
                    notify_touch                touch,
                    notify_update               update) :
        data_(data),
        untouched_(true),
        touch_(touch),
        update_(update),
        finished_([](){return;})
    {
    }

    FeedbackMemberDecorator(container_t &data,
                    notify_touch touch) :
        data_(data),
        untouched_(true),
        touch_(touch),
        update_([](const T&){return;}),
        finished_([](){return;})
    {
    }

    FeedbackMemberDecorator(container_t &data) :
        data_(data),
        untouched_(true),
        touch_([](){return;}),
        update_([](const T&){return;}),
        finished_([](){return;})
    {
    }


    virtual ~FeedbackMemberDecorator()
    {
        if(!untouched_)
            finished_();
    }

    /**
     * @brief  begin returns and iterator pointing to the begin of the data structure
     *         to be iterated.
     * @return iterator at position zero of data
     */
    inline iterator_t begin()
    {
        if(untouched_) {
            untouched_ = false;
            touch_();
        }

        return iterator_t(&(*data_.begin()), update_);
    }

    /**
     * @brief end returns an iterator pointing to the end of the data structure being iterated.
     *        As the std iterator, the end iterator points to the position n+1 if the content
     *        of data is of size n.
     * @return iterato at position n+1
     */
    inline iterator_t end() {
        return iterator_t(&(*data_.end()), update_);
    }

    /**
     * @brief getData allows read only access encapsulated data.
     * @return  const reference
     */
    inline const container_t& getData() const
    {
        return data_;
    }

private:
    container_t               &data_;      /// the container to be iterated
    bool                       untouched_;
    notify_touch               touch_;    /// notify wether an iterator is in use or not
    notify_update              update_;    /// on update callback
    notify_finished            finished_;
};
}
}
#endif // CSLIBS_UTILITY_FEEDBACK_MEMBER_ITERATOR_HPP
