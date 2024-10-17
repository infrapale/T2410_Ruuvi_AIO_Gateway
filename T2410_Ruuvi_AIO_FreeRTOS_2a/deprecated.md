
The FreeRTOS semaphore was causing assert error when used

//SemaphoreHandle_t sema_radio = NULL;  Machts alles kaputt

  // sema_radio = xSemaphoreCreateMutex();
  // if (sema_radio = NULL)  { printf("Semaphore failure!!\n"); while(true); }
  // else printf("sema_radio is OK\n");
  // rc = xSemaphoreTake(sema_radio, portMAX_DELAY);
  // Serial.printf("Semaphore sema_radio = %d\n", rc);
// rc = xSemaphoreGive(sema_radio);
 
SemaphoreHandle_t sema_radio;


  // xTaskCreatePinnedToCore(Task1code,"Task1",10000,NULL,1,&Task1,0);                         
  // delay(500); 

  // xTaskCreatePinnedToCore(Task2code,"Task2",10000,NULL,1,&Task2,1);          
  // delay(500); 


void Task1code( void * parameter ){
  Serial.print("Task1 is running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    digitalWrite(led_1, HIGH);
    //Serial.print("#");
    vTaskDelay(500);
    digitalWrite(led_1, LOW);
    vTaskDelay(500);
  } 
}

void Task2code( void * parameter ){
  Serial.print("Task2 is running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    digitalWrite(led_2, HIGH);
    //Serial.print("@");
    vTaskDelay(1500);
    digitalWrite(led_2, LOW);
    vTaskDelay(1500);
  }
}
