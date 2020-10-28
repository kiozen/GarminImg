/**********************************************************************************************
    Copyright (C) 2013 Oliver Eichler oliver.eichler@gmx.de

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

#include "argv.h"
#include "CEncoder.h"
#include "CMap.h"

#include <gdal_priv.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
    CMap map;
    CEncoder encoder("test.img");

    int quality         = -1;
    int subsampling     = -1;

    const char *copyright = "There should be a copy right";
    const char *mapname   = "QLandkarte Test Map first try to create ratser map in gmapsupp";

    char * pCopyright   = 0;
    char * pMapname     = 0;

    int skip_next_arg = 0;

    GDALAllRegister();

    for(int i = 1; i < (argc - 1); i++)
    {
        if (skip_next_arg)
        {
            skip_next_arg = 0;
            continue;
        }

        if (argv[i][0] == '-')
        {
            if (towupper(argv[i][1]) == 'Q')
            {
                quality = atol(argv[i + 1]);
                skip_next_arg = 1;
                continue;
            }
            else if (towupper(argv[i][1]) == 'S')
            {
                subsampling = atol(argv[i + 1]);
                skip_next_arg = 1;
                continue;
            }
            else if (towupper(argv[i][1]) == 'C')
            {
                copyright = pCopyright = get_argv(i + 1, argv);
                skip_next_arg = 1;
                continue;
            }
            else if (towupper(argv[i][1]) == 'N')
            {
                mapname = pMapname = get_argv(i + 1, argv);
                skip_next_arg = 1;
                continue;
            }
        }

        map.addMapSource(argv[i]);
    }

    map.setName(mapname);
    map.split();

    encoder.start(map);

    GDALDestroyDriverManager();
    if(pCopyright)
    {
        free(pCopyright);
    }
    if(pMapname)
    {
        free(pMapname);
    }

    return 0;
}
