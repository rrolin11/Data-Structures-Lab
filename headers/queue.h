#ifndef QUEUE_H
#define QUEUE_H

#include "list.h"

/// @brief Implementa una cola FIFO dinámica de tipo T.
template <class T>
class Queue
{
    private:
        int _population;
        List<T> _queueList;

    public:
        explicit Queue()
        {
            _population = 0;
            _queueList = List<T>();
        }

        ~Queue(){}

        /// @brief Agrega un elemento a la cola
        void enqueue(T value)
        {
            _queueList.add(value);
            _population++;
        }

        /// @brief Quita de la cola al primero.
        /// Si la cola está vacía, no tiene efecto.
        void dequeue()
        {
            if(size() > 0)
            {                    
                _queueList.removeIndex(0);
                _population--;
            }
        }

        /// @brief Retorna el primer elemento de la cola.
        /// Precondición: La cola no está vacía.
        T front() const
        {
            assert(!isEmpty());
            return _queueList.get(0);
        }

        /// @brief Retorna el tamaño actual de la cola.
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