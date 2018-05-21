// Record all model structure position/rotation/scale data in time series

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
//using UnityEditor;
//using UnityEditor.Animations;
//using UnityEditor.AnimatedValues;
using System;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;
using System.Xml.Schema;
using System.Xml.XPath;
using System.Xml.Xsl;
//using System.Diagnostics;

public class Script_D19668 : MonoBehaviour
{
    
    public GameObject Amodel;
    public static Animator anim;
    //int layer = 1;
    XmlWriter RecordXml;
    //System.Diagnostics.EventLog eventLog = new System.Diagnostics.EventLog();

    // Use this for initialization
    void Start()
    {
        FileStream fs = new FileStream(Application.persistentDataPath + "\\" + "testUnity.txt", FileMode.Create);
        byte[] data = System.Text.Encoding.Default.GetBytes("output txt test");
        fs.Write(data, 0, data.Length);
        fs.Flush();
        fs.Close();
        Debug.Log(Application.persistentDataPath);
        //FileInfo f = new FileInfo(Application.persistentDataPath + "\\" + "testUnity.txt");
        //StreamWriter w = f.CreateText();
        //w.WriteLine("test txt output");
        //w.Close();

        //if (Amodel == null) { Amodel = GameObject.Find("A_01-D19668"); }
        anim = GetComponent<Animator>();

        //QualitySettings.vSyncCount = 0;
        Debug.Log("!!! Hello Hello!");
        XmlWriterSettings settings = new XmlWriterSettings();
        settings.Indent = true;
        settings.CloseOutput = true;
        settings.IndentChars = "    ";
        settings.Encoding = Encoding.UTF8;

        RecordXml = XmlWriter.Create("RealTimeAni_D19668.xml", settings);
        RecordXml.WriteStartElement("Animation");
    }

    List<string> mapPosition = new List<string>();
    List<string> mapRotation = new List<string>();
    List<string> mapScale = new List<string>();

    public void ViewChildMove(GameObject obj, XmlWriter RecordXml, int layer, ref int NodeIndex, ref int lastlayer)
    {
        //if (obj != null && obj.transform.name.ToString().Substring(0, 6) != "3DGeom")
        if (obj != null)
        {

            layer++;
            for (int nchild = 0; nchild < obj.transform.childCount; nchild++)
            {
                // Geom ==1 means this is no meaning geometry which is auto converts with SimLab (from SolidEdge model)
                int Geom = 0;
                if (obj.transform.GetChild(nchild).name.ToString().Length >= 6 && obj.transform.GetChild(nchild).name.ToString().Substring(0, 6) == "3DGeom")
                {
                    //if (obj.transform.GetChild(nchild).transform.position.ToString() == "(0.0, 0.0, 0.0)"
                    //    && obj.transform.GetChild(nchild).transform.eulerAngles.ToString() == "(0.0, 0.0, 0.0)"
                    //    && obj.transform.GetChild(nchild).transform.localScale.ToString() == "(1.0, 1.0, 1.0)")
                    // {
                    Geom = 1;
                    // }
                }
                if (Time.time == 0 && Geom == 0)
                {
                    NodeIndex++;
                    mapPosition.Add(obj.transform.GetChild(nchild).transform.position.ToString());
                    //mapPosition.Add(obj.transform.GetChild(nchild).name.ToString(), obj.transform.GetChild(nchild).transform.position.ToString());
                    mapRotation.Add(obj.transform.GetChild(nchild).transform.eulerAngles.ToString());
                    mapScale.Add(obj.transform.GetChild(nchild).transform.localScale.ToString());

                    if (lastlayer > layer) // compare the node layer from last XML node, to decide the XML Decrease Indent  <<
                    {
                        //int n = lastlayer - layer;
                        for (int n = 0; n < lastlayer - layer; n++)
                        {
                            RecordXml.WriteEndElement();
                        }
                    }

                    RecordXml.WriteStartElement("Part");
                    RecordXml.WriteAttributeString("name", obj.transform.GetChild(nchild).name.ToString());
                    RecordXml.WriteAttributeString("id", NodeIndex.ToString());
                    //RecordXml.WriteAttributeString("layer", layer.ToString());
                    //RecordXml.WriteAttributeString("lastlayer", lastlayer.ToString());
                    lastlayer = layer;
                    //if (obj.transform.parent != null) { RecordXml.WriteAttributeString("parent", obj.name.ToString()); }
                    if (obj.transform.GetChild(nchild).name.ToString() == "01-D20410.asm:1")
                    {
                        obj.transform.parent.name.ToString();
                        //Debug.Log("@@@@@@ Obj: 20410");
                        //Debug.Log("@@@ parent: " + obj.transform.parent.name.ToString());
                    }
                    RecordXml.WriteElementString("Position", obj.transform.GetChild(nchild).transform.position.ToString());
                    RecordXml.WriteElementString("Rotation", obj.transform.GetChild(nchild).transform.eulerAngles.ToString());
                    RecordXml.WriteElementString("Scale", obj.transform.GetChild(nchild).transform.localScale.ToString());
                    int ischild = 0;
                    for (int nnchild = 0; nnchild < obj.transform.GetChild(nchild).childCount; nnchild++)
                    {
                        if (obj.transform.GetChild(nchild).GetChild(nnchild).name.ToString().Length >= 6 && obj.transform.GetChild(nchild).GetChild(nnchild).name.ToString().Substring(0, 6) != "3DGeom")
                        { ischild++; }
                        if (obj.transform.GetChild(nchild).GetChild(nnchild).name.ToString().Length < 6) { ischild++; }
                    }
                    if (ischild == 0)
                    {
                        RecordXml.WriteEndElement();
                    }

                }

                //Debug.Log("----child- " + obj.transform.GetChild(nchild).name);
                //Debug.Log("@@@@@@ ObjectPosition  : " + obj.transform.GetChild(nchild).transform.position); // this is global position (position + parent+position)
                if (Time.time > 0 && Geom == 0)
                {
                    NodeIndex++;
                    bool diffP = (obj.transform.GetChild(nchild).transform.position.ToString() == mapPosition[NodeIndex]);
                    bool diffR = (obj.transform.GetChild(nchild).transform.eulerAngles.ToString() == mapRotation[NodeIndex]);
                    bool diffS = (obj.transform.GetChild(nchild).transform.localScale.ToString() == mapScale[NodeIndex]);

                    if (diffP == false || diffR == false || diffS == false)
                    {
                        //Debug.Log("----moving child- " + obj.transform.GetChild(nchild).name);
                        //Debug.Log("@@@@@@ ObjectPosition  : " + obj.transform.GetChild(nchild).transform.position); // this is global position (position + parent+position)

                        RecordXml.WriteStartElement("Part");
                        RecordXml.WriteAttributeString("name", obj.transform.GetChild(nchild).name.ToString());
                        RecordXml.WriteAttributeString("id", NodeIndex.ToString());
                        //RecordXml.WriteAttributeString("layer", layer.ToString());
                        //if (obj.transform.parent != null) { RecordXml.WriteAttributeString("parent", obj.name.ToString()); }
                        //RecordXml.WriteAttributeString("position", obj.transform.GetChild(nchild).transform.position.ToString());
                        if (diffP == false)
                        {
                            RecordXml.WriteElementString("Position", obj.transform.GetChild(nchild).transform.position.ToString());
                            //if (obj.transform.GetChild(nchild).name.ToString() == "01-D20206.asm:1") { Debug.Log("!!!!!!!!!!!!!!!!!!!!!!@!@!@ index: " + NodeIndex.ToString()); }
                            mapPosition[NodeIndex] = obj.transform.GetChild(nchild).transform.position.ToString();
                        }
                        if (diffR == false)
                        {
                            RecordXml.WriteElementString("Rotation", obj.transform.GetChild(nchild).transform.eulerAngles.ToString());
                            mapRotation[NodeIndex] = obj.transform.GetChild(nchild).transform.eulerAngles.ToString();
                        }
                        if (diffS == false)
                        {
                            RecordXml.WriteElementString("Scale", obj.transform.GetChild(nchild).transform.localScale.ToString());
                            mapScale[NodeIndex] = obj.transform.GetChild(nchild).transform.localScale.ToString();
                        }
                        RecordXml.WriteEndElement();
                    }
                }

                ViewChildMove(obj.transform.GetChild(nchild).gameObject, RecordXml, layer, ref NodeIndex, ref lastlayer); // view all NodeTree of GameObject

            }
        }
    }
    // Update is called once per frame
    double TimeStep = 0.5;
    double RecordTime = -0.5;  //0 - TimeStep;
    //int speedOn = 1;
    
