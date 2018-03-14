# SolidEdge-and-FBX
1. Using Solid Edge SDK (C#)   2. Using Autodesk FBX SDK to get all information of 3D animated model


### Solid Edge SDK
I am using Solid Edge ST10 APP. (2018)  
Visual Studio 2013  
Reference: [Solid Edge ST7 SDK](http://support.industrysoftware.automation.siemens.com/training/se/107/api/webframe.html) and [API doc](https://www.plm.automation.siemens.com/zh_cn/Images/Solid_Edge_API_tcm78-125829.pdf)   
There are full sample in: https://github.com/SolidEdgeCommunity/Samples  
Of cause the program can be written without sample. With C#, the NuGet package: Interop.SolidEdge and SolidEdge.Community should be installed in the project. (In Visual Studio, TOOLS -> NuGet Package Manager.)  

```
The things I found:
It seems that the SDK is mainly used to "quickly get up to speed with automating Solid Edge".  
So the function of SDK is almost the same as Solid Edge APP itself. (juse used in automatically)  
I can not get the information of Solid Edge Assembly-Part file without Solid Edge APP.  

```
`SolidEdge/Program.cs` is used to  
1. auto view the file tree of Assembly(.asm) and  
2. get the physical properties of each Part(.par) within the Assembly(.asm) in order.  
3. output the results into .xml file format  
Mainly reference in: "Samples-master/Assembly/ReportDocumentTree" and "Samples-master/Part/ReportPhysicalProperties".
