using SolidEdgeCommunity;
using SolidEdgeCommunity.Extensions; // https://github.com/SolidEdgeCommunity/SolidEdge.Community/wiki/Using-Extension-Methods
using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using SolidEdgeAssembly;
using SolidEdgePart;
using SolidEdgeFramework;

namespace ReportDocumentTree
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            //string fileToOpen = @"C:\Users\wttcpchang\Downloads\3D CAD\03-22811_A~A.ASM";
            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.IndentChars = "    ";
            settings.Encoding = Encoding.UTF8;
            XmlWriter writerXml = XmlWriter.Create("PhyProperty.xml", settings);

            SolidEdgeFramework.Application application = null;
            SolidEdgeFramework.Documents seDocuments = null;
            SolidEdgeAssembly.AssemblyDocument seAssemblyDocument = null;
            SolidEdgePart.PartDocument sePartDocument = null;

            SolidEdgePart.Models models = null; // collection of Methods
            SolidEdgePart.Model model = null; // 3d model

            //The variables for use
            int status;
            double volume, area, mass, density, accuracy, relativeAccuracyAchieved;

            // The matrix data
            Array centerOfGravity = Array.CreateInstance(typeof(double), 3);
            Array centerOfVolume = Array.CreateInstance(typeof(double), 3);
            Array globalMomentsOfInteria = Array.CreateInstance(typeof(double), 6); //Ixx, Iyy, Izz, Ixy, Iyz, Ixz
            Array principalMomentsOfInteria = Array.CreateInstance(typeof(double), 3); //Ixx, Iyy, Izz
            Array principalAxes = Array.CreateInstance(typeof(double), 9);
            Array radiiOfGyration = Array.CreateInstance(typeof(double), 9);

            FileStream ostrm;
            StreamWriter writer;
            TextWriter oldOut = Console.Out;

            try
            {

                ostrm = new FileStream("./SEinfo.txt", FileMode.Create, FileAccess.Write);
                writer = new StreamWriter(ostrm);
                Console.SetOut(writer);
                //OleMessageFilter.Register();
                //application = SolidEdgeUtils.Connect();
                //var assemblyDocument = application.GetActiveDocument<SolidEdgeAssembly.AssemblyDocument>(false);
                Console.WriteLine(args[0]);
                //Write the root element
                writerXml.WriteStartElement("RootAssemblyFile");
                writerXml.WriteAttributeString("File", args[0]);

                ///////// open Solid Edge file in SE
                application = SolidEdgeCommunity.SolidEdgeUtils.Connect(true);
                seDocuments = application.Documents;
                //seAssemblyDocument = (AssemblyDocument)seDocuments.Open(fileToOpen);
                seAssemblyDocument = (AssemblyDocument)seDocuments.Open(args[0].ToString());

                var assemblyDocument = application.GetActiveDocument<SolidEdgeAssembly.AssemblyDocument>(false);
                /////////

                // Optional settings you may tweak for performance improvements. Results may vary.
                application.DelayCompute = true;
                application.DisplayAlerts = false;
                application.Interactive = false;
                application.ScreenUpdating = false;

                if (assemblyDocument != null)
                {
                    var rootItem = new DocumentItem();
                    rootItem.FileName = assemblyDocument.FullName;

                    // Begin the recurisve extraction process.
                    PopulateDocumentItems(assemblyDocument.Occurrences, rootItem);

                    // Write each DocumentItem to console.
                    foreach (var documentItem in rootItem.AllDocumentItems)
                    {
                        String file = documentItem.FileName.ToString();

                        if (file.Substring(file.Length - 3) == "PAR" || file.Substring(file.Length - 3) == "par" || file.Substring(file.Length - 3) == "Par")
                        {
                            sePartDocument = (PartDocument)seDocuments.Open(documentItem.FileName.ToString()); // SE open the tree PAR
                            var seActivePartDocument = application.GetActiveDocument<SolidEdgePart.PartDocument>(false); // connect to the active PAR
                        }
                        Console.WriteLine("==============================================================");
                        Console.WriteLine("connect to active file (within Document tree):");
                        //Console.WriteLine(seActivePartDocument.FullName);
                        Console.WriteLine(documentItem.FileName);

                        //if (application.ActiveDocumentType == DocumentTypeConstants.igPartDocument)
                        if ( file.Substring(file.Length - 3) == "PAR" || file.Substring(file.Length - 3) == "par" || file.Substring(file.Length - 3) == "Par")
                        {
                            writerXml.WriteStartElement("DocumentTree_Part");
                            writerXml.WriteAttributeString("File", documentItem.FileName);

                            Console.WriteLine("-------- ## Physical Properties: ## --------");
                            // get the collection of models
                            models = sePartDocument.Models;

                            // get the model
                            model = models.Item(1);

                            // get the Volume, Area, Mass,....
                            model.GetPhysicalProperties(
                                Status: out status,
                                Density: out density,
                                Accuracy: out accuracy,
                                Volume: out volume,
                                Area: out area,
                                Mass: out mass,
                                CenterOfGravity: ref centerOfGravity,
                                CenterOfVolume: ref centerOfVolume,
                                GlobalMomentsOfInteria: ref globalMomentsOfInteria,
                                PrincipalMomentsOfInteria: ref principalMomentsOfInteria,
                                PrincipalAxes: ref radiiOfGyration,
                                RadiiOfGyration: ref radiiOfGyration,
                                RelativeAccuracyAchieved: out relativeAccuracyAchieved
                                );


                            // do anything what you want....
                            Console.WriteLine("\tVolume, \"" + volume + "\"");
                            Console.WriteLine("\tArea, \"" + area + "\"");
                            Console.WriteLine("\tMass, \"" + mass + "\"");
                            //Console.ReadKey();
                            //writerXml.WriteStartElement("Volume");
                            writerXml.WriteElementString("Volume", volume.ToString());
                            writerXml.WriteElementString("Area", area.ToString());
                            writerXml.WriteElementString("Mass", mass.ToString());
                            // Write results to screen.

                            Console.WriteLine("\tDensity, \"{0}\"", density);
                            Console.WriteLine("\tAccuracy, \"{0}\"", accuracy);

                            writerXml.WriteElementString("Density", density.ToString());
                            writerXml.WriteElementString("Accuracy", accuracy.ToString());


                            // Convert from System.Array to double[].  double[] is easier to work with.
                            double[] m = centerOfGravity.OfType<double>().ToArray();

                            Console.WriteLine("CenterOfGravity:");
                            Console.WriteLine("\tCenterOfGravity_x, \"{0}\"", m[0]);
                            Console.WriteLine("\tCenterOfGravity_y, \"{0}\"", m[1]);
                            Console.WriteLine("\tCenterOfGravity_z, \"{0}\"", m[2]);
                            writerXml.WriteElementString("CenterOfGravity_x", m[0].ToString());
                            writerXml.WriteElementString("CenterOfGravity_y", m[1].ToString());
                            writerXml.WriteElementString("CenterOfGravity_z", m[2].ToString());

                            //Console.WriteLine("\t|{0}, {1}, {2}|", m[0], m[1], m[2]);

                            m = centerOfVolume.OfType<double>().ToArray();

                            Console.WriteLine("CenterOfVolume:");
                            Console.WriteLine("\tCenterOfVolume_x, \"{0}\"", m[0]);
                            Console.WriteLine("\tCenterOfVolume_y, \"{0}\"", m[1]);
                            Console.WriteLine("\tCenterOfVolume_z, \"{0}\"", m[2]);
                            writerXml.WriteElementString("CenterOfVolume_x", m[0].ToString());
                            writerXml.WriteElementString("CenterOfVolume_y", m[1].ToString());
                            writerXml.WriteElementString("CenterOfVolume_z", m[2].ToString());
                            //Console.WriteLine("\t|{0}, {1}, {2}|", m[0], m[1], m[2]);

                            m = globalMomentsOfInteria.OfType<double>().ToArray();

                            Console.WriteLine("GlobalMomentsOfInteria:");
                            Console.WriteLine("\tGlobalMomentsOfInteria_xx, \"{0}\"", m[0]);
                            Console.WriteLine("\tGlobalMomentsOfInteria_yy, \"{0}\"", m[1]);
                            Console.WriteLine("\tGlobalMomentsOfInteria_zz, \"{0}\"", m[2]);
                            Console.WriteLine("\tGlobalMomentsOfInteria_xy, \"{0}\"", m[3]);
                            Console.WriteLine("\tGlobalMomentsOfInteria_xz, \"{0}\"", m[4]);
                            Console.WriteLine("\tGlobalMomentsOfInteria_yz, \"{0}\"", m[5]);
                            writerXml.WriteElementString("GlobalMomentsOfInteria_xx", m[0].ToString());
                            writerXml.WriteElementString("GlobalMomentsOfInteria_yy", m[1].ToString());
                            writerXml.WriteElementString("GlobalMomentsOfInteria_zz", m[2].ToString());
                            writerXml.WriteElementString("GlobalMomentsOfInteria_xy", m[3].ToString());
                            writerXml.WriteElementString("GlobalMomentsOfInteria_xz", m[4].ToString());
                            writerXml.WriteElementString("GlobalMomentsOfInteria_yz", m[5].ToString());
                            //Console.WriteLine("\t|{0}, {1}, {2}|", m[0], m[1], m[2]);
                            //Console.WriteLine("\t|{0}, {1}, {2}|", m[3], m[4], m[5]);

                            m = principalMomentsOfInteria.OfType<double>().ToArray();

                            Console.WriteLine("PrincipalMomentsOfInteria:");
                            Console.WriteLine("\tPrincipalMomentsOfInteria_xx, \"{0}\"", m[0]);
                            Console.WriteLine("\tPrincipalMomentsOfInteria_yy, \"{0}\"", m[1]);
                            Console.WriteLine("\tPrincipalMomentsOfInteria_zz, \"{0}\"", m[2]);
                            writerXml.WriteElementString("PrincipalMomentsOfInteria_xx", m[0].ToString());
                            writerXml.WriteElementString("PrincipalMomentsOfInteria_yy", m[1].ToString());
                            writerXml.WriteElementString("PrincipalMomentsOfInteria_zz", m[2].ToString());
                            //Console.WriteLine("\t|{0}, {1}, {2}|", m[0], m[1], m[2]);

                            m = principalAxes.OfType<double>().ToArray();

                            Console.WriteLine("PrincipalAxes:");
                            Console.WriteLine("\tPrincipalAxes_xx, \"{0}\"", m[0]);
                            Console.WriteLine("\tPrincipalAxes_xy, \"{0}\"", m[1]);
                            Console.WriteLine("\tPrincipalAxes_xz, \"{0}\"", m[2]);

                            Console.WriteLine("\tPrincipalAxes_yx, \"{0}\"", m[3]);
                            Console.WriteLine("\tPrincipalAxes_yy, \"{0}\"", m[4]);
                            Console.WriteLine("\tPrincipalAxes_yz, \"{0}\"", m[5]);

                            Console.WriteLine("\tPrincipalAxes_zx, \"{0}\"", m[6]);
                            Console.WriteLine("\tPrincipalAxes_zy, \"{0}\"", m[7]);
                            Console.WriteLine("\tPrincipalAxes_zz, \"{0}\"", m[8]);

                            writerXml.WriteElementString("PrincipalAxes_xx", m[0].ToString());
                            writerXml.WriteElementString("PrincipalAxes_xy", m[1].ToString());
                            writerXml.WriteElementString("PrincipalAxes_xz", m[2].ToString());
                            writerXml.WriteElementString("PrincipalAxes_yx", m[3].ToString());
                            writerXml.WriteElementString("PrincipalAxes_yy", m[4].ToString());
                            writerXml.WriteElementString("PrincipalAxes_yz", m[5].ToString());
                            writerXml.WriteElementString("PrincipalAxes_zx", m[6].ToString());
                            writerXml.WriteElementString("PrincipalAxes_zy", m[7].ToString());
                            writerXml.WriteElementString("PrincipalAxes_zz", m[8].ToString());
                            //Console.WriteLine("\t|{0}, {1}, {2}|", m[0], m[1], m[2]);
                            //Console.WriteLine("\t|{0}, {1}, {2}|", m[3], m[4], m[5]);
                            //Console.WriteLine("\t|{0}, {1}, {2}|", m[6], m[7], m[8]);

                            m = radiiOfGyration.OfType<double>().ToArray();

                            Console.WriteLine("RadiiOfGyration:");
                            Console.WriteLine("\tRadiiOfGyration_xx, \"{0}\"", m[0]);
                            Console.WriteLine("\tRadiiOfGyration_xy, \"{0}\"", m[1]);
                            Console.WriteLine("\tRadiiOfGyration_xz, \"{0}\"", m[2]);

                            Console.WriteLine("\tRadiiOfGyration_yx, \"{0}\"", m[3]);
                            Console.WriteLine("\tRadiiOfGyration_yy, \"{0}\"", m[4]);
                            Console.WriteLine("\tRadiiOfGyration_yz, \"{0}\"", m[5]);

                            Console.WriteLine("\tRadiiOfGyration_zx, \"{0}\"", m[6]);
                            Console.WriteLine("\tRadiiOfGyration_zy, \"{0}\"", m[7]);
                            Console.WriteLine("\tRadiiOfGyration_zz, \"{0}\"", m[8]);
                            writerXml.WriteElementString("RadiiOfGyration_xx", m[0].ToString());
                            writerXml.WriteElementString("RadiiOfGyration_xy", m[1].ToString());
                            writerXml.WriteElementString("RadiiOfGyration_xz", m[2].ToString());
                            writerXml.WriteElementString("RadiiOfGyration_yx", m[3].ToString());
                            writerXml.WriteElementString("RadiiOfGyration_yy", m[4].ToString());
                            writerXml.WriteElementString("RadiiOfGyration_yz", m[5].ToString());
                            writerXml.WriteElementString("RadiiOfGyration_zx", m[6].ToString());
                            writerXml.WriteElementString("RadiiOfGyration_zy", m[7].ToString());
                            writerXml.WriteElementString("RadiiOfGyration_zz", m[8].ToString());
                            //Console.WriteLine("\t|{0}, {1}, {2}|", m[0], m[1], m[2]);
                            //Console.WriteLine("\t|{0}, {1}, {2}|", m[3], m[4], m[5]);
                            //Console.WriteLine("\t|{0}, {1}, {2}|", m[6], m[7], m[8]);
                            Console.WriteLine();
                            Console.WriteLine("\tRelativeAccuracyAchieved, \"{0}\"", relativeAccuracyAchieved);
                            writerXml.WriteElementString("RelativeAccuracyAchieved", relativeAccuracyAchieved.ToString());
                            Console.WriteLine("\tStatus, \"{0}\"", status);
                            writerXml.WriteElementString("Status", status.ToString());
                            Console.WriteLine();
                            writerXml.WriteEndElement();
                        }
                        //Console.ReadKey();
                    }
                    writerXml.WriteFullEndElement();
                    writerXml.Close();
                    Console.SetOut(oldOut);
                    writer.Close();
                    ostrm.Close();

                    // Demonstration of how to save the BOM to various formats.

                    // Convert the document items to JSON.
                    string json = Newtonsoft.Json.JsonConvert.SerializeObject(rootItem, Newtonsoft.Json.Formatting.Indented);
                }
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.ReadKey();
            }
            finally
            {
                if (application != null)
                {
                    application.DelayCompute = false;
                    application.DisplayAlerts = true;
                    application.Interactive = true;
                    application.ScreenUpdating = true;
                }

                OleMessageFilter.Unregister();
            }
        }

        static void PopulateDocumentItems(SolidEdgeAssembly.Occurrences occurrences, DocumentItem parentItem)
        {
            foreach (SolidEdgeAssembly.Occurrence occurrence in occurrences)
            {
                var occurrenceItem = new DocumentItem(occurrence);

                // Make sure the DocumentItem hasn't already been added.
                if (parentItem.DocumentItems.Contains(occurrenceItem) == false)
                {
                    parentItem.DocumentItems.Add(occurrenceItem);

                    if (occurrence.SubOccurrences != null)
                    {
                        PopulateDocumentItems(occurrence.SubOccurrences, occurrenceItem);
                    }
                }
            }
        }

        static void PopulateDocumentItems(SolidEdgeAssembly.SubOccurrences subOccurrences, DocumentItem parentItem)
        {
            foreach (SolidEdgeAssembly.SubOccurrence subOccurrence in subOccurrences)
            {
                var occurrenceItem = new DocumentItem(subOccurrence);

                // Make sure the DocumentItem hasn't already been added.
                if (parentItem.DocumentItems.Contains(occurrenceItem) == false)
                {
                    parentItem.DocumentItems.Add(occurrenceItem);

                    if (subOccurrence.SubOccurrences != null)
                    {
                        PopulateDocumentItems(subOccurrence.SubOccurrences, occurrenceItem);
                    }
                }
            }
        }
    }
}
