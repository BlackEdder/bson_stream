
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

class TestIn : public CxxTest::TestSuite {
	public:

		void testDouble() {
			BSONObjBuilder bbuild;
			bbuild << "a" << 1.0;
			BSONObj bobj = BSONObjBuilder().append("a", 1.0).obj();
			TS_ASSERT_EQUALS( bobj, bbuild.obj() );
		}
};
