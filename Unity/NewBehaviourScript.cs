using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEditor.Animations;
using UnityEditor.AnimatedValues;
using System;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;
using System.Xml.Schema;
using System.Xml.XPath;
using System.Xml.Xsl;


public class NewBehaviourScript : MonoBehaviour {
    
    public static void ViewChild(GameObject obj, int layer)
    {
        if (obj != null)
        {
            layer++;
            for (int nchild = 0; nchild < obj.transform.childCount; nchild++)
            {
                if (obj.GetComponent<MeshFilter>() != null)
                {

                    Mesh mesh = obj.GetComponent<MeshFilter>().mesh;
                    Debug.Log("------mesh name: " + mesh.name);
                    Debug.Log("mesh vertice: " + mesh.vertices);
                    Debug.Log("mesh normals: " + mesh.normals);
                }

                Debug.Log("----child- " + obj.transform.GetChild(nchild).name +", layer = "+ layer);
                Debug.Log("@@@@@@ ObjectPosition  : " + obj.transform.GetChild(nchild).transform.position); // this is global position (position + parent+position)
                //Debug.Log("@@@@@@ GlobalPosition  : " + obj.transform.GetChild(nchild).transform.TransformDirection(obj.transform.position));
                ViewChild(obj.transform.GetChild(nchild).gameObject, layer); // view all NodeTree of GameObject
            }
        }
    }
  
