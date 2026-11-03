#include <linux/io.h>

static int __init mt7621_hardware_overclock(void)
{
	void __iomem *sysc = ioremap(0x1e000000, 0x100);
	if (sysc) {
		/* Физическая запись 1100 МГц (0x4190a000) в регистр тактирования CLKCFG0 */
		writel(0x4190a000, sysc + 0x10);
		iounmap(sysc);
	}
	return 0;
}
/* Запуск на самой ранней стадии инициализации платформы */
pure_initcall(mt7621_hardware_overclock);
