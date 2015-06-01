/*
 * common.hpp
 *
 *  Created on: Jun 1, 2015
 *      Author: vale
 */

#ifndef COMMON_HPP_
#define COMMON_HPP_

template<typename T, size_t N>
 T * end(T (&ra)[N]) {
    return ra + N;
};

#endif /* COMMON_HPP_ */
