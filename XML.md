
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


