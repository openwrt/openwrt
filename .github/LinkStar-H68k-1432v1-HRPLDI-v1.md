---
title: Draft LinkStar H68k Device Inventory
author: Erik Beck
date: 4 March 2025
---




# Device #
Seeed LinkStar

## Device Name and Version ##
* Seeed LinkStar H68K 1432 Version 1

## Basic Description ##

### System on a Chip (SoC) ###
* Rockchip RK3568 Family
* Package Type
    - FCBGA636L (body: 19mm x 19mm; ball size: 0.35mm; ball pitch: 0.65mm)
	
#### CPU ####
* Rockchip RK3568 Family

##### CPU/Microprocesor Details #####
* Architecture_1: ARM Cortex-A55 CPU
* Architecture_2: ARM64 aarch64
* Number of Cores: 4
* Quad-core ARM Cortex-A55 CPU
* ARM Neon Advanced SIMD (single instruction, multiple data) support
for accelerated media and signal processing computation
* Include VFP hardware to support single and double-precision operations
* ARMv8 Cryptography Extensions
* Integrated 32KB L1 instruction cache, 32KB L1 data cache with ECC
* 512KB unified system L3 cache with ECC
* TrustZone technology support
* Separate power domains for CPU core system to support internal power
switch and externally turn on/off based on different application 
scenario
    - PD_A55_0: 1st Cortex-A55 + Neon + FPU + L1 I/D Cache
    - PD_A55_1: 2nd Cortex-A55 + Neon + FPU + L1 I/D Cache
    - PD_A55_2: 3rd Cortex-A55 + Neon + FPU + L1 I/D Cache
    - PD_A55_3: 4th Cortex-A55 + Neon + FPU + L1 I/D Cache
* One isolated voltage domain


#### Memory Organization ####
* Internal on-chip memory
    - BootROM
    - SYSTEM_SRAM in the voltage domain of VD_LOGIC
    - PMU_SRAM in the voltage domain of VD_PMU for low power application
	
* Internal BootRom
    - Support system boot from the following device:
    - SPI Flash interface
    - Nand Flash
    - eMMC interface
    - SDMMC interface
* Support system code download by the following interface:
    - USB OTG interface (Device mode)
* SYSTEM_SRAM
    - Size: 64KB
* PMU_SRAM
    - Size: 8KB

* External off-chip memory (available, but not necessarily present)
    - DDR3/DDR3L/DDR4/LPDDR3/LPDDR4/LPDDR4X
    - SPI Nor/Nand Flash
    - eMMC
    - SD_Card
    - 8bits Async Nand Flash
    - 8bits toggle Nand Flash
    - 8bits ONFI Nand Flash

* 1.2.5 External Memory or Storage device
    - Dynamic Memory Interface (DDR3/DDR3L/DDR4/LPDDR2/LPDDR3/LPDDR4/LPDDR4X)
        + Compatible with JEDEC standards
        + Compatible with 
		DDR3-2133/DDR3L-2133/LPDDR3-2133/DDR4-3200/LPDDR4- 3200/LPDDR4X-3200
        + Support 32bits data width, 2 ranks (chip selects), total
		addressing space is 8GB(max) for DDR3/DDR3L/DDR4
        + Support 32bits data width, 4 ranks (chip selects), total
		addressing space is 8GB(max) for LPDDR3/LPDDR4/LPDDR4X
        + Low power modes, such as power-down and self-refresh for SDRAM
        + Compensation for board delays and variable latencies through
		programmable pipelines
        + Support 8bits ECC for DDR3/DDR3L/DDR4
        + Programmable output and ODT impedance with dynamic PVT
          compensation
    - eMMC Interface
        + Compatible with standard iNAND interface
        + Compatible with eMMC specification 4.41, 4.51, 5.0 and 5.1
        + Support three data bus width: 1bit, 4bits or 8bits
        + Support HS200;
        + Support CMD Queue
    - SD/MMC Interface
        + Compatible with SD3.0, MMC ver4.51
        + Data bus width is 4bits
    - Nand Flash Interface
        + Support async nand flash, each channel 8bits, up to 4 banks
        + Support ONFI Synchronous Flash Interface, each channel
          8bits, up to 4 banks
        + Support Toggle Flash Interface, each channel 8bits, up to 4 banks
        + Support sync DDR nand flash, each channel 8bits, up to 4 bank
        + Support LBA nand flash in async or sync mode
        + Up to 70bits/1KB hardware ECC
        + For DDR nand flash, support DLL bypass and 1/4 or 1/8 clock
		adjust, maximum clock rate is 75MHz
        + For async nand flash, support configurable interface timing, 
		maximum data rate is 16bits/cycle
    - SPI Flash Interface
        + Support Serial NOR Flash, NAND Flash, pSRAM and SRAM
        + Support SDR mode
        + Support 1bit/2bit/4bit data width

