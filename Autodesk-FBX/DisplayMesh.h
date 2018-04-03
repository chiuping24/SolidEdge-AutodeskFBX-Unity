/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#ifndef _DISPLAY_MESH_H
#define _DISPLAY_MESH_H

#include "DisplayCommon.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;

void DisplayMesh(FbxNode* pNode, vector<vector<string>> NullName, int NextGroup, int nLayer, int NextLayer);

#endif // #ifndef _DISPLAY_MESH_H


