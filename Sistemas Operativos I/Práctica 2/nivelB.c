/*
nivelB.c
Miembros del grupo:
    - Sergi Mayol Matos
    - Alejandro Rodríguez Arguimbau 
    - Nicolás Sanz Tuñón
*/

#define _POSIX_C_SOURCE 200112L

#define DEBUGN1 1 //parse_args()
#define DEBUGN3 1 //execute_line()

#define PROMPT_PERSONAL 1 // si no vale 1 el prompt será solo el carácter de PROMPT

#define RESET_FORMATO "\033[0m"
#define NEGRO_T "\x1b[30m"
#define NEGRO_F "\x1b[40m"
#define GRIS "\x1b[94m"
#define ROJO_T "\x1b[31m"
#define VERDE_T "\x1b[32m"
#define AMARILLO_T "\x1b[33m"
#define AZUL_T "\x1b[34m"
#define MAGENTA_T "\x1b[35m"
#define CYAN_T "\x1b[36m"
#define BLANCO_T "\x1b[97m"
#define NEGRITA "\x1b[1m"

#define COMMAND_LINE_SIZE 1024
#define ARGS_SIZE 64
#define N_JOBS 24 // cantidad de trabajos permitidos

char const PROMPT = '$';

#include <errno.h>     //errno
#include <stdio.h>     //printf(), fflush(), fgets(), stdout, stdin, stderr, fprintf()
#include <stdlib.h>    //setenv(), getenv()
#include <string.h>    //strcmp(), strtok(), strerror()
#include <unistd.h>    //NULL, getcwd(), chdir()
#include <sys/types.h> //pid_t
#include <sys/wait.h>
#include <signal.h>   //signal(), SIG_DFL, SIG_IGN, SIGINT, SIGCHLD
#include <fcntl.h>    //O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h> //S_IRUSR, S_IWUSR

int check_internal(char **args);
int internal_cd(char **args);
int internal_export(char **args);
int internal_source(char **args);
int internal_jobs();
int internal_bg(char **args);
int internal_fg(char **args);

char *read_line(char *line);
int parse_args(char **args, char *line);
int execute_line(char *line);

static char mi_shell[COMMAND_LINE_SIZE]; //variable global para guardar el nombre del minishell

//static pid_t foreground_pid = 0;
struct info_process
{
    pid_t pid;
    char status; // ‘N’, ’E’, ‘D’, ‘F’ (‘N’: ninguno, ‘E’: Ejecutándose y ‘D’: Detenido, ‘F’: Finalizado)
    char cmd[COMMAND_LINE_SIZE];
};
static struct info_process jobs_list[N_JOBS]; //Tabla de procesos. La posición 0 será para el foreground

void imprimir_prompt();

int check_internal(char **args)
{
    if (!strcmp(args[0], "cd"))
        return internal_cd(args);
    if (!strcmp(args[0], "export"))
        return internal_export(args);
    if (!strcmp(args[0], "source"))
        return internal_source(args);
    if (!strcmp(args[0], "jobs"))
        return internal_jobs(args);
    if (!strcmp(args[0], "bg"))
        return internal_bg(args);
    if (!strcmp(args[0], "fg"))
        return internal_fg(args);
    if (!strcmp(args[0], "exit"))
        exit(0);
    return 0; // no es un comando interno
}

int internal_cd(char **args)
{
    printf("[internal_cd()→ comando interno no implementado]\n");
    return 1;
}

int internal_export(char **args)
{
    printf("[internal_export()→ comando interno no implementado]\n");
    return 1;
}

int internal_source(char **args)
{
    printf("[internal_source()→ comando interno no implementado]\n");
    return 1;
}

int internal_jobs(char **args)
{
#if DEBUGN1
    printf("[internal_jobs()→ Esta función mostrará el PID de los procesos que no estén en foreground]\n");
#endif
    return 1;
}

int internal_fg(char **args)
{
#if DEBUGN1
    printf("[internal_fg()→ Esta función enviará un trabajo detenido al foreground reactivando su ejecución, o uno del background al foreground ]\n");
#endif
    return 1;
}

