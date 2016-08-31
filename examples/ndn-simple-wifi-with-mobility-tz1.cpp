/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/ndnSIM/utils/tracers/ndn-l3-rate-tracer.hpp"

#include "ns3/ndnSIM-module.h"

using namespace std;
namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ndn.WifiExample");

const bool debug = true;

//
// DISCLAIMER:  Note that this is an extremely simple example, containing just 2 wifi nodes
// communicating directly over AdHoc channel.
//

// Ptr<ndn::NetDeviceFace>
// MyNetDeviceFaceCallback (Ptr<Node> node, Ptr<ndn::L3Protocol> ndn, Ptr<NetDevice> device)
// {
//   // NS_LOG_DEBUG ("Create custom network device " << node->GetId ());
//   Ptr<ndn::NetDeviceFace> face = CreateObject<ndn::MyNetDeviceFace> (node, device);
//   ndn->AddFace (face);
//   return face;
// }

void printNodes(NodeContainer nodes)
{
	std::cout << "**********************************************" << std::endl;
	  std::cout << "node(0) has " << (*nodes.Get(0)).GetNDevices() << " faces." << std::endl;
	  std::cout << "node(1) has " << (*nodes.Get(1)).GetNDevices() << " faces." << std::endl;
	  std::cout << "node(2) has " << (*nodes.Get(2)).GetNDevices() << " faces." << std::endl;
	  std::cout << "node(3) has " << (*nodes.Get(3)).GetNDevices() << " faces." << std::endl;
	  std::cout << "node(4) has " << (*nodes.Get(4)).GetNDevices() << " faces." << std::endl;
	  std::cout << "**********************************************" << std::endl;

	  std::cout << "**********************************************" << std::endl;
	  std::cout << "node: " << (*nodes.Get(0)).GetId() << " with faces: ";
	  for(uint32_t i = 0; i < (*nodes.Get(0)).GetNDevices(); i++)
	  {
		  std::cout << (*nodes.Get(0)->GetDevice(i)).GetTypeId() << "  ";
	  }
	  std::cout << std::endl;
	  std::cout << "node: " << (*nodes.Get(1)).GetId() << " with faces: ";
	  for(uint32_t i = 0; i < (*nodes.Get(1)).GetNDevices(); i++)
	  {
		  std::cout << (*nodes.Get(0)->GetDevice(i)).GetTypeId() << "  ";
	  }
	  std::cout << std::endl;
	  std::cout << "node: " << (*nodes.Get(2)).GetId() << " with faces: ";
	  for(uint32_t i = 0; i < (*nodes.Get(2)).GetNDevices(); i++)
	  {
		  std::cout << (*nodes.Get(0)->GetDevice(i)).GetTypeId() << "  ";
	  }
	  std::cout << std::endl;
	  std::cout << "node: " << (*nodes.Get(3)).GetId() << " with faces: ";
	  for(uint32_t i = 0; i < (*nodes.Get(3)).GetNDevices(); i++)
	  {
		  std::cout << (*nodes.Get(0)->GetDevice(i)).GetTypeId()  << "  ";
	  }
	  std::cout << std::endl;
	  std::cout << "node: " << (*nodes.Get(4)).GetId() << " with faces: ";
	  for(uint32_t i = 0; i < (*nodes.Get(4)).GetNDevices(); i++)
	  {
		  std::cout << (*nodes.Get(0)->GetDevice(i)).GetTypeId() << "  ";
	  }
	  std::cout << std::endl;
	  std::cout << "**********************************************" << std::endl;
}

void showPosition (Ptr<Node> node, double deltaTime)
{
  uint32_t nodeId = node->GetId ();
  Ptr<MobilityModel> mobModel = node->GetObject<MobilityModel> ();
  Vector3D pos = mobModel->GetPosition ();
  Vector3D speed = mobModel->GetVelocity ();
  std::cout << "At " << Simulator::Now ().GetSeconds () << " node " << nodeId
            << ": Position(" << pos.x << ", " << pos.y << ", " << pos.z
            << ");   Speed(" << speed.x << ", " << speed.y << ", " << speed.z
            << ")" << std::endl;

  Simulator::Schedule (Seconds (deltaTime), &showPosition, node, deltaTime);
}


