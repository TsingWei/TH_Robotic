#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <QStringList>

typedef struct
{
    double position;
    double speed;
    double current;
} CanAnalysis;

enum {
    PROTOCOL_TYPE_POS=1, PROTOCOL_TYPE_SYNC_POS, PROTOCOL_TYPE_INTER_POS,
    PROTOCOL_TYPE_SPD, PROTOCOL_TYPE_CUR, PROTOCOL_TYPE_START,
    PROTOCOL_TYPE_OPENVALVE, PROTOCOL_TYPE_CLOSEVALVE,
    PROTOCOL_TYPE_CAL, PROTOCOL_TYPE_SHUTDOWN, PROTOCOL_TYPE_PAUSE,
    PROTOCOL_TYPE_SPD_SET, PROTOCOL_TYPE_POS_RF,
    PROTOCOL_TYPE_WHELL_RESET, PROTOCOL_TYPE_WHELL_SELECT,
    PROTOCOL_TYPE_WHELL_SPD, PROTOCOL_TYPE_WHELL_REF, PROTOCOL_TYPE_SPD_PAW
};

#define NONE_CONNECT            0
#define CONNECT_TYPE_ALYSIST    1
#define CONNECT_TYPE_GC         2

#define NODE_NUM 12
#define DRIVE_NEW 1
#define SIMULATE_CONNECT        /*NONE_CONNECT//*/CONNECT_TYPE_ALYSIST

class GlobalData
{
private:
    GlobalData();

public:
    static CanAnalysis currentCanAnalyticalData[NODE_NUM];//12+2
    static int connectType;
    static QStringList currentCanData;
    static bool showDebugInfo;
    static unsigned int sendId[NODE_NUM];
    static int runningId[NODE_NUM];
    static int statusId[NODE_NUM];

    static void init();
};

#endif // GLOBALDATA_H
