#ifndef TUPLE_H
#define TUPLE_H

/// @brief Implementa una tupla de dos datos, donde el primero es de tipo A, y
/// el segundo es de tipo B.
template <class A, class B>
class Tuple
{
    public:
        A first;
        B second;

    Tuple(){}

    Tuple(A first, B second) : first(first), second(second) {}

    bool operator==(const Tuple<A, B> &other) const
    {
        return (first == other.first && second == other.second);
    }

};

#endif