 /*!
	 \file CAD9833.cpp
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

	 \note Dropped from the MultiTool project because I couldn't get it to work!
 */
#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "CAD9833.h"

/**
 * \brief Constructor
 * 
 * \param ssport Port used for Chip Select 
 * \param sspin  Pin used for Chip Select 
 * 
 * \return 
 */
CAD9833::CAD9833(volatile uint8_t* ssport, uint8_t sspin)
{
	m_sspin = sspin;
	m_ssport = ssport;
	
	// Set defaults
	for (uint8_t i = 0; i < 2; i++)
	{
		m_deviceInfo[i].m_frequency = 1000.0;
		m_deviceInfo[i].m_mode = SINE_WAVE;
		m_deviceInfo[i].m_phase = 0.0;
		m_deviceInfo[i].m_controlReg = 0;
	}
	m_outputEnabled = false;
	m_activeReg = REG0;
	
	Init();
}

/**
 * \brief Initialize the device
 * 
 * \return void
 *
 * \note From the datasheet and it's wrong;
 *		 SCK idles high between write operations (CPOL = 0)
 *		 data is valid on the SCK falling edge (CPHA = 1)
 *	If the clock idles high, which in this case it does then
 *	    CPOL = 1 and CPHA = 0.
 */
void CAD9833::Init()
{
	// Set as master
	SPCR |= (1 << MSTR);
	// Set CPOL Clock idles high.  This is what makes it work!
	SPCR |= (1 << CPOL);
	// Enable SPI
	SPCR |= (1 << SPE);
	
	// Set pins as output
	*(m_ssport - 1) |= (1 << m_sspin) | (1 << PIN_MOSI) | (1 << PIN_CLK);
	// Set SS idle high
	*m_ssport |= (1 << m_sspin);
}

/**
 * \brief Resets the device
 * 
 * \param state
 * 
 * \return void
 */
void CAD9833::Reset()
{
	WriteReg(0x0100);
	
	for (uint8_t i = 0; i < 2; i++)
	{
		SetFrequency(i, m_deviceInfo[i].m_frequency);
		SetPhase(i, m_deviceInfo[i].m_phase);
		SetMode(i, m_deviceInfo[i].m_mode);	
	}
	
	WriteReg(0x0000);
}

void CAD9833::Power(bool state)
{
	m_outputEnabled = state;
	
	uint16_t r = SLEEP1 | SLEEP12;
	if (state)
		WriteReg(m_deviceInfo[m_activeReg].m_controlReg);
	else
		WriteReg(r);	
}

/**
 * \brief Configure the channel
 * 
 * \param reg	Either Reg0 or Reg1
 * \param waveType Sine, Sawtooth or Square
 * \param frequencyInHz Registers Frequency
 * \param phaseInDeg Registers Phase
 * 
 * \return void
 *
 * \note The AD9833 device has two registers that may
 *		be independently configured; Reg0 and Reg1.
 */
void CAD9833::ConfigureRegister(uint8_t reg, WaveformType waveType,
		float frequencyInHz, float phaseInDeg) 
{
	m_deviceInfo[reg].m_frequency = frequencyInHz;
	m_deviceInfo[reg].m_phase = phaseInDeg;
	m_deviceInfo[reg].m_mode = waveType;
	
	SetFrequency (reg, m_deviceInfo[reg].m_frequency);
	SetPhase(reg, m_deviceInfo[reg].m_phase);
	
	SetMode(reg, waveType);
}

/**
 * \brief Get the active register/channel
 * 
 * \return REGS Either Reg0 or Reg1
 *
 * \note The DeviceInfo structure 
 *
	 typedef struct
	 {
		 WaveformType	m_mode;
		 uint16_t		m_controlReg;
		 float			m_frequency;
		 float			m_phase;
	 } DeviceInfo;
 */
uint8_t CAD9833::GetActiveRegister()
{
	return m_activeReg;
}

/**
 * \brief Switches to the register specified 
 * 
 * \param reg The register to switch too
 * 
 * \return void
 */
void CAD9833::SetActiveRegister(uint8_t reg)
{
	m_activeReg = reg;

	if (reg == REG1)
		m_deviceInfo[reg].m_controlReg |= (FSELECT | PSELECT);
	else
		m_deviceInfo[reg].m_controlReg &= ~(FSELECT | PSELECT);
	
	WriteReg(m_deviceInfo[reg].m_controlReg);
}

/**
 * \brief Sets frequency on specified register
 * 
 * \param reg Register 
 * \param freq Frequency
 * 
 * \return void
 */
