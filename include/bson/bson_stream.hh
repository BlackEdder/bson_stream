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
#include<map>
#include "bson/bson.h"


template<class T>
void operator>>( const mongo::BSONElement &bel, T &t ) {
	bel.Val( t );
}

template<class T>
void operator>>( const mongo::BSONObj &bobj, T &t ) {
	// If we have a object we probably want to pipe the object as a whole into 
	// class T. To do that we first turn it into an element (containing an sub object)
	// and then pipe that to our class T. I could not find a simple way of turning an
	// object into an element, so we'll wrap it up and then when we unwrap it we get
	// an element back
	mongo::BSONObjBuilder bbuild;
	bbuild << "a" << bobj;
	bbuild.obj()["a"] >> t;
}

void operator>>( const mongo::BSONElement &bel, double &t ) {
	t = bel.Number();
}

template<class T>
void operator>>( const mongo::BSONElement &bel, std::vector<T> &v ) {
	v.clear();
	auto barr = bel.Array();
	for ( auto & bson_el : barr ) {
		// This will only work if T has an empty constructor T()
		// I am not aware of a more general way of doing this though
		T el;
		bson_el >> el;
		v.push_back( el );
	}
}

template<class T>
void operator>>( const mongo::BSONElement &bel, std::list<T> &v ) {
	v.clear();
	auto barr = bel.Array();
	for ( auto & bson_el : barr ) {
		// This will only work if T has an empty constructor T()
		// I am not aware of a more general way of doing this though
		T el;
		bson_el >> el;
		v.push_back( el );
	}
}

template<class T>
void operator>>( const mongo::BSONElement &bel, std::set<T> &v ) {
	v.clear();
	auto barr = bel.Array();
	for ( auto & bson_el : barr ) {
		// This will only work if T has an empty constructor T()
		// I am not aware of a more general way of doing this though
		T el;
		bson_el >> el;
		v.insert( el );
	}
}

template<class K, class V>
void operator>>( const mongo::BSONObj &bobj, std::map<K,V> &map ) {
	map.clear();
	for ( mongo::BSONObj::iterator i = bobj.begin(); i.more(); ) {
		mongo::BSONElement el = i.next();
		V value;
		el >> value;
		map[el.fieldName()] = value;
	}
}

namespace mongo {
	class BSONEmitter;

	class BSONValueEmitter {
		public:
			BSONValueEmitter( BSONEmitter *pEmitter );

			template<class T>
				BSONEmitter &append( const T &t );

			BSONEmitter &append( const double &t );
			BSONEmitter &append( const long long &t );
			BSONEmitter &append( const bool &t );
			BSONEmitter &append( const int &t );
			BSONEmitter &append( const std::string &t );
			BSONEmitter &append( const BSONArray &t );
			BSONEmitter &append( const BSONObj &t );

			BSONEmitter *pEmitter;
			BSONObjBuilderValueStream builder;
	};

	/**
	 * \brief Define an emitter for BSONObjects
	 *
	 * Behaviour is partly based on BSONObjBuilder, but different enough that
	 * we need a separate class
	 */
	class BSONEmitter {
		public:
			BSONEmitter() : builder( new BSONObjBuilder() ), v_emitter( this ) {}
			BSONEmitter( BSONObjBuilder *builder ) 
				: builder( builder ), v_emitter( this )
			{}

			BSONObj obj() {
				auto bobj = builder->obj();
				// This invalidates builder any way, so we can delete it
				delete builder;
				return bobj;
			}

			BSONValueEmitter &append( const std::string &name ) {
				v_emitter.builder.endField( name.c_str() );
				return v_emitter;
			}

			BSONValueEmitter &append( const char *name ) {
				v_emitter.builder.endField( name );
				return v_emitter;
			}


			BSONObjBuilder *builder;
			BSONValueEmitter v_emitter;
	};

	BSONValueEmitter::BSONValueEmitter( BSONEmitter *pEmitter ) 
		: pEmitter( pEmitter ), builder( pEmitter->builder ) {
		}

	template<class T>
		BSONEmitter &BSONValueEmitter::append( const T &t ) {
			mongo::BSONEmitter b;
			b << t;
			return this->append( b.obj() );
		}

