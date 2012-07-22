/**
 * Property is a type-shifting key-value container.  In order
 * to avoid RTTI facilities, types are mapped to integers at
 * compile time.  
 *
 * To enable Property to hold a new value type, invoke
 * REGISTER_PROPERTY_TYPE(type, index)
 *
 * A compiler error will be generated if there is already a
 * mapping for the type or the index.
 */

// STD Includes
#include <cassert>

// Library Includes

// Project Includes

#ifndef PROPERTY_H
#define PROPERTY_H

/**
 * Holds the type index
 */
template<int N>
struct registered_index
{
    enum { value = N };
};

/**
 * Holds a type
 */
template<typename T>
struct registered_type
{ 
    typedef T type;
};

/**
 * Defines a unique mapping from a user specified type to a 
 * user specified int.  If a mapping already exists for the
 * specified int, a compiler error will be generated.  This
 * happens since instantiations of the friend function differ
 * only by return types.  Since their signatures are identitcal
 * otherwise, there is an overload ambiguity.
 */
template<typename T, int N>
struct register_type : registered_index<N>, registered_type<T>
{
private:
    friend registered_type<T> dummy(registered_index<N> index)
    {
        return registered_type<T>();
    }
};

template<typename T>
struct PropertyTypeRegistry;

#define REGISTER_PROPERTY_TYPE(TYPE, N) \
    template<> struct PropertyTypeRegistry<TYPE> : register_type<TYPE, (N)> \
    { \
        enum { value = (N) }; \
    };
    
REGISTER_PROPERTY_TYPE(int, 0)
REGISTER_PROPERTY_TYPE(double, 1)

template<typename KeyType>
class Property
{
private:
    class Placeholder
    {
    public:
        virtual ~Placeholder() {}
    public:
        virtual int type() const = 0;
        virtual Placeholder* clone() = 0;
    };
    
    template<typename T>
    class Holder : public Placeholder
    {
    public:
        typedef T value_type;

        Holder(const T& value) : m_held(value) {}
        
        virtual int type() const
        {
            return Property<KeyType>::type<T>();
        }

        virtual Placeholder * clone()
        {
            return new Holder(m_held);
        }

        T get() const
        {
            return m_held;
        }

    private: // do not implement
        Holder & operator = (const Holder&);

    private:
        T m_held;
    };

public:
    typedef KeyType key_type;

    template<typename T> static const int type()
    {
        return PropertyTypeRegistry<T>::value;
    }

    template<typename T>
    Property(const key_type& key, T defaultValue = T())
        : m_key(key),
          m_type(type<T>()),
          m_value(new Holder<T>(defaultValue))
    {
    }

    ~Property()
    {
        delete m_value;
    }

    int type() const { return m_type; }
    const key_type& key() const { return m_key; }
    
    template<typename T> T get() const
    {
        // in debug mode, die loudly
        assert(type<T>() == m_type && "Invalid Type Conversion");
        return static_cast<Holder<T>*>(m_value)->get();
    }

    template<typename T> void set(const T& value)
    {
        delete m_value;
        m_type = type<T>();
        m_value = new Holder<T>(value);
    }

private:
    KeyType m_key;
    int m_type;
    Placeholder* m_value;
};

#endif // PROPERTY_H
