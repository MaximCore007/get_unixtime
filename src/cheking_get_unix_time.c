/*
 ============================================================================
 Name        : cheking_get_unix_time.c
 Author      : Maxim
 Version     :
 Copyright   : Your copyright notice
 Description : Cheking timestamp UNIX in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <time.h>

typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

/**
  * @brief  RTC Time structure definition
  */
typedef struct
{
  uint8_t RTC_Hours;    /*!< Specifies the RTC Time Hour.
                        This parameter must be set to a value in the 0-12 range
                        if the RTC_HourFormat_12 is selected or 0-23 range if
                        the RTC_HourFormat_24 is selected. */

  uint8_t RTC_Minutes;  /*!< Specifies the RTC Time Minutes.
                        This parameter must be set to a value in the 0-59 range. */

  uint8_t RTC_Seconds;  /*!< Specifies the RTC Time Seconds.
                        This parameter must be set to a value in the 0-59 range. */

  uint8_t RTC_H12;      /*!< Specifies the RTC AM/PM Time.
                        This parameter can be a value of @ref RTC_AM_PM_Definitions */
}RTC_TimeTypeDef;

/**
  * @brief  RTC Date structure definition
  */
typedef struct
{
  uint8_t RTC_WeekDay; /*!< Specifies the RTC Date WeekDay.
                        This parameter can be a value of @ref RTC_WeekDay_Definitions */

  uint8_t RTC_Month;   /*!< Specifies the RTC Date Month (in BCD format).
                        This parameter can be a value of @ref RTC_Month_Date_Definitions */

  uint8_t RTC_Date;     /*!< Specifies the RTC Date.
                        This parameter must be set to a value in the 1-31 range. */

  uint8_t RTC_Year;     /*!< Specifies the RTC Date Year.
                        This parameter must be set to a value in the 0-99 range. */
}RTC_DateTypeDef;

uint32_t GetTimeUNIX(RTC_DateTypeDef date, RTC_TimeTypeDef time)
{
	int32_t utime, year, month;
	int32_t oneday = 3600;
	uint8_t is_leap_year;

	// Leap year
	if (!date.RTC_Year % 4)
		is_leap_year = 1;
	else
		is_leap_year = 0;

	year = (date.RTC_Year + 2000 - 1970) * 31536000; // 1970 year is unix time
	month = date.RTC_Month;

	utime = year;
	utime +=  518400; // 6 leap years with 1970 to 2000
	if (month < 8) {
		if (month == 2) { // February
				if (!date.RTC_Year % 4) // Leap year
					utime += (month * 2505600); // 29 days
				else
					utime += (month * 2419200); // 28 days
		}
		else if (month % 2)
			utime += (month * 2678400); // 31 days
		else
			utime += (month * 2592000); // 30 days
	}
	else if (month >= 8) {
		if (month % 2)
			utime += (month * 2592000); // 30 days
		else
			utime += (month * 2678400); // 31 days
	}
	utime += date.RTC_Date * 86400;
	utime += time.RTC_Hours * 3600;
//	utime += 3600 * g_params.timezone;				// becouse timezone
	utime += time.RTC_Minutes * 60;
	utime += time.RTC_Seconds;

	return utime;
}

int main(void) {
	puts("Cheking timestamp UNIX (from STM32 MCU structures).");

	time_t rawtime;
	struct tm *timeinfo;

	RTC_DateTypeDef stm32_data;
	RTC_TimeTypeDef stm32_time;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("Current local time and date: %s", asctime(timeinfo));

	stm32_data.RTC_Year = timeinfo->tm_year - 100;
	stm32_data.RTC_Month = timeinfo->tm_mon + 1;
	stm32_data.RTC_Date = timeinfo->tm_mday;
	stm32_time.RTC_Hours = timeinfo->tm_hour;
	stm32_time.RTC_Minutes = timeinfo->tm_min;
	stm32_time.RTC_Seconds = timeinfo->tm_sec;
	printf("Converted time in STM32 structures:");
	printf("\nYear: %i; Month: %i; Date: %i; Hours: %i; Minutes: %i; Seconds: %i",
			stm32_data.RTC_Year, stm32_data.RTC_Month, stm32_data.RTC_Date,
			stm32_time.RTC_Hours, stm32_time.RTC_Minutes, stm32_time.RTC_Seconds);

	printf("\n\nCurrent local time and date in UNIX time: %i", (uint32_t)mktime(timeinfo));
	printf("\nConverted time from STM32 to unix time: %i", GetTimeUNIX(stm32_data, stm32_time));

	return EXIT_SUCCESS;
}
