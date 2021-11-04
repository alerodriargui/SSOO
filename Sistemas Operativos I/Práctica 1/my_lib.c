#include <stdio.h>
#include <stdlib.h>
#include "my_lib.h"

size_t my_strlen(const char *str)
{
    size_t len = 0;
    int i = 0;
    while (str[i] != '\0')
    {
        i++;
        len++;
    }
    return len;
}

int my_strcmp(const char *str1, const char *str2)
{
    int i = 0;
    //Recorremos ambos strings para comprobar si hay algun caracter diferente
    //entre ellos, si es así se termina el bucle y se devuelve el resultado
    while (str1[i] != '\0' && str2[i] != '\0' && str1[i] == str2[i])
    {
        i++;
    }
    //Devolvemos resultado obtenido
    if (*str1 == *str2)
    {
        return 0;
    }
    else if (*str1 > *str2)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

char *my_strcpy(char *dest, const char *src)
{
    char *tmp = dest;

    while (*dest = *src)
    {
        dest++;
        src++;
    }

    return tmp;
}

char *my_strncpy(char *dest, const char *src, size_t n)
{
    for (int i = 0; i < n; i++)
    {
        dest[i] = src[i];
    }
    return src;
}

char *my_strcat(char *dest, const char *src)
{
    char *tmp = dest;
    //Situarse al final del String dest para añadir src a continuación
    while (*tmp != '\0')
    {
        tmp++;
    }
    //Adicion de src
    while (*src != '\0')
    {
        *tmp++ = *src++;
    }
    *tmp = '\0';
    return dest;
}

char *my_strchr(const char *str, int c)
{
    //Recorrer frase hasta el final o encontrar una coincidencia
    while (*str++)
    {
        //Si coinciden se devuelve el resto de la frase
        if (*str == (char)c)
        {
            return str;
        }
    }
    //Si no hay ninguna coincidencia se devuelve NULL
    return NULL;
}

struct my_stack *my_stack_init(int size)
{
    //Creación de variable y reserva de espacio
    struct my_stack *Pila = malloc(sizeof(struct my_stack));
    //Campo size = datos pasados por parámetro
    Pila->size = size;
    //Puntero top apuntando a NULL
    Pila->top = NULL;
    //Devolver Pila
    return Pila;
}

int my_stack_push(struct my_stack *stack, void *data)
{
    struct my_stack_node *nodo;
    //Si el nuevo nodo esta vacio o su tamaño es <= 0 se devuelve -1
    if ((stack == NULL) && (stack->size <= 0))
    {
        //EXIT_FAILURE
        return -1;
    }
    nodo = malloc(sizeof(struct my_stack_node));
    //Si el nuevo nodo esta vacio se devuelve -1
    if (nodo == NULL)
    {
        //EXIT_FAILURE
        return -1;
    }
    nodo->data = data;
    nodo->next = stack->top - 1;
    stack->top = nodo->next;
    return 0;
}

void *my_stack_pop(struct my_stack *stack)
{
    //Variable nodo que apunta al nodo superior de la pila pasada por parametro
    struct my_stack_node *nodo = stack->top;
    //Se inicializa data a devolver
    void *data = NULL;
    //Mientras no sea final de pila se puede sacar nodos
    if (nodo != NULL)
    {
        //Se asigna la data del nodo que se retira de la pila
        data = nodo->data;
        //Y el nodo que había siguinete al que se retira pasa a ser
        // el superior en la pila
        stack->top = nodo->next;
        //Liberamos memoria del nodo retirado
        free(nodo);
    }
    return data;
}

int my_stack_len(struct my_stack *stack)
{
    //Contador de nodos
    int len = 0;
    //Apuntador al primer nodo de la lista
    struct my_stack_node *nodo = stack->top;
    //Se recorre la pila hasta llegar al final
    while (nodo != NULL)
    {
        //se aumenta el contador
        len++;
        //Siguiente nodo
        nodo = nodo->next;
    }
    //Se devuelve el contador nodos
    return len;
}

int my_stack_purge(struct my_stack *stack)
{
    //Contador de nodos borrados
    int n = 0;
    //Variables de tipo nodo
    struct my_stack_node *nodo = stack->top;
    struct my_stack_node *nodoAux = NULL;
    //Se recorre la pila hasta llegar al final
    while (nodo != NULL)
    {
        //Se asigna el siguiente nodo a eliminar
        nodoAux = nodo->next;
        //Se libera espacio en memoria del nodo actual
        free(nodo->data);
        free(nodo);
        //Se prepara siguiente nodo a borrar
        nodo = nodoAux;
        //Aumenta el contador
        n++;
    }
    //Se libera espacio en memoria
    free(stack);
    return n;
}

int my_stack_write(struct my_stack *stack, char *filename)
{
}

struct my_stack *my_stack_read(char *filename)
{
    int file;
    file = open(filename, O_RDONLY);
    struct my_stack *pila;
    pila = my_stack_init(sizeof(struct my_stack));

    if(file==-1){
        printf("No se ha podido abrir el archivo");
    }

    else{
        read(file, pila, 4);
    }


}