// See LICENSE for details

#pragma once
#ifndef SIMPLE_MATCH_SOMEPTR_HPP_JRB_2015_03_21
#define SIMPLE_MATCH_SOMEPTR_HPP_JRB_2015_03_21
#include <memory>
#include <type_traits>

namespace simple_match {
	/* Matches a shared_ptr that can be cast to the given type. */
	template <typename cast_to>
	struct someptr {
		template <class T>
		auto get(const std::shared_ptr<T>& t) {
			return t;
		}
	};

	namespace customization {
		template <typename cast_from, typename cast_to>
		struct matcher<cast_from, someptr<cast_to>> {
			static bool check(const cast_from& t, const someptr<cast_to>& v) {
				return std::dynamic_pointer_cast<cast_to>(t) != nullptr;
			}

			static auto get(const cast_from& t, const someptr<cast_to>& v) {
				auto cast = std::dynamic_pointer_cast<cast_to>(t);
				return std::make_tuple(cast);
			}
		};
	}

	/* Matches a shared pointer that isn't null. */
	struct anyptr_t {
		template <class T>
		auto get(const std::shared_ptr<T>& t) {
			return t;
		}
	};
	const static auto anyptr = anyptr_t();

	namespace customization {
		template <typename cast_from>
		struct matcher<cast_from, anyptr_t> {
			static bool check(const cast_from& t, const anyptr_t& v) {
				return t != nullptr;
			}

			static auto get(const cast_from& t, const anyptr_t& v) {
				return std::tie(t);
			}
		};
	}

	/* Matches a shared pointer that is null. */
	struct noneptr_t {
		template <class T>
		auto get(const std::shared_ptr<T>& t) {
			return t;
		}
	};
	const static auto noneptr = noneptr_t();

	namespace customization {
		template <typename cast_from>
		struct matcher<cast_from, noneptr_t> {
			static bool check(const cast_from& t, const noneptr_t& v) {
				return t == nullptr;
			}

			static auto get(const cast_from& t, const noneptr_t& v) {
				return std::tie();
			}
		};
	}

}
			
#endif
