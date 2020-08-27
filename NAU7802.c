/*
 * Library of functions to read the 24 bit ADC output from the NAU7802
 * load amplifier on the RPi using wiringPi.
 */

/* include headers */
#include "NAU7802.h"
#include <stdlib.h>
#include <stdint.h>

/*
 * Initialize NAU7802. Clear registers using RR bit,
 * set PUD bit to 1, wait 200 uS and check PWRUP bit
 * is 1.  This is shown as the PUR bit in data sheet
 * and is 1 if power up is normal.
 *
 * Return PUR bit;
 */
int
NAU7802_init(int fd){
	unsigned int pwrup;
	wiringPiI2CWriteReg8(fd, PU_CTRL, RESET);
	wiringPiI2CWriteReg8(fd, PU_CTRL, NORMAL_OP); 
	delayMicroseconds(200);
	return NAU7802_readBit(fd, PU_CTRL, PUR);
}

/*
 * Enable NAU7802.  Set bits PUD, PUA, CS in 
 * R0x00 (PU_CTRL).  This will start convesions
 * with the AVDDS and OSCS at default.
 *
 * Return 1 on success. Rregister value of lower 5 bits should
 * be 0x1E if successfull.
 */
int
NAU7802_enable(int fd){
	unsigned int reg;
	wiringPiI2CWriteReg8(fd, PU_CTRL, ENABLE);
	reg = wiringPiI2CReadReg8(fd, PU_CTRL);
	reg = reg >> 1;
	reg &= 0x0F;
	return (int)(reg == 0x0F);
}

/*
 * Reset device but do not enable.
 * Calls NAU7802_init()
 *
 * Returns PUR bit.
 */
int
NAU7802_resetWait(int fd){
	return NAU7802_init(fd);
}

/*
 * Reset device and start conversions.
 * Calls init and enable if init passes.
 *
 * Returns 1 on success else 0 if fail on
 * enable or -1 if fail on init.
 */
int
NAU7802_resetReady(int fd){
	if(NAU7802_init(fd))
		return NAU7802_enable(fd);
	return -1;
}

/*
 * Reset the device to default, power down.
 *
 * Return RR bit.
 */
int
NAU7802_resetPowerDown(int fd){
	return NAU7802_writeBit(fd, PU_CTRL, RR, 1);
}

/* 
 * Read Cycle Ready (CR) bit from R0x00(PU_CTRL) on
 * NAU7802. 
 * *** Did not use readBit to keep this read as fast
 * as possible ***
 *
 * Return CR bit.
 */
int
NAU7802_CR(int fd){
	unsigned int cr;
	cr = wiringPiI2CReadReg8(fd, PU_CTRL);
	cr = cr >> 5;
	cr &= 0x01;
	return (int)cr;
}

/*
 * Set the gain of the NAU7802.
 *
 * Returns the gain read back from the CTRL1
 * register or -1 if failure. Return value
 * will be 0-7 on success where 0-7 is 
 * 2^n value.
 */
int
NAU7802_setGain(int fd, int gain){
	uint32_t g, reg;
	if(gain == 128)
		g = 0x07;
	else if(gain == 64)
		g = 0x06;
	else if(gain == 32)
		g = 0x05;
	else if(gain == 16)
		g = 0x04;
	else if(gain == 8)
		g = 0x03;
	else if(gain == 4)
		g = 0x02;
	else if(gain == 2)
		g = 0x01;
	else if(gain == 1)
		g = 0x00;
	else
		return -1;
	reg = wiringPiI2CReadReg8(fd, CTRL1);
	reg &= 0xf8; /* zero lower 3 bits */
	g = g | reg;
	wiringPiI2CWriteReg8(fd, CTRL1, g);
	g = wiringPiI2CReadReg8(fd, CTRL1);
	g &= 0x07;
	return (int)g;
}

/*
 * Read the raw value from the ADC conversion
 * of the NAU7802.  This is a 24bit value from
 * 3 registers.  The value is locked in sfter the
 * read to R0x12 (ADCO_B0). Use shifting to fit
 * 24 bit value into a 32 bit type and maintain
 * the sign. The Shift value can be used to shift
 * out noisy bits. Example: For AVDD=3.3 and 
 * PGA gain=128 noise free bits are 16.54 so that
 * shifting out an additional 8 bits at these
 * setting will reduce noise.
 *
 * Return the ADC value.
 */
