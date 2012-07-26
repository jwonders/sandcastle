// STD Includes
#include <cassert>

// Library Includes
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

// Project Includes

#ifndef _WONDERS_ALGORIHTM_H
#define _WONDERS_ALGORITHM_H

template<typename T, typename U>
struct TypePair
{
    typedef T typeA;
    typedef U typeB;
};

/**
 * Holds the type index
 */
template<int N>
struct algo_registered_index
{
    enum { value = N };
};

/**
 * Holds a type
 */
template<typename T>
struct algo_registered_type
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
struct algo_register_type : algo_registered_index<N>, algo_registered_type<T>
{
private:
    friend algo_registered_type<T> dummy(algo_registered_index<N> index)
    {
        return algo_registered_type<T>();
    }
};

template<typename T>
struct AlgorithmTypeRegistry;

#define REGISTER_ALGORITHM_TYPE(TYPE_A, TYPE_B, N)                       \
    template<> struct AlgorithmTypeRegistry<TypePair<TYPE_A, TYPE_B> > : \
        algo_register_type<TypePair<TYPE_A, TYPE_B>, (N)>                     \
    {                                                                    \
        enum { value = (N) };                                            \
    };
    
REGISTER_ALGORITHM_TYPE(int, int, 0);
REGISTER_ALGORITHM_TYPE(double, double, 1);

class Algorithm
{
private:

    class AlgorithmPlaceholder
    {
    public:
        virtual ~AlgorithmPlaceholder() {}

    public:
        virtual int type() const = 0;
        virtual boost::shared_ptr<AlgorithmPlaceholder> clone() = 0;
    };

    template<typename ArgT, typename ResT>
    class AlgorithmHolder : public AlgorithmPlaceholder
    {
    public:
        typedef ArgT argument_type;
        typedef ResT result_type;
        typedef boost::function<result_type (argument_type)> function_type;

        AlgorithmHolder(function_type func)
            : m_held(func)
        {
        }

        virtual int type() const
        {
            return Algorithm::type<argument_type, result_type>();
        }

        virtual boost::shared_ptr<AlgorithmPlaceholder> clone()
        {
            return boost::shared_ptr<AlgorithmHolder>(new AlgorithmHolder(m_held));
        }

        result_type call(argument_type arg) const
        {
            return m_held(arg);
        }

    private:
        function_type m_held;
    };

public:
    template<typename ArgT, typename ResT> static const int type()
    {
        return AlgorithmTypeRegistry<TypePair<ArgT, ResT> >::value;
    }

    template<typename ArgT, typename ResT>
    Algorithm(boost::function<ResT (ArgT)> algorithm)
        : m_type(type<ArgT, ResT>())
        , m_algorithm(new AlgorithmHolder<ArgT, ResT>(algorithm))
    {
    }

    ~Algorithm()
    {
    }

    int type() const { return m_type; }
    
    template<typename ArgT, typename ResT> 
    ResT call(ArgT arg) const
    {
        assert( (type<ArgT, ResT>()) == m_type && "Invalid Type Conversion");
        AlgorithmPlaceholder* aptr = m_algorithm.get();
        AlgorithmHolder<ArgT, ResT>* algorithm = static_cast<AlgorithmHolder<ArgT, ResT>*>(aptr);
        return algorithm->call(arg);
    }

    template<typename ArgT, typename ResT> 
    void set(boost::function<ResT (ArgT)> algorithm)
    {
        m_type = type<ArgT, ResT>();
        m_algorithm = boost::shared_ptr<AlgorithmHolder<ArgT, ResT> >(
            new AlgorithmHolder<ArgT, ResT>(algorithm));
    }

private:
    int m_type;
    boost::shared_ptr<AlgorithmPlaceholder> m_algorithm;
};

#endif // _WONDERS_ALGORITHM_H
