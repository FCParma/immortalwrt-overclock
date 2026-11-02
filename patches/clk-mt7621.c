#include <linux/clk-provider.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <dt-bindings/clock/mt7621-clk.h>

#define MT7621_CLK_CPU_DEFAULT	1100000000
#define MT7621_CLK_BUS_DEFAULT	220000000

/* Драйвер ядра скомпилирует эту константу как базовую */
