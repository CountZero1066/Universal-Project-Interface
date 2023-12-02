
#ifndef vertical_text_scroll_h
#define vertical_text_scroll_h

String text_buffer[150];
int y[] = { 4, 12, 20, 28, 36, 44, 52};
//-------------------------------------------------------------------------------------------
void Add_text_to_scroll (String new_text_block, boolean new_block){
  int index = 0;
  if(new_block == true){
    text_buffer[0] = "1";
    text_buffer[1] = new_text_block;
  }
  else if(new_block == false){
    index = text_buffer[0].toInt();
    if (index != 0 && index <= 149){
        text_buffer[index + 1] = new_text_block;
        text_buffer[0] = String(index + 1);
    }  
  }
}
//-------------------------------------------------------------------------------------------
void write_text_buffer_to_display(){
  int index = text_buffer[0].toInt();
  if(index > 0 ){
    display1.clearDisplay();
    if (index < 8){
      for(int i = 0; i <= index; i++){
      display1.setTextColor(WHITE);
      display1.setTextSize(1);
      display1.setCursor(4, y[i]);
      display1.println(">" + text_buffer[i+1]);
      display1.display();
      delay(100);
    }
    }
    else if (index >= 8){
      for(int i = 7; i <= index -1; i++){
        display1.clearDisplay();
        int cursor_pos = 0;
        for(int n = 6; n >= 0; n--){
          display1.setTextColor(WHITE);
          display1.setTextSize(1);
          display1.setCursor(4, y[cursor_pos]);
          display1.println(">" + text_buffer[i - n]);
          //Serial.println(text_buffer[i - n]);
          cursor_pos ++;
        }
        display1.display();
        delay(100); 
      }
    }
  }
}

#endif