#### GPU ####
* 3D Graphics Engine:
    - Mali-G52 1-Core-2EE
    - Support OpenGL ES 1.1, 2.0, and 3.2
    - Support Vulkan 1.0 and 1.1
    - Support OpenCL 2.0 Full Profile
    - Support 1600Mpix/s fill rate when 800MHz clock frequency
    - Support 38.4GLOPs when 800MHz clock frequency
* 2D Graphics Engine:
    - Data format
* Support input of
     - RGB/RGB888/RGB565/RGB4444/RGB5551/YUV420/YUV422/YUYV；
* Support input of YUV422SP10bit/YUV420SP10bit(YUV-8bits out)
* Support output of
    - ARGB/RGB888/RGB565/RGB4444/RGB5551/YUV420/YUV422/YUYV；
* Pixel Format conversion, BT.601/BT.709
* Dither operation, Y dither update;
* Max resolution: 8192x8192 source, 4096x4096 destination



#### Connectivity ####
* SDIO interface
    - Compatible with SDIO3.0 protocol
    - 4bits data bus widths
* MAC 10/100/1000 Ethernet Controller
    - Support two identical Ethernet controllers
    - Support 10/100/1000 Mbps data transfer rates with the RGMII interfaces
    - Support 10/100 Mbps data transfer rates with the RMII interfaces
    - Support both full-duplex and half-duplex operation
    - Supports IEEE 802.1Q VLAN tag detection for reception frames
    - Support detection of LAN wake-up frames and AMD Magic Packet frames
    - Support checking IPv4 header checksum and TCP, UDP, or ICMP 
	checksum encapsulated in IPv4 or IPv6 datagram
    - Support for TCP Segmentation Offload (TSO) and UDP Fragmentation
      Offload (UFO)

* USB 2.0 Host
    - Support two USB2.0 Host
    - Compatible with USB 2.0 specification
    - Supports high-speed(480Mbps), full-speed(12Mbps) and
      low-speed(1.5Mbps) mode
    - Support Enhanced Host Controller Interface Specification (EHCI),
      Revision 1.0
    - Support Open Host Controller Interface Specification (OHCI), Revision 1.0a

* Multi-PHY Interface
    - Support three multi-PHYs with PCIe2.1/SATA3.0/USB3.0/QSGMII controller
    - Up to one USB3 Host controller
    - Up to one USB3 OTG controller
    - Up to one PCIe2.1 controller
    - Up to three SATA controller
    - Up to one QSGMII or SGMII PCS controller
    - Multi-PHY0 support one of the following interfaces
        + USB3.0 OTG
        + SATA0
    - Multi-PHY1 support one of the following interfaces
        + USB3.0 Host
        + SATA1
        + QSGMII/SGMI
    - Multi-PHY2 support one of the following interfaces
        + PCIe2.1
        + SATA2
        + QSGMII/SGMII
    - USB 3.0 xHCI Host Controller
        + Support 1 USB2.0 port and 1 Super-Speed port
        + Concurrent USB3.0/USB2.0 traffic, up to 8.48Gbps bandwidth
        + Support standard or open-source xHCI and class driver
    - USB 3.0 Dual-Role Device (DRD) Controller
        + Static USB3.0 Device
        + Static USB3.0 xHCI host
        + USB3.0/USB2.0 OTG A device and B device basing on ID
    - PCIe2.1 interface
        + Compatible with PCI Express Base Specification Revision 3.0
        + Support Root Complex(RC) mode
        + Support 2.5Gbps and 5.0Gbps serial data transmission rate
		per lane per direction
        + Support one lane
    - SATA interface
        + Compatible with Serial ATA 3.3 and AHCI Revision 1.3.1
        + Support eSATA
        + Support 1.5Gb/s, 3.0Gb/s, 6.0Gb/s
        + Support 3 SATA controller
    - QSGMII/SGMII interface
        + Support one QSGMII, only two GMII controller supported
        + Support SGMII mode with 1000Mbps
