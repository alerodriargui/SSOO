/*
nivelD.c
Miembros del grupo:
    - Sergi Mayol Matos
    - Alejandro Rodríguez Arguimbau 
    - Nicolás Sanz Tuñón
*/

//Implementa comando interno fg
int internal_fg(char**args){
    int pos;
    //Se pasa de char a int para pos
    pos = atoi(args);
    //Si pos mayor o igual que n_pids o pos igual a 0
    if(pos>=n_pids || pos == 0){
        //Entonces no existe el trabajo
        fprintf(stderr, ": no existe ese trabajo");
    }

    //Si el status del trabajo es 'D' (está detenido)
    if(jobs_list[pos].status == 'D'){
            //Se le hace continuar y su status pasa a 'E'
            kill(jobs_list[pos].pid, SIGCONT);
            jobs_list[pos].status = 'E';

            printf("[internal_fg()→ señal SIGCONT enviada a %d]\n", jobs_list[pos].pid);
        }

    //Se recorre hasta el final o encontrar '&'
    for(int i = 0; jobs_list[pos].cmd[i] != 0 &&
              jobs_list[pos].cmd[i] != '&'; i++){               
              }

    //Si hay '&'
    if(jobs_list[pos].cmd[i] == '&'){
        //Se elimina el '&'
        jobs_list[pos].cmd[i-1] = 0; 
    }

    //Copiar datos a jobs_list[0]
    jobs_list[0] = jobs_list[pos]; 
    //Eliminación jobs_list[pos]
    jobs_list_remove(pos);  
    printf("%s\n", jobs_list[0].cmd);

    //Mientras haya un proceso en ejecución en foreground 
    while (jobs_list[0].pid != 0) {
        //Ejecutar un pause()
        pause();  
     }

     return 1;
}

//Implementa comando interno bg
int internal_bg(char **args){
    int pos;
    //Se pasa de char a int para pos
    pos = atoi(args);
    //Si pos mayor o igual que n_pids o pos igual a 0
    if(pos>=n_pids || pos == 0){
        //Entonces no existe el trabajo
        fprintf(stderr, ": no existe ese trabajo");
        //Salir
        exit(-1);
    }

    //Si el status del trabajo es 'E'
    if(jobs_list[pos].status == 'E'){

            fprintf(,stderr, "el trabajo ya esta en segundo plano", args[1]);
        }
    //Sino
    else{
        //Status pasa a 'E'
        jobs_list[pos].status = 'E';

        //Se recorre hasta el final o encontrar '&'
        for(int i = 0; jobs_list[pos].cmd[i] != 0 &&
                jobs_list[pos].cmd[i] != '&'; i++){               
                }

        //Si no hay '&'
        if(jobs_list[pos].cmd[i] == '0'){
            //Se añade '&' al cmd
            jobs_list[pos].cmd[i] = ' ';
            jobs_list[pos].cmd[i+1] = '&';
            jobs_list[pos].cmd[i+2] = 0;
        }
    }
    //Enviar a jobs_list[pos].pid la señal SIGCONT
    kill(jobs_list[pos].pid, SIGCONT);  
    printf("[internal_bg()→ señal SIGCONT enviada a %d]\n",
                jobs_list[pos].pid);
    //Mostrar por pantalla el nº de trabajo, el PID, el estado y el cmd.
    printf("[%d] %d\t%c\t%s\n", pos, jobs_list[pos].pid,
                jobs_list[pos].status, jobs_list[pos].cmd);
    

     return 1;
}
