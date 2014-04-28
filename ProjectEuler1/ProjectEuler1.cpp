// ProjectEuler1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <memory>
#include <array>
#include <iostream>
//#include <type_traits>

template <typename valueType>
void logLine(valueType aValue)
{
    std::cout << aValue << std::endl;
}

template <typename firstValueType, typename... otherValueTypes>
void logLine(firstValueType aFirstValue, otherValueTypes... someOtherValues)
{
    std::cout << aFirstValue;
    logLine(someOtherValues...);
}

template <unsigned int ourDigitCount=10, typename digitSetType=unsigned long long>
class StaticDigitCountInteger
{
public:
    static const unsigned int ourDigitCount = ourDigitCount;
    static const unsigned int ourSetByteCount = sizeof(digitSetType);
    static const unsigned int ourSetBitCount = ourSetByteCount * 8;
    static const unsigned int ourBitPerDigit = 4;
    static const unsigned int ourDigitPerSet = ourSetBitCount / ourBitPerDigit;
    static const unsigned int ourArraySize = (ourDigitCount - 1) / ourDigitPerSet + 1;

    typedef std::array<digitSetType, ourArraySize> ourArrayType;
private:
    ourArrayType myArray;
public:
    StaticDigitCountInteger(unsigned int anInteger)
    {
        for (int i = 0; i < ourArraySize; ++i)
        {
            digitSetType set = digitSetType(0);
            for (int k = 0; k < ourDigitPerSet; ++k)
            {
                set |= (anInteger % 10) << (k*ourBitPerDigit);
                anInteger /= 10;
            }
            myArray[i] = set;
        }
    }
    //StaticDigitCountInteger& operator++()
    //{
    //    for (int i = 0; i < ourArraySize; ++i)
    //    {
    //        digitSetType set = myArray[i];
    //        for (int k = 0; k < ourDigitPerSet; ++k)
    //        {
    //            set |= (anInteger % 10) << (k*ourBitPerDigit);
    //            anInteger /= 10;
    //        }
    //        myArray[i] = set;
    //    }
    //}

    unsigned int digitAt(unsigned int anIndex)
    {
        return (myArray[anIndex / ourDigitPerSet] >> (ourBitPerDigit*(anIndex%ourDigitPerSet))) & 15;
    }
    bool divisibleBy5()
    {
        auto digit0 = digitAt(0);
        return digit0 == 0 || digit0 == 5;
    }
    bool divisibleBy3()
    {
        unsigned int sum = 0;
        for (int i = 0; i < ourArraySize; ++i)
        {
            auto set = myArray[i];
            for (int k = 0; k < ourDigitPerSet; ++k)
            {
                sum += set & 15;
                set >>= 4;
            }
        }
        return (sum % 3) == 0;
    }
};

int _tmain(int argc, _TCHAR* argv[])
{
    logLine("sizeof(StaticDigitCountInteger<>)=", sizeof(StaticDigitCountInteger<>));
    logLine("DigitCount  =", StaticDigitCountInteger<>::ourDigitCount);
    logLine("SetByteCount=", StaticDigitCountInteger<>::ourSetByteCount);
    logLine("SetBitCount =", StaticDigitCountInteger<>::ourSetBitCount);
    logLine("BitPerDigit =", StaticDigitCountInteger<>::ourBitPerDigit);
    logLine("DigitPerSet =", StaticDigitCountInteger<>::ourDigitPerSet);
    logLine("ArraySize   =", StaticDigitCountInteger<>::ourArraySize);

    StaticDigitCountInteger<> a(1230);
    auto d3 = a.divisibleBy3();
    auto d5 = a.divisibleBy5();
    return 0;
}

