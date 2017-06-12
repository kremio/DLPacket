/*
 Unit tests for the DLPacket library.
 
 Created by Jonathan Cremieux<kremio.software@gmail.com> for Design Research Lab<drlab.org> on 12/06/2017.
 */
#include "lest.hpp"
#include "Arduino.h"
#include "Stream.h"
#include "DLPacket.cpp"

#include <iostream>
using namespace std;

const lest::test specification[] =
{
  CASE( "Empty string has length zero (succeed)" )
    {
        EXPECT( 1 == string(  ).length() );
        EXPECT( 0 == string("").length() );
    },
};

int main( int argc, char * argv[] )
{
    return lest::run( specification, argc, argv );
}
