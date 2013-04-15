
#include <cxxtest/TestSuite.h>
#include "bson/bson_stream.hh"
using namespace mongo;

class test {
	public:
		double a;
		double b;
		test() {};
		test( double a, double b ) : a(a), b(b) {}

		friend void operator>>( const BSONElement &bel, test &t ) {
			bel["a"] >> t.a;
			bel["b"] >> t.b;
		}
};

class TestOut : public CxxTest::TestSuite {
	public:
		BSONObj bobj;
		void setUp() {
			std::vector<double> v = {1.1, -2.9};

			BSONObj test_class_bson = BSONObjBuilder().append("a", 2.01).append("b", 3.1 ).obj();
			BSONObj test_class_bson2 = BSONObjBuilder().append("a", -2.01).append("b", 1.1 ).obj();
			BSONArray test_class_vector = BSONArrayBuilder().append( test_class_bson ).append( test_class_bson2 ).arr();

			bobj = BSONObjBuilder().append("a", 2.0).append("b", v)
				.append("test", test_class_bson).append("test_vector", test_class_vector ).obj();

			// bobj now contains:
			// { a: 2.0, b: [ 1.1, -2.9 ], test: { a: 2.01, b: 3.1 }, test_vector: [ { a: 2.01, b: 3.1 }, { a: -2.01, b: 1.1 } ] }
		}
		void testDouble() {
			double a;
			bobj["a"] >> a;
			TS_ASSERT_EQUALS( a, 2.0 );
		}

		void testSimpleVector() {
			std::vector<double> b;
			bobj["b"] >> b;
			TS_ASSERT_EQUALS( b.size(), 2 );

			TS_ASSERT_EQUALS( b[0], 1.1 );
			TS_ASSERT_EQUALS( b[1], -2.9 );
			bobj["b"] >> b;
			TS_ASSERT_EQUALS( b.size(), 2 );
		}

		void testSimpleSet() {
			std::set<double> b;
			bobj["b"] >> b;

			std::set<double> compare = { 1.1, -2.9 };
			TS_ASSERT_EQUALS( b.size(), 2 );
			TS_ASSERT_EQUALS( b, compare );
			bobj["b"] >> b;
			TS_ASSERT_EQUALS( b.size(), 2 );
		}

		void testSimpleList() {
			std::list<double> b;
			bobj["b"] >> b;

			std::list<double> compare = { 1.1, -2.9 };
			TS_ASSERT_EQUALS( b.size(), 2 );
			TS_ASSERT_EQUALS( b, compare );
			bobj["b"] >> b;
			TS_ASSERT_EQUALS( b.size(), 2 );
		}

		void testObjectToMap() {
			BSONObj bobj = BSONObjBuilder().append("a", 2.01).append("b", 3.1 ).obj();
			std::map<std::string, double> mymap;
			bobj >> mymap;
			TS_ASSERT_EQUALS( mymap["a"], 2.01 );
			TS_ASSERT_EQUALS( mymap["b"], 3.1 );

			mymap["c"] = 3.0;
			bobj >> mymap;
			TS_ASSERT_EQUALS( mymap.size(), 2 );
		}

		void testClass() {
			test test_c;
			bobj["test"] >> test_c;
			TS_ASSERT_EQUALS( test_c.a, 2.01 );
			TS_ASSERT_EQUALS( test_c.b, 3.1 );
		}

		void testFirst() {
			std::vector<test> test_vector;
			bobj["test_vector"] >> test_vector;
			TS_ASSERT_EQUALS( test_vector[0].a, 2.01 );
			TS_ASSERT_EQUALS( test_vector[0].b, 3.1 );
			TS_ASSERT_EQUALS( test_vector[1].a, -2.01 );
			TS_ASSERT_EQUALS( test_vector[1].b, 1.1 );
		}
};
