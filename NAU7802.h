/*
 * Header for  reading the 24 bit ADC output from the NAU7802
 * load amplifier on a RaspberryPi using I2C and the wiringPi
 * library.
 *
 * Compile gcc with -lwiringPi argument.
 */

#ifndef NAU7802_H
#define NAU7802_H

/* include headers */
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdint.h>
#include <float.h>

/* define macros */
/* device I2C address */
#define NAU7802_ADDR 0x2A	/* load amp 12c address */

/* registers */
#define PU_CTRL 0x00		/* power up and control register */
#define CTRL1 0x01		/* control register 1 */
#define CTRL2 0x02		/* control register 2 */
#define OCAL1_B2 0x03		/* R0x03 Channel 1 offset calibration bits 23:16 */
#define OCAL1_B1 0x04		/* R0x04 Channel 1 offset calibration bits 15:8 */
#define OCAL1_B0 0x05		/* R0x05 Channel 1 offset calibration bits 7:0 */
#define GCAL1_B3 0x06		/* R0x06 Channel 1 gain calibration bits 31:24 */
#define GCAL1_B2 0x07		/* R0x07 Channel 1 gain calibration bits 23:16 */
#define GCAL1_B1 0x08		/* R0x08 Channel 1 gain calibration bits 15:8 */
#define GCAL1_B0 0x09		/* R0x09 Channel 1 gain calibration bits 7:0 */
#define OCAL2_B2 0x0A		/* R0x0A Channel 2 offset calibration bits 23:16 */
#define OCAL2_B1 0x0B		/* R0x0B Channel 2 offset calibration bits 15:8 */
#define OCAL2_B0 0x0C		/* R0x0C Channel 2 offset calibration bits 7:0 */
#define GCAL2_B3 0x0D		/* R0x0D Channel 2 gain calibration bits 31:24 */
#define GCAL2_B2 0x0E		/* R0x0E Channel 2 gain calibration bits 23:16 */
#define GCAL2_B1 0x0F		/* R0x0F Channel 2 gain calibration bits 15:8 */
#define GCAL2_B0 0x10		/* R0x10 Channel 2 gain calibration bits 7:0 */
#define I2C_CONTROL 0x11	/* R0x11 I2C Control register */
#define ADCO_B2 0x12		/* ADC conversion result bit 23:16 */
#define ADCO_B1 0x13 		/* ADC conversion result bit 15:8 */
#define ADCO_B0 0x14		/* ADC conversion result bit 7:0 */
#define OTP_B1 0x15		/* Read Back ADC or OTP 32:24 */
#define OTP_B0 0x16		/* OTP 23:16 */
#define OTP_BX 0x17		/* OTP 15:8 */
#define PGA 0x1B		/* PGA Registers */
#define POWER_CTRL 0x1C		/* Power Control Register */
#define DRC 0x1F		/* Device Revision Code */

/* bits for PU_CTRL register */
#define RR 0			/* register reset of PU_CTRL */
#define PUD 1			/* power up digital circuit of PU_CTRL */
#define PUA 2			/* power up analog circuit of PU_CTRL */
#define PUR 3			/* power up ready bit */
#define CS 4			/* cycle start of PU_CTRL */
#define CR 5			/* read only the cycle ready status */
#define OSCS 6			/* system clock source select for external clock */
#define AVDDS 7			/* AVDD source select for internal LDO */
#define RESET 0x01		/* write 1 to PU_CTRL to reset registers */
#define NORMAL_OP 0x02		/* write 1 to PU_CTRL enter normal operation */
#define ENABLE 0x16		/* write to PU_CTRL for CS and PUA*/
#define AVDD_INT 0x01		/* use internal LDO voltage regulator */
#define AVDD_PIN 0x00		/* use AVDD pin input */

/* bits for CTRL1 register */
#define DRDY_SEL 6		/* data ready pin function */
#define CRP 7			/* conversion ready pin polarity */
#define V4_5 0x00		/* VLDO 4.5 volts */
#define V4_2 0x01		/* VLDO 4.2 volts */
#define V3_9 0x02		/* VLDO 3.9 volts */
#define V3_6 0x03		/* VLDO 3.6 volts */
#define V3_3 0x04		/* VLDO 3.3 volts */
#define V3_0 0x05		/* VLDO 3.0 volts */
#define V2_7 0x06		/* VLDO 2.7 volts */
#define V2_4 0x07		/* VLDO 2.4 volts */

/* bits for CTRL2 register */
#define CALMOD_GCS 0x03		/* Gain Calibration System */
#define CALMOD_OCS 0x02		/* Offset Calibration System */
#define CALMOD_R 0x01		/* Reserved */
#define CALMOD_OCI 0x00		/* Offset Calibration Internal (default) */
#define CHS 7			/* analog input channel select */
#define CAL_ERR 3		/* Calibration result 1=error 0=good */
#define CALS 2			/* write 1 to this bit to cal, resets to 0 when finished */
#define CRS_10 0x00		/* Conversion rate select 10SPS */
#define CRS_20 0x01		/* CRS 20SPS */
#define CRS_40 0x02		/* CRS 40SPS */
#define CRS_80 0x03		/* CRS 80SPS */
#define CRS_320 0x07		/* CRS 320SPS */

/* bits for PGA register R0x1B */
#define RD_OTP_SEL 7		/* read R0x15 output select 0=ADC 1=OTP */
#define LDOMODE 6		/* 1=improved stab lower gain 0=improved accuracy higher gain */
#define PGA_BUF_EN 5		/* PGA Buffer 1=output buffer enable 0=output buffer disable */
#define PGA_BYPASS_EN 4		/* 1=PGA bypass enable 0=PGA bypass disable */
#define PGAINV 3		/* 1=invert PGA input phase 0=default */
#define PGACHPDIS 0		/* 1=chopper disabled 0=default */

/* bits for Power Control register 0x1C */
#define PGA_CAP_EN 7		/* enables PGA output bypass cap across Vin2p and Vin2N */

/* use for ADC to load conversion */
struct load_cal{
	double gain;		/* cal load multiplier */
	double zero; 		/* cal load zero, b value */
	uint8_t shift; 		/* bits to shift out */
	double offset;		/* this is for software offsets */
};

int NAU7802_init(int fd);

int NAU7802_enable(int fd);

int NAU7802_resetWait(int fd);

int NAU7802_resetReady(int fd);

int NAU7802_resetPowerDown(int fd);

int NAU7802_CR(int fd);

int NAU7802_setGain(int fd, int gain);

int NAU7802_readADCS(int fd, int8_t shift);

int NAU7802_readADC(int fd);

int NAU7802_setLDO(int fd, int voltage);

int NAU7802_AVDDSourceSelect(int fd, int source);

int NAU7802_readBit(int fd, int reg, uint8_t bit);

int NAU7802_writeBit(int fd, int reg, uint8_t bit, uint8_t val);

unsigned int NAU7802_readRegByBit(int fd, int reg);

int NAU7802_calibrate(int fd, uint8_t caltype);

int NAU7802_ch1ReadOffsetCal(int fd);

int NAU7802_ch1ReadGainCal(int fd);

int NAU7802_ch2ReadOffsetCal(int fd);

int NAU7802_ch2ReadGainCal(int fd);

int NAU7802_getChipRevId(int fd);

int NAU7802_enablePGABypassCap(int fd);

int NAU7802_disablePGABypassCap(int fd);

int NAU7802_togglePGAPhase(int fd);

int NAU7802_invertPGAPhase(int fd);

int NAU7802_normalPGAPhase(int fd);

int NAU7802_togglePGAChopper(int fd);

int NAU7802_disablePGAChopper(int fd);

int NAU7802_enablePGAChopper(int fd);

double NAU7802_getLinearLoad(int fd, struct load_cal *lc);

double NAU7802_getAvgLinearLoad(int fd, struct load_cal *lc);

int NAU7802_setSampleRate(int fd, uint8_t rate);

int NAU7802_getSampleRate(int fd);

double NAU7802_setLoadCalGain(struct load_cal *lc, double gain);

double NAU7802_getLoadCalGain(struct load_cal *lc);

double NAU7802_setLoadCalZero(struct load_cal *lc, double zero);

double NAU7802_getLoadCalZero(struct load_cal *lc);

double NAU7802_tareLoad(int fd, struct load_cal *lc);

double NAU7802_removeTareLoad(struct load_cal *lc);

double NAU7802_getOffsetLoad(struct load_cal *lc);

double NAU7802_setOffsetLoad(struct load_cal *lc, double offset);

int NAU7802_setShiftLoad(struct load_cal *lc, uint8_t);

int NAU7802_getShiftLoad(struct load_cal *lc);

void NAU7802_init_load_cal(struct load_cal *lc);

#endif
