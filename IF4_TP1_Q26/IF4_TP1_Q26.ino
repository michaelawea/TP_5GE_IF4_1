/* Programme IF4_TP1_Q26
 * Implémentation d'un point de rendez-vous entre 3 tâches
 * La tâche 3 doit attendre que les tâches 1 et 2 aient terminé leur travail
 */

// Priorités des tâches
int task1_prio = 5;
int task2_prio = 5;
int task3_prio = 10;

// Variables globales représentant les tâches
TaskHandle_t Task1, Task2, Task3;

// Sémaphore compteur pour synchroniser les 3 tâches
// Le sémaphore sera initialisé à 0 et comptera jusqu'à 2
SemaphoreHandle_t xCountingSemaphore = NULL;

/**
 * Simule du travail en affichant des messages
 */
void simulate_work(const char* taskName, int duration_ms) {
  Serial.printf("[INFO] %s commence son travail\n", taskName);
  for(int i = 0; i < 5; i++) {
    Serial.printf("[INFO] %s travaille... (%d/5)\n", taskName, i+1);
    delay(duration_ms);
  }
  Serial.printf("[INFO] %s a terminé son travail\n", taskName);
}

/******************************
 * Code de la tâche 1
 *****************************/
void vTask1(void *pvParameters)
{
  Serial.printf("[INFO] Task1 / core %d démarrée\n", xPortGetCoreID());
  
  // Simule du travail (par exemple 200ms * 5 = 1 seconde)
  simulate_work("Task1", 200);
  
  // Signale que la tâche 1 a terminé
  xSemaphoreGive(xCountingSemaphore);
  Serial.printf("[INFO] Task1 a signalé sa fin (compteur = %d)\n", 
                uxSemaphoreGetCount(xCountingSemaphore));
  
  // La tâche se termine
  vTaskDelete(NULL);
}

/******************************
 * Code de la tâche 2
 *****************************/
void vTask2(void *pvParameters)
{
  Serial.printf("[INFO] Task2 / core %d démarrée\n", xPortGetCoreID());
  
  // Simule du travail différent (par exemple 150ms * 5 = 0.75 seconde)
  simulate_work("Task2", 150);
  
  // Signale que la tâche 2 a terminé
  xSemaphoreGive(xCountingSemaphore);
  Serial.printf("[INFO] Task2 a signalé sa fin (compteur = %d)\n", 
                uxSemaphoreGetCount(xCountingSemaphore));
  
  // La tâche se termine
  vTaskDelete(NULL);
}

/******************************
 * Code de la tâche 3
 *****************************/
void vTask3(void *pvParameters)
{
  Serial.printf("[INFO] Task3 / core %d démarrée\n", xPortGetCoreID());
  Serial.printf("[INFO] Task3 attend que Task1 et Task2 terminent...\n");
  
  // Attend que les 2 premières tâches aient terminé
  // On doit prendre le sémaphore 2 fois
  for(int i = 0; i < 2; i++) {
    xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);
    Serial.printf("[INFO] Task3 a reçu signal %d/2\n", i+1);
  }
  
  Serial.printf("[INFO] Task3 : Les deux tâches précédentes ont terminé!\n");
  Serial.printf("[INFO] Task3 peut maintenant commencer son travail\n");
  
  // Task3 fait maintenant son propre travail
  simulate_work("Task3", 100);
  
  Serial.printf("[INFO] *** TOUTES LES TÂCHES SONT TERMINÉES ***\n");
  
  // La tâche se termine
  vTaskDelete(NULL);
}

/**
 * Fonction setup() appelée au démarrage
 */
void setup()
{
  // Initialisation de la communication série
  Serial.begin(115200);
  while (!Serial);
  Serial.println("===================================");
  Serial.println("[INFO] Démarrage du programme");
  Serial.println("[INFO] Point de rendez-vous (Rendezvous)");
  Serial.println("===================================\n");

  // Création d'un sémaphore compteur
  // Valeur maximale : 2 (pour compter les 2 premières tâches)
  // Valeur initiale : 0 (aucune tâche n'a terminé)
  xCountingSemaphore = xSemaphoreCreateCounting(2, 0);
  
  if(xCountingSemaphore == NULL) {
    Serial.println("[ERREUR] Impossible de créer le sémaphore");
    for(;;); // Boucle infinie en cas d'erreur
  }

  // Création des tâches
  // Task1 et Task2 ont la même priorité et s'exécutent en parallèle
  // Task3 a une priorité plus haute pour s'exécuter dès que les conditions sont remplies
  xTaskCreatePinnedToCore(vTask1, "Task1", 10000, NULL, task1_prio, &Task1, 0);
  xTaskCreatePinnedToCore(vTask2, "Task2", 10000, NULL, task2_prio, &Task2, 1);
  xTaskCreatePinnedToCore(vTask3, "Task3", 10000, NULL, task3_prio, &Task3, 0);
  
  Serial.println("[INFO] Toutes les tâches ont été créées\n");
  
  // Le setup se termine et se supprime
  vTaskDelete(NULL);
}

/**
 * Fonction loop() - non utilisée
 */
void loop()
{
  // Vide - toutes les tâches sont gérées par FreeRTOS
}
