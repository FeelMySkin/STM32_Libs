#include "gps_controller.h"

GPS_Controller::GPS_Controller()
{
	completed = false;
	main_flags.desync = true;
	pointer = 0;
	timeout = 0;
	//full_buffer.SetSize(512);
}
//

void GPS_Controller::Init(UsartController*	usart)
{
	this->usart = usart;
	//InitGPIO();
	//InitUART();
	char gtk[18] = "$PMTK869,2,1*36\r\n";
	usart->Send(gtk);
	while(!usart->IsSent())
	{
		osDelay(5);
	}
	rmc.signal_level = 0;
}
//

void GPS_Controller::PushBuffer()
{
	//full_buffer.push(LL_USART_ReceiveData8(gps.uart));
}
//

void GPS_Controller::ProcessRMC()
{
	test_bool = false;
	uint8_t summ = 0,r_summ = 0;
	
		
	completed = false;
	uint8_t buf_ptr = pointer;
	pointer = 0;
	rmc.signal_level = 0;
	
	if(CompareWildStrings("G?RMC,#*??",rmc_buffer,'#') != 0)
	{
		for(int i = 0;i<buf_ptr-3;++i)
		{
			summ^= rmc_buffer[i];
		}
		sscanf(rmc_buffer+buf_ptr-2,"%2hhX",&r_summ);
		if(r_summ == summ)
		{
			char* buf = rmc_buffer+5;
			short yy,mm,dd,h,m,s,ms, lat_d, lon_d;
			double lat_m,lon_m;
			test_bool = true;
			
			for(int i = 0;i<=12;)
			{
				buf++;
				if (*(buf-1)=='*' || *(buf-1)==0x00) break;
				if (*(buf-1)==',') {
					i++;
					if (i==1) {
						sscanf(buf,"%2hd%2hd%2hd.%hd",&h,&m,&s,&ms);
					} else if (i==2) {
						if (*buf!='A') return;
					} else if (i==3) {
						if (sscanf(buf,"%2hd%lf",&lat_d,&lat_m)!=2) return;
					} else if (i==4) {
						if (*buf=='S') {
							lat_d= -lat_d;
							lat_m= -lat_m;
						}
					} else if (i==5) {
						if (sscanf(buf,"%3hd%lf",&lon_d,&lon_m)!=2) return;
					} else if (i==6) {
						if (*buf=='W') {
							lon_d= -lon_d;
							lon_m= -lon_m;
						}
					} else if (i==9) {
						if (sscanf(buf,"%2hd%2hd%2hd",&dd,&mm,&yy)!=3) return;
					} else if (i==12) {
						if (*buf=='N') return;
					}
				}
			}
			
			if(ms != 0) return;
	
			rmc.year = yy;
			rmc.month = mm;
			rmc.day = dd;
			rmc.hours = h;
			rmc.mins = m;
			rmc.sec = s;
			rmc.msec = ms;
			rmc.lattitude = (double)lat_d + lat_m/60.;
			rmc.longitude = (double)lon_d + lon_m/60.;
			rmc.signal_level = 31;
		}
	}

}
//

void GPS_Controller::ProcessTime()
{
	if(rmc.signal_level != 0 && (time.GMT&0x80)==0)
	{
		int32_t rmc_time;
		int32_t curr_rmc;
		int32_t shift = ((time.GMT&0x40)==0?1:-1)*15*60*(time.GMT&0x3F);
		rmc_time = rmc.hours*3600+rmc.mins*60+rmc.sec + shift;
		if(rmc_time>=86400)
		{
			rmc_time-=86400;
			rmc.day++;
			
			if(	(rmc.month == 1 && rmc.day == 32) ||
				(rmc.month == 2 && ((rmc.day == 30 && rmc.year%4 == 0) || (rmc.day == 29 && rmc.year%4 != 0))) ||
				(rmc.month == 3 && rmc.day == 32) ||
				(rmc.month == 4 && rmc.day == 31) ||
				(rmc.month == 5 && rmc.day == 32) ||
				(rmc.month == 6 && rmc.day == 31) ||
				(rmc.month == 7 && rmc.day == 32) ||
				(rmc.month == 8 && rmc.day == 32) ||
				(rmc.month == 9 && rmc.day == 31) ||
				(rmc.month == 10 && rmc.day == 32) ||
				(rmc.month == 11 && rmc.day == 31) ||
				(rmc.month == 12 && rmc.day == 32))
				{
					rmc.month++;
					rmc.day = 1;
				}
				
				if(time.month == 13)
				{
					rmc.year++;
					rmc.month = 1;
				}
		}
		if(rmc_time<0)
		{
			rmc_time+=86400;
			rmc.day--;
			if(rmc.day==0)
			{
				rmc.month--;
				if(rmc.month == 0)
				{
					rmc.year--;
					rmc.month = 12;
				}
				if(rmc.month == 1 || rmc.month == 3 || rmc.month == 5 || rmc.month == 7 || rmc.month == 8 || rmc.month == 10 || rmc.month == 12) rmc.day = 31;
				else if(rmc.month != 2) rmc.day = 30;
				else
				{
					if(rmc.year%4 == 0) rmc.day=29;
					else rmc.day = 28;
				}
			}
		}
		rmc.hours=(rmc_time-rmc.sec)/3600;
		rmc.mins = (rmc_time-rmc.hours*3600-rmc.sec)/60;
		
		curr_rmc = time.hours*3600+time.mins*60+time.secs;
		
		if((rmc_time+86400-curr_rmc)%86400>=30 || rmc.month != time.month)
		{
			time.day = rmc.day;
			time.month = rmc.month;
			time.year = rmc.year;
			time.secs = rmc.sec;
			time.mins = rmc.mins;
			time.hours = rmc.hours;
		}
		timeout = 0;
		main_flags.time_init = true;
		main_flags.desync = false;
	}
}
//


void GPS_Controller::Process()
{
	char log_buf[12];
	uint8_t ptr = 0;
	
	if(pointer ==0 )
	{
		while(usart->Length() >= 11)
		{
			if(usart->Pull() == '$')
			{
				ptr = 0;
				while(usart->GetChar() != ',' && usart->GetChar() != '*')
				{
					log_buf[ptr] = usart->Pull();
					ptr++;
					if(ptr == 12) break;
					log_buf[ptr] = 0;
				}
				if(CompareWildStrings("G?RMC",log_buf)!=0)
				{
					for(int i = 0;i<ptr;++i)
					{
						rmc_buffer[i] = log_buf[i];
						pointer = ptr;
					}
					break;
				}
				
			}
		}
	}
		//
		
		if(pointer != 0)
		{
			while(usart->Length()>0)
			{
				rmc_buffer[pointer] = usart->Pull();
				pointer++;
				
				if(pointer==100)
				{
					pointer = 0;
					break;
				}
				rmc_buffer[pointer] = 0;
				
				if(usart->GetChar() == '\r' || usart->GetChar() == '\n')
				{
					completed = true;
					break;
				}
			}
		}
		//
	
	if(completed)
	{
		ProcessRMC();
		ProcessTime();
	}
	

	
}
//

void GPS_Controller::ProcessSecond()
{
	if(timeout>=3600) main_flags.desync = true;
	else timeout++;
}
//
