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

#include <algorithm>
#include <functional>
#include <map>
#include <vector>

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

        /* Flattes a vector-of-vectors into a vector. */
        template <typename V>
        static inline auto flatten(const V& v)
            -> std::vector<typename V::value_type::value_type>
        {
            std::vector<typename V::value_type::value_type> o;
            for (const auto& vv: v)
                for (const auto& e: vv)
                    o.push_back(e);
            return o;
        }

        /* Finds only objects of the given type, returning a filtered vector.
         * */
        template<typename T, typename V>
        static inline auto filter(const V& v)
        {
            auto out = std::vector<std::shared_ptr<T>>();
            for (const auto& e: v) {
                auto cast = std::dynamic_pointer_cast<T>(e);
                if (cast != nullptr)
                    out.push_back(cast);
            }
            return out;
        }

        /* Converts a bunch of shared pointers into weak pointers. */
        template<typename T>
        static inline std::vector<std::weak_ptr<T>> to_weak(
            const std::vector<std::shared_ptr<T>>& v)
        {
            auto out = std::vector<std::weak_ptr<T>>();
            for (const auto& e: v)
                out.push_back(e);
            return out;
        }

        /* Multimaps aren't useful, this fixes them. */
        template<typename K, typename V, typename F>
        static inline void myfmmw(
            const std::multimap<K, V>& map,
            const F& func)
        {
            if (map.size() == 0)
                return;

            auto last_key = map.begin()->first;
            auto cur = std::vector<V>{ map.begin()->second };
            for (auto it = ++(map.begin()); it != map.end(); ++it) {
                if (it->first != last_key) {
                    func(last_key, cur);
                    last_key = it->first;
                    cur = std::vector<V>{ it->second };
                }

                cur.push_back(it->second);
            }

            func(last_key, cur);
        }

        /* A functional fold. */
        template<typename V, typename F, typename B>
        static inline auto fold(const V& v, const B& b, const F& f)
          -> decltype(f(b, std::declval<typename V::value_type>()))
        {
            decltype(f(b, std::declval<typename V::value_type>())) prev = b;
            for (const auto& next: v)
                prev = f(prev, next);
            return prev;
        }

        /* Reverses a collection. */
        template<typename V>
        static inline V reverse(const V& in)
        {
            auto out = V();
            for (const auto& e: in)
                out.push_back(e);
            std::reverse(out.begin(), out.end());
            return out;
        };

         /* Both a map and a zip */
        template <class V1, class V2, typename F>
        static inline auto map_zip(const V1& v1, const V2& v2, const F f)
            -> std::vector<decltype(f(std::declval<typename V1::value_type>(), std::declval<typename V2::value_type>()))>
        {
            std::vector<decltype(f(std::declval<typename V1::value_type>(), std::declval<typename V2::value_type>()))> o;

            auto size = v1.size();
            if (v1.size() != v2.size())
                abort();
            for (size_t i = 0; i < size; ++i)
                o.push_back(f(v1[i], v2[i]));

            return o;
        }

        /* Just a zip. */
        template <class I, typename F>
        static inline auto zip(const I& low, const I& high, const F f)
            -> std::vector<decltype(f(low))>
        {
            std::vector<decltype(f(low))> out;
            for (I i = low; i < high; ++i)
                out.push_back(f(i));
            return out;
        }

        template <typename I, typename F>
        static inline auto sort(const I& input, const F comp) -> I
        {
            I out = input;
            std::sort(out.begin(), out.end(), comp);
            return out;
        }
   }
}

#endif
