/*
  ST7735_Menu.h - Menu for ST7735
  Copyright (c) 2021 Luke Mills.  All right reserved.
*/

#include "ST7735_Portfolio_Editor.h"
#include "HardwareSerial.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Colours.h>

// Constructor for Portfolio Editor
ST7735_Portfolio_Editor::ST7735_Portfolio_Editor(Adafruit_ST7735* display, COIN* coin_arr) {
	tft = display;
	
	active = 0;
	
	coins = coin_arr;
	
	selector = (Price_Selector*) malloc(sizeof(Price_Selector));
    *selector = Price_Selector(tft);
	
	selected_portfolio_index = 0;
	changing_amount = 0;
	
	// Initialise portfolio arrays
	selected_portfolio_indexes = (int*) malloc(sizeof(int) * 9);

	for (int i = 0; i < 9; i++)
		selected_portfolio_indexes[i] = -1;
}

// Sets the Portfolio Editor to be in a selected state
void ST7735_Portfolio_Editor::setActive(){
	active = 1;
}

// Displays the Portfolio Editor
void ST7735_Portfolio_Editor::display(){
  tft->fillRect(0, 0, tft->width(), tft->height() - 12, BLACK);
  tft -> setTextColor(WHITE);
  
  int i = 0;
  while(i < COIN_COUNT){
    tft -> fillRect(map(i % 4, 0, 3, 2, tft -> width() - 43), map(floor(i / 4), 0, 5, 10, 
      tft -> height() / 2), 4, 7, coins[i].portfolio_colour);
    tft -> setCursor(map(i % 4, 0, 3, 9, tft -> width() - 36), map(floor(i / 4), 0, 5, 10, 
      tft -> height() / 2));
    tft -> print(coins[i].coin_code);
    i++;
  }

  tft -> fillRect(map(selected_portfolio_index % 4, 0, 3, 9, tft -> width() - 36) - 1, 
    map(floor(selected_portfolio_index / 4), 0, 5, 10, tft -> height() / 2) - 1, 31, 9, GRAY);
  tft -> setCursor(map(selected_portfolio_index % 4, 0, 3, 9, tft -> width() - 36), 
    map(floor(selected_portfolio_index / 4), 0, 5, 10, tft -> height() / 2));
  tft -> setTextColor(BLACK);
  tft -> print(coins[selected_portfolio_index].coin_code);
}

// Interacts with the portfolio editor using the passed decoded IR data
int ST7735_Portfolio_Editor::interact(uint32_t* ir_data){
    if (*ir_data == 0xF20DFF00){
      active = 0;
      return 0;
    }
    
    if (changing_amount == 0){
      tft -> setTextSize(1);
      tft -> fillRect(map(selected_portfolio_index % 4, 0, 3, 9, tft -> width() - 36) - 1, 
        map(floor(selected_portfolio_index / 4), 0, 5, 10, tft -> height() / 2) - 1, 31, 9, BLACK);
      tft -> setTextColor(WHITE);
      tft -> setCursor(map(selected_portfolio_index % 4, 0, 3, 9, tft -> width() - 36), 
        map(floor(selected_portfolio_index / 4), 0, 5, 10, tft -> height() / 2));
      tft -> print(coins[selected_portfolio_index].coin_code);
      
      // left
      if (*ir_data == 0xF708FF00){
        selected_portfolio_index = selected_portfolio_index == 0 ? COIN_COUNT-1 
          : selected_portfolio_index - 1;
      }
  
      //right
      if (*ir_data == 0xA55AFF00){
        selected_portfolio_index = selected_portfolio_index == COIN_COUNT - 1 ? 0 
          : selected_portfolio_index + 1;
      }
  
      //ok
      if (*ir_data == 0xE31CFF00){
        selector -> setValue(coins[selected_portfolio_index].amount);
        selector -> display();
        changing_amount = 1;
      }

      tft -> setTextSize(1);
      tft -> fillRect(map(selected_portfolio_index % 4, 0, 3, 9, tft -> width() - 36) - 1, 
        map(floor(selected_portfolio_index / 4), 0, 5, 10, tft -> height() / 2) - 1, 31, 9, GRAY);
      tft -> setCursor(map(selected_portfolio_index % 4, 0, 3, 9, tft -> width() - 36), 
        map(floor(selected_portfolio_index / 4), 0, 5, 10, tft -> height() / 2));
      tft -> setTextColor(BLACK);
      tft -> print(coins[selected_portfolio_index].coin_code);
    } else {
      // left
      if (*ir_data == 0xF708FF00)
        selector ->  increaseValueToAdd();
  
      //right
      if (*ir_data == 0xA55AFF00)
        selector -> decreaseValueToAdd();

      //up
      if (*ir_data == 0xE718FF00)
        selector -> addValue();

      //down
      if (*ir_data == 0xAD52FF00)
        selector -> subValue();

      //ok
      if (*ir_data == 0xE31CFF00){
        changing_amount = 0;
        coins[selected_portfolio_index].amount = selector -> getValue();
        selector -> clear();
      }
    }
    delay(100);
	return 1;
}



// Constructor for Amount Selector
Price_Selector::Price_Selector(Adafruit_ST7735* display) {
	tft = display;
	
	selected_index = 0;
	value = 0;
	value_to_add = 1;
}

// Adds the currently selected value
void Price_Selector::addValue() {
	value = value + value_to_add < 999999999999 ? 
		value + value_to_add : value;
	redrawValue();
}

// Subtracts the currently selected value
void Price_Selector::subValue() {
	value = value - value_to_add >= 0 ? value - value_to_add : 0;
	redrawValue();
}

// Display the selector on the screen
void Price_Selector::display() {
	redrawValue();
	redrawValueChange();
}

// Sets the value to the passed value
void Price_Selector::setValue(double val) {
	value = val;
}

// Returns the current value
double Price_Selector::getValue() {
	return value;
}

// Multiplies the selected value to add by 10
void Price_Selector::increaseValueToAdd() {
	value_to_add = value_to_add * 10 < 10000000000 ? 
		value_to_add * 10 : value_to_add;
	redrawValueChange();
}

// Divides the selected value to add by 10
void Price_Selector::decreaseValueToAdd() {
	value_to_add = value_to_add / 10 >= 0.000001 ? 
		value_to_add / 10 : value_to_add;
	redrawValueChange();
}

// Clears the area occupied by the selector
void Price_Selector::clear() {
	tft->fillRect(0, 75, tft->width(), 40, BLACK);
	tft->setTextSize(2);
}

// Redraws the value selected by the user
void Price_Selector::redrawValue() {
	tft->fillRect(0, 75, tft->width(), 15, BLACK);
	tft->setTextColor(WHITE);
	tft->setCursor(2, 75);
	tft->setTextSize(2);
	
	if (value >= 1000000){
		tft->print(value, 0);
	} else {
		tft->print(value, 6);
	}
}

// Redraws the value change selected by the user
void Price_Selector::redrawValueChange() {
	tft->setTextSize(1);
	tft->fillRect(0, 100, tft->width(), 8, BLACK);
	tft->setCursor(2, 100);
	tft->setTextColor(GREEN);
	tft->print('+');
	tft->setTextColor(RED);
	tft->print('-');
	tft->setTextColor(WHITE);
	tft->print(value_to_add, 6);
}