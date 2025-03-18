#ifndef LIS2DE_COMMANDS_H
#define LIS2DE_COMMANDS_H

#define LIS2DE_ADDRESS 		0x19

#define FLOAT_MAX_8BIT   	16384.0
#define FLOAT_MAX_16BIT   	32768.0

#define LIS_STATUS_REG_AUX	0x07
#define LIS_OUT_TEMP_L		0x0C
#define LIS_OUT_TEMP_H		0x0D
#define LIS_WHO_AM_I		0x0F
#define LIS_CTRL_REG0		0x1E
#define LIS_TEMP_CFG_REG	0x1F
#define LIS_CTRL_REG1		0x20
#define LIS_CTRL_REG2		0x21
#define LIS_CTRL_REG3		0x22
#define LIS_CTRL_REG4		0x23
#define LIS_CTRL_REG5		0x24
#define LIS_CTRL_REG6		0x25
#define LIS_REFERENCE		0x26
#define LIS_STATUS_REG		0x27
#define LIS_FIFO_READ_START	0x28
#define LIS_OUT_X_H			0x29
#define LIS_OUT_Y_H			0x2B
#define LIS_OUT_Z_H			0x2D
#define LIS_FIFO_CTRL_REG	0x2E
#define LIS_FIFO_SRC_REG	0x2F
#define LIS_INT1_CFG		0x30
#define LIS_INT1_SRC		0x31
#define LIS_INT1_THS		0x32
#define LIS_INT1_DURATION	0x33
#define LIS_INT2_CFG		0x34
#define LIS_INT2_SRC		0x35
#define LIS_INT2_THS		0x36
#define LIS_INT2_DURATION	0x37
#define LIS_CLICK_CFG		0x38
#define LIS_CLICK_SRC		0x39
#define LIS_CLICK_THS		0x3A
#define LIS_TIME_LIMIT		0x3B
#define LIS_TIME_LATENCY	0x3C
#define LIS_TIME_WINDOW		0x3D
#define LIS_ACT_THS			0x3E
#define LIS_ACT_DUR			0x3


//aux stat
#define LIS_STATUS_REG_AUX_TOR_Msk		(1<<6) //Temperature Data Overrun (0 no ovr, 1 ovr)
#define LIS_STATUS_REG_AUX_TDA_Msk		(1<<2) //Temp new data avail. (0 not av., 1 av.)

//ctrl reg 0
#define LIS_CTRL_REG0_SDO_PU_DISC_Msk	(1<<7) //Disconnect SDO/SA0 pull-up (0 con, 1 disc)

//Temp cfg reg
#define LIS_TEMP_CFG_REG_TEMP_EN_Msk	(3<<6) //Enable Temp mask (00 dis, 11 en)

//Ctrl reg1
#define LIS_CTRL_REG1_ODR_PD_Msk		(0<<4) //Power Down mode
#define LIS_CTRL_REG1_ODR_1HZ_Msk		(1<<4) //Normal Mode 1Hz
#define LIS_CTRL_REG1_ODR_10HZ_Msk		(2<<4) //Normal Mode 10Hz
#define LIS_CTRL_REG1_ODR_25HZ_Msk		(3<<4) //Normal Mode 25Hz
#define LIS_CTRL_REG1_ODR_50HZ_Msk		(4<<4) //Normal Mode 50Hz
#define LIS_CTRL_REG1_ODR_100HZ_Msk		(5<<4) //Normal Mode 100Hz
#define LIS_CTRL_REG1_ODR_200HZ_Msk		(6<<4) //Normal Mode 200Hz
#define LIS_CTRL_REG1_ODR_400HZ_Msk		(7<<4) //Normal Mode 400Hz
#define LIS_CTRL_REG1_ODR_1_62kHZ_Msk	(8<<4) //Normal Mode 1.62kHz
#define LIS_CTRL_REG1_ODR_5_376kHZ_Msk	(9<<4) //Normal Mode 5.376kHz
#define LIS_CTRL_REG1_LPen_Msk			(1<<3) //Enable device?
#define LIS_CTRL_REG1_X_En_Msk			(1<<2) //Enable X axis
#define LIS_CTRL_REG1_Y_En_Msk			(1<<1) //Enable Y axis
#define LIS_CTRL_REG1_Z_En_Msk			(1<<0) //Enable Z axis

//ctrl reg2
#define LIS_CTRL_REG2_HPM_MODE1_Msk		(0<<6) //High-Pass filter normal mode (reset by reference)
#define LIS_CTRL_REG2_HPM_MODE2_Msk		(1<<6) //High-Pass filter Reference signal for filtering
#define LIS_CTRL_REG2_HPM_MODE3_Msk		(2<<6) //High-Pass filter normal mode
#define LIS_CTRL_REG2_HPM_MODE4_Msk		(3<<6) //High-Pass filter autoreset on interrupt event
#define LIS_CTRL_REG2_HPCF_MODE1_Msk	(0<<4) //Cutoff Frequency mode 1 (see table)
#define LIS_CTRL_REG2_HPCF_MODE2_Msk	(1<<4) //Cutoff Frequency mode 2 (see table)
#define LIS_CTRL_REG2_HPCF_MODE3_Msk	(2<<4) //Cutoff Frequency mode 3 (see table)
#define LIS_CTRL_REG2_HPCF_MODE4_Msk	(3<<4) //Cutoff Frequency mode 4 (see table)
#define LIS_CTRL_REG2_FDS_Msk			(1<<3) // Filter data sel (0 filter bypass, 1 filter to output)
#define LIS_CTRL_REG2_HPCLICK_Msk		(1<<2) //Enable HP for CLICK
#define LIS_CTRL_REG2_HP_IA2_Msk		(1<<1) //Enable HP for AOI on INT2
#define LIS_CTRL_REG2_HP_IA1_Msk		(1<<0) //Enable HP for AOI on INT1

