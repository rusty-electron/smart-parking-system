    #include <NewPing.h>
     
    #define TRIGGER_PIN  9
    #define ECHO_PIN     10
    #define MAX_DISTANCE 350
    #define SIZE 30
     
    NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
    unsigned short dataSet[SIZE], dataIdx=0;
    unsigned short mean,sum,currentValue;
    
    void setup() {
      dataSet[SIZE] = {0};
      Serial.begin(115200);
    }
     
    void loop() {
      sum=0;
      if(dataIdx<SIZE){
        dataIdx++;
      }else{
        dataIdx=0;
      }
      currentValue = sonar.ping_cm();
      dataSet[dataIdx]=currentValue;

      for(int i=0;i<SIZE;i++){
        sum+=dataSet[i];
      }
      mean = sum/SIZE;
      delay(50);
      Serial.print(currentValue);
      Serial.print(",");
      Serial.println(mean);
    }

