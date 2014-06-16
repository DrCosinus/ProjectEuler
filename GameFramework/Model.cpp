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
    enum class FieldType
    {
        Integer,
        Float,
        String,
    };

    template<typename T>
    struct FieldTypeOf;

    template<>
    struct FieldTypeOf < int >
    {
        static const FieldType value = FieldType::Integer;
    };

    template<>
    struct FieldTypeOf < float >
    {
        static const FieldType value = FieldType::Float;
    };

    //typedef std::tuple<FieldTypeOf<int>, FieldTypeOf<float>> AllFieldTypeOf;

    //template<FieldType ftype, typename T>
    //struct FieldTypeType;

    //template<FieldType ftype>
    //struct FieldTypeType < ftype, std::tuple<> >
    //{
    //    typedef nullptr_t type;
    //};

    //template<FieldType ftype, typename First, typename... Others>
    //struct FieldTypeType <ftype, std::tuple<First, Others...> >
    //{
    //    typedef typename std::conditional < First::value == ftype, First, typename FieldTypeType<type, std::tuple<Others...>>::type>::type type;
    //};

    struct FieldDeclaration
    {
        const char* myName;
        FieldType myType;
        std::size_t myOffset;
        std::size_t mySize;
        bool myIsPtr;

        FieldDeclaration(const char* aName, FieldType aType, std::size_t anOffset, std::size_t aSize, bool anIsPtr) :
            myName(aName),
            myType(aType),
            myOffset(anOffset),
            mySize(aSize),
            myIsPtr(anIsPtr)
        {
            std::cout << "Construct FieldDeclaration " << myName << std::endl;
        }
        //FieldDeclaration(const FieldDeclaration&) = delete;
        //FieldDeclaration(FieldDeclaration&&) = default;
    };

    //template<std::size_t N>
    //struct FieldArrayDeclaration : public FieldDeclaration
    //{
    //    const static std::size_t myRank = N;
    //    std::array<std::size_t, N> myDimensions; // vide si ça n'est pas un tableau
    //};

    struct ModelDeclaration
    {
        const char* myName;
        const std::vector<const ModelDeclaration*> myBases;
        const std::vector<const FieldDeclaration> myFields;

        ModelDeclaration(const char* aName, std::vector<const ModelDeclaration*>&& someBases, std::vector<const FieldDeclaration>&& someFieldDeclarations) :
            myName(aName),
            myBases(someBases),
            myFields(someFieldDeclarations)
        {
            std::cout << "Construct ModelDeclaration " << myName << std::endl;
        }
        ModelDeclaration(const ModelDeclaration& anOtherModelDeclaration) = delete;
        ModelDeclaration(ModelDeclaration&& anOtherModelDeclaration) = delete;
        ModelDeclaration& operator=(const ModelDeclaration& anOtherModelDeclaration) = delete;
        ModelDeclaration& operator=(ModelDeclaration&& anOtherModelDeclaration) = delete;
    };

    class Object abstract
    {
        std::string myName;
    public:
        const ModelDeclaration& myModelDeclaration;
    protected:
        Object(std::string&& aName, const ModelDeclaration& aModelDeclaration) :
            myName(aName),
            myModelDeclaration(aModelDeclaration)
        {
        }
    public:
        const std::string& name() const { return myName; }
        void name(const std::string& aName) { myName = aName; }
    };

    class Light : public Object
    {
    protected:
        static const ModelDeclaration& ourModelDeclaration;

    private:
        float myIntensity;

    public:
        Light(float anIntensity, const ModelDeclaration& aModelDeclaration = Light::ourModelDeclaration) :
            Object("xxx", aModelDeclaration),
            myIntensity(anIntensity) 
        {
        }
        Light() = delete;
        Light(const Light&) = delete;
        Light(Light&&) = delete;
        Light& operator=(const Light&) = delete;
        Light& operator=(Light&&) = delete;
    };

#define DECLARE_FIELD(_CLASS_NAME_, _FIELD_NAME_)  { #_FIELD_NAME_, FieldTypeOf<decltype(_CLASS_NAME_::_FIELD_NAME_)>::value, offsetof(_CLASS_NAME_, _FIELD_NAME_), sizeof(_CLASS_NAME_::_FIELD_NAME_), std::is_pointer<decltype(_CLASS_NAME_::_FIELD_NAME_)>::value }

    const ModelDeclaration& Light::ourModelDeclaration =
    {
        "Light",
        {},
        {
            DECLARE_FIELD(Light, myIntensity)
        }
    };

    class OmniLight : public Light
    {
        static const ModelDeclaration& ourModelDeclaration;
    public:
        float myRadius;
        int myFoo;
    public:
        OmniLight(float anIntensity, float aRadius) : 
            Light(anIntensity, OmniLight::ourModelDeclaration),
            myRadius(aRadius),
            myFoo(707)
        {
        }
    };

    const ModelDeclaration& OmniLight::ourModelDeclaration =
    {
        "OmniLight",
        {
            &Light::ourModelDeclaration
        },
        {
            //DECLARE_FIELD(OmniLight, myIntensity),
            DECLARE_FIELD(OmniLight, myRadius),
            DECLARE_FIELD(OmniLight, myFoo)
        }
    };
}

void Dump(const Object::Object* anObject, const Object::ModelDeclaration& md)
{
    for (auto x : md.myBases)
    {
        Dump(anObject, *x);
    }
    for (auto x : md.myFields)
    {
        std::cout << x.myName << " ";
        switch (x.myType)
        {
            case Object::FieldType::Float:
            {
                auto y = *(float*)(((char*)anObject) + x.myOffset);
                std::cout << y;
            }
            break;
            case Object::FieldType::Integer:
            {
                auto y = *(int*)(((char*)anObject) + x.myOffset);
                std::cout << y;
            }
                break;
            case Object::FieldType::String:
            {
                auto y = (char*)*(int*)(((char*)anObject) + x.myOffset);
                std::cout << y;
            }
                break;
        }

        std::cout << " (from '" << md.myName << "')" << std::endl;
    }
};

void Dump(const Object::Object* anObject)
{
    Dump(anObject, anObject->myModelDeclaration);
}

void NewTest()
{
    auto l = new Object::Light(42.7f);
    auto o = new Object::OmniLight(509.13f, 17.8f);

    Dump(l);
    Dump(o);

    std::cout << "kilt " << l->myModelDeclaration.myName << " ... " << std::endl;
}

namespace _
{
    AutoRun<> _(NewTest);
}

// Hidden in plain sight
// Tower fall