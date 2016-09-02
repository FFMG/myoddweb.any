/*
 * vector.h
 *
 *  Created on: Sep 2, 2016
 *      Author: florentg
 *  Sample of using any with vectors
 */

#pragma once

#include <vector>
#include <assert.h>
#include <iostream>

#include "../any.h"

void SampleVector()
{
  // we need a vector of anys.
  std::vector< ::myodd::dynamic::Any > myVector;

  // add a few items.
  myVector.push_back( "Hello" ); // a char*
  myVector.push_back( L"World" ); // a wide char*
  myVector.push_back( 42 ); // an int
  myVector.push_back( -12.345 ); // a double

  // check some values
  assert( myVector[0] == "Hello" );
  assert( myVector[1] == L"World" );
  assert( myVector[2] == 42 );

  std::cout << "All vectors are good!";
}
