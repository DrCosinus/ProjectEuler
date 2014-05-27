#include "stdafx.h"
#include "Model.h"

#include <vector>
#include <functional>
#include <memory>
#include <string>

class Model abstract
{
protected:
    struct protected_tag {};
};

class ModelLight : public Model
{
    template<typename T>
    friend void With(std::function<void(T&)> aFunction);

    static std::vector<ModelLight> ourInstances;

    float myRadius;

    ModelLight(const protected_tag &, float aRadius) : Model(), myRadius(aRadius)
    {
    }
    ModelLight(const protected_tag & aTag) : ModelLight(aTag, 0.0f)
    {
    }

public:
    static std::vector<ModelLight>::const_reference Create(float aRadius)
    {
        ourInstances.emplace_back(protected_tag{}, aRadius);
        return ourInstances.back();
    }
};

//std::vector<ModelLight> ModelLight::ourInstances;

class ModelRenderer : public Model
{
    template<typename T>
    friend void With(std::function<void(T&)> aFunction);

    static std::vector<ModelRenderer> ourInstances;
public:
};

//std::vector<ModelLight> ModelLight::ourInstances;




template<typename T>
void With(std::function<void (T&)> aFunction)
{
    for (auto& i : T::ourInstances)
    {
        aFunction(i);
    }
}

void yoyo(ModelLight& aLight)
{
}

void Test()
{
    
    With<ModelLight>([](ModelLight& aLight)
    {
    });
    With<ModelLight>(yoyo);
}