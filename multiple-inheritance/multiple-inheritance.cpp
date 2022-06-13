#include <iostream>
#include <string>


/*

Implementing a rhombus inheritance:

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

    // The only important note to be made here is that if we omit `virtual`
    // inheritance keyword above, we will get two copies of Top member fields bound to Left and Right classes.
    // We will also loose the ability of addressing base_field1 without specifying a base class
}
