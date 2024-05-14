#ifndef HEAP_H
#define HEAP_H

enum Type {MAX , MIN};

/// @brief Implementa un heap max o min según se especifique, de tipo de dato T.
/// @tparam T 
template <class T>
class Heap
{
private:
    T *_array;
    int _population;
    int _size;
    Type _type;

    void free()
    {
        delete[] _array;
    }    

    void swap(int minElementIndex, int maxElementIndex)
    {
        if (minElementIndex != maxElementIndex)
        {
            T maxElementValue = _array[maxElementIndex];
            T minElementValue = _array[minElementIndex];

            _array[minElementIndex] = maxElementValue;
            _array[maxElementIndex] = minElementValue;
        }
    } 

    void swim(int index)
    {
        int parentIndex = (int)index / 2;
        while (parentIndex > 0 && index > 0 && 
        ((_type == MAX) ? _array[parentIndex] < _array[index] : _array[parentIndex] > _array[index]))
        {
            swap(index, parentIndex);

            index = parentIndex;
            parentIndex = (int)index / 2;
        }
    }

    void sink(int index)
    {
        int leftChildIndex = index * 2;
        int rightChildIndex = index * 2 + 1;

        bool needToSink = true;

        while (needToSink && index <= _population)
        {
            int topElementIndex = index;

            if (leftChildIndex <= _population &&
                (_type == MAX ? _array[leftChildIndex] > _array[index] : _array[leftChildIndex] < _array[index]))
            {

                topElementIndex = leftChildIndex;

                if (rightChildIndex <= _population &&
                    (_type == MAX ? _array[rightChildIndex] > _array[topElementIndex] : _array[rightChildIndex] < _array[topElementIndex]))
                {

                    topElementIndex = rightChildIndex;
                }
            }
            else if (rightChildIndex <= _population &&
                     (_type == MAX ? _array[rightChildIndex] > _array[index] : _array[rightChildIndex] < _array[index]))
            {
                topElementIndex = rightChildIndex;
            }
            else
            {
                needToSink = false;
            }

            swap(index, topElementIndex);

            index = topElementIndex;

            leftChildIndex = index * 2;
            rightChildIndex = index * 2 + 1;
        }
    }

    int getPopulation() const
    {
        return _population;
    }

public:

    Heap(){}
    
    /// @brief Crea un heap dado un tamaño, y su tipo.
    /// @param size Cantidad de elmentos que podrá alojar el heap.
    /// @param type MIN o MAX para construir un minheap o maxheap.
    explicit Heap(int size, Type type)
    {
        _type = type;
        _size = size + 1;
        _population = 0;
        _array = new T[_size];
    }

    ~Heap()
    {
        free();
    }

    T *getArray()
    {
        return _array;
    }

    bool isEmpty() const
    {
        return _population == 0;
    }

    bool isFull() const
    {
        return _population == _size - 1;
    }

    T top() const
    {
        return _array[1];
    }

    bool add(T element)
    {
        bool added = false;

        if (!isFull())
        {
            _population++;
            _array[_population] = element;
            swim(_population);

            added = true;
        }

        return added;
    }

    bool removeTop()
    {
        bool removed = false;

        if (!isEmpty())
        {
            _array[1] = _array[_population];

            _population--;
            sink(1);

            removed = true;
        }
        return removed;
    }

};

#endif