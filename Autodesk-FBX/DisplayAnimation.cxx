/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#include "DisplayCommon.h"
#include "DisplayAnimation.h"
#include "../../../../tinyxml2-master/tinyxml2-master/tinyxml2.h"
#include <string>
#include <cstring>
#include <sstream>


using namespace tinyxml2;
using namespace std;

#if defined (FBXSDK_ENV_MAC)
// disable the “format not a string literal and no format arguments?warning since
// the FBXSDK_printf calls made here are all valid calls and there is no secuity risk
#pragma GCC diagnostic ignored "-Wformat-security"
#endif

void DisplayAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, XMLElement *StackElement, bool isSwitcher = false);
void DisplayAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode, XMLElement *LayerElement, bool isSwitcher = false);
void DisplayAnimation(FbxAudioLayer* pAudioLayer, bool isSwitcher = false);

void DisplayChannels(XMLElement *NodeElement, FbxNode* pNode, FbxAnimLayer* pAnimLayer, void(*DisplayCurve) (FbxAnimCurve* pCurve, XMLElement *TRSElement), void(*DisplayListCurve) (FbxAnimCurve* pCurve, FbxProperty* pProperty), bool isSwitcher);
void DisplayCurveKeys(FbxAnimCurve* pCurve, XMLElement *TRSElement);
void DisplayListCurveKeys(FbxAnimCurve* pCurve, FbxProperty* pProperty);
XMLDocument xml_doc;
//XMLElement *AniElement = xml_doc.NewElement("DisplayAnimation");

