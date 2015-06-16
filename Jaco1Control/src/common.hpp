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
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>
#include <boost/date_time.hpp>
#include <boost/shared_ptr.hpp>
#include  <boost/make_shared.hpp>
#include <armadillo>


// typedef
typedef arma::vec                                      State;
typedef State*                                         State_ptr;
typedef boost::lockfree::queue<State_ptr>              DataFlow;
typedef std::list<State>                               DataStore;
typedef boost::atomic<State_ptr>                       DataLast;

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str();

// template function
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
T* GetData(std::vector<T>* vec)
{
	T * p=&(*vec);
	return p;
}
template <typename T>
T* GetData(arma::Col<T>* vec)
{
	T * p = vec->memptr();
}
template <typename T>
T* GetData(arma::Row<T>* vec)
{
	T * p = vec->memptr();
}




#endif /* COMMON_HPP_ */