    public Animator anim;
    int layer = 1;
    XmlWriter RecordXml;
    // Use this for initialization
    void Start ()
    {
        Debug.Log("!!! Hello Hello!");
        XmlWriterSettings settings = new XmlWriterSettings();
        settings.Indent = true;
        settings.CloseOutput = true;
        settings.IndentChars = "    ";
        settings.Encoding = Encoding.UTF8;
        XmlWriter writerXml = XmlWriter.Create("UnityAni.xml", settings);

        writerXml.WriteStartElement("Animation");


        RecordXml = XmlWriter.Create("RealTimeAni.xml", settings);
        RecordXml.WriteStartElement("Animation");


        // get root objects in scene
        UnityEngine.SceneManagement.Scene scene = UnityEngine.SceneManagement.SceneManager.GetActiveScene();
        List<GameObject> rootObjects = new List<GameObject>(scene.rootCount + 1);
        scene.GetRootGameObjects(rootObjects);

        

        // iterate root objects and do something
        for (int i = 0; i < rootObjects.Count; i++)
        {
            Debug.Log("## Hello Object!");
            GameObject gameObj = rootObjects[i];
            if (gameObj != null)
            {
                ViewChild(gameObj.transform.gameObject, layer); // view all NodeTree of GameObject
                layer--;
                Debug.Log(gameObj.name);
            }
        }


        anim = GetComponent<Animator>();
        Debug.Log("$ anim name: " + anim.ToString());
        string assetPath = AssetDatabase.GetAssetPath(anim.runtimeAnimatorController);
        AnimatorController controller = AssetDatabase.LoadAssetAtPath<AnimatorController>(assetPath);
        Debug.Log("$ control name: " + controller.ToString());


        //////// loop by layer to get animation in each layer (object)
        for (int j = 0; j < controller.layers.Length; j++)
        {
            writerXml.WriteStartElement("AnimationLayer");
            writerXml.WriteAttributeString("name", (controller.layers[j].name).ToString());
            if (j == 0) { writerXml.WriteAttributeString("weight", "1"); }
            if (j != 0) { writerXml.WriteAttributeString("weight", (controller.layers[j].defaultWeight).ToString()); }
            writerXml.WriteAttributeString("iKPass", (controller.layers[j].iKPass).ToString());
            if (controller.layers[j].syncedLayerAffectsTiming == true)
            {
                writerXml.WriteAttributeString("SyncLayer", (controller.layers[j].syncedLayerIndex).ToString());
            }

            if (controller.layers[j].avatarMask)
            {
                
                Debug.Log("! ! ! ! Controller layer  " + j);
                Debug.Log("! Controller layer avatar  : " + controller.layers[j].avatarMask.name);
                Debug.Log("! Controller layer avatar transformCount : " + controller.layers[j].avatarMask.transformCount);
                writerXml.WriteStartElement("AvatarMask");
                for (int k = 0; k< controller.layers[j].avatarMask.transformCount; k++)
                {
                    
                    if (controller.layers[j].avatarMask.GetTransformActive(k) == true)
                    {
                        //writerXml.WriteStartElement("AvatarMask");
                        //Debug.Log("! C : " + controller.layers[j].avatarMask.GetTransformActive(k).GetHashCode());
                        writerXml.WriteElementString(controller.layers[j].avatarMask.name, k.ToString());
                        //writerXml.WriteEndElement();
                    }   
                }
                writerXml.WriteEndElement();
                //Debug.Log("! Controller layer avatar transform active : " + controller.layers[j].avatarMask.GetTransformActive(0));
            }
            AnimatorClipInfo[] m_AnimatorClipInfo = anim.GetCurrentAnimatorClipInfo(j); // (0):layer 0
            //anim.GetNextAnimatorClipInfo(j);
            Debug.Log("Starting clip[0] : " + m_AnimatorClipInfo[0].clip);
            Debug.Log("     clipInfo length : " + m_AnimatorClipInfo.Length);
            //Debug.Log("           duration : " + m_AnimatorClipInfo[0].clip.length);
            //Debug.Log("           event # : " + m_AnimatorClipInfo[0].clip.);
            Debug.Log("           speed : " + m_AnimatorClipInfo[0].clip.averageSpeed);
            for (int c = 0; c < m_AnimatorClipInfo.Length; c++) // run through each AniClip
            { 
                writerXml.WriteStartElement("AnimationClip");
                writerXml.WriteAttributeString("name", (m_AnimatorClipInfo[c].clip).ToString());
                Debug.Log("Starting clip[c] : " + m_AnimatorClipInfo[c].clip);
                foreach (var binding in AnimationUtility.GetCurveBindings(m_AnimatorClipInfo[c].clip))
                {

                    //GameObject.Find((m_AnimatorClipInfo[c].clip).ToString());
                    writerXml.WriteStartElement(binding.propertyName);
                    for (int io = 0; io < rootObjects.Count; io++)
                    {
                        GameObject gameObj = rootObjects[io];
                        if (gameObj != null && AnimationUtility.GetAnimatedObject(gameObj, binding)!= null)
                        {
                            writerXml.WriteElementString("Part", AnimationUtility.GetAnimatedObject(gameObj, binding).ToString());
                            //writerXml.WriteAttributeString("Part", AnimationUtility.GetAnimatedObject(gameObject, binding).ToString());
                            Debug.Log("@@@@@@ AnimatedObject : " + AnimationUtility.GetAnimatedObject(gameObj, binding));
                            GameObject Objj = GameObject.Find(AnimationUtility.GetAnimatedObject(gameObj, binding).ToString()
                                .Substring(0, (AnimationUtility.GetAnimatedObject(gameObj, binding).ToString()).Length - 24));
                            Debug.Log("@@@@@@ ObjectPosition : " + Objj.transform.position);  // this is global position (position + parent+position)
                            //Debug.Log("@@@@@@ GlobalPosition : " + Objj.transform.TransformDirection(Objj.transform.position));
                            writerXml.WriteElementString("PartPosition", Objj.transform.position.ToString());  // this is global position (position + parent+position)
                            //writerXml.WriteElementString("PartLPosition", Objj.transform.TransformDirection(Objj.transform.position).ToString());
                        }
                    }
                    
                   
                    Debug.Log("@@@ curve binding : " + binding.propertyName);
                    AnimationCurve curve = AnimationUtility.GetEditorCurve(m_AnimatorClipInfo[c].clip, binding);
                    //Debug.Log("@@@ curve length : " + curve.length);
                    //Debug.Log("@@@ curve keys length : " + curve.keys.Length);
                    for (int ii = 0; ii < curve.keys.Length; ii++)
                    {

                        writerXml.WriteStartElement("Key");
                        writerXml.WriteAttributeString("time", (curve.keys[ii].time).ToString());
                        writerXml.WriteString((curve.keys[ii].value).ToString());
                        writerXml.WriteEndElement();
                        //writerXml.WriteElementString((curve.keys[ii].time).ToString(), (curve.keys[ii].value).ToString());
                        Debug.Log("@ curve keys[ii] time : " + curve.keys[ii].time + ", value: " + curve.keys[ii].value);
                        //Debug.Log("@ curve keys[i] value : " + curve.keys[i].value);
                    }
                    writerXml.WriteEndElement();
                    //Debug.Log("@@@ curve keys[0] time : " + curve.keys[0].time);
                }
                writerXml.WriteEndElement();
            }
            writerXml.WriteEndElement();
        }
        
        //UnityEditor.Animations.AnimatorController ac = Selection.activeObject as UnityEditor.Animations.AnimatorController;
        Debug.Log("Controller name : " + controller.name);
        Debug.Log("Controller layer# : " + controller.layers.Length);
        //Debug.Log("Controller layer avatar : " + controller.layers[0].avatarMask.name);
        //Debug.Log("Controller layer[1] name : " + controller.layers[1].name);
        Debug.Log("Controller layer[0] state machine : " + controller.layers[0].stateMachine.name);
        Debug.Log("Controller layer[0] timing : " + controller.layers[0].syncedLayerAffectsTiming);
        //Debug.Log("Controller layer iK : " + controller.layers[0].iKPass);
        //Debug.Log("Time : " + GetComponent<Animator>().runtimeAnimatorController.animationClips[0].length); // 1.8333 my duration of animation in animator

        //Debug.Log(anim.runtimeAnimatorController.animationClips);

        writerXml.WriteFullEndElement();
        writerXml.Close();
    }


