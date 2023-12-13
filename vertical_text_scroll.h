
//Code for displaying and navigating large bodies of text

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
//-------------------------------------------------------------------------------------------
void display_from_buff_index(int buff_index){
  int index = text_buffer[0].toInt();
  if(index ==0 || index <7){ //are we printing a text buffer that can be accommadated on a single screen?
    for(int i = 1; i <= index; i++){
              display1.setTextColor(WHITE);
              display1.setTextSize(1);
              display1.setCursor(4, y[i]);
              display1.println(">" + text_buffer[i]);
            }
  }
  else{
    if(buff_index <= index +3){ //are we near to the min limit of the text buffer?
      int n = 0;
      for(int i = buff_index -3; i <= buff_index + 3;i++){
              display1.setTextColor(WHITE);
              display1.setTextSize(1);
              display1.setCursor(4, y[n]);
              display1.println(">" + text_buffer[i]); 
              n++;
            }
     }
      
            else if (buff_index <= 3){ //are we close to the upper limit of the text buffer?
              for(int i = 1; i <= index; i++){
              display1.setTextColor(WHITE);
              display1.setTextSize(1);
              display1.setCursor(4, y[i]);
              display1.println(">" + text_buffer[i]);
            }
    }
  }
}
//-------------------------------------------------------------------------------------------
//draws a scoll bar that is representative of the volume of data in the text buffer and the users progress scrolling through it
void  Draw_scroll_progress_bar(int buffer_index){
  int index = text_buffer[0].toInt();
  display1.fillRect(120,0,8,64, BLACK);
  display1.drawRect(120,0,8,64, WHITE);
  int x = ((float)buffer_index/(float)index) * 100;
  int y = ((float)60 / (float)100) * (float)x; //determine in px where the relative location of the index is
  float w = ((float)7/(float)index) * 100; 
  int s = ((float)60 / (float)100) * (float)w; //the proportionate size of the scrolling sidebar 
  display1.fillRect(122,y,4,s, WHITE);
  display1.display();       
}
//-------------------------------------------------------------------------------------------
//allows scrolling up and down data stored in the text buffer array 
int scroll_buffer(int button, int buff_index){
  int index = text_buffer[0].toInt();
  if(index ==0 || index <8){ //ie: more data than can be accommodated on the display at once
  }
  else{
    switch(button){
      case 1: //up
          if(buff_index > 7){
            display1.clearDisplay();
            buff_index --;
            int n = 0;
            for(int i = buff_index -4; i <= buff_index + 2;i++){
              display1.setTextColor(WHITE);
              display1.setTextSize(1);
              display1.setCursor(4, y[n]);
              display1.println(">" + text_buffer[i]); 
              n++;
              
            }
          }
          
      break;
      case 3: //down
           if(buff_index < index - 3 && buff_index > 3){
            display1.clearDisplay();
            buff_index ++;
            int n = 0;
            for(int i = buff_index -2; i <= buff_index + 4;i++){
              display1.setTextColor(WHITE);
              display1.setTextSize(1);
              display1.setCursor(4, y[n]);
              display1.println(">" + text_buffer[i]); 
              n++;
            }
          }
      break;
    }
    display1.display();
  }
  Draw_scroll_progress_bar(buff_index -4);
  return(buff_index);
  
}

#endif
