/**
 * @file vpu_util.hpp
 * @author Matthijs Bakker
 * @brief Compile-time functions that are not in the std for some reason
 */

#ifndef VPU_UTIL_HPP
#define VPU_UTIL_HPP

constexpr std::size_t vpu_apow(std::size_t num, std::size_t exp) {
	return num * (2 << exp);
}

// from https://baptiste-wicht.com/posts/2014/07/compile-integer-square-roots-at-compile-time-in-cpp.html
constexpr std::size_t vpu_sqrt(std::size_t res, std::size_t l, std::size_t r) {
	if (l == r) {
		return r;
	}
	const size_t mid = (r + l) / 2;

	if (mid * mid >= res) {
		return vpu_sqrt(res, l, mid);
	} else {
		return vpu_sqrt(res, mid + 1, r);
	}
}

constexpr std::size_t vpu_sqrt(std::size_t res) {
    return vpu_sqrt(res, 1, res);
}

#endif /* VPU_UTIL_HPP */

