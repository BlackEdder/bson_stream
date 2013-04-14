
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

		friend mongo::BSONObjBuilder &operator<<( 
				mongo::BSONObjBuilderValueStream &bbuild, const test &t ) { 
			mongo::BSONObjBuilder b;
			b << "a" << t.a; 
			b << "b" << t.b;
			return bbuild << b.obj();
		}
};

class TestIn : public CxxTest::TestSuite {
	public:

		void testDouble() {
			BSONObjBuilder bbuild;
			bbuild << "a" << 1.0;
			BSONObj bobj = BSONObjBuilder().append("a", 1.0).obj();
			TS_ASSERT_EQUALS( bobj, bbuild.obj() );
		}

		void testVector() {
			std::vector<double> v = { 1.1, -2.1 };
			BSONObjBuilder bbuild;
			bbuild << "a" << v;
			BSONObj bobj = BSONObjBuilder().append("a", v).obj();
			TS_ASSERT_EQUALS( bobj, bbuild.obj() );
		}

		void testMap() {
			std::map<std::string, double> mymap = {{"a", 1.0}};
			BSONObjBuilder bbuild;
			bbuild << mymap;
			BSONObj bobj = BSONObjBuilder().append("a", 1.0).obj();
			TS_ASSERT_EQUALS( bobj, bbuild.obj() );
		}

		void testMapAsValue() {
			std::map<std::string, double> mymap = {{"a", 1.0}};
			BSONObjBuilder bbuild;
			bbuild << "map" << mymap;
			BSONObj bobj = BSONObjBuilder().append( "map",
				BSONObjBuilder().append("a", 1.0).obj() ).obj();
			TS_ASSERT_EQUALS( bobj, bbuild.obj() );
		}

		void testVectorAsValue() {
			std::vector<double> v = {-1.1, 1.0};
			BSONObjBuilder bbuild;
			bbuild << "a" << v;
			BSONObj bobj = BSONObjBuilder().append( "a", v
				).obj();
			TS_ASSERT_EQUALS( bobj, bbuild.obj() );
		}

		void testTestClassAsValue() {
			test t(-1.1, 1.0);
			BSONObjBuilder bbuild;
			bbuild << "test" << t;
			BSONObj bobj = BSONObjBuilder().append( "test", 
					BSONObjBuilder().append(
						"a", t.a).append( "b", t.b ).obj()).obj();
			TS_ASSERT_EQUALS( bobj, bbuild.obj() );
		}

};
