/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#include "DisplayMesh.h"

#include "DisplayMaterial.h"
#include "DisplayTexture.h"
#include "DisplayLink.h"
#include "DisplayShape.h"
#include "DisplayCache.h"
#include "../../../../tinyxml2-master/tinyxml2-master/tinyxml2.h"
#include <vector>
#include <string>


using namespace tinyxml2;
using namespace std;

#if defined (FBXSDK_ENV_MAC)
// disable the “format not a string literal and no format arguments?warning since
// the FBXSDK_printf calls made here are all valid calls and there is no secuity risk
#pragma GCC diagnostic ignored "-Wformat-security"
#endif

#define MAT_HEADER_LENGTH 200

void DisplayControlsPoints(FbxMesh* pMesh, XMLElement *MeshElement);
void DisplayPolygons(FbxMesh* pMesh, XMLElement *MeshElement);
void DisplayMaterialMapping(FbxMesh* pMesh);
void DisplayTextureMapping(FbxMesh* pMesh);
void DisplayTextureNames( FbxProperty &pProperty, FbxString& pConnectionString );
void DisplayMaterialConnections(FbxMesh* pMesh);
void DisplayMaterialTextureConnections( FbxSurfaceMaterial* pMaterial, 
                                       char * header, int pMatId, int l );

XMLDocument doc;
XMLElement *RootElement = doc.NewElement("Display_Model");
//XMLElement *MeshElement = doc.NewElement("Mesh_name");
//XMLElement *ControlPointElement = doc.NewElement("Control_Point");
//XMLElement *PolyElement = doc.NewElement("Polygons");
//XMLElement *PoElement = doc.NewElement("Polygon");
//XMLElement *MaterialElement = doc.NewElement("Material");
XMLElement *MeshsElement = doc.NewElement("Module");
//doc.LoadFile("DisplayMesh.xml");
// here demo ASM file,  mesh layer nLayer= 4
XMLElement *GroupElement[50]; // max within 20 layer node
string lastNullNodeName[50]; // max within 20 layer node
void DisplayMesh(FbxNode* pNode, vector<vector<string>> NullName, int NextGroup, int nLayer, int NextLayer)
{

	string GroupName = "NA";
	if (NullName[nLayer - 1].size() > 0){
		GroupName = NullName[nLayer - 1][NullName[nLayer - 1].size() - 1];
	}
	if (NextLayer == 1){ FBXSDK_printf("   ============================= New Layer Node ============== "); }
	if (NextGroup == 1){ FBXSDK_printf("   ================ New Group Node ============== "); }
	DisplayString("\n@@@@@@@@@@@@@ Group Name: ", GroupName.c_str());
	//XMLElement *MeshsElement = doc.NewElement("DisplayMesh");
	//doc.LoadFile("DisplayMesh.xml");
	doc.InsertEndChild(RootElement);
	RootElement->LinkEndChild(MeshsElement);

	if (NextGroup == 1)
	{

		//if (NextLayer == 1)
		//if (GroupName == "pasted__64-22811A_X" || GroupName == "pasted__pasted__64-22811A_X")
		//{
			for (int o = nLayer - 1; o > 1; o--)
			{
				//if (!GroupElement[nLayer - o] && nLayer - o > 1)
				//{
				if (lastNullNodeName[nLayer - o] != NullName[nLayer - o][NullName[nLayer - o].size() - 1])
				{
					//DisplayString("@@@@@ ", lastNullNodeName[nLayer - o].c_str());
					//DisplayString("@@@@@ ", NullName[nLayer - o][NullName[nLayer - o].size() - 1].c_str());
					//system("pause");
					GroupElement[nLayer - o] = doc.NewElement("ASM");
					GroupElement[nLayer - o]->SetAttribute("name", NullName[nLayer - o][NullName[nLayer - o].size() - 1].c_str());
					if (!GroupElement[nLayer - (o + 1)]){ MeshsElement->LinkEndChild(GroupElement[nLayer - o]); }
					if (GroupElement[nLayer - (o + 1)]){ GroupElement[nLayer - (o + 1)]->LinkEndChild(GroupElement[nLayer - o]); }
				}
				lastNullNodeName[nLayer - o] = NullName[nLayer - o][NullName[nLayer - o].size() - 1];
			}
		//}
		//GroupElement = doc.NewElement("Part");
		//GroupElement->SetAttribute("name", GroupName.c_str());
		//MeshsElement->LinkEndChild(GroupElement);
		GroupElement[nLayer - 1] = doc.NewElement("Part");
		GroupElement[nLayer - 1]->SetAttribute("name", GroupName.c_str());
		if (GroupElement[nLayer - 2]){ GroupElement[nLayer - 2]->LinkEndChild(GroupElement[nLayer - 1]); }
		else { MeshsElement->LinkEndChild(GroupElement[nLayer - 1]); }
	}


	XMLElement *MeshElement = doc.NewElement("Mesh");
	//MeshElement->SetAttribute("group", GroupName.c_str());
	MeshElement->SetAttribute("name", (char *)pNode->GetName());
	//doc.LoadFile("DisplayMesh.xml");
	//doc.InsertEndChild(RootElement);

	if (GroupElement[nLayer - 1]){ GroupElement[nLayer - 1]->LinkEndChild(MeshElement); }
	//for (int l = 1; l < 4; l++)
	//{
	//	if (GroupElement[nLayer - l]){ GroupElement[nLayer - l]->LinkEndChild(MeshElement); }
	//}


	//XMLElement *MeshNameElement = doc.NewElement("name");
	//MeshElement->LinkEndChild(MeshNameElement);
	//XMLText *MeshNameContent = doc.NewText((char *)pNode->GetName());
	//MeshNameElement->LinkEndChild(MeshNameContent);

	FbxMesh* lMesh = (FbxMesh*)pNode->GetNodeAttribute();

	DisplayString("Mesh Name: ", (char *)pNode->GetName());
	DisplayMetaDataConnections(lMesh);

	//DisplayControlsPoints(lMesh, MeshElement);

	DisplayPolygons(lMesh, MeshElement);
	doc.SaveFile("DisplayMesh.xml"); 
	//DisplayMaterialMapping(lMesh);
	//DisplayMaterial(lMesh);
	//DisplayTexture(lMesh);
	//DisplayMaterialConnections(lMesh);
	//DisplayLink(lMesh);
	//DisplayShape(lMesh);

	//DisplayCache(lMesh);
	
}


void DisplayControlsPoints(FbxMesh* pMesh, XMLElement *MeshElement)
{
    int i, lControlPointsCount = pMesh->GetControlPointsCount();
    FbxVector4* lControlPoints = pMesh->GetControlPoints();
	XMLElement *ControlPointsElement = doc.NewElement("ControlPoints");
	MeshElement->LinkEndChild(ControlPointsElement);

    DisplayString("    Control Points");

    for (i = 0; i < lControlPointsCount; i++)
    {
		//XMLElement *ControlPointElement = doc.NewElement("Control_Point");
		//ControlPointsElement->LinkEndChild(ControlPointElement);
		//ControlPointElement->SetAttribute("id", i);
		//ControlPointElement->SetAttribute("coordinate", lControlPoints[i]);

		//XMLElement *CPcoordinateElement = doc.NewElement("Coordinate");
		//ControlPointElement->LinkEndChild(CPcoordinateElement);

		FbxString lString;
		FbxVector4 pValue = lControlPoints[i];
		FbxString lFloatValue1 = (float)pValue[0];
		FbxString lFloatValue2 = (float)pValue[1];
		FbxString lFloatValue3 = (float)pValue[2];

		lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
		lFloatValue1 = pValue[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
		lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
		lFloatValue2 = pValue[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();
		lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
		lFloatValue3 = pValue[2] >= HUGE_VAL ? "INFINITY" : lFloatValue3.Buffer();

		lString = "(";
		lString += lFloatValue1;
		lString += ", ";
		lString += lFloatValue2;
		lString += ", ";
		lString += lFloatValue3;
		lString += ")";
		
		//scoreElement->SetAttribute("_", i *10);
		//XMLText *cpContent = doc.NewText(lString);
		//CPcoordinateElement->LinkEndChild(cpContent);

		DisplayInt("        Control Point ", i);
		Display3DVector("            Coordinates: ", lControlPoints[i]);


		/*
        for (int j = 0; j < pMesh->GetElementNormalCount(); j++)
        {
			//XMLElement *VectorElement = doc.NewElement("NormalVector");
			//ControlPointElement->LinkEndChild(VectorElement);

            FbxGeometryElementNormal* leNormals = pMesh->GetElementNormal( j);
			if (leNormals->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				char header[100];
				if (i == 0 || i == 1 || i == lControlPointsCount - 2 || i == lControlPointsCount - 1)
				{
					FBXSDK_sprintf(header, 100, "            Normal Vector: ");
				}
				if (leNormals->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					if (i == 0 || i == 1 || i == lControlPointsCount - 2 || i == lControlPointsCount - 1)
					{
						Display3DVector(header, leNormals->GetDirectArray().GetAt(i));
					}
					pValue = leNormals->GetDirectArray().GetAt(i);
					lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
					lFloatValue1 = pValue[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
					lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
					lFloatValue2 = pValue[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();
					lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
					lFloatValue3 = pValue[2] >= HUGE_VAL ? "INFINITY" : lFloatValue3.Buffer();

					lString = "(";
					lString += lFloatValue1;
					lString += ", ";
					lString += lFloatValue2;
					lString += ", ";
					lString += lFloatValue3;
					lString += ")";
					XMLText *vContent = doc.NewText(lString);

					//VectorElement->LinkEndChild(vContent);
				}
			}
        } */
    }

    DisplayString("");
}


void DisplayPolygons(FbxMesh* pMesh, XMLElement *MeshElement)
{
	XMLElement *PolysElement = doc.NewElement("Polygons");
	MeshElement->LinkEndChild(PolysElement);


    int i, j, lPolygonCount = pMesh->GetPolygonCount();
    FbxVector4* lControlPoints = pMesh->GetControlPoints(); 
    char header[100];

    DisplayString("    Polygons");

    int vertexId = 0;
    for (i = 0; i < lPolygonCount; i++)
    {
		XMLElement *PolyElement = doc.NewElement("Polygon");
		PolysElement->LinkEndChild(PolyElement);
		PolyElement->SetAttribute("id", i);
		//XMLText *poContent = doc.NewText("coordinate");
		//PolyElement->LinkEndChild(poContent);
		if (i == 0 || i == 1 || i == lPolygonCount - 2 || i == lPolygonCount - 1)
		{
			DisplayInt("        Polygon ", i);
		}
        int l;

        for (l = 0; l < pMesh->GetElementPolygonGroupCount(); l++)
        {
            FbxGeometryElementPolygonGroup* lePolgrp = pMesh->GetElementPolygonGroup(l);
			switch (lePolgrp->GetMappingMode())
			{
			case FbxGeometryElement::eByPolygon:
				if (lePolgrp->GetReferenceMode() == FbxGeometryElement::eIndex)
				{
					FBXSDK_sprintf(header, 100, "        Assigned to group: "); 
					int polyGroupId = lePolgrp->GetIndexArray().GetAt(i);
					DisplayInt(header, polyGroupId);
					break;
				}
			default:
				// any other mapping modes don't make sense
				DisplayString("        \"unsupported group assignment\"");
				break;
			}
        }

        int lPolygonSize = pMesh->GetPolygonSize(i);
		
		PolyElement->SetAttribute("PolygonSize", pMesh->GetPolygonSize(i));


		for (j = 0; j < lPolygonSize; j++)
		{
			int lControlPointIndex = pMesh->GetPolygonVertex(i, j);
			XMLElement *PoElement = doc.NewElement("Coordinate");
			PolyElement->LinkEndChild(PoElement);
			PoElement->SetAttribute("id", j);
			//XMLText *poContent = doc.NewText("coordinate");
			//PoElement->LinkEndChild(poContent);
			FbxString lString;
			FbxVector4 pValue = lControlPoints[lControlPointIndex];
			FbxString lFloatValue1 = (float)pValue[0];
			FbxString lFloatValue2 = (float)pValue[1];
			FbxString lFloatValue3 = (float)pValue[2];
			lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
			lFloatValue1 = pValue[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
			lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
			lFloatValue2 = pValue[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();
			lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
			lFloatValue3 = pValue[2] >= HUGE_VAL ? "INFINITY" : lFloatValue3.Buffer();

			lString = "(";
			lString += lFloatValue1;
			lString += ", ";
			lString += lFloatValue2;
			lString += ", ";
			lString += lFloatValue3;
			lString += ")";
			XMLText *pContent = doc.NewText(lString);

			PoElement->LinkEndChild(pContent);

			if (i == 0 || i == 1 || i == lPolygonCount - 2 || i == lPolygonCount - 1)
			{
				Display3DVector("            Coordinates: ", lControlPoints[lControlPointIndex]);
			}
			for (l = 0; l < pMesh->GetElementVertexColorCount(); l++)
			{
				FbxGeometryElementVertexColor* leVtxc = pMesh->GetElementVertexColor( l);
				FBXSDK_sprintf(header, 100, "            Color vertex: "); 




				switch (leVtxc->GetMappingMode())
				{
				default:
				    break;
				case FbxGeometryElement::eByControlPoint:
					switch (leVtxc->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						DisplayColor(header, leVtxc->GetDirectArray().GetAt(lControlPointIndex));
						break;
					case FbxGeometryElement::eIndexToDirect:
						{
							int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
							DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
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
							DisplayColor(header, leVtxc->GetDirectArray().GetAt(vertexId));
							break;
						case FbxGeometryElement::eIndexToDirect:
							{
								int id = leVtxc->GetIndexArray().GetAt(vertexId);
								DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
							}
							break;
						default:
							break; // other reference modes not shown here!
						}
					}
					break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
			}

			//XMLElement *UVElement = doc.NewElement("Texture_UV");
			//PoElement->LinkEndChild(UVElement);

			for (l = 0; l < pMesh->GetElementUVCount(); ++l)
			{
				XMLElement *UVElement = doc.NewElement("TextureUV");
				PolyElement->LinkEndChild(UVElement);
				//UVElement->SetAttribute("id", l);

				FbxGeometryElementUV* leUV = pMesh->GetElementUV( l);
				if (i == 0 || i == 1 || i == lPolygonCount - 2 || i == lPolygonCount - 1)
				{
					FBXSDK_sprintf(header, 100, "            Texture UV: ");
				}
				switch (leUV->GetMappingMode())
				{
				default:
				    break;
				case FbxGeometryElement::eByControlPoint:
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						if (i == 0 || i == 1 || i == lPolygonCount - 2 || i == lPolygonCount - 1)
						{
							Display2DVector(header, leUV->GetDirectArray().GetAt(lControlPointIndex));
						}
						pValue = leUV->GetDirectArray().GetAt(lControlPointIndex);
				//		UVElement->LinkEndChild(UVElement);
						break;
					case FbxGeometryElement::eIndexToDirect:
						{
							int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
							if (i == 0 || i == 1 || i == lPolygonCount - 2 || i == lPolygonCount - 1)
							{
								Display2DVector(header, leUV->GetDirectArray().GetAt(id));
							}
							pValue = leUV->GetDirectArray().GetAt(id);
				//			UVElement->LinkEndChild(UVElement);
						}
						break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
					{
						int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
						switch (leUV->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						case FbxGeometryElement::eIndexToDirect:
							{
								if (i == 0 || i == 1 || i == lPolygonCount - 2 || i == lPolygonCount - 1)
								{
									Display2DVector(header, leUV->GetDirectArray().GetAt(lTextureUVIndex));
								}
								pValue = leUV->GetDirectArray().GetAt(lTextureUVIndex);
			//					UVElement->LinkEndChild(UVElement);
							}
							break;
						default:
							break; // other reference modes not shown here!
						}
					}
					break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}

				//FbxVector4 pValue = lControlPoints[lControlPointIndex];

				lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
				lFloatValue1 = pValue[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
				lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
				lFloatValue2 = pValue[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();
				lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
				lFloatValue3 = pValue[2] >= HUGE_VAL ? "INFINITY" : lFloatValue3.Buffer();

				lString = "(";
				lString += lFloatValue1;
				lString += ", ";
				lString += lFloatValue2;
				lString += ", ";
				lString += lFloatValue3;
				lString += ")";
				XMLText *UVContent = doc.NewText(lString);

				UVElement->LinkEndChild(UVContent);

			}
			for( l = 0; l < pMesh->GetElementNormalCount(); ++l)
			{
				FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal( l);
				if (i == 0 || i == 1 || i == lPolygonCount - 2 || i == lPolygonCount - 1)
				{
					FBXSDK_sprintf(header, 100, "            Normal: ");
				}

				if(leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						if (i == 0 || i == 1 || i == lPolygonCount - 2 || i == lPolygonCount - 1)
						{
							Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));
						}
						break;
					case FbxGeometryElement::eIndexToDirect:
						{
							int id = leNormal->GetIndexArray().GetAt(vertexId);
							if (i == 0 || i == 1 || i == lPolygonCount - 2 || i == lPolygonCount - 1)
							{
								Display3DVector(header, leNormal->GetDirectArray().GetAt(id));
							}
						}
						break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			for( l = 0; l < pMesh->GetElementTangentCount(); ++l)
			{
				FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent( l);
				FBXSDK_sprintf(header, 100, "            Tangent: "); 

				if(leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leTangent->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						Display3DVector(header, leTangent->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
						{
							int id = leTangent->GetIndexArray().GetAt(vertexId);
							Display3DVector(header, leTangent->GetDirectArray().GetAt(id));
						}
						break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			for( l = 0; l < pMesh->GetElementBinormalCount(); ++l)
			{

				FbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal( l);

				FBXSDK_sprintf(header, 100, "            Binormal: "); 
				if(leBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leBinormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						Display3DVector(header, leBinormal->GetDirectArray().GetAt(vertexId));
						break;
					case FbxGeometryElement::eIndexToDirect:
						{
							int id = leBinormal->GetIndexArray().GetAt(vertexId);
							Display3DVector(header, leBinormal->GetDirectArray().GetAt(id));
						}
						break;
					default:
						break; // other reference modes not shown here!
					}
				}
			}
			vertexId++;
		} // for polygonSize
    } // for polygonCount


    //check visibility for the edges of the mesh
	for(int l = 0; l < pMesh->GetElementVisibilityCount(); ++l)
	{
		FbxGeometryElementVisibility* leVisibility=pMesh->GetElementVisibility(l);
		FBXSDK_sprintf(header, 100, "    Edge Visibility : ");
		DisplayString(header);
		switch(leVisibility->GetMappingMode())
		{
		default:
		    break;
			//should be eByEdge
		case FbxGeometryElement::eByEdge:
			//should be eDirect
			for(j=0; j!=pMesh->GetMeshEdgeCount();++j)
			{
				DisplayInt("        Edge ", j);
				DisplayBool("              Edge visibility: ", leVisibility->GetDirectArray().GetAt(j));
			}

			break;
		}
	}
    DisplayString("");
}

void DisplayTextureNames( FbxProperty &pProperty, FbxString& pConnectionString )
{
    int lLayeredTextureCount = pProperty.GetSrcObjectCount<FbxLayeredTexture>();
    if(lLayeredTextureCount > 0)
    {
        for(int j=0; j<lLayeredTextureCount; ++j)
        {
            FbxLayeredTexture *lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
            int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
            pConnectionString += " Texture ";

            for(int k =0; k<lNbTextures; ++k)
            {
                //lConnectionString += k;
                pConnectionString += "\"";
                pConnectionString += (char*)lLayeredTexture->GetName();
                pConnectionString += "\"";
                pConnectionString += " ";
            }
            pConnectionString += "of ";
            pConnectionString += pProperty.GetName();
            pConnectionString += " on layer ";
            pConnectionString += j;
        }
        pConnectionString += " |";
    }
    else
    {
        //no layered texture simply get on the property
        int lNbTextures = pProperty.GetSrcObjectCount<FbxTexture>();

        if(lNbTextures > 0)
        {
            pConnectionString += " Texture ";
            pConnectionString += " ";

            for(int j =0; j<lNbTextures; ++j)
            {
                FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);
                if(lTexture)
                {
                    pConnectionString += "\"";
                    pConnectionString += (char*)lTexture->GetName();
                    pConnectionString += "\"";
                    pConnectionString += " ";
                }
            }
            pConnectionString += "of ";
            pConnectionString += pProperty.GetName();
            pConnectionString += " |";
        }
    }
}

void DisplayMaterialTextureConnections( FbxSurfaceMaterial* pMaterial, char * header, int pMatId, int l )
{
    if(!pMaterial)
        return;

    FbxString lConnectionString = "            Material %d -- ";
    //Show all the textures

    FbxProperty lProperty;
    //Diffuse Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
    DisplayTextureNames(lProperty, lConnectionString);

    //DiffuseFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
    DisplayTextureNames(lProperty, lConnectionString);

    //Emissive Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
    DisplayTextureNames(lProperty, lConnectionString);

    //EmissiveFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sEmissiveFactor);
    DisplayTextureNames(lProperty, lConnectionString);


    //Ambient Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
    DisplayTextureNames(lProperty, lConnectionString); 

    //AmbientFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sAmbientFactor);
    DisplayTextureNames(lProperty, lConnectionString);          

    //Specular Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
    DisplayTextureNames(lProperty, lConnectionString);  

    //SpecularFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
    DisplayTextureNames(lProperty, lConnectionString);

    //Shininess Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
    DisplayTextureNames(lProperty, lConnectionString);

    //Bump Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sBump);
    DisplayTextureNames(lProperty, lConnectionString);

    //Normal Map Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
    DisplayTextureNames(lProperty, lConnectionString);

    //Transparent Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sTransparentColor);
    DisplayTextureNames(lProperty, lConnectionString);

    //TransparencyFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
    DisplayTextureNames(lProperty, lConnectionString);

    //Reflection Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sReflection);
    DisplayTextureNames(lProperty, lConnectionString);

    //ReflectionFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sReflectionFactor);
    DisplayTextureNames(lProperty, lConnectionString);

    //Update header with material info
	bool lStringOverflow = (lConnectionString.GetLen() + 10 >= MAT_HEADER_LENGTH); // allow for string length and some padding for "%d"
	if (lStringOverflow)
	{
		// Truncate string!
		lConnectionString = lConnectionString.Left(MAT_HEADER_LENGTH - 10);
		lConnectionString = lConnectionString + "...";
	}
	FBXSDK_sprintf(header, MAT_HEADER_LENGTH, lConnectionString.Buffer(), pMatId, l);
	DisplayString(header);
}

void DisplayMaterialConnections(FbxMesh* pMesh)
{
    int i, l, lPolygonCount = pMesh->GetPolygonCount();

    char header[MAT_HEADER_LENGTH];

    DisplayString("    Polygons Material Connections");

    //check whether the material maps with only one mesh
    bool lIsAllSame = true;
    for (l = 0; l < pMesh->GetElementMaterialCount(); l++)
    {

        FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(l);
		if( lMaterialElement->GetMappingMode() == FbxGeometryElement::eByPolygon) 
		{
			lIsAllSame = false;
			break;
		}
    }

    //For eAllSame mapping type, just out the material and texture mapping info once
    if(lIsAllSame)
    {
        for (l = 0; l < pMesh->GetElementMaterialCount(); l++)
        {

            FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial( l);
			if( lMaterialElement->GetMappingMode() == FbxGeometryElement::eAllSame) 
			{
				FbxSurfaceMaterial* lMaterial = pMesh->GetNode()->GetMaterial(lMaterialElement->GetIndexArray().GetAt(0));    
				int lMatId = lMaterialElement->GetIndexArray().GetAt(0);
				if(lMatId >= 0)
				{
					DisplayInt("        All polygons share the same material in mesh ", l);
					DisplayMaterialTextureConnections(lMaterial, header, lMatId, l);
				}
			}
        }

		//no material
		if(l == 0)
			DisplayString("        no material applied");
    }

    //For eByPolygon mapping type, just out the material and texture mapping info once
    else
    {
        for (i = 0; i < lPolygonCount; i++)
        {
            DisplayInt("        Polygon ", i);

            for (l = 0; l < pMesh->GetElementMaterialCount(); l++)
            {

                FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial( l);
				FbxSurfaceMaterial* lMaterial = NULL;
				int lMatId = -1;
				lMaterial = pMesh->GetNode()->GetMaterial(lMaterialElement->GetIndexArray().GetAt(i));
				lMatId = lMaterialElement->GetIndexArray().GetAt(i);

				if(lMatId >= 0)
				{
					DisplayMaterialTextureConnections(lMaterial, header, lMatId, l);
				}
            }
        }
    }
}


void DisplayMaterialMapping(FbxMesh* pMesh)
{
	//RootElement->LinkEndChild(MaterialElement);

    const char* lMappingTypes[] = { "None", "By Control Point", "By Polygon Vertex", "By Polygon", "By Edge", "All Same" };
    const char* lReferenceMode[] = { "Direct", "Index", "Index to Direct"};

    int lMtrlCount = 0;
    FbxNode* lNode = NULL;
    if(pMesh){
        lNode = pMesh->GetNode();
        if(lNode)
            lMtrlCount = lNode->GetMaterialCount();    
    }

    for (int l = 0; l < pMesh->GetElementMaterialCount(); l++)
    {
		//MaterialElement->SetAttribute("id", l);
		//MaterialElement->LinkEndChild(MaterialElement);
		//if (l == pMesh->GetElementMaterialCount() - 1){ doc.SaveFile("DisplayMesh.xml"); }
        FbxGeometryElementMaterial* leMat = pMesh->GetElementMaterial( l);
        if (leMat)
        {
            char header[100];
            FBXSDK_sprintf(header, 100, "    Material Element %d: ", l); 
            DisplayString(header);


            DisplayString("           Mapping: ", lMappingTypes[leMat->GetMappingMode()]);
            DisplayString("           ReferenceMode: ", lReferenceMode[leMat->GetReferenceMode()]);

            int lMaterialCount = 0;
            FbxString lString;

            if (leMat->GetReferenceMode() == FbxGeometryElement::eDirect ||
                leMat->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
            {
                lMaterialCount = lMtrlCount;
            }

            if (leMat->GetReferenceMode() == FbxGeometryElement::eIndex ||
                leMat->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
            {
                int i;

                lString = "           Indices: ";

                int lIndexArrayCount = leMat->GetIndexArray().GetCount(); 
                for (i = 0; i < lIndexArrayCount; i++)
                {
                    lString += leMat->GetIndexArray().GetAt(i);

                    if (i < lIndexArrayCount - 1)
                    {
                        lString += ", ";
                    }
                }

                lString += "\n";

                FBXSDK_printf(lString);
            }
        }
    }

    DisplayString("");
}
