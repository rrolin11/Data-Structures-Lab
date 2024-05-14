#ifndef ITERATOR_H
#define ITERATOR_H

/// @brief Template de un iterador para cualquier clase de datos.
/// @tparam T El tipo de elementos sobre cual itera.
template <class T>
class Iterator
{
    public:
        virtual bool hasNext() = 0;
        virtual T next() = 0;
};

#endif