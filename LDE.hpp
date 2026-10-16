#pragma once 
#include <iostream>

using namespace std;

template <typename T>
struct Node
{
    int id;
    T data;
    Node<T> *prox = NULL;
    Node<T> *prev = NULL;
};

template <typename T>
struct LDE
{
    Node<T> *head = NULL;
    Node<T> *tail = NULL;
};

template <typename T>
void initializeLDE(LDE<T> &list)
{
    list.head = NULL;
    list.tail = NULL;
}

// Inseri sempre no final
template <typename T>
bool insertFinal(LDE<T> *list, int id, T data)
{
    Node<T> *new_node = new Node<T>;
    new_node->id = id;
    new_node->data = data;
    new_node->prox = NULL;
    new_node->prev = NULL;

    if (list->tail == NULL)
    {
        list->head = new_node;
        list->tail = new_node;
    }
    else
    {
        list->tail->prox = new_node;
        new_node->prev = list->tail;
        list->tail = new_node;
    }

    return true;
}

// Sempre de forma ordenada pelo id
template <typename T>
bool insertNode(LDE<T> *list, int id, T data)
{
    Node<T> *new_node = new Node<T>;
    new_node->id = id;
    new_node->data = data;
    new_node->prox = NULL;
    new_node->prev = NULL;

    Node<T> *aux;
    Node<T> *aux2;

    if (list->tail == NULL)
    {
        list->head = new_node;
        list->tail = new_node;
    }
    // Inserir no inicio
    else if (id < list->head->id)
    {
        aux = list->head;
        new_node->prox = list->head;
        list->head = new_node;
        aux->prev = list->head;
    }
    // Inserir no final
    else if (id > list->tail->id)
    {
        aux = list->tail;
        new_node->prev = list->tail;
        list->tail = new_node;
        aux->prox = list->tail;
    }
    // Inserir no meio
    else
    {
        aux = list->head;
        while (aux != NULL)
        {
            if (aux->prox == NULL)
            {
                break;
            }

            if (id > aux->id && id < aux->prox->id)
            {
                aux2 = aux->prox;

                new_node->prox = aux2;
                new_node->prev = aux;
                aux->prox = new_node;
                aux2->prev = new_node;
            }
            aux = aux->prox;
        }
    }

    return true;
}

// Remover node pelo id
template <typename T>
bool removeNode(LDE<T> *list, int id)
{
    Node<T> *aux, *aux2, *aux3;

    if (list->head == NULL)
    {
        return false;
    }

    // Caso seja o primeiro valor
    if (id == list->head->id)
    {
        aux = list->head;
        // Caso só exista esse valor na lista
        if (aux->prox == NULL)
        {
            list->head = NULL;
            list->tail = NULL;
            delete aux;
            return true;
        }
        // Caso tenha mais valores na lista
        else
        {
            aux2 = aux->prox;
            aux->prox = NULL;
            aux2->prev = NULL;
            list->head = aux2;
            delete aux;
            return true;
        }
    }

    // Caso seja o ultimo valor
    else if (id == list->tail->id)
    {
        aux = list->tail;
        aux2 = aux->prev;

        aux->prev = NULL;
        aux2->prox = NULL;

        list->tail = aux2;
        delete aux;
        return true;
    }
    else
    {
        aux = list->head;
        // Remover do meio
        while (aux != NULL)
        {
            if (aux->id == id)
            {
                aux2 = aux->prev;
                aux3 = aux->prox;
                aux2->prox = aux3;
                aux3->prev = aux2;

                aux->prox = NULL;
                aux->prev = NULL;

                delete aux;
                return true;
            }

            aux = aux->prox;
        }
    }

    return false;
}

template <typename T>
bool searchNode(LDE<T> *lista, int id){
    Node <T> *aux;

    if(lista->head == NULL) return false;

    aux = lista->head;

    while(aux != NULL){
        if(aux->id == id) return true;
        aux = aux->prox;
    }
    return false;
}

// Mostrar no terminal
template <typename T>
void showLDE(LDE<T> *lista)
{
    Node<T> *aux = lista->head;
    while (aux != NULL)
    {
        cout << aux->id << " ";
        aux = aux->prox;
    }
}

// Retornar node
template <typename T>
Node <T> *findNode(LDE<T> *list, int id){
    Node <T> *aux = list->head;
    while(aux != NULL){
        if(aux->id == id) return aux;
        aux = aux->prox;
    }
    return NULL;
}

// Liberar LDE
template <typename T>
void deleteLDE(LDE<T> *list)
{

    Node<T> *temp, *aux = list->head;

    while (aux != NULL)
    {
        temp = aux;
        aux = aux->prox;
        delete temp;
    }
    list->head = list->tail = NULL;
}