#include "global_res.h"

#include <QListWidget>
#include <QModbusClient>

const QStringList dev_typeList = { "iTU-11.GW", "iTU-11.6.16", "iTU-11.12.16", "iTU-11.MP.4","iTU-11.6.16D","iTU-11.12.16D" };

static QModbusClient *modbusDevice = nullptr;

QModbusClient* getModbusClient()
{
    return modbusDevice;
}

void setModbusClient(QModbusClient* client)
{
    modbusDevice = client;
}

void destroyModbusClient()
{
    delete modbusDevice;
    modbusDevice = nullptr;
}

uint32_t getListMask(QListWidget *list)
{
    uint32_t rval = 0;
    int rowCount = list->count();

    for(int i = 0; i< rowCount;i++)
    {
       if(Qt::Checked == list->item(i)->checkState())
       {
           rval = (1<< i)|rval;
       }
    }
    return rval;
}

void setListMask(QListWidget *list,uint32_t mask)
{
    int rowCount = list->count();
    for(int i = 0; i<rowCount;i++)
    {
        if((1<<i)&mask)
        {
            list->item(i)->setCheckState(Qt::Checked);
        }
        else
            list->item(i)->setCheckState(Qt::Unchecked);
    }
}

void setMaskBits(uint8_t *bytebuf, uint16_t offset, uint8_t numberOfBits, uint8_t value)
{
    uint16_t          usWordBuf;
    uint16_t          usMask;
    uint16_t          usByteOffset;
    uint16_t          usNPreBits;
    uint16_t          usValue = value;

    assert( numberOfBits <= 8 );
    assert( ( size_t )BITS_uint8_t == sizeof( uint8_t ) * 8 );

    /* Calculate byte offset for first byte containing the bit values starting
     * at usBitOffset. */
    usByteOffset = ( uint16_t )( ( offset ) / BITS_uint8_t );

    /* How many bits precede our bits to set. */
    usNPreBits = ( uint16_t )( offset - usByteOffset * BITS_uint8_t );

    /* Move bit field into position over bits to set */
    usValue <<= usNPreBits;

    /* Prepare a mask for setting the new bits. */
    usMask = ( uint16_t )( ( 1 << ( uint16_t ) numberOfBits ) - 1 );
    usMask <<= offset - usByteOffset * BITS_uint8_t;

    /* copy bits into temporary storage. */
    usWordBuf = bytebuf[usByteOffset];
    usWordBuf |= bytebuf[usByteOffset + 1] << BITS_uint8_t;

    /* Zero out bit field bits and then or value bits into them. */
    usWordBuf = ( uint16_t )( ( usWordBuf & ( ~usMask ) ) | usValue );

    /* move bits back into storage */
    bytebuf[usByteOffset] = ( uint8_t )( usWordBuf & 0xFF );
    bytebuf[usByteOffset + 1] = ( uint8_t )( usWordBuf >> BITS_uint8_t );
}
