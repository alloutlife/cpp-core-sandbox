#include <iostream>
#include <string>


/*

Implementing a rhombus(diamond) inheritance:

           Top
            ^
     _______|________
    |                |
  Left             Right
    ^                ^
    |                |
    |________________|
            ^
            |
          Bottom


*/
class Top {
public:

    Top( void ) = default;
    Top( const char* caller_name )
    { std::cout << "Top is called from " << caller_name << std::endl; }

    virtual void meth1( void )
    {
        std::cout << "Top::meth1" << std::endl;
    }

protected:
    std::string base_field1;
};

class Left : virtual public Top
{
public:
    Left()
        : Top{ "left" }         // This will be ignored!
    {
        base_field1 = "L";
    }
    void meth1( void ) override
    {
        std::cout << "Left::meth1" << std::endl;
    }
};


class Right : virtual public Top
{
public:
    Right()
        : Top{ "right" }         // This will be ignored!
    {
        base_field1 = "R";
    }
    void meth1( void ) override
    {
        std::cout << "Right::meth1" << std::endl;
    }
};


class bottom : public Left, public Right
{
public:
    bottom()
        : Top{ "bottom" }         // This will be called sinse bottom is the final inheritor
    {
    }

    void meth1( void ) override
    {
        if( Left::base_field1 != Right::base_field1 ) {
            std::cout << "Left::base_field1 and Right::base_field1 have different values!" << std::endl;
        } else {
            std::cout << "Left::base_field1 equals to Right::base_field1. Virtual inheritance is probably in action" << std::endl;
        }
    }
};

int main( void )
{
    bottom b;
    b.meth1();

    dynamic_cast< Left* >( &b )->meth1();
    dynamic_cast< Right* >( &b )->meth1();

    // The first important note to be made here is that if we omit `virtual`
    // inheritance keyword above, we will get two copies of Top member fields bound to Left and Right classes.
    // We will also loose the ability of addressing base_field1 without specifying a base class

    // The second important notice is that if we had invoked the constructor for `A` inside the inheritor classes, it would be only invoked in constructor of `Bottom`
    // and ignored in `Left` and `Right`.

    return 0;
}
