
#### Write XML file format in C#
```csharp
using System;
using System.IO;
using System.Xml;

namespace WriteXML
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.CloseOutput = true; // close the underlying stream
            settings.IndentChars = "    ";
            settings.Encoding = Encoding.UTF8;
            XmlWriter writerXml = XmlWriter.Create("PhyProperty.xml", settings);
            try
            {
                writerXml.WriteStartElement("RootAssemblyFile");
                writerXml.WriteAttributeString("File", args[0]);
                // loop here start
                writerXml.WriteStartElement("DocumentTree_Part");
                writerXml.WriteAttributeString("File", documentItem.FileName);
                
                writerXml.WriteElementString("Volume", volume.ToString());
                // loop end
                writerXml.WriteEndElement();
                writerXml.WriteFullEndElement();
                writerXml.Close();
            }
        }
    }
}
                             
                
```
```csharp  
writerXml.WriteElementString("Key", "2"); 
```

`<Key>2</Key> `
  
  
```csharp
writerXml.WriteStartElement("Key");
writerXml.WriteAttributeString("time", "0.1");
writerXml.WriteString("2");
writerXml.WriteEndElement();
```
 
`<Key time="0.1">2</Key>`


#### Using XPath (XQuery) to view and rewrite the XML structure in C#  
For re-structure or find the node of XML data.  
XPath/ XQuery is the good tool for SQL language.  
Here shows the C# example to rewrite the XML from "PART - Tx - KeyTime/Value" to "KeyTime - PART - Tx/Value".  
```csharp
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using System.Xml.Schema;
using System.Xml.XPath;
using System.Xml.Xsl;



namespace XmlQuery
{
    class Program
    {
        static void Main(string[] args)
        {
            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.IndentChars = "    ";
            settings.Encoding = Encoding.UTF8;
            XmlWriter writerXml = XmlWriter.Create("OutputAni.xml", settings);

            writerXml.WriteStartElement("Animation");
            XmlDocument xml = new XmlDocument();
            xml.Load(args[0]);

            List<int> Key = new List<int>();
            XmlNodeList xnList0 = xml.SelectNodes("/Display_Model/Animation/Animation_Stack/AnimLayer/Part/*/Key");
            // * =Tx Ty Tz R..S..

            foreach (XmlNode xn in xnList0)
            {
                int reTime = 0;
                foreach (int i in Key)
                {
                    if (int.Parse(xn.Attributes["Time_ms"].InnerText) == i) { reTime = 1; }
                }
                if (reTime == 0) { Key.Add(int.Parse(xn.Attributes["Time_ms"].InnerText)); }
            }
            Key.Sort(); 
            int nTime = 0;
            foreach (int i in Key)
            {
                //Console.WriteLine("#########=======");
                //Console.WriteLine(i);
                nTime++;
                if (nTime > 1) { writerXml.WriteEndElement(); }
                String time = "/Display_Model/Animation/Animation_Stack/AnimLayer/Part/*/Key[@Time_ms="
                        + i + "]";
                XmlNodeList xnListTime = xml.SelectNodes(time);
                writerXml.WriteStartElement("KeyTime");
                writerXml.WriteAttributeString("ms", i.ToString());
                String PART = "n";
                int nPART = 0;
                foreach (XmlNode xnTime in xnListTime)
                {

                    if (xnTime.ParentNode.ParentNode.Attributes["name"].InnerText != PART)
                    {
                        nPART++;
                        if (nPART > 1) { writerXml.WriteEndElement(); }
                        writerXml.WriteStartElement("Module");
                        writerXml.WriteAttributeString("name", 
                            xnTime.ParentNode.ParentNode.Attributes["name"].InnerText);
                    }
                    PART = xnTime.ParentNode.ParentNode.Attributes["name"].InnerText;
                    writerXml.WriteElementString(xnTime.ParentNode.Name, xnTime.InnerText);
                    //writerXml.WriteEndElement();
                    Console.WriteLine("time = " + xnTime.Attributes["Time_ms"].InnerText + ", " 
                        + xnTime.ParentNode.ParentNode.Attributes["name"].InnerText
                        + ", " + xnTime.ParentNode.Name + " " + xnTime.InnerText);
                }
                //writerXml.WriteEndElement();

                Console.WriteLine("=======");
                writerXml.WriteEndElement();
            }
            writerXml.WriteFullEndElement();
            writerXml.Close();

        }
    }
}   

```
