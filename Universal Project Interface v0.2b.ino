/*
Universal Project Interface v0.2b
Robert James Hastings
Github CountZero1066

   ###############################################################
   #              Universal Project Interface v0.2b              #
   #      A handheld, ESP32 based platform intended to serve     #
   #      as an interface for all personal projects via          #
   #      a tile based GUI and facilitated by ESPnow/Bluetooth   #
   ###############################################################
   
   Version Detail:
            -Basic GUI interaction   

   Hardware:
            ESP32(NodeMCU)
            PCF8574
            2x SSD1306 OLED (32x128, 64x128)      
            3.7V 500mAh Lipo
            TP4056 battery charging module
            3.7V - 5V boost converter module
            5x 4 pin momentary push button switches
            2x 4.7k ohm resistors
            Dupont & JST connectors
*/
//-------------------------------------------------------------------------------------------
//Preprocessor directives,Libaries,Headers,Global Variables and Definitions
//-------------------------------------------------------------------------------------------
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "bitmap_images.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3D
#define splash_height   128
#define splash_width    64
#define LOGO_HEIGHT   51
#define LOGO_WIDTH    52
#define I2C_SDA 23
#define I2C_SCL 22
//use non-default I2C SDA and SCL pins via Twowire instance
TwoWire I2Cbus1 = TwoWire(0);
Adafruit_SSD1306 display1(128, 64, &I2Cbus1, OLED_RESET);
Adafruit_SSD1306 display2(128, 32, &I2Cbus1, OLED_RESET);

TaskHandle_t Task_0;
TaskHandle_t Task_1;

short int i;
int menu_index = 0, prior_menu_index = 0, check_index_change = 0;
bool main_menu_active = false;
const unsigned short int gui_x_cords[] = {4, 35, 66, 97};
//OLED coordinates for redrawing main menu tiles
const unsigned short int redraw_menu_tile_array[8][4] = {{5,5,25,25}, 
                                                        {36,5,25,25}, 
                                                        {67,5,25,25}, 
                                                        {98,5,25,25}, 
                                                        {5,35,25,25}, 
                                                        {36,35,25,25}, 
                                                        {67,35,25,25}, 
                                                        {98,35,25,25}};
//Pointer array for programatically cycling through and accessing stored BMP icons defined in 'bitmap_images.h'
const unsigned char* main_menu_bmp_pointer[] = {keyboard_bmp, 
                                                Power_bmp, 
                                                Ozone_bmp, 
                                                JSON_bmp, 
                                                wifi_bmp, 
                                                esp_now_bmp, 
                                                RFID_bmp, 
                                                settings_bmp, 
                                                keyboard_bmp_inverted,
                                                Power_bmp_inverted,
                                                Ozone_bmp_inverted,
                                                JSON_bmp_inverted,
                                                wifi_bmp_inverted,
                                                esp_now_bmp_inverted,
                                                RFID_bmp_inverted,
                                                settings_bmp_inverted
                                                };
//-------------------------------------------------------------------------------------------
//Setup
//-------------------------------------------------------------------------------------------
void setup() {  
  Serial.begin(115200);
  I2Cbus1.begin(I2C_SDA, I2C_SCL,400000); //initialize the I2C communication with non-default pins
  display1.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  delay(50);
  display2.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    disableCore0WDT();
    disableCore1WDT();            
  display1.clearDisplay();
  delay(50);
  display2.clearDisplay();
  display2.setTextColor(WHITE);
  display2.setTextSize(1);
  display2.setCursor(20, 16);
  display2.println("CountZero1066");
  display2.display();
  show_splash_bitmap();
  //draw_github_details();
  //progress_bar();
  Draw_GUI();
  main_menu_active = true;
//set up keypad
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.beginTransmission(0x20);
  Wire.write(0xFF);
  Wire.endTransmission();
  
          xTaskCreatePinnedToCore(
                            Task_Core_0,   /* Task function. */
                            "Task_0",     /* name of task. */
                            16000,       /* Stack size of task */
                            NULL,        /* parameter of the task */
                            6,           /* priority of the task */
                            &Task_0,      /* Task handle to keep track of created task */
                            0);          /* pin task to core 0 */                  
        
          //create a task that will be executed in the Task_Draw_Dispaly_code() function, with priority 1 and executed on core 1
          xTaskCreatePinnedToCore(
                            Task_Core_1,   /* Task function. */
                            "Task_1",     /* name of task. */
                            10000,       /* Stack size of task */
                            NULL,        /* parameter of the task */
                            7,           /* priority of the task */
                            &Task_1,      /* Task handle to keep track of created task */
                            1);          /* pin task to core 1 */
}

