#pragma once
#include <json.hpp>
#include "./fbx_common.h"
using json = nlohmann::json;

json exportNode(FbxNode *node)
{
    json j = {};
    j["name"] = node->GetName();
    return j;
}

json exportScene(FbxScene *pScene)
{
    json j = {};
    j["RootNode"] = exportNode(pScene->GetRootNode());
    return j;
}