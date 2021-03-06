 /*!
 \file CAD9833.h
 \author Mike Hankey
 \date Tue 5 28 2019

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

***********************************************************************/

#ifndef CAD9833_H_
#define CAD9833_H_

#define MODEBIT		0x0002
#define DIV2		0x0008
#define OPBITEN		0x0020
#define SLEEP12		0x0040
#define SLEEP1		0x0080
#define RESET		0x0100
#define PSELECT		0x0400
#define FSELECT		0x0800
#define HLB			0x1000
#define B28			0x2000
#define DB14		0x4000
#define DB15		0x8000

#define FREQ0_WRITE_REG		0x4000
#define FREQ1_WRITE_REG		0x8000
#define PHASE0_WRITE_REG	0x2000
#define PHASE_WRITE_CMD		0xc000

#define DEVICE_FREQ		25000000UL
#define MAX_FREQ		12.5e6
#define BITS_PER_DEG	11.3777777777778	// 4096 / 360

#define PIN_MOSI	PB3
#define PIN_MISO	PB4
#define PIN_CLK		PB5

#define REG0 0
#define REG1 1

typedef enum 
{ 
	SINE_WAVE = 0x0000, 
	SAWTOOTH_WAVE = 0x0002,
	SQUARE_WAVE = 0x0028, 
	HALF_SQUARE_WAVE = 0x0020 
} WaveformType;

typedef struct 
{
	WaveformType	m_mode;
	uint16_t		m_controlReg;
	float			m_frequency;
	float			m_phase;
} DeviceInfo;

class CAD9833
{
	private:
		
		volatile uint8_t* m_ssport;
		uint8_t		m_sspin;

		DeviceInfo	m_deviceInfo[2];
		uint8_t		m_activeReg;
		bool		m_outputEnabled;
		
	public:
		CAD9833(volatile uint8_t*, uint8_t);

		void Init();
		void Reset();
		void Power(bool);

		void SetFrequency(uint8_t reg, float freq);
		void SetPhase(uint8_t reg, float ph);
		void SetMode(uint8_t reg, WaveformType mode);
		
		// Methods to act on active register
		void SetFrequency(float freq);
		void SetPhase(float ph);
		void SetMode(WaveformType mode);

		uint8_t GetActiveRegister();
		float GetFrequency(uint8_t);
		float GetPhase(uint8_t);
		WaveformType GetMode(uint8_t);
		float GetFrequency();
		float GetPhase();
		WaveformType GetMode();
		
		void ConfigureRegister(uint8_t reg,
							WaveformType waveType, 
							float frequencyInHz,
							float phaseInDeg = 0.0 );
		void SetActiveRegister(uint8_t reg);

	private:	
		void WriteReg(uint16_t cmd);
		uint8_t TransferData(uint8_t);	
};

#endif /* CAD9833_H_ */