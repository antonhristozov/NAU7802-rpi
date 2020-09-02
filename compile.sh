gcc -o load NAU7802_driver.c NAU7802.c -lwiringPi -lm
gcc -o test test.c NAU7802.c -lwiringPi
