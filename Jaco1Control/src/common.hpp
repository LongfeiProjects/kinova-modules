/*
 * common.hpp
 *
 *  Created on: Jun 1, 2015
 *      Author: vale
 */

#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <cassert>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <ctime>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>
#include <boost/date_time.hpp>
#include <boost/chrono.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lockable_concepts.hpp>
#include <armadillo>


// typedef
typedef arma::vec                                      State;
typedef State*                                         State_ptr;
typedef std::vector<State>                             Log;
typedef boost::lockfree::queue<State_ptr>              DataFlow;
typedef std::list<State>                               DataStore;
typedef DataStore::iterator                            DataStoreIt;
typedef DataStoreIt*                                   PtrDataStoreIt;
typedef boost::atomic<State_ptr>                       DataLast;

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str();

// variable
struct Option{
   std::string type_of_controller;               // CartesianKinematic
   std::vector<std::string> meas_val;
   int  control_action;
};

// template function
template<typename T, size_t N>
 T * End(T (&ra)[N]) {
    return ra + N;
}

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

// api mutex (to use when  the api implementation is not threaded)
extern boost::recursive_mutex api_mutex;
extern boost::atomic<bool>  _first;
extern boost::atomic<bool>  _second;

// Constant

extern double DEG;

// Mathematical routine
void Mat2RPY(arma::mat m,arma::vec & rpy);

#endif /* COMMON_HPP_ */

