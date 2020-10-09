#ifndef GLOBAL_RES_H
#define GLOBAL_RES_H

#include <stdint.h>
/**
 * @brief Deletes object and sets the pointer to null.
 *
 * Deletes object under given pointer, but only if the pointer is not null.
 * Also sets the pointer to the null after deleting is done.
 */
#define safe_delete(var) \
    if (var) \
    { \
        delete var; \
        var = nullptr; \
    }

#define qobject_safe_delete(var) \
    if (var) \
    { \
        var->deleteLater(); \
        var = nullptr; \
    }


#define DECLARE_SINGLETON(Cls) \
    public: \
        static Cls* getInstance(); \
        static void destroy(); \
    \
    private: \
        static Cls* _instance;

#define DEFINE_SINGLETON(Cls) \
    Cls* Cls::_instance = nullptr; \
    \
    Cls* Cls::getInstance() \
    { \
        if (!_instance) \
            _instance = new Cls(); \
    \
        return _instance; \
    } \
    \
    void Cls::destroy() \
    { \
        safe_delete(_instance); \
    }


#define BITS_uint8_t      8U



class QListWidget;
class QModbusClient;
class QStringList;

extern const QStringList dev_typeList;

uint32_t getListMask(QListWidget *list);
void setListMask(QListWidget *list,uint32_t mask);
void setMaskBits(uint8_t* bytebuf,uint16_t offset,uint8_t numberOfBits,uint8_t value);

QModbusClient* getModbusClient();
void setModbusClient(QModbusClient* client);
void destroyModbusClient();

#endif // GLOBAL_RES_H
