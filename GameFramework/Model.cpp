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
/*
struct TypeBase
{
    virtual void Marshal(void*, const void*) const = 0;
    virtual void Demarshal(const void*, void*) const = 0;
    virtual const char* name() const = 0;
    virtual size_t size() const = 0;
};

struct member_t
{
    char const *name;
    TypeBase *type;
    size_t offset;
};

template <typename T> 
T& instance() 
{
    static T t;
    return t;
}

template<typename T> 
struct Type : TypeBase
{
    static Type<T> instance;
    // custom marshaling is handled by template specialization
    void Marshal(void *dst, void const *src) const { memcpy(dst, src, sizeof(T)); }
    void Demarshal(void const *src, void *dst) const { memcpy(dst, src, sizeof(T)); }
    char const *name() const { return typeid(T).name(); }
    size_t size() const { return sizeof(T); }
};

template<typename T, typename Q>
TypeBase* get_type(Q T::*mem) 
{
    return &instance<Type<Q> >();
}

#define MEMBER(x) \
   { #x, get_type(&type::x), offsetof( type, x ) },

struct ReflectionBase abstract
{
    void ReflectionConstruct();
    //virtual size_t size() const = 0;
    virtual char const *name() const = 0;
    virtual size_t memberCount() const = 0;
    virtual member_t const *members() const = 0;
};

void ReflectionBase::ReflectionConstruct()
{
    //  todo: register myself in some table, etc
    members();
    memberCount();
    name();
    //size();
}

#define RTTI(_type, _members) \
    typedef _type type; \
    struct _info : ReflectionBase \
    { \
        const size_t size = sizeof(_type); \
        inline const char* name() const override { return #_type; } \
        inline size_t memberCount() const override { size_t count; get_members(count); return count; } \
        inline member_t const *members() const override { size_t cnt; return get_members(cnt); } \
        _info() { ReflectionConstruct(); } \
        static inline member_t const *get_members(size_t &cnt) \
        { \
            static member_t members[] = { _members }; \
            cnt = sizeof(members) / sizeof(members[0]); \
            return members; \
        } \
        static inline _info& info() \
        { \
            return instance<_info>(); \
        } \
    }; \
    static const member_t* members() { return _info::info().members(); } \
    static _info _theInfo; \
    static inline const _info& info() { return _theInfo; }
    */
#define DECLARE_MODEL(_MODEL_CLASSNAME_, _MODEL_SUPERCLASSNAME_) \
public: \
    typedef _MODEL_SUPERCLASSNAME_ Super; \
private: \
    static const char* ourModelName; \
    static unsigned int ourNextInstanceIndex; \
    static std::vector<std::shared_ptr<_MODEL_CLASSNAME_>> ourInstances; \
    \
public: \
    _MODEL_CLASSNAME_(const _MODEL_CLASSNAME_&) = delete; \
    _MODEL_CLASSNAME_& operator= (const _MODEL_CLASSNAME_&) = delete; \
    template<typename... Ts> \
    static auto Create(Ts... args) \
    { \
        auto item = std::make_shared<_MODEL_CLASSNAME_>(protected_tag{}, combineName(ourModelName, ourNextInstanceIndex++), args...); \
        ourInstances.push_back(item); \
        return item; \
    } \
    static void ForEach(std::function<void(_MODEL_CLASSNAME_&)> aFunction) \
    { \
        for (auto& i : ourInstances) \
        { \
            aFunction(*i); \
        } \
    }

#define DECLARE_MODEL_VARIABLE(_MODEL_VARIABLE_TYPE_, _MODEL_VARIABLE_NAME_) \
    _MODEL_VARIABLE_TYPE_ _MODEL_VARIABLE_NAME_;    

#define IMPLEMENT_MODEL(_MODEL_CLASSNAME_) \
    unsigned int _MODEL_CLASSNAME_::ourNextInstanceIndex = 0; \
    const char* _MODEL_CLASSNAME_::ourModelName = #_MODEL_CLASSNAME_; \
    std::vector<std::shared_ptr<_MODEL_CLASSNAME_>> _MODEL_CLASSNAME_::ourInstances;

namespace Model
{
    static std::string combineName(const char* myPrefix, unsigned int myIndex)
    {
        std::ostringstream stringBuilder;
        stringBuilder << myPrefix;
        stringBuilder.width(3);
        stringBuilder.fill('0');
        //stringBuilder << std::hex;
        stringBuilder << myIndex;
        return std::move(stringBuilder.str());
    }