void CAD9833::SetFrequency(uint8_t reg, float freq)
{
	if (freq < 0)
		freq = 0;

	if (freq > MAX_FREQ)
		freq = MAX_FREQ;

	m_deviceInfo[reg].m_frequency = freq;

	int32_t freqw = (freq * pow(2, 28)) / (float) DEVICE_FREQ;
	int16_t msb = (int16_t)((freqw & 0xfffc000) >> 14);
	int16_t lsb = (int16_t)(freqw & 0x3fff);

	uint16_t res = (reg == REG0) ? FREQ0_WRITE_REG : FREQ1_WRITE_REG;
	lsb |= res;
	msb |= res;

	WriteReg(B28 | m_deviceInfo[reg].m_controlReg);

	WriteReg(lsb);
	WriteReg(msb);
}

/**
 * \brief Gets Frequency
 * 
 * \param reg New frequency
 * 
 * \return Frequency
 */
float CAD9833::GetFrequency(uint8_t reg) { return m_deviceInfo[reg].m_frequency; }
float CAD9833::GetFrequency() { return m_deviceInfo[m_activeReg].m_frequency; }
	
/**
 * \brief Sets Frequency on active register
 * 
 * \param freq New frequency
 * 
 * \return void
 */
void CAD9833::SetFrequency(float freq)
{
	SetFrequency(m_activeReg, freq);
}

/**
 * \brief Set phase on active register
 *
 * \param reg The register to set
 * \param ph New phase
 * 
 * \return void
 */
void CAD9833::SetPhase(uint8_t reg, float ph)
{
	ph = fmod(ph, 360);
	if (ph < 0)
		ph += 360;

	uint16_t pv = (uint16_t)(BITS_PER_DEG * ph) & 0x0fff;
	pv |= PHASE_WRITE_CMD;

	m_deviceInfo[reg].m_phase = ph;
	if (reg == REG1)
		pv |= B28;

	WriteReg(pv);
}

/**
 * \brief Set phase on active register
 * 
 * \param ph New phase
 * 
 * \return void
 */
void CAD9833::SetPhase(float ph)
{
	SetPhase(m_activeReg, ph);
}

/**
 * \brief Get phase
 * 
 * \param ph New phase
 * 
 * \return Phase
 */
float CAD9833::GetPhase(uint8_t reg) { return m_deviceInfo[reg].m_phase; }
float CAD9833::GetPhase() { return m_deviceInfo[m_activeReg].m_phase; }
	
/**
 * \brief Set the Wave mode
 * 
 * \param reg Register to set
 * \param mode Mode; (Sine, Sawtooth, Square or Half Square)
 * 
 * \return void
 */
void CAD9833::SetMode(uint8_t reg, WaveformType mode)
{
	m_deviceInfo[reg].m_mode = mode;
	
	m_deviceInfo[reg].m_controlReg &= ~SQUARE_WAVE;
	m_deviceInfo[reg].m_controlReg |= mode;
	
	// Power down DAC when in one of the Square modes
	if (mode == SQUARE_WAVE || mode == HALF_SQUARE_WAVE)
		m_deviceInfo[reg].m_controlReg |= SLEEP12;		
}

/**
 * \brief Set the Wave mode for active register
 * 
 * \param mode Mode; (Sine, Sawtooth, Square or Half Square)
 * 
 * \return void
 */
void CAD9833::SetMode(WaveformType mode)
{
	SetMode(m_activeReg, mode);
}

/**
 * \brief Get the Wave mode
 * 
 * \param mode Mode; (Sine, Sawtooth, Square or Half Square)
 * 
 * \return WaveModeType mode
 */
WaveformType CAD9833::GetMode(uint8_t reg) { return m_deviceInfo[reg].m_mode; }
WaveformType CAD9833::GetMode() { return m_deviceInfo[m_activeReg].m_mode; }

/**
 * \brief Write data to the device
 * 
 * \param cmd Data to write
 * 
 * \return void
 */
void CAD9833::WriteReg(uint16_t cmd)
{
	*m_ssport &= ~(1 << m_sspin);

	TransferData((cmd >> 8) & 0x0ff);
	TransferData(cmd & 0x00ff);

	*m_ssport |= (1 << m_sspin);
}

/**
 * \brief Low level write to SPI channel
 * 
 * \param data Byte to write
 * 
 * \return uint8_t Data returned from SPI channel
 */
uint8_t CAD9833::TransferData(uint8_t data)
{
	SPDR = data;
	
	//Wait for the transfer to complete
	while(!(SPSR & (1 << SPIF))){}
	
	return SPDR;
}