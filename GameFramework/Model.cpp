#include "stdafx.h"
#include "Model.h"

#include <vector>
#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>

namespace Model
{
    class PreModel
    {
        template<typename T>
        friend class InstanceList;
    protected:
        struct protected_tag {};
    };

    template<typename T>
    class InstanceList
    {
        static std::vector<std::shared_ptr<T>> ourInstances;

        static unsigned int ourNextInstanceIndex;

        static std::string combineName(const char* myPrefix, unsigned int myIndex)
        {
            std::ostringstream stringBuilder;
            stringBuilder << myPrefix;
            stringBuilder.width(3);
            stringBuilder.fill('0');
            //stringBuilder << std::hex;
            stringBuilder << myIndex;
            return stringBuilder.str();
        }
    public:
        template<typename... Ts>
        static std::shared_ptr<T>& Create(Ts... args)
        {
            ourInstances.emplace_back(std::make_shared<T>(T::protected_tag{}, combineName(T::ourTypeName, ourNextInstanceIndex++), args...));
            return ourInstances.back();
        }
        static void ForEach(std::function<void(T&)> aFunction)
        {
            for (auto& i : ourInstances)
            {
                aFunction(*i);
            }
        }
    };

    //template<typename T>
    class Model abstract : virtual public PreModel
    {
        std::string myName;
    protected:
        //struct protected_tag {};
        Model(const protected_tag&, std::string&& aName) : myName(aName)
        {
        }
        //template<typename T>
        //Model(const protected_tag& aProtectedTag) : Model(aProtectedTag)
        //{
        //}

    public:
        const std::string& getName() const { return myName; }

        //template<typename... Ts>
        //static std::shared_ptr<T>& Create(Ts... args)
        //{
        //    ourInstances.emplace_back(std::make_shared<T>(protected_tag{}, args...));
        //    return ourInstances.back();
        //}

        template<typename T, typename... Ts>
        static std::shared_ptr<T>& Create(Ts... args)
        {
            T::Create(args...);
        }

        //static void ForEach(std::function<void(T&)> aFunction)
        //{
        //    for (auto& i : T::ourInstances)
        //    {
        //        aFunction(*i);
        //    }
        //}
    };

    template<typename T>
    unsigned int InstanceList<T>::ourNextInstanceIndex = 0;

    template<typename T>
    std::vector<std::shared_ptr<T>> InstanceList<T>::ourInstances;

    class Light : public Model
    {
    public:
        static const char* ourTypeName;
        float myIntensity;

    public:
        Light(const protected_tag& aProtectedTag, std::string&& aName, float anIntensity) : Model(aProtectedTag, aName), myIntensity(anIntensity)
        {
        }
        Light(const protected_tag& aProtectedTag) : Light(aProtectedTag, 0.0f)
        {
        }
        Light(const Light&) = delete;
        Light& operator= (const Light&) = delete;
    };
    const char* Light::ourTypeName = "Light";

    class OmniLight : public Light //Model<OmniLight>, public Light
    {
    public:
        static const char* ourTypeName;
        float myRadius;
    public:
        OmniLight(const protected_tag& aProtectedTag, std::string&& aName, float anIntensity, float aRadius) : Light(aProtectedTag, aName, anIntensity), myRadius(aRadius)
        {
        }
    };
    const char* OmniLight::ourTypeName = "OmniLight";

    class Renderer : public Model //<Renderer>
    {
    public:
        static const char* ourTypeName;
    public:
        Renderer(const protected_tag & aProtectedTag, std::string&& aName) : Model(aProtectedTag, aName)
        {
        }
    };
    const char* Renderer::ourTypeName = "Renderer";

    //template<typename T>
    //void With(std::function<void(T&)> aFunction)
    //{
    //    for (auto& i : T::ourInstances)
    //    {
    //        aFunction(*i);
    //    }
    //}
}



void Test()
{
    auto l = Model::InstanceList<Model::Light>::Create(5.0f);
    auto l2 = Model::InstanceList<Model::Light>::Create(17.0f);

    auto o = Model::InstanceList<Model::OmniLight>::Create(17.0f, 5.0f);

    //Model::Light lx(*l);
    l->myIntensity = 409.0f;
    Model::InstanceList<Model::Renderer>::Create();

    //With<Model::Light>([](Model::Light& aLight)
    //{
    //    std::cout << "Light: " << aLight.getName() << ", myRadius = " << aLight.myRadius << std::endl;
    //});
    Model::InstanceList<Model::Light>::ForEach([](Model::Light& aLight)
    {
        std::cout << "Light: " << aLight.getName() << ", myRadius = " << aLight.myIntensity << std::endl;
    });

    //Model::With<Model::Renderer>([](Model::Renderer& aRenderer)
    //{
    //    std::cout << "Renderer: " << aRenderer.getName() << std::endl;
    //});
    Model::InstanceList<Model::Renderer>::ForEach([](Model::Renderer& aRenderer)
    {
        std::cout << "Renderer: " << aRenderer.getName() << std::endl;
    });

    Model::InstanceList<Model::OmniLight>::ForEach([](Model::OmniLight& aRenderer)
    {
        std::cout << "OmniLight: " << aRenderer.getName() << std::endl;
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
