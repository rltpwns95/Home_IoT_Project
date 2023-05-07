
int ledArray[3] = {3,5,6};  //R, G, B
void setup() {
  // put your setup code here, to run once:

    Serial.begin(9600);
    pinMode(3, OUTPUT);  //R
    pinMode(5, OUTPUT); //G  
    pinMode(6, OUTPUT); //B        
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned char r = 255;
  unsigned char g = 255;
  unsigned char b = 255;


  analogWrite(ledArray[0],~r); //r
  analogWrite(ledArray[1],~g); //g       
  analogWrite(ledArray[2],~b); //b    

}
