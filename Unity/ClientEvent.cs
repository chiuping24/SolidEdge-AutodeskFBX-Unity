using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
using System;
using UnityEngine.UI;
using UnityEngine.Events;
using UnityEngine.XR;
using Vuforia;


public class ClientEvent : MonoBehaviour {
    public static Animator anim;
    public GameObject Canvas;
    public GameObject CanvasE;
    //public Text texting;
    int port = 7777;
    string ip = "localhost";

    // The id we use to identify our messages and register the handler
    public static short messageID = 1000;

    // The network client
    //public static is added for DefaultTrackableEventHandler tracking to send message to server!
    public static NetworkClient client;

    void CreateClient()
    {
        var config = new ConnectionConfig();

        // Config the Channels we will use
        config.AddChannel(QosType.ReliableFragmented);
        config.AddChannel(QosType.UnreliableFragmented);

        // Create the client ant attach the configuration
        client = new NetworkClient();
        client.Configure(config, 1);

        // Register the handlers for the different network messages
        RegisterHandlers();

        // Connect to the server
        client.Connect(ip, port);
    }
    // Register the handlers for the different message types
    void RegisterHandlers()
    {

        // Unity have different Messages types defined in MsgType
        client.RegisterHandler(messageID, OnMessageReceived);
        client.RegisterHandler(MsgType.Connect, OnConnected);
        client.RegisterHandler(MsgType.Disconnect, OnDisconnected);
        //client.RegisterHandler(messageIDspace, onSpace);
    }
    void ShowStart(bool t)
    {
        for (int j = 0; j < CanvasE.transform.childCount; j++)
        {
            if (CanvasE.transform.GetChild(j).name.ToString().IndexOf("Start") != -1)
            {
                CanvasE.transform.GetChild(j).gameObject.SetActive(t);
            }
        }
    }
    void ShowSpeed(bool t)
    {
        for (int j = 0; j < Canvas.transform.childCount; j++)
        {
            if (Canvas.transform.GetChild(j).name.ToString().IndexOf("Button") != -1)
            {
                Canvas.transform.GetChild(j).gameObject.SetActive(t);
            }
        }
    }

    void StartAni()
    {// show the speed up /down button when pause OnStart Button!
        ShowStart(false);
        for (int j = 0; j < Canvas.transform.childCount; j++)
        {
            Canvas.transform.GetChild(j).gameObject.SetActive(true);
        }
        anim = GetComponent<Animator>();
        anim.SetBool("B20410", false);
        anim.SetFloat("AnimSpeed", 1);
    }
    public void OnStart()
    {
        MyNetworkMessage messageEv = new MyNetworkMessage();
        messageEv.message = "On Start!";
        client.Send(messageID, messageEv);
        //anim.SetFloat("AnimSpeed", anim.GetFloat("AnimSpeed") + 1);
    }

    public void SpeedUp()
    {
        //anim.SetFloat("AnimSpeed", anim.GetFloat("AnimSpeed") + 1);
        MyNetworkMessage messageEv = new MyNetworkMessage();
        messageEv.message = "On SpeedUp!";
        client.Send(messageID, messageEv);
    }
    public void SpeedDown()
    {
        //anim.SetFloat("AnimSpeed", anim.GetFloat("AnimSpeed") - 1);
        MyNetworkMessage messageEv = new MyNetworkMessage();
        messageEv.message = "On SpeedDown!";
        client.Send(messageID, messageEv);
    }


    void onSpace()
    {
        Debug.Log("now in onSpace void!");
        MyNetworkMessage messageSpace = new MyNetworkMessage();
        messageSpace.message = "On space!";
        client.Send(messageID, messageSpace);
        //GameObject.Find("Cube").transform.RotateAround(Vector3.zero, Vector3.up, 100 * Time.deltaTime);
    }

    void OnConnected(NetworkMessage message)
    {
        // Do stuff when connected to the server

        MyNetworkMessage messageContainer = new MyNetworkMessage();
        messageContainer.message = "Hello server!";

        // Say hi to the server when connected
        client.Send(messageID, messageContainer);
    }

    void OnDisconnected(NetworkMessage message)
    {
        // Do stuff when disconnected to the server
    }

    // Message received from the server
    void OnMessageReceived(NetworkMessage netMessage)
    {
        // You can send any object that inherence from MessageBase
        // The client and server can be on different projects, as long as the MyNetworkMessage or the class you are using have the same implementation on both projects
        // The first thing we do is deserialize the message to our custom type
        var objectMessage = netMessage.ReadMessage<MyNetworkMessage>();

        Debug.Log("Message received==" + objectMessage.message + "==");
        if (objectMessage.message == "Get someone client : On space!")
        {
            Debug.Log("get broadcast OnSpace");
            GameObject.Find("Cube").transform.RotateAround(Vector3.zero, Vector3.up, 100 * Time.deltaTime);
        }
        if (objectMessage.message == "Get someone client : On SpeedUp!")
        {
            Debug.Log("get broadcast OnSpeedUp");
            anim.SetFloat("AnimSpeed", anim.GetFloat("AnimSpeed") + 1);
        }
        if (objectMessage.message == "Get someone client : On SpeedDown!")
        {
            Debug.Log("get broadcast OnSpeedUp");
            anim.SetFloat("AnimSpeed", anim.GetFloat("AnimSpeed") - 1);
        }
        if ((objectMessage.message == "Get someone client : On Track!" )|| (objectMessage.message == "Get someone client : On Start!"))
        {
            StartAni();
        }
        if (objectMessage.message == "Get someone client : On TrackLost!")
        {
            anim.SetFloat("AnimSpeed", 0);
            ShowStart(true);
            ShowSpeed(false);
        }

        //texting.text = "Message received: " + objectMessage.message;
    }

    NetworkMessage netMessageNN;
    UnityEvent m_MyEvent = new UnityEvent();
    // Use this for initialization
    void Start()
    {
        ShowSpeed(false);
        //texting = GameObject.Find("Canvas/Text").GetComponentInChildren<Text>();
        CreateClient();
        m_MyEvent.AddListener(() => onSpace());



    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Space) && m_MyEvent != null)
        {
            m_MyEvent.Invoke();
        }
    }
}
