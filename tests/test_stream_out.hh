
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
		void testFirst() {
			std::vector<double> v = {1.1, -2.9};

			BSONObj test_class_bson = BSONObjBuilder().append("a", 2.01).append("b", 3.1 ).obj();
			BSONObj test_class_bson2 = BSONObjBuilder().append("a", -2.01).append("b", 1.1 ).obj();
			BSONArray test_class_vector = BSONArrayBuilder().append( test_class_bson ).append( test_class_bson2 ).arr();

			BSONObj bobj = BSONObjBuilder().append("a", 2.0).append("b", v)
				.append("test", test_class_bson).append("test_vector", test_class_vector ).obj();
			std::cout << "The bson to test: " << bobj << std::endl;

			double a;
			bobj["a"] >> a;
			std::cout << "a: " << a << std::endl;

			std::vector<double> b;
			bobj["b"] >> b;
			std::cout << "b: " << "[ ";
			for ( auto el : b )
				std::cout << el << ", ";
			std::cout << "]" << std::endl;

			test test_c;
			bobj["test"] >> test_c;
			std::cout << "Test object: " << test_c.a << " " << test_c.b << std::endl;

			std::vector<test> test_vector;
			bobj["test_vector"] >> test_vector;
		}
	

};