    class Model abstract
    {
        std::string myName;

    protected:
        struct protected_tag {};
    protected:
        Model(const protected_tag&, std::string&& aName) : myName(aName)
        {
        }
    public:
        const std::string& name() const { return myName; }
        void name(const std::string& aName) { myName = aName; }
    };

    class Light : public Model
    {
        DECLARE_MODEL(Light, Model)
        //DECLARE_MODEL_VARIABLE(float, myIntensity);

        //RTTI(Light, MEMBER(myIntensity));
        float myIntensity;
    public:
        Light(const protected_tag& aProtectedTag, std::string&& aName, float anIntensity) : Model(aProtectedTag, std::forward<std::string>(aName)), myIntensity(anIntensity)
        {
        }
        Light(std::string&& aName, float anIntensity) : Light(protected_tag{}, std::forward<std::string>(aName), anIntensity) {}
    };
    IMPLEMENT_MODEL(Light)

    class OmniLight : public Light
    {
        DECLARE_MODEL(OmniLight, Light)
        //DECLARE_MODEL_VARIABLE(float, myRadius);

        float myRadius;
    public:
        OmniLight(const protected_tag& aProtectedTag, std::string&& aName, float anIntensity, float aRadius) : Light(aProtectedTag, std::forward<std::string>(aName), anIntensity), myRadius(aRadius)
        {
        }
    };
    IMPLEMENT_MODEL(OmniLight)

    class Renderer : public Model
    {
        DECLARE_MODEL(Renderer, Model)
    public:
        Renderer(const protected_tag & aProtectedTag, std::string&& aName) : Model(aProtectedTag, std::forward<std::string>(aName))
        {
        }
    };
    IMPLEMENT_MODEL(Renderer)

    //Light::_info Light::_theInfo;
}
//template<typename T>
//typename T::_info T::_theInfo;


//#define PP_NARG( ...) PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
//#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)
//#define PP_ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,[..],_61,_62,_63,N,...) N
//#define PP_RSEQ_N() 63,62,61,60,[..],9,8,7,6,5,4,3,2,1,0

//template<typename T>
//struct function_traits;
//
//template<typename R, typename... Ts>
//struct function_traits<R(Ts...)>
//{
//    using return_type = R;
//    static const std::size_t arity = sizeof...(Ts);
//
//    template <std::size_t N>
//    struct argument
//    {
//        static_assert(N < arity, "invalid parameter index.");
//        using type = typename std::tuple_element<N, std::tuple<Ts...>>::type;
//    };
//};

//#define SIXTYFOURTH(_1,_2,_3,_4_,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,_61,_62,_63, N,...) N
//#define SIZEOF_VA_ARG(...) SIXTYFOURTH(__VA_ARGS__,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
//#define REFLECTABLE(...) function_traits<void(__VA_ARGS__)>::arity


//constexpr char* plop() { return "plop"; }
//typedef void zozo(float toto, float tata, int titi);
//constexpr std::size_t plop() { return function_traits<zozo /*void(*)(float toto, float tata, int titi)*/>::arity; }

//constexpr std::size_t plop() { return function_traits<void(float toto, float tata, int titi, char* yoyo)>::arity; }
//
//template<typename T>
//constexpr const auto& name();
//
//template<>
//constexpr const auto& name<Model::Light>()
//{
//    return "Light";
//}



constexpr int static_strcmp(const char* l, const char* r)
{
    return (*l == *r) ? (*l == '\0' ? 0 : static_strcmp(l + 1, r + 1)) : *l - *r;
}
//typedef std::tuple<Model::OmniLight, Model::Light, Model::Renderer> ModelTypes;

//class ConstString
//{
//    const char* s;
//    const int n;
//public:
//    constexpr ConstString(const char* s1, int n1) : s(s1), n(n1 - 1){};
//
//    constexpr char operator[](int j)
//    {
//        return (j < 0 || j >= n ? throw "ERROR: ConstString index out of bounds" : s[j]);
//    }
//    constexpr int size() { return n; }
//    constexpr const char* c_str() { return s; }
//    //operator std::string() const { return std::string(s, n); }  // not a constexpr operator  
//};