int internal_bg(char **args)
{
#if DEBUGN1
    printf("[internal_bg()→ Esta función reactivará un proceso detenido para que siga ejecutándose pero en segundo plano]\n");
#endif
    return 1;
}

void imprimir_prompt()
{
#if PROMPT_PERSONAL == 1
    printf(NEGRITA ROJO_T "%s" BLANCO_T ":", getenv("USER"));
    printf(AZUL_T "MINISHELL" BLANCO_T "%c " RESET_FORMATO, PROMPT);
#else
    printf("%c ", PROMPT);

#endif
    fflush(stdout);
    return;
}

char *read_line(char *line)
{

    imprimir_prompt();
    char *ptr = fgets(line, COMMAND_LINE_SIZE, stdin); // leer linea
    if (ptr)
    {
        // ELiminamos el salto de línea (ASCII 10) sustituyéndolo por el \0
        char *pos = strchr(line, 10);
        if (pos != NULL)
        {
            *pos = '\0';
        }
    }
    else
    { //ptr==NULL por error o eof
        printf("\r");
        if (feof(stdin))
        { //se ha pulsado Ctrl+D
            fprintf(stderr, "Bye bye\n");
            exit(0);
        }
    }
    return ptr;
}

int parse_args(char **args, char *line)
{
    int i = 0;

    args[i] = strtok(line, " \t\n\r");
#if DEBUGN1
    fprintf(stderr, GRIS "[parse_args()→ token %i: %s]\n" RESET_FORMATO, i, args[i]);
#endif
    while (args[i] && args[i][0] != '#')
    { // args[i]!= NULL && *args[i]!='#'
        i++;
        args[i] = strtok(NULL, " \t\n\r");
#if DEBUGN1
        fprintf(stderr, GRIS "[parse_args()→ token %i: %s]\n" RESET_FORMATO, i, args[i]);
#endif
    }
    if (args[i])
    {
        args[i] = NULL; // por si el último token es el símbolo comentario
#if DEBUGN1
        fprintf(stderr, GRIS "[parse_args()→ token %i corregido: %s]\n" RESET_FORMATO, i, args[i]);
#endif
    }
    return i;
}

int execute_line(char *line) //! no finalizado falta Asociar el manejador ctrlc a la señal SIGINT.
{
    char *args[ARGS_SIZE];
    pid_t pid, status;
    char command_line[COMMAND_LINE_SIZE];

    //copiamos la línea de comandos sin '\n' para guardarlo en el array de structs de los procesos
    memset(command_line, '\0', sizeof(command_line));
    strcpy(command_line, line); //antes de llamar a parse_args() que modifica line
    //Comprobar si tenemos argumentos en nuestro comando
    if (parse_args(args, line) > 0)
    {
        //Se identifica si se trata de un comando interno o externo
        if (!check_internal(args))
        {
            //Se crea un hijo con fork(), para que se encargue de que se ejecute el comando de forma externa.
            pid = fork();
            //Proceso hijo
            if (pid == 0)
            {
                //Asociar la acción por defecto a SIGCHLD
                signal(SIGCHLD, SIG_DFL);
                //Ignora la señal SIGINT
                signal(SIGINT, SIG_IGN);
                fprintf(stderr, "[execute_line()→ PID hijo: %d(%s)]\n", getpid(), args[0]);
                execvp(args[0], args);
                fprintf(stderr, "%s: no se encontró la orden\n", line);
                exit(-1);
            }
            else if (pid > 0) //Proceso padre
            {
                fprintf(stderr, "[execute_line()→ PID padre: %d(%s)]\n", getpid(), mi_shell);
                jobs_list[0].pid = pid;
                jobs_list[0].status = 'E'; // Proceso en ejecución
                while (jobs_list[0].pid != 0)
                {
                    pause();
                }
            }
            else
            {
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                //perror("Fork");
            }
        }
    }
    return 0;
}

