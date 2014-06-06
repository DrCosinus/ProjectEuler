#pragma once

#include <cstddef>

struct FieldInfo
{
    const char* myName;
    const size_t myOffset;
    const size_t mySize;

    FieldInfo(const char* const aName, size_t anOffset, size_t aSize)
        : myName(aName)
        , myOffset(anOffset)
        , mySize(aSize)
    {
    }
    FieldInfo() = delete;
    FieldInfo(const FieldInfo&) = delete;
    FieldInfo(FieldInfo&& aFieldInfo) 
        : myName( aFieldInfo.myName )
        , myOffset(aFieldInfo.myOffset )
        , mySize(aFieldInfo.mySize)
    {
    }
};

template<typename T>
struct FieldInfoT : public FieldInfo
{
    const T myDefaultValue;
    FieldInfoT(const char* const aName, size_t anOffset, size_t aSize, T aDefaultValue)
        : FieldInfo(aName, anOffset, aSize)
        , myDefaultValue(aDefaultValue)
    {
    }

};

class plop
{
public:
    float myRadius = 0.0f;
    float myIntensity = 0.0f;
};


template<typename T>
struct ClassInfo
{
    static const char* const ourName;
    static const FieldInfo ourFieldInfos[];
};

template<typename T>
const char* const ClassInfo<T>::ourName = "Unknown";

template<typename T>
const char* const ClassInfo<T>::ourFieldInfos;


const char* const ClassInfo<plop>::ourName = "plop";

const FieldInfo ClassInfo<plop>::ourFieldInfos[] =
{ 
    FieldInfoT<float>("myRadius", offsetof(plop, myRadius), sizeof(plop::myRadius), 0.0f),
    FieldInfoT<float>("myIntensity", offsetof(plop, myIntensity), sizeof(plop::myIntensity), 0.0f),
};
