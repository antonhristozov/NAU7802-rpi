/*
 * Test reading the 24 bit ADC output from the NAU7802
 * load amplifier.
 */

/* include headers */
#include "NAU7802.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

void
test0(int fd){
	if(NAU7802_resetPowerDown(fd))
		printf("Powered Down\n");
}

void
test1(int fd){
	int z, i=0, shift=4;
	do{
	z = NAU7802_calibrate(fd, CALMOD_GCS);
	printf("CAL_ERR : %i\n", z);
	++i;
	delay(200);
	}while(z && i<10);
	delay(1000);
	NAU7802_readADC(fd);
	printf("gain : %i\n", NAU7802_ch1ReadGainCal(fd));

	printf("REG%X : %u\t", i, NAU7802_readRegByBit(fd, 0));
	for(i=0x06; i<0x0A; i++)
		printf("REG%X : %u\t", i, NAU7802_readRegByBit(fd, i));
	printf("\n");
	/* read value from adc */
	printf("\n...Test...1\n");
	delay(5000);
	for(;;){
		while(!NAU7802_CR(fd));
		printf("ADCS%i : %i\t\tADC : %i\n",
			shift,
			NAU7802_readADCS(fd, shift),
			NAU7802_readADC(fd));
	}
}

void
test2(int fd){
	int z, i=0;
	printf("\n..Test...2\n");
	z = NAU7802_enablePGABypassCap(fd);
	printf("Bypass Cap : %i\n", z);
	do{
	z = NAU7802_calibrate(fd, CALMOD_GCS);
	printf("CAL_ERR : %i\n", z);
	++i;
	delay(1000);
	}while(z && i<10);
	delay(5000);
	for(;;){
		while(!NAU7802_CR(fd));
		printf("ADC : %i\t\t%i\n",
			NAU7802_readADCS(fd, 8),
			NAU7802_readADC(fd));
	}
}

void
test3(int fd){
	int z, i=0;
	printf("\n...Test...3\n");
	z = NAU7802_disablePGABypassCap(fd);
	printf("Bypass Cap : %i\n", z);
	z = NAU7802_togglePGAPhase(fd);
	printf("Phase inv=1 : %i\n", z);
	do{
	z = NAU7802_calibrate(fd, CALMOD_GCS);
	printf("CAL_ERR : %i\n", z);
	i++;
	delay(1000);
	}while(z && i<10);
	delay(5000);
	for(;;){
		while(!NAU7802_CR(fd));
		printf("ADC : %i\t\t%i\n",
			NAU7802_readADCS(fd, 8),
			NAU7802_readADC(fd));
	}
}

void
test4(int fd){
	int z, i=0;
	printf("\n...Test...4\n");
	z = NAU7802_disablePGAChopper(fd);
	printf("Chopper Disabled=1 : %i\n", z);
	do{
	z = NAU7802_calibrate(fd, CALMOD_GCS);
	printf("CAL_ERR : %i\n", z);
	i++;
	}while(z);
	delay(1000);
	for(;;){
		while(!NAU7802_CR(fd));
		printf("ADC : %i\t\t%i\n",
			NAU7802_readADCS(fd, 8),
			NAU7802_readADC(fd));
	}
}

void
test5(int fd){
	int z;
	struct load_cal lc;
	lc.gain = 0.2816;
	lc.offset = 0.0;
	lc.shift = 3;
	printf("\n...Test...5\n");
	z = NAU7802_calibrate(fd, CALMOD_GCS);
	printf("CAL_ERR : %i\n", z);
	delay(1000);
	for(;;){
		while(!NAU7802_CR(fd));
		printf("ADC : %-10i\tLoad : %+10.4f\n",
				NAU7802_readADC(fd),
				NAU7802_getLinearLoad(fd, &lc));
	}
}

void
test6(int fd, int rate){
	int z;
	struct load_cal lc;
	lc.gain = 0.2816;
	lc.zero = 0.0;
	lc.offset = 0.0;
	lc.shift = 3;
	printf("\n...Test...6\n");
	z = NAU7802_setSampleRate(fd, rate);
	if(z == -1)
		printf("Sample Rate : Invalid\n");
	else
		printf("Sample Rate : %i\n", z);
	z = NAU7802_calibrate(fd, CALMOD_GCS);
	printf("CAL_ERR : %i\n", z);
	delay(1000);
	delay(5000);
	for(;;){
		while(!NAU7802_CR(fd));
		printf("ADC : %-10i\tLoad : %+10.4f\n",
				NAU7802_readADC(fd),
				NAU7802_getLinearLoad(fd, &lc));
	}
}