int
NAU7802_readADCS(int fd, int8_t shift){
	int32_t adc;
	int8_t r_shift=8;
	adc = wiringPiI2CReadReg8(fd, ADCO_B2) << 24;
	adc |= wiringPiI2CReadReg8(fd, ADCO_B1) << 16;
	adc |= wiringPiI2CReadReg8(fd, ADCO_B0) << 8;
	return (int)(adc >> (r_shift+shift));
}

/*
 * Read raw value from ADC. 
 *
 * Return int with shift of 0 bits.
 */
int
NAU7802_readADC(int fd){
	return (int)NAU7802_readADCS(fd, 0);
}

/*
 * Set the LDO voltage.
 * Before using this the LDO bit 7 of PU_CTRL
 * ADDVS should be set to 1.
 *
 * Return value 0-7 for vlotage level or 
 * -1 if vlotage arg was not valid or
 *  -2 if AVDDS is is not set to LDO.
 */
int
NAU7802_setLDO(int fd, int voltage){
	uint32_t v, reg;

	if(!(NAU7802_readBit(fd, PU_CTRL, 7)))
		return -2;

	if(!(	voltage == V4_5 ||
		voltage == V4_2 ||
		voltage == V3_9 ||
		voltage == V3_6 ||
		voltage == V3_3 ||
		voltage == V3_0 ||
		voltage == V2_7 ||
		voltage == V2_4))
		return -1;

	reg = wiringPiI2CReadReg8(fd, CTRL1);
	reg &= 0xC7; /* zero bits 5:3 */ 
	v = reg | (voltage << 3);
	wiringPiI2CWriteReg8(fd, CTRL1, v);
	v = wiringPiI2CReadReg8(fd, CTRL1);
	v = v >> 3;
	v &= 0x07;
	return (int)v;
}

/*
 * Set the source for AVDDS in PU_CTRL.
 * Select excitation to be from pin input
 * or internal LDO.
 *
 * Return value of bit 7 or -1 if arg
 * not valid.
 */
int
NAU7802_AVDDSourceSelect(int fd, int source){
	uint32_t s, reg;
	if(source == AVDD_INT)
		s = AVDD_INT;
	else if(source == AVDD_PIN)
		s = AVDD_PIN;
	else
		return -1;

	return NAU7802_writeBit(fd, PU_CTRL, AVDDS, s);
}

/*
 * Read a bit from a register.
 * Helper function.
 *
 * Return bit value or -1 if bit not valid.
 */
int
NAU7802_readBit(int fd, int reg, uint8_t bit){
	uint8_t b, r, mask;
	if(bit == 7)
		mask = 0x80;
	else if(bit == 6)
		mask = 0x40;
	else if(bit == 5)
		mask = 0x20;
	else if(bit == 4)
		mask = 0x10;
	else if(bit == 3)
		mask = 0x08;
	else if(bit == 2)
		mask = 0x04;
	else if(bit == 1)
		mask = 0x02;
	else if(bit == 0)
		mask = 0x01;
	else
		return -1;
	
	r = wiringPiI2CReadReg8(fd, reg);
	b = r & mask; 
	return (int)(b >> bit);
}

/*
 * Write a bit to register.
 * Helper function.
 *
 * Return bit value or -1 if bit not valid.
 */
int
NAU7802_writeBit(int fd, int reg, uint8_t bit, uint8_t val){
	uint8_t b, r, mask;
	if(bit == 7)
		mask = 0x7F;
	else if(bit == 6)
		mask = 0xBF;
	else if(bit == 5)
		mask = 0xDF;
	else if(bit == 4)
		mask = 0xEF;
	else if(bit == 3)
		mask = 0xF7;
	else if(bit == 2)
		mask = 0xFB;
	else if(bit == 1)
		mask = 0xFD;
	else if(bit == 0)
		mask = 0xFE;
	else
		return -1;

	r = wiringPiI2CReadReg8(fd, reg);
	b = val << bit; /* move val into bit pso */
	r = r & mask; /* set bit in reg to 0 */
	b |= r; /* put bit val into reg */
	wiringPiI2CWriteReg8(fd, reg, b);
	return NAU7802_readBit(fd, reg, bit);
}

/*
 * Read a register one bit at a time.
 * This is mostly for debgging.
 *
 * Return the registers bits as unsigned int.
 */
