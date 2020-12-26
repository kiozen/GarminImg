/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#include "CException.h"
#include "CGarminEncoder.h"
#include "version.h"
#include <gdal_priv.h>
#include <iostream>
#include <QtCore>

QCommandLineParser allArgs;

int main(int argc, char ** argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(_MKSTR(APPLICATION_NAME));
    QCoreApplication::setApplicationVersion(VER_STR);

    allArgs.setApplicationDescription(""
                                      "------------------ " WHAT_STR " ------------------\n"
                                      "Encode a raster map into a Garmin IMG file.\n"
                                      "");
    QCommandLineOption helpOption = allArgs.addHelpOption();
    QCommandLineOption verOption = allArgs.addVersionOption();


    allArgs.addOptions({
        {
            "j", "Number of threads to use. Default MAX_CORE", "numThreads", "-1"
        },
        {
            "tmpdir", "Temporary working path for intermediate files: Default: " + QDir::tempPath(), "path", QDir::tempPath()
        },
        {
            "keep", "Keep temporary data in " + QDir::tempPath()
        },
    });

    allArgs.addPositionalArgument("path-to-file", "A raster map file to encode.");

    if (!allArgs.parse(app.arguments()))
    {
        std::cerr << allArgs.errorText().toUtf8().constData();
        std::cerr << allArgs.helpText().toUtf8().constData();
        exit(-1);
    }

    if (allArgs.isSet(helpOption) || allArgs.isSet(verOption))
    {
        std::cout << allArgs.helpText().toUtf8().constData();
        exit(0);
    }

    QStringList sourcefiles = allArgs.positionalArguments();

    if(sourcefiles.count() != 1)
    {
        std::cout << allArgs.helpText().toUtf8().constData();
        exit(-1);
    }

    int result = 0;
    GDALAllRegister();
    try
    {
        CGarminEncoder encoder(sourcefiles[0]);
        encoder.splitIntoTiles();
        encoder.createImage("test.img", "Taufers test map");
    }
    catch(const CException& e)
    {
        std::cerr << QString(e).toUtf8().constData();
        result = -1;
    }

    printf("\n\n\n");
    GDALDestroyDriverManager();
    return result;
}

