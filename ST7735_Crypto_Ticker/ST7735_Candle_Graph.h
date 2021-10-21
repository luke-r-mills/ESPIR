/*
  ST7735_Candle_Graph.h - Menu for ST7735
  Copyright (c) 2021 Luke Mills.  All right reserved.
*/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <String.h>  

#define CANDLE_WIDTH        3

// ensure this library description is only included once
#ifndef ST7735_Candle_Graph_h
#define ST7735_Candle_Graph_h

// For candle graph element
typedef struct graph_candle {
  float opening;
  float closing;
  float high;
  float low;
} G_CANDLE;

class Candle_Graph
{
	public:
		Candle_Graph(Adafruit_ST7735*, int, int, int, int);
		void display();
		G_CANDLE* candles;
		int* count;
		int current_candles;
		int top;
		int bottom;
		int labels;
		void initialiseCandles();
		void nextTimePeriod(float);
		void addPrice(float);
		
	private:
		Adafruit_ST7735* tft;

};

#endif