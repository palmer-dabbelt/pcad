// See LICENSE for details

#pragma once
#ifndef SIMPLE_MATCH_SOMEOPTION_HPP_JRB_2015_03_21
#define SIMPLE_MATCH_SOMEOPTION_HPP_JRB_2015_03_21
#include <pcad/util/option.h++>
#include <pson/option.h++>
#include <type_traits>

namespace simple_match {
	namespace customization {
		template <typename cast_from, typename cast_to>
		struct matcher<pcad::util::option<cast_from>, pcad::util::option<cast_to>> {
			static bool check(const pcad::util::option<cast_from>& t, const pcad::util::option<cast_to>& v) {
				return t == v;
			};

			static auto get(const pcad::util::option<cast_from>& t, const pcad::util::option<cast_to>& v) {
				return std::make_tuple(t);
			};
		};
	}

	namespace customization {
		template <typename cast_from, typename cast_to>
		struct matcher<pson::option<cast_from>, pcad::util::option<cast_to>> {
			static bool check(const pson::option<cast_from>& t, const pcad::util::option<cast_to>& v) {
				if (t.valid() == false && v.valid() == false)
					return true;

				if (t.valid() == false || v.valid() == false)
					return false;

				return t.data() == v.data();
			};

			static auto get(const pson::option<cast_from>& t, const pcad::util::option<cast_to>& v) {
				return std::make_tuple(t);
			};
		};
	}
}

#endif
