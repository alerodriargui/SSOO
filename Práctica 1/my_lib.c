#include <stdio.h>

struct my_stack_node {      // nodo de la pila (elemento)
  void *data;
  struct my_stack_node *next;
};
 
struct my_stack {   // pila
  int size;       // tamaño de data, nos lo pasarán por parámetro
  struct my_stack_node *top;  // apunta al nodo de la parte superior
};

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
    for(int i = 0; i < n; i++) dest[i] = src[i];
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
    while(*src!= '\0'){
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
    //Si no hay ninguna coincidencia se de NULL
    return NULL;
}

//Método que reserva espacio para variable struct my_stack
struct my_stack*my_stack_init(int size)
{
    //Creación de variable y reserva de espacio
    struct my_stack* Pila = malloc(sizeof(struct my_stack));
    //Campo size = datos pasados por parámetro
    Pila->size=size;
    //Puntero top apuntando a NULL
    Pila->top=NULL;   

    //Devolver Pila
    return Pila;
}

//Método que inserta nodo en la pila
int my_stack_push (struct my_stack *stack, void *data)
{
    //Llamar método my_stack_init a stack para reservar espacio
    stack = my_stack_init(sizeof(struct my_stack_node));

    //Si stack == NULL...
    if(stack==NULL){
        //Se devuelve -1 (EXIT_FAILURE)
        return -1;
    }

    //Si campo size del stack es igual o menor que 0...
    else if ((stack->size=0) || (stack->size<0))
    {
        //Se devuelve -1 (EXIT_FAILURE)
        return -1;
    }

    //Y si stack != NULL y campo size mayor que 0...
    else if ((stack!=NULL) && (stack->size>0))
    {
        //Se podrá insertar el nodo
       //Creación Nodo
        struct my_stack_node* Nodo = malloc(sizeof(struct my_stack_node));
        //Campo data del nodo igual al dado por parámetro
        Nodo->data=data;
        //Y el nodo apunta adonde apunta top
        Nodo->next=stack->top;
        //Y el top de la pila ahora va a apuntar al nodo insertado
        stack->top=Nodo;

        //Se devuelve 0 (EXIT_SUCCESS)
        return 0;
    }
}

//Método que elimina nodo superior pila
void *my_stack_pop (struct my_stack *stack)
{
    //Si pila vacía...
    if(stack==NULL){
        //Devuelve NULL
        return NULL;
    }
    //Sino
    else{
        //Puntero pila apunta donde apunta nodo superior
        stack->top=stack->top->next;
        //Y nodo superior apunta ahora a NULL para desaparecer de la pila
        stack->top->next=NULL;
    }
    
}

//Método que cuenta cuantos nodos hay
int my_stack_len (struct my_stack *stack)
{
    //Si pila vacía...
    if(stack==NULL){
        //Se devuelve 0
        return 0;
    }
    //Sino
    else{
        int i;
        //Se recorre la pila hasta que el puntero de la misma sea igual a NULL
        for(int i = 1; stack->top!=NULL;i++){
            //Pila apunta al siguiente elemento
            stack->top=stack->top->next;
         }
         //Se devuelve num. de nodos
         return i;
    }
    
}
