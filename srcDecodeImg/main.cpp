/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler oliver.eichler@gmx.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/

#include "CException.h"
#include "CFuzzyMap.h"
#include "CGarminDecoder.h"
#include "version.h"
#include <gdal_priv.h>
#include <iostream>
#include <QtCore>

QCommandLineParser allArgs;
double MIN_DELTA = 0.00001;

int main(int argc, char ** argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(_MKSTR(APPLICATION_NAME));
    QCoreApplication::setApplicationVersion(VER_STR);

    allArgs.setApplicationDescription(""
                                      "------------------ " WHAT_STR " ------------------\n"
                                      "Export the raster map part from Garmin IMG files.\n"
                                      "");
    QCommandLineOption helpOption = allArgs.addHelpOption();
    QCommandLineOption verOption = allArgs.addVersionOption();
    allArgs.addOptions({
        {
            "j", "Number of threads to use. Default MAX_CORE", "numThreads", "-1"
        },
        {
            "d-scale", "Max. scale difference per scale level. Default: 0.00001 ", "scale", "0.00001"
        },
        {
            "dry-run", "Dry-run. Read all information but do not export the map."
        },
        {
            "allow-empty", "Allow the export of blank tiles with only a single color for all pixel."
        },
    });

    allArgs.addPositionalArgument("path-to-file", "A Garmin IMG file to decode.");

    if (!allArgs.parse(app.arguments()))
    {
        std::cerr << allArgs.errorText().toUtf8().constData();
        std::cerr << allArgs.helpText().toUtf8().constData();
        exit(1);
    }

    if (allArgs.isSet(helpOption) || allArgs.isSet(verOption))
    {
        std::cout << allArgs.helpText().toUtf8().constData();
        exit(0);
    }

    MIN_DELTA = allArgs.value("d-scale").toDouble();

    QStringList gmapsupp = allArgs.positionalArguments();

    if(gmapsupp.count() == 0)
    {
        std::cout << allArgs.helpText().toUtf8().constData();
        exit(-1);
    }

    int result = 0;
    GDALAllRegister();
    try
    {
        CGarminDecoder decoder(gmapsupp[0]);
        decoder.loadBasics();
//        decoder.dumpSubfiles();
        decoder.analyse();
    }
    catch(const CException& e)
    {
        fflush(stdout);
        fflush(stderr);
        std::cerr << std::endl << "Kaboom!" << std::endl << QString(e).toUtf8().constData();
        result = -1;
    }

    printf("\n\n\n");
    GDALDestroyDriverManager();
    return result;
}
