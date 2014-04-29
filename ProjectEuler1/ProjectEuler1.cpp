// ProjectEuler1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cassert>
#include <memory>
#include <vector>
#include <iostream>
//#include <ostream>
//#include <type_traits>

namespace console
{
    template <typename valueType>
    void writeLine(const valueType& aValue)
    {
        std::cout << aValue << std::endl;
    }

    template <typename firstValueType, typename... otherValueTypes>
    void writeLine(const firstValueType& aFirstValue, const otherValueTypes&... someOtherValues)
    {
        std::cout << aFirstValue;
        writeLine(someOtherValues...);
    }
    void writeLine()
    {
        std::cout << std::endl;
    }


    template <typename valueType>
    void write(valueType aValue)
    {
        std::cout << aValue;
    }

    template <typename firstValueType, typename... otherValueTypes>
    void write(firstValueType aFirstValue, otherValueTypes... someOtherValues)
    {
        std::cout << aFirstValue;
        write(someOtherValues...);
    }
}

template <unsigned int ourDigitCount = 10, typename digitSetType = unsigned long /*long*/>
class StaticDigitCountInteger
{
public:
    static const unsigned int ourDigitCount = ourDigitCount;
    static const unsigned int ourSetByteCount = sizeof(digitSetType);
    static const unsigned int ourSetBitCount = ourSetByteCount * 8;
    static const unsigned int ourBitPerDigit = 4;
    static const unsigned int ourDigitMask = (1 << ourBitPerDigit) - 1;
    static const unsigned int ourDigitPerSet = ourSetBitCount / ourBitPerDigit;
    static const unsigned int ourArraySize = (ourDigitCount - 1) / ourDigitPerSet + 1;

    typedef std::vector<digitSetType> ourArrayType;
private:
    ourArrayType myArray;
public:
    // For now, I disable the copy constructor and the copy assignment operator to fail to compile if one uses one of them by mistake.
    StaticDigitCountInteger(const StaticDigitCountInteger&) = delete; // no copy constructor
    StaticDigitCountInteger& operator=(const StaticDigitCountInteger&) = delete; // no copy assignment

    StaticDigitCountInteger()
    {
        myArray.resize(ourArraySize);
    }
    StaticDigitCountInteger(StaticDigitCountInteger&& anInteger) : StaticDigitCountInteger()
    {
        myArray = std::move(anInteger.myArray);
    }
    StaticDigitCountInteger& operator=(StaticDigitCountInteger&& anInteger)
    {
        myArray = std::move(anInteger.myArray);
        return *this;
    }
    StaticDigitCountInteger(unsigned int anInteger) : StaticDigitCountInteger()
    {
        for (int i = 0; i < ourArraySize; ++i)
        {
            digitSetType set = 0;
            for (int k = 0; k < ourDigitPerSet; ++k)
            {
                set |= (anInteger % 10) << (k*ourBitPerDigit);
                anInteger /= 10;
            }
            myArray[i] = set;
        }
    }
private:
    static unsigned int getDigitFromSetAt(digitSetType aSet, unsigned int anIndexInSet)
    {
        assert(anIndexInSet < ourDigitPerSet);
        return (aSet >> (ourBitPerDigit*anIndexInSet)) & ourDigitMask;
    }
public:
    unsigned int digitAt(unsigned int anIndex) const
    {
        return  getDigitFromSetAt(myArray[anIndex / ourDigitPerSet], anIndex%ourDigitPerSet);
    }
    unsigned int digitCount() const
    {
        for (unsigned int i = ourArraySize - 1; i >= 0; --i)
        {
            digitSetType set = myArray[i];
            if (set == 0)
                continue;
            
            for (unsigned int k = ourDigitPerSet - 1; k >= 0; --k)
            {
                if (getDigitFromSetAt(set, k))
                {
                    return i*ourDigitPerSet + k + 1;
                }
            }
        }
        return 0;
    }

