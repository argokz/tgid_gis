#include <QApplication>
#include "hasp_hl.h"

QString getHaspErrorMessage(int errorCode) {
    switch (errorCode) {
        case HASP_STATUS_OK                     /* 0 */ : return "Ошибок нет";
        case HASP_MEM_RANGE                     /* 1 */ : return "Неверный адрес памяти";
        case HASP_INV_PROGNUM_OPT               /* 2 */ : return "Неизвестный/неверный идентификатор функции";
        case HASP_INSUF_MEM                     /* 3 */ : return "Не удалось выделить память";
        case HASP_TMOF                          /* 4 */ : return "Слишком много открытых функций";
        case HASP_ACCESS_DENIED                 /* 5 */ : return "Доступ к функции запрещен";
        case HASP_INCOMPAT_FEATURE              /* 6 */ : return "Несовместимая функция";
        case HASP_CONTAINER_NOT_FOUND           /* 7 */ : return "Лицензионный контейнер не найден";
        case HASP_TOO_SHORT                     /* 8 */ : return "Длина для шифрования/дешифрования слишком короткая";
        case HASP_INV_HND                       /* 9 */ : return "Неверный дескриптор";
        case HASP_INV_FILEID                   /* 10 */ : return "Неверный идентификатор файла/дескриптор памяти";
        case HASP_OLD_DRIVER                   /* 11 */ : return "Версия драйвера или демона поддержки устарела";
        case HASP_NO_TIME                      /* 12 */ : return "Поддержка реального времени недоступна";
        case HASP_SYS_ERR                      /* 13 */ : return "Общая ошибка системного вызова хоста";
        case HASP_NO_DRIVER                    /* 14 */ : return "Драйвер ключа не найден";
        case HASP_INV_FORMAT                   /* 15 */ : return "Нераспознаваемый формат информации";
        case HASP_REQ_NOT_SUPP                 /* 16 */ : return "Запрос не поддерживается";
        case HASP_INV_UPDATE_OBJ               /* 17 */ : return "Неверный объект обновления";
        case HASP_KEYID_NOT_FOUND              /* 18 */ : return "Ключ с запрашиваемым идентификатором не найден";
        case HASP_INV_UPDATE_DATA              /* 19 */ : return "Проверка согласованности данных обновления не удалась";
        case HASP_INV_UPDATE_NOTSUPP           /* 20 */ : return "Обновление не поддерживается этим ключом";
        case HASP_INV_UPDATE_CNTR              /* 21 */ : return "Несоответствие счетчика обновления";
        case HASP_INV_VCODE                    /* 22 */ : return "Неверный код поставщика";
        case HASP_ENC_NOT_SUPP                 /* 23 */ : return "Запрошенный алгоритм шифрования не поддерживается";
        case HASP_INV_TIME                     /* 24 */ : return "Неверная дата/время";
        case HASP_NO_BATTERY_POWER             /* 25 */ : return "Часы не имеют питания";
        case HASP_NO_ACK_SPACE                 /* 26 */ : return "Требуется подтверждение обновления, но область для возврата его отсутствует";
        case HASP_TS_DETECTED                  /* 27 */ : return "Обнаруженные службы терминалов (удаленный терминал)";
        case HASP_FEATURE_TYPE_NOT_IMPL        /* 28 */ : return "Тип функции не реализован";
        case HASP_UNKNOWN_ALG                  /* 29 */ : return "Неизвестный алгоритм";
        case HASP_INV_SIG                      /* 30 */ : return "Проверка подписи не удалась";
        case HASP_FEATURE_NOT_FOUND            /* 31 */ : return "Функция не найдена";
        case HASP_NO_LOG                       /* 32 */ : return "Логирование не включено";
        case HASP_INVALID_OBJECT              /* 500 */ : return "Неверный объект";
        case HASP_INVALID_PARAMETER           /* 501 */ : return "Неверный параметр";
        case HASP_ALREADY_LOGGED_IN           /* 502 */ : return "Уже авторизован";
        case HASP_ALREADY_LOGGED_OUT          /* 503 */ : return "Еще не авторизован";
        case HASP_OPERATION_FAILED            /* 525 */ : return "Операция не выполнена";
        case HASP_NO_EXTBLOCK                 /* 600 */ : return "Недоступен классический блок расширения памяти";
        case HASP_INV_PORT_TYPE               /* 650 */ : return "Неверный тип порта";
        case HASP_INV_PORT                    /* 651 */ : return "Неверное значение порта";
        case HASP_NOT_IMPL                    /* 698 */ : return "Возможность недоступна";
        case HASP_INT_ERR                     /* 699 */ : return "Внутренняя ошибка API";
        default: return "Неизвестный код ошибки";
    }
}
