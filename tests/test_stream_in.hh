
#include <cxxtest/TestSuite.h>
#include "bson/bson_stream.hh"

//using namespace mongo;

class test {
	public:
		double a;
		double b;
		test() {};
		test( double a, double b ) : a(a), b(b) {}

		friend void operator>>( const mongo::BSONElement &bel, test &t ) {
			bel["a"] >> t.a;
			bel["b"] >> t.b;
		} 

		friend mongo::BSONEmitter &operator<<( 
				mongo::BSONEmitter &bbuild, const test &t ) { 
			bbuild << "a" << t.a; 
			bbuild << "b" << t.b;
			return bbuild;
		}
};

class test2 {
	public:
		std::vector<test> test_vector;
		std::vector<double> double_vector;
		test2() 
			: test_vector( { test( 1.0, 0.1 ) } ), double_vector( { 1.0, 0.1 } ) {}

		friend void operator>>( const mongo::BSONElement &bel, test2 &t ) {
			bel["test_vector"] >> t.test_vector;
			bel["double_vector"] >> t.double_vector;
		} 


		friend mongo::BSONEmitter &operator<<( 
				mongo::BSONEmitter &bbuild, const test2 &t ) { 
			bbuild << "double_vector" << t.double_vector; 
			bbuild << "test_vector" << t.test_vector;
			return bbuild;
		}

};

class TestIn : public CxxTest::TestSuite {
	public:

		template<class T>
		void helpTypes( const T &t ) {
			mongo::BSONEmitter bbuild;
			bbuild << "a" << t;
			mongo::BSONObj bobj = mongo::BSONObjBuilder().append("a", t).obj();
			TS_ASSERT_EQUALS( bobj, bbuild.obj() );
		}

		void testDouble() {
			mongo::BSONEmitter bbuild;
			bbuild << "a" << 1.0;
			mongo::BSONObj bobj = mongo::BSONObjBuilder().append("a", 1.0).obj();
			TS_ASSERT_EQUALS( bobj, bbuild.obj() );
		}

		void testTypes() {
			long long v = 1;
			helpTypes( v );
			bool b = true;
			helpTypes( b );
			int i = 2;
			helpTypes( i );
			double d = 2.1;
			helpTypes( d );
			std::string s = "Hello world!";
			helpTypes( s );
		}

		void testTwoDouble() {
			mongo::BSONEmitter bbuild;
			bbuild << "a" << 1.0 << "b" << 2.1;
			mongo::BSONObj bobj = mongo::BSONObjBuilder().append("a", 1.0).append("b", 2.1).obj();
			TS_ASSERT_EQUALS( bobj, bbuild.obj() );
		}

		void testMemoryManagement() {
			mongo::BSONEmitter bbuild;
			bbuild << "a" << 1.0 << "b" << 2.1;
			auto bobj = bbuild.obj();
			std::cout << bobj << std::endl; // This will crash if something went wrong
			std::cout << bobj << std::endl; // This will crash if something went wrong
		}

		void testVector() {
			std::vector<double> v = { 1.1, -2.1 };
			mongo::BSONEmitter bbuild;
			bbuild << "a" << v;
			mongo::BSONObj bobj = mongo::BSONObjBuilder().append("a", v).obj();
			TS_ASSERT_EQUALS( bobj, bbuild.obj() );
		}

		void testMap() {
			std::map<std::string, double> mymap = {{"a", 1.0}};
			mongo::BSONEmitter bbuild;
			bbuild << mymap;
			mongo::BSONObj bobj = mongo::BSONObjBuilder().append("a", 1.0).obj();
			TS_ASSERT_EQUALS( bobj, bbuild.obj() );
		}

		void testMapAsValue() {
			std::map<std::string, double> mymap = {{"a", 1.0}};
			mongo::BSONEmitter bbuild;
			bbuild << "map" << mymap;
			auto obj = bbuild.obj();
			mongo::BSONObj bobj = mongo::BSONObjBuilder().append( "map",
				mongo::BSONArrayBuilder().append(
					mongo::BSONArrayBuilder().append("a").append(1.0).arr()).arr() ).obj();
			TS_ASSERT_EQUALS( bobj, obj );
		}

