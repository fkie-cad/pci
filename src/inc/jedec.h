#pragma once

// found in uefiTool/common/descriptor.cpp

#define JDEC_VENDOR_WINBOND     (0xEF)
#define JDEC_VENDOR_MACRONIX    (0xC2)
#define JDEC_VENDOR_GIGA_DEVICE (0xC8)

#define JDEC_DEVICE_W25Q32JV      (0x1670EF)
#define JDEC_DEVICE_W25Q64FV_SPI  (0x1740EF)
#define JDEC_DEVICE_W25Q64FV_QSPI (0x1760EF)
#define JDEC_DEVICE_W25Q128_SPI   (0x1840EF)
#define JDEC_DEVICE_W25Q128_QSPI  (0x1860EF)
#define JDEC_DEVICE_W25Q256       (0x1940EF)

#define JDEC_DEVICE_MX25U16     (0x1620C2)
#define JDEC_DEVICE_MX25L6408   (0x1720C2)
#define JDEC_DEVICE_MX25L12805  (0x1820C2)
#define JDEC_DEVICE_MX_UNKNWN_0 (0x1975C2)

#define JDEC_DEVICE_GD_UNKNWN_0    (0x1840C8)
#define JDEC_DEVICE_GD25X256C   (0x1940C8)

FORCEINLINE 
PCHAR jdecIdToString(UINT32 Id)
{
    switch ( Id )
    {
        //case JDEC_DEVICE_W25Q128_SPI: return "Winbond W25Q128 SPI";
        //case JDEC_DEVICE_W25Q128_QSPI: return "Winbond W25Q128 QSPI";
        //case JDEC_DEVICE_W25Q64FV_SPI: return "Winbond W25Q64FV SPI";
        //case JDEC_DEVICE_W25Q64FV_QSPI: return "Winbond W25Q64FV QSPI";
        //case JDEC_DEVICE_W25Q32JV: return "Winbond W25Q32JV";
        //case JDEC_DEVICE_W25Q256: return "Winbond W25Q256";
        
        //case JDEC_DEVICE_MX25U16: return "Macronix MX25U16";
        //case JDEC_DEVICE_MX25L6408: return "Macronix MX25L6408";
        //case JDEC_DEVICE_MX25L12805: return "Macronix MX25L12805";
        
        //case JDEC_DEVICE_GD25X256C: return "GigaDevice GD25x256C";
        
        // Winbond
        case 0x1030EF: return "Winbond W25X05";
        case 0x1130EF: return "Winbond W25X10";
        case 0x1230EF: return "Winbond W25X20";
        case 0x1330EF: return "Winbond W25X40";
        case 0x1430EF: return "Winbond W25X80";
        case 0x1530EF: return "Winbond W25X16";
        case 0x1630EF: return "Winbond W25X32";
        case 0x1730EF: return "Winbond W25X64";
        case 0x1240EF: return "Winbond W25Q20";
        case 0x1340EF: return "Winbond W25Q40";
        case 0x1440EF: return "Winbond W25Q80";
        case 0x1540EF: return "Winbond W25Q16";
        case 0x1640EF: return "Winbond W25Q32";
        case 0x1740EF: return "Winbond W25Q64 FV SPI";
        case 0x1840EF: return "Winbond W25Q128 SPI";
        case 0x1940EF: return "Winbond W25Q256";
        case 0x1160EF: return "Winbond W25Q10";
        case 0x1260EF: return "Winbond W25Q20";
        case 0x1360EF: return "Winbond W25Q40";
        case 0x1460EF: return "Winbond W25Q80";
        case 0x1560EF: return "Winbond W25Q16";
        case 0x1660EF: return "Winbond W25Q32";
        case 0x1760EF: return "Winbond W25Q64 FV SPI";
        case 0x1860EF: return "Winbond W25Q128 QSPI";
        case 0x1960EF: return "Winbond W25Q256";
        case 0x1670EF: return "Winbond W25Q32 JV";
        case 0x1871EF: return "Winbond W25M256";
        case 0x1971EF: return "Winbond W25M512";

        // Macronix
        case 0x1320C2: return "Macronix MX25L40";
        case 0x1420C2: return "Macronix MX25L80";
        case 0x1520C2: return "Macronix MX25L16";
        case 0x1620C2: return "Macronix MX25U16";
        case 0x1720C2: return "Macronix MX25L64 08";
        case 0x1820C2: return "Macronix MX25L128 05";
        case 0x1920C2: return "Macronix MX25L256";
        case 0x1A20C2: return "Macronix MX66L512";
        case 0x1524C2: return "Macronix MX25L16";
        case 0x1525C2: return "Macronix MX25L16";
        case 0x3425C2: return "Macronix MX25U80";
        case 0x3525C2: return "Macronix MX25U16";
        case 0x3625C2: return "Macronix MX25U32";
        case 0x3725C2: return "Macronix MX25U64";
        case 0x3825C2: return "Macronix MX25U128";
        case 0x3925C2: return "Macronix MX25U256";
        case 0x3A25C2: return "Macronix MX25U512";
        case 0x1726C2: return "Macronix MX25L64";
        case 0x1826C2: return "Macronix MX25L128";
        case 0x165EC2: return "Macronix MX25L32";
        case 0x1795C2: return "Macronix MX25L64";

        // Micron
        case 0x142020: return "Micron M25P80";
        case 0x152020: return "Micron M25P16";
        case 0x162020: return "Micron M25P32";
        case 0x172020: return "Micron M25P64";
        case 0x182020: return "Micron M25P128";
        case 0x114020: return "Micron M45PE10";
        case 0x124020: return "Micron M45PE20";
        case 0x134020: return "Micron M45PE40";
        case 0x144020: return "Micron M45PE80";
        case 0x154020: return "Micron M45PE16";
        case 0x174020: return "Micron XM25QH64C";
        case 0x184020: return "Micron XM25QH128C";
        case 0x194020: return "Micron XM25QH256C";
        case 0x184320: return "Micron XM25RH128C";
        case 0x147120: return "Micron M25PX80";
        case 0x157120: return "Micron M25PX16";
        case 0x167120: return "Micron M25PX32";
        case 0x177120: return "Micron M25PX64";
        case 0x118020: return "Micron M25PE10";
        case 0x128020: return "Micron M25PE20";
        case 0x138020: return "Micron M25PE40";
        case 0x148020: return "Micron M25PE80";
        case 0x158020: return "Micron M25PE16";
        case 0x15BA20: return "Micron N25Q016";
        case 0x16BA20: return "Micron N25Q032";
        case 0x17BA20: return "Micron N25Q064";
        case 0x18BA20: return "Micron N25Q128";
        case 0x19BA20: return "Micron N25Q256";
        case 0x20BA20: return "Micron N25Q512";
        case 0x21BA20: return "Micron N25Q00A";
        case 0x15BB20: return "Micron N25Q016";
        case 0x16BB20: return "Micron N25Q032";
        case 0x17BB20: return "Micron N25Q064";
        case 0x18BB20: return "Micron MT25Q128";
        case 0x19BB20: return "Micron MT25Q256";
        case 0x20BB20: return "Micron MT25Q512";

        // Intel
        case 0x118989: return "Intel 25F160S33B8";
        case 0x128989: return "Intel 25F320S33B8";
        case 0x138989: return "Intel 25F640S33B8";
        case 0x158989: return "Intel 25F160S33T8";
        case 0x168989: return "Intel 25F320S33T8";
        case 0x178989: return "Intel 25F640S33T8";

        // Atmel / Adesto
        case 0x17321F: return "Atmel AT25SF641";
        case 0x16421F: return "Atmel AT25SL321";
        case 0x18421F: return "Atmel AT25SL128A";
        case 0x17431F: return "Atmel AT25SL641";
        case 0x00451F: return "Atmel AT26DF081";
        case 0x01451F: return "Atmel AT26DF081A";
        case 0x02451F: return "Atmel AT25DF081";
        case 0x00461F: return "Atmel AT26DF161";
        case 0x01461F: return "Atmel AT26DF161A";
        case 0x02461F: return "Atmel AT25DF161";
        case 0x00471F: return "Atmel AT25DF321";
        case 0x01471F: return "Atmel AT25DF321A";
        case 0x00481F: return "Atmel AT25DF641";
        case 0x18701F: return "Atmel AT25QF128";
        case 0x00861F: return "Atmel AT25DQ161";
        case 0x00881F: return "Atmel AT25DQ641";

        // Microchip
        case 0x4125BF: return "Microchip SST25VF016B";
        case 0x4A25BF: return "Microchip SST25VF032B";
        case 0x8D25BF: return "Microchip SST25VF040B";
        case 0x8E25BF: return "Microchip SST25VF080B";
        case 0x4B25BF: return "Microchip SST25VF064C";

        // EON / ESMT
        case 0x13301C: return "EON EN25Q40";
        case 0x14301C: return "EON EN25Q80";
        case 0x15301C: return "EON EN25Q16";
        case 0x16301C: return "EON EN25Q32";
        case 0x17301C: return "EON EN25Q64";
        case 0x18301C: return "EON EN25Q128";
        case 0x14311C: return "EON EN25F80";
        case 0x15311C: return "EON EN25F16";
        case 0x16311C: return "EON EN25F32";
        case 0x17311C: return "EON EN25F64";
        case 0x11381C: return "EON EN25S10";
        case 0x12381C: return "EON EN25S20";
        case 0x13381C: return "EON EN25S40";
        case 0x14381C: return "EON EN25S80";
        case 0x15381C: return "EON EN25S16";
        case 0x16381C: return "EON EN25S32";
        case 0x17381C: return "EON EN25S64";
        case 0x14701C: return "EON EN25QH80";
        case 0x15701C: return "EON EN25QH16";
        case 0x16701C: return "EON EN25QH32";
        case 0x17701C: return "EON EN25QH64";
        case 0x18701C: return "EON EN25QH128";
        case 0x19701C: return "EON EN25QH256";

        // GigaDevice
        case 0x1440C8: return "GigaDevice GD25x80";
        case 0x1540C8: return "GigaDevice GD25x16";
        case 0x1640C8: return "GigaDevice GD25x32";
        case 0x1740C8: return "GigaDevice GD25x64";
        case 0x1840C8: return "GigaDevice GD25x128";
        case 0x1940C8: return "GigaDevice GD25x256C";
        case 0x1560C8: return "GigaDevice GD25LQ16V";
        case 0x1760C8: return "GigaDevice GD25Lx64";
        case 0x1860C8: return "GigaDevice GD25Lx128";
        case 0x1960C8: return "GigaDevice GD25LQ256C";

        // Fidelix
        case 0x1532F8: return "Fidelix FM25Q16";
        case 0x1632F8: return "Fidelix FM25Q32";
        case 0x1732F8: return "Fidelix FM25Q64";
        case 0x1832F8: return "Fidelix FM25Q128";

        // Spansion
        case 0x154001: return "Spansion S25FL116K";
        case 0x164001: return "Spansion S25FL132K";
        case 0x174001: return "Spansion S25FL164K";

        // AMIC Technology
        case 0x153037: return "AMIC A25L016";
        case 0x163037: return "AMIC A25L032";
        case 0x154037: return "AMIC A25LQ16";
        case 0x164037: return "AMIC A25LQ32A";

        // PMC
        case 0x13F79D: return "PMC Pm25LV080B";
        case 0x14F79D: return "PMC Pm25LV016B";
        case 0x44F79D: return "PMC Pm25LQ080C";
        case 0x45F79D: return "PMC Pm25LQ016C";
        case 0x46F79D: return "PMC Pm25LQ032C";
        case 0x7BF79D: return "PMC Pm25LV512A";
        case 0x7CF79D: return "PMC Pm25LV010A";
        case 0x7DF79D: return "PMC Pm25LV020";
        case 0x7EF79D: return "PMC Pm25LV040";

        // ISSI
        case 0x14609D: return "ISSI Ix25LP080";
        case 0x15609D: return "ISSI Ix25LP016";
        case 0x16609D: return "ISSI Ix25LP032";
        case 0x17609D: return "ISSI Ix25LP064";
        case 0x18609D: return "ISSI Ix25LP128";
        case 0x19609D: return "ISSI Ix25LP256";
        case 0x14709D: return "ISSI Ix25WP080";
        case 0x15709D: return "ISSI Ix25WP016";
        case 0x16709D: return "ISSI Ix25WP032";
        case 0x17709D: return "ISSI Ix25WP064";
        case 0x18709D: return "ISSI Ix25WP128";
        case 0x19709D: return "ISSI Ix25WP256";

        default: return "Unknown";
    }
}
