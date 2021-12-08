/*
nivelC.c
Miembros del grupo:
    - Sergi Mayol Matos
    - Alejandro Rodríguez Arguimbau
    - Nicolás Sanz Tuñón
*/

#define _POSIX_C_SOURCE 200112L

#define DEBUGN1 1 // parse_args()
#define DEBUGN3 1 // execute_line()

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

void reaper(int signum);
void ctrlc(int signum);
int is_background(char **args);
int jobs_list_add(pid_t pid, char status, char *cmd);
int jobs_list_find(pid_t pid);
int jobs_list_remove(int pos);
void ctrlz(int signum);

static char mi_shell[COMMAND_LINE_SIZE]; // variable global para guardar el nombre del minishell
int n_pids = 1;

// static pid_t foreground_pid = 0;
struct info_process
{
    pid_t pid;
    char status; // ‘N’, ’E’, ‘D’, ‘F’ (‘N’: ninguno, ‘E’: Ejecutándose y ‘D’: Detenido, ‘F’: Finalizado)
    char cmd[COMMAND_LINE_SIZE];
};
static struct info_process jobs_list[N_JOBS]; // Tabla de procesos. La posición 0 será para el foreground

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

// Muestra por pantalla la lista de trabajos que hay en segundo plano
int internal_jobs(char **args)
{
    for (int i = 1; i < n_pids; i++)
    {
        // Imprime cada elemento
        printf("[%d] %d\t%c\t%s\n", i, jobs_list[i].pid, jobs_list[i].status, jobs_list[i].cmd);
    }
    return 0;
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
    { // ptr==NULL por error o eof
        printf("\r");
        if (feof(stdin))
        { // se ha pulsado Ctrl+D
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
    /*#if DEBUGN1
        fprintf(stderr, GRIS "[parse_args()→ token %i: %s]\n" RESET_FORMATO, i, args[i]);
    #endif*/
    while (args[i] && args[i][0] != '#')
    { // args[i]!= NULL && *args[i]!='#'
        i++;
        args[i] = strtok(NULL, " \t\n\r");
        /*#if DEBUGN1
                fprintf(stderr, GRIS "[parse_args()→ token %i: %s]\n" RESET_FORMATO, i, args[i]);
        #endif*/
    }
    if (args[i])
    {
        args[i] = NULL; // por si el último token es el símbolo comentario
        /*#if DEBUGN1
                fprintf(stderr, GRIS "[parse_args()→ token %i corregido: %s]\n" RESET_FORMATO, i, args[i]);
        #endif*/
    }
    return i;
}

int execute_line(char *line)
{
    char *args[ARGS_SIZE];
    pid_t pid;
    int background;
    char command_line[COMMAND_LINE_SIZE];

    // copiamos la línea de comandos sin '\n' para guardarlo en el array de structs de los procesos
    memset(command_line, '\0', sizeof(command_line));
    strcpy(command_line, line); // antes de llamar a parse_args() que modifica line
    // Comprobar si tenemos argumentos en nuestro comando
    if (parse_args(args, line) > 0)
    {
        // Se identifica si se trata de un comando interno o externo
        if (args[0] && !check_internal(args))
        {
            // Se llama a la función is_background para analizar si en la línea de comandos hay un & al final
            background = is_background(args);
            // Se crea un hijo con fork(), para que se encargue de que se ejecute el comando de forma externa.
            pid = fork();
            // Proceso hijo
            if (pid == 0)
            {
                // Asociar la acción por defecto a SIGCHLD
                signal(SIGCHLD, SIG_DFL);
                // Ignora la señal SIGINT
                signal(SIGINT, SIG_IGN);
                execvp(args[0], args);
                exit(-1);
            }
            else if (pid > 0) // Proceso padre
            {
                fprintf(stderr, "[execute_line()→ PID padre: %d(%s)]\n", getpid(), mi_shell);
                fprintf(stderr, "[execute_line()→ PID hijo: %d(%s)]\n", pid, command_line);
                if (background) // Background
                {
                    // Se incopora el comando a la lista de trabajos
                    jobs_list_add(pid, 'E', command_line);
                }
                else // Foreground
                {
                    jobs_list[0].pid = pid;
                    jobs_list[0].status = 'E'; // Proceso en ejecución
                    strcpy(jobs_list[0].cmd, command_line);
                    while (jobs_list[0].pid != 0)
                    {
                        pause();
                    }
                }
            }
            else
            {
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            }
        }
    }
    return 0;
}

// Manejador propio para la señal SIGCHLD (señal enviada a un proceso cuando uno
// de sus procesos hijos termina), también denominado enterrador de hijos.
void reaper(int signum)
{
    int status;
    pid_t ended;
    int posFinal;
    signal(SIGCHLD, reaper);
    // Analizamos el valor de waitpid para saber que hijo termina
    while ((ended = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (ended == jobs_list[0].pid) // Foreground
        {
            if (WIFEXITED(status))
            {
                fprintf(stderr, "\n[reaper()→ Proceso hijo %d en foreground (%s) "
                                "finalizado con exit code %d]\n",
                        jobs_list[0].pid, jobs_list[0].cmd, WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status))
            {
                fprintf(stderr, "\n[reaper()→ Proceso hijo %d en foreground (%s) "
                                "finalizado por señal %d]\n",
                        ended, jobs_list[0].cmd, WTERMSIG(status));
            }
            jobs_list[0].pid = 0;
            jobs_list[0].status = 'F';
            strcpy(jobs_list[0].cmd, "");
        }
        else // Background
        {
            posFinal = jobs_list_find(ended);
            if (posFinal)
            {
                if (WIFEXITED(status))
                {
                    fprintf(stderr, "\n[reaper()→ Proceso hijo %d en background (%s) "
                                    "finalizado con exit code %d]\n",
                            ended, jobs_list[posFinal].cmd, WEXITSTATUS(status));
                    fflush(stderr);
                }
                else if (WIFSIGNALED(status))
                {
                    fprintf(stderr, "\n[reaper()→ Proceso hijo %d en background (%s) "
                                    "finalizado por señal %d]\n",
                            ended, jobs_list[posFinal].cmd, WTERMSIG(status));
                    fflush(stderr);
                }
                fprintf(stderr, "\nTerminado PID %d (%s) en jobs_list[%d] con estatus %d\n",
                        jobs_list[posFinal].pid,
                        jobs_list[posFinal].cmd,
                        posFinal, status);
                jobs_list_remove(posFinal);
                fflush(stderr);
            }
        }
    }
}

// Manejador propio para la señal SIGINT (Ctrl+C).
void ctrlc(int signum)
{
    // Llamada a la función signal asociandola a la misma funcion porque en algunos sistemas
    // una vez capturada por primera vez, se restaura a la acción por defecto.
    signal(SIGINT, ctrlc);
    // Si hay un proceso en foreground
    if (jobs_list[0].pid > 0)
    {
        // Si el proceso en foreground NO es el mini shell, ya que puedo haber ejecutado el minishell  dentro del minishell
        if (strcmp(jobs_list[0].cmd, mi_shell))
        {
            // Enviar la señal SIGTERM y notificarlo por pantalla.
            fprintf(stderr, "\n[ctrlc()→ Soy el proceso con PID %d(%s), el proceso en foreground es %d (%s)]\n",
                    getpid(), mi_shell, jobs_list[0].pid, jobs_list[0].cmd);
            fprintf(stderr, "[ctrlc()→ Señal %d (SIGTERM) enviada a %d(%s) por %d(%s)]", SIGTERM, jobs_list[0].pid, jobs_list[0].cmd, getpid(), mi_shell);
            kill(jobs_list[0].pid, SIGTERM);
        }
        else
        {
            // Informar del proceso y del error producido
            fprintf(stderr, "\n[ctrlc()→ Soy el proceso con PID %d(%s), el proceso en foreground es %d (%s)]\n",
                    getpid(), mi_shell, jobs_list[0].pid, jobs_list[0].cmd);
            fprintf(stderr, "[ctrlc()→ Señal %d (SIGTERM) no enviada por %d(%s) debido a que su proceso en foreground es el shell]",
                    SIGTERM, getpid(), mi_shell);
        }
    }
    else
    {
        // Informar del proceso y del error producido
        fprintf(stderr, "\n[ctrlc()→ Soy el proceso con PID %d(%s), el proceso en foreground es %d(%s)]\n",
                getpid(), mi_shell, jobs_list[0].pid, jobs_list[0].cmd);
        fprintf(stderr, "[ctrlc()→ Señal %d no enviada por %d(%s)] debido a que no hay proceso en foreground]",
                SIGTERM, getpid(), mi_shell);
    }
    // Salto de linea seguido de un fflush(stdout) para forzar el vaciado del buffer de salida
    printf("\n");
    fflush(stdout);
}

// Devuelve 1 si localiza background y sino, devuelve 0 (foreground)
int is_background(char **args)
{
    int i = 0;
    // Recorrer hasta encontrar el último token
    while (args[i] != NULL)
    {
        i++;
    }
    // Si el último token es "&"
    if (strcmp(args[i - 1], "&") == 0)
    {
        // Lo ponemos a NULL para que se pueda ejecutar
        args[i - 1] = NULL;
        // Se devuelve 1 si se cumple
        return 1;
    }
    // Sino, se devuelve 0 (foreground)
    return 0;
}

// Añade trabajos a la array si no se ha llegado a la cantidad
// máxima de trabajos permitidos (N_JOBS)
int jobs_list_add(pid_t pid, char status, char *cmd)
{
    // Si no se ha llegado al nº máximo de trabajos permitidos...
    if (n_pids < N_JOBS)
    {
        // Y se añade el trabajo dado por parámetros a la array
        jobs_list[n_pids].pid = pid;
        jobs_list[n_pids].status = status;
        strcpy(jobs_list[n_pids].cmd, cmd);
        printf("[%d]\t%d\t%c\t%s\n", n_pids, jobs_list[n_pids].pid, jobs_list[n_pids].status,
               jobs_list[n_pids].cmd);
        // Se suma 1 a la variable global n_pids
        n_pids++;
        return n_pids;
    }
    fprintf(stderr, "Se ha alcanzado el nº máximo de trabajos permitidos\n");
    return -1;
}

// Busca el pid dado por parámetro en el array de trabajos y
// devuelve su posición
int jobs_list_find(pid_t pid)
{
    // Bucle for para recorre los trabajos no finalizados
    for (int i = 0; i <= n_pids; i++)
    {
        // Si el pid coincide con alguno de la lista de
        // trabajos se devuelve su posicion
        if (jobs_list[i].pid == pid)
        {
            return i;
        }
    }
    return 0;
}

// Elimina el trabajo de la posición dada por parámetro de la array
int jobs_list_remove(int pos)
{
    // Si la pos dada se situa en la array actual de trabajos
    if (pos < n_pids)
    {
        // Y como se ha eliminado un trabajo,
        // se decrementa n_pids
        n_pids--;
        // Se mueve los registros del último elemento de la lista
        // al de la posición dada
        jobs_list[pos] = jobs_list[n_pids];
        /*jobs_list[pos].pid = jobs_list[n_pids].pid;
        jobs_list[pos].status = jobs_list[n_pids].status;
        strcpy(jobs_list[pos].command_line, jobs_list[n_pids].command_line)*/
    }
    else
    {
        return -1;
    }
    return 0;
}

// controlador de la señal SIGSTSP
void ctrlz(int signum)
{
    // Asociamos ctrlz a SIGTSTP
    signal(SIGTSTP, ctrlz);
    fprintf(stderr, "\n[ctrlz()→ Soy el proceso con PID %d, el proceso en foreground es %d(%s)]\n", getpid(), jobs_list[0].pid, jobs_list[0].cmd);
    if (jobs_list[0].pid > 0)
    {
        if (strcmp(jobs_list[0].cmd, mi_shell))
        {
            // Envio señal SIGTSTP e informar
            kill(jobs_list[0].pid, SIGTSTP);
            fprintf(stderr, "[ctrlz()→ Señal %d (SIGSTOP) enviada a %d (%s) por %d(%s)]\n", signum,
                    jobs_list[0].pid, jobs_list[0].cmd, getpid(), mi_shell);
            // Cambiar el status del proceso a 'D' (detenido).
            jobs_list[0].status = 'D';
            // Añadir los datos del proceso detenido a jobs_list[n_pids].
            jobs_list_add(jobs_list[0].pid, jobs_list[0].status, jobs_list[0].cmd);
            // Resetear los datos de jobs_list[0] ya que el proceso ha dejado de ejecutarse en foreground.
            jobs_list[0].pid = 0;
            jobs_list[0].status = 'F';
            strcpy(jobs_list[0].cmd, "");
        }
        else
        {
            // Mensaje de info. error
            fprintf(stderr, "[ctrlz()→ Señal %d no enviada por %d(%s) debido a que su proceso en foreground es el shell]\n",
                    signum, getpid(), mi_shell);
        }
    }
    else
    {
        // Mensaje de info. error
        fprintf(stderr, "[ctrlz()→ Señal %d no enviada por %d(%s)] debido a que nohay proceso en foreground]\n",
                signum, getpid(), mi_shell);
    }
}

int main(int argc, char *argv[])
{
    // Llamada al enterrador de zombies cuando un hijo acaba (señal SIGCHLD)
    signal(SIGCHLD, reaper);
    // SIGINT es la señal de interrupción que produce Ctrl+C
    signal(SIGINT, ctrlc);
    // SIGTSTP es la señal de interrupción que produce Ctrl+Z
    signal(SIGTSTP, ctrlz);
    // Obtenemos nombre del programa y lo guardamos en mi_shell
    strcpy(mi_shell, argv[0]);
    // Inicializacion de jobs_list[0] con pid = 0 status = 'N' y el cmd todo a '\0'
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