unsigned int
NAU7802_readRegByBit(int fd, int reg){
	uint8_t r=0;
	int bit;
	for(bit=7; bit>=0; bit--){
		r |= (uint8_t)NAU7802_readBit(fd, reg, bit);
		if(bit>0)
			r = r << 1;
	}	
	return (unsigned int)r;
}

/*
 * Calibrate the device.  Should be done after powerup,
 * long duration power down, PGA gain change, supply change, 
 * tsignificant temp change. sample rate change,
 * channel select change. caltype should be supplied
 * with a macro.
 * Blocks until calibrtion is finished.
 * Add a delay of 1 second before reading any calibration
 * registers.  This is needed to allow
 * time for values to be written to calibration registers
 * for correct readings if done immediately after.
 *
 * Return CAL_ERR status bit. 1=ERROR, 0=NO ERROR.
 * Return -1 on caltype error.
 */
int
NAU7802_calibrate(int fd, uint8_t caltype){
	uint8_t reg;
	if(!(	caltype == CALMOD_GCS ||
		caltype == CALMOD_OCS ||
		caltype == CALMOD_OCI))
		return -1;
	
	reg = wiringPiI2CReadReg8(fd, CTRL2);
	reg &= 0xF8; /* zero bits 2:0 */
	reg |= (caltype | 0x04);
	wiringPiI2CWriteReg8(fd, CTRL2, reg);
	while(!(NAU7802_readBit(fd, CTRL2, CALS)));
	return NAU7802_readBit(fd, CTRL2, CAL_ERR);
}

/*
 * Reads register correctly since delay was
 * implemented.
 * Make sure to delay 1 sec after calibrating
 * and before reading any calibration register.
 * Read channel 1 offset calibartion.
 * R0x03-R0x05
 *
 * Return offset calibration.
 */
int
NAU7802_ch1ReadOffsetCal(int fd){
	uint32_t offset;
	offset = wiringPiI2CReadReg8(fd, OCAL1_B2) << 24;
	printf("offset_B2 : %i\t", offset);
	offset |= wiringPiI2CReadReg8(fd, OCAL1_B1) << 16;
	printf("offset_B1 : %i\t", offset);
	offset |= wiringPiI2CReadReg8(fd, OCAL1_B0) << 8;
	printf("offset_B0 : %i\n", offset);
	return (int)(offset >> 8);
}

/*
 * Reads register correctly since delay was
 * implemented.
 * Make sure to delay 1 sec after calibrating
 * and before reading any calibration register.
 *
 * Read channel 1 gain calibrtion.
 * R0x06-R0x09
 *
 * Return gain calibration.
 */
int
NAU7802_ch1ReadGainCal(int fd){
	uint32_t gain;
	gain = wiringPiI2CReadReg8(fd, GCAL1_B3) ;
	printf("gain_B3 : %X\t", gain);
	gain = wiringPiI2CReadReg8(fd, GCAL1_B2) ;
	printf("gain_B2 : %X\t", gain);
	gain = wiringPiI2CReadReg8(fd, GCAL1_B1) ;
	printf("gain_B1 : %X\t", gain);
	gain = wiringPiI2CReadReg8(fd, GCAL1_B0);
	printf("gain_B0 : %X\n", gain);
	return (int)gain;
}

/*
 * UNTESTED: LOW PRIORITY
 * Make sure to delay 1 sec after calibrating
 * and before reading any calibration register.
 *
 * Read channel 2 offset calibartion.
 * R0x0A-R0x0C
 *
 * Return offset calibration.
 */
int
NAU7802_ch2ReadOffsetCal(int fd){
	int32_t offset;
	offset = wiringPiI2CReadReg8(fd, OCAL2_B2) << 24;
	offset |= wiringPiI2CReadReg8(fd, OCAL2_B1) << 16;
	offset |= wiringPiI2CReadReg8(fd, OCAL2_B0) << 8;
	return (int)(offset >> 8);
}

/*
 * UNTESTED: LOW PRIORITY
 * Make sure to delay 1 sec after calibrating
 * and before reading any calibration register.
 *
 * Read channel 2 gain calibrtion.
 * R0x0D-R0x10
 *
 * Return gain calibration.
 */