//-------------------------------------------------------------------------------------------
//Main Loop
//-------------------------------------------------------------------------------------------
//Core0
//-------------------------------------------------------------------------------------------
void Task_Core_0( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  for(;;){ 
      main_menu_selection();
      delay(50);          
  }
  vTaskDelay(10);
}
//-------------------------------------------------------------------------------------------
//Core1
//-------------------------------------------------------------------------------------------
void Task_Core_1( void * Parameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  
  for(;;){ 
    Wire.requestFrom(0x20,1);
    while (Wire.available()) {
        uint8_t bits = Wire.read();
        char c = ((char)Wire.read()); 
             if(bits != 255){
                Serial.println(c);      
                Serial.println( bits);
                      switch(bits){
                        Serial.println(":" + menu_index); 
                              case 247:
                              //up key
                                     if(main_menu_active ==true){
                                          if(menu_index <= 3){
                                           prior_menu_index = menu_index;
                                           menu_index = menu_index + 4;
                                           }
                                           else if(menu_index >=4){
                                           prior_menu_index = menu_index;
                                           menu_index = menu_index -4;
                                           }      
                                     }
                              break;

                              case 254:
                              //down key
                                     if(main_menu_active ==true){
                                          if(menu_index >= 4){
                                          prior_menu_index = menu_index;
                                          menu_index = menu_index  - 4;
                                          }
                                          else if(menu_index <=3){
                                          prior_menu_index = menu_index;
                                          menu_index = menu_index  +4;
                                          }      
                                     }
                              break;

                              case 239:
                              //left key
                                     if(main_menu_active ==true){
                                          if(menu_index <= 0){
                                          prior_menu_index = menu_index;
                                          menu_index = 7;
                                          }
                                          else {
                                          prior_menu_index = menu_index;
                                          menu_index--;
                                          }
                                     }
                              break;

                              case 251:
                              //right key
                                     if(main_menu_active ==true){
                                          if(menu_index >= 7){
                                          prior_menu_index = menu_index;
                                          menu_index = 0;
                                          }
                                          else {
                                          prior_menu_index = menu_index;
                                          menu_index++;
                                          }
                                     }
                             break;

                             case 253:
                             //center key
                                    if(main_menu_active ==true){
                                    //index select
                                    Serial.println("Select button pressed, no current function associated");
                                    }
                             break;
                    } //end of switch
                      
                    } //end of If
                    delay(150); //Important Note: I believe from experimentation, that this delay prevents (or at least minimises) the chance of an I2C bus collision
                                //                In addition, because this is effectivly an infinite loop, the delay should help prevent a Core panic while also
                                //                allowing the user more control over the button selection as the delay translates into how long you have for a 
                                //                single button press to be registered before it's interrupted as a consecutive button press
            }  //end of while
     }
     
     vTaskDelay( pdMS_TO_TICKS( 150 ) );
  }
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
void loop() {
 vTaskDelete(NULL);
}
//-------------------------------------------------------------------------------------------
//Main menu tile selection
//-------------------------------------------------------------------------------------------
void main_menu_selection(void){
  
 
  if (check_index_change != menu_index){
      check_index_change = menu_index;
         if(menu_index == 0 && prior_menu_index == 0){
          Serial.print("prior_menu_index:");
  Serial.print(prior_menu_index);
  Serial.print(" menu_index:");
  Serial.print(menu_index);
  printf("Previous image memory address: %x\n", &main_menu_bmp_pointer[prior_menu_index]);
  printf("Current image memory address: %x\n", &main_menu_bmp_pointer[menu_index]);
  printf("Active image memory address: %x\n", &main_menu_bmp_pointer[menu_index + 8]);
            display1.fillRect(redraw_menu_tile_array[0][0],redraw_menu_tile_array[0][1],25,25,BLACK);
            display1.drawBitmap(redraw_menu_tile_array[0][0], redraw_menu_tile_array[0][0], main_menu_bmp_pointer[8],25,25,WHITE);
            display1.display();
         }
         else if(menu_index != prior_menu_index) {
          Serial.println("check_index_change:");
  Serial.print(check_index_change);
  Serial.print(" menu_index:");
  Serial.print(menu_index);
  main_menu_context_display();
  printf("Previous image memory address: %x\n", &main_menu_bmp_pointer[prior_menu_index]);
  printf("Current image memory address: %x\n", &main_menu_bmp_pointer[menu_index]);
  printf("Active image memory address: %x\n", &main_menu_bmp_pointer[menu_index + 8]);
           display1.fillRect(redraw_menu_tile_array[prior_menu_index][0],redraw_menu_tile_array[prior_menu_index][1],25,25,BLACK);
           display1.drawBitmap(redraw_menu_tile_array[prior_menu_index][0], redraw_menu_tile_array[prior_menu_index][1], main_menu_bmp_pointer[prior_menu_index],25,25,WHITE);
           display1.fillRect(redraw_menu_tile_array[menu_index][0],redraw_menu_tile_array[menu_index][1],25,25,BLACK);
           display1.drawBitmap(redraw_menu_tile_array[menu_index][0], redraw_menu_tile_array[menu_index][1], main_menu_bmp_pointer[menu_index + 8],25,25,WHITE);
           display1.display();
         }
      
  }
}

