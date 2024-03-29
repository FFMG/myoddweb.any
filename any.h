// ***********************************************************************
// Copyright (c) 2016-2022 Florent Guelfucci
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// @see https://opensource.org/licenses/MIT
// ***********************************************************************
#pragma once

// string representation of the version number
#define MYODD_ANY_VERSION        "0.1.17"

// the version number is #.###.###
// first number is major
// then 3 numbers for minor
// and 3 numbers for tiny
#define MYODD_ANY_VERSION_NUMBER 0001017

#if defined(_MSC_VER)
#   if _MSC_VER < 1800 
#       error This project needs atleast Visual Studio 2013
#   endif
// 199711L - c++98
// 201103L - c++11
// 201300L - c++14
#elif __cplusplus <= 201103L
#   error This project can only be compiled with a compiler that supports C++14
#endif

/* What version of GCC is being used.  0 means GCC is not being used */
/* from sqlite 3*/
#ifdef __GNUC__
# define GCC_VERSION (__GNUC__*1000000+__GNUC_MINOR__*1000+__GNUC_PATCHLEVEL__)
#else
# define GCC_VERSION 0
#endif

#include <typeinfo>       // std::bad_cast
#include <algorithm>      // memcpy
#include <math.h>         // modf
#include <cstring>
#include <string>
#include <locale>		      //  std::wstring_convert
#include <cctype>         //  isdigit
#include <codecvt>        //  string <-> wstring
#include <stdlib.h>       //  std::strtoll / std::strtoull
#include <type_traits>    //  std::is_trivially_copyable
                          //  std::is_pointer
#include <cstddef>        //  nullptr_t
#include <memory>         //  std::unique_ptr

#include "types.h"        // data type
#include <iostream>       // std::cout, std::right, std::endl

namespace myodd {
  namespace _Check
  {
    struct No {};
    template<typename T, typename Arg> No operator== (const T&, const Arg&);

    template<typename T, typename Arg = T>
    struct EqualExists
    {
      enum { value = !std::is_same<decltype(*(T*)(0) == *(Arg*)(0)), No>::value };
    };
  }

  namespace dynamic {
    class Any
    {
    private:
      /**
      * This is the type of comparaison we ar doing.
      */
      enum CompareType {
        CompareType_Equal,
        CompareType_LessThan,
        CompareType_MoreThan
      };

      // the string status, does it represent a number? a floating number?
      // is it a partial or non partial number?
      enum StringStatus {
        StringStatus_Not_A_Number,                    // 'blah' or 'blah123'
        StringStatus_Partial_Pos_Number,              // '+123blah'
        StringStatus_Partial_Neg_Number,              // '-123blah'

        StringStatus_Pos_Number,                      // '+123' or '123' or '0' or '-0'
        StringStatus_Neg_Number,                      // '-123'

        StringStatus_Floating_Partial_Pos_Number,    // '+123.00blah' or '123.00blah'
        StringStatus_Floating_Partial_Neg_Number,    // '-123.00blah'
        StringStatus_Floating_Pos_Number,            // '+123.1' or '123.1' or '0.1' or '-0.1'
        StringStatus_Floating_Neg_Number,            // '-123.1'
      };

    public:
      /**
      * default constructor.
      */
      Any() :
        _unkvalue(nullptr),
        _llivalue(0),
        _ldvalue(0),
        _cvalue(nullptr),
        _lcvalue(0),
        _stringStatus(StringStatus_Not_A_Number),
        _svalue(nullptr),
        _swvalue(nullptr),
        _type(Type::Misc_null)
      {
      }

      /**
      * Copy constructor
      * @see CreateFrom(T)
      * @param T value the value we want to copy/set
      */
      template<class T>
      Any(const T& value) :
        Any()
      {
        CreateFrom(value);
      }

      template<class T>
      Any(T source, size_t num ) :
        Any()
      {
        CreateFromCharacters(source, num );
      }

      /**
      * Copy constructor
      * @param const Any& any the value we are copying.
      */
      Any(const Any& any) :
        Any()
      {
        *this = any;
      }

