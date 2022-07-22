/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#include "ImportExport.h"
#include <sstream>

// declare global
FbxManager*   gSdkManager = NULL;

#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(gSdkManager->GetIOSettings()))
#endif


// a UI file provide a function to print messages
extern void UI_Printf(const char* msg, ...);
void UI_Printf(const char *msg, ...)
{

}

static bool ConvertColorToTangent(FbxScene* pScene);

// to read and write a file using the FBXSDK readers/writers
//
// const char *ImportFileName : the full path of the file to be read
// const char* ExportFileName : the full path of the file to be written
// int pWriteFileFormat       : the specific file format number
//                                  for the writer

void ImportExport(
                  const char *ImportFileName,
                  const char* ExportFileName,
                  int pWriteFileFormat
                  )
{
	// Create a scene
	FbxScene* lScene = FbxScene::Create(gSdkManager,"");

    UI_Printf("------- Import started ---------------------------");

    // Load the scene.
    bool r = LoadScene(gSdkManager, lScene, ImportFileName);
    if(r)
        UI_Printf("------- Import succeeded -------------------------");
    else
    {
        UI_Printf("------- Import failed ----------------------------");

        // Destroy the scene
		lScene->Destroy();
        return;
    }


	UI_Printf("------- Convert color to tangent started -------------------------");
	
    UI_Printf("\r\n"); // add a blank line
	if(r) UI_Printf("------- Convert succeeded -------------------------");
    else  UI_Printf("------- Convert failed!!! ----------------------------");
	if (!r) {
		lScene->Destroy();
		return;
	}

    UI_Printf("------- Export started ---------------------------");

    // Save the scene.
    r = SaveScene(gSdkManager, 
        lScene,               // to export this scene...
        ExportFileName,       // to this path/filename...
        pWriteFileFormat,     // using this file format.
        false);               // Don't embed media files, if any.

    if(r) UI_Printf("------- Export succeeded -------------------------");
    else  UI_Printf("------- Export failed ----------------------------");

	// destroy the scene
	lScene->Destroy();
}

// Creates an instance of the SDK manager.
void InitializeSdkManager()
{
    // Create the FBX SDK memory manager object.
    // The SDK Manager allocates and frees memory
    // for almost all the classes in the SDK.
    gSdkManager = FbxManager::Create();

	// create an IOSettings object
	FbxIOSettings * ios = FbxIOSettings::Create(gSdkManager, IOSROOT );
	gSdkManager->SetIOSettings(ios);

}

// Destroys an instance of the SDK manager
void DestroySdkObjects(
                       FbxManager* pSdkManager,
					   bool pExitStatus
                       )
{
    // Delete the FBX SDK manager.
    // All the objects that
    // (1) have been allocated by the memory manager, AND that
    // (2) have not been explicitly destroyed
    // will be automatically destroyed.
    if( pSdkManager ) pSdkManager->Destroy();
	if( pExitStatus ) FBXSDK_printf("Program Success!\n");
}

