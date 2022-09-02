#pragma once
#include <json.hpp>
#include "./fbx_common.h"
using json = nlohmann::ordered_json;

class Fbx2Json
{
public:
	static json exportNode(FbxNode *node)
	{
		json j = {};
		j["name"] = node->GetName();
		j["mesh"] = exportMesh(node->GetMesh());
		std::vector<json> childrenData;
		for (int i = 0; i < node->GetChildCount(); i++)
		{
			childrenData.push_back(exportNode(node->GetChild(i)));
		}
		j["children"] = json(childrenData);
		return j;
	}

	static json exportScene(FbxScene *pScene)
	{
		json j = {};
		j["RootNode"] = exportNode(pScene->GetRootNode());
		return j;
	}

	static json dumpIndexArray(const FbxLayerElementArrayTemplate<int>& indexArray)
	{
		std::vector<int> indexData;
		indexData.reserve(indexArray.GetCount());
		for (int i = 0; i < indexArray.GetCount(); ++i) {
			indexData.push_back(indexArray.GetAt(i));
		}
		return json(indexData);
	}
	static json dumpColorArray(const FbxLayerElementArrayTemplate<FbxColor>& colorArray)
	{
		std::vector<json> colorData;
		colorData.reserve(colorArray.GetCount());
		for (int i = 0; i < colorArray.GetCount(); ++i) {
			const FbxColor& c = colorArray.GetAt(i);
			colorData.push_back(json({ c.mRed,c.mGreen,c.mBlue,c.mAlpha }));
		}
		return json(colorData);
	}
	static json dumpVector2Array(const FbxLayerElementArrayTemplate<FbxVector2>& v2Array)
	{
		std::vector<json> data;
		data.reserve(v2Array.GetCount());
		for (int i = 0; i < v2Array.GetCount(); ++i) {
			const auto& c = v2Array.GetAt(i);
			data.push_back(json({ c[0],c[1] }));
		}
		return json(data);
	}

	template<class V>
	static json dumpVector4Array(const FbxLayerElementArrayTemplate<FbxVector4>& vectorArray)
	{
		std::vector<json> data;
		data.reserve(vectorArray.GetCount());
		for (int i = 0; i < vectorArray.GetCount(); ++i) {
			const auto& c = vectorArray.GetAt(i);
			data.push_back(json({ c[0],c[1],c[2],c[3] }));
		}
		return json(data);
	}

	static std::string MappingModeEnumString(FbxLayerElement::EMappingMode mode) {
		static const std::vector<std::string> strEMappingMode = {
			"eNone",
			"eByControlPoint",
			"eByPolygonVertex",
			"eByPolygon",
			"eByEdge",
			"eAllSame"
		};
		return strEMappingMode[(int)mode];
	}
	static std::string ReferenceModeString(FbxLayerElement::EReferenceMode mode) {
		static const std::vector<std::string> strERefMode = {
			"eDirect",
			"eIndex",
			"eIndexToDirect"
		};
		return strERefMode[mode];
	}

	static bool getVertexColors(FbxMesh *pMesh, int lControlPointIndex, int vertexId, FbxColor &out)
	{
		FbxGeometryElementVertexColor *leVtxc = pMesh->GetElementVertexColor(0);
		FbxGeometryElementVertexColor * pLayerVertex = pMesh->GetLayer(0)->GetVertexColors();
		switch (leVtxc->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
			switch (leVtxc->GetReferenceMode())
			{
				case FbxGeometryElement::eDirect:
					out = leVtxc->GetDirectArray().GetAt(lControlPointIndex);
					return true;
				case FbxGeometryElement::eIndexToDirect:
				{
					int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
					out = leVtxc->GetDirectArray().GetAt(id);
					return true;
				}
				break;
			default:
				break; // other reference modes not shown here!
			}
			break;
		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (leVtxc->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				out = leVtxc->GetDirectArray().GetAt(vertexId);
				return true;
			}
			case FbxGeometryElement::eIndexToDirect:
			{
				int id = leVtxc->GetIndexArray().GetAt(vertexId);
				out = leVtxc->GetDirectArray().GetAt(id);
				return true;
			}
			break;
			default:
				break; // other reference modes not shown here!
			}
		}
		case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
		case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
		case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
			break;
		default:
			break;
		}
		return false;
	}

	static json dumpColor(const FbxColor& color) {
		return json({ color.mRed, color.mGreen, color.mBlue, color.mAlpha });
	}
	static json dumpVector4(const FbxVector4& v) {
		return json({v[0], v[1], v[2], v[3]});
	}

	static json exportMesh(FbxMesh *pFbxMesh)
	{
		if (pFbxMesh == nullptr) {
			return json(nullptr);
		}

		json ret = {};
		ret["name"] = pFbxMesh->GetName();

		//control points
		FbxVector4* lControlPoints = pFbxMesh->GetControlPoints();
		int nPtCount = pFbxMesh->GetControlPointsCount();
		std::vector<json> data;
		data.reserve(nPtCount);
		for (int i = 0; i < nPtCount; ++i) {
			const auto& c = pFbxMesh->GetControlPointAt(i);
			data.push_back(json({ c[0],c[1],c[2],c[3] }));
		}
		ret["controlPoints"] = data;

		//ploygons
		std::vector<json> polygons;
		int vertexId = 0;
		for (int i = 0; i < pFbxMesh->GetPolygonCount(); i++) {
			int lPolygonSize = pFbxMesh->GetPolygonSize(i);
			std::vector<int> pointIndex;
			for(int j=0;j<lPolygonSize;j++){
				int lControlPointIndex = pFbxMesh->GetPolygonVertex(i, j);
				pointIndex.push_back(lControlPointIndex);
			}
			polygons.push_back(json(pointIndex));
		}
		ret["polygons"] = polygons;

		//vertex colors
		std::vector<json> vcData = {};
		int clrChannelCnt = pFbxMesh->GetElementVertexColorCount();
		for (int i = 0; i < clrChannelCnt; i++) {
			json j = {};
			FbxGeometryElementVertexColor *clrElem = pFbxMesh->GetElementVertexColor(i);
			j["name"] = clrElem->GetName();
			j["mappingMode"] = MappingModeEnumString(clrElem->GetMappingMode());
			j["refMode"] = ReferenceModeString(clrElem->GetReferenceMode());
			auto& dirArray = clrElem->GetDirectArray();
			auto& indexArray = clrElem->GetIndexArray();
			j["indexArray"] = dumpIndexArray(indexArray);
			j["directArray"] = dumpColorArray(dirArray);
			vcData.push_back(j);
		}
		ret["vertexColors"] = json(vcData);
		
		//element uvs
		int uvChannelCnt = pFbxMesh->GetElementUVCount();
		std::vector<json> uvData = {};
		for (int uvi = 0; uvi < uvChannelCnt; ++uvi)
		{
			json j = {};
			FbxGeometryElementUV* pElementUV = pFbxMesh->GetElementUV(uvi);
			auto mappingMode = pElementUV->GetMappingMode();
			auto refMode = pElementUV->GetReferenceMode();
			auto& directArray = pElementUV->GetDirectArray();
			auto& idxArray = pElementUV->GetIndexArray();

			j["name"] = pElementUV->GetName();
			j["mappingMode"] = MappingModeEnumString(mappingMode);
			j["refMode"] = ReferenceModeString(refMode);
			j["indexArray"] = dumpIndexArray(idxArray);
			j["directArray"] = dumpVector2Array(directArray);
			uvData.push_back(j);
		}
		ret["uv"] = json(uvData);
		return ret;
	}
};