int
NAU7802_ch2ReadGainCal(int fd){
	int32_t gain;
	gain = wiringPiI2CReadReg8(fd, GCAL2_B3) << 24;
	gain |= wiringPiI2CReadReg8(fd, GCAL2_B2) << 16;
	gain |= wiringPiI2CReadReg8(fd, GCAL2_B1) << 8;
	gain |= wiringPiI2CReadReg8(fd, GCAL2_B0);
	return (int)gain;
}

/*
 * Read Chip Revision ID.
 *
 * Return Chip Revision ID.
 */
int
NAU7802_getChipRevId(int fd){
	uint8_t id;
	id = wiringPiI2CReadReg8(fd, DRC) & 0x0F;
	return (int)id;
}

/*
 * Enable PGA bypass cap across Vin2n and
 * Vin2p.
 * 
 * Return value of PGA_CAP_EN.
 */
int
NAU7802_enablePGABypassCap(int fd){
	return NAU7802_writeBit(fd, POWER_CTRL, PGA_CAP_EN, 1);
}

/*
 * Disable PGA bypass cap across Vin2n and
 * Vin2p.
 * 
 * Return value of PGA_CAP_EN.
 */
int
NAU7802_disablePGABypassCap(int fd){
	return NAU7802_writeBit(fd, POWER_CTRL, PGA_CAP_EN, 0);
}

/*
 * Toggle Invert of PGA input phase.
 *
 * Return PGAINV bit.
 */
int
NAU7802_togglePGAPhase(int fd){
	if(NAU7802_readBit(fd, PGA, PGAINV))
		return NAU7802_writeBit(fd, PGA, PGAINV, 0);
	return NAU7802_writeBit(fd, PGA, PGAINV, 1);
}

/*
 * Invert PGA input phase.
 *
 * Return PGAINV bit.
 */
int
NAU7802_invertPGAPhase(int fd){
	return NAU7802_writeBit(fd, PGA, PGAINV, 1);
}

/*
 * Normal PGA input phase.
 *
 * Return PGAINV bit.
 */
int
NAU7802_normalPGAPhase(int fd){
	return NAU7802_writeBit(fd, PGA, PGAINV, 0);
}

/*
 * Toggle Chopper of PGA input phase.
 *
 * Return PGACHPDIS bit.
 */
int
NAU7802_togglePGAChopper(int fd){
	if(NAU7802_readBit(fd, PGA, PGACHPDIS))
		return NAU7802_writeBit(fd, PGA, PGACHPDIS, 0);
	return NAU7802_writeBit(fd, PGA, PGACHPDIS, 1);
}

/*
 * Disable PGA Chopper.
 *
 * Return PGACHPDIS bit. 1=disabled.
 */
int
NAU7802_disablePGAChopper(int fd){
	return NAU7802_writeBit(fd, PGA, PGACHPDIS, 1);
}

/*
 * Enable PGA Chopper.
 *
 * Return PGACHPDIS bit. 0=enabled.
 */
int
NAU7802_enablePGAChopper(int fd){
	return NAU7802_writeBit(fd, PGA, PGACHPDIS, 0);
}

/*
 * Convert ADC value to a load value.
 * This is a linear function of type
 * y = mx + b where y is the returned
 * load and m is the gain, x is the ADC
 * reading, and b is the zero.
 * Offset is used for a software tare
 * of the load and is not to be part of
 * the load calibration.
 *
 * Returns load value.
 */
double
NAU7802_getLinearLoad(int fd, struct load_cal *lc){
	return NAU7802_readADCS(fd, lc->shift) 
		* lc->gain
		+ lc->zero
	       	- lc->offset;
}

/*
 * Get an average of the load readout.  This is based
 * on the sample rate. Number of samples to average
 * is rate / 10.  This means no average will be done
 * for rate of 10.  This keeps the minimum rate for
 * data at 10Hz.
 *
 * Return the average load.
 */
double
NAU7802_getAvgLinearLoad(int fd, struct load_cal *lc){
	long double avg=0.0;
	int i, rate;
	rate = NAU7802_getSampleRate(fd);
	for(i=0; i<(rate/10); ++i){
		while(!(NAU7802_CR(fd)));
		avg += NAU7802_getLinearLoad(fd, lc);
	}
	return  (double)(avg / rate);
}

/*
 * Set the sample rate.  Use the Macros to
 * change the sample rate.
 *
 * Return the sample rate or -1 on invalid
 * rate.
 */