//Manejador propio para la señal SIGCHLD (señal enviada a un proceso cuando uno
//de sus procesos hijos termina), también denominado enterrador de hijos.
void reaper(int signum)
{
    int status;
    pid_t ended;
    signal(SIGCHLD, reaper);
    //Analizamos el valor de waitpid para saber que hijo termina
    while ((ended = waitpid(-1, &status, WNOHANG)) > 0)
    {
        //En este nivel mostraremos un mensaje indicando el PID del hijo que ha finalizado y si ha sido por una señal.
        if (WIFEXITED(status))
        {
            fprintf(stderr, "\n[reaper()→ Proceso hijo %d(%s) finalizado con exit code %d]\n", ended, jobs_list[0].cmd, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            fprintf(stderr, "[reaper()→ Proceso hijo %d(%s) finalizado por señal %d]\n", ended, jobs_list[0].cmd, WTERMSIG(status));
        }
        //Reseteo de jobs_list
        jobs_list[0].pid = 0;
        jobs_list[0].status = 'F';
        strcpy(jobs_list[0].cmd, "");
    }
}

//Manejador propio para la señal SIGINT (Ctrl+C).
void ctrlc(int signum)
{
    //Llamada a la función signal asociandola a la misma funcion porque en algunos sistemas
    //una vez capturada por primera vez, se restaura a la acción por defecto.
    signal(SIGINT, ctrlc);
    //Si hay un proceso en foreground
    if (jobs_list[0].pid > 0)
    {
        //Si el proceso en foreground NO es el mini shell, ya que puedo haber ejecutado el minishell  dentro del minishell
        if (strcmp(jobs_list[0].cmd, mi_shell))
        {
            //Enviar la señal SIGTERM y notificarlo por pantalla.
            fprintf(stderr, "[ctrlc()→ Soy el proceso con PID %d(%s), el proceso en foreground es %d (%s)]\n",
                    getpid(), mi_shell, jobs_list[0].pid, jobs_list[0].cmd);
            printf("[ctrlc()→ Señal %d enviada a %d(%s) por %d(%s)]", SIGTERM, jobs_list[0].pid, jobs_list[0].cmd, getpid(), mi_shell);
            kill(jobs_list[0].pid, SIGTERM);
        }
        else
        {
            //Informar del proceso y del error producido
            fprintf(stderr, "[ctrlc()→ Soy el proceso con PID %d(%s), el proceso en foreground es %d (%s)]\n",
                    getpid(), mi_shell, jobs_list[0].pid, jobs_list[0].cmd);
            fprintf(stderr, "[ctrlc()→ Señal %d no enviada por %d(%s) debido a que su proceso en foreground es el shell]",
                    SIGTERM, getpid(), mi_shell);
        }
    }
    else
    {
        //Informar del proceso y del error producido
        fprintf(stderr, "\n[ctrlc()→ Soy el proceso con PID %d(%s), el proceso en foreground es %d(%s)]\n",
                getpid(), mi_shell, jobs_list[0].pid, jobs_list[0].cmd);
        fprintf(stderr, "[ctrlc()→ Señal %d no enviada por %d(%s)] debido a que no hay proceso en foreground]",
                SIGTERM, getpid(), mi_shell);
    }
    //Salto de linea seguido de un fflush(stdout) para forzar el vaciado del buffer de salida
    printf("\n");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    //llamada al enterrador de zombies cuando un hijo acaba (señal SIGCHLD)
    signal(SIGCHLD, reaper);
    //SIGINT es la señal de interrupción que produce Ctrl+C
    signal(SIGINT, ctrlc);
    //Obtenemos nombre del programa y lo guardamos en mi_shell
    strcpy(mi_shell, argv[0]);
    //Inicializacion de jobs_list[0] con pid = 0 status = 'N' y el cmd todo a '\0'
    jobs_list[0].pid = 0;
    jobs_list[0].status = 'N';
    strcpy(jobs_list[0].cmd, "");
    char line[COMMAND_LINE_SIZE];
    memset(line, 0, COMMAND_LINE_SIZE);
    while (1)
    {
        if (read_line(line))
        { // !=NULL
            execute_line(line);
        }
    }
    return 0;
}
