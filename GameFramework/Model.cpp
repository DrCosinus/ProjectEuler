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
    class Object;

    struct FieldTypeAbstract abstract
    {
        std::string myName;
        std::size_t myOffset;
        bool myIsPtr;

        FieldTypeAbstract(const std::string& aName, std::size_t anOffset, bool anIsPtr) :
            myName(aName),
            myOffset(anOffset),
            myIsPtr(anIsPtr)
        {
        }
        virtual ~FieldTypeAbstract()
        {
        }

        virtual std::string toString(const Object* o) const abstract;
        virtual void fromString(Object* anObject, const std::string& aString) const abstract;
    };

    template<typename T>
    struct FieldType : public FieldTypeAbstract
    {
        const static bool ourIsPtr = std::is_pointer<T>::value;

        FieldType(const std::string& aName, std::size_t anOffset) : FieldTypeAbstract(aName, anOffset, ourIsPtr)
        {
        }
        std::string toString(const Object* anObject) const override
        {
            std::ostringstream oss;
            oss << *reinterpret_cast<const T*>(reinterpret_cast<const char*>(anObject)+myOffset);
            return oss.str();
        }
        void fromString(Object* anObject, const std::string& aString) const override
        {
            std::istringstream iss(aString);
            iss >> *reinterpret_cast<T*>(reinterpret_cast<char*>(anObject)+myOffset);
        }

        FieldType(const FieldType&) = delete;
        FieldType(FieldType&&) = delete;
        FieldType& operator= (const FieldType&) = delete;
        FieldType& operator= (FieldType&&) = delete;
    };

    template<>
    struct FieldType<void> : public FieldTypeAbstract
    {
        FieldType(const std::string& aName) : FieldTypeAbstract(aName, 0, false)
        {
        }
        std::string toString(const Object*) const override;
        void fromString(Object* anObject, const std::string&) const override;

        FieldType(const FieldType&) = delete;
        FieldType(FieldType&&) = delete;
        FieldType& operator= (const FieldType&) = delete;
        FieldType& operator= (FieldType&&) = delete;
    };

    struct ModelDeclaration
    {
        const char* myName;
        const std::vector<const ModelDeclaration*> myBases;
        const std::vector<const FieldTypeAbstract*> myFields;

        ModelDeclaration(const char* aName, std::vector<const ModelDeclaration*>&& someBases, std::vector<const FieldTypeAbstract*>&& someFieldDeclarations) :
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

        const FieldTypeAbstract& operator[](const std::string& aFieldName) const
        {
            auto result = std::find_if(begin(myFields), end(myFields), [&aFieldName](auto f) { return aFieldName == f->myName; });
            if (result == end(myFields))
            {
                static auto r = std::make_unique<FieldType<void>>(aFieldName); // maintient la dernière occurence vivante... pas genial, en attendant une meilleure idee
                return *r;
            }
            else
                return **result;
        }
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

        // o->myModelDeclaration.myFields[1]->fromString(o, "1972");

    };


    std::string FieldType<void>::toString(const Object* anObject) const
    {
        std::cout << "toString: No field " << myName << " on " << anObject->name() << "(" << anObject->myModelDeclaration.myName << ")" << std::endl;
        return "unknownField";
    }
    void FieldType<void>::fromString(Object* anObject, const std::string&) const
    {
        std::cout << "fromString: No field " << myName << " on " << anObject->name() << "(" << anObject->myModelDeclaration.myName << ")" << std::endl;
    }




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

//#define DECLARE_FIELD(_CLASS_NAME_, _FIELD_NAME_)  new FieldType<decltype(_CLASS_NAME_::_FIELD_NAME_)>( #_FIELD_NAME_, offsetof(_CLASS_NAME_, _FIELD_NAME_), std::is_pointer<decltype(_CLASS_NAME_::_FIELD_NAME_)>::value )
#define DECLARE_FIELD(_CLASS_NAME_, _FIELD_NAME_)  new FieldType<decltype(_CLASS_NAME_::_FIELD_NAME_)>( #_FIELD_NAME_, offsetof(_CLASS_NAME_, _FIELD_NAME_) )

    const ModelDeclaration& Light::ourModelDeclaration =
    {
        "Light",
        {},
        {
            DECLARE_FIELD(Light, myIntensity)
            //new FieldType<decltype(Light::myIntensity)>("myIntensity", offsetof(Light, myIntensity), std::is_pointer<decltype(Light::myIntensity)>::value)
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
    for (auto& x : md.myFields)
    {
        std::cout << x->myName << " ";

        std::cout << x->toString(anObject);

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

    o->myModelDeclaration["myRadiusX"].fromString(o, "1972");
    o->myModelDeclaration["myRadius"].fromString(o, "1977");

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