		void testVectorMapAsValue() {
			std::vector<std::map<std::string, double> > mymap = {{{"a", 1.0}}};
			mongo::BSONEmitter bbuild;
			bbuild << "map" << mymap;
			auto obj = bbuild.obj();
			std::cout << obj << std::endl;
			mongo::BSONObj bobj = mongo::BSONObjBuilder().append( "map",
					mongo::BSONArrayBuilder()
					.append( 
						mongo::BSONObjBuilder().append("a", 1.0).obj()).arr()
					).obj();
			TS_ASSERT_EQUALS( bobj, obj );
		}


		void testVectorAsValue() {
			std::vector<double> vd = {-1.1, 1.0};
			helpTypes( vd );
			std::vector<long long> vl = {1, 2};
			helpTypes( vl );

			std::vector<bool> vb = { true, true };
			helpTypes( vb );
			std::vector<int> vi = {1, 2};
			helpTypes( vi );
			std::vector<std::string> vs = { "Hello world!", "Bla" };
			helpTypes( vs );
		}

		void testSetAsValue() {
			std::set<std::string> vs = { "Hello world!", "Bla" };
			helpTypes( vs );
		}

		void testListAsValue() {
			std::list<std::string> vs = { "Hello world!", "Bla" };
			helpTypes( vs );
		}


		void testTestClassAsValue() {
			test t(-1.1, 1.0);
			mongo::BSONEmitter bbuild;
			bbuild << "test" << t;
			auto obj = bbuild.obj();
			mongo::BSONObj bobj = mongo::BSONObjBuilder().append( "test", 
					mongo::BSONObjBuilder().append(
						"a", t.a).append( "b", t.b ).obj()).obj();
			TS_ASSERT_EQUALS( bobj, obj );
		}

		void testTestClassAsVector() {
			std::vector<test> vt = { test(-1.1, 1.0), test( -2.0, -1.1 ) };
			mongo::BSONEmitter bbuild;
			bbuild << "test" << vt;
			auto obj = bbuild.obj();
			mongo::BSONObj bobj = mongo::BSONObjBuilder().append( "test", 
					mongo::BSONArrayBuilder()
					.append( 
						mongo::BSONObjBuilder().append( "a", vt[0].a).append( "b", vt[0].b ).obj())
					.append( 
						mongo::BSONObjBuilder().append( "a", vt[1].a).append( "b", vt[1].b ).obj())
					.arr()).obj();
			TS_ASSERT_EQUALS( bobj, obj );
		}
		

		// The following tests just stream into and then out of bson to check
		// if we get the same back as we put into (above tests all specifically
		// test the bson itself that is the step in the middle
		void testTestClassTest2() {
			mongo::BSONEmitter bbuild;
			test2 t = test2();
			bbuild << "test2" << t;
			auto obj = bbuild.obj();
			test2 t2;
			t2.test_vector.push_back( test(0,0) );
			TS_ASSERT_DIFFERS( t2.test_vector.size(), t.test_vector.size() );
			obj["test2"] >> t2;
			TS_ASSERT_EQUALS( t2.test_vector.size(), t.test_vector.size() );
		}

		void testNonStringPair() {
			mongo::BSONEmitter bbuild;
			std::pair< int, double > pair = {1, 2.1};
			bbuild << "pair" << pair;
			auto obj = bbuild.obj();
			std::pair< int, double > pair_cpy = {0, 0};
			TS_ASSERT_DIFFERS( pair, pair_cpy );
			obj["pair"] >> pair_cpy;
			TS_ASSERT_EQUALS( pair, pair_cpy );
		}

		void testNonStringMap() {
			mongo::BSONEmitter bbuild;
			std::map< int, double > map = {{1, 2.1},{2,3.1}};
			bbuild << "map" << map;
			auto obj = bbuild.obj();
			std::cout << obj << std::endl;
			std::map< int, double > map_cpy = {{0, 0}};
			TS_ASSERT_DIFFERS( map, map_cpy );
			obj["map"] >> map_cpy;
			TS_ASSERT_EQUALS( map, map_cpy );
		}
};
