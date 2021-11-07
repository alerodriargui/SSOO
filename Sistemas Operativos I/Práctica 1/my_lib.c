#include <stdio.h>
#include <stdlib.h>
#include "my_lib.h"

/*
Miembros del grupo:
    Sergi Mayol Matos
    Alejandro Rodríguez Arguimbau 
    Nicolás Sanz Tuñón
*/

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
    int i;
    //Se copia el str hasta llegar al final
    for (i = 0; src[i] != '\0'; i++)
    {
        dest[i] = src[i];
    }
    //Adicion final str al dest
    dest[i] = '\0';
    //Se devuelve
    return dest;
}

char *my_strncpy(char *dest, const char *src, size_t n)
{
    //Se copia el str hasta que sea menor que n
    for (int i = 0; i < n; i++)
    {
        dest[i] = src[i];
    }
    //Se devuelve
    return dest;
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
            return (char *)str;
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
    //Variables
    struct my_stack_node *nodo;
    //Se reserva espacio para el nuevo nodo
    nodo = malloc(sizeof(struct my_stack_node));
    //Se asigna la data correspondiente al nodo
    nodo->data = data;
    //Si la pila no esta vacia y su tamaño es mayor que 0 se
    // prodece a añadir el nuevo nodo
    if ((stack != NULL) && (stack->size > 0))
    {
        if (stack->top == NULL)
        {
            stack->top = nodo;
            nodo->next = NULL;
        }
        else
        {
            nodo->next = stack->top;
            stack->top = nodo;
        }
        //No ha ocurrido ningun fallo
        return 0;
    }
    else
    {
        //Se ha producido algun error
        return -1;
    }
}

void *my_stack_pop(struct my_stack *stack)
{
    //Variable nodo que apunta al nodo superior de la pila pasada por parametro
    struct my_stack_node *nodo = stack->top;
    //Se inicializa data a devolver
    void *data = NULL;
    //Mientras no sea final de pila se puede sacar nodos
    if (nodo == NULL)
    {
        return NULL;
    }
    //Y el nodo que había siguiente al que se retira pasa a ser
    // el superior en la pila
    stack->top = nodo->next;
    //Se asigna la data del nodo que se retira de la pila
    data = nodo->data;
    //Liberamos memoria del nodo retirado
    free(nodo);
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
    //Numero de bytes que ocupa la pila
    int bytesLiberados = sizeof(struct my_stack);
    //Se libera el espacio en memoria hasta llegar al final
    while (stack->top != NULL)
    {
        //Espacio que ocupa el nodo
        bytesLiberados += sizeof(struct my_stack_node);
        //Espacio que ocupan los datos
        bytesLiberados += stack->size;
        //Con pop sabemos la posicion del espacio en memoria que se tiene que liberar
        free(my_stack_pop(stack));
    }
    //Se libera espacio en memoria ocupado por el stack
    free(stack);
    return bytesLiberados;
}

//Función que recorre la pila nodo a nodo hasta el final y va escribiendo en fichero
void escrituraNodosRecursiva(struct my_stack_node *nodo, int fichero, int size)
{
    //Se escribe el nodo
    if (write(fichero, nodo->data, size) == -1)
    {
        //Si se produce algun fallo/error se vuelve
        return;
    }
    //Si el siguiente nodo al actual no es el final se escribe en el fichero
    if (nodo->next != NULL)
    {
        //Se escribe el siguiente nodo llamando a la funcion
        escrituraNodosRecursiva(nodo->next, fichero, size);
    }
}

int my_stack_write(struct my_stack *stack, char *filename)
{
    //Variables
    int size;
    int fichero;
    struct my_stack_node *nodo;

    //Nodo que apunta al nodo superior de la pila
    nodo = stack->top;
    //Se abre el enlace con el fichero a escribir dando los permisos correspondientes
    fichero = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    //Se asigna el tamaño de la pila
    size = stack->size;
    //Comprobacion de posible fallo en apertura de fichero
    if (fichero == -1)
    {
        //Si se produce algun fallo/error se devuelve -1
        return -1;
    }
    //Comprobacion de posible fallo en la escritura en el fichero
    if (write(fichero, &stack->size, sizeof(size)) == -1)
    {
        //Si se produce algun fallo/error se devuelve -1
        return -1;
    }
    //Se escribe nodo a nodo en el fichero de manera ordenada, para ello se debe escribir
    // de manera inversa los nodos de la pila.
    escrituraNodosRecursiva(nodo, fichero, size);
    //Cierre enlace fichero
    close(fichero);
    //Se devuelve la longitud de la pila
    return my_stack_len(stack);
}

struct my_stack *my_stack_read(char *filename)
{
    //Variables
    int fichero;
    int size;
    int bytes;
    void *datos;
    struct my_stack *pila;

    //Se abre el enlace con el fichero a leer dando los permisos correspondientes
    fichero = open(filename, O_RDONLY, S_IRUSR);
    //Comprobacion de posible fallo en apertura de fichero
    if (fichero == -1)
    {
        //Si se produce algun fallo/error se devuelve NULL
        return NULL;
    }
    //Se hace una lectura para obtener el tamaño de los datos a leer
    bytes = read(fichero, &size, sizeof(int));
    //Comprobacion de posible fallo en la lectura del fichero
    if (bytes == -1)
    {
        //Si se produce algun fallo/error se devuelve NULL
        return NULL;
    }
    //Se inicializa la pila
    pila = my_stack_init(size);
    //Se asigna a datos el espacio en memoria del tamaño de la pila
    datos = malloc(pila->size);
    //Leemos del fichero la cantidad de datos
    bytes = read(fichero, datos, pila->size);
    while (bytes == pila->size)
    {
        //Se hace un push en la pila con los datos obtenidos en lectura
        my_stack_push(pila, datos);
        //Se asigna a datos el tamaño de la pila
        datos = malloc(pila->size);
        //Se lee siguiente
        bytes = read(fichero, datos, pila->size);
    }
    //Se libera espacio en memoria ocupado por datos
    free(datos);
    //Cierre enlace fichero
    close(fichero);
    //Se devuelve la pila con los nodos leidos
    return pila;
}