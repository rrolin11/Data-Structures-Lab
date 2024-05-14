#ifndef HASHTABLE_H
#define HASHTABLE_H

/// @brief Implementa un diccionario con una tabla de hash 
/// cerrada (open adressing) de redistribución cuadrática.
/// @tparam K clave de búsqueda del diccionario. 
/// @tparam V el valor que se asocia a la clave.
/// @note El cliente debe implementar la sobrecarga de operaciones
/// operator==, operator< y operator> para sus clases K y V en caso
/// sean custom.
template <class K, class V>
class HashTable
{
    private:
        class Bucket
        {
            public:
                K key;
                V value;
                Bucket(K key, V value) : key{key}, value{value} { }
        };
        int _population;
        int _size;
        bool * _removedBucketsMap;
        Bucket* * _table;
        int (*_hash)(K);

        float loadFactor() const
        {
            return (_population/_size);
        }

        bool wasBucketRemoved(int index) const
        {
            return _removedBucketsMap[index];
        }

        /// @brief Redimensiona la tabla de hash al próximo primo que sea mayor al doble 
        /// del tamaño de la tabla actual, y redistribuye todos sus buckets ocupados.
        void reHash() 
        {   
            int oldSize = _size;
            Bucket* * oldTable = _table;
            _size = nextPrime(oldSize*2);
            _table = new Bucket*[_size];
            for (int i = 0; i < oldSize; i++)
            {
                if(oldTable[i] != NULL)
                {
                    add(oldTable[i]->key, oldTable[i]->value);
                }
                delete oldTable[i];
                oldTable[i] = NULL;
            }
            delete[] _removedBucketsMap;
            _removedBucketsMap = new bool[_size];
            delete[] oldTable;
            oldTable = NULL;
        }
        
        /// @brief Libera la memoria de todos los bucket y de la tabla de hash.
        void free()
        {            
            for (int i = 0; i < _size; i++) {
                delete _table[i];
                _table[i] = NULL;
            };
            delete[] _table;
            delete[] _removedBucketsMap;
            _table = NULL;
            _removedBucketsMap = NULL;
        }

        /// @brief Utiliza el método de redistribución cuadrática para obtener el índice del bucket.
        /// @param i el número de iteraciones hechas hasta el momento para encontrar un bucket libre.
        int calculateIndex(K key, int i) const {
            return ((_hash(key) + i*i) % _size);
        }

        /// @brief Retorna el índice en la tabla de hash según la clave dada. 
        /// Si la clave no está en la tabla retorna -1.
        int getIndexByKey(const K key)
        {   
            int tries = 0;
            int candidateIndex = calculateIndex(key, tries);
            while ((_table[candidateIndex] != NULL && _table[candidateIndex]->key != key) 
                    || wasBucketRemoved(candidateIndex)) 
            {
                tries++;
                candidateIndex = calculateIndex(key, tries);
            };
            if(_table[candidateIndex] != NULL && _table[candidateIndex]->key == key)
            {
                return candidateIndex;
            }
            return -1;
        }
        
        int nextPrime(int top) 
        {
            if (top > 1) {
                for (int i = 2; i <= top/2; i++) {
                    if (top % i == 0) {
                        top++;
                        i = 2;
                    };
                };
            };
            return top;
        };

    public:
        explicit HashTable(int size, int (*hashFunction)(K))
        {
            _size = nextPrime(size*2);
            _table = new Bucket*[_size];
            _removedBucketsMap = new bool[_size];
            _population = 0;
            _hash = hashFunction;
            for (int i = 0; i < _size; i++)
            {
                _table[i] = NULL;
                _removedBucketsMap[i] = false;
            }
        }

        ~HashTable()
        {
            free();
        }

        /// @brief Retorna true si agregó la asociación al diccionario. 
        /// Si la clave ya existía, retorna false y el método no tiene efecto.
        bool add(K key, V value)
        {
            int tries = 0;
            int candidateIndex = calculateIndex(key, tries);
            while (_table[candidateIndex] != NULL) 
            {
                // No se permite volver a agregar la misma clave.
                if(_table[candidateIndex]->key == key)
                {
                    return false;
                }
                tries++;
                candidateIndex = calculateIndex(key, tries);
            };  
            _table[candidateIndex] = new Bucket(key, value);
            _removedBucketsMap[candidateIndex] = false;
            _population++;
            if(loadFactor() >= 0.5)
            {
                reHash();
            }
            return true;
        }

        void remove(const K key)
        {
            int index = getIndexByKey(key);
            if (index > -1) 
            {
                _removedBucketsMap[index] = true;
                delete _table[index];  
                _table[index] = NULL;
                _population--;
            }
        }

        bool tryGetValue(const K key, V &outValue)
        {
            bool found = false;
            int index = getIndexByKey(key);
            if (index > -1) {
                outValue = _table[index]->value;
                found = true;
            };
            return found;
        }

        /// @brief Retorna true si en el diccionario existe el valor dado.
        bool containsValue(const V value) const 
        {
            bool found = false;
            for (int i = 0; i < _size && !found; i++)
            {
                found = (_table[i]->value == value);
            }
            return found;
        }

        /// @brief Retorna true si en el diccionario existe la clave dada.
        bool containsKey(const K key) const 
        {
            return (getIndexByKey(key) > -1);
        }

        void printHash()
        {
            for (int i = 0; i < _size; i++)
            {
                if(_table[i] != NULL)
                {
                    cout << "Bucket " << i << ": " << _table[i]->value << endl;
                }                
            }            
        }
};

#endif