    // Unity UI bottom ===============================
    public void SpeedUp()
    {
        //speedOn++;
        //Debug.Log("speed up = "+ speedOn.ToString());
        anim.SetFloat("AnimSpeed", anim.GetFloat("AnimSpeed") +1);
    }
    public void SpeedDown()
    {
        //Debug.Log("speed up = "+ speedOn.ToString());
        anim.SetFloat("AnimSpeed", anim.GetFloat("AnimSpeed") - 1);
    }

    public void Exit()
    {
        Debug.Log(" Bye Bye! ");
        Application.Quit();
    }
    //==================================================
    
    void Update()
    {
        //Application.targetFrameRate = 1;
        UnityEngine.SceneManagement.Scene scene = UnityEngine.SceneManagement.SceneManager.GetActiveScene();
        List<GameObject> rootObjects = new List<GameObject>(scene.rootCount + 1);
        scene.GetRootGameObjects(rootObjects);
        //Debug.Log(" --- " + GameObject.Find("Plane").transform.position.z.ToString());

        if (Time.time - RecordTime >= TimeStep  )
        {
            //foreach (EventLogEntry log in eventLog.Entries)
            //{
            //    Console.WriteLine("{0}\nEventLog: ", log.Message);
            //}
            //Debug.Log(" --- " + GameObject.Find("Plane").transform.position.z.ToString());

            RecordTime = Time.time;
            Debug.Log("@@@@@################# Update fun - Hello Object!, time: " + Time.time);
            RecordXml.WriteStartElement("Time");
            RecordXml.WriteAttributeString("second", Time.time.ToString());
            RecordXml.WriteAttributeString("speed", anim.GetFloat("AnimSpeed").ToString());
            int NodeIndex = -1;
            // iterate root objects and do something
            //for (int i = 0; i < rootObjects.Count; i++)
            //{
                int lastlayer = -1;
                int layer = 0;
                //GameObject gameObj = rootObjects[i];
                if (ASMmodel != null)
                {
                    //int NodeIndex = -1;    
                    ViewChildMove(ASMmodel.transform.gameObject, RecordXml, layer, ref NodeIndex, ref lastlayer); // view all NodeTree of GameObject
                    layer--;
                    //Debug.Log(gameObj.name);
                }
            //}
            RecordXml.WriteEndElement();
        }

        if (Time.time > 10)
        {
            RecordXml.WriteFullEndElement();
            RecordXml.Close();
            enabled = true; // will not call Update function anymore!
            //Time.timeScale = 0;
        }
    }
}
