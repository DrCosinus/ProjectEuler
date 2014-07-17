#include "stdafx.h"
#include "Model.h"

#include "ClassDef.h"

#include <vector>
#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>


// insparation: http://www.enchantedage.com/cpp-reflection

template<typename... Ts>
class AutoRun
{
public:
    AutoRun(std::function<void (Ts...)> F, Ts... args)
    {
        F(args...); 
    }
};

#include <array>

namespace Object
{
    // c'est ModelDeclaration qui contient la structure du Model
   

    // voir: http://www.codeproject.com/Articles/318690/C-Non-intrusive-enum-class-with-reflection-supp

    class Serializer
    {
    public:
        template<typename T> void operator()(T& aValue, char* aName);
    };


    class Object abstract
    {
        std::string myName;
    protected:
        Object(std::string&& aName) :
            myName(aName)
        {
        }
    public:
        const std::string& name() const { return myName; }
        void name(const std::string& aName) { myName = aName; }

        // o->myModelDeclaration.myFields[1]->fromString(o, "1972");

    };



    class Light : public Object
    {
    private:
        float myIntensity;

    public:
        Light(float anIntensity) :
            Object("xxx"),
            myIntensity(anIntensity) 
        {
        }
        Light() = delete;
        Light(const Light&) = delete;
        Light(Light&&) = delete;
        Light& operator=(const Light&) = delete;
        Light& operator=(Light&&) = delete;

        template<typename S>
        void Serialize(S& aSerializer)
        {
            aSerializer(myIntensity, "myIntensity");
        }
    };

    class OmniLight : public Light
    {
    public:
        float myRadius;
        int myFoo;
    public:
        OmniLight(float anIntensity, float aRadius) : 
            Light(anIntensity),
            myRadius(aRadius),
            myFoo(707)
        {
        }

        template<typename S>
        void Serialize(S& aSerializer)
        {
            Light::Serialize(aSerializer);
            aSerializer(myRadius, "myRadius");
            aSerializer(myFoo, "myFoo");
        }
    };
}

struct ConsoleWriter
{
    template<typename T>
    void operator()(T& aValue, const char* aValueName)
    {
        std::cout << aValueName << " ";

        std::cout << aValue;

        //std::cout << " (from '" << md.myName << "')";
        
        std::cout << std::endl;
    }
};

template<typename T>
void Dump(Object::T* anObject)
{
    anObject->Serialize(ConsoleWriter());
};

class BaseHolder
{
public:
    virtual ~BaseHolder() {}
};

template<typename T>
class HoldData : public BaseHolder
{
public:
    HoldData(const T& aValue) : myValue(aValue) {}
private:
    T myValue;
};

class Variant
{
public:
    template<typename T>
    Variant(const T& t) : myData(std::make_unique<HoldData<T>>(t)) {}
    //template<typename T>
    //T& operator T() { return reinterpret_cast<HoldData<T>>(myData).myValue; }
private:
    std::unique_ptr<BaseHolder> myData;
};


struct ValueWriter
{
    const void* myValue;
    const size_t myValueSize;
    const char* myValueName;
    const type_info& myTypeInfo;

    template<typename T>
    ValueWriter(const char* aValueName, const T& aValue)
        : myValueName(aValueName)
        , myValue(new T(aValue))
        , myTypeInfo(typeid(T))
        , myValueSize(sizeof(T))
    {
    }
    template<typename T>
    void operator()(T& aValue, const char* aValueName)
    {
        if (myTypeInfo==typeid(T) && strcmp(myValueName, aValueName) == 0)
        {
            aValue = *(T*)myValue;
        }
    }

};

void NewTest()
{
    auto l = new Object::Light(42.7f);
    auto o = new Object::OmniLight(509.13f, 17.8f);

    //o->myModelDeclaration["myRadiusX"].fromString(o, "1972");
    //o->myModelDeclaration["myRadius"].fromString(o, "1977");

    o->Serialize(ValueWriter("myRadius", 1977.f));

    Dump(l);
    Dump(o);

    //std::cout << "kilt " << l->myModelDeclaration.myName << " ... " << std::endl;
}

namespace _
{
    AutoRun<> _(NewTest);
}

// Hidden in plain sight
// Tower fall