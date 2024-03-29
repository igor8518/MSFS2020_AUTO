


#include "PlanesWork.h"
#include "ExQThread.h"
#include "msfs2020_autoflight.h"
#include "mainlogic.h"




//SimData* DataT;
//sAllData AllData;



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    PlanesWork* planesWork = new PlanesWork(0);
    ExQThread* TPlanesWork = new ExQThread("planesWork", NULL);
    planesWork->moveToThread(TPlanesWork);
    TPlanesWork->start();

    MSFS2020_AutoFlight* w = new MSFS2020_AutoFlight(planesWork);
    MainLogic* m = new MainLogic(planesWork, w);
    w->mainlogic = m;
    w->maimThread = new ExQThread("mainThread", m->utils);
    m->moveToThread(w->maimThread);
    w->maimThread->start();

    QObject::connect(w, SIGNAL(Connect()), m, SLOT(Connect()));
    QObject::connect(w, SIGNAL(PMDGSend(DWORD)), m, SLOT(SendDataPMDG(DWORD)));
    QObject::connect(w, SIGNAL(StartSend()), m, SLOT(StartStopSim()));
    w->setWindowTitle("MSFS 2020 AutoFlight");
    qApp->setStyle(QStyleFactory::create("Fusion"));
    w->show();
    return a.exec();
}