void Test()
{

    //std::cout
    //    << "Debug: " << function_traits<void(float toto, float tata/*, int titi, char* yoyo*/)>::arity
    //    << ",zuzu: " << name<Model::Light>()
    //    << " --- " << c_strcmp("Light", "Lumiere")
    //    << std::endl;

    //std::cout
    //    << Model::Light::info().name()
    //    << " "
    //    << Model::Light::info().size
    //    << std::endl;
    std::cout 
        << ClassInfo<plop>::ourName
        << " -+*#*+- "
        << typeid(plop).raw_name()
        << std::endl;

    for (const auto& i : ClassInfo<plop>::ourFieldInfos)
    {
        std::cout
            << "    "
            << i.myName
            << ", offset=" << i.myOffset
            << ", size=" << i.mySize
            //<< ", type=" << typeid(decltype(i)::type).name()
            << std::endl;

    }

    auto l = Model::Light::Create(5.0f);
    auto l2 = Model::Light::Create(17.0f);

    auto o = Model::OmniLight::Create(17.0f, 5.0f);

    l->myIntensity = 409.0f;
    Model::Renderer::Create();

    Model::Light::ForEach([](Model::Light& aLight)
    {
        std::cout << "Light: " << aLight.name() << ", myIntensity = " << aLight.myIntensity << std::endl;
    });

    Model::Renderer::ForEach([](Model::Renderer& aRenderer)
    {
        std::cout << "Renderer: " << aRenderer.name() << std::endl;
    });

    Model::OmniLight::ForEach([](Model::OmniLight& anOmniLight)
    {
        std::cout << "OmniLight: " << anOmniLight.name() << ", myRadius = " << anOmniLight.myRadius << ", myIntensity = " << anOmniLight.myIntensity << std::endl;
    });
}

template<typename... Ts>
class AutoRun
{
public:
    AutoRun(std::function<void (Ts...)> F, Ts... args)
    {
        F(args...); 
    }
};

namespace
{
    AutoRun<> _(Test);
}
//#include <type_traits>
#include <array>
//#include <cstddef>
namespace Object
{
    enum class ID
    {
        Light = 159
    };

    // c'est ModelDeclaration qui contient la structure du Model
   
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

    struct FieldDeclaration
    {
        std::string myName;
        FieldType myType;
        std::size_t myOffset;
        std::size_t mySize;
        bool myIsPtr;
    };

    template<std::size_t N>
    struct FieldArrayDeclaration : public FieldDeclaration
    {
        const static std::size_t myRank = N;
        std::array<std::size_t, N> myDimensions; // vide si �a n'est pas un tableau
    };

    class ModelDeclaration
    {
        const std::vector<FieldDeclaration>& myFields;
    public:
        ModelDeclaration(const std::vector<FieldDeclaration>& someFieldDeclarations) :
            myFields(someFieldDeclarations)
        {
        }
    };


    class Model abstract
    {
        std::string myName;

        template<typename T>
        friend struct FieldTypeOf;

    protected:
        struct protected_tag {};
    protected:
        Model(const protected_tag&, std::string&& aName) : myName(aName)
        {
            //myModelDeclaration = std::make_shared<ModelDeclaration>();
        }
    public:
        const std::string& name() const { return myName; }
        void name(const std::string& aName) { myName = aName; }
    };

    class Light : public Model
    {
        const ModelDeclaration& myModelDeclaration;
        static std::vector<FieldDeclaration> ourFieldDeclarations;
    public:
        static const ID ourId = ID::Light;
        static const char* ourName;

    protected:
        float myIntensity;

    public:
        Light(float anIntensity) : 
            Model(protected_tag{}, "xxx"), 
            myModelDeclaration(Light::ourFieldDeclarations),
            myIntensity(anIntensity) 
        {
        }
        Light() = delete;
        Light(const Light&) = delete;
        Light(Light&&) = delete;
        Light& operator=(const Light&) = delete;
        Light& operator=(Light&&) = delete;
    };
    const char* Light::ourName = "Light";

#define DECLARE_FIELD(_CLASS_NAME_, _FIELD_NAME_)  { #_FIELD_NAME_, FieldTypeOf<decltype(_CLASS_NAME_::_FIELD_NAME_)>::value, offsetof(_CLASS_NAME_, _FIELD_NAME_), sizeof(_CLASS_NAME_::_FIELD_NAME_), std::is_pointer<decltype(_CLASS_NAME_::_FIELD_NAME_)>::value }