* PCIe3.0 PHY Interface
    - Support PCIe3.1(8Gbps) protocol and backward compatible with the 
	PCIe2.1 and PCIe1.1 protocol
    - Support two lane
    - Support two PCIe controller with x1 mode or one PCIe controller
      with x2 mode
    - Two lane PCIe3.0 controller
        + Compatible with PCI Express Base Specification Revision 3.0
        + Dual operation mode: Root Complex(RC)and End Point(EP)
        + Support 2.5Gbps, 5.0Gbps and 8.0Gbps serial data 
		transmission rate per lane per direction
        + Support two lanes
    - One lane PCIe3.0 controller
        + Compatible with PCI Express Base Specification Revision 3.0
        + Support Root Complex(RC) mode
        + Support 2.5Gbbps, 5.0Gbps and 8.0Gbps serial data
		transmission rate per lane per direction
        + Support one lane

* SPI interface
    - Support four SPI Controller
    - Support one chip-select output and the other support two
      chip-select output
    - Support serial-master and serial-slave mode, software-configurable
* I2C interface
    - Support six I2C interface
    - Support 7bits and 10bits address mode
    - Software programmable clock frequency
    - Data on the I2C-bus can be transferred at rates of up to 
	100Kbit/s in the Standard- mode, up to 400Kbit/s in the Fast-mode or 
	up to 1 Mbit/s in Fast-mode Plus.

* UART Controller
    - Support ten UART interfaces
    - Embedded two 64-byte FIFO for TX and RX operation respectively
    - Support 5bits,6bits,7bits,8bits serial data transmit or receive
    - Standard asynchronous communication bits such as start, stop and parity
    - Support different input clock for UART operation to get up to
      4Mbps baud rate
    - Support auto flow control mode for UART0/UART1/UART3/UART4/UART5


#### Misc SOC System Components ####

*  CRU (clock & reset unit)
    - Support clock gating control for individual components
    - One oscillator with 24MHz clock input
    - Support global soft-reset control for whole chip, also
	individual soft-reset for each component
*  MCU
    - 32bits microcontroller core
    - Harvard architecture separate Instruction and Data memories
    - Integrated Programmable Interrupt Controller (IPIC)
    - Integrated Debug Controller with JTAG interface

*  PMU(power management unit)
    - 5 separate voltage domains(VD_CORE/VD_LOGIC/VD_NPU/VD_GPU/VD_PMU)
    - 15 separate power domains, which can be power up/down by 
	software based on different application scenes
    - Multiple configurable work modes to save power by different 
	frequency or automatic clock gating control or power domain on/off 
	control
* Timer
    - Six 64bits timers with interrupt-based operation for non-secure
      application
    - Two 64bits timers with interrupt-based operation for secure application
    - Support two operation modes: free-running and user-defined count
    - Support timer work state checkable
* Watchdog
    - 32bits watchdog counter
    - Counter counts down from a preset value to 0 to indicate the 
	occurrence of a timeout
    - WDT can perform two types of operations when timeout occurs:
        + Generate a system reset
        + First generate an interrupt and if this is not cleared by 
		the service routine by the time a second timeout occurs then generate 
		a system reset
    - Programmable reset pulse length
    - Totally 16 defined-ranges of main timeout period
    - One Watchdog for non-secure application
    - One Watchdog for secure application