int
NAU7802_setSampleRate(int fd, uint8_t rate){
	uint8_t reg, r,  mask=0x8F;
	if(!(	rate == CRS_10 ||
		rate == CRS_20 ||
		rate == CRS_40 ||
		rate == CRS_80 ||
		rate == CRS_320))
		return -1;
	reg = wiringPiI2CReadReg8(fd, CTRL2) & mask;
	r = rate << 4;
	r |= reg;
	wiringPiI2CWriteReg8(fd, CTRL2, r);
	reg = wiringPiI2CReadReg8(fd, CTRL2) >> 4;
	return (int)(reg & 0x07);
}

/*
 * Get the current sample rate.
 *
 * Returns sample rate in SPS or
 * -1 if read failed.
 */
int
NAU7802_getSampleRate(int fd){
	uint8_t rate;
	rate = wiringPiI2CReadReg8(fd, CTRL2) >> 4;
	rate &= 0x07;
	if(rate == CRS_10)
		return 10;
	else if(rate == CRS_20)
		return 20;
	else if(rate == CRS_40)
		return 40;
	else if(rate == CRS_80)
		return 80;
	else if(rate == CRS_320)
		return 320;
	else 
		return -1; 	/* error occurred */
}

/*
 * Set the gain for load calibration.
 *
 * Returns the previous gain value.
 */
double
NAU7802_setLoadCalGain(struct load_cal *lc, double gain){
	double old_gain;
	old_gain = lc->gain;
	lc->gain = gain;
	return old_gain;
}

/*
 * Get the load cal gain.
 *
 * Returns the current load cal gain.
 */
double
NAU7802_getLoadCalGain(struct load_cal *lc){
	return lc->gain;
}

/*
 * Set the zero for load calibration.
 *
 * Returns the prvious zero.
 */
double
NAU7802_setLoadCalZero(struct load_cal *lc, double zero){
	double old_zero;
	old_zero = lc->zero;
	lc->zero = zero;
	return old_zero;
}

/*
 * Get the load cal zero.
 *
 * Returns the current load cal zero.
 */
double
NAU7802_getLoadCalZero(struct load_cal *lc){
	return lc->zero;
}

/*
 * Tare the load.  Use this to remove any offset
 * that is not part of calibration.  This will take
 * an average of readings over 1 second and tare
 * that value by placing it into the load_cal->offset.
 *
 * Returns the difference ofthe old offset and
 * new offset. Returns the macro constant DBL_MAX
 * int float.h on error reading sample rate.
 */
double
NAU7802_tareLoad(int fd, struct load_cal *lc){
	double avg=0, old_offset;
	int rate, i;
	rate = NAU7802_getSampleRate(fd);
	if(rate == -1)
		return DBL_MAX;
	for(i=rate; i>0; --i)
		avg += NAU7802_getLinearLoad(fd, lc);
	avg = avg / rate;
	old_offset = lc->offset;
	lc->offset = avg;
	return old_offset - lc->offset;
}

/*
 * Remove the tare (offset) from the load_cal.
 *
 * Returns the previous offset value.
 */
double
NAU7802_removeTareLoad(struct load_cal *lc){
	double old_offset;
	old_offset = lc->offset;
	lc->offset = 0.0;
	return old_offset;
}

/*
 * Get the current offset(tare) value.
 *
 * Returns the current offset.
 */
double
NAU7802_getOffsetLoad(struct load_cal *lc){
	return lc->offset;
}

/*
 * Set the offset(tare) of load_cal.
 *
 * Returns the previous offset.
 */
double
NAU7802_setOffsetLoad(struct load_cal *lc, double offset){
	double old_offset;
	old_offset = lc->offset;
	lc->offset = offset;
	return old_offset;
}

/*
 * Set the load cal shift value.
 * This can get rid of noisy bits.
 *
 * Return previous shift value.
 */
int
NAU7802_setShiftLoad(struct load_cal *lc, uint8_t shift){
	uint8_t old_shift;
	old_shift = lc->shift;
	lc->shift = shift;
	return (int)old_shift;
}

/*
 * Get the current shift value.
 *
 * Return the current shift value.
 */
int
NAU7802_getShiftLoad(struct load_cal *lc){
	return lc->shift;
}

/*
 * Set default values and initialize a 
 * struct load_cal.
 */
void
NAU7802_init_load_cal(struct load_cal *lc){
	lc->gain = 1.0;
	lc->zero = 0.0;
	lc->shift = 0;
	lc->offset = 0.0;
}
