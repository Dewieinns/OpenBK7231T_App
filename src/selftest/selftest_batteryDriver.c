#ifdef WINDOWS

#include "selftest_local.h"
#include "../driver/drv_battery.h"

// P23 is ADC, it is connected to two resistors, both 1k,
// one is connected to P26, second to VDD
void Test_Battery_SmokeSensor() {
	// reset whole device
	SIM_ClearOBK(0);

	PIN_SetPinRoleForPinIndex(26, IOR_BAT_Relay_n);
	// some random unused channel
	PIN_SetPinChannelForPinIndex(26, 5);

	PIN_SetPinRoleForPinIndex(23, IOR_BAT_ADC);
	PIN_SetPinChannelForPinIndex(23, 5);

	CMD_ExecuteCommand("startDriver Battery", 0);
	// Battery 1.997V - raw ADC 1714
	// Battery 2.998V - raw ADC 2588
	// args minbatt and maxbatt in mv. optional V_divider(2), Vref(default 2400) and ADC bits(4096)
	CMD_ExecuteCommand("Battery_Setup 2000 3000 2 2400 4096", 0);

	// Battery 2.998V - raw ADC 2588 - 100%
	SIM_SetIntegerValueADCPin(23, 2588);

	Simulator_Force_Batt_Measure();

	// assert: current ,expected, max difference, the voltage is in mV
	SELFTEST_ASSERT_FLOATCOMPAREEPSILON(Battery_lastreading(OBK_BATT_VOLTAGE), 2998, 100);
	// assert: current ,expected, max difference
	SELFTEST_ASSERT_FLOATCOMPAREEPSILON(Battery_lastreading(OBK_BATT_LEVEL), 100, 5.0f);

	// Battery 1.997V - raw ADC 1714 - 0%
	SIM_SetIntegerValueADCPin(23, 1714);

	Simulator_Force_Batt_Measure();

	// assert: current ,expected, max difference, the voltage is in mV
	SELFTEST_ASSERT_FLOATCOMPAREEPSILON(Battery_lastreading(OBK_BATT_VOLTAGE), 1997, 100);
	// assert: current ,expected, max difference
	SELFTEST_ASSERT_FLOATCOMPAREEPSILON(Battery_lastreading(OBK_BATT_LEVEL), 0, 5.0f);
}
// No BAT_ADC pin: the module runs straight from the cell (e.g. CB3S leak
// sensors), so the driver reads the chip's internal VBAT channel instead.
void Test_Battery_InternalVBAT() {
	SIM_ClearOBK(0);

	CMD_ExecuteCommand("startDriver Battery", 0);
	CMD_ExecuteCommand("Battery_Setup 2000 3000 2 2400 4096", 0);

	// raw 2560 -> 2560 * 2400 / 4096 * 2 = 3000 mV -> 100%
	SIM_SetSimulatedVBAT(2560);
	Simulator_Force_Batt_Measure();
	SELFTEST_ASSERT_FLOATCOMPAREEPSILON(Battery_lastreading(OBK_BATT_VOLTAGE), 3000, 5);
	SELFTEST_ASSERT_FLOATCOMPAREEPSILON(Battery_lastreading(OBK_BATT_LEVEL), 100, 1.0f);

	// raw 2134 -> 2500 mV -> 50%
	SIM_SetSimulatedVBAT(2134);
	Simulator_Force_Batt_Measure();
	SELFTEST_ASSERT_FLOATCOMPAREEPSILON(Battery_lastreading(OBK_BATT_VOLTAGE), 2500, 5);
	SELFTEST_ASSERT_FLOATCOMPAREEPSILON(Battery_lastreading(OBK_BATT_LEVEL), 50, 1.0f);

	// a BAT_ADC pin still takes priority over the internal channel
	PIN_SetPinRoleForPinIndex(23, IOR_BAT_ADC);
	PIN_SetPinChannelForPinIndex(23, 5);
	SIM_SetIntegerValueADCPin(23, 1707);
	Simulator_Force_Batt_Measure();
	SELFTEST_ASSERT_FLOATCOMPAREEPSILON(Battery_lastreading(OBK_BATT_VOLTAGE), 2000, 5);

	SIM_SetSimulatedVBAT(-1);
}
void Test_Battery() {
	Test_Battery_SmokeSensor();
	Test_Battery_InternalVBAT();

}

#endif
