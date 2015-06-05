/*
 * common.hpp
 *
 *  Created on: Jun 1, 2015
 *      Author: vale
 */

#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <vector>
#include <cassert>
#include <algorithm>

template<typename T, size_t N>
 T * End(T (&ra)[N]) {
    return ra + N;
};


template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b)
{
	//FIX THIS!!!
    //std::assert( a.size() == b.size() );

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::plus<T>());
    return result;
}


template <typename T>
std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b)
{
	//FIX THIS!!!
    //std::assert( a.size() == b.size() );

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::minus<T>());
    return result;
}

template <typename T>
std::vector<T> operator*(T b,const std::vector<T>& a)
{
	//FIX THIS!!!
    //std::assert( a.size() == b.size() );

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), a.begin(),
    			   std::back_inserter(result),std::bind1st(std::multiplies<T>(),b));
    return result;
}
template <typename T>
std::vector<T> operator*(const std::vector<T>& a,T b)
{
	//FIX THIS!!!
    //std::assert( a.size() == b.size() );

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), a.begin(),
    			   std::back_inserter(result),std::bind1st(std::multiplies<T>(),b));
    return result;
}


//ADD OPERATOR scalar product and subtraction

#endif /* COMMON_HPP_ */