int
main(int argc, char* argv[])
{

    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    LogComponentEnable("ndn.StrategyChoiceHelper", LOG_LEVEL_DEBUG);
    LogComponentEnable("nfd.StrategyChoiceManager", LOG_LEVEL_DEBUG);
    // LogComponentEnable("nfd.Forwarder", LOG_LEVEL_DEBUG);

  // disable fragmentation
  Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue("2200"));
  Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));
  Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode",
                     StringValue("OfdmRate24Mbps"));

  std::cout.precision (2);
  std::cout.setf (std::ios::fixed);
  int nodeNum = 5;

  double deltaTime = 10;
  std::string traceFile1 = "src/ndnSIM/examples/trace-files/ndn-simple-wifi-tracefile1";
  std::string traceFile2 = "src/ndnSIM/examples/trace-files/ndn-simple-wifi-tracefile2";
  std::string traceFile3 = "src/ndnSIM/examples/trace-files/ndn-simple-wifi-tracefile3";

  std::string currentTraceFile = traceFile3;

  CommandLine cmd;
  cmd.AddValue ("traceFile", "Ns2 movement trace file", currentTraceFile);
  cmd.AddValue ("deltaTime", "time interval (s) between updates (default 100)", deltaTime);
  // apparently that must be provided with same number of nodes as in tracefile
  cmd.AddValue ("nodeNum", "Number of nodes", nodeNum);
  cmd.Parse(argc, argv);

  //////////////////////
  WifiHelper wifi = WifiHelper::Default();
  // wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
  wifi.SetStandard(WIFI_PHY_STANDARD_80211a);
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
                               StringValue("OfdmRate24Mbps"));

  YansWifiChannelHelper wifiChannel; // = YansWifiChannelHelper::Default ();
  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss("ns3::ThreeLogDistancePropagationLossModel");
  wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");

  // YansWifiPhy wifiPhy = YansWifiPhy::Default();
  YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();
  wifiPhyHelper.SetChannel(wifiChannel.Create());
  wifiPhyHelper.Set("TxPowerStart", DoubleValue(5));
  wifiPhyHelper.Set("TxPowerEnd", DoubleValue(5));

  NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default();
  wifiMacHelper.SetType("ns3::AdhocWifiMac");

  Ptr<Node> n0 = CreateObject<Node> ();
  Ptr<Node> n1 = CreateObject<Node> ();
  Ptr<Node> n2 = CreateObject<Node> ();
  Ptr<Node> n3 = CreateObject<Node> ();
  Ptr<Node> n4 = CreateObject<Node> ();

  NodeContainer nodes;
  nodes.Add(n0);
  nodes.Add(n1);
  nodes.Add(n2);
  nodes.Add(n3);
  nodes.Add(n4);

  if(debug) printNodes(nodes);

  ////////////////
  ///////////////
  // 1. Install Wifi
  NetDeviceContainer wifiNetDevices = wifi.Install(wifiPhyHelper, wifiMacHelper, nodes);
  // NetDeviceContainer wifiNetDevices2 = wifi.Install(wifiPhyHelper, wifiMacHelper, nodes);

  // 2. Install Mobility model
  //mobility.Install(nodes);

  	  Ns2MobilityHelper ns2 = Ns2MobilityHelper (currentTraceFile);
  	  ns2.Install ();

  // 3. Install NDN stack
  NS_LOG_INFO("Installing NDN stack");
  ndn::StackHelper ndnHelper;
  // ndnHelper.AddNetDeviceFaceCreateCallback (WifiNetDevice::GetTypeId (), MakeCallback
  // (MyNetDeviceFaceCallback));
  ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000");
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.Install(nodes);

  // Set BestRoute strategy
  ndn::StrategyChoiceHelper::Install(nodes, "/", "/localhost/nfd/strategy/multicast");

  // 4. Set up applications
  NS_LOG_INFO("Installing Applications");

  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  consumerHelper.SetPrefix("/test/prefix");
  consumerHelper.SetAttribute("Frequency", DoubleValue(5.0));
  consumerHelper.Install(nodes.Get(0));

  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetPrefix("/");
  producerHelper.SetAttribute("PayloadSize", StringValue("1200"));
  producerHelper.Install(nodes.Get(4));

  if(debug) printNodes(nodes);

  ndn::L3RateTracer::InstallAll("rate-trace.txt", Seconds(1));

  Simulator::Stop(Seconds(3));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