    bool operator==(const StaticDigitCountInteger& aNumber)
    {
        for (int i = 0; i < ourArraySize; ++i)
        {
            if (myArray[i] != aNumber.myArray[i])
            {
                return false;
            }
        }
        return true;
    }

    StaticDigitCountInteger& operator++()
    {
        for (int i = 0; i < ourArraySize; ++i)
        {
            digitSetType set = myArray[i];
            digitSetType res = 0;
            digitSetType carry = 1;
            for (int k = 0; k < ourDigitPerSet; ++k)
            {
                digitSetType sum = (set & ourDigitMask) + carry;
                set >>= ourBitPerDigit;
                if (sum >= 10)
                {
                    sum -= 10;
                }
                else
                {
                    carry = 0;
                }
                res >>= ourBitPerDigit;
                res |= sum << ((ourDigitPerSet - 1) * ourBitPerDigit);
            }
            myArray[i] = res;
            if (carry == 0)
                break;
        }
        return *this;
    }
    StaticDigitCountInteger& operator++(int)
    {
        return operator++();
    }

    StaticDigitCountInteger& operator+= (const StaticDigitCountInteger& aNumber)
    {
        digitSetType carry = 0;
        for (int i = 0; i < ourArraySize; ++i)
        {
            digitSetType op1 = myArray[i];
            digitSetType op2 = aNumber.myArray[i];
            digitSetType res = 0;
            for (int j = 0; j < ourDigitPerSet; ++j)
            {
                digitSetType sum = getDigitFromSetAt(op1, j) + getDigitFromSetAt(op2, j) + carry;
                if (sum < 10)
                {
                    carry = 0;
                }
                else
                {
                    sum -= 10;
                    carry = 1;
                }
                res |= sum << (ourBitPerDigit * j);
            }
            myArray[i] = res;
        }
        return *this;
    }

    bool divisibleBy5() const
    {
        auto digit0 = digitAt(0);
        return digit0 == 0 || digit0 == 5;
    }
    bool divisibleBy3() const
    {
        unsigned int sum = 0;
        for (int i = 0; i < ourArraySize; ++i)
        {
            auto set = myArray[i];
            for (int k = 0; k < ourDigitPerSet; ++k)
            {
                sum += set & ourDigitMask;
                set >>= ourBitPerDigit;
            }
        }
        return (sum % 3) == 0;
    }

    static void DebugInfos()
    {
        //console::writeLine("sizeof()    = ", sizeof(StaticDigitCountInteger));
        console::writeLine("DigitCount  = ", ourDigitCount);
        console::writeLine("SetByteCount= ", ourSetByteCount);
        console::writeLine("SetBitCount = ", ourSetBitCount);
        console::writeLine("BitPerDigit = ", ourBitPerDigit);
        console::writeLine("DigitPerSet = ", ourDigitPerSet);
        console::writeLine("ArraySize   = ", ourArraySize);
        console::writeLine();
    }
};



template <unsigned int ourDigitCount, typename digitSetType>
std::ostream& operator<<(std::ostream& os, const StaticDigitCountInteger<ourDigitCount, digitSetType>& anInteger)
{
    auto n = anInteger.digitCount();
    if (n == 0)
        n = 1;
    for (int i = n - 1; i >= 0; --i)
        os << anInteger.digitAt(i);
    return os;
}

#include <chrono>
namespace profile
{
    class stopwatch
    {
        typedef std::chrono::high_resolution_clock::duration    duration_type;
        typedef std::chrono::high_resolution_clock::time_point  time_point_type;

        time_point_type myStart;
        duration_type   myDuration;
        bool            myIsRunning;
    public:
        stopwatch(bool anAutoStart = false) : myIsRunning(false)
        {
            if (anAutoStart)
                start();
        }
        void start()
        {
            if (!myIsRunning)
            {
                myIsRunning = true;
                myStart = std::chrono::high_resolution_clock::now();
            }
        }
        void stop()
        {
            if (myIsRunning)
            {
                auto end = std::chrono::high_resolution_clock::now();
                myDuration += end - myStart;
                myIsRunning = false;
            }
        }
        void reset()
        {
            myIsRunning = false;
            myDuration = duration_type::zero();
        }
        duration_type elapsed()
        {
            return myIsRunning ? myDuration + std::chrono::high_resolution_clock::now() - myStart : myDuration;
        }
    };
}

