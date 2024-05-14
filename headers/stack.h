#ifndef STACK_H
#define STACK_H

#include "list.h"

/// @brief Implementa un stack dinámico de tipo T.
/// @tparam T el tipo genérico que guarda el stack.
template <class T>
class Stack
{
    private:
        int _population;
        List<T> _stackList;

    public:
        explicit Stack()
        {
            _population = 0;
            _stackList = List<T>();
        }

        ~Stack(){}

        /// @brief Agrega un elemento en la cima de la pila.
        void push(T value)
        {
            _stackList.add(value);
            _population++;
        }

        /// @brief Quita de la pila el elemento de la cima. 
        /// Si la pila está vacía, no tiene efecto.
        void pop()
        {
            if(size() > 0)
            {                    
                _stackList.removeIndex(size()-1);
                _population--;
            }
        }

        /// @brief Retorna el elemento de la cima de la pila.
        /// Precondición: La pila no está vacía.
        T peek() const
        {
            assert(!isEmpty());
            return _stackList.get(size()-1);
        }

        /// @brief Retorna el tamaño actual del stack.
        int size() const
        {
            return _population;
        }

        bool isEmpty() const
        {
            return _population == 0;
        }

};

#endif