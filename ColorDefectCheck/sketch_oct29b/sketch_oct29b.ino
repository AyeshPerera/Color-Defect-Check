#include <SoftwareSerial.h>

SoftwareSerial esp(2,3); //RX,TX


int cl=0;

String data;
      
String server = "192.168.8.162"; // your hostinger parent link

String uri = "/pepim/upload.php?clr=" + String(cl) ;// php file which is changing data in server
      
const int s0 = 8;  
const int s1 = 9;  
const int s2 = 12;  
const int s3 = 11;  
const int out = 10;   

// LED pins connected to Arduino

int redLed = 2;  
int greenLed = 3;  
int blueLed = A5;

// Variables  

int red = 0;  
int green = 0;  
int blue = 0;  

int def=0;
int clr=0;
int enable=1;
int sendd=0;

int val=0;

int clrout=0;

void setup()

{

      Serial.begin(9600);
      esp.begin(115200);
      
      pinMode(s0, OUTPUT);  
      pinMode(s1, OUTPUT);  
      pinMode(s2, OUTPUT);  
      pinMode(s3, OUTPUT);  
      pinMode(out, INPUT);

      pinMode(A0, INPUT);
      pinMode(A1, INPUT);
      pinMode(A2, INPUT);
      
      pinMode(A5, OUTPUT);
      pinMode(A4, OUTPUT);
      pinMode(A3, OUTPUT);
      digitalWrite(A5, HIGH); 

      //pinMode(inPin,INPUT);
      
      digitalWrite(s0, HIGH);  
      digitalWrite(s1, HIGH);  

      resetmod();
           

}

     
void loop() {

     uri = "/pepim/upload.php?clr=" + String(cl) + "&def=" + String(def) ;
     Serial.println(uri);  

     digitalWrite(A5 , HIGH);
     digitalWrite(A4 , LOW);

      if (digitalRead(A0) == HIGH) {
           digitalWrite(A3 , HIGH);
           enable=1;
           sendd=1;
      }
      if (digitalRead(A1) == HIGH) {           
           enable=0;
           sendd=0;
      }
      if (digitalRead(A2) == HIGH) {
           digitalWrite(A4 , HIGH);
           digitalWrite(A3 , LOW);

           def=0;
           clr=0;
           
           enable=1;
           sendd=0;
      }
     
   
       if(enable==0){  
             digitalWrite(A4 , HIGH);   
             getcolor();
            
       }
       if(sendd==1){
         senddata();
       }
    
 }

void senddata(){

     httppost();
          delay(1000);
 
}


void getcolor(){ 
  color(); 
  Serial.print("R Value:");  
  Serial.print(red, DEC);  
  Serial.print(" G Value: ");  
  Serial.print(green, DEC);  
  Serial.print(" B Value: ");  
  Serial.print(blue, DEC);
  Serial.print(" Defects: ");  
  Serial.print(def, DEC);   
  
  //Serial.println();  

  if (red < blue && red < green && red < 20)
  {  
   cl=1; 
   Serial.println(" - (Red Color)");  
   
  }  

  else if (blue < red && blue < green)   
  {  
   cl=2; 
   Serial.println(" - (Blue Color)");
    
  
  }  

  else if (green < red && green < blue)  
  {  

   cl=3; 
   Serial.println(" - (Green Color)"); 
    
 
  }  
  else{
  Serial.println(" - (Defect Found)"); 
  def=def+1;
  delay(500); 
  }
  delay(300);   
 
  
}

void color()  
{    
  digitalWrite(s2, LOW);  
  digitalWrite(s3, LOW);  
  //count OUT, pRed, RED  
  red = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);  
  digitalWrite(s3, HIGH);  
  //count OUT, pBLUE, BLUE  
  blue = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);  
  digitalWrite(s2, HIGH);  
  //count OUT, pGreen, GREEN  
  green = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);  
}
 
void resetmod() {

    esp.println("AT+RST");
    
    delay(500);
    
    if(esp.find("OK") ) Serial.println("Module Reset");

    delay(500);

    esp.print("AT+CWJAP=\"Void_Main(4G)\",\"3B5beee1\"\r\n");
    delay(2000);

} 

void httppost() {

    esp.println("AT+CIPSTART=\"TCP\",\"" + server + "\",80"); //start a TCP connection.

    delay(500);
    
    if ( esp.find((char*)"OK")) {
    
        Serial.println("TCP connection ready");

    }

        delay(500);
        
        String postRequest =
        
        "GET " + uri + " HTTP/1.1\r\n" +
        
        "Host: " + server + "\r\n" +
        
        "Accept: *" + "/" + "*\r\n" +
        
        "Content-Length: " + data.length() + "\r\n" +
        
        "Content-Type: application/x-www-form-urlencoded\r\n" +

        "\r\n";

        String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.
        
        esp.print(sendCmd);
        
        esp.println(postRequest.length());
        
        delay(500);

        if (esp.find((char*)">")) {
        
          Serial.println("Sending.."); esp.print(postRequest);
          delay(500);
        
          if ( esp.find((char*)"SEND OK")) {
          
              Serial.println("Packet sent");
              delay(500);
              
              esp.println("AT+CIPCLOSE");
              delay(500);
          
          }
      }
  
      delay(1000);
      sendd=0;
      resetmod();
      delay(2000);
}
