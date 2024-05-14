#ifndef LIST_H
#define LIST_H
#include "Iterator.h"

/// @brief Template de una lista doblemente enlazada no ordenada.
/// @param T El tipo genérico de la lista.
template <class T>
class List
{
    private: 
    
        class Node 
        {
            public:
                T element;
                Node *next;
                Node *previous;
                Node() {}
                Node(Node *next, Node *previous, T element) : next{next}, previous{previous}, element{element} {}
                ~Node() {}
        };

        int _size;
        Node *_head; // Puntero fijo al primer elemento de la lista.
        Node *_tail; // Puntero fijo al último elemento de la lista.

        class ListIterator : public Iterator<T> 
        {
            private:
                Node *_current;

            public:
                ListIterator(Node *current) : _current{current} {}

                bool hasNext() 
                {
                    return _current != NULL;
                }
                T next() 
                {
                    T element = _current->element;
                    _current = _current->next;
                    return element;
                }
                ~ListIterator(){}
        };

        /// @brief Elimina el nodo de la lista y ajusta los punteros según el caso. Este método no actualiza el tamaño global de la lista.
        /// @param ptr El puntero del nodo a eliminar.
        void removeNode(Node *ptr) 
        {
            if(!isEmpty())
            {
                if(size() == 1)
                {
                    _head = NULL;
                    _tail = NULL;
                }
                else if(ptr == _head)
                {
                    _head = ptr->next;
                    ptr->next->previous = NULL;
                }
                else if(ptr == _tail)
                {
                    _tail = ptr->previous;
                    ptr->previous->next = NULL;
                }
                else
                {
                    ptr->previous->next = ptr->next;
                    ptr->next->previous = ptr->previous;
                }
                delete ptr;
            }            
        }

    public:
        /// @brief Constructor de la lista.
        explicit List() 
        {            
            _size = 0;
            _head = NULL;
            _tail = NULL;
        }
        /// @brief Destructor por defecto.
        ~List() 
        {
            clear();
        }
        
        /// @brief Agrega los elementos a la lista de menor (en primer lugar) a mayor.
        void addInOrder(T element)
        {
            Node * cursor = _head;
            while (cursor != NULL && cursor->element <= element)
            {
                cursor = cursor->next;
            }
            if(cursor == NULL)
            {
                // Se llegó al final de la lista.
                add(element);
            }
            else
            {   // El cursor es mayor al nuevo nodo en este caso.
                Node *newNode = new Node(cursor, NULL, element);
                newNode->previous = cursor->previous;
                if(cursor == _head)
                {
                    _head = newNode;
                }
                else
                {
                    cursor->previous->next = newNode;
                }
                cursor->previous = newNode;
                _size++;
            }
        }

        /// @brief Agrega un elemento al final de la lista.
        void add(T element) 
        {   
            Node *newNode = new Node(NULL, this->_tail, element);
            if(isEmpty()) 
            {
                _head = newNode;
                _tail = newNode;
            } 
            else 
            {
                // Se inserta el nodo al final de la lista.
                _tail->next = newNode;
                _tail = newNode;
            }
            _size++;
        } 

        /// @brief Retorna el elemento que se encuentra en el índice dado.
        /// @pre El índice se encuentra en el rango de la lista.
        /// @param index Posición del elemento a retornar.
        T get(int index) const 
        {
            assert(index >= 0 && index < size());
            if(index == 0) 
            {
                return _head->element;
            }
            if(index == size()-1) 
            {
                return _tail->element;
            }
            Node *cursor = _head;
            for (int i = 0; i < index; i++)
            {
                cursor = cursor->next;
            }
            return cursor->element;
        }      

        /// @brief Retorna el tamaño de la lista.
        int size() const 
        {
            return _size;
        }

        /// @brief Retorna true si la lista es vacía.
        bool isEmpty() const 
        {
            return size() == 0;
        }        

        /// @brief Elimina de la lista al elemento en caso exista. De lo contrario, el procedimiento no tiene efecto.
        void remove(T element) 
        {
            if(!isEmpty())
            {   
                Node *cursor = _head;
                while (cursor != NULL && cursor->element != element)
                {
                    cursor = cursor->next;
                }
                // Si el cursor no es nulo entonces se encontró al elemento.
                if(cursor != NULL)
                {
                    removeNode(cursor);
                    _size--;
                }                
            }
        }

        /// @brief Elimina de la lista al elemento que se encuentre en el índice en caso exista. 
        /// De lo contrario, el procedimiento no tiene efecto.
        /// @param index El índice del elemento a eliminar.
        /// @remark Elimina del principio y del final en orden 1.
        void removeIndex(int index) 
        {
            if(!isEmpty() && index < _size)
            {   
                if(index == 0) 
                {
                    removeNode(_head);
                } 
                else if(index == size()-1) 
                {
                    removeNode(_tail);
                } 
                else 
                {
                    Node *cursor = _head;
                    for (int i = 0; i < index; i++)
                    {
                        cursor = cursor->next;
                    }
                    removeNode(cursor);
                }                
                _size--;             
            }
        }
        
        /// @brief Elimina todos los elementos de la lista.
        void clear()
        {
            while (!isEmpty())
            {
                removeNode(_tail);
                _size--;
            }            
        }

        /// @brief Retorna un iterador de elementos de tipo T 
        /// situado en el primer elemento de la lista. Solicita memoria al retornar.
        Iterator<T> * getIterator()
        {
            return new ListIterator(_head);
        }
    
};

#endif