// Creates an importer object, and uses it to
// import a file into a scene.
bool LoadScene(
               FbxManager* pSdkManager,  // Use this memory manager...
               FbxScene* pScene,            // to import into this scene
               const char* pFilename         // the data from this file.
               )
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    int i, lAnimStackCount;
    bool lStatus;
    char lPassword[1024];

    // Get the version number of the FBX files generated by the
    // version of FBX SDK that you are using.
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);
	
    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create(pSdkManager,"");
    
    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, pSdkManager->GetIOSettings() );

    // Get the version number of the FBX file format.
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if( !lImportStatus )  // Problem with the file to be imported
    {
        FbxString error = lImporter->GetStatus().GetErrorString();
        UI_Printf("Call to FbxImporter::Initialize() failed.");
        UI_Printf("Error returned: %s", error.Buffer());

        if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
        {
            UI_Printf("FBX version number for this FBX SDK is %d.%d.%d",
                lSDKMajor, lSDKMinor, lSDKRevision);
            UI_Printf("FBX version number for file %s is %d.%d.%d",
                pFilename, lFileMajor, lFileMinor, lFileRevision);
        }

        return false;
    }

    UI_Printf("FBX version number for this FBX SDK is %d.%d.%d",
        lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        UI_Printf("FBX version number for file %s is %d.%d.%d",
            pFilename, lFileMajor, lFileMinor, lFileRevision);

        // In FBX, a scene can have one or more "animation stack". An animation stack is a
        // container for animation data.
        // You can access a file's animation stack information without
        // the overhead of loading the entire file into the scene.

        UI_Printf("Animation Stack Information");

        lAnimStackCount = lImporter->GetAnimStackCount();

        UI_Printf("    Number of animation stacks: %d", lAnimStackCount);
        UI_Printf("    Active animation stack: \"%s\"",
            lImporter->GetActiveAnimStackName());

        for(i = 0; i < lAnimStackCount; i++)
        {
            FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            UI_Printf("    Animation Stack %d", i);
            UI_Printf("         Name: \"%s\"", lTakeInfo->mName.Buffer());
            UI_Printf("         Description: \"%s\"",
                lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the animation stack should
            // be imported under a different name.
            UI_Printf("         Import Name: \"%s\"", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false
            // if the animation stack should be not be imported.
            UI_Printf("         Import State: %s", lTakeInfo->mSelect ? "true" : "false");
        }

        // Import options determine what kind of data is to be imported.
        // The default is true, but here we set the options explictly.

        IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
        IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
        IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
        IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
        IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
        IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
        IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(pScene);

    if(lStatus == false &&     // The import file may have a password
        lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
    {
        UI_Printf("Please enter password: ");

        lPassword[0] = '\0';

        FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
        scanf("%s", lPassword);
        FBXSDK_CRT_SECURE_NO_WARNING_END
        FbxString lString(lPassword);
        
        IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
        IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);


        lStatus = lImporter->Import(pScene);

        if(lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
        {
            UI_Printf("Incorrect password: file not imported.");
        }
    }

    // Destroy the importer
    lImporter->Destroy();

    return lStatus;
}

bool ConvertColorToTangentPerMeshByControlPoint(FbxMesh* mesh) {
	FbxLayerElementArray* array = NULL;
	FbxGeometryElementTangent* tangent = mesh->GetElementTangent(0);

	UI_Printf("Un-supported Mapping Mode");
	return false;
}

bool ConvertColorToTangentPerMeshByPolygonVertex(FbxMesh* mesh, FbxGeometryElementVertexColor* colorElm) {
	int polygonCount = mesh->GetPolygonCount();
	int polygonVertexCount = 0;
	for (int i = 0; i < polygonCount; ++i) {
		for (int j = 0; j < mesh->GetPolygonSize(i); ++j) {
			++polygonVertexCount;
		}
	}

	FbxGeometryElementTangent* tangentElm = NULL;
	//if (mesh->GetElementTangentCount() < 1) {
	//	mesh->InitTangents(polygonVertexCount);
	//}
	tangentElm = mesh->GetElementTangent();
	if (!tangentElm) {
		UI_Printf("Cannot find tangents");
		return false;
	}

	if (tangentElm->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
		UI_Printf("Tangent by control point");
	}

	tangentElm->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	tangentElm->SetReferenceMode(FbxGeometryElement::eDirect);
	FbxLayerElementArrayTemplate<FbxVector4>& tangentArr = tangentElm->GetDirectArray();


	UI_Printf("control point count: %d", mesh->GetControlPointsCount());
	
	UI_Printf("tangent count before: %d", tangentArr.GetCount());
	tangentArr.Resize(polygonVertexCount);
	UI_Printf("tangent count after: %d", tangentArr.GetCount());

	FbxLayerElementArrayTemplate<FbxColor>& colorArr = colorElm->GetDirectArray();
	int vertexIndex = 0;
	for (int i = 0; i < polygonCount; ++i) {
		for (int j = 0; j < mesh->GetPolygonSize(i); ++j) {
			FbxColor color;
			switch (colorElm->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
				{
					color = colorArr.GetAt(vertexIndex);		
				}
				break;
			case FbxLayerElement::eIndexToDirect:
				{
					int index = colorElm->GetIndexArray().GetAt(vertexIndex);
					color = colorArr.GetAt(index);
				}
				break;
			default:
				UI_Printf("Invalid Reference");
				return false;
			}
			FbxVector4 tangent(color.mRed, color.mGreen, color.mBlue);
			tangent = (tangent - 0.5) * 2;
			tangent.mData[3] = 0;
			tangentArr.SetAt(vertexIndex, tangent);
			++vertexIndex;
		}
	}
	
	return true;
}

bool ConvertColorToTangentPerMesh(FbxMesh* mesh)
{
	FbxGeometryElementVertexColor* elm = NULL;
	for (int i = 0; i < mesh->GetElementVertexColorCount(); ++i) {
		FbxGeometryElementVertexColor* each_elm = mesh->GetElementVertexColor(i);

		if (strcmp(each_elm->GetName(), "OutlineNormal") == 0) {
			elm = each_elm;
			break;
		}
	}
	if (!elm) {
		UI_Printf("WARNING: There is no color set \"OutlineNormal\" in the mesh");
		return true;
	}

	switch (elm->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint: 
		{
			if (!ConvertColorToTangentPerMeshByControlPoint(mesh)) return false;
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
		{
			if (!ConvertColorToTangentPerMeshByPolygonVertex(mesh, elm)) return false;
		}
		break;
	default:
		return false;
	}
	
	return true;
}

static bool ConvertColorToTangentPerNode(FbxNode* pNode)
{
    if(pNode) {
        for(int i = 0; i < pNode->GetChildCount(); i++) {
			FbxNode* pChild = pNode->GetChild(i);
			FbxMesh* mesh = pChild->GetMesh();
			if (mesh) {
				UI_Printf("-----------------------------");
				UI_Printf(pChild->GetName());
				UI_Printf("-----------------------------");

				if (!ConvertColorToTangentPerMesh(mesh)) return false;
			}

			if (!ConvertColorToTangentPerNode(pChild)) return false;
		}   
    }
	return true;
}

static bool ConvertColorToTangent(FbxScene* pScene)
{
	FbxNode* pRootNode = pScene->GetRootNode();
	return ConvertColorToTangentPerNode(pRootNode);
}

// Exports a scene to a file
bool SaveScene(
               FbxManager* pSdkManager,
               FbxScene* pScene,
               const char* pFilename,
               int pFileFormat,
               bool pEmbedMedia
               )
{
    int lMajor, lMinor, lRevision;
    bool lStatus = true;

    // Create an exporter.
    FbxExporter* lExporter = FbxExporter::Create(pSdkManager, "");

    if( pFileFormat < 0 ||
        pFileFormat >=
        pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount() )
    {
        // Write in fall back format if pEmbedMedia is true
        pFileFormat =
            pSdkManager->GetIOPluginRegistry()->GetNativeWriterFormat();
     
        if (!pEmbedMedia)
        {
            //Try to export in ASCII if possible
            int lFormatIndex, lFormatCount =
                pSdkManager->GetIOPluginRegistry()->
                GetWriterFormatCount();

            for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
            {
                if (pSdkManager->GetIOPluginRegistry()->
                    WriterIsFBX(lFormatIndex))
                {
                    FbxString lDesc = pSdkManager->GetIOPluginRegistry()->
                        GetWriterFormatDescription(lFormatIndex);
                    if (lDesc.Find("ascii")>=0)
                    {
                        pFileFormat = lFormatIndex;
                        break;
                    }
                }
            }
        }
    }

	////TODO: write more generally
	// pFileFormat = pSdkManager->GetIOPluginRegistry()->FindWriterIDByDescription("FBX 6.0 binary (*.fbx)");

    // Initialize the exporter by providing a filename.
    if(lExporter->Initialize(pFilename, pFileFormat, pSdkManager->GetIOSettings()) == false)
    {
        UI_Printf("Call to FbxExporter::Initialize() failed.");
        UI_Printf("Error returned: %s", lExporter->GetStatus().GetErrorString());
        return false;
    }

    FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
    UI_Printf("FBX version number for this FBX SDK is %d.%d.%d",
        lMajor, lMinor, lRevision);

    if (pSdkManager->GetIOPluginRegistry()->WriterIsFBX(pFileFormat))
    {
        // Export options determine what kind of data is to be imported.
        // The default (except for the option eEXPORT_TEXTURE_AS_EMBEDDED)
        // is true, but here we set the options explicitly.
        IOS_REF.SetBoolProp(EXP_FBX_MATERIAL,        true);
        IOS_REF.SetBoolProp(EXP_FBX_TEXTURE,         true);
        IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED,        pEmbedMedia);
        IOS_REF.SetBoolProp(EXP_FBX_SHAPE,           true);
        IOS_REF.SetBoolProp(EXP_FBX_GOBO,            true);
        IOS_REF.SetBoolProp(EXP_FBX_ANIMATION,       true);
        IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);
    }

    // Export the scene.

    lStatus = lExporter->Export(pScene);

    // Destroy the exporter.
    lExporter->Destroy();

    return lStatus;
}

// Get the filters for the <Open file> dialog
// (description + file extention)
const char *GetReaderOFNFilters()
{
    int nbReaders =
		gSdkManager->GetIOPluginRegistry()->GetReaderFormatCount();

    FbxString s;
    int i = 0;

    for(i=0; i < nbReaders; i++)
    {
        s += gSdkManager->GetIOPluginRegistry()->
            GetReaderFormatDescription(i);
        s += "|*.";
        s += gSdkManager->GetIOPluginRegistry()->
            GetReaderFormatExtension(i);
        s += "|";
    }

    // replace | by \0
    int nbChar   = int(strlen(s.Buffer())) + 1;
    char *filter = new char[ nbChar ];
    memset(filter, 0, nbChar);

    FBXSDK_strcpy(filter, nbChar, s.Buffer());

    for(i=0; i < int(strlen(s.Buffer())); i++)
    {
        if(filter[i] == '|')
        {
            filter[i] = 0;
        }
    }

    // the caller must delete this allocated memory
    return filter;
}

// Get the filters for the <Save file> dialog
// (description + file extention)
const char *GetWriterSFNFilters()
{
    int nbWriters =
        gSdkManager->GetIOPluginRegistry()->GetWriterFormatCount();

    FbxString s;
    int i=0;

    for(i=0; i < nbWriters; i++)
    {
        s += gSdkManager->GetIOPluginRegistry()->
            GetWriterFormatDescription(i);
        s += "|*.";
        s += gSdkManager->GetIOPluginRegistry()->
            GetWriterFormatExtension(i);
        s += "|";
    }

    // replace | by \0
    int nbChar   = int(strlen(s.Buffer())) + 1;
    char *filter = new char[ nbChar ];
    memset(filter, 0, nbChar);

    FBXSDK_strcpy(filter, nbChar, s.Buffer());

    for(i=0; i < int(strlen(s.Buffer())); i++)
    {
        if(filter[i] == '|')
        {
            filter[i] = 0;
        }
    }

    // the caller must delete this allocated memory
    return filter;
}

// to get a file extention for a WriteFileFormat
const char *GetFileFormatExt(
                             const int pWriteFileFormat
                             )
{
    char *buf = new char[10];
    memset(buf, 0, 10);

    // add a starting point .
    buf[0] = '.';
    const char * ext = gSdkManager->GetIOPluginRegistry()->
        GetWriterFormatExtension(pWriteFileFormat);
    FBXSDK_strcat(buf, 10, ext);

    // the caller must delete this allocated memory
    return buf;
}