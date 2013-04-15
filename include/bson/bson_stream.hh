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

/**
 * Stream in
 *
 * YAML works as follows:
 Emitter out;
 out << Emitter::Key << "name";
 out << Emitter::Value << "Ryan Braun";
 out << Emitter::Key << "position";
 out << Emitter::Value << "LF";
 BSONobj bobj = out.obj();

 I guess we should start with just building on BSONBuilder, which requires that 
 a value could be any object, which then needs to be turned into a BSONElement, so
 one first needs to support <<( BSONElement &bel, T &t )

 It depends a bit on how BSONObjBuilder works if just adding my own classes 
 to <<(BSONElement &bel, myclass &c ) will be enough, 
 or do we need to use <<(BSONObjBuilder, myclass). If the second then it will be
 cleaner to add <<(BSONObjBuilder, T) which in turn calls <<(BSONElement, T)
 */


namespace mongo {
/**
 * \brief Define an emitter for BSONObjects
 *
 * Behaviour is partly based on BSONObjBuilder, but different enough that
 * we need a separate class
 */
class BSONEmitter {
	public:
		BSONEmitter() : v_emitter( this ) {}
		BSONEmitter( BSONObjBuilder *builder ) 
			: builder( builder ), v_emitter( this )
		{}

		BSONObj obj() {
			return builder->obj();
		}
		BSONValueEmitter &append( const StringData &name ) {
		 	v_emitter.builder.endField( name );
			return v_emitter;
		}

		BSONObjBuilder *builder;
		BSONValueEmitter v_emitter;
};

class BSONValueEmitter {
	public:
		BSONValueEmitter( BSONEmitter *pEmitter ) 
			: pEmitter( pEmitter ), builder( pEmitter.builder ) {}

		template<class T>
		BSONEmitter &append( const T &t ) {
			pEmitter->builder = builder.append( t );
			return pEmitter;
		}

		BSONObjBuilderValueStream builder;
		BSONEmitter *pEmitter;
};
};

// Would prefer to define these as friend, but not possible due to 
// BSONValueEmitter not being define at that point in the header file 
template<class T>
mongo::BSONValueEmitter &operator<<( mongo::BSONEmitter &emitter, const T &t ) {
	return emitter.append( t );
}

template<class T>
mongo::BSONEmitter &operator<<( mongo::BSONValueEmitter &emitter, const T &t ) {
	return emitter.append( t );
}

/*
template<class K, class V>
mongo::BSONObjBuilder &operator<<( mongo::BSONObjBuilder &bbuild, 
		const std::pair<K,V> &pair ) { 
	bbuild << pair.first << pair.second;
	return bbuild;
}

template<class K, class V>
mongo::BSONObjBuilder &operator<<( mongo::BSONObjBuilder &bbuild, 
		const std::map<K,V> &map ) { 
	for ( auto pair : map ) {
		bbuild << pair;
	}
	return bbuild;
}

mongo::BSONObjBuilder &operator<<( 
				mongo::BSONObjBuilderValueStream &bbuild, const double d ) { 
	return bbuild << d;
}

template<class T>
mongo::BSONObjBuilder &operator<<( 
				mongo::BSONObjBuilderValueStream &bbuild, const T &t ) { 
	mongo::BSONObjBuilder bob;
	bob << t;
	return bbuild << bob.obj();
}


template<class K, class V>
mongo::BSONObjBuilder &operator<<( mongo::BSONObjBuilderValueStream &bbuild, 
		const std::map<K,V> &map ) { 
	mongo::BSONObjBuilder b;
	for ( auto pair : map )
		b << pair.first << pair.second;
	return bbuild << b.obj();
}

mongo::BSONObjBuilder &operator<<( mongo::BSONObjBuilderValueStream &bbuild, 
		const std::vector<double> &vt ) { 
	mongo::BSONArrayBuilder b;
	for ( auto el : vt ) {
		b << el;
	}
	return bbuild << b.arr();
}

template<class T>
mongo::BSONObjBuilder &operator<<( mongo::BSONObjBuilderValueStream &bbuild, 
		const std::vector<T> &vt ) { 
	mongo::BSONArrayBuilder b;
	for ( T el : vt ) {
		mongo::BSONObjBuilder bob;
		bob << el;
		b << bob.obj();
	}
	return bbuild << b.arr();
}*/
#endif
