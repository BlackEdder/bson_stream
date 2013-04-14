/* Copyright 2013 Edwin van Leeuwen.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef BSON_STREAM_H
#define BSON_STREAM_H

#include "bson/bson.h"

template<class T>
void operator>>( const mongo::BSONElement &bel, T &t ) {
	bel.Val( t );
}

void operator>>( const mongo::BSONElement &bel, double &t ) {
	t = bel.Number();
}

template<class T>
void operator>>( const mongo::BSONElement &bel, std::vector<T> &v ) {
	auto barr = bel.Array();
	for ( auto bson_el : barr ) {
		// This will only work if T has an empty constructor T()
		// I am not aware of a more general way of doing this though
		T el;
		bson_el >> el;
		v.push_back( el );
	}
}

template<class T>
void operator>>( const mongo::BSONElement &bel, std::list<T> &v ) {
	auto barr = bel.Array();
	for ( auto bson_el : barr ) {
		// This will only work if T has an empty constructor T()
		// I am not aware of a more general way of doing this though
		T el;
		bson_el >> el;
		v.push_back( el );
	}
}

template<class T>
void operator>>( const mongo::BSONElement &bel, std::set<T> &v ) {
	auto barr = bel.Array();
	for ( auto bson_el : barr ) {
		// This will only work if T has an empty constructor T()
		// I am not aware of a more general way of doing this though
		T el;
		bson_el >> el;
		v.insert( el );
	}
}

template<class K, class V>
void operator>>( const mongo::BSONObj &bobj, std::map<K,V> &map ) {
	for ( mongo::BSONObj::iterator i = bobj.begin(); i.more(); ) {
		mongo::BSONElement el = i.next();
		V value;
		el >> value;
		map[el.fieldName()] = value;
	}
}

#endif
