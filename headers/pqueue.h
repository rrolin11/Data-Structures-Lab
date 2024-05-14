#ifndef PQUEUE_H
#define PQUEUE_H

#include "heap.h"

/// @brief Implementa una cola de prioridad basada en un heap. 
/// El valor de prioridad menor es el más prioritario.
template <class T>
class PQueue
{
    public:
        class Pair
        {
            public:
                float priority;
                T value;
                Pair() {}
                Pair(T value, float priority) : value(value), priority(priority) {}
                ~Pair() {}
                
                bool operator==(const Pair &o) const
                {
                    return this->priority == o.priority;
                };

                bool operator<=(const Pair &o) const
                {
                    return this->priority == o.priority || this->priority < o.priority;
                };

                bool operator>=(const Pair &o) const
                {
                    return this->priority == o.priority || this->priority > o.priority;
                };

                bool operator<(const Pair &o) const
                {
                    return this->priority < o.priority;
                };

                bool operator>(const Pair &o) const
                {
                    return this->priority > o.priority;
                };
        };
        int _population;
        int _size;
        Heap<Pair> * _queueHeap;

    public:

        PQueue(){}

        explicit PQueue(int size)
        {
            _population = 0;
            _size = size;
            _queueHeap = new Heap<Pair>(_size, MIN);
        }

        ~PQueue()
        {
            delete _queueHeap;
        }

        /// @brief Agrega un elemento a la cola. Si la cola está llena, no tiene efecto.
        /// Precondición: La cola no está llena.
        void enqueue(T value, float priority)
        {
            assert(!isFull());
            Pair p = Pair(value, priority);
            _queueHeap->add(p);
            _population++;
        }

        /// @brief Quita de la cola al primero.
        /// Si la cola está vacía, no tiene efecto.
        void dequeue()
        {
            if(!isEmpty())
            {                    
                _queueHeap->removeTop();
                _population--;
            }
        }

        /// @brief Retorna el primer elemento de la cola.
        /// Precondición: La cola no está vacía.
        T front() const
        {
            assert(!isEmpty());
            Pair p = _queueHeap->top();
            return p.value;
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

        bool isFull() const
        {
            return _population == _size;
        }

};

#endif