      /**
      * Destructor.
      */
      virtual ~Any()
      {
        CleanValues();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      template<class T>
      operator T() const {
        return CastToCopy<T>();
      }

      template<class T>
      operator T*()  const
      {
        return CastToCopyPtr<T*>();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator bool() const 
      { 
        // cast *this to value
        return CastToBool();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator float() const
      {
        // cast *this to value
        return CastToFloat();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator double() const
      {
        // cast *this to value
        return CastToDouble();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator long double() const
      {
        // cast *this to value
        return CastToLongDouble();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator int() const
      {
        // cast *this to value
        return CastToInt();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator short int() const
      {
        // cast *this to value
        return CastToShortInt();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator unsigned short int() const
      {
        // cast *this to value
        return CastToUnsignedShortInt();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator unsigned int() const
      {
        // cast *this to value
        return CastToUnsignedInt();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator long int() const
      {
        // cast *this to value
        return CastToLongInt();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator unsigned long int() const
      {
        // cast *this to value
        return CastToUnsignedLongInt();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator long long int() const
      {
        // cast *this to value
        return CastToLongLongInt();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator unsigned long long int() const
      {
        // cast *this to value
        return CastToUnsignedLongLongInt();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator char*() const
      {
        // cast *this to value
        return CastToChars();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator const char*() const
      {
        // cast *this to value
        return CastToChars();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator std::string() const
      {
        // cast *this to value
        return CastToChars();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator char() const
      {
        // cast *this to value
        return CastToChar();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator const signed char*() const
      {
        // cast *this to value
        return CastToSignedChars();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator signed char*() const
      {
        // cast *this to value
        return CastToSignedChars();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator signed char() const
      {
        // cast *this to value
        return CastToSignedChar();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator unsigned char*() const
      {
        // cast *this to value
        return CastToUnsignedChars();
      }
      
      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator const unsigned char*() const
      {
        // cast *this to value
        return CastToUnsignedChars();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator unsigned char() const
      {
        // cast *this to value
        return CastToUnsignedChar();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator const wchar_t*() const
      {
        // cast *this to value
        return CastToWideChars();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator std::wstring() const
      {
        // cast *this to value
        return CastToWideChars();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator wchar_t*() const
      {
        // cast *this to value
        return CastToWideChars();
      }

      /**
      * The T operator, cast a value to T
      * @see CastTo
      * @return T the template operator.
      */
      operator wchar_t() const
      {
        // cast *this to value
        return CastToWideChar();
      }

      friend std::ostream& operator<< (std::ostream& stream, const Any& any)
      {
        // write obj to stream
        if ( dynamic::is_type_copy( any.Type() ))
        {
          stream << "Copy Value";
        }
        else
        {
          try
          {
            std::string value;
            any.CastToCharacters(value);
            stream << value.c_str();
          }
          catch (...)
          {
            // no idea how to display this.
            stream << "NAN";
          }
        }
        return stream;
      }

      /**
      * The logical negation operator.
      * @return false if the current value can be represented as true
      */
      bool operator !() const
      {
        return !((bool)*this);
      }

      /**
      * The equal operator
      * @param const Any& other the value we are trying to set.
      * @return const Any& this value.
      */
      const Any& operator = (const Any& other)
      {
        if (this != &other)
        {
          // clear everything
          CleanValues();

          // copy the values over.
          _type = other._type;

          // long long int
          _llivalue = other._llivalue;

          // long double
          _ldvalue = other._ldvalue;

          // copy the unknown type
          if (other._unkvalue)
          {
            ++other._unkvalue->_counter;
            _unkvalue = other._unkvalue;
          }
          
          // copy the character value
          if (other._lcvalue > 0 && other._cvalue)
          {
            _lcvalue = other._lcvalue;
            _cvalue = new char[_lcvalue];
            std::memset(_cvalue, '\0', _lcvalue);
            std::memcpy(_cvalue, other._cvalue, _lcvalue);
            _stringStatus = other._stringStatus;
          }
        }
        return *this;
      }

      /**
      * The equal operator
      * @param const Any &other the value we are comparing
      * @return bool if the values are equal
      */
      bool operator==(const Any& other) const { return Compare(*this, other, CompareType_Equal ); }

      /**
      * The friend equal operator.
      * @param const Any &other the value we are comparing
      * @return bool if the values are equal
      */
      template<class T> friend bool operator==(const T& lhs, const Any& rhs){ return Compare(Any(lhs), rhs, CompareType_Equal); }

      /**
      * The friend equal operator.
      * @param const Any &lhs the value we are comparing
      * @param const T &rhs the rhs value we are comparing.
      * @return bool if the values are equal
      */
      template<class T> friend bool operator==(const Any& lhs, const T& rhs){ return Compare(lhs, Any(rhs), CompareType_Equal); }

      /**
      * The not equal operator
      * @param const Any &other the value we are comparing
      * @return bool if the values are _not_ equal
      */
      bool operator!=(const Any &other) const{ return !Compare(*this, other, CompareType_Equal); }

      /**
      * The friend equal operator.
      * @param const Any &other the value we are comparing
      * @return bool if the values are equal
      */
      template<class T> friend bool operator!=(const T& lhs, const Any& rhs){ return !Compare(Any(lhs), rhs, CompareType_Equal); }

      /**
      * The friend equal operator.
      * @param const Any &other the value we are comparing
      * @return bool if the values are equal
      */
      template<class T> friend bool operator!=(const Any& lhs, const T& rhs){ return !Compare(lhs, Any(rhs), CompareType_Equal); }

      /**
      * Relational operator less than
      * @param const Any& rhs
      * @return bool if *this < rhs
      */
      bool operator< (const Any& rhs) const { return Compare(*this, rhs, CompareType_LessThan ); }

      /**
       * Relational operator less than
       * @param const Any& lhs
       * @param const T& rhs
       * @return bool if lhs < rhs
       */
      template<class T> friend bool operator< (const Any& lhs, const T& rhs ){ return Compare(lhs, Any(rhs), CompareType_LessThan); }

      /**
      * Relational operator less than
      * @param const T& lhs
      * @param const Any& rhs
      * @return bool if lhs < rhs
      */
      template<class T> friend bool operator< (const T& lhs, const Any& rhs) { return Compare(Any(lhs), rhs, CompareType_LessThan); }

      /**
      * Relational operator greater than
      * @param const Any& rhs
      * @return bool if lhs > rhs
      */
      bool operator> (const Any& rhs) const { return Compare(*this, rhs, CompareType_MoreThan); }

      /**
      * Relational operator greater than
      * @param const T& lhs
      * @param const Any& rhs
      * @return bool if lhs > rhs
      */
      template<class T>  friend bool operator> (const T& lhs, const Any& rhs) { return Compare(Any(lhs), rhs, CompareType_MoreThan); }

      /**
      * Relational operator greater than
      * @param const T& lhs
      * @param const Any& rhs
      * @return bool if lhs > rhs
      */
      template<class T>  friend bool operator> (const Any& lhs, const T& rhs) { return Compare(lhs, Any(rhs), CompareType_MoreThan); }

      /**
      * Relational operator less or equal than
      * @param const Any& rhs
      * @return bool if lhs <= rhs
      */
      bool operator<=(const Any& rhs) const { return !(*this > rhs); }

      /**
      * Relational operator less or equal than
      * @param const T& lhs
      * @param const Any& rhs
      * @return bool if lhs <= rhs
      */
      template<class T> friend bool operator<=(const T& lhs, const Any& rhs) { return !(Any(lhs) > rhs); }

      /**
      * Relational operator less or equal than
      * @param const Any& lhs
      * @param const T& rhs
      * @return bool if lhs <= rhs
      */
      template<class T> friend bool operator<=(const Any& lhs, const T& rhs) { return !(lhs > Any(rhs)); }

      /**
      * Relational operator more or equal than
      * @param const Any& rhs
      * @return bool if lhs >= rhs
      */
      bool operator>=(const Any& rhs) const { return !(*this < rhs); }

      /**
      * Relational operator more or equal than
      * @param const T& lhs
      * @param const Any& rhs
      * @return bool if lhs >= rhs
      */
      template<class T> friend bool operator>=(const T& lhs, const Any& rhs) { return !(Any(lhs) < rhs); }

      /**
      * Relational operator more or equal than
      * @param const T& lhs
      * @param const Any& rhs
      * @return bool if lhs >= rhs
      */
      template<class T> friend bool operator>=(const Any& lhs, const T& rhs) { return !(lhs < Any(rhs)); }

      //
      // +operators
      //  
#ifdef _MSC_VER
# pragma region
#endif
      /**
      * Binary arithmetic operators - addition
      * @param const Any& the item we are adding to this.
      * @return Any& *this+rhs
      */
      Any& operator+=(const Any& rhs)
      {
        if (rhs.UseUnsignedInteger())
        {
          return AddNumber(CalculateType(*this, rhs), (unsigned long long int)rhs._llivalue);
        }
        if (rhs.UseSignedInteger())
        {
          return AddNumber(CalculateType(*this, rhs), (long long int)rhs._llivalue);
        }
        return AddNumber(CalculateType(*this, rhs), rhs._ldvalue);
      }

      /**
      * Default += function add the rhs to *this.
      * @param const T& rhs the value we are adding to *this
      * @param *this + rhs.
      */
      template<class T>
      Any& operator+=(T rhs)
      {
        *this += Any(rhs);
        return *this;
      }

      /**
      * Specialized += function add the rhs to *this.
      * @param float rhs the value we are adding to *this
      * @param *this + rhs.
      */
      Any& operator+=(long double rhs)
      {
        return AddNumber(CalculateType(*this, dynamic::Floating_point_long_double), rhs);
      }

      /**
      * Binary arithmetic operators - addition
      * @param const Any& the item we are adding to this.
      * @return Any *this+rhs
      */
      Any operator+(const Any& rhs) const
      {
        // copy the value
        Any value = *this;

        // add the rhs
        value += rhs;

        // return the result.
        return value;
      }

      /**
      * Binary arithmetic operators - addition
      * @param Any the item we are adding to this.
      * @param const Any& the item we are adding to this.
      * @return Any *this+rhs
      */
      template<class T> friend Any operator+(const Any& lhs, const T& rhs) { auto tmp = lhs; tmp += rhs; return tmp; }

      /**
      * Binary arithmetic operators - addition
      * @param Any the item we are adding to this.
      * @param const Any& the item we are adding to this.
      * @return Any *this+rhs
      */
      template<class T> friend Any operator+(const T& lhs, const Any& rhs) { auto tmp = Any(lhs); tmp += rhs; return tmp; }

      /**
      * Add one to the current value.
      * @return Any *this +1
      */
      Any& operator++()
      {
        // save the current type.
        dynamic::Type type = NumberType();

        // add an int.
        if (dynamic::is_type_floating(type))
        {
          // we cannot call ++_ldvalue as the value is passed by reference.
          // to CreateFrom( cons T& ) and the first thing we do is clear the value
          // so _ldvalue/_llivalue become 0
          CreateFrom(_ldvalue + 1);
        }
        else
        {
          // we cannot call ++_ldvalue as the value is passed by reference.
          // to CreateFrom( cons T& ) and the first thing we do is clear the value
          // so _ldvalue/_llivalue become 0
          CreateFrom(_llivalue + 1);
        }

        // update the type.
        _type = CalculateType(type, dynamic::Integer_int);

        // return this.
        return *this;
      }

      /**
      * Add one to the current value.
      * @return Any *this +1
      */
      Any operator++(int)
      {
        Any tmp(*this);
        operator++();
        return tmp;
      }

      //
      // +operators
      //
#ifdef _MSC_VER
# pragma endregion +=operators
#endif

      //
      // -operators
      //  
#ifdef _MSC_VER
# pragma region
#endif
      /**
      * Binary arithmetic operators - subtraction
      * @param const Any& the item we are subtracting to this.
      * @return Any& *this-rhs
      */
      Any& operator-=(const Any& rhs)
      {
        if (rhs.UseUnsignedInteger())
        {
          return SubtractNumber(CalculateType(*this, rhs), (unsigned long long int)rhs._llivalue);
        }

        if (rhs.UseSignedInteger())
        {
          return SubtractNumber(CalculateType(*this, rhs), (long long int)rhs._llivalue);
        }
        return SubtractNumber(CalculateType(*this, rhs), rhs._ldvalue);
      }

      /**
      * Default -= function substract the rhs to *this.
      * @param const T& rhs the value we are subtracting from *this
      * @param *this - rhs.
      */
      template<class T>
      Any& operator-=(T rhs)
      {
        *this -= Any(rhs);
        return *this;
      }

      /**
      * Specialized -= function substract the rhs to *this.
      * @param int rhs the value we are subtracting from *this
      * @param *this - rhs.
      */
      Any& operator-=(int rhs)
      {
        return SubtractNumber(CalculateType(*this, dynamic::Integer_int), rhs);
      }

      /**
      * Specialized -= function substract the rhs to *this.
      * @param short rhs the value we are subtracting from *this
      * @param *this - rhs.
      */
      Any& operator-=(short int rhs)
      {
        return SubtractNumber(CalculateType(*this, dynamic::Integer_short_int), rhs);
      }

      /**
      * Specialized -= function substract the rhs to *this.
      * @param unsigned short int rhs the value we are subtracting from *this
      * @param *this - rhs.
      */
      Any& operator-=(unsigned short int rhs)
      {
        return SubtractNumber(CalculateType(*this, dynamic::Integer_unsigned_short_int), rhs);
      }

      /**
      * Specialized -= function substract the rhs to *this.
      * @param unsigned int rhs the value we are subtracting from *this
      * @param *this - rhs.
      */
      Any& operator-=(unsigned int rhs)
      {
        return SubtractNumber(CalculateType(*this, dynamic::Integer_unsigned_int), rhs);
      }

      /**
      * Specialized -= function substract the rhs to *this.
      * @param long int rhs the value we are subtracting from *this
      * @param *this - rhs.
      */
      Any& operator-=(long int rhs)
      {
        return SubtractNumber(CalculateType(*this, dynamic::Integer_long_int), rhs);
      }

      /**
      * Specialized -= function substract the rhs to *this.
      * @param unsigned long int rhs the value we are subtracting from *this
      * @param *this - rhs.
      */
      Any& operator-=(unsigned long int rhs)
      {
        return SubtractNumber(CalculateType(*this, dynamic::Integer_unsigned_long_int), rhs);
      }

      /**
      * Specialized -= function substract the rhs to *this.
      * @param long long int rhs the value we are subtracting from *this
      * @param *this - rhs.
      */
      Any& operator-=(long long int rhs)
      {
        return SubtractNumber(CalculateType(*this, dynamic::Integer_long_long_int), rhs);
      }

      /**
      * Specialized -= function substract the rhs to *this.
      * @param unsigned long long int rhs the value we are subtracting from *this
      * @param *this - rhs.
      */
      Any& operator-=(unsigned long long int rhs)
      {
        return SubtractNumber(CalculateType(*this, dynamic::Integer_unsigned_long_long_int), rhs);
      }

      /**
      * Specialized -= function substract the rhs to *this.
      * @param float rhs the value we are subtracting from *this
      * @param *this - rhs.
      */
      Any& operator-=(float rhs)
      {
        return SubtractNumber(CalculateType(*this, dynamic::Floating_point_float), rhs);
      }

      /**
      * Specialized -= function substract the rhs to *this.
      * @param double rhs the value we are subtracting from *this
      * @param *this - rhs.
      */
      Any& operator-=(double rhs)
      {
        return SubtractNumber(CalculateType(*this, dynamic::Floating_point_double), rhs);
      }

      /**
      * Specialized -= function substract the rhs to *this.
      * @param float rhs the value we are subtracting from *this
      * @param *this - rhs.
      */
      Any& operator-=(long double rhs)
      {
        return SubtractNumber(CalculateType(*this, dynamic::Floating_point_long_double), rhs);
      }

      /**
      * Binary arithmetic operators - substraction
      * @param const Any& the item we are subtracting from *this.
      * @return Any *this-rhs
      */
      Any operator-(const Any& rhs) const
      {
        // copy the value
        Any value = *this;

        // substract the rhs
        value -= rhs;

        // return the result.
        return value;
      }

      /**
      * Binary arithmetic operators - substraction
      * @param Any the item we are subtracting from *this.
      * @param const Any& the item we are subtracting from *this.
      * @return Any *this-rhs
      */
      template<class T> friend Any operator-( const Any& lhs, const T& rhs) { auto tmp = lhs; tmp -= rhs; return tmp; }

      /**
      * Binary arithmetic operators - substraction
      * @param Any the item we are subtracting from *this.
      * @param const Any& the item we are subtracting from *this.
      * @return Any *this-rhs
      */
      template<class T> friend Any operator-(const T& lhs, const Any& rhs) { auto tmp = Any(lhs); tmp -= rhs; return tmp; }

      /**
      * substract one from the current value.
      * @return Any *this -1
      */
      Any& operator--()
      {
        // save the current type.
        dynamic::Type type = NumberType();

        // substract an int
        // add an int.
        if (dynamic::is_type_floating(type))
        {
          // we cannot call ++_ldvalue as the value is passed by reference.
          // to CreateFrom( cons T& ) and the first thing we do is clear the value
          // so _ldvalue/_llivalue become 0
          CreateFrom(_ldvalue - 1);
        }
        else
        {
          // we cannot call ++_ldvalue as the value is passed by reference.
          // to CreateFrom( cons T& ) and the first thing we do is clear the value
          // so _ldvalue/_llivalue become 0
          CreateFrom(_llivalue - 1);
        }

        // update the type.
        _type = CalculateType(type, dynamic::Integer_int);

        // return this.
        return *this;
      }

      /**
      * substract one from the current value.
      * @return Any *this -1
      */
      Any operator--(int)
      {
        Any tmp(*this);
        operator--();
        return tmp;
      }

      //
      // -operators
      //
#ifdef _MSC_VER
# pragma endregion -=operators
#endif

      //
      // *operators
      //  
#ifdef _MSC_VER
# pragma region
#endif
      /**
      * Binary arithmetic operators - multiplication
      * @param const Any& the item we are multiplying from this.
      * @return Any& *this*rhs
      */
      Any& operator*=(const Any& rhs)
      {
        if (dynamic::is_type_floating(rhs.NumberType()))
        {
          return MultiplyNumber(CalculateType(*this, rhs), rhs._ldvalue);
        }
        return MultiplyNumber(CalculateType(*this, rhs), rhs._llivalue);
      }

      /**
      * Default *= function multiply the rhs and *this.
      * @param const T& rhs the value we are multiplying with *this
      * @param *this * rhs.
      */
      template<class T>
      Any& operator*=(T rhs)
      {
        *this *= Any(rhs);
        return *this;
      }

      /**
      * Specialized *= function multiply rhs and *this.
      * @param short int rhs the value we are multiplying to *this
      * @param *this * rhs.
      */
      Any& operator*=(short int rhs)
      {
        return MultiplyNumber(CalculateType(*this, dynamic::Integer_short_int), rhs);
      }

      /**
      * Specialized *= function multiply rhs and *this.
      * @param unsigned short int rhs the value we are multiplying to *this
      * @param *this * rhs.
      */
      Any& operator*=(unsigned short int rhs)
      {
        return MultiplyNumber(CalculateType(*this, dynamic::Integer_unsigned_short_int), rhs);
      }

      /**
      * Specialized *= function multiply rhs and *this.
      * @param int rhs the value we are multiplying to *this
      * @param *this * rhs.
      */
      Any& operator*=(int rhs)
      {
        return MultiplyNumber(CalculateType(*this, dynamic::Integer_int), rhs);
      }

      /**
      * Specialized *= function multiply rhs and *this.
      * @param unsigned int rhs the value we are multiplying to *this
      * @param *this * rhs.
      */
      Any& operator*=(unsigned int rhs)
      {
        return MultiplyNumber(CalculateType(*this, dynamic::Integer_unsigned_int), rhs);
      }

      /**
      * Specialized *= function multiply rhs and *this.
      * @param long int rhs the value we are multiplying to *this
      * @param *this * rhs.
      */
      Any& operator*=(long int rhs)
      {
        return MultiplyNumber(CalculateType(*this, dynamic::Integer_long_int), rhs);
      }

      /**
      * Specialized *= function multiply rhs and *this.
      * @param unsigned long int rhs the value we are multiplying to *this
      * @param *this * rhs.
      */
      Any& operator*=(unsigned long int rhs)
      {
        return MultiplyNumber(CalculateType(*this, dynamic::Integer_unsigned_long_int), rhs);
      }

      /**
      * Specialized *= function multiply rhs and *this.
      * @param long long int rhs the value we are multiplying to *this
      * @param *this * rhs.
      */
      Any& operator*=(long long int rhs)
      {
        return MultiplyNumber(CalculateType(*this, dynamic::Integer_long_long_int), rhs);
      }

      /**
      * Specialized *= function multiply rhs and *this.
      * @param unsigned long long int rhs the value we are multiplying to *this
      * @param *this * rhs.
      */
      Any& operator*=(unsigned long long int rhs)
      {
        return MultiplyNumber(CalculateType(*this, dynamic::Integer_unsigned_long_long_int), rhs);
      }

      /**
      * Specialized *= function multiply rhs and *this.
      * @param float rhs the value we are multiplying to *this
      * @param *this * rhs.
      */
      Any& operator*=(float rhs)
      {
        return MultiplyNumber(CalculateType(*this, dynamic::Floating_point_float), rhs);
      }

      /**
      * Specialized *= function multiply rhs and *this.
      * @param double rhs the value we are multiplying to *this
      * @param *this * rhs.
      */
      Any& operator*=(double rhs)
      {
        return MultiplyNumber(CalculateType(*this, dynamic::Floating_point_double), rhs);
      }

      /**
      * Specialized *= function multiply rhs and *this.
      * @param long double rhs the value we are multiplying to *this
      * @param *this * rhs.
      */
      Any& operator*=(long double rhs)
      {
        return MultiplyNumber(CalculateType(*this, dynamic::Floating_point_long_double), rhs);
      }

      /**
      * Binary arithmetic operators - multiplication
      * @param const Any& the item we are multiplying from this.
      * @return Any *this*rhs
      */
      Any operator*(const Any& rhs) const
      {
        // copy the value
        Any value = *this;

        // multiply the rhs
        value *= rhs;

        // return the result.
        return value;
      }

      /**
      * Binary arithmetic operators - multiplication
      * @param Any the item we are multiplying from this.
      * @param const Any& the item we are multiplying from this.
      * @return Any *this*rhs
      */
      template<class T> friend Any operator*(const Any& lhs, const T& rhs) { auto tmp = lhs; tmp *= rhs; return tmp; }

      /**
      * Binary arithmetic operators - multiplication
      * @param Any the item we are multiplying from this.
      * @param const Any& the item we are multiplying from this.
      * @return Any *this*rhs
      */
      template<class T> friend Any operator*(const T& lhs, const Any& rhs) { auto tmp = rhs; tmp *= lhs; return tmp; }

      //
      // *operators
      //
#ifdef _MSC_VER
# pragma endregion *=operators
#endif

      //
      // /operators
      //  
#ifdef _MSC_VER
# pragma region
#endif
      /**
      * Binary arithmetic operators - division
      * @param const Any& the item we are deviding this by
      * @return Any& *this/rhs
      */
      Any& operator/=(const Any& rhs)
      {
        // we use the double number as it is more precise
        if (rhs.Type() == dynamic::Misc_null)
        {
          // anything over null is the same as zero
          throw std::overflow_error("Division by zero.");
        }

        if (rhs.UseUnsignedInteger())
        {
          return DivideNumber(CalculateDivideType(Type(), rhs.Type()), (unsigned long long int)rhs._llivalue);
        }
        if (rhs.UseSignedInteger())
        {
          return DivideNumber(CalculateDivideType(Type(), rhs.Type()), (long long int)rhs._llivalue);
        }

        // use long double
        return DivideNumber(CalculateDivideType(Type(), rhs.Type()), rhs._ldvalue);
      }

      /**
      * Default /= function divide *this by rhs.
      * @param const T& rhs the value we are dividing *this with
      * @param *this / rhs.
      */
      template<class T>
      Any& operator/=(T rhs)
      {
        *this /= Any(rhs);
        return *this;
      }

      /**
      * Specialized /= function divide *this by rhs.
      * @param short int rhs the value we are dividing *this with
      * @param *this / rhs.
      */
      Any& operator/=(short int rhs)
      {
        return DivideNumber(CalculateDivideType(Type(), dynamic::Integer_short_int), rhs);
      }

      /**
      * Specialized /= function divide *this by rhs.
      * @param unsigned short int rhs the value we are dividing *this with
      * @param *this / rhs.
      */
      Any& operator/=(unsigned short int rhs)
      {
        return DivideNumber(CalculateDivideType(Type(), dynamic::Integer_unsigned_short_int), rhs);
      }

      /**
      * Specialized /= function divide *this by rhs.
      * @param int rhs the value we are dividing *this with
      * @param *this / rhs.
      */
      Any& operator/=(int rhs)
      {
        return DivideNumber(CalculateDivideType(Type(), dynamic::Integer_int), rhs);
      }

      /**
      * Specialized /= function divide *this by rhs.
      * @param unsigned int rhs the value we are dividing *this with
      * @param *this / rhs.
      */
      Any& operator/=(unsigned int rhs)
      {
        return DivideNumber(CalculateDivideType(Type(), dynamic::Integer_unsigned_int), rhs);
      }

      /**
      * Specialized /= function divide *this by rhs.
      * @param long int rhs the value we are dividing *this with
      * @param *this / rhs.
      */
      Any& operator/=(long int rhs)
      {
        return DivideNumber(CalculateDivideType(Type(), dynamic::Integer_long_int), rhs);
      }

      /**
      * Specialized /= function divide *this by rhs.
      * @param unsigned long int rhs the value we are dividing *this with
      * @param *this / rhs.
      */
      Any& operator/=(unsigned long int rhs)
      {
        return DivideNumber(CalculateDivideType(Type(), dynamic::Integer_unsigned_long_int), rhs);
      }

      /**
      * Specialized /= function divide *this by rhs.
      * @param long long int rhs the value we are dividing *this with
      * @param *this / rhs.
      */
      Any& operator/=(long long int rhs)
      {
        return DivideNumber(CalculateDivideType(Type(), dynamic::Integer_long_long_int), rhs);
      }

      /**
      * Specialized /= function divide *this by rhs.
      * @param unsigned long long int rhs the value we are dividing *this with
      * @param *this / rhs.
      */
      Any& operator/=(unsigned long long int rhs)
      {
        return DivideNumber(CalculateDivideType(Type(), dynamic::Integer_unsigned_long_long_int), rhs);
      }

      /**
      * Specialized /= function divide *this by rhs.
      * @param float rhs the value we are dividing *this with
      * @param *this / rhs.
      */
      Any& operator/=(float rhs)
      {
        return DivideNumber(CalculateDivideType(Type(), dynamic::Floating_point_float), rhs);
      }

      /**
      * Specialized /= function divide *this by rhs.
      * @param double rhs the value we are dividing *this with
      * @param *this / rhs.
      */
      Any& operator/=(double rhs)
      {
        return DivideNumber(CalculateDivideType(Type(), dynamic::Floating_point_double), rhs);
      }

      /**
      * Specialized /= function divide *this by rhs.
      * @param long double rhs the value we are dividing *this with
      * @param *this / rhs.
      */
      Any& operator/=(long double rhs)
      {
        return DivideNumber(CalculateDivideType(Type(), dynamic::Floating_point_long_double), rhs);
      }

      /**
      * Binary arithmetic operators - division
      * @param const Any& the item we are deviding from this.
      * @return Any *this/ rhs
      */
      Any operator/(const Any& rhs) const
      {
        // copy the value
        Any value = *this;

        // multiply the rhs
        value /= rhs;

        // return the result.
        return value;
      }

      /**
      * Binary arithmetic operators - division
      * @param Any the item we are dividing from this.
      * @param const Any& the item we are dividing from this.
      * @return Any *this / rhs
      */
      template<class T> friend Any operator/(const Any& lhs, const T& rhs) { auto tmp = lhs; tmp /= rhs; return tmp; }

      /**
      * Binary arithmetic operators - division
      * @param Any the item we are dividing from this.
      * @param const Any& the item we are dividing from this.
      * @return Any *this / rhs
      */
      template<class T> friend Any operator/(const T& lhs, const Any& rhs) { auto tmp = Any(lhs); tmp /= rhs; return tmp; }
      //
      // /operators
      //
#ifdef _MSC_VER
# pragma endregion /=operators
#endif

      /**
      * Return the data type
      * @return const Type& the data type.
      */
      const dynamic::Type& Type() const
      {
        return _type;
      }

    protected:
      /**
      * Regadless the data type, we try and guess that the number type could be.
      * mainly used for string, so we can guess the string type.
      * If this is a number we will return the number type, otherwse we will try and work it out.
      * If we do not know we will return Integer_int
      * @return dynamic::Type the string type.
      */
      dynamic::Type NumberType() const
      {
        //  if it is not a character then just use whatever we have
        if (dynamic::is_type_null(Type()))
        {
          return dynamic::Integer_int;
        }

        //  if it is not a character then just use whatever we have
        if (!dynamic::is_type_character(Type()))
        {
          return Type();
        }

        // if the string is not a number, just use an int.
        if (!IsStringNumber(true))
        {
          // this is not a number, so zero will be used.
          // we might as well use an int.
          return dynamic::Integer_int;
        }

        // it seems to be a number, so we need to calculate what type of number it is.
        switch (_stringStatus)
        {
        case dynamic::Any::StringStatus_Pos_Number:
        case dynamic::Any::StringStatus_Partial_Pos_Number:
          if (_llivalue < 0)
          {
            return dynamic::Integer_unsigned_long_long_int;
          }
          return dynamic::Integer_long_long_int;

        case dynamic::Any::StringStatus_Neg_Number:
        case dynamic::Any::StringStatus_Partial_Neg_Number:
          return dynamic::Integer_long_long_int;

        case dynamic::Any::StringStatus_Floating_Pos_Number:
        case dynamic::Any::StringStatus_Floating_Neg_Number:
        case dynamic::Any::StringStatus_Floating_Partial_Pos_Number:
        case dynamic::Any::StringStatus_Floating_Partial_Neg_Number:
          return dynamic::Floating_point_long_double;

        default:
          throw std::runtime_error("Unknown string type!");
        }
      }

      /**
      * Calculate the type for division
      * @param const dynamic::Type& lhs the lhs type
      * @param const dynamic::Type& rhs the rhs type
      * @return dynamic::Type the best type for division
      */
      static dynamic::Type CalculateDivideType(const dynamic::Type& lhs, const dynamic::Type& rhs)
      {
        if (lhs == dynamic::Floating_point_long_double || rhs == dynamic::Floating_point_long_double)
        {
          return dynamic::Floating_point_long_double;
        }

        if (lhs == dynamic::Integer_long_long_int || rhs == dynamic::Integer_long_long_int)
        {
          return dynamic::Floating_point_long_double;
        }

        if (lhs == dynamic::Integer_unsigned_long_long_int || rhs == dynamic::Integer_unsigned_long_long_int)
        {
          return dynamic::Floating_point_long_double;
        }

        else if (lhs == dynamic::Integer_long_int || rhs == dynamic::Integer_long_int)
        {
          return dynamic::Floating_point_long_double;
        }

        if (lhs == dynamic::Integer_unsigned_long_int || rhs == dynamic::Integer_unsigned_long_int)
        {
          return dynamic::Floating_point_long_double;
        }

        // default value.
        return dynamic::Floating_point_double;
      }

      /**
      * This function is used to re-calculate the 'best' type after an arithmetic opereation
      * For example int*int could give us a long long
      *          or int / int could give us a double.
      * NB: this function does not _set_ the type, it only calculates the posible value.
      *     it is up to the call function to set the new type.
      * @param const Any& lhs the original number on the lhs of the operation
      * @param const dynamic::Type& rhsOriginal the original type on the rhs of the operation
      * @return dynamic::Type the posible new type.
      */
      static dynamic::Type CalculateType(const Any& lhs, const Any& rhs)
      {
        auto lhsOriginal = lhs.NumberType();
        auto rhsOriginal = rhs.NumberType();
        return CalculateType(lhsOriginal, rhsOriginal);
      }

      /**
      * This function is used to re-calculate the 'best' type after an arithmetic opereation
      * For example int*int could give us a long long
      *          or int / int could give us a double.
      * NB: this function does not _set_ the type, it only calculates the posible value.
      *     it is up to the call function to set the new type.
      * @param const Any& lhs the original number on the lhs of the operation
      * @param const dynamic::Type& rhsOriginal the original type on the rhs of the operation
      * @return dynamic::Type the posible new type.
      */
      static dynamic::Type CalculateType(const Any& lhs, const dynamic::Type& rhsOriginal)
      {
        return CalculateType(lhs.NumberType(), rhsOriginal);
      }

      /**
      * This function is used to re-calculate the 'best' type after an arithmetic opereation
      * For example int*int could give us a long long
      *          or int / int could give us a double.
      * NB: this function does not _set_ the type, it only calculates the posible value.
      *     it is up to the call function to set the new type.
      * @param const dynamic::Type& lhsOriginal the original type on the lhs of the operation
      * @param const dynamic::Type& rhsOriginal the original type on the rhs of the operation
      * @return dynamic::Type the posible new type.
      */
      static dynamic::Type CalculateType(const dynamic::Type& lhsOriginal, const dynamic::Type& rhsOriginal)
      {
        //  null values become ints.
        if (dynamic::is_type_null(lhsOriginal))
        {
          return CalculateType(dynamic::Integer_int, rhsOriginal);
        }

        if (dynamic::is_type_null(rhsOriginal))
        {
          return CalculateType(lhsOriginal, dynamic::Integer_int);
        }

        //  char values become ints.
        if (is_type_character(lhsOriginal))
        {
          return CalculateType(dynamic::Integer_int, rhsOriginal);
        }

        if (is_type_character(rhsOriginal))
        {
          return CalculateType(lhsOriginal, dynamic::Integer_int);
        }

        //  booleans values become ints.
        if (is_type_boolean(lhsOriginal))
        {
          return CalculateType(dynamic::Integer_int, rhsOriginal);
        }

        if (is_type_boolean(rhsOriginal))
        {
          return CalculateType(lhsOriginal, dynamic::Integer_int);
        }

        //  the possible type
        dynamic::Type type = lhsOriginal;

        // first we check for floating points.
        //
        // if either is long double, they both long double.
        if (lhsOriginal == dynamic::Floating_point_long_double || rhsOriginal == dynamic::Floating_point_long_double)
        {
          type = dynamic::Floating_point_long_double;
        }
        // if either is a double
        else if (lhsOriginal == dynamic::Floating_point_double || rhsOriginal == dynamic::Floating_point_double)
        {
          type = dynamic::Floating_point_double;
        }
        // if either is a float
        else if (lhsOriginal == dynamic::Floating_point_float || rhsOriginal == dynamic::Floating_point_float)
        {
          type = dynamic::Floating_point_float;
        }
        // if either is an unsigned long long int
        else if (lhsOriginal == dynamic::Integer_unsigned_long_long_int || rhsOriginal == dynamic::Integer_unsigned_long_long_int)
        {
          type = dynamic::Integer_unsigned_long_long_int;
        }
        // if either is an long long int
        else if (lhsOriginal == dynamic::Integer_long_long_int || rhsOriginal == dynamic::Integer_long_long_int)
        {
          type = dynamic::Integer_long_long_int;
        }
        else if (lhsOriginal == dynamic::Integer_unsigned_long_int || rhsOriginal == dynamic::Integer_unsigned_long_int)
        {
          type = dynamic::Integer_unsigned_long_int;
        }
        // if either is an long and other is unsigned int
        else if ((lhsOriginal == dynamic::Integer_long_int && rhsOriginal == dynamic::Integer_unsigned_int)
          ||
          (rhsOriginal == dynamic::Integer_long_int && lhsOriginal == dynamic::Integer_unsigned_int)
          )
        {
          type = dynamic::Integer_unsigned_long_int;
        }
        // if either is long
        else if (lhsOriginal == dynamic::Integer_long_int || rhsOriginal == dynamic::Integer_long_int)
        {
          type = dynamic::Integer_long_int;
        }
        // if either is unsigned int
        else if (lhsOriginal == dynamic::Integer_unsigned_int || rhsOriginal == dynamic::Integer_unsigned_int)
        {
          type = dynamic::Integer_unsigned_int;
        }
        else
        {
          type = dynamic::Integer_int;
        }

        // if we are here, they are both the same type, (floating/integer)
        // so we need to return the greatest of them both.
        return type;
      }

      /**
       * Calculate if the lhs is 'smaller' then the rhs
       * @param const Any& lhs the lhs value been compared.
       * @param const Any& rhs the rhs value been compared.
       * @param const CompareType&  the type of comparaison been done.
       * @return bool if the lhs < rhs
       */
      static bool Compare(const Any& lhs, const Any& rhs, const CompareType& compareType )
      {
        // validates that we have known types.
        if (!dynamic::is_known_type(lhs.Type()) || !dynamic::is_known_type(rhs.Type()))
        {
          throw std::runtime_error("Unknown data Type");
        }

        // check for null types.
        if (dynamic::is_type_null(lhs.Type()) && dynamic::is_type_null(rhs.Type() ))
        {
          switch (compareType)
          {
          case CompareType_Equal:
            // both are the same, so if both null then they are the same.
            // all the values should be the same but there is no point in checking.
            return true;

          case CompareType_LessThan:
          case CompareType_MoreThan:
            // both are the same, so if both null then they are
            // neither greater or less than the other one.
            return false;

          default:
            throw std::runtime_error("Unknown compare type");
          }
        }

        // are we comparing trivial structures
        if (dynamic::is_type_copy(lhs.Type()) || dynamic::is_type_copy(rhs.Type()))
        {
          switch (compareType)
          {
          case CompareType_Equal:
            return EqualCopy(lhs, rhs);

          case CompareType_LessThan:
          case CompareType_MoreThan:
            // we cannot compare greater/less than objects.
            return false;

          default:
            throw std::runtime_error("Unknown compare type");
          }
        }

        // if either of them is a string, then we need to check them first.
        if (dynamic::is_type_character(lhs.Type()) || dynamic::is_type_character(rhs.Type()))
        {
          return CompareString(lhs, rhs, compareType);
        }

        return CompareNumber(lhs, rhs, compareType);
      }

      /**
       * Calculate if the lhs is 'smaller' then the rhs
       * This is the default behaviour, in the case of a numeric compare.
       * @param const Any& lhs the lhs value been compared.
       * @param const Any& rhs the rhs value been compared.
       * @param const CompareType&  the type of comparaison been done.
       * @return bool if the lhs < rhs
       */
      static bool CompareNumber(const Any& lhs, const Any& rhs, const CompareType& compareType)
      {
        auto type = CalculateType(lhs, rhs);
        switch (type)
        {
        case Boolean_bool:
        case Character_signed_char:
        case Character_unsigned_char:
        case Character_char:
        case Character_wchar_t:

        // short integers are always converted to Integer_Int
        case Integer_short_int:
        case Integer_unsigned_short_int:
          throw std::runtime_error("Logic error, the function CalculateType() should never return those types.");

        case Integer_int:
        case Integer_unsigned_int:
          if (lhs.UseSignedInteger() && rhs.UseSignedInteger())
          {
            switch (compareType)
            {
            case CompareType_Equal:
              return ((int)lhs._llivalue == (int)rhs._llivalue);

            case CompareType_LessThan:
              return ((int)lhs._llivalue < (int)rhs._llivalue);

            case CompareType_MoreThan:
              return ((int)lhs._llivalue > (int)rhs._llivalue);

            default:
              throw std::runtime_error("Unknown compare type");
            }
          }
          else if (lhs.UseUnsignedInteger() && rhs.UseSignedInteger())
          {
            // as we know that rhs is signed then if rhs < 0 then it must be smaller than unsigned lhs
            switch (compareType)
            {
            case CompareType_Equal:
              return (rhs._llivalue >= 0 && (unsigned int)lhs._llivalue == (unsigned int)rhs._llivalue);

            case CompareType_LessThan:
              return (rhs._llivalue >= 0 && (unsigned int)lhs._llivalue < (unsigned int)rhs._llivalue);

            case CompareType_MoreThan:
              return (rhs._llivalue < 0 || (unsigned int)lhs._llivalue > (unsigned int)rhs._llivalue);

            default:
              throw std::runtime_error("Unknown compare type");
            }
          }
          else if (lhs.UseSignedInteger() && rhs.UseUnsignedInteger())
          {
            // as we know that lhs is signed then if lhs < 0 then it must be smaller than unsigned rhs
            switch (compareType)
            {
            case CompareType_Equal:
              return (lhs._llivalue >= 0 && (unsigned int)lhs._llivalue == (unsigned int)rhs._llivalue);

            case CompareType_LessThan:
              return (lhs._llivalue < 0 || (unsigned int)lhs._llivalue < (unsigned int)rhs._llivalue);

            case CompareType_MoreThan:
              return (lhs._llivalue >= 0 && (unsigned int)lhs._llivalue > (unsigned int)rhs._llivalue);

            default:
              throw std::runtime_error("Unknown compare type");
            }
          }
          else
          {
            switch (compareType)
            {
            case CompareType_Equal:
              return ((unsigned int)lhs._llivalue == (unsigned int)rhs._llivalue);

            case CompareType_LessThan:
              return ((unsigned int)lhs._llivalue < (unsigned int)rhs._llivalue);

            case CompareType_MoreThan:
              return ((unsigned int)lhs._llivalue > (unsigned int)rhs._llivalue);

            default:
              throw std::runtime_error("Unknown compare type");
            }
          }
          break;

        case Integer_long_int:
        case Integer_unsigned_long_int:
          if (lhs.UseSignedInteger() && rhs.UseSignedInteger())
          {
            switch (compareType)
            {
            case CompareType_Equal:
              return ((long int)lhs._llivalue == (long int)rhs._llivalue);

            case CompareType_LessThan:
              return ((long int)lhs._llivalue < (long int)rhs._llivalue);

            case CompareType_MoreThan:
              return ((long int)lhs._llivalue > (long int)rhs._llivalue);

            default:
              throw std::runtime_error("Unknown compare type");
            }
          }
          else if (lhs.UseUnsignedInteger() && rhs.UseSignedInteger())
          {
            // as we know that rhs is signed then if rhs < 0 then it must be smaller than unsigned lhs
            switch (compareType)
            {
            case CompareType_Equal:
              return (rhs._llivalue >= 0 && (unsigned long int)lhs._llivalue == (unsigned long int)rhs._llivalue);

            case CompareType_LessThan:
              return (rhs._llivalue >= 0 && (unsigned long int)lhs._llivalue < (unsigned long int)rhs._llivalue);

            case CompareType_MoreThan:
              return (rhs._llivalue < 0 || (unsigned long int)lhs._llivalue > (unsigned long int)rhs._llivalue);

            default:
              throw std::runtime_error("Unknown compare type");
            }
          }
          else if (lhs.UseSignedInteger() && rhs.UseUnsignedInteger())
          {
            // as we know that lhs is signed then if lhs < 0 then it must be smaller than unsigned rhs
            switch (compareType)
            {
            case CompareType_Equal:
              return (lhs._llivalue >= 0 && (unsigned long int)lhs._llivalue == (unsigned long int)rhs._llivalue);

            case CompareType_LessThan:
              return (lhs._llivalue < 0 || (unsigned long int)lhs._llivalue < (unsigned long int)rhs._llivalue);

            case CompareType_MoreThan:
              return (lhs._llivalue >= 0 && (unsigned long int)lhs._llivalue > (unsigned long int)rhs._llivalue);

            default:
              throw std::runtime_error("Unknown compare type");
            }
          }
          else
          {
            switch (compareType)
            {
            case CompareType_Equal:
              return ((unsigned long int)lhs._llivalue == (unsigned long int)rhs._llivalue);

            case CompareType_LessThan:
              return ((unsigned long int)lhs._llivalue < (unsigned long int)rhs._llivalue);

            case CompareType_MoreThan:
              return ((unsigned long int)lhs._llivalue > (unsigned long int)rhs._llivalue);

            default:
              throw std::runtime_error("Unknown compare type");
            }
          }
          break;

        case Integer_long_long_int:
        case Integer_unsigned_long_long_int:
          if (lhs.UseSignedInteger() && rhs.UseSignedInteger())
          {
            switch (compareType)
            {
            case CompareType_Equal:
              return ((long long int)lhs._llivalue == (long long int)rhs._llivalue);

            case CompareType_LessThan:
              return ((long long int)lhs._llivalue < (long long int)rhs._llivalue);

            case CompareType_MoreThan:
              return ((long long int)lhs._llivalue > (long long int)rhs._llivalue);

            default:
              throw std::runtime_error("Unknown compare type");
            }
          }
          else if (lhs.UseUnsignedInteger() && rhs.UseSignedInteger())
          {
            // as we know that rhs is signed then if rhs < 0 then it must be smaller than unsigned lhs
            switch (compareType)
            {
            case CompareType_Equal:
              return (rhs._llivalue >= 0 && (unsigned long long int)lhs._llivalue == (unsigned long long int)rhs._llivalue);

            case CompareType_LessThan:
              return (rhs._llivalue >= 0 && (unsigned long long int)lhs._llivalue < (unsigned long long int)rhs._llivalue);

            case CompareType_MoreThan:
              return (rhs._llivalue < 0 || (unsigned long long int)lhs._llivalue > (unsigned long long int)rhs._llivalue);

            default:
              throw std::runtime_error("Unknown compare type");
            }
          }
          else if (lhs.UseSignedInteger() && rhs.UseUnsignedInteger())
          {
            // as we know that lhs is signed then if lhs < 0 then it must be smaller than unsigned rhs
            switch (compareType)
            {
            case CompareType_Equal:
              return (lhs._llivalue >= 0 && (unsigned long long int)lhs._llivalue == (unsigned long long int)rhs._llivalue);

            case CompareType_LessThan:
              return (lhs._llivalue < 0 || (unsigned long long int)lhs._llivalue < (unsigned long long int)rhs._llivalue);

            case CompareType_MoreThan:
              return (lhs._llivalue >= 0 && (unsigned long long int)lhs._llivalue > (unsigned long long int)rhs._llivalue);

            default:
              throw std::runtime_error("Unknown compare type");
            }
          }
          else
          {
            switch (compareType)
            {
            case CompareType_Equal:
              return ((unsigned long long int)lhs._llivalue == (unsigned long long int)rhs._llivalue);

            case CompareType_LessThan:
              return ((unsigned long long int)lhs._llivalue < (unsigned long long int)rhs._llivalue);

            case CompareType_MoreThan:
              return ((unsigned long long int)lhs._llivalue > (unsigned long long int)rhs._llivalue);

            default:
              throw std::runtime_error("Unknown compare type");
            }
          }
          break;

          // Floating point
        case Floating_point_float:
          switch (compareType)
          {
          case CompareType_Equal:
            return ((float)lhs._ldvalue == (float)rhs._ldvalue);

          case CompareType_LessThan:
            return ((float)lhs._ldvalue < (float)rhs._ldvalue);

          case CompareType_MoreThan:
            return ((float)lhs._ldvalue > (float)rhs._ldvalue);

          default:
            throw std::runtime_error("Unknown compare type");
          }
          break;

        case Floating_point_double:
          switch (compareType)
          {
          case CompareType_Equal:
            return ((double)lhs._ldvalue == (double)rhs._ldvalue);

          case CompareType_LessThan:
            return ((double)lhs._ldvalue < (double)rhs._ldvalue);

          case CompareType_MoreThan:
            return ((double)lhs._ldvalue > (double)rhs._ldvalue);

          default:
            throw std::runtime_error("Unknown compare type");
          }
          break;

        case Floating_point_long_double:
          switch (compareType)
          {
          case CompareType_Equal:
            return (lhs._ldvalue == rhs._ldvalue);

          case CompareType_LessThan:
            return (lhs._ldvalue < rhs._ldvalue);

          case CompareType_MoreThan:
            return (lhs._ldvalue > rhs._ldvalue);

          default:
            throw std::runtime_error("Unknown compare type");
          }
          break;

        default:
          throw std::bad_cast();
        }

        //  never reached.
        throw std::runtime_error("Imposible, how did we get this far?");
      }

      /**
      * Calculate if the lhs string is 'smaller' then the rhs
      * This is the string behaviour.
      * @param const Any& lhs the lhs value been compared.
      * @param const Any& rhs the rhs value been compared.
      * @param const CompareType&  the type of comparaison been done.
      * @return bool if the lhs < rhs
      */
      static bool CompareString(const Any& lhs, const Any& rhs, const CompareType& compareType)
      {
        // the equal compare is slightly different.
        if (compareType == CompareType_Equal)
        {
          return EqualString(lhs, rhs);
        }

        // if either of them is _not_ a stringand we know the other is a string
        // then we have to treat the other as zero, (or maybe valid number)
        // for example 12 < "Hello"
        if (!dynamic::is_type_character(lhs.Type()) || !dynamic::is_type_character(rhs.Type()))
        {
          return CompareNumber(lhs, rhs, compareType);
        }

        // if either of them is a number, (even partial), then we have to compare it as a number
        // and if the other is not a string, then we will compare them both.
        // for example "12 bottles of beer" > "Hello" (12 > 0)
        if (lhs.IsStringNumber( true ) || rhs.IsStringNumber( true ))
        {
          return CompareNumber(lhs, rhs, compareType);
        }

        //  if we are here, then neither values can be null.
        if (!lhs._cvalue || !rhs._cvalue)
        {
          // we throw a runtime error as it should never happen
          // how can we have a string and the actual value for it be null??
          throw std::runtime_error( "This is imposible, how can a string have a null value!" );
        }
        
        // it might not be '\0' terminated, so we have to go by the len.
        // <0	the first character that does not match has a lower value in str1 than in str2
        //  0	the contents of both strings are equal
        // >0	the first character that does not match has a greater value in str1 than in str2
        switch (compareType)
        {
        case CompareType_LessThan:
          return (memcmp(lhs._cvalue, rhs._cvalue, (lhs._lcvalue <= rhs._lcvalue ? lhs._lcvalue : rhs._lcvalue)) < 0);

        case CompareType_MoreThan:
          return (memcmp(lhs._cvalue, rhs._cvalue, (lhs._lcvalue <= rhs._lcvalue ? lhs._lcvalue : rhs._lcvalue)) > 0);

        default:
          throw std::runtime_error("Unknown compare type");
        }
      }
      
      /**
      * Compare two non-number strings.
      * This function assumes that both values are string and either of them is _not_ a number.
      * Note that Wide/AscII are not the same, so "Hello" != L"Hello"
      * @throw if we are unable to compare, (not same types, not same sizes etc...)
      * @param const Any& lhs the lhs value been compared.
      * @param const Any& rhs the rhs value been compared.
      * @return bool if they are the same or not.
      */
      static bool EqualString(const Any& lhs, const Any& rhs)
      {
        // if either of them is _not_ a stringand we know the other is a string
        // then we have to treat the other as zero, (or maybe valid number)
        // for example 0 == "Hello"
        if (!dynamic::is_type_character(lhs.Type()) || !dynamic::is_type_character(rhs.Type()))
        {
          return CompareNumber(lhs, rhs, CompareType_Equal );
        }

        // if both of those strings are full numbers then we could compare them as numbers.
        // if the other is not a full number, then we will compare them as string.
        // so "12 Hello" != "12 Bye" but "12.0" == "12"
        if (lhs.IsStringNumber(false) && rhs.IsStringNumber(false))
        {
          return CompareNumber(lhs, rhs, CompareType_Equal);
        }

        //  if we are here, then neither values can be null.
        if (!lhs._cvalue || !rhs._cvalue)
        {
          // we throw a runtime error as it should never happen
          // how can we have a string and the actual value for it be null??
          throw std::runtime_error("This is imposible, how can a string have a null value!");
        }

        // are both strings the same lengh?
        // if not then they are not the same.
        if (lhs._lcvalue != rhs._lcvalue)
        {
          return false;
        }

        // the lenght is the same, so we can use the size of lhs
        // it does not matter if they are both wide or not, we are 
        // just comparing that both balues are the same.
        return (0 == std::memcmp(lhs._cvalue, rhs._cvalue, lhs._lcvalue));
      }

      /**
      * Compare one or more trivial cases.
      * @throw if we are unable to compare, (not same types, not same sizes etc...)
      * @return bool if they are the same or not.
      */
      static bool EqualCopy(const Any& lhs, const Any& rhs)
      {
        // are they both trivial?
        if (!dynamic::is_type_copy(lhs.Type()) || !dynamic::is_type_copy(rhs.Type()) )
        {
          // if either one of them is of type copy, but
          // not the other then they are not equal...
          if (dynamic::is_type_copy(lhs.Type()) || dynamic::is_type_copy(rhs.Type()))
          {
            return false;
          }
          // We cannot compare non trivials.
          throw std::bad_cast();
        }

        // are they both trivial types?
        if (lhs.Type() == dynamic::Misc_copy && rhs.Type() == dynamic::Misc_copy)
        {
          if (lhs._unkvalue->Size() != rhs._unkvalue->Size())
          {
            return false;
          }
          // both are the same, (trivial or non-trivial), just compare by size.
          return lhs._unkvalue->Equal(rhs._unkvalue->Data());
        }

        // not sure how to compare those.
        throw std::bad_cast();
      }

#ifdef _MSC_VER
# pragma region
#endif
      /**
      * Try and create from a given value.
      * @throw std::bad_cast() if we are trying to create from an unknwon value.
      * @param const T& value the value we are trying to create from.
      */
      template<class T>
      void CreateFrom(const T& value)
      {
        // clear all the values.
        CleanValues();

        // set the type
        _type = dynamic::get_type<T>::value;
        switch (Type())
        {
        case dynamic::Misc_null:
          _llivalue = 0;
          _ldvalue = 0;
          return;

        case dynamic::Misc_unknown:
          // Objects of trivially - copyable types are the only C++ objects that 
          // may be safely copied with std::memcpy
          CreateFromUnknown(value);

          // done
          return;

        default:
          // is this a new type that we are not handling?
          throw std::runtime_error("Dynamic type is not handled.");
        }

        // we could not deduce the value from this.
        throw std::bad_cast();
      }

      /**
      * Try and create from a given value.
      * @throw std::bad_cast() if we are trying to create from an unknwon value.
      * @param const T& value the value we are trying to create from.
      */
      template<class T>
      void CreateFrom(T* value)
      {
        // clear all the values.
        CleanValues();

        // set the type
        _type = dynamic::get_type<T>::value;

        // if unknown try and set is as a pointer.
        if (_type == dynamic::Misc_unknown)
        {
          // create from an unknown pointer type.
          CreateFromUnknown<T*>(value);

          // we are done
          return;
        }

        // if not null then we can set it.
        if (nullptr != value)
        {
          CreateFrom(*value);
        }
        else
        {
          // if it is null we must still set the type, but default the values to zeros.
          switch (Type())
          {
          case dynamic::Misc_null:
            _llivalue = 0;
            _ldvalue = 0;
            return;

            // boolean
          case dynamic::Boolean_bool:

            // int
          case dynamic::Integer_short_int:
          case dynamic::Integer_unsigned_short_int:
          case dynamic::Integer_int:
          case dynamic::Integer_unsigned_int:
          case dynamic::Integer_long_int:
          case dynamic::Integer_unsigned_long_int:
          case dynamic::Integer_long_long_int:
          case dynamic::Integer_unsigned_long_long_int:

            // floating points.
          case dynamic::Floating_point_double:
          case dynamic::Floating_point_float:
          case dynamic::Floating_point_long_double:
            _llivalue = 0;
            _ldvalue = 0;
            return;

          case dynamic::Misc_unknown:
            break;

          default:
            // is this a new type that we are not handling?
            throw std::runtime_error("Dynamic type is not handled.");
          }

          // we could not deduce the value from this.
          throw std::bad_cast();
        }
      }

      /**
      * Create from a boolean value.
      * @param const bool& value the bool value.
      */
      void CreateFrom(const bool& value)
      {
        // clear all the values.
        CleanValues();

        // set the type
        _type = dynamic::get_type<bool>::value;

        // set the values
        _llivalue = (value ? 1 : 0);
        _ldvalue = (value ? 1 : 0);
      }

      /**
      * Create from a float value.
      * @param const float& value the number value.
      */
      void CreateFrom(const float& value)
      {
        CreateFromDouble(value);
      }

      /**
      * Create from a double value.
      * @param const double& value the number value.
      */
      void CreateFrom(const double& value)
      {
        CreateFromDouble(value);
      }

      /**
      * Create from a long double value.
      * @param const long double& value the number value.
      */
      void CreateFrom(const long double& value)
      {
        CreateFromDouble(value);
      }

      /**
      * Create from a short int value.
      * @param const short int& value the number value.
      */
      void CreateFrom(const short int& value)
      {
        CreateFromInteger(value);
      }

      /**
      * Create from an unsigned short int value.
      * @param const unsigned short int& value the number value.
      */
      void CreateFrom(const unsigned short int& value)
      {
        CreateFromInteger(value);
      }

      /**
      * Create from an int value.
      * @param const int& value the number value.
      */
      void CreateFrom(const int& value)
      {
        CreateFromInteger(value);
      }

      /**
      * Create from an unsigned int value.
      * @param const unsigned int& value the number value.
      */
      void CreateFrom(const unsigned int& value)
      {
        CreateFromInteger(value);
      }

      /**
      * Create from a long int value.
      * @param const long int& value the number value.
      */
      void CreateFrom(const long int& value)
      {
        CreateFromInteger(value);
      }

      /**
      * Create from a unsigned long int value.
      * @param const unsigned long int& value the number value.
      */
      void CreateFrom(const unsigned long int& value)
      {
        CreateFromInteger(value);
      }

      /**
      * Create from a long long int value.
      * @param const long long int& value the number value.
      */
      void CreateFrom(const long long int& value)
      {
        CreateFromInteger(value);
      }

      /**
      * Create from a unsigned long int value.
      * @param const unsigned long long int& value the number value.
      */
      void CreateFrom(const unsigned long long int& value)
      {
        CreateFromInteger(value);
      }

      /**
      * Create from a character pointer.
      * @param char* value the value we are trying to create from.
      */
      void CreateFrom(char* value)
      {
        CreateFromCharacters(value);
      }

      /**
      * Create from a character pointer.
      * @param char* value the value we are trying to create from.
      */
      void CreateFrom(const char* value)
      {
        CreateFromCharacters(value);
      }

      /**
      * Create from a character pointer.
      * @param signed char* value the value we are trying to create from.
      */
      void CreateFrom(signed char* value)
      {
        CreateFromCharacters(value);
      }

      /**
      * Create from a character pointer.
      * @param signed char* value the value we are trying to create from.
      */
      void CreateFrom(const signed char* value)
      {
        CreateFromCharacters(value);
      }

      /**
      * Create from a character pointer.
      * @param unsigned char* value the value we are trying to create from.
      */
      void CreateFrom(unsigned char* value)
      {
        CreateFromCharacters(value);
      }

      /**
      * Create from a character pointer.
      * @param unsigned char* value the value we are trying to create from.
      */
      void CreateFrom(const unsigned char* value)
      {
        CreateFromCharacters(value);
      }

      /**
      * Create from a character pointer.
      * @param wchar_t* value the value we are trying to create from.
      */
      void CreateFrom(wchar_t* value)
      {
        CreateFromCharacters(value);
      }

      /**
      * Create from a character pointer.
      * @param wchar_t* value the value we are trying to create from.
      */
      void CreateFrom(const wchar_t* value)
      {
        CreateFromCharacters(value);
      }

      /**
      * Try and create from a given value.
      * @throw std::bad_cast() if we are trying to create from an unknwon value.
      * @param const T& value the value we are trying to create from.
      */
      void CreateFrom(nullptr_t)
      {
        // clear all the values.
        CleanValues();
      }

      /**
      * Create from a given value.
      * @param const char& value the value we are trying to create from.
      */
      void CreateFrom(const char& value)
      {
        CreateFromCharacter(value);
      }

      /**
      * Create from a given value.
      * @param const signed char& value the value we are trying to create from.
      */
      void CreateFrom(const signed char& value)
      {
        CreateFromCharacter(value);
      }

      /**
      * Create from a given value.
      * @param const unsigned char& value the value we are trying to create from.
      */
      void CreateFrom(const unsigned char& value)
      {
        CreateFromCharacter(value);
      }

      /**
      * Create from a given value.
      * @param const wchar_t& value the value we are trying to create from.
      */
      void CreateFrom(const wchar_t& value)
      {
        CreateFromCharacter(value);
      }

      /**
      * Create from a const std::string.
      * @param std::string& value the value we are trying to create from.
      */
      void CreateFrom(std::string& value)
      {
        CreateFromCharacters(value.c_str());
      }

      /**
      * Create from a const std::string*.
      * @param std::string* value the value we are trying to create from.
      */
      void CreateFrom(std::string* value)
      {
        CreateFromCharacters(value ? value->c_str() : (const char*)nullptr);
      }

      /**
      * Create from a const std::string*.
      * @param const std::string* value the value we are trying to create from.
      */
      void CreateFrom(const std::string* value)
      {
        CreateFromCharacters(value ? value->c_str() : (const char*)nullptr);
      }

      /**
      * Create from a std::wstring.
      * @param std::wstring& value the value we are trying to create from.
      */
      void CreateFrom(std::wstring& value)
      {
        CreateFromCharacters(value.c_str());
      }

      /**
      * Create from a const std::wstring*.
      * @param std::wstring* value the value we are trying to create from.
      */
      void CreateFrom(std::wstring* value)
      {
        CreateFromCharacters(value ? value->c_str() : (wchar_t*)nullptr);
      }

      /**
      * Create from a const std::wstring*.
      * @param const std::wstring* value the value we are trying to create from.
      */
      void CreateFrom(const std::wstring* value)
      {
        CreateFromCharacters(value ? value->c_str() : (wchar_t*)nullptr);
      }

      /**
      * Create from a const std::string.
      * @param const std::string& value the value we are trying to create from.
      */
      void CreateFrom(const std::string& value)
      {
        CreateFromCharacters(value.c_str());
      }

      /**
      * Create from a const std::wstring.
      * @param const std::wstring& value the value we are trying to create from.
      */
      void CreateFrom(const std::wstring& value)
      {
        CreateFromCharacters(value.c_str());
      }

      /**
      * Create a value from a double/float/long double number..
      * @param const T* number the number we are creating from.
      */
      template<class T>
      void CreateFromDouble(const T& number)
      {
        // clear all the values.
        CleanValues();

        // set the type
        _type = dynamic::get_type<T>::value;

        // set the values
        _ldvalue = static_cast<long double>(number);
        _llivalue = static_cast<long long int>(_ldvalue);
      }

      /**
      * Create a value from a int/long/long long...
      * @param const T* number the number we are creating from.
      */
      template<class T>
      void CreateFromInteger(const T& number)
      {
        // clear all the values.
        CleanValues();

        // set the type
        _type = dynamic::get_type<T>::value;

        // set the values.
        _llivalue = static_cast<long long int>(number);
        _ldvalue = static_cast<long double>(_llivalue);
      }

      /**
      * Create from an unknown type.
      * Objects of trivially - copyable types are the only C++ objects that may be safely copied with std::memcpy
      * @param const T& trivial the structure/class we want to copy from.
      */
      template<class T>
      std::enable_if_t<!std::is_pointer<T>::value> CreateFromUnknown(const T& unknown )
      {
        if (std::is_copy_constructible<T>::value)
        {
          //  copy construct the value.
          CreateFromCopyConstructible(unknown);

          // done
          return;
        }

        // as it is not a pointer value, it has to be trivially copyable.
        if (std::is_trivially_copyable<T>::value)
        {
          // trivial value
          CreateFromCopyConstructible(unknown);

          // done
          return;
        }

        // not sure what it is.
        throw std::bad_cast();
      }

      /**
      * Create from an unknown type.
      * Objects of trivially - copyable types are the only C++ objects that may be safely copied with std::memcpy
      * @param const T& trivial the structure/class we want to copy from.
      */
      template<class T>
      std::enable_if_t<std::is_pointer<T>::value> CreateFromUnknown(const T& unknown )
      {
        // it is a pointer, so it is an unknown pointer.
        CreateFromUnknownPtr(unknown );
      }

      /**
      * This function should never be called by a non copy constructible T.
      * But the compiler needs a placeholder function.
      * @param const T& copy the structure/class we want to copy from.
      */
      template<class T>
      std::enable_if_t<!std::is_copy_constructible<T>::value> CreateFromCopyConstructible(const T& copy)
      {
        throw std::bad_cast();
      }

      /**
      * Create from a copy constructible value.
      * @param const T& copy the structure/class we want to copy from.
      */
      template<class T>
      std::enable_if_t<std::is_copy_constructible<T>::value> CreateFromCopyConstructible(const T& copy )
      {
        // clear all the values.
        CleanValues();

        // set the type
        _type = dynamic::Misc_copy;

        // set the values.
        _llivalue = 0;
        _ldvalue = 0;

        // copy the trival value.
        _unkvalue = new UnknownItem<typename std::remove_pointer<T>::type>(copy);
      }

      /**
      * Create from a trivally copyable value.
      * Objects of trivially - copyable types are the only C++ objects that may be safely copied with std::memcpy
      * @param const T& trivial the structure/class we want to copy from.
      */
      template<class T>
      void CreateFromUnknownPtr(const T& unkptr)
      {
        // clear all the values.
        CleanValues();

        // set the type
        _type = dynamic::Misc_copy_ptr;

        // set the values.
        _llivalue = 0;
        _ldvalue = 0;

        // copy the trival pointer value that was given to us.
        _unkvalue = new UnknownItem<T>(unkptr);
      }

      /**
      * Create a value from a multiple characters..
      * @param const T* value the character we are creating from.
      */
      template<class T>
      void CreateFromCharacters(const T* value)
      {
        // get the len of the given pointer, as we have no lenght give
        // we are usuming that this is a null terminated string.
        size_t givenLen = value ? ((std::strlen((const char*)value) + 1) * sizeof(T)) : 0;

        // we can now try and create it with the given len.
        CreateFromCharacters(value, givenLen);
      }

      /**
       * Create this with a signed/unsigned char*
       * @param const T source the char value we are creating from.
       * @param size_t the lenght we are working with.
       */
      template<class T>
      std::enable_if_t<std::is_pointer<T>::value> CreateFromCharacters(const T source, size_t sourceLen )
      {
        // clean the values.
        CleanValues();

        // set the type
        _type = dynamic::get_type< typename std::remove_pointer<T>::type >::value;

        // if this is a wide char item, we need to handle it slightly differently.
        // this is because our numbers are not quite the same.
        if (_type == dynamic::Character_wchar_t)
        {
          CreateFromWideCharacters((wchar_t*)source, sourceLen);
          return;
        }
        
        if (nullptr != source)
        {
          // get the number of characters.
          _lcvalue = sourceLen;

          // create the character, we know it is at least one, even for an empty string.
          _cvalue = new char[_lcvalue];

          // memory clear
          std::memset(_cvalue, '\0', _lcvalue);
          std::memcpy(_cvalue, source, _lcvalue);

          if (_lcvalue > 1)
          {
            // it does not matter if this is signed or not signed
            // we are converting it to an unsigned long long and back to a long long
            // in reality they both take the same amount of space.
            _llivalue = static_cast<long long int>(std::strtoull((const char*)source, nullptr, 10));

            // try and get the value as a long double.
            // this is represented in a slightly different way in memory
            // hence the reason we cannot just cast our long long to long double.
            _ldvalue = std::strtold((const char*)source, nullptr);
          }
          else
          {
            // the size is only one, (for our trailling '\0')
            // so we know that the value has to be zero.
            _llivalue = 0;
            _ldvalue = 0;
          }
        }
        else
        {
          // create a default value for the string.
          const char c = '\0';

          // default values.
          _lcvalue = sizeof( typename std::remove_pointer<T>::type );
          _cvalue = new char[_lcvalue];
          std::memset(_cvalue, 0, _lcvalue);
          std::memcpy(_cvalue, &c, _lcvalue);

          // default values are 0
          _llivalue = 0;
          _ldvalue = 0;
        }

        // parse the string to set the string flag
        ParseStringStatus((const char*)source, sourceLen );
      }

      /**
      * Create a value from a multiple characters..
      * @param const T* value the character we are creating from.
      */
      void CreateFromCharacters(const wchar_t* value)
      {
        // get the len of the given pointer, as we have no lenght give
        // we are usuming that this is a null terminated string.
        size_t givenLen = value ? ((std::wcslen((const wchar_t*)value) + 1) * sizeof(wchar_t)) : 0;

        // we can now try and create it with the given len.
        CreateFromWideCharacters(value, givenLen);
      }

      /**
      * Create this with a wchar_t*
      * @param const T* source the char value we are creating from.
      * @param size_t the lenght we are working with.
      */
      void CreateFromWideCharacters(const wchar_t* source, size_t sourceLen)
      {
        // clean the values.
        CleanValues();

        // set the type
        _type = dynamic::get_type<wchar_t>::value;

        if (nullptr != source)
        {
          // default values.
          _lcvalue = sourceLen;

          // create the character, we know it is at least one, even for an empty string.
          _cvalue = new char[_lcvalue];

          // memory clear
          std::memset(_cvalue, '\0', _lcvalue);
          std::memcpy(_cvalue, source, _lcvalue);

          if (_lcvalue > 1)
          {
            // it does not matter if this is signed or not signed
            // we are converting it to an unsigned long long and back to a long long
            // in reality they both take the same amount of space.
            _llivalue = static_cast<long long int>(std::wcstoull((const wchar_t*)source, nullptr, 10));

            // try and get the value as a long double.
            // this is represented in a slightly different way in memory
            // hence the reason we cannot just cast our long long to long double.
            _ldvalue = std::wcstold((const wchar_t*)source, nullptr);
          }
          else
          {
            // the size is only one, (for our trailling '\0')
            // so we know that the value has to be zero.
            _llivalue = 0;
            _ldvalue = 0;
          }
        }
        else
        {
          // create a default value for the string.
          const wchar_t wide = L'\0';
          _lcvalue = sizeof(wchar_t);
          _cvalue = new char[_lcvalue];
          std::memset(_cvalue, 0, _lcvalue);
          std::memcpy(_cvalue, &wide, _lcvalue);

          // default values are 0
          _llivalue = 0;
          _ldvalue = 0;
        }

        // parse the string to set the string flag
        ParseStringStatus((const wchar_t*)source, sourceLen);
      }

      /**
      * Create a value from a single character.
      * @param const char value the character we are creating from.
      */
      template<class T>
      void CreateFromCharacter(const T value)
      {
        // clean the values.
        CleanValues();

        // set the type
        _type = dynamic::get_type<T>::value;

        // create the character.
        _lcvalue = sizeof(T);
        _cvalue = new char[_lcvalue];
        std::memset(_cvalue, '\0', _lcvalue);
        std::memcpy(_cvalue, &value, _lcvalue);

        if (value >= '0' && value <= '9')
        {
          auto number = value - '0';
          _llivalue = number;
          _ldvalue = number;

          //  if has to be a non floating point number.
          _stringStatus = StringStatus_Pos_Number;
        }
        else
        {
          _llivalue = 0;
          _ldvalue = 0;

          //  this is not a number.
          _stringStatus = StringStatus_Not_A_Number;
        }
      }

      /**
      * Create a value from a single wide character.
      * @param const wchar_t value the character we are creating from.
      */
      void CreateFromCharacter(const wchar_t value)
      {
        // clean the values.
        CleanValues();

        // set the type
        _type = dynamic::get_type<wchar_t>::value;

        // create the character.
        _lcvalue = sizeof(wchar_t);
        _cvalue = new char[_lcvalue];
        std::memset(_cvalue, '\0', _lcvalue);
        std::memcpy(_cvalue, &value, _lcvalue);

        // copy it.
        if (value >= L'0' && value <= L'9')
        {
          auto number = value - L'0';
          _llivalue = number;
          _ldvalue = number;

          //  if has to be a non floating point number.
          _stringStatus = StringStatus_Pos_Number;
        }
        else
        {
          _llivalue = 0;
          _ldvalue = 0;

          //  this is not a number.
          _stringStatus = StringStatus_Not_A_Number;
        }
      }
#ifdef _MSC_VER
# pragma endregion CreateFrom - Create *this from T
#endif

#ifdef _MSC_VER
# pragma region
#endif
      /**
      * Try and cast this to a posible value.
      * @return T the value we are looking for.
      */
      template<class T>
      void CastTo(T& value) const
      {
        switch (Type())
        {
        case dynamic::Misc_copy:
        case dynamic::Misc_copy_ptr:
          throw std::bad_cast();
          break;

          // none of the fundamental types are handled here.
          // each has its own function
        default:
          // we cannot cast this.
          throw std::bad_cast();
        }
      }

      /**
      * Cast this to a fundamental type
      * @return short int the value.
      */
      float CastToFloat() const
      {
        float value;
        CastToFundamental(value);
        return value;
      }

      /**
      * Cast this to a fundamental type
      * @return double the value.
      */
      double CastToDouble() const
      {
        double value;
        CastToFundamental(value);
        return value;
      }

      /**
      * Cast this to a fundamental type
      * @return long double the value.
      */
      long double CastToLongDouble() const
      {
        long double value;
        CastToFundamental(value);
        return value;
      }

      /**
      * Cast this to a fundamental type
      * @return short int the value.
      */
      short int CastToShortInt() const
      {
        short int value;
        CastToFundamental(value);
        return value;
      }

      /**
      * Cast this to a fundamental type
      * @return unsigned short int the value.
      */
      unsigned short int CastToUnsignedShortInt() const
      {
        unsigned short int value;
        CastToFundamental(value);
        return value;
      }

      /**
      * Cast this to a fundamental type
      * @return int the value.
      */
      int CastToInt() const
      {
        int value = 0;
        CastToFundamental(value);
        return value;
      }

      /**
      * Cast this to a fundamental type
      * @return unsigned int the value.
      */
      unsigned int CastToUnsignedInt() const
      {
        unsigned int value;
        CastToFundamental(value);
        return value;
      }

      /**
      * Cast this to a fundamental type
      * @return long the value.
      */
      long int CastToLongInt() const
      {
        long int value;
        CastToFundamental(value);
        return value;
      }

      /**
      * Cast this to a fundamental type
      * @return unsigned long the value.
      */
      unsigned long int CastToUnsignedLongInt() const
      {
        unsigned long int value;
        CastToFundamental(value);
        return value;
      }

      /**
      * Cast this to a fundamental type
      * @return long long the value.
      */
      long long int CastToLongLongInt() const
      {
        long long int value;
        CastToFundamental(value);
        return value;
      }

      /**
      * Cast this to a fundamental type
      * @return unsigned long long the value.
      */
      unsigned long long int CastToUnsignedLongLongInt() const
      {
        unsigned long long int value;
        CastToFundamental(value);
        return value;
      }

      /**
      * Try and cast this to a posible value.
      * @return char* the value we are looking for.
      */
      char* CastToChars() const
      {
        char* value;
        CastToCharacters(value);
        return value;
      }

      /**
      * Try and cast this to a posible value.
      * @param signed char* value the value we are looking for.
      */
      signed char* CastToSignedChars() const
      {
        return (signed char*)CastToChars();
      }

      /**
      * Try and cast this to a posible value.
      * @param unsigned char* value the value we are looking for.
      */
      unsigned char* CastToUnsignedChars() const
      {
        return (unsigned char*)CastToChars();
      }

      /**
      * Try and cast this to a posible value.
      * @return wchar_t* the value we are looking for.
      */
      wchar_t* CastToWideChars() const
      {
        wchar_t* value;
        CastToCharacters(value);
        return value;
      }

      /**
      * Try and cast this to a posible value.
      * @return wchar_t* the value we are looking for.
      */
      std::wstring CastToWideString(std::wstring& value) const
      {
        return CastToWideChars();
      }

      /**
      * Try and cast this to a posible value.
      * @return char the value we are looking for.
      */
      char CastToChar() const
      {
        switch (Type())
        {
        case dynamic::Misc_copy:
        case dynamic::Misc_copy_ptr:
          throw std::bad_cast();

        case dynamic::Misc_null:
          return '\0';

        case dynamic::Character_wchar_t:
          return static_cast<char>(*(wchar_t*)_cvalue);

        case dynamic::Character_char:
          return static_cast<char>(*(char*)_cvalue);

        case dynamic::Character_signed_char:
          return static_cast<char>(*(signed char*)_cvalue);

        case dynamic::Character_unsigned_char:
          return static_cast<char>(*(unsigned char*)_cvalue);

        default:
          return static_cast<char>(_llivalue);
        }
      }

      /**
      * Try and cast this to a posible value.
      * @return wchar_t the value we are looking for.
      */
      wchar_t CastToWideChar() const
      {
        switch (Type())
        {
        case dynamic::Misc_copy:
        case dynamic::Misc_copy_ptr:
          throw std::bad_cast();

        case dynamic::Misc_null:
          return '\0';

        case dynamic::Character_wchar_t:
          return static_cast<wchar_t>(*(wchar_t*)_cvalue);

        case dynamic::Character_char:
          return static_cast<wchar_t>(*(char*)_cvalue);

        case dynamic::Character_signed_char:
          return static_cast<wchar_t>(*(signed char*)_cvalue);

        case dynamic::Character_unsigned_char:
          return static_cast<wchar_t>(*(unsigned char*)_cvalue);

        default:
          return static_cast<wchar_t>(_llivalue);
        }
      }

      /**
      * Try and cast this to a posible value.
      * @return unsigned char the value we are looking for.
      */
      unsigned char CastToUnsignedChar() const
      {
        switch (Type())
        {
        case dynamic::Misc_copy:
        case dynamic::Misc_copy_ptr:
          throw std::bad_cast();

        case dynamic::Misc_null:
          return '\0';
          break;

        case dynamic::Character_wchar_t:
          return static_cast<unsigned char>(*(wchar_t*)_cvalue);
          break;

        case dynamic::Character_char:
          return static_cast<unsigned char>(*(char*)_cvalue);
          break;

        case dynamic::Character_signed_char:
          return static_cast<unsigned char>(*(signed char*)_cvalue);
          break;

        case dynamic::Character_unsigned_char:
          return static_cast<unsigned char>(*(unsigned char*)_cvalue);
          break;

        default:
          return static_cast<unsigned char>(_llivalue);
          break;
        }
      }

      /**
      * Try and cast this to a posible value.
      * @return unsigned char the value we are looking for.
      */
      signed char CastToSignedChar() const
      {
        switch (Type())
        {
        case dynamic::Misc_copy:
        case dynamic::Misc_copy_ptr:
          throw std::bad_cast();

        case dynamic::Misc_null:
          return '\0';

        case dynamic::Character_wchar_t:
          return static_cast<signed char>(*(wchar_t*)_cvalue);

        case dynamic::Character_char:
          return static_cast<signed char>(*(char*)_cvalue);

        case dynamic::Character_signed_char:
          return static_cast<signed char>(*(signed char*)_cvalue);

        case dynamic::Character_unsigned_char:
          return static_cast<signed char>(*(unsigned char*)_cvalue);

        default:
          return static_cast<signed char>(_llivalue);
        }
      }

      /**
      * Try and cast this to a posible value.
      * we have a specialised function as casting to bool can be inefficent.
      * @return bool the value we are looking for.
      */
      bool CastToBool() const
      {
        switch (Type())
        {
        case dynamic::Misc_copy:
        case dynamic::Misc_copy_ptr:
          throw std::bad_cast();

        case dynamic::Misc_null:
          // null is false/
          return false;

        case dynamic::Misc_unknown:
        case dynamic::Boolean_bool:
        case dynamic::Character_signed_char:
        case dynamic::Character_unsigned_char:
        case dynamic::Character_char:
        case dynamic::Character_wchar_t:
        case dynamic::Integer_short_int:
        case dynamic::Integer_unsigned_short_int:
        case dynamic::Integer_int:
        case dynamic::Integer_unsigned_int:
        case dynamic::Integer_long_int:
        case dynamic::Integer_unsigned_long_int:
        case dynamic::Integer_long_long_int:
        case dynamic::Integer_unsigned_long_long_int:
        case dynamic::Floating_point_float:
        case dynamic::Floating_point_double:
        case dynamic::Floating_point_long_double:
          break;

        default:
          throw std::runtime_error("Unknown data Type");
        }

        // if we are a float we must use it, in case we have 0.0001
        // if we were using the long long int then we would only have 0
        if (dynamic::is_type_floating(NumberType()))
        {
          return (_ldvalue != 0);
        }
        return  (_llivalue != 0);
      }

      /**
      * Cast this to a fundamental type
      * @return short int the value.
      */
      template<class T, typename = std::enable_if_t< std::is_pointer<T>::value> >
      T CastToCopyPtr() const
      {
        if (!dynamic::is_type_copy( Type() ))
        {
          // we cannot convert this to a trivial type.
          throw std::bad_cast();
        }

        // we konw, that we handle certain pointers, (strings, ints etc)
        // so there is no way that we can cast a trivial value to something
        // we know it cannot be, only unknown types are 'trivial'
        if (dynamic::Misc_unknown != dynamic::get_type< typename std::remove_pointer<T>::type >::value)
        {
          // we cannot cast to this T* as we know
          // that it was not what it was created with, (as we handle known pointers).
          throw std::bad_cast();
        }

        // are we a pointer or an actual value?
        if (Type() == dynamic::Misc_copy)
        {
          //  as we are not a pointer, we cannot use the pointer value.
          auto unknownItem = reinterpret_cast<UnknownItem<typename std::remove_pointer<T>::type>*>(_unkvalue);

          // did the cast work?
          if (nullptr == unknownItem)
          {
            throw std::bad_cast();
          }

          // we want the address of what we know is a structure.
          return unknownItem->Get();
        }

        // get the item value
        auto unknownItem = reinterpret_cast<UnknownItem<T>*>(_unkvalue);

        // did the cast work?
        if (nullptr == unknownItem)
        {
          throw std::bad_cast();
        }

        // we want the address of what we know is a pointer.
        return *unknownItem->Get();
      }

      /**
      * Cast this to a fundamental type
      * @return short int the value.
      */
      template<class T, typename = std::enable_if_t< !std::is_pointer<T>::value> >
      T CastToCopy() const
      {
        if (!dynamic::is_type_copy( Type() ))
        {
          // we cannot convert this to a trivial type.
          throw std::bad_cast();
        }

        // we konw, that we handle certain pointers, (strings, ints etc)
        // so there is no way that we can cast a trivial value to something
        // we know it cannot be, only unknown types are 'trivial'
        if (dynamic::Misc_unknown != dynamic::get_type< typename std::remove_pointer<T>::type >::value)
        {
          // we cannot cast to this T* as we know
          // that it was not what it was created with, (as we handle known pointers).
          throw std::bad_cast();
        }

        // are we a pointer or an actual value?
        if (Type() == dynamic::Misc_copy)
        {
          //  as we are not a pointer, we cannot use the pointer value.
          auto unknownItem = reinterpret_cast<UnknownItem<typename std::remove_pointer<T>::type>*>(_unkvalue);

          // did the cast work?
          if (nullptr == unknownItem)
          {
            throw std::bad_cast();
          }

          // we want the address of what we know is a structure.
          return *unknownItem->Get();
        }

        // get the item value
        auto unknownItem = reinterpret_cast<UnknownItem<T>*>(_unkvalue);

        // did the cast work?
        if (nullptr == unknownItem)
        {
          throw std::bad_cast();
        }

        // we want the address of what we know is a pointer.
        return *unknownItem->Get();
      }

      /**
      * Do common casting to known fundamental type.
      * T the 'fundamental' cast
      */
      template<class T>
      std::enable_if_t<!std::is_pointer<T>::value> CastToFundamental(T& value) const
      {
        switch (Type())
        {
        case dynamic::Misc_copy:
        case dynamic::Misc_copy_ptr:
          throw std::bad_cast();

        case dynamic::Misc_null:
          value = 0;
          break;

          // char
        case dynamic::Character_char:
        case dynamic::Character_unsigned_char:
        case dynamic::Character_signed_char:
        case dynamic::Character_wchar_t:
          if (dynamic::is_type_floating(NumberType()))
          {
            value = static_cast<T>(_ldvalue);
          }
          else
          {
            value = static_cast<T>(_llivalue);
          }
          break;

          // Integer
        case dynamic::Integer_unsigned_int:
        case dynamic::Integer_int:
        case dynamic::Integer_short_int:
        case dynamic::Integer_unsigned_short_int:
        case dynamic::Integer_long_int:
        case dynamic::Integer_unsigned_long_int:
        case dynamic::Integer_long_long_int:
        case dynamic::Integer_unsigned_long_long_int:
          value = static_cast<T>(_llivalue);
          break;

        case dynamic::Floating_point_double:
        case dynamic::Floating_point_float:
        case dynamic::Floating_point_long_double:
          value = static_cast<T>(_ldvalue);
          break;

        case dynamic::Boolean_bool:
          value = static_cast<T>(_ldvalue);
          break;

        default:
          // we cannot cast this.
          throw std::bad_cast();
        }
      }

      /**
      * Return a character
      * @return T* the character we want to return no.
      */
      template<class T>
      std::enable_if_t<std::is_pointer<T>::value> CastToCharacters(T& value) const
      {
        switch (Type())
        {
        case dynamic::Misc_copy:
        case dynamic::Misc_copy_ptr:
          throw std::bad_cast();

        case dynamic::Misc_null:
        case dynamic::Character_wchar_t:
          if (nullptr == _svalue)
          {
            const_cast<Any*>(this)->CreateString();
          }
          value = (T)_svalue->c_str();
          break;

        case dynamic::Character_char:
        case dynamic::Character_signed_char:
        case dynamic::Character_unsigned_char:
          value = static_cast<char*>(_cvalue);
          break;

        default:
          // do we need to create the string representation?
          if (nullptr == _svalue)
          {
            const_cast<Any*>(this)->CreateString();
          }
          value = (T)_svalue->c_str();
          break;
        }
      }

      /**
      * Return a character
      * std::wstring& value the character we want to return no.
      */
      void CastToCharacters(std::wstring& value) const
      {
        wchar_t* wc = nullptr;
        CastToCharacters(wc);
        value = std::wstring(wc);
      }

      /**
      * Return a character
      * std::string& value the character we want to return no.
      */
      void CastToCharacters(std::string& value) const
      {
        char* c = nullptr;
        CastToCharacters(c);
        value = std::string(c);
      }

      /**
      * Return a character
      * @return T* the character we want to return no.
      */
      void CastToCharacters(wchar_t*& value) const
      {
        switch (Type())
        {
        case dynamic::Misc_copy:
        case dynamic::Misc_copy_ptr:
          throw std::bad_cast();

        case dynamic::Character_wchar_t:
          value = static_cast<wchar_t*>((void*)_cvalue);
          break;

        case dynamic::Misc_null:
        case dynamic::Character_char:
        case dynamic::Character_signed_char:
        case dynamic::Character_unsigned_char:
          if (nullptr == _swvalue)
          {
            const_cast<Any*>(this)->CreateWideString();
          }
          value = const_cast<wchar_t*>(_swvalue->c_str());
          break;

        default:
          // do we need to create the string representation?
          if (nullptr == _swvalue)
          {
            const_cast<Any*>(this)->CreateWideString();
          }
          value = const_cast<wchar_t*>(_swvalue->c_str());
          break;
        }
      }

      /**
      * Return a character
      * @return T* the character we want to return no.
      */
      void CastToCharacters(char& value) const
      {
        char* c;
        CastToCharacters(c);
        value = (c != nullptr && _lcvalue > 0) ? c[0] : '\0';
      }

#ifdef _MSC_VER
# pragma endregion  CastTo - Cast *this to T&
#endif

      /**
      * Create the cosmetic representation of the string.
      */
      void CreateWideString()
      {
        //  do we need to do anyting?
        if (nullptr != _swvalue)
        {
          return;
        }

        // create the new string
        _swvalue = new std::wstring();

        // are we a char?
        switch (Type())
        {
        case dynamic::Misc_null:
          *_swvalue = L"";
          return;

        case dynamic::Character_char:
        case dynamic::Character_unsigned_char:
        case dynamic::Character_signed_char:
        {
          if (nullptr == _cvalue)
          {
            *_swvalue = L"";
            return;
          }

          using convert_typeX = std::codecvt_utf8<wchar_t>;
          std::wstring_convert<convert_typeX, wchar_t> converterX;
          *_swvalue = converterX.from_bytes((const char*)_cvalue);
          return;
        }

        case dynamic::Misc_unknown:
        case dynamic::Misc_copy:
        case dynamic::Misc_copy_ptr:
        case dynamic::Boolean_bool:
        case dynamic::Character_wchar_t:
        case dynamic::Integer_short_int:
        case dynamic::Integer_unsigned_short_int:
        case dynamic::Integer_int:
        case dynamic::Integer_unsigned_int:
        case dynamic::Integer_long_int:
        case dynamic::Integer_unsigned_long_int:
        case dynamic::Integer_long_long_int:
        case dynamic::Integer_unsigned_long_long_int:
        case dynamic::Floating_point_float:
        case dynamic::Floating_point_double:
        case dynamic::Floating_point_long_double:
          break;

        default:
          // unknown
          throw std::runtime_error("Unknown data Type");
        }

        if (dynamic::is_type_floating(NumberType()))
        {
          *_swvalue = std::to_wstring(_ldvalue);
        }
        else
        {
          *_swvalue = std::to_wstring(_llivalue);
        }
      }

      /**
      * Create the cosmetic representation of the string.
      */
      void CreateString()
      {
        //  do we need to do anyting?
        if (nullptr != _svalue)
        {
          return;
        }

        //  we will need a new string
        _svalue = new std::string();

        // are we a char?
        switch (Type())
        {
        case dynamic::Misc_null:
          *_svalue = "";
          return;

        case dynamic::Character_wchar_t:
        {
          if (nullptr == _cvalue)
          {
            *_svalue = "";
            return;
          }

          using convert_typeX = std::codecvt_utf8<wchar_t>;
          std::wstring_convert<convert_typeX, wchar_t> converterX;
          *_svalue = converterX.to_bytes((const wchar_t*)_cvalue);
          return;
        }

        case dynamic::Character_char:
        case dynamic::Character_unsigned_char:
        case dynamic::Character_signed_char:

        case dynamic::Misc_unknown:
        case dynamic::Misc_copy:
        case dynamic::Misc_copy_ptr:
        case dynamic::Boolean_bool:
        
        case dynamic::Integer_short_int:
        case dynamic::Integer_unsigned_short_int:
        case dynamic::Integer_int:
        case dynamic::Integer_unsigned_int:
        case dynamic::Integer_long_int:
        case dynamic::Integer_unsigned_long_int:
        case dynamic::Integer_long_long_int:
        case dynamic::Integer_unsigned_long_long_int:
        case dynamic::Floating_point_float:
        case dynamic::Floating_point_double:
        case dynamic::Floating_point_long_double:
          break;

        default:
          // unknown
          throw std::runtime_error("Unknown data Type");
        }

        if (dynamic::is_type_floating(NumberType()))
        {
          *_svalue = std::to_string(_ldvalue);
        }
        else
        {
          *_svalue = std::to_string(_llivalue);
        }
      }

      /**
      * Clean up the value(s)
      */
      void CleanValues()
      {
        // delete the char if need be
        delete _cvalue;

        // delete the cosmetic strings
        delete _svalue;
        delete _swvalue;

        // delete the unknown value
        if (_unkvalue)
        {
          --_unkvalue->_counter;
          if (0 == _unkvalue->_counter)
          {
            delete _unkvalue;
          }
        }

        // reset the values
        _llivalue = 0;
        _ldvalue = 0;
        _lcvalue = 0;
        _cvalue = nullptr;
        _svalue = nullptr;
        _swvalue = nullptr;
        _unkvalue = nullptr;
      }

      /**
      * depending on the type we return if we should use the unsigned integer in a formula
      * @return bool if we should use the long long int as an unsigned signed integer.
      */
      bool UseUnsignedInteger() const
      {
        // divide the values and set it.
        switch (NumberType())
        {
        case dynamic::Integer_unsigned_short_int:
        case dynamic::Integer_unsigned_int:
        case dynamic::Integer_unsigned_long_int:
        case dynamic::Integer_unsigned_long_long_int:
          return true;

        case dynamic::Misc_unknown:
        case dynamic::Misc_null:
        case dynamic::Misc_copy:
        case dynamic::Misc_copy_ptr:
        case dynamic::Boolean_bool:
        case dynamic::Character_signed_char:
        case dynamic::Character_unsigned_char:
        case dynamic::Character_char:
        case dynamic::Character_wchar_t:
        case dynamic::Integer_short_int:
        case dynamic::Integer_int:
        case dynamic::Integer_long_int:
        case dynamic::Integer_long_long_int:
        case dynamic::Floating_point_float:
        case dynamic::Floating_point_double:
        case dynamic::Floating_point_long_double:
          return false;
        }

        // unknown
        throw std::runtime_error("Unknown data Type");
      }

      /**
      * depending on the type we return if we should use the signed integer in a formula
      * @return bool if we should use the long long int as a signed integer.
      */
      bool UseSignedInteger() const
      {
        // divide the values and set it.
        switch (NumberType())
        {
        case dynamic::Integer_short_int:
        case dynamic::Integer_int:
        case dynamic::Integer_long_int:
        case dynamic::Integer_long_long_int:
          return true;

        case dynamic::Misc_unknown:
        case dynamic::Misc_null:
        case dynamic::Misc_copy:
        case dynamic::Misc_copy_ptr:
        case dynamic::Boolean_bool:
        case dynamic::Character_signed_char:
        case dynamic::Character_unsigned_char:
        case dynamic::Character_char:
        case dynamic::Character_wchar_t:
        case dynamic::Integer_unsigned_short_int:
        case dynamic::Integer_unsigned_int:
        case dynamic::Integer_unsigned_long_int:
        case dynamic::Integer_unsigned_long_long_int:
        case dynamic::Floating_point_float:
        case dynamic::Floating_point_double:
        case dynamic::Floating_point_long_double:
          return false;
        }

        // unknown
        throw std::runtime_error("Unknown data Type");
      }

      /**
      * Divide *this number with T number.
      * @param dynamic::Type type the type we want to set the value with.
      * @param T number the number we will be dividing with.
      * @return *this the divided number.
      */
      template<class T>
      Any& DivideNumber(dynamic::Type type, T number)
      {

        // check for division by zero.
        if (number == 0)
        {
          // *this / null = std::overflow_error
          throw std::overflow_error("Division by zero.");
        }

        // divide the values and set it.
        if (UseUnsignedInteger())
        {
          CreateFrom((unsigned long long int)_llivalue / (long double)number);
        }
        else if (UseSignedInteger())
        {
          CreateFrom((long long int)_llivalue / (long double)number);
        }
        else
        {
          CreateFrom(_ldvalue / (long double)number);
        }

        // update the type.
        _type = type;

        // return the value.
        return *this;
      }

      /**
      * Multiply T number and *this number.
      * @param dynamic::Type type the type we want to set the value with.
      * @param T number the number we will be adding.
      * @return *this the multiplied number.
      */
      template<class T>
      Any& MultiplyNumber(dynamic::Type type, T number)
      {
        // add the values.
        if (dynamic::is_type_floating(type))
        {
          CreateFrom(_ldvalue * number);
        }
        else
        {
          CreateFrom(_llivalue * number);
        }

        // update the type.
        _type = type;

        // done
        return *this;
      }

      /**
      * Add T number to *this number.
      * @param dynamic::Type type the type we want to set the value with.
      * @param T number the number we will be adding.
      * @return *this the added number.
      */
      template<class T>
      Any& AddNumber(dynamic::Type type, T number)
      {
        if (UseUnsignedInteger())
        {
          CreateFrom((unsigned long long int)_llivalue + number);
        }
        else if (UseSignedInteger())
        {
          CreateFrom((long long int)_llivalue + number);
        }
        else
        {
          CreateFrom(_ldvalue + number);
        }

        // update the type.
        _type = type;

        // done
        return *this;
      }

      /**
      * Subtract T number from *this.
      * @param dynamic::Type type the type we want to set the value with.
      * @param T number the number we will be subtracting from this.
      * @return *this the subtracted number.
      */
      template<class T>
      Any& SubtractNumber(dynamic::Type type, T number)
      {
        if (UseUnsignedInteger())
        {
          CreateFrom((unsigned long long int)_llivalue - number);
        }
        else if (UseSignedInteger())
        {
          CreateFrom((long long int)_llivalue - number);
        }
        else
        {
          CreateFrom(_ldvalue - number);
        }

        // update the type.
        _type = type;

        // done
        return *this;
      }

      /**
      * Check if our string is a number or not.
      * @param bool allowPartial if partial strings are allowed or not.
      * @return bool if this string represents a number or not.
      */
      bool IsStringNumber(bool allowPartial) const
      {
        switch (_stringStatus)
        {
        case dynamic::Any::StringStatus_Partial_Pos_Number:
        case dynamic::Any::StringStatus_Partial_Neg_Number:
        case dynamic::Any::StringStatus_Floating_Partial_Pos_Number:
        case dynamic::Any::StringStatus_Floating_Partial_Neg_Number:
          return allowPartial; // only true if we allow partials.

        case dynamic::Any::StringStatus_Floating_Pos_Number:
        case dynamic::Any::StringStatus_Floating_Neg_Number:
        case dynamic::Any::StringStatus_Pos_Number:
        case dynamic::Any::StringStatus_Neg_Number:
          return true;

        case dynamic::Any::StringStatus_Not_A_Number:
        default:
          break;
        }

        // not a number
        return false;
      }

      /**
      * Parse a string to see if it is a number, partial or not.
      * @param const char *str the string we are parsing.
      * @param size_t sourceLen the source len
      */
      void ParseStringStatus(const char *source, size_t sourceLen)
      {
        //  call the const char* equivalent.
        ParseStringStatus(source, sourceLen, '+', '-', '.', '\0' );
      }

      /**
      * Parse a string to see if it is a number, partial or not.
      * @param const wchar_t *str the string we are parsing.
      * @param size_t sourceLen the source len
      */
      void ParseStringStatus(const wchar_t *source, size_t sourceLen)
      {
        //  call the const wide char* equivalent.
        ParseStringStatus(source, sourceLen, L'+', L'-', L'.', L'\0');
      }

      /**
      * check if this is a digit char, (0-9)
      * @param const char c the character we are checking.
      * @return bool if the number is a digit or not.
      */
      inline bool _isdigit(const char c) const { return (c >= -1 && c <= 255) && (isdigit(c) != 0); }

      /**
      * check if this is a space wide char
      * @param const wchar_t c the character we are checking.
      * @return bool if the char is a space or not.
      */
      inline bool _isspace(const char c) const { return (c >= -1 && c <= 255) && (isspace(c) != 0); }

      /**
      * check if this is a digit wide char, (0-9)
      * @param const wchar_t c the character we are checking.
      * @return bool if the number is a digit or not.
      */
      inline bool _isdigit(const wchar_t c)const { return (c >= -1 && c <= 255) && (iswdigit(c) != 0); }

      /**
      * check if this is a space wide char
      * @param const wchar_t c the character we are checking.
      * @return bool if the char is a space or not.
      */
      inline bool _isspace(const wchar_t c)const { return (c >= -1 && c <= 255) && (iswspace(c) != 0); }

      /**
      * Parse a string to check if it is a number or not.
      * -0 and +0 keep their sign and 12.00 remains a floating point.
      * because this is how it was pased to us, it is up to the user to make sure they pass
      * a valid number that makes it posible to investigate.
      * @param const T* source the string we are checking.
      * @param size_t sourceLen the len of the 'string' we are checking.
      * @param const T str_plus the plus sign, ('+')
      * @param const T str_minus the minus sign, ('-')
      * @param const T str_decimal how a decimal is represented, , ('.')
      * @param const T str_eol the eol character, ('\0')
      */
      template<typename T>
      void ParseStringStatus(const T* source, size_t sourceLen, const T str_plus, const T str_minus, const T str_decimal, const T str_eol)
      {
        // sanity check
        if (nullptr == source)
        {
          // null is not a number
          _stringStatus = StringStatus_Not_A_Number;
          return;
        }

        short sign = 0;       //  0=unknown, 1=positive, 2=negative.
        short found = 0;      // the number of ... numbers we found.
        bool partial = false; // if we found some non characters.
        bool decimal = false;

        // go around all the characters.
        auto loopLen = size_t(sourceLen / sizeof(T));
        for ( size_t i = 0; i < loopLen; i+=1 )
        {
          // get the  character.
          const T *it = (source + i);

          //  is it a space?
          if (_isspace(*it))
          {
            continue;
          }

          // minus sign? as long as we have not done it yet...
          if (*it == str_minus && sign == 0)
          {
            sign = 2;
            continue;
          }

          // + sign? as long as we have not done it yet...
          if (*it == str_plus && sign == 0)
          {
            sign = 1;
            continue;
          }

          // is it a decimal? as long as we have done done it.
          if (*it == str_decimal && false == decimal)
          {
            decimal = true;
            continue;
          }

          // we are done? 
          if (*it == str_eol)
          {
            // this might not be the end of the string, 
            // but it cannot be a number any more
            // if i == sourceLen then it is the end of the string, (and not partial)
            // otherwise it is the end, (and partial)
            partial = (i+1 != loopLen );
            break;
          }

          // is it a digit?
          if (!_isdigit(*it))
          {
            // not a number, (anymore)
            partial = true;
            break;
          }

          // this is a number
          ++found;
        }

        //
        //  using all the values together we need to check what king of string this is.
        //
        if (0 == found)
        {
          // we found no number at all, so it cannot be a string.
          // of by the time we found a non string, we had no number.
          _stringStatus = StringStatus_Not_A_Number;
        }
        else if (true == partial)
        {
          if (sign == 1 || sign == 0)
          {
            //  '+' sign or no sign - it is positive.
            _stringStatus = (decimal) ? StringStatus_Floating_Partial_Pos_Number : StringStatus_Partial_Pos_Number;
          }
          else if (sign == 2)
          {
            // -ve sign.
            _stringStatus = (decimal) ? StringStatus_Floating_Partial_Neg_Number : StringStatus_Partial_Neg_Number;
          }
        }
        else
        {
          if (sign == 1 || sign == 0)
          {
            //  '+' sign or no sign - it is positive.
            _stringStatus = (decimal) ? StringStatus_Floating_Pos_Number : StringStatus_Pos_Number;
          }
          else if (sign == 2)
          {
            // -ve sign.
            _stringStatus = (decimal) ? StringStatus_Floating_Neg_Number : StringStatus_Neg_Number;
          }
        }
      }

      struct UnknownItemBase
      {
        UnknownItemBase() : _counter(1) {}
        virtual ~UnknownItemBase() { }
        unsigned short _counter;

        virtual void* Data() const = 0;
        virtual size_t Size() const = 0;
        virtual bool Equal(void* to) const = 0;
        virtual bool IsTrivial() const = 0;
      };

      template<class T>
      struct UnknownItem : UnknownItemBase
      {
        UnknownItem(const T& value) : UnknownItemBase(), _value(nullptr)
        {
          _value = new T(value);
        }
        ~UnknownItem()
        {
          delete _value;
        }
        T* Get() const { return _value; }

        virtual void* Data() const { return (void*)_value; }
        virtual size_t Size() const { return sizeof(T); }

        template<class Q = T>
        std::enable_if_t<::myodd::_Check::EqualExists<Q, Q>::value, bool > _Compare(void* to) const {
          Q* toWhat = reinterpret_cast<Q*>(to);
          if (!toWhat) {
            return false;
          }
          return ((*_value) == (*toWhat));
        }

        template<class Q = T>
        std::enable_if_t<!::myodd::_Check::EqualExists<Q, Q>::value, bool > _Compare(void* to) const {
          if (!IsTrivial())
          {
            throw std::runtime_error("Trying to compare 2 items, but this class does have an equal(==) operator.");
          }
          return (0 == std::memcmp(_value, to, Size()));
        }

        virtual bool Equal(void* to) const {
          return _Compare(to);
        }

        virtual bool IsTrivial() const {
          return std::is_trivially_copyable<T>::value;
        }

      protected:
        T* _value;
      };
     
 
      UnknownItemBase* _unkvalue;

      // the biggest integer value.
      long long int _llivalue;

      // the biggest floating point value
      long double _ldvalue;

      // this is the given character value either char/signed char/unsigned char/wide
      char* _cvalue;
      size_t _lcvalue;

      // the status of the string.
      StringStatus _stringStatus;

      // 'cosmetic' representations of the numbers, both wide and non wide strings.
      // the values are only created if/when the caller call a to string function.
      std::string* _svalue;
      std::wstring* _swvalue;

      // the variable type
      dynamic::Type _type;
    };
  }
}
