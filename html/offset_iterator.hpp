#ifndef __OFFSET_ITERATOR_HPP__
#define __OFFSET_ITERATOR_HPP__

#include <iterator>

namespace boost { namespace spirit {

namespace offset { namespace impl_ {
	template <typename _Iterator>
	struct iterator_base_creator
	{
		typedef std::iterator
		<
		typename std::forward_iterator_tag,
		typename std::iterator_traits<_Iterator>::value_type,
		typename std::iterator_traits<_Iterator>::difference_type,
		typename std::iterator_traits<_Iterator>::pointer,
		typename std::iterator_traits<_Iterator>::reference
		> type;
	};
}}
	
	template <typename _Iterator>
	class offset_iterator
		: public offset::impl_::iterator_base_creator<_Iterator>::type
	{
		typedef
			typename offset::impl_::iterator_base_creator<_Iterator>::type
			_Base;

		public:
			typedef typename _Base::value_type value_type;
			typedef typename _Base::difference_type difference_type;
			typedef typename _Base::reference reference;
			typedef typename _Base::pointer pointer;

			offset_iterator();
			explicit offset_iterator(_Iterator input);
			explicit offset_iterator(_Iterator input, difference_type off);

			~offset_iterator();

			offset_iterator(const offset_iterator &rhs);
			offset_iterator &operator=(const offset_iterator &rhs);

			difference_type offset() const;
			_Iterator iterator() const;

			reference operator*() const;
			pointer operator->() const;
			offset_iterator &operator++();
			offset_iterator operator++(int);

			bool operator==(const offset_iterator &x) const;
			bool operator<(const offset_iterator &x) const;

		private:
			difference_type mOffset;
			_Iterator mInput;
	};

	template <typename _Iterator>
	inline
	offset_iterator<_Iterator>::offset_iterator() : mOffset(0) {}

	template <typename _Iterator>
	inline
	offset_iterator<_Iterator>::offset_iterator(_Iterator i)
		: mOffset(0), mInput(i) {}

	template <typename _Iterator>
	inline
	offset_iterator<_Iterator>::offset_iterator(_Iterator i,
			typename offset_iterator<_Iterator>::difference_type off)
		: mOffset(off), mInput(i) {}

	template <typename _Iterator>
	inline
	offset_iterator<_Iterator>::~offset_iterator() {}

	template <typename _Iterator> 
	inline
	offset_iterator<_Iterator>::offset_iterator(const offset_iterator &x)
		: mOffset(x.mOffset), mInput(x.mInput) {}

	template <typename _Iterator>
	inline
	offset_iterator<_Iterator> &
	offset_iterator<_Iterator>::operator=(const offset_iterator &x)
	{
		mOffset = x.mOffset;
		mInput = x.mInput;
	}

	template <typename _Iterator>
	inline
	typename offset_iterator<_Iterator>::difference_type
	offset_iterator<_Iterator>::offset() const { return mOffset; }

	template <typename _Iterator>
	inline
	_Iterator
	offset_iterator<_Iterator>::iterator() const { return mInput; }
	
	template <typename _Iterator>
	inline
	typename offset_iterator<_Iterator>::reference
	offset_iterator<_Iterator>::operator*() const
	{
		return *mInput;
	}

	template <typename _Iterator>
	inline
	typename offset_iterator<_Iterator>::pointer
	offset_iterator<_Iterator>::operator->() const
	{
		return &(*mInput);
	}

	template <typename _Iterator>
	inline
	offset_iterator<_Iterator> &
	offset_iterator<_Iterator>::operator++()
	{
		++mOffset;
		++mInput;
		return *this;
	}

	template <typename _Iterator>
	inline
	offset_iterator<_Iterator>
	offset_iterator<_Iterator>::operator++(int)
	{
		offset_iterator<_Iterator> tmp(*this);
		++*this;
		return tmp;
	}

	template <typename _Iterator>
	inline
	bool offset_iterator<_Iterator>::operator==(const offset_iterator &x) const
	{
		return mInput == x.mInput;
	}

	template <typename _Iterator>
	inline
	bool offset_iterator<_Iterator>::operator<(const offset_iterator &x) const
	{
		return mInput < x.mInput;
	}

	template <typename _Iterator>
	inline
	bool operator!=(const offset_iterator<_Iterator> &x,
			const offset_iterator<_Iterator> &y)
	{
		return !(x == y);
	}

	template <typename _Iterator>
	inline
	bool operator>(const offset_iterator<_Iterator> &x,
			const offset_iterator<_Iterator> &y)
	{
		return y < x;
	}

	template <typename _Iterator>
	inline
	bool operator>=(const offset_iterator<_Iterator> &x,
			const offset_iterator<_Iterator> &y)
	{
		return !(x < y);
	}

	template <typename _Iterator>
	inline
	bool operator<=(const offset_iterator<_Iterator> &x,
			const offset_iterator<_Iterator> &y)
	{
		return !(y < x);
	}
}}

#endif
