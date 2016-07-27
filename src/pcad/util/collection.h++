/*
 * Copyright (C) 2015 Palmer Dabbelt
 *   <palmer@dabbelt.com>
 *
 * This file is part of putil
 * 
 * putil is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * putil is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with putil.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PUTIL__COLLECTION_HXX
#define PUTIL__COLLECTION_HXX

#include <vector>
#include <algorithm>
#include <functional>

namespace putil {
    namespace collection {
        /* A functional map. */
        template <class V, typename F>
        static inline auto map(const V& v, const F f)
            -> std::vector<decltype(f(std::declval<typename V::value_type>()))>
        {
            std::vector<decltype(f(std::declval<typename V::value_type>()))> o;
            std::transform(v.begin(), v.end(), std::back_inserter(o), f);
            return o;
        }
    }
}

#endif
