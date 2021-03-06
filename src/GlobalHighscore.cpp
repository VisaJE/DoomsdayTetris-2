#include <iostream>
#include "GlobalHighscore.h"
#include "SqlConn.h"
#include "WebConf.h"
#include <unistd.h>
#include "Highscorer.h"
#include "UniqueIdentifier.h"
#include "Log.h"

using namespace tet;
GlobalHighscore::GlobalHighscore(Highscorer *context): context(context)
{
}

bool GlobalHighscore::updateData()
{
    configuration = WebConf();
    configuration.initiate();
    if (!configuration.serviceEnabled)
    {
        names[0] = "Online";
        names[1] = "Services";
        names[2] = "Are turned";
        names[3] = "Off!";
        names[5] = "The server";
        names[6] = "_config";
        names[7] = "File not";
        names[8] = "configured";
        return false;
    }
    SqlConn connection = SqlConn(configuration.configuration);
    int tscores[10];
    std::string tnames[10];
    Uid ids[10];
    context->getHighscore(tnames, tscores, ids);

    connection.pushList(tnames, tscores, ids);

    connection.topList(names, scores);

    return true;
}

GlobalHighscore::~GlobalHighscore()
{
}

