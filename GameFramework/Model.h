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
    typedef T type;
    //const T myDefaultValue;
    FieldInfoT(const char* const aName, size_t anOffset, size_t aSize/*, T aDefaultValue*/)
        : FieldInfo(aName, anOffset, aSize)
        //, myDefaultValue(aDefaultValue)
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


//const char* const ClassInfo<plop>::ourName = "plop";
//
//const FieldInfo ClassInfo<plop>::ourFieldInfos[] =
//{ 
//    FieldInfoT<float>("myRadius", offsetof(plop, myRadius), sizeof(plop::myRadius), 0.0f),
//    FieldInfoT<float>("myIntensity", offsetof(plop, myIntensity), sizeof(plop::myIntensity), 0.0f),
//};

#define CLASSINFO(_CLASS_NAME_) \
/*{ typedef _CLASS_NAME_ _type_;*/ \
const char* const ClassInfo<_CLASS_NAME_>::ourName = #_CLASS_NAME_;
#define CLASSINFO_FIELDINFO_BEGIN(_CLASS_NAME_) const FieldInfo ClassInfo<_CLASS_NAME_>::ourFieldInfos[] = {

#define CLASSINFO_FIELDINFO(_CLASS_NAME_, _FIELD_NAME) FieldInfoT<decltype(_CLASS_NAME_::_FIELD_NAME)>(#_FIELD_NAME, offsetof(_CLASS_NAME_, _FIELD_NAME), sizeof(_CLASS_NAME_::_FIELD_NAME)),

#define CLASSINFO_FIELDINFO_END };

//typedef plop _type;
using _type = plop;

CLASSINFO(plop)
CLASSINFO_FIELDINFO_BEGIN(plop)
CLASSINFO_FIELDINFO(plop, myRadius)
CLASSINFO_FIELDINFO(plop, myIntensity)
CLASSINFO_FIELDINFO_END

#undef _type
_type lol;

