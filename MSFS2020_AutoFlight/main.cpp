


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
    w->maimThread = new ExQThread("mainThread", m->utils);
    m->moveToThread(w->maimThread);
    w->maimThread->start();

    QObject::connect(w, SIGNAL(Connect()), m, SLOT(Connect()));
    w->setWindowTitle("MSFS 2020 AutoFlight");
    w->setStyle(QStyleFactory::create("Fusion"));
    w->show();
    return a.exec();
}
