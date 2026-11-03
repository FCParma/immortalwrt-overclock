name: Build Overclocked ImmortalWrt v8

on:
  workflow_dispatch:

jobs:
  build:
    runs-on: ubuntu-24.04
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Install build dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y build-essential clang flex bison g++ gawk gcc-multilib g++-multilib \
            gettext git libncurses5-dev libssl-dev python3-setuptools rsync swig unzip zlib1g-dev \
            file wget subversion texinfo ecj fastjar java-propose-classpath lzma

      - name: Clone ImmortalWrt Source
        run: |
          git clone --depth 1 -b openwrt-25.12 https://github.com/immortalwrt/immortalwrt openwrt
          
      - name: Update and install feeds
        run: |
          cd openwrt
          ./scripts/feeds update -a
          ./scripts/feeds install -a
          rm -rf feeds/luci/applications/luci-app-weechat

      - name: Inject Compiler Level Overclock
        run: |
          # Создаем целевую папку в исходниках ядра, куда копируем наш Си-код
          DEST_DIR="openwrt/target/linux/ramips/patches-6.12"
          mkdir -p $DEST_DIR
          
          # Заставим ядро скопировать файл mt7621_overclock.h в корень исходников ядра при распаковке
          # Для этого мы создаем легальный пустой патч, создающий этот файл
          cat << 'EOF' > $DEST_DIR/999-mt7621-overclock.patch
          --- /dev/null
          +++ b/include/linux/mt7621_overclock.h
          @@ -0,0 +1,14 @@
          +#include <linux/io.h>
          +static int __init mt7621_hardware_overclock(void)
          +{
          +	void __iomem *sysc = ioremap(0x1e000000, 0x100);
          +	if (sysc) {
          +		writel(0x4190a000, sysc + 0x10);
          +		iounmap(sysc);
          +	}
          +	return 0;
          +}
          +pure_initcall(mt7621_hardware_overclock);
          EOF

      - name: Configure Compiler Options
        run: |
          cd openwrt
          # Добавляем в глобальные флаги компиляции ядра требование автоматически внедрять наш оверклок
          # во все файлы архитектуры MIPS Ralink
          echo "CONFIG_TARGET_ramips=y" > .config
          echo "CONFIG_TARGET_ramips_mt7621=y" >> .config
          echo "CONFIG_TARGET_ramips_mt7621_DEVICE_xiaomi_mi-router-3g=y" >> .config
          echo "CONFIG_KERNEL_CFLAGS=\"-include linux/mt7621_overclock.h\"" >> .config
          make defconfig

      - name: Compile Final Firmware
        run: |
          cd openwrt
          make -j2 download
          make -j2 V=s || make -j1 V=s

      - name: Upload Firmware Artifacts
        uses: actions/upload-artifact@v4
        with:
          name: Overclocked-ImmortalWrt-8
          path: openwrt/bin/targets/ramips/mt7621/*sysupgrade.bin
