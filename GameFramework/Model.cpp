#include "stdafx.h"
#include "Model.h"

#include <vector>
#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>

template<typename T>
class Model abstract
{
    template<typename T>
    friend void With(std::function<void(T&)> aFunction);

    std::string myName;
protected:
    struct protected_tag {};
    Model(const protected_tag &, std::string aName) : myName(aName)
    {
    }

    std::string combineName(char* myPrefix, unsigned int myIndex)
    {
        std::ostringstream stringBuilder;
        stringBuilder << myPrefix;
        stringBuilder.width(3);
        stringBuilder.fill('0');
        //stringBuilder << std::hex;
        stringBuilder << myIndex;
        return stringBuilder.str();
    }

    static std::vector<std::shared_ptr<T>> ourInstances;
    static unsigned int ourNextInstanceIndex;

public:
    const std::string& getName() { return myName; }

    template<typename... Ts>
    static std::shared_ptr<T>& Create(Ts... args)
    {
        ourInstances.emplace_back(std::make_shared<T>(protected_tag{}, args...));
        return ourInstances.back();
    }
};

template<typename T>
unsigned int Model<T>::ourNextInstanceIndex = 0;

template<typename T>
std::vector<std::shared_ptr<T>> Model<T>::ourInstances;

class ModelLight : public Model<ModelLight>
{
public:
    float myRadius;

public:
    ModelLight(const protected_tag & aProtectedTag, float aRadius) : Model(aProtectedTag, combineName("Light", ourNextInstanceIndex++)), myRadius(aRadius)
    {
    }
    ModelLight(const protected_tag & aProtectedTag) : ModelLight(aProtectedTag, 0.0f)
    {
    }
    //ModelLight(const ModelLight&) = delete;

    //static std::shared_ptr<ModelLight>& Create(float aRadius)
    //{
    //    ourInstances.emplace_back(std::make_shared<ModelLight>(protected_tag{}, aRadius));
    //    return ourInstances.back();
    //}
};

class ModelRenderer : public Model<ModelRenderer>
{
public:
    ModelRenderer(const protected_tag & aProtectedTag) : Model(aProtectedTag, combineName("Renderer", ourNextInstanceIndex++))
    {
    }
};


template<typename T>
void With(std::function<void (T&)> aFunction)
{
    for (auto& i : T::ourInstances)
    {
        aFunction(*i);
    }
}

void Test()
{
    std::shared_ptr<ModelLight> l = ModelLight::Create(5.0f);
    ModelLight::Create(17.0f);
    l->myRadius = 409.0f;
    ModelRenderer::Create();

    With<ModelLight>([](ModelLight& aLight)
    {
        std::cout << "Light: " << aLight.getName() << ", myRadius = " << aLight.myRadius << std::endl;
    });

    With<ModelRenderer>([](ModelRenderer& aRenderer)
    {
        std::cout << "Renderer: " << aRenderer.getName() << std::endl;
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
