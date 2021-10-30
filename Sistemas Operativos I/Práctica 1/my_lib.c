#include <stdio.h>

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
