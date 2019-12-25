#include "documenthandler.h"

#include "globalsettings.h"

DocumentHandler::DocumentHandler(){
    connect(GlobalSettings::instance(),SIGNAL(clearOutput()),this,SIGNAL(clearText()));
    connect(GlobalSettings::instance(),SIGNAL(addOutput(const QString)),this,SIGNAL(addText(const QString)));
}