void main_menu_context_display(){
  display2.clearDisplay();
  delay(10);
                      display2.setTextColor(WHITE);
                      display2.setTextSize(1);
                      display2.setCursor(5, 16);
  if(menu_index == 0){
                        display2.println("Keyboard");
                      }
                      else if(menu_index == 1){
                        display2.println("Smart 12V Power");
                      }
                      else if(menu_index == 2){
                        display2.println("Ozone Generator");
                      }
                      else if(menu_index == 3){
                        display2.println("JSON");
                      }
                      else if(menu_index == 4){
                        display2.println("Connect WiFi");
                      }
                      else if(menu_index == 5){
                        display2.println("Remote");
                      }
                      else if(menu_index == 6){
                        display2.println("RFID Lock");
                      }
                      else if(menu_index == 7){
                        display2.println("Settings");
                      }

//                      switch(menu_index){
//                       case 0:
//                            display2.println("Keyboard");
//                            break;
//                       case 1:
//                            display2.println("Smart 12V Power");
//                            break;
//                       case 2:
//                            display2.println("Ozone Generator");
//                            break;
//                       case 3:
//                            display2.println("JSON");
//                            break;
//                       case 4:
//                            display2.println("Connect WiFi");
//                            break;
//                       case 5:
//                            display2.println("Remote");
//                            break;
//                       case 6:
//                            display2.println("RFID Lock");
//                            break;
//                       case 7:
//                            display2.println("Settings");
//                            break;
//                       default:
//                            display2.println(menu_index);
//                            break;
//                       }

                display2.display();
}
//-------------------------------------------------------------------------------------------
//Draw Bitmap Splashscreen
//-------------------------------------------------------------------------------------------
void show_splash_bitmap(void) {
  display1.clearDisplay();
  display1.drawBitmap(0, 0, bitmap, splash_height, splash_width, WHITE);
  display1.display();
  delay(3000);
}
//-------------------------------------------------------------------------------------------
//Draw Bitmap image of Github Logo
//-------------------------------------------------------------------------------------------
void draw_github_details(void) {
  display1.clearDisplay();
  display1.drawBitmap(38, 0, github_bmp_logo,52,52,WHITE);
  display1.setTextColor(WHITE);
  display1.setTextSize(1);
  display1.setCursor(26, 53);
  display1.println("CountZero1066");
  display1.display();
  delay(1500); //main program tasks have yet to be invoked, thus a blocking delay has no negative impact 
}
//-------------------------------------------------------------------------------------------
//Animated Loading Screen
//-------------------------------------------------------------------------------------------
void progress_bar(){  
  display1.clearDisplay();
  display1.setTextSize(2);
  display1.setTextColor(WHITE);
  display1.setCursor(22, 20);
  display1.println("LOADING");
  display1.display();
  display1.drawRect(0, 44, 128, 20, WHITE);
  display1.display();
  delay(1000);  
      for(int i = 0; i <= 125; i = i + 5){
        display1.fillRect(2, 46, i, 16, WHITE); 
        display1.display();
        delay(25);
      }
  delay(200);
}
//-------------------------------------------------------------------------------------------
//Draw Main Menu
//-------------------------------------------------------------------------------------------
void Draw_GUI(){
  menu_index = 0;
  display1.clearDisplay();
  display1.setTextSize(1); 
  //first row
   for(int i = 0; i <=3; i++){
     display1.drawRect(gui_x_cords[i], 4, 27, 27, WHITE);
   }
   display1.drawBitmap(5, 5, keyboard_bmp_inverted,25,25,WHITE);
   display1.drawBitmap(36, 5, Power_bmp,25,25,WHITE);
   display1.drawBitmap(67, 5, Ozone_bmp,25,25,WHITE);
   display1.drawBitmap(98, 5, JSON_bmp,25,25,WHITE);
//second row
for(int i = 0; i <=3; i++){
     display1.drawRect(gui_x_cords[i], 34, 27, 27, WHITE);
   }
   display1.drawBitmap(5, 34, wifi_bmp,25,25,WHITE);
   display1.drawBitmap(36, 34, esp_now_bmp,25,25,WHITE);
   display1.drawBitmap(67, 34, RFID_bmp,25,25,WHITE);
   display1.drawBitmap(98, 34, settings_bmp,25,25,WHITE);  
//perimeter   
   display1.drawRect(0, 0, 128, 64, WHITE);
   display1.display();
}
//-------------------------------------------------------------------------------------------
//Draw Keyboard
//-------------------------------------------------------------------------------------------
void Draw_keyboard(){
  display1.clearDisplay();
  display1.drawRect(2, 2, 10, 10, WHITE);
  display1.drawRect(14, 2, 10, 10, WHITE);
  display1.drawRect(26, 2, 10, 10, WHITE);
  display1.drawRect(38, 2, 10, 10, WHITE);
  display1.drawRect(50, 2, 10, 10, WHITE);
  display1.drawRect(62, 2, 10, 10, WHITE);
  display1.drawRect(74, 2, 10, 10, WHITE);
  display1.drawRect(86, 2, 10, 10, WHITE);
  display1.drawRect(98, 2, 10, 10, WHITE);
  display1.drawRect(110, 2, 10, 10, WHITE);
  
  display1.setTextSize(1);
  display1.setTextColor(WHITE);
  display1.setCursor(4, 3);
  display1.println("a");
  display1.setCursor(16, 3);
  display1.println("b");
  display1.setCursor(28, 3);
  display1.println("c");
  display1.setCursor(40, 3);
  display1.println("d");
  display1.setCursor(52, 3);
  display1.println("e");
  display1.setCursor(64, 3);
  display1.println("f");
  display1.setCursor(76, 3);
  display1.println("g");
  display1.setCursor(88, 3);
  display1.println("h");
  display1.setCursor(100, 3);
  display1.println("i");
  display1.setCursor(112, 3);
  display1.println("j");

  display1.display();
  delay(100);
}