//ctrl reg3
#define LIS_CTRL_REG3_I1_CLICK_Msk		(1<<7) //CLICK int on INT1 enable
#define LIS_CTRL_REG3_I1_IA1_Msk		(1<<6) //IA1 int on INT1 enable
#define LIS_CTRL_REG3_I1_IA2_Msk		(1<<5) //IA2 int on INT1 enable
#define LIS_CTRL_REG3_I1_ZYXDA_Msk		(1<<4) //ZYXDA int on INT1 enable
#define LIS_CTRL_REG3_I1_WTM_Msk		(1<<2) //FIFO WATERMARK int on INT1 enable
#define LIS_CTRL_REG3_I1_OVERRUN_Msk	(1<<1) //FIFO_OVERRUN int on INT1 enable

//ctrl reg4
#define LIS_CTRL_REG4_BDU_Msk			(1<<7) //Block Data update (0 - cont. update, 1 - not update until read)
#define LIS_CTRL_REG4_FS_Pos			4      //Position
#define LIS_CTRL_REG4_FS_2g_Msk			(0<<4) //Full-Scale sel
#define LIS_CTRL_REG4_FS_4g_Msk			(1<<4) //Full-Scale sel
#define LIS_CTRL_REG4_FS_8g_Msk			(2<<4) //Full-Scale sel
#define LIS_CTRL_REG4_FS_16g_Msk		(3<<4) //Full-Scale sel
#define LIS_CTRL_REG4_ST_NORMAL_Msk		(0<<1) //Self-Test disable
#define LIS_CTRL_REG4_FS_MODE0_Msk		(1<<1) //Self-Test 0
#define LIS_CTRL_REG4_FS_MODE1_Msk		(2<<1) //Self-Test 1
#define LIS_CTRL_REG4_SIM_Msk			(1<<0) //SPI mode sel (0 - 4-wire, 1 - 3-wire)

//ctrl reg5
#define LIS_CTRL_REG5_BOOT_Msk			(1<<7) //Reboot Memory Content (0:normal, 1: reboot mem content)
#define LIS_CTRL_REG5_FIFO_EN_Msk		(1<<6) //FIFO en (0: dis, 1: en)
#define LIS_CTRL_REG5_LIR_INT1_Msk		(1<<3) //Latch int1 request (0: not latched, 1: latched)
#define LIS_CTRL_REG5_D4D_INT1_Msk		(1<<2) //4D enable on int1 (when 6d bit en in int1_cfg)
#define LIS_CTRL_REG5_LIR_INT2_Msk		(1<<1) //Latch int2 request (0: not latched, 1: latched)
#define LIS_CTRL_REG5_D4D_INT2_Msk		(1<<0) //4D enable on int2 (when 6d bit en in int2_cfg)

//ctrl reg6
#define LIS_CTRL_REG6_I2_CLICK_Msk		(1<<7) //CLICK int on INT2 enable
#define LIS_CTRL_REG6_I2_IA1_Msk		(1<<6) //IA1 int on INT2 enable
#define LIS_CTRL_REG6_I2_IA2_Msk		(1<<5) //IA2 int on INT2 enable
#define LIS_CTRL_REG6_I2_BOOT_Msk		(1<<4) //BOOT on INT2 enable
#define LIS_CTRL_REG6_I2_ACT_Msk		(1<<3) //Activity int on INT2 enable
#define LIS_CTRL_REG6_INT_POLARITY_Msk	(1<<1) //INT1 and INT2 polarity (0: active high, 1: active low)

//status reg
#define LIS_STATUS_REG_ZYXOR_Msk		(1<<7) //x,y,z overrun
#define LIS_STATUS_REG_ZOR_Msk			(1<<6) //z overrun
#define LIS_STATUS_REG_YOR_Msk			(1<<5) //y overrrun
#define LIS_STATUS_REG_XOR_Msk			(1<<4) //x overrun
#define LIS_STATUS_REG_ZYXDA_Msk		(1<<3) //x,y,z new data available
#define LIS_STATUS_REG_ZDA_Msk			(1<<2) //z new data available
#define LIS_STATUS_REG_YDA_Msk			(1<<1) //y new data available
#define LIS_STATUS_REG_XDA_Msk			(1<<1) //x new data available

//fifo ctrl reg
#define LIS_FIFO_CTRL_REG_FM_BP_Msk		(0<<6) //Bypass mode
#define LIS_FIFO_CTRL_REG_FM_FIFO_Msk	(1<<6) //FIFO mode
#define LIS_FIFO_CTRL_REG_FM_STREAM_Msk	(2<<6) //Stream mode
#define LIS_FIFO_CTRL_REG_FM_StF_Msk	(3<<6) //Stream to FIFO mode
#define LIS_FIFO_CTRL_REG_FM_TRIG_Msk	(1<<5) //0:trigger on INT1, 1: trigger on INT2
#define LIS_FIFO_CTRL_REG_FM_FTH_Msk	(0<<0) //?

#endif
