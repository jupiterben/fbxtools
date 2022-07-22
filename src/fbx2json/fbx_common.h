/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/
#pragma once
#include <fbxsdk.h>

void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);

bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int fileVersion, int pFileFormat=-1, bool pEmbedMedia=false);
bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int& fileVersion);

// to get a string from the node name and attribute type
FbxString GetNodeNameAndAttributeTypeName(const FbxNode *pNode);

// to get a string from the node default translation values
FbxString GetDefaultTranslationInfo(const FbxNode *pNode);

// to get a string from the node visibility value
FbxString GetNodeVisibility(const FbxNode *pNode);
