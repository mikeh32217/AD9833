 /*!
	 \file AD9833_Harness.cpp
	 \author Mike Hankey
	 \date 3/3/2021

	 \copyright
	 Copyright (c) 2015 Mike Hankey <mikeh32217@yahoo.com>

	 Permission is hereby granted, free of charge, to any person
	 obtaining a copy of this software and associated documentation
	 files (the "Software"), to deal in the Software without
	 restriction, including without limitation the rights to use, copy,
	 modify, merge, publish, distribute, sub license, and/or sell copies
	 of the Software, and to permit persons to whom the Software is
	 furnished to do so, subject to the following conditions:

	 The above copyright notice and this permission notice shall be
	 included in all copies or substantial portions of the Software.

	 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
	 DEALINGS IN THE SOFTWARE.

	 \note Dropped from the MultiTool project because I couldn't get it to work!
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "CAD9833.h"

#define PIN_SS		PB2

// Instantiate the signal generator providing the
//	Port and pin for the Chip Select.
CAD9833 gen(&PORTB, PIN_SS);

int main(void)
{
	// Configure the channels
	//	Reg(0 || 1) - Register on the chip
	//	WaveType (SINE_WAVE, SAWTOOTH_WAVE, HALF_SQUARE_WAVE, SQUARE_WAVE)
	//	Frequency (1-2^14)
	//	Phase (0-360)
	//
	//	Note: When setting the frequency for HALF_SQUARE_WAVE remember
	//		that it will be half of whatever frequency you configure.
	gen.ConfigureRegister(REG0, SQUARE_WAVE, 32768, 0.0);
	gen.ConfigureRegister(REG1, SINE_WAVE, 8000, 0.0);
	
	gen.Reset();

	gen.SetActiveRegister(REG0);
	
	// Switch from one channel to the other every second.
    while (1) 
    {	
		//gen.SetActiveRegister(REGS::Reg0);
		//_delay_ms(2000);
		//
		//// Power down device
		//gen.Power(false);
		//_delay_ms(2000);
		//
		//// Power up device, sets the output to whatever value the
		////	current register is configured.  In this case Reg0!
		//gen.Power(true);
		//_delay_ms(2000);
		//
		//gen.SetActiveRegister(REGS::Reg1);
		//_delay_ms(2000);
    }
}

