#ifndef MEASSERVERMAP_H
#define MEASSERVERMAP_H

#include <QtGlobal>

const qint16 REG_FACTORY_CODE1    = 0x0001;
const qint16 REG_FACTORY_CODE2    = 0x0002;
const qint16 REG_PRODUCT_CODE     = 0x0003;
const qint16 REG_DETECTS_COUNT    = 0x0010;
const qint16 REG_LIST_ID          = 0x0100;
const qint16 REG_CURRENT_MEAS     = 0x0200;
const qint16 REG_TYPE_DETECT      = 0x0206;
const qint16 REG_DATE_MANUF       = REG_TYPE_DETECT + 2;
const qint16 REG_YEAR_MANUF       = REG_DATE_MANUF + 1;
const qint16 REG_INTERVAL         = REG_YEAR_MANUF + 1;
const qint16 REG_SET_ZERO         = REG_INTERVAL + 1;
const qint16 REG_PRESET           = REG_SET_ZERO + 1;
const qint16 REG_UNIT_DETECT      = 0x0210;
const qint16 REG_NAME_DETECT      = 0x0220;

const qint16 LEN_DATA_DETECT      = 0x30;
const qint16 LEN_STEP_DETECT      = 128;
const qint16 LEN_UNIT_DETECT      = 8;
const qint16 LEN_NAME_DETECT      = 16;

const qint16 FACTORY_CODE1        = 0x6D25;
const qint16 FACTORY_CODE2        = 0xB177;
const qint16 PRODUCT_CODE         = 0x0001;

const qint16 MAX_COUNT_DETECT     = 127;

#endif // MEASSERVERMAP_H