    public static void ViewChildMove(GameObject obj, XmlWriter RecordXml)
    {
        if (obj != null)
        {

            for (int nchild = 0; nchild < obj.transform.childCount; nchild++)
            {
                Debug.Log("----child- " + obj.transform.GetChild(nchild).name);
                Debug.Log("@@@@@@ ObjectPosition  : " + obj.transform.GetChild(nchild).transform.position); // this is global position (position + parent+position)
                RecordXml.WriteStartElement("Part");
                RecordXml.WriteAttributeString("name", obj.transform.GetChild(nchild).name.ToString());
                //RecordXml.WriteAttributeString("position", obj.transform.GetChild(nchild).transform.position.ToString());
                RecordXml.WriteElementString("Position", obj.transform.GetChild(nchild).transform.position.ToString());
                RecordXml.WriteElementString("Rotation", obj.transform.GetChild(nchild).transform.eulerAngles.ToString());
                RecordXml.WriteElementString("Scale", obj.transform.GetChild(nchild).transform.localScale.ToString());

                RecordXml.WriteEndElement();

                ViewChildMove(obj.transform.GetChild(nchild).gameObject, RecordXml); // view all NodeTree of GameObject
            }
        }
    }

    // Update is called once per frame
    float RecordTime = -2;
    //TextWriter txt;


    void Update () {

        UnityEngine.SceneManagement.Scene scene = UnityEngine.SceneManagement.SceneManager.GetActiveScene();
        List<GameObject> rootObjects = new List<GameObject>(scene.rootCount + 1);
        scene.GetRootGameObjects(rootObjects);

        if (Time.time - RecordTime > 1)
        {

            RecordTime = Time.time;
            Debug.Log("################################ Update fun - Hello Object!, time: " + Time.time);
            RecordXml.WriteStartElement("Time");
            RecordXml.WriteAttributeString("second", Time.time.ToString());

            // iterate root objects and do something
            for (int i = 0; i < rootObjects.Count; i++)
            {

                GameObject gameObj = rootObjects[i];
                if (gameObj != null)
                {
                    ViewChildMove(gameObj.transform.gameObject, RecordXml); // view all NodeTree of GameObject
                    Debug.Log(gameObj.name);
                }
                
            }
            RecordXml.WriteEndElement();
        }

        if (Time.time > 20)
        {
            RecordXml.WriteFullEndElement();
            RecordXml.Close();
            enabled = false; // will not call Update function anymore!
            //Time.timeScale = 0;
            //Application.Quit();
        }
    }
}
