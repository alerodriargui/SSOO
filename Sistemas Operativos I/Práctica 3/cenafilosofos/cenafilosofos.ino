/**
   Significado mensajes:
  TOC TOC → quiere entrar a comer
  |▄|    → se ha sentado a comer
  ¡o      → coge palillo izquierdo
  ¡o¡     → coge palillo derecho
  /o\ ÑAM → está comiendo
  ¡o_     → suelta palillo derecho
  _o      → suelta palillo izquierdo
  |_|     → sale de comer
*/

// Només fem servir el nucli app_cpu per simplicitat
// i tenint en compte que alguns esp32 són unicore
// unicore    -> app_cpu = 0
// 2 core  -> app_cpu = 1 (prog_cpu = 0)

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif
//#define INCLUDE_vTaskSuspend    1    //ja està posat per defecte sino descomentar es el temps de espera                //infinit als semàfors

/*************************** Variables Globals i definicions **************************************/

#define NUM_OF_PHILOSOPHERS 5                         //Nombre de filòsofs
#define MAX_NUMBER_ALLOWED (NUM_OF_PHILOSOPHERS - 1)  // Màxim nombre de filòsofs a l'habitació  (un menys que el total per evitar deadlock)
#define ESPERA 200  //interval d'espera de vTaskDelay

// Settings
enum { TASK_STACK_SIZE = 2048 };  // Bytes in ESP32, words in vanilla FreeRTOS

// Globals
static SemaphoreHandle_t bin_sem;   // Wait for parameters to be read
static SemaphoreHandle_t done_sem;  // Notifies main task when done
static SemaphoreHandle_t chopstick[NUM_OF_PHILOSOPHERS];

//*****************************************************************************
// Tasks

// The only task: eating
void eat(void *parameters) {

  int num;
  char buf[50];
  long randomNumber = random(0, ESPERA);
  // Copiar parámetro e incrementar el contador de semáforos
  num = *(int *)parameters;
  xSemaphoreGive(bin_sem);

  // El filósofo i quiere sentarse a comer
  sprintf(buf, "Filósofo %i: TOC TOC", num);
  Serial.println(buf);

  // El filósofo i se ha sentado a comer
  sprintf(buf, "Filósofo %i: |▄|", num);
  Serial.println(buf);
  
  // El filósofo i coge el palillo izquierdo
  xSemaphoreTake(chopstick[num], portMAX_DELAY);
  sprintf(buf, "Filósofo %i: ¡o", num);
  Serial.println(buf);

  // Add some delay to force deadlock
  //vTaskDelay(1 / portTICK_PERIOD_MS);

  //Cuando un filósofo ha cogido el palillo de la izquierda,  
  //pasa un tiempo aleatorio pensando en sus cosas de entre 0 y ESPERA 
  //(el tiempo de espera definido en el código) hasta coger el de su derecha .
  vTaskDelay(randomNumber / portTICK_PERIOD_MS);
  
  // El filósofo i coge el palillo derecho
  xSemaphoreTake(chopstick[(num+1)%NUM_OF_PHILOSOPHERS], portMAX_DELAY);
  sprintf(buf, "Filósofo %i: ¡o¡", num);
  Serial.println(buf);

 //Antes de decidirse a comer, 
 //todos los filósofos pasan un tiempo aleatorio pensando entre 0 y ESPERA.
  vTaskDelay(randomNumber / portTICK_PERIOD_MS);

  // El filósofo i se ha sentado a comer
  sprintf(buf, "Filósofo %i: |▄|", num);
  Serial.println(buf);
  // El filósofo i come
  sprintf(buf, "Filósofo %i: ÑAM", num);
  Serial.println(buf);
  vTaskDelay(randomNumber / portTICK_PERIOD_MS);

  // El filósofo i deja el palillo derecho
  xSemaphoreGive(chopstick[(num+1)%NUM_OF_PHILOSOPHERS]);
  sprintf(buf, "Filósofo %i: ¡o_", num);
  Serial.println(buf);

  // El filósofo i deja el palillo izquierdo
  xSemaphoreGive(chopstick[num]);
  sprintf(buf, "Filósofo %i: _o", num);
  Serial.println(buf);

  // Notify main task and delete self
  if (xSemaphoreGive(done_sem)) {
    sprintf(buf, "Filósofo %i: |_|", num);
  Serial.println(buf);
  }
  vTaskDelete(NULL);
}

//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup() {

  char task_name[20];
  // Configurar serial
  Serial.begin(115200);

  // Esperar un momento para no perder ningún dato
  vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("@ filósofo 0");
    Serial.println("@ filósofo 1");
    Serial.println("@ filósofo 2");
    Serial.println("@ filósofo 3");
    Serial.println("@ filósofo 4");
  

  // Create kernel objects before starting tasks
  bin_sem = xSemaphoreCreateBinary();
  done_sem = xSemaphoreCreateCounting(NUM_OF_PHILOSOPHERS, 0);
  for (int i = 0; i < NUM_OF_PHILOSOPHERS; i++) {
    chopstick[i] = xSemaphoreCreateMutex();
  }

  // Have the philosphers start eating
  for (int i = 0; i < NUM_OF_PHILOSOPHERS; i++) {
    sprintf(task_name, "Philosopher %i", i);
    xTaskCreatePinnedToCore(eat,
                            task_name,
                            TASK_STACK_SIZE,
                            (void *)&i,
                            1,
                            NULL,
                            app_cpu);
    xSemaphoreTake(bin_sem, portMAX_DELAY);
  }


  // Wait until all the philosophers are done
  for (int i = 0; i < NUM_OF_PHILOSOPHERS; i++) {
    xSemaphoreTake(done_sem, portMAX_DELAY);
  }

  // Say that we made it through without deadlock
  Serial.println("\nNO ha habido deadlock, el programa ha finalizado!");
}

void loop() {
  // Do nothing in this task
}