* Interrupt Controller
    - Support 3 PPI interrupt sources and 256 SPI interrupt sources input
	from different components
    - Support 16 software-triggered interrupts
    - Two interrupt outputs (nFIQ and nIRQ) separately for each 
	Cortex-A55, both are low-level sensitive
    - Support different interrupt priority for each interrupt source, 
	and they are always software-programmable
*  DMAC
    - Two identical DMAC blocks supported(DMAC0/DMAC1)
    - Micro-code programming based DMA
    - The specific instruction set provides flexibility for
      programming DMA transfers
    - Linked list DMA function is supported to complete scatter-gather transfer
    - Support internal instruction cache
    - Embedded DMA manager thread
    - Support data transfer types with memory-to-memory, memory-to-peripheral,
	peripheral-to-memory
    - Signals the occurrence of various DMA events using the interrupt
      output signals
    - Mapping relationship between each channel and different
	interrupt outputs is software-programmable
    - One embedded DMA controller for system
    - DMAC features:
        + 8 channels totally
        + 32 hardware request from peripherals
        + 2 interrupt outputs



### Board Elements ###

#### Real Time Clock ####
* RK809
* Battery Connection
    - 2 pin jst connector (1.0 mm pitch?)
	- 6 volt
	- Not included with board

#### Data Connections ####

##### Ethernet #####
* Two 1Gbit Ethernet connections
    - via GMAC
	- Fly_CORE
	- FC1503NL	
	    +22197
	- Have PTP hardware timestamping capabilities
	- Billed as compatible with RTL8211F
* Two 2.5Gbit Ethernet connections
    - via PCIe
	- Fly_CORE
	- FC5011
	    +2243
	- Billed as compatible with RTL8125B
	
##### USB #####
* Two USB 3 Type A
* One USB 2 Type A
* One USB 3 Type C

##### Serial/UART #####
* One available external serial console UART
    - 3 pin jst connector, 1.2mm pitch
* Potentially a JTAG connector

##### Wireless #####
* M.2 key for wireless
    - MediaTek chip included
	    - WF-M921E-MPA1
		- Ver. V00
		- Model MT7921
		
	- Two antennas
	
#### External Power ####
* 12 Volt Barrel Connector
* USB C connector
* Includes 12 volt power supply
* Voltage range:  5-24V 
* Recommended power supply:12V-1A DC
* Power consumption: 7.5 watts
* Operating temp: -10 C ~ 55 C


#### LED ####
* Each (4) ethernet port has a green and a yellow LED
* Front panel has:
    - "STA" LED
	- "SSD" LED
	- "ETH Indicator" LED
	
#### Chip Sets ####
* KOWIN 
   - KASC 6311
       + 2225
   - eMMC
* CXMT 
    - CXDB5CCAM
	- DDR SDRAM
	
* 4RT
* 301

   
#### Data Storage ####
* SD Card holder/reader (micro SD)
    - Problematic with high-speed cards
	- Size limit is probably 32 gb (needs verification)
	- Labeled as "Trans Flash (TF)" in documents. Not a usual term in
      North America, but is the same as microSD
	  
* eMMC
    - 32 GB

#### Audio & Video ####
* HDMI
    - 2.0
* Audio jack

#### Other Human Interface (buttons, etc) ####
* Infra-red receiver
* Update recessed button
* Factory Reset recessed button
* Reboot recessed button


##### Misc Board Components #####
* Lighted power switch of uncertain type (spst momentary something)
* Two antenna connections, with two antennas

#### Notable Absences ####
* No external SATA connection
* No external PCIe connection
    - Internal, yes

#### Misc Board Markings ####
* V1_210825
* OML-Factory-3568
* LPDD4
* 2022/09/06
* OPC_H68K_TOP_V2
* OPC_H68K_V2





# References #
* https://opensource.rock-chips.com/images/b/b6/Rockchip_RK3568_Datasheet_V1.3-20220929P.PDF
