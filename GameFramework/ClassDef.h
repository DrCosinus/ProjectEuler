// http://accu.org/index.php/journals/428
// http://accu.org/index.php/journals/421
/*
#include <string>
#include <vector>

namespace RFX
{
    class Value;

    class BaseValue abstract
    {
    public:
        virtual ~BaseValue()
        {}
        virtual BaseValue* clone() const = 0;
        virtual std::string asString() const = 0; // fromString() 
        virtual void set(const Value& v) = 0; // no get()! 
    private:
        // Type info 
    };



    template <typename PlainT>
    class RealValue : public BaseValue
    {
    public:
        RealValue(PlainT v) : val(v)
        {}
        RealValue* clone() const
        {
            return new RealValue(*this);
        }
        std::string asString() const override
        {
            std::ostringstream os;
            os << val;
            return os.str();
        }
        operator PlainT() const // conversion to plain type 
        {
            return val;
        }
        void RealValue<PlainT>::set(Value const & v) override
        {
            val = v.get<PlainT>();
        }
    private:
        PlainT val;
    };



    class Value // Value handle 
    {
    public:
        Value(const BaseValue& bv) : v(bv.clone())
        {}
        Value(const Value& rhs) : v(rhs.v ? rhs.v->clone() : nullptr)
        {}
        explicit Value(BaseValue* bv = nullptr) : v(bv)
        {}
        ~Value()
        {
            if (v)
            {
                delete v;
                v = nullptr;
            }
        }
        Value& operator=(const Value& rhs)
        {
            // this is not a typical pimpl assignment, but a set() 
            if (v)
            {
                if (rhs.v)
                {
                    v->set(rhs);
                }
                else
                {
                    delete v;
                    v = nullptr;
                }
            }
            else
            {
                v = (rhs.v ? rhs.v->clone() : nullptr);
            }
            return *this;
        }

        template <typename PlainT>
        PlainT get() const
        {
            if (v)
            {
                const RealValue<PlainT>& rv = dynamic_cast<const RealValue<PlainT>&>(*v);
                return rv;
            }
            else
            {
                return PlainT();
            }
        }
        std::string asString() const
        {
            if (v)
            {
                return v->asString();
            }
            else
            {
                return std::string();
            }
        }
    private:
        BaseValue*  v;
    };



    class Type
    {
    public:
        enum TypeT
        {
            stringT,
            intT,
            doubleT,
            unknownT
        };
        explicit Type(TypeT typeId_) : typeId(typeId_)
        {
        }
        BaseValue* newValue() const
        {
            return prototypes[typeId]->clone();
        }
        TypeT getType() const
        {
            return typeId;
        }
        static void init()
        {
            prototypes[stringT] = new RealValue<std::string>("");
            prototypes[intT] = new RealValue<int>(0);
            prototypes[doubleT] = new RealValue<double>(0);
        }
    private:
        TypeT typeId;
        static std::vector<BaseValue*> prototypes;
    };
    std::vector<BaseValue*> Type::prototypes(Type::unknownT);



    class Attribute
    {
    public:
        Attribute(const std::string & name_, Type::TypeT typeId) : name(name_), type_(typeId)
        {
        }
        const std::string & getName() const
        {
            return name;
        }
        Type getType() const
        {
            return type_;
        }
    private:
        std::string name;
        Type type_;
    };


    class Object;

    class ClassDef
    {
        friend class Object;
        //typedefs Container, Iterator for attributes 
        typedef std::vector<Attribute> AttributeContainer;
        typedef AttributeContainer::iterator AttrIterator;
    public:
        ClassDef(const ClassDef* base, const std::string& name_)
            : baseClass(base)
            , name(name_)
            , definitionFix(false)
        {
            baseInit();
            effectiveAttributes.insert(effectiveAttributes.end(), ownAttributes.begin(), ownAttributes.end());
        }
        template <typename iterator>
        ClassDef(const ClassDef* base, const std::string& name_, iterator attribBegin, iterator attribEnd)
            : baseClass(base), name(name_)
            , ownAttributes(attribBegin, attribEnd)
            , definitionFix(false)
        {
            baseInit();
            effectiveAttributes.insert(effectiveAttributes.end(), ownAttributes.begin(), ownAttributes.end());
        }
        std::string getName() const;
        Object* newObject() const;
        AttrIterator attribBegin() const;
        AttrIterator attribEnd() const;
        const Attribute& getAttribute(size_t idx) const;
        void addAttribute(const Attribute&);
        size_t getAttributeCount() const;
        size_t findAttribute(const std::string& name) const
        {
            // this does a reverse search to find the most derived 
            AttributeContainer::const_reverse_iterator i;
            for (i = effectiveAttributes.rbegin(); i != effectiveAttributes.rend(); ++i)
            {
                if (i->getName() == name)
                {
                    return std::distance(i, effectiveAttributes.rend()) - 1;
                }
            }
            return getAttributeCount();
        }
    private:
        void baseInit()
        {
            if (baseClass)
            {
                baseClass->definitionFix = true;
                std::copy(baseClass->attribBegin(), baseClass->attribEnd(), std::back_inserter<AttributeContainer>(effectiveAttributes));
            }
        }
        const ClassDef* const baseClass;
        std::string name;
        AttributeContainer ownAttributes, effectiveAttributes;
        mutable bool definitionFix;
    };



    class Object
    {
    public:
        explicit Object(const ClassDef* class_) : myClass(class_), values(class_->getAttributeCount())
        {
            buildValueList();
        }
        const ClassDef& instanceOf() const
        {
            return *myClass;
        }
        Value getValue(size_t attribIdx) const
        {
            return *values[attribIdx]; // calls Value(BaseValue &) 
        }
        Value getValue(std::string const &attribName) const
        {
            size_t idx = instanceOf().findAttribute(attribName); // should check for not found 
            return getValue(idx);
        }
        void setValue(size_t idx, Value const& v)
        {
            values[idx]->set(v);
        }
        void setValue(std::string const& attribName, Value const &v)
        {
            size_t idx = instanceOf().findAttribute(attribName); // should check for not found 
            setValue(idx, v);
        }
    private:
        typedef std::vector<BaseValue*> ValueContainer;
        void buildValueList()
        {
            ClassDef::AttrIterator a;
            ValueContainer::iterator i = values.begin();
            for (a = instanceOf().attribBegin(); a != instanceOf().attribEnd(); ++a, ++i)
            {
                *i = a->getType().newValue();
            }
        }
        ClassDef const * const myClass;
        ValueContainer values;
    };


    Object* ClassDef::newObject() const
    {
        definitionFix = true;
        return new Object(this);
    }
}
*/