	BSONEmitter &BSONValueEmitter::append( const double &t ) {
		pEmitter->builder = &(builder << t);
		return (*pEmitter);
	}

	BSONEmitter &BSONValueEmitter::append( const long long &t ) {
		pEmitter->builder = &(builder << t);
		return (*pEmitter);
	}

	BSONEmitter &BSONValueEmitter::append( const bool &t ) {
		pEmitter->builder = &(builder << t);
		return (*pEmitter);
	}

	BSONEmitter &BSONValueEmitter::append( const int &t ) {
		pEmitter->builder = &(builder << t);
		return (*pEmitter);
	}

	BSONEmitter &BSONValueEmitter::append( const std::string &t ) {
		pEmitter->builder = &(builder << t);
		return (*pEmitter);
	}

	BSONEmitter &BSONValueEmitter::append( const BSONArray &t ) {
		pEmitter->builder = &(builder << t);
		return (*pEmitter);
	}

	BSONEmitter &BSONValueEmitter::append( const BSONObj &t ) {
		pEmitter->builder = &(builder << t);
		return (*pEmitter);
	}



	class BSONArrayEmitter {
		public:
			BSONArrayEmitter() {}

			template<class T>
				BSONArrayEmitter &append( const T &t ) {
					mongo::BSONEmitter b;
					b << t;
					builder.append( b.obj() ); 
					return *this;
				}

			BSONArrayEmitter &append(	const double &t ) {
				builder.append( t );
				return *this;
			}

			BSONArrayEmitter &append(	const long long &t ) {
				builder.append( t );
				return *this;
			}

			BSONArrayEmitter &append(	const bool &t ) {
				builder.append( t );
				return *this;
			}

			BSONArrayEmitter &append(	const int &t ) {
				builder.append( t );
				return *this;
			}

			BSONArrayEmitter &append(	const std::string &t ) {
				builder.append( t );
				return *this;
			}

			BSONArray arr() {
				return builder.arr();
			}

			BSONArrayBuilder builder;
	};

	template<class K, class V>
		BSONEmitter &operator<<( BSONEmitter &wrap, 
				const std::pair<K,V> &t ) {
			wrap.append( t.first ).append( t.second );
			return wrap;
		}

	template<class K, class V>
		BSONEmitter &operator<<( BSONEmitter &wrap, 
				const std::map<K,V> &t ) {
			for (auto & pair : t)
				wrap << pair;
			return wrap;
		}


};

template<class T>
mongo::BSONValueEmitter &operator<<( mongo::BSONEmitter &emitter, const T &t ) {
	return emitter.append( t );
} 

/*//! Add map easily to an emitter, map keys become keys in the resulting BSONObj
template<class K, class V>
mongo::BSONEmitter &operator<<( mongo::BSONEmitter &emitter, const std::map<K,V> &map ) {
	(*emitter.builder) << map;
	return emitter;
}*/

template<class T>
mongo::BSONEmitter &operator<<( mongo::BSONValueEmitter &emitter, const T &t ) {
	return emitter.append( t );
}

template<class T>
mongo::BSONArrayEmitter &operator<<( mongo::BSONArrayEmitter &barr,
		const T &t ) {
	return barr.append( t );
}


template<class T>
mongo::BSONEmitter &operator<<( mongo::BSONValueEmitter &bbuild, 
		const std::vector<T> &vt ) { 
	mongo::BSONArrayEmitter b;
	for ( const T &el : vt ) {
		b << el;
	}
	return bbuild.append( b.arr() );
}

template<class T>
mongo::BSONEmitter &operator<<( mongo::BSONValueEmitter &bbuild, 
		const std::set<T> &vt ) { 
	mongo::BSONArrayEmitter b;
	for ( const T &el : vt ) {
		b << el;
	}
	return bbuild.append( b.arr() );
}

template<class T>
mongo::BSONEmitter &operator<<( mongo::BSONValueEmitter &bbuild, 
		const std::list<T> &vt ) { 
	mongo::BSONArrayEmitter b;
	for ( const T &el : vt ) {
		b << el;
	}
	return bbuild.append( b.arr() );
}

#endif
