# Installation 
bson_stream uses cmake for building tests. You can install it by running
```
cmake .
make install
```

Alternatively, you can just copy the header file (bson_stream.hh) to a location of your choosing.

# Examples

Usage of this library is very similar to yaml-cpp and based on the official mongodb c++ driver. See below for some examples.

A very simple example, piping to an BSONObj and back into a double

```C++
    #include "bson/bson_stream.hh"

    int main() {
        mongo::BSONEmitter emit;
        emit << "a" << 1.0;
        mongo::BSONObj obj = emit.obj();

        double d;
        obj["a"] >> d;
        if (d==1.0)
            std::cout << "Success!" << std::endl;
        else
            std::cout << "Wrong!" << std::endl;
        return EXIT_SUCCESS;
    }
```

To overload user defined classes you only have to define two helper function:
```C++
    #include "bson/bson_stream.hh"

    class test {
        public:
            double a, b;
            test() : a(1.1), b(2.1) {}
            friend void operator>>( const mongo::BSONElement &el, test &t ) {
                el["a"] >> t.a;
                el["b"] >> t.b;
            }

            friend mongo::BSONEmitter &operator<<( mongo::BSONEmitter &emit, const test &t ) {
                emit << "a" << t.a;
                emit << "b" << t.b;
                return emit;
            }
    };

    int main() {
        test t;
        t.a = 1.4; t.b = -1.1;
        mongo::BSONEmitter emit;
        emit << t;
        mongo::BSONObj obj = emit.obj();
        obj >> t;
        if (t.a==1.4 && t.b == -1.1)
            std::cout << "Success!" << std::endl;
        else
            std::cout << "Wrong!" << std::endl;

        // We can also add the test class as a value:
        mongo::BSONEmitter emit2;
        emit2 << "a" << t;
        obj = emit2.obj();
        obj["a"] >> t;
        if (t.a==1.4 && t.b == -1.1)
            std::cout << "Success!" << std::endl;
        else
           std::cout << "Wrong!" << std::endl;

        // Or a vector of tests
        mongo::BSONEmitter emit3;
        std::vector<test> vt;
        vt.push_back( t );
        emit3 << "a" << vt;
        obj = emit3.obj();
        std::vector<test> vt2;
        obj["a"] >> vt;
        t = vt[0];
        if (t.a==1.4 && t.b == -1.1)
             std::cout << "Success!" << std::endl;
        else
            std::cout << "Wrong!" << std::endl;
        return EXIT_SUCCESS;
    }
```