    std::vector<FieldDeclaration> Light::ourFieldDeclarations =
    {
        //{ "myIntensity", FieldType::Integer, 0, 4, false }
        //{ "myIntensity", FieldTypeOf<decltype(Light::myIntensity)>::value, 0, 4, false }
        //{ "myIntensity", FieldTypeOf<decltype(Light::myIntensity)>::value, offsetof(Light, myIntensity), 4, false }
        //{ "myIntensity", FieldTypeOf<decltype(Light::myIntensity)>::value, offsetof(Light, myIntensity), sizeof(Light::myIntensity), std::is_pointer<decltype(Light::myIntensity)>::value }
        DECLARE_FIELD(Light, myIntensity)
    };

    class OmniLight : public Light
    {
        //DECLARE_MODEL(OmniLight, Light)
        //DECLARE_MODEL_VARIABLE(float, myRadius);
        const ModelDeclaration& myModelDeclaration;
        static const char* ourName;
        static std::vector<FieldDeclaration> ourFieldDeclarations;

        float myRadius;
    public:
        OmniLight(float anIntensity, float aRadius) : 
            Light(anIntensity), 
            myModelDeclaration(OmniLight::ourFieldDeclarations),
            myRadius(aRadius)
        {
        }
    };

    std::vector<FieldDeclaration> OmniLight::ourFieldDeclarations =
    {
        //{ "myIntensity", FieldType::Integer, 0, 4, false }
        //{ "myIntensity", FieldTypeOf<decltype(Light::myIntensity)>::value, 0, 4, false }
        //{ "myIntensity", FieldTypeOf<decltype(Light::myIntensity)>::value, offsetof(Light, myIntensity), 4, false }
        //{ "myIntensity", FieldTypeOf<decltype(Light::myIntensity)>::value, offsetof(Light, myIntensity), sizeof(Light::myIntensity), std::is_pointer<decltype(Light::myIntensity)>::value }
        DECLARE_FIELD(OmniLight, myIntensity),
        DECLARE_FIELD(OmniLight, myRadius)
    };

    /*
    typedef std::tuple<Light> classes;

    template<typename T, ID id>
    struct from_id
    {
        typedef nullptr_t type;
    };

    template<typename First, typename... Others, ID id>
    struct from_id<std::tuple<First, Others...>, id>
    {
        typedef typename std::conditional<First::ourId==id, First, typename from_id<std::tuple<Others...>, id>::type>::type type;
    };

    template<ID id>
    struct from_id<std::tuple<>, id>
    {
        typedef nullptr_t type;
    };


    template<typename T, const char* name>
    struct from_name;
    //{
    //    typedef nullptr_t type;
    //};

    template<typename First, typename... Others, const char* name>
    struct from_name<std::tuple<First, Others...>, name>
    {
        typedef typename std::conditional< !static_strcmp(First::ourName, name), First, typename from_name<std::tuple<Others...>, name>::type>::type type;
    };

    template<const char* name>
    struct from_name<std::tuple<>, name>
    {
        typedef nullptr_t type;
    };


    template<const ID anId, typename... Ts>
    auto Create(Ts... someParameters)
    {
        typedef typename from_id<classes, anId>::type type;
        return std::make_shared<type>( someParameters... );
    }

    template<typename... Ts>
    auto Create(const char* name, Ts... someParameters)
    {
        typedef typename from_name<classes, name>::type type;
        return std::make_shared<type>(someParameters...);
    }*/
    //template<typename T, typename... Ts>
    //auto Create(Ts... someParameters)
    //{
    //    return new T(someParameters...);
    //}
}

void NewTest()
{
    //auto light = Object::Create<Model::Light>("Light", 5.0f);
    //auto lightX = Object::Create<Object::ID::Light>(5.0f);
    //auto lightY = Object::Create("Light", 5.0f);
    
    //auto plop = Object::from_id<Object::classes, Object::ID::Light>::type::ourId;
    //std::cout << "plop " << (int)plop << " " << Object::from_id<Object::classes, Object::ID::Light>::type::ourName << " " << lightX->myIntensity << std::endl;
    //std::declval

    auto l = new Object::Light(42.7f);

    std::cout << "kilt " 
        << "name=" << l->ourFieldDeclarations[0].myName 
        << ", offset=" << l->ourFieldDeclarations[0].myOffset
        << ", size=" << l->ourFieldDeclarations[0].mySize
        << ", is_ptr=" << l->ourFieldDeclarations[0].myIsPtr
        << ", type=" << (int)l->ourFieldDeclarations[0].myType
        << std::endl;
}

namespace _
{
    AutoRun<> _(NewTest);
}

// Hidden in plain sight
// Tower fall