void
test7(int fd){
	int z;
	struct load_cal lc;
	NAU7802_init_load_cal(&lc);
	printf("\n...Test...7\n");
	NAU7802_setLoadCalGain(&lc, 0.25);
	NAU7802_setShiftLoad(&lc, 0);
	z = NAU7802_calibrate(fd, CALMOD_GCS);
	printf("CAL_ERR : %i\n", z);
	delay(1000);
	NAU7802_getLinearLoad(fd, &lc);
	delay(5000);
	NAU7802_tareLoad(fd, &lc);
	for(;;){
		while(!NAU7802_CR(fd));
		printf("ADC : %-10i\tLoad : %+10.4f\n",
				NAU7802_readADC(fd),
				NAU7802_getLinearLoad(fd, &lc));
	}
}

void
test8(int fd){
	int z, shift, rate;
	struct load_cal lc;
	float cGain;
	NAU7802_init_load_cal(&lc);
	printf("\n...Test...8\n");
	printf("\nChip ID : %i\n",
			NAU7802_getChipRevId(fd));
	printf("Enter Cal Gain : ");
	scanf("%f", &cGain);
	printf("Enter shift bits : ");
	scanf("%i", &shift);
	printf("Enter rate bits : ");
	scanf("%i", &rate);
	NAU7802_setLoadCalGain(&lc, cGain);
	NAU7802_setShiftLoad(&lc, shift);
	NAU7802_setSampleRate(fd, rate);
	z = NAU7802_calibrate(fd, CALMOD_GCS);
	printf("CAL_ERR : %i\n", z);
	delay(1000);
	NAU7802_getLinearLoad(fd, &lc);
	delay(5000);
	NAU7802_tareLoad(fd, &lc);
	for(;;){
		while(!NAU7802_CR(fd));
		printf("Load : %+10.4f\n",
				NAU7802_getSmoothLoad(fd, &lc));
	}
}

void
test9(int fd){
	int i, z, shift, rate, readings;
	struct load_cal lc;
	float cGain, lpf;
	NAU7802_init_load_cal(&lc);
	printf("\n...Test...9\n");
	printf("\nChip ID : %i\n",
			NAU7802_getChipRevId(fd));
	printf("Enter Cal Gain : ");
	scanf("%f", &cGain);
	printf("Enter shift bits : ");
	scanf("%i", &shift);
	printf("Enter rate bits : ");
	scanf("%i", &rate);
	NAU7802_setLoadCalGain(&lc, cGain);
	NAU7802_setShiftLoad(&lc, shift);
	NAU7802_setSampleRate(fd, rate);
	z = NAU7802_calibrate(fd, CALMOD_GCS);
	printf("CAL_ERR : %i\n", z);
	delay(1000);
	NAU7802_getLinearLoad(fd, &lc);
	readings = (int)(pow(2, (double)rate) * 600);
	delay(1000);
	for(;;){
		NAU7802_tareLoad(fd, &lc);
		for(i=0; i<readings; i++){
			while(!NAU7802_CR(fd));
			printf("Load : %+10.4f\n",
				NAU7802_getSmoothLoad(fd, &lc));
		}	
		printf("Enter Cal Gain : ");
		scanf("%f", &cGain);
		printf("Enter shift bits : ");
		scanf("%i", &shift);
		printf("Enter LPF_Beta : ");
		scanf("%f", &lpf);
		NAU7802_setLoadCalGain(&lc, cGain);
		NAU7802_setShiftLoad(&lc, shift);
		lc.LPF_Beta = lpf;
		NAU7802_removeTareLoad(&lc);
		delay(100);
	}
}


int
main(int argc, char **argv){
	int fd;
	int z, gain=128;
	fd = wiringPiI2CSetup(NAU7802_ADDR);

	/* initialize NAU7802 */
	if((z = NAU7802_init(fd)) == 1)
		printf("Powerup Normal : %d\n", z);
	else
		printf("Powerup Fail : %d\n", z);
	delay(200);

	/* enable NAU7802 */
	if((z = NAU7802_enable(fd)) == 1)
		printf("Enabled : %d\n", z);
	else
		printf("Enable Failed : %d\n", z);
	delay(200);

	/* set gain, set from argv[2] if given */
	if(argc >= 3)
		gain = atoi(argv[2]);
	z = NAU7802_setGain(fd, gain);
	printf("Gain set to : %d\n", z);
	delay(200);

	z = NAU7802_AVDDSourceSelect(fd, AVDD_INT);
	printf("Source : %i\n", z);
	delay(200);

	z = NAU7802_setLDO(fd, V3_0);
	printf("Voltage : %i\n", z);
	delay(2000);

	/*select test from argv[1] */
	if(argc >= 2)
		z = atoi(argv[1]);
	else
		z = 1;

	if(z == 0)
		test0(fd);
	else if(z == 1)
		test1(fd);
	else if(z == 2)
		test2(fd);
	else if(z == 3)
		test3(fd);
	else if(z == 4)
		test4(fd);
	else if(z == 5)
		test5(fd);
	else if(z == 6){
		printf("Enter Rate : ");
		scanf("%i", &z);
		test6(fd, z);
	}
	else if(z == 7)
		test7(fd);
	else if(z == 8)
		test8(fd);
	else if(z == 9)
		test9(fd);
	else
		printf("+++++ Test not found +++++\n");


	return 0;
}
