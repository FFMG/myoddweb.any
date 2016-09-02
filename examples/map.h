/*
 * vector.h
 *
 *  Created on: Sep 2, 2016
 *      Author: florentg
 *  Sample of using any with maps
 */

#pragma once

#include <map>
#include <assert.h>
#include <iostream>

#include "../any.h"

void SampleMap()
{
  // we need a vector of anys.
  std::map< ::myodd::dynamic::Any, ::myodd::dynamic::Any > myMap;

  // add a few items.
  myMap[1] = "Hello";
  myMap["Something"] = "Else";

  assert( myMap[1] == "Hello" );
  assert( myMap["Something"] == "Else" );

  std::cout << "All maps are good!";
}