static long long toMilli(const std::chrono::high_resolution_clock::duration& aDuration)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(aDuration).count();
}
static long long toMicro(const std::chrono::high_resolution_clock::duration& aDuration)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(aDuration).count();
}
static long long toNano(const std::chrono::high_resolution_clock::duration& aDuration)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(aDuration).count();
}

int _tmain(int argc, _TCHAR* argv[])
{
    // DEBUG
    {
        //StaticDigitCountInteger<>::DebugInfos();
    }

    // #001
    {
        profile::stopwatch sw(true);
        StaticDigitCountInteger<4> big(0);
        StaticDigitCountInteger<4> sum(0);
        for (int i = 0; i < 1000; ++i, ++big)
        {
            if (big.divisibleBy3() || big.divisibleBy5())
            {
                sum += big;
            }
        }
        sw.stop();
        auto time = toMilli(sw.elapsed());
        console::writeLine("sum  = ", sum);
        console::writeLine("time = ", time, "ms");
    }

    // #025
    {
        profile::stopwatch sw(true);
        StaticDigitCountInteger<1000> a(1);
        StaticDigitCountInteger<1000> b(1);
        unsigned int n = 2;
        while (a.digitCount() != 1000)
        {
            b += a;
            std::swap(a, b);
            ++n;
        }
        sw.stop();
        auto time = toMilli(sw.elapsed());
        console::writeLine("n    = ", n);
        console::writeLine("time = ", time, "ms");
    }

    // #038 (not resolved yet: for now only counting permutation of 1 to 9 number)
    {
        profile::stopwatch sw(true);
        int n = 0;
        for (int i0 = 1; i0 <= 9; ++i0)
        {
            int mask0 = 1 << i0;
            for (int i1 = 1; i1 <= 9; ++i1)
            {
                //if (mask0 & (1 << i1))
                if (i0 != i1)
                    continue;
                int mask1 = mask0 | (1 << i1);
                for (int i2 = 1; i2 <= 9; ++i2)
                {
                    //if (mask1 & (1 << i2))
                    if (i2 != i1 && i2 != i0)
                        continue;
                    int mask2 = mask1 | (1 << i2);
                    for (int i3 = 1; i3 <= 9; ++i3)
                    {
                        if (mask2 & (1 << i3))
                            continue;
                        int mask3 = mask2 | (1 << i3);
                        for (int i4 = 1; i4 <= 9; ++i4)
                        {
                            if (mask3 & (1 << i4))
                                continue;
                            int mask4 = mask3 | (1 << i4);
                            for (int i5 = 1; i5 <= 9; ++i5)
                            {
                                if (mask4 & (1 << i5))
                                    continue;
                                int mask5 = mask4 | (1 << i5);
                                for (int i6 = 1; i6 <= 9; ++i6)
                                {
                                    if (mask5 & (1 << i6))
                                        continue;
                                    int mask6 = mask5 | (1 << i6);
                                    for (int i7 = 1; i7 <= 9; ++i7)
                                    {
                                        if (mask6 & (1 << i7))
                                            continue;
                                        int mask7 = mask6 | (1 << i7);
                                        for (int i8 = 1; i8 <= 9; ++i8)
                                        {
                                            if (mask7 & (1 << i8))
                                                continue;
                                            //int mask8 = mask7 | (1 << i8);
                                            ++n;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        sw.stop();
        auto time = toMilli(sw.elapsed());
        console::writeLine("n    = ", n);
        console::writeLine("time = ", time, "ms");
    }
    return 0;
}

