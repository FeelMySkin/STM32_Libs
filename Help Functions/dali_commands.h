#ifndef DALI_COMMANDS_H
#define DALI_COMMANDS_H

/*DALI INFO*/
#define UNLOCKED 		0x55
#define LOCKED 			0
#define ANSWER_TIMEOUT	20 //in ms

/*DALI ADDRESSING Commands*/
#define GROUP 0x80
#define BROADCAST 0xFE
#define SPECIAL1 0xA1
#define SPECIAL2 0xC1
#define INSTANCE_SPECIAL	0x60

/*DALI Commands*/
#define QUERY_DEVICETYPE 0x99
#define QUERY_GROUPS07 0xC0
#define QUERY_GROUPS815 0xC1
#define REMOVE_GROUP 0x70
#define ADD_GROUP 0x60
#define STORE_DTR_AS_SHORT 0x80
#define ENABLE_WRITE_MEMORY 0x81
#define READ_MEMORY_LOCATION 0xC5

/*DALI2 Commands*/
#define D2_RESET				0x10
#define D2_SAVE_VARIABLES		0x22 //Send Twice
#define D2_ENABLE_INSTANCE		0x62 //Send Twice
#define D2_DISABLE_INSTANCE		0x63 //Send Twice
#define D2_QUERY_INSTANCES		0x35
#define D2_READ_MEMORY_LOCATION	0x3C
#define D2_QUERY_TYPE			0x80
#define D2_SET_EVENT_FILTER		0x68 //Send Twice
#define D2_QUERY_EVENT_FILTER1	0x90
#define D2_QUERY_EVENT_FILTER2	0x91
#define D2_QUERY_EVENT_FILTER3	0x92
#define D2_ENABLE_WRITE_MEMORY	0x15 //Send Twice


/*DALI Special Commands*/
#define TERMINATE 0xA1
#define DTR 0xA3
#define INITIALIZE 0xA5
#define RANDOMIZE 0xA7
#define COMPARE 0xA9
#define WITHDRAW 0xAB
#define SEARCHADDRH 0xB1
#define SEARCHADDRM 0xB3
#define SEARCHADDRL 0xB5
#define PROGRAMSHORT 0xB7
#define VERIFYSHORT 0xB9
#define QUERYSHORT 0xBB
#define DTR1 0xC3
#define DTR2 0xC5
#define WRITE_MEMORY_LOCATION 0xC7

/*DALI Special Instance*/
#define INSTANCE_TERMINATE 0x00
#define INSTANCE_INITIALIZE 0x01
#define INSTANCE_RANDOMISE 0x02
#define INSTANCE_COMPARE 0x03
#define INSTANCE_WITHDRAW 0x04
#define INSTANCE_SEARCHADDRH 0x05
#define INSTANCE_SEARCHADDRM 0x06
#define INSTANCE_SEARCHADDRL 0x07
#define INSTANCE_PROGRAMSHORT 0x08
#define INSTANCE_VERIFYSHORT 0x09
#define INSTANCE_QUERYSHORT 0x0A
#define INSTANCE_WRITE_MEMORY_LOCATION 0x20
#define INSTANCE_WRITE_MEMORY_LOCATION_NO_REPLY 0x21
#define INSTANCE_DTR0 0x30
#define INSTANCE_DTR1 0x31
#define INSTANCE_DTR2 0x32




/*DALI2 INSTACNE SPECIFIC*/
//MMW SENSOR
#define D2_CATCH_MOVEMENT	0x20
#define D2_SET_HOLD_TIM		0x21
#define D2_SET_REPORT_TIM	0x22
#define D2_SET_DEAD_TIM		0x23
#define D2_CANCEL_HOLD_TIM	0x24
#define D2_QUERY_DEAD_TIM	0x2C
#define D2_QUERY_HOLD_TIM	0x2D
#define D2_QUERY_REPORT_TIM	0x2E
#define D2_QUERY_CATCHING	0x2F

//GAS SENSOR
#define D2_GAS_CALIBRATE	0x20



//Firmware
#define D2_FIRMWARE_STATE_STARTED		0x11
#define D2_FIRMWARE_STATE_OK			0x22
#define D2_FIRMWARE_STATE_END_OK		0x33
#define D2_FIRMWARE_STATE_SHIFT_ERROR	0x55
#define D2_FIRMWARE_STATE_FAIL			0xFF



#endif