void DisplayAnimation(FbxScene* pScene)
{
	//XMLDocument xml_doc;
	xml_doc.LoadFile("DisplayMesh.xml");
	//XMLError eResult = xml_doc.LoadFile("DisplayMesh.xml");
	XMLNode* root = xml_doc.RootElement(); 
	if (root == nullptr) { FBXSDK_printf("Null RootNode in DisplayMesh.xml !"); }

	XMLElement *AniElement = xml_doc.NewElement("Animation");//
	root->InsertEndChild(AniElement);


    int i;
    for (i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
    {
		XMLElement *StackElement = xml_doc.NewElement("Animation_Stack");
		//root->InsertEndChild(StackElement);
		AniElement->LinkEndChild(StackElement);

        FbxAnimStack* lAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);

        FbxString lOutputString = "Animation Stack Name: ";
        lOutputString += lAnimStack->GetName();
        lOutputString += "\n";
        FBXSDK_printf(lOutputString);

		StackElement->SetAttribute("StackName", lAnimStack->GetName());
		StackElement->SetAttribute("layer_count", pScene->GetSrcObject<FbxAnimStack>(i)->GetMemberCount<FbxAnimLayer>());
		//XMLText *nameContent = xml_doc.NewText("flower");
		//StackElement->LinkEndChild(nameContent);

		DisplayAnimation(lAnimStack, pScene->GetRootNode(), StackElement);
    }

	XMLDocument xml_doc2;
	XMLError eResult2 = xml_doc2.LoadFile("PhyProperty.xml");
	XMLNode* root2 = xml_doc2.RootElement(); //
	//if (root == nullptr) return tinyxml2::XML_ERROR_FILE_READ_ERROR;
	//XMLElement* element = root->FirstChildElement("Stu");

	//---------------------

	XMLElement *PhysicalElement = xml_doc.NewElement("PhysicalData");//Stu  
	//xml_doc.InsertFirstChild(TeaElement);
	root->InsertFirstChild(PhysicalElement);

	for (XMLElement* child = root2->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
	{
		XMLElement *PhyElement = xml_doc.NewElement("Part");
		PhysicalElement->LinkEndChild(PhyElement);
		const char * name;
		name = child->Attribute("File");
		//cout << "PART file: " << name << endl;
		PhyElement->SetAttribute("File", name);

		for (XMLElement* child2 = child->FirstChildElement(); child2 != NULL; child2 = child2->NextSiblingElement())
		{
			string name2 = child2->Name();
			XMLElement *ProElement = xml_doc.NewElement("Property");
			PhyElement->LinkEndChild(ProElement);
			ProElement->SetAttribute("parameter", child2->Name());
			XMLText *dataContent = xml_doc.NewText(child2->GetText());
			ProElement->LinkEndChild(dataContent);
			//cout << "Node name: " << name2 << ", value: " << child2->GetText() << endl;
			//system("pause");
		}

		//system("pause");
	}



	xml_doc.SaveFile("DisplayModel.xml");
}

void DisplayAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, XMLElement *StackElement, bool isSwitcher)
{
    int l;
    int nbAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
	int nbAudioLayers = pAnimStack->GetMemberCount<FbxAudioLayer>();
    FbxString lOutputString;

    lOutputString = "   contains ";
	if (nbAnimLayers==0 && nbAudioLayers==0)
		lOutputString += "no layers";

	if (nbAnimLayers)
	{
		lOutputString += nbAnimLayers;
		lOutputString += " Animation Layer";
		if (nbAnimLayers > 1)
			lOutputString += "s";
	}

	if (nbAudioLayers)
	{
		if (nbAnimLayers)
			lOutputString += " and ";

		lOutputString += nbAudioLayers;
		lOutputString += " Audio Layer";
		if (nbAudioLayers > 1)
			lOutputString += "s";
	}
	lOutputString += "\n\n";
    FBXSDK_printf(lOutputString);

    for (l = 0; l < nbAnimLayers; l++)
    {
		XMLElement *LayerElement = xml_doc.NewElement("AnimLayer");
		StackElement->LinkEndChild(LayerElement);
		LayerElement->SetAttribute("id", l);

        FbxAnimLayer* lAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(l);

        lOutputString = "AnimLayer ";
        lOutputString += l;
        lOutputString += "\n";
        FBXSDK_printf(lOutputString);

        DisplayAnimation(lAnimLayer, pNode, LayerElement, isSwitcher);
    }

	for (l = 0; l < nbAudioLayers; l++)
	{
		FbxAudioLayer* lAudioLayer = pAnimStack->GetMember<FbxAudioLayer>(l);

		lOutputString = "AudioLayer ";
		lOutputString += l;
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);

		DisplayAnimation(lAudioLayer, isSwitcher);
		FBXSDK_printf("\n");
	}
}

void DisplayAnimation(FbxAudioLayer* pAudioLayer, bool)
{
    int lClipCount;
    FbxString lOutputString;

	lClipCount = pAudioLayer->GetMemberCount<FbxAudio>();
	
    lOutputString = "     Name: ";
    lOutputString += pAudioLayer->GetName();
	lOutputString += "\n\n";
	lOutputString += "     Nb Audio Clips: ";
	lOutputString += lClipCount;
    lOutputString += "\n";
    FBXSDK_printf(lOutputString);

	for (int i = 0; i < lClipCount; i++)
	{
		FbxAudio* lClip = pAudioLayer->GetMember<FbxAudio>(i);
		lOutputString = "        Clip[";
		lOutputString += i;
		lOutputString += "]:\t";
		lOutputString += lClip->GetName();
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);
	}
}

void DisplayAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode, XMLElement *LayerElement, bool isSwitcher)
{
	int lModelCount;
	if (pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X) ||
		pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y) ||
		pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z) ||
		pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X) ||
		pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y) ||
		pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z) ||
		pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X) ||
		pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y) ||
		pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z)
		)
	{
		XMLElement *NodeElement = xml_doc.NewElement("Part");
		LayerElement->LinkEndChild(NodeElement);
		NodeElement->SetAttribute("name", pNode->GetName());

		FbxString lOutputString;

		lOutputString = "     Node Name: ";
		lOutputString += pNode->GetName();
		lOutputString += "\n\n";
		FBXSDK_printf(lOutputString);

		DisplayChannels(NodeElement, pNode, pAnimLayer, DisplayCurveKeys, DisplayListCurveKeys, isSwitcher);
		FBXSDK_printf("\n");
	}

    for(lModelCount = 0; lModelCount < pNode->GetChildCount(); lModelCount++)
    {
        DisplayAnimation(pAnimLayer, pNode->GetChild(lModelCount), LayerElement, isSwitcher);
    }
}


void DisplayChannels(XMLElement *NodeElement, FbxNode* pNode, FbxAnimLayer* pAnimLayer, void(*DisplayCurve) (FbxAnimCurve* pCurve, XMLElement *TRSElement), void(*DisplayListCurve) (FbxAnimCurve* pCurve, FbxProperty* pProperty), bool isSwitcher)
{
	//XMLElement *TRSElement = xml_doc.NewElement("Channel");
	//NodeElement->LinkEndChild(TRSElement);

	//XMLText *nameContent = xml_doc.NewText("flowier");
	//TRSElement->LinkEndChild(nameContent);

    FbxAnimCurve* lAnimCurve = NULL;

    // Display general curves.
    if (!isSwitcher)
    {
		int withAnim = 0;
        lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if (lAnimCurve)
        {
			withAnim++;
            FBXSDK_printf("        TX\n");

			XMLElement *TRSElement = xml_doc.NewElement("Tx");
			NodeElement->LinkEndChild(TRSElement);

			//XMLText *nameContent = xml_doc.NewText("Tx");
			//TRSElement->LinkEndChild(nameContent);
            DisplayCurve(lAnimCurve, TRSElement);
        }
        lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if (lAnimCurve)
        {
			withAnim++;
            FBXSDK_printf("        TY\n");

			XMLElement *TRSElement = xml_doc.NewElement("Ty");
			NodeElement->LinkEndChild(TRSElement);

			//XMLText *nameContent = xml_doc.NewText("Ty");
			//TRSElement->LinkEndChild(nameContent);
			DisplayCurve(lAnimCurve, TRSElement);
        }
        lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if (lAnimCurve)
        {
			withAnim++;
            FBXSDK_printf("        TZ\n");
			XMLElement *TRSElement = xml_doc.NewElement("Tz");
			NodeElement->LinkEndChild(TRSElement);

			//XMLText *nameContent = xml_doc.NewText("Tz");
			//TRSElement->LinkEndChild(nameContent);
			DisplayCurve(lAnimCurve, TRSElement);
        }
		if (withAnim > 0)
		{
			//system("pause");
		}
        lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if (lAnimCurve)
        {
			withAnim++;
            FBXSDK_printf("        RX\n");
			XMLElement *TRSElement = xml_doc.NewElement("Rx");
			NodeElement->LinkEndChild(TRSElement);

			DisplayCurve(lAnimCurve, TRSElement);
        }
        lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if (lAnimCurve)
        {
			withAnim++;
            FBXSDK_printf("        RY\n");
			XMLElement *TRSElement = xml_doc.NewElement("Ry");
			NodeElement->LinkEndChild(TRSElement);

			DisplayCurve(lAnimCurve, TRSElement);
        }
        lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if (lAnimCurve)
        {
			withAnim++;
            FBXSDK_printf("        RZ\n");
			XMLElement *TRSElement = xml_doc.NewElement("Rz");
			NodeElement->LinkEndChild(TRSElement);
			DisplayCurve(lAnimCurve, TRSElement);
        }
		if (withAnim > 0)
		{
			//system("pause");
		}
        lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if (lAnimCurve)
        {
			withAnim++;
            FBXSDK_printf("        SX\n");
			XMLElement *TRSElement = xml_doc.NewElement("Sx");
			NodeElement->LinkEndChild(TRSElement);

			DisplayCurve(lAnimCurve, TRSElement);
        }    
        lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if (lAnimCurve)
        {
			withAnim++;
            FBXSDK_printf("        SY\n");
			XMLElement *TRSElement = xml_doc.NewElement("Sy");
			NodeElement->LinkEndChild(TRSElement);

			DisplayCurve(lAnimCurve, TRSElement);
        }
        lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if (lAnimCurve)
        {
			withAnim++;
            FBXSDK_printf("        SZ\n");
			XMLElement *TRSElement = xml_doc.NewElement("Sz");
			NodeElement->LinkEndChild(TRSElement);

			DisplayCurve(lAnimCurve, TRSElement);
        }
		if (withAnim > 0)
		{
			FBXSDK_printf("^^ === DisplayAnimation === \n\n");
			//system("pause");
		}
    }

    // Display curves specific to a light or marker.
    FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

    if (lNodeAttribute)
    {
        lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_RED);
        if (lAnimCurve)
        {
            FBXSDK_printf("        Red\n");
            //DisplayCurve(lAnimCurve);
        }
        lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_GREEN);
        if (lAnimCurve)
        {
            FBXSDK_printf("        Green\n");
            //DisplayCurve(lAnimCurve);
        }
        lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_BLUE);
        if (lAnimCurve)
        {
            FBXSDK_printf("        Blue\n");
            //DisplayCurve(lAnimCurve);
        }

        // Display curves specific to a light.
        FbxLight* light = pNode->GetLight();
        if (light)
        {
            lAnimCurve = light->Intensity.GetCurve(pAnimLayer);
            if (lAnimCurve)
            {
                FBXSDK_printf("        Intensity\n");
                //DisplayCurve(lAnimCurve);
            }

            lAnimCurve = light->OuterAngle.GetCurve(pAnimLayer);
            if (lAnimCurve)
            {
                FBXSDK_printf("        Outer Angle\n");
                //DisplayCurve(lAnimCurve);
            }

            lAnimCurve = light->Fog.GetCurve(pAnimLayer);
            if (lAnimCurve)
            {
                FBXSDK_printf("        Fog\n");
               // DisplayCurve(lAnimCurve);
            }
        }

        // Display curves specific to a camera.
        FbxCamera* camera = pNode->GetCamera();
        if (camera)
        {
            lAnimCurve = camera->FieldOfView.GetCurve(pAnimLayer);
            if (lAnimCurve)
            {
                FBXSDK_printf("        Field of View\n");
                //DisplayCurve(lAnimCurve);
            }

            lAnimCurve = camera->FieldOfViewX.GetCurve(pAnimLayer);
            if (lAnimCurve)
            {
                FBXSDK_printf("        Field of View X\n");
                //DisplayCurve(lAnimCurve);
            }

            lAnimCurve = camera->FieldOfViewY.GetCurve(pAnimLayer);
            if (lAnimCurve)
            {
                FBXSDK_printf("        Field of View Y\n");
                //DisplayCurve(lAnimCurve);
            }

            lAnimCurve = camera->OpticalCenterX.GetCurve(pAnimLayer);
            if (lAnimCurve)
            {
                FBXSDK_printf("        Optical Center X\n");
                //DisplayCurve(lAnimCurve);
            }

            lAnimCurve = camera->OpticalCenterY.GetCurve(pAnimLayer);
            if(lAnimCurve)
            {
                FBXSDK_printf("        Optical Center Y\n");
                //DisplayCurve(lAnimCurve);
            }

            lAnimCurve = camera->Roll.GetCurve(pAnimLayer);
            if(lAnimCurve)
            {
                FBXSDK_printf("        Roll\n");
                //DisplayCurve(lAnimCurve);
            }
        }

        // Display curves specific to a geometry.
        if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
            lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
            lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
        {
            FbxGeometry* lGeometry = (FbxGeometry*) lNodeAttribute;

			int lBlendShapeDeformerCount = lGeometry->GetDeformerCount(FbxDeformer::eBlendShape);
			for(int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
			{
				FbxBlendShape* lBlendShape = (FbxBlendShape*)lGeometry->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);

				int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
				for(int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; ++lChannelIndex)
				{
					FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
					const char* lChannelName = lChannel->GetName();

					lAnimCurve = lGeometry->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer, true);
					if(lAnimCurve)
					{
						FBXSDK_printf("        Shape %s\n", lChannelName);
						//DisplayCurve(lAnimCurve);
					}
				}
			}
        }
    }

    // Display curves specific to properties
    FbxProperty lProperty = pNode->GetFirstProperty();
    while (lProperty.IsValid())
    {
        if (lProperty.GetFlag(FbxPropertyFlags::eUserDefined))
        {
            FbxString lFbxFCurveNodeName  = lProperty.GetName();
            FbxAnimCurveNode* lCurveNode = lProperty.GetCurveNode(pAnimLayer);

            if (!lCurveNode){
                lProperty = pNode->GetNextProperty(lProperty);
                continue;
            }

            FbxDataType lDataType = lProperty.GetPropertyDataType();
			if (lDataType.GetType() == eFbxBool || lDataType.GetType() == eFbxDouble || lDataType.GetType() == eFbxFloat || lDataType.GetType() == eFbxInt)
            {
                FbxString lMessage;

                lMessage =  "        Property ";
                lMessage += lProperty.GetName();
                if (lProperty.GetLabel().GetLen() > 0)
                {
                    lMessage += " (Label: ";
                    lMessage += lProperty.GetLabel();
                    lMessage += ")";
                };

                DisplayString(lMessage.Buffer());

                for( int c = 0; c < lCurveNode->GetCurveCount(0U); c++ )
                {
                    lAnimCurve = lCurveNode->GetCurve(0U, c);
                    //if (lAnimCurve)
                        //DisplayCurve(lAnimCurve);
                }
            }
			else if(lDataType.GetType() == eFbxDouble3 || lDataType.GetType() == eFbxDouble4 || lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT))
            {
				char* lComponentName1 = (lDataType.Is(FbxColor3DT) ||lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_RED : (char*)"X";
                char* lComponentName2 = (lDataType.Is(FbxColor3DT) ||lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_GREEN : (char*)"Y";
                char* lComponentName3 = (lDataType.Is(FbxColor3DT) ||lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_BLUE  : (char*)"Z";
                FbxString      lMessage;
                
				lMessage =  "        Property ";
                lMessage += lProperty.GetName();
                if (lProperty.GetLabel().GetLen() > 0)
                {
                    lMessage += " (Label: ";
                    lMessage += lProperty.GetLabel();
                    lMessage += ")";
                }
                DisplayString(lMessage.Buffer());

                for( int c = 0; c < lCurveNode->GetCurveCount(0U); c++ )
                {
                    lAnimCurve = lCurveNode->GetCurve(0U, c);
                    if (lAnimCurve)
                    {
                        DisplayString("        Component ", lComponentName1);
                        //DisplayCurve(lAnimCurve);
                    }
                }

                for( int c = 0; c < lCurveNode->GetCurveCount(1U); c++ )
                {
                    lAnimCurve = lCurveNode->GetCurve(1U, c);
                    if (lAnimCurve)
                    {
                        DisplayString("        Component ", lComponentName2);
                        //DisplayCurve(lAnimCurve);
                    }
                }

                for( int c = 0; c < lCurveNode->GetCurveCount(2U); c++ )
                {
                    lAnimCurve = lCurveNode->GetCurve(2U, c);
                    if (lAnimCurve)
                    {
                        DisplayString("        Component ", lComponentName3);
                        //DisplayCurve(lAnimCurve);
                    }
                }
            }
			else if (lDataType.GetType() == eFbxEnum)
            {
                FbxString lMessage;

				lMessage =  "        Property ";
                lMessage += lProperty.GetName();
                if (lProperty.GetLabel().GetLen() > 0)
                {
                    lMessage += " (Label: ";
                    lMessage += lProperty.GetLabel();
                    lMessage += ")";
                };
                DisplayString(lMessage.Buffer());

                for( int c = 0; c < lCurveNode->GetCurveCount(0U); c++ )
                {
                    lAnimCurve = lCurveNode->GetCurve(0U, c);
                    if (lAnimCurve)
                        DisplayListCurve(lAnimCurve, &lProperty);
                }                
            }
        }

        lProperty = pNode->GetNextProperty(lProperty);
    } // while

}


static int InterpolationFlagToIndex(int flags)
{
	if( (flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant ) return 1;
    if( (flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear ) return 2;
	if( (flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic ) return 3;
    return 0;
}

static int ConstantmodeFlagToIndex(int flags)
{
    if( (flags & FbxAnimCurveDef::eConstantStandard) == FbxAnimCurveDef::eConstantStandard ) return 1;
    if( (flags & FbxAnimCurveDef::eConstantNext) == FbxAnimCurveDef::eConstantNext ) return 2;
    return 0;
}

static int TangentmodeFlagToIndex(int flags)
{
    if( (flags & FbxAnimCurveDef::eTangentAuto) == FbxAnimCurveDef::eTangentAuto ) return 1;
    if( (flags & FbxAnimCurveDef::eTangentAutoBreak)== FbxAnimCurveDef::eTangentAutoBreak ) return 2;
    if( (flags & FbxAnimCurveDef::eTangentTCB) == FbxAnimCurveDef::eTangentTCB ) return 3;
    if( (flags & FbxAnimCurveDef::eTangentUser) == FbxAnimCurveDef::eTangentUser ) return 4;
    if( (flags & FbxAnimCurveDef::eTangentGenericBreak) == FbxAnimCurveDef::eTangentGenericBreak ) return 5;
    if( (flags & FbxAnimCurveDef::eTangentBreak) == FbxAnimCurveDef::eTangentBreak ) return 6;
    return 0;
}

static int TangentweightFlagToIndex(int flags)
{
    if( (flags & FbxAnimCurveDef::eWeightedNone) == FbxAnimCurveDef::eWeightedNone ) return 1;
    if( (flags & FbxAnimCurveDef::eWeightedRight) == FbxAnimCurveDef::eWeightedRight ) return 2;
    if( (flags & FbxAnimCurveDef::eWeightedNextLeft) == FbxAnimCurveDef::eWeightedNextLeft ) return 3;
    return 0;
}

static int TangentVelocityFlagToIndex(int flags)
{
    if( (flags & FbxAnimCurveDef::eVelocityNone) == FbxAnimCurveDef::eVelocityNone ) return 1;
    if( (flags & FbxAnimCurveDef::eVelocityRight) == FbxAnimCurveDef::eVelocityRight ) return 2;
    if( (flags & FbxAnimCurveDef::eVelocityNextLeft) == FbxAnimCurveDef::eVelocityNextLeft ) return 3;
    return 0;
}

void DisplayCurveKeys(FbxAnimCurve* pCurve, XMLElement *TRSElement)
{
    static const char* interpolation[] = { "?", "constant", "linear", "cubic"};
    static const char* constantMode[] =  { "?", "Standard", "Next" };
    static const char* cubicMode[] =     { "?", "Auto", "Auto break", "Tcb", "User", "Break", "User break" };
    static const char* tangentWVMode[] = { "?", "None", "Right", "Next left" };

    FbxTime   lKeyTime;
    float   lKeyValue;
	float   lKeyLeftV;
	float   lKeyRightV;
    char    lTimeString[256];
    FbxString lOutputString;
    int     lCount;

    int lKeyCount = pCurve->KeyGetCount();

	XMLElement *FrameRateElement = xml_doc.NewElement("FrameRate");
	TRSElement->LinkEndChild(FrameRateElement);
	std::stringstream ss;
	ss << lKeyTime.GetFrameRate(lKeyTime.GetGlobalTimeMode());
	XMLText *FrameRateContent = xml_doc.NewText(ss.str().c_str());
	FrameRateElement->LinkEndChild(FrameRateContent);

    for(lCount = 0; lCount < lKeyCount; lCount++)
    {
		lKeyValue = static_cast<float>(pCurve->KeyGetValue(lCount));
		lKeyTime = pCurve->KeyGetTime(lCount);
		lKeyLeftV = static_cast<float>(pCurve->KeyGetLeftTangentVelocity(lCount));
		lKeyRightV = static_cast<float>(pCurve->KeyGetRightTangentVelocity(lCount));

		XMLElement *KeyElement = xml_doc.NewElement("Key");
		TRSElement->LinkEndChild(KeyElement);
		//KeyElement->SetAttribute("KeyTime", lKeyTime.GetTimeString(lTimeString, FbxUShort(256)));
		KeyElement->SetAttribute("Time_ms", lKeyTime.GetMilliSeconds());
		//KeyElement->SetAttribute("FrameRate", lKeyTime.GetFrameRate(lKeyTime.GetGlobalTimeMode()));


		std::stringstream s_val;
		s_val << lKeyValue;
		XMLText *keyContent = xml_doc.NewText(s_val.str().c_str());
		KeyElement->LinkEndChild(keyContent);

        lOutputString = "            Key Time: ";
        lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
        lOutputString += ".... Key Value: ";
        lOutputString += lKeyValue;
		//lOutputString += ".... Key R_Velocity: ";
		//lOutputString += lKeyRightV;
		//lOutputString += ".... Key L_Velocity: ";
		//ls -alOutputString += lKeyLeftV;
        lOutputString += " [ ";
        lOutputString += interpolation[ InterpolationFlagToIndex(pCurve->KeyGetInterpolation(lCount)) ];
        if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
        {
            lOutputString += " | ";
            lOutputString += constantMode[ ConstantmodeFlagToIndex(pCurve->KeyGetConstantMode(lCount)) ];
        }
        else if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
        {
            lOutputString += " | ";
            lOutputString += cubicMode[ TangentmodeFlagToIndex(pCurve->KeyGetTangentMode(lCount)) ];
            lOutputString += " | ";
			lOutputString += tangentWVMode[ TangentweightFlagToIndex(pCurve->KeyGet(lCount).GetTangentWeightMode()) ];
            lOutputString += " | ";
			lOutputString += tangentWVMode[ TangentVelocityFlagToIndex(pCurve->KeyGet(lCount).GetTangentVelocityMode()) ];
        }
        lOutputString += " ]";
        lOutputString += "\n";
        FBXSDK_printf (lOutputString);
    }

}

void DisplayListCurveKeys(FbxAnimCurve* pCurve, FbxProperty* pProperty)
{
    FbxTime   lKeyTime;
    int     lKeyValue;
    char    lTimeString[256];
    FbxString lListValue;
    FbxString lOutputString;
    int     lCount;

    int lKeyCount = pCurve->KeyGetCount();

    for(lCount = 0; lCount < lKeyCount; lCount++)
    {
        lKeyValue = static_cast<int>(pCurve->KeyGetValue(lCount));
        lKeyTime  = pCurve->KeyGetTime(lCount);

        lOutputString = "            Key Time: ";
        lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
        lOutputString += ".... Key Value: ";
        lOutputString += lKeyValue;
        lOutputString += " (";
        lOutputString += pProperty->GetEnumValue(lKeyValue);
        lOutputString += ")";

        lOutputString += "\n";
        FBXSDK_printf (lOutputString);

    }
	FBXSDK_printf("^^ === DisplayAnimation === \n\n");
	//system("pause");
}
