
/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/////////////////////////////////////////////////////////////////////////
//
// This program converts any file in a format supported by the FBX SDK
// into DAE, FBX, 3DS, OBJ and DXF files.
//
// Steps:
// 1. Initialize SDK objects.
// 2. Load a file(fbx, obj,...) to a FBX scene.
// 3. Create a exporter.
// 4. Retrieve the writer ID according to the description of file format.
// 5. Initialize exporter with specified file format
// 6. Export.
// 7. Destroy the exporter
// 8. Destroy the FBX SDK manager
//
/////////////////////////////////////////////////////////////////////////

#include "./common/ImportExport.h"

const char *lFileTypes[] =
    {
        // "_dae.dae", "Collada DAE (*.dae)",
        // "_fbx7binary.fbx", "FBX binary (*.fbx)",
        "_ascii.fbx", "FBX ascii (*.fbx)",
        // "_fbx6binary.fbx", "FBX 6.0 binary (*.fbx)",
        // "_fbx6ascii.fbx", "FBX 6.0 ascii (*.fbx)",
        // "_obj.obj", "Alias OBJ (*.obj)",
        // "_dxf.dxf", "AutoCAD DXF (*.dxf)"
        };

int main(int argc, char **argv)
{
    FbxString lFilePath("");
    for (int i = 1, c = argc; i < c; ++i)
    {
        if (FbxString(argv[i]) == "-test")
            continue;
        else if (lFilePath.IsEmpty())
            lFilePath = argv[i];
    }
    if (lFilePath.IsEmpty())
    {
        FBXSDK_printf("\n\nUsage: ConvertScene <FBX file name>\n\n");
        return 0;
    }

    
    return 0;
}
