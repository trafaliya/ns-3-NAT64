/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 Strasbourg University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: David Gross <gdavid.devel@gmail.com>
 */

// Network topology
//
//             STA2
//              |
//              |
//   R1         R2
//   |          |
//   |          |
//   ------------
//           |
//           |
//          STA 1
//
// - Initial configuration :
//         - STA1 default route : R1
//         - R1 static route to STA2 : R2
//         - STA2 default route : R2
// - STA1 send Echo Request to STA2 using its default route to R1
// - R1 receive Echo Request from STA1, and forward it to R2
// - R1 send an ICMPv6 Redirection to STA1 with Target STA2 and Destination R2
// - Next Echo Request from STA1 to STA2 are directly sent to R2

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv6-static-routing-helper.h"
#include "ns3/nat64-helper.h"
#include "ns3/nat64.h"
#include "ns3/ipv6-routing-table-entry.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("mine");

/**
 * \class StackHelper
 * \brief Helper to set or get some IPv6 information about nodes.
 */
class StackHelper
{
public:
  /**
   * \brief Print the routing table.
   * \param n the node
   */
  inline void PrintRoutingTable (Ptr<Node>& n)
  {
    Ptr<Ipv6StaticRouting> routing = 0;
    Ipv6StaticRoutingHelper routingHelper;
    Ptr<Ipv6> ipv6 = n->GetObject<Ipv6> ();
    uint32_t nbRoutes = 0;
    Ipv6RoutingTableEntry route;

    routing = routingHelper.GetStaticRouting (ipv6);

    std::cout << "Routing table of " << n << " : " << std::endl;
    std::cout << "Destination\t\t\t\t" << "Gateway\t\t\t\t\t" << "Interface\t" << "Prefix to use" << std::endl;

    nbRoutes = routing->GetNRoutes ();
    for(uint32_t i = 0; i < nbRoutes; i++)
      {
        route = routing->GetRoute (i);
        std::cout << route.GetDest () << "\t   "
                  << route.GetGateway () << "\t   "
                  << route.GetInterface () << "\t   "
                  << route.GetPrefixToUse () << "\t   "
                  << std::endl;
      }
  }

  /**
   * \brief Add an host route.
   * \param n node 
   * \param dst destination address
   * \param nextHop next hop for destination
   * \param interface output interface
   */
  inline void AddHostRouteTo (Ptr<Node>& n, Ipv6Address dst, uint32_t interface)
  {
    Ptr<Ipv6StaticRouting> routing = 0;
    Ipv6StaticRoutingHelper routingHelper;
    Ptr<Ipv6> ipv6 = n->GetObject<Ipv6> ();

    routing = routingHelper.GetStaticRouting (ipv6);
    routing->AddHostRouteTo (dst, interface);
  }

  inline void AddHostRouteTo (Ptr<Node>& n, Ipv6Address dst, Ipv6Address nextHop, uint32_t interface)
  {
    Ptr<Ipv6StaticRouting> routing = 0;
    Ipv6StaticRoutingHelper routingHelper;
    Ptr<Ipv6> ipv6 = n->GetObject<Ipv6> ();

    routing = routingHelper.GetStaticRouting (ipv6);
    routing->AddHostRouteTo (dst, nextHop, interface);
  }


  inline void AddDefaultRouteTo (Ptr<Node>& n, Ipv6Address nextHop, uint32_t interface)
  {
    Ptr<Ipv6StaticRouting> routing = 0;
    Ipv6StaticRoutingHelper routingHelper;
    Ptr<Ipv6> ipv6 = n->GetObject<Ipv6> ();

    routing = routingHelper.GetStaticRouting (ipv6);
    routing->SetDefaultRoute (nextHop, interface);
  }

  inline void AddNetworkRouteTo (Ptr<Node>& n, Ipv6Address dst, uint32_t interface)
  {
    Ptr<Ipv6StaticRouting> routing = 0;
    Ipv6StaticRoutingHelper routingHelper;
    Ptr<Ipv6> ipv6 = n->GetObject<Ipv6> ();
    static Ipv6Prefix ones((uint8_t)128);
    routing = routingHelper.GetStaticRouting (ipv6);
    routing->AddNetworkRouteTo (dst,ones, interface);
  }
};

int main (int argc, char **argv)
{
#if 0 
  LogComponentEnable ("Icmpv6RedirectExample", LOG_LEVEL_INFO);
  LogComponentEnable ("Icmpv6L4Protocol", LOG_LEVEL_INFO);
  LogComponentEnable ("Ipv6L3Protocol", LOG_LEVEL_ALL);
  LogComponentEnable ("Ipv6StaticRouting", LOG_LEVEL_ALL);
  LogComponentEnable ("Ipv6Interface", LOG_LEVEL_ALL);
  LogComponentEnable ("Icmpv6L4Protocol", LOG_LEVEL_ALL);
  LogComponentEnable ("NdiscCache", LOG_LEVEL_ALL);
#endif

  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  //LogComponentEnable ("Ipv6StaticRouting", LOG_LEVEL_ALL);

  CommandLine cmd;
  cmd.Parse (argc, argv);

  NS_LOG_INFO ("Create nodes.");
  Ptr<Node> r1 = CreateObject<Node> ();
  Ptr<Node> r2 = CreateObject<Node> ();
  Ptr<Node> r3 = CreateObject<Node> ();
  Ptr<Node> r4 = CreateObject<Node> ();
  NodeContainer net1 (r1, r2);
  NodeContainer net2 (r2,r3);
  NodeContainer net4 (r3,r4);
  NodeContainer net3 (r1,r2,r3,r4);


  StackHelper stackHelper;

  InternetStackHelper internetv6;
  internetv6.Install(net3);
  //internetv6.Install(net1);
  //internetv6.Install(net2);



  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer ndc1 = pointToPoint.Install (net1);
  NetDeviceContainer ndc2 = pointToPoint.Install (net2);
  NetDeviceContainer ndc3 = pointToPoint.Install (net4);


  NS_LOG_INFO ("Assign IPv6 Addresses.");
  Ipv6AddressHelper ipv6;

  ipv6.SetBase (Ipv6Address ("2001:1::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer iic1 = ipv6.Assign (ndc1);
  iic1.SetRouter(1, true);
  iic1.SetRouter(0, true);
  Ipv6InterfaceContainer iic2 = ipv6.Assign (ndc2);
  iic2.SetRouter(0, true);
  iic2.SetRouter(1, true);

  //ipv6.SetBase(Ipv6Address ("3001:1::"), Ipv6Prefix(64));
  //Ipv6InterfaceContainer iic3 = ipv6.Assign (ndc3);
  //iic3.SetRouter(1, true);
  //iic3.SetRouter(0, true);
  Ipv4AddressHelper address2;
  address2.SetBase ("203.82.48.0", "255.255.255.0");
  Ipv4InterfaceContainer iic3 = address2.Assign (ndc3);
  //stackHelper.AddHostRouteTo (r1, iic2.GetAddress(1,1), iic1.GetInterfaceIndex (0));
  //stackHelper.AddHostRouteTo (r2, iic2.GetAddress(1,1), iic2.GetInterfaceIndex (0));
  //stackHelper.AddHostRouteTo (r2, iic1.GetAddress(0,1), iic1.GetInterfaceIndex (1));
  //stackHelper.AddHostRouteTo (r3, iic1.GetAddress(0,1), iic2.GetInterfaceIndex (1));
  //stackHelper.AddNetworkRouteTo (r2, Ipv6Address ("3001:1::"), iic2.GetInterfaceIndex (0));
  //stackHelper.AddNetworkRouteTo (r1, Ipv6Address ("3001:1::"), iic1.GetInterfaceIndex (0));
  //stackHelper.AddNetworkRouteTo (r4, Ipv6Address ("2001:1::"), iic3.GetInterfaceIndex (1));
  //stackHelper.AddHostRouteTo (r2, iic3.GetAddress(1,1), iic2.GetAddress(1,1),iic2.GetInterfaceIndex (0));
  //stackHelper.AddHostRouteTo (r1, iic3.GetAddress(1,1), iic1.GetAddress(1,1),iic1.GetInterfaceIndex (0));
  //stackHelper.AddHostRouteTo (r4, iic1.GetAddress(0,1), iic3.GetAddress(0,1),iic3.GetInterfaceIndex (1));

  stackHelper.AddDefaultRouteTo (r1, iic1.GetAddress(1,1),iic1.GetInterfaceIndex (0));
  stackHelper.AddDefaultRouteTo (r2, iic2.GetAddress(1,1),iic2.GetInterfaceIndex (0));

  //uint32_t x=2;
  //stackHelper.AddDefaultRouteTo (r3, iic3.GetAddress(1),iic2.GetInterfaceIndex (1));
  //stackHelper.AddDefaultRouteTo (r4, iic3.GetAddress(0,1),iic3.GetInterfaceIndex (1));

  Nat64Helper natHelper;
  Ptr<Nat64> nat = natHelper.Install (net4.Get (0));
  nat.SetInside (1);
  nat.SetOutside (2);
  nat.AddPortPool(10000,10500);
  //uint16_t cport=nat.GetNewOutsidePort();

  BIB bib(iic1.GetAddress(0,1),9,iic2.GetAddress(1,1),cport)
  nat.AddBIBentry(bib);

  NS_LOG_INFO("here aa"<<iic1.GetAddress(0,1)<<" aa "<<iic1.GetAddress(1,1) << " aa "<< iic2.GetAddress(1,1));
  NS_LOG_INFO("here aa"<<iic1.GetAddress(1,1)<<" aa "<<iic2.GetAddress(0,1) <<" aa "<<iic3.GetAddress(0,1) <<" aa "<<iic3.GetAddress(1,1) );

  Simulator::Schedule (Seconds (0.0), &StackHelper::PrintRoutingTable, &stackHelper, r1);
  Simulator::Schedule (Seconds (0.0), &StackHelper::PrintRoutingTable, &stackHelper, r2);
  Simulator::Schedule (Seconds (0.0), &StackHelper::PrintRoutingTable, &stackHelper, r3);
  Simulator::Schedule (Seconds (0.0), &StackHelper::PrintRoutingTable, &stackHelper, r4);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (net4.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (iic3.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (512));

  ApplicationContainer clientApps = echoClient.Install (net1.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  AsciiTraceHelper ascii;
  //csma.EnableAsciiAll (ascii.CreateFileStream ("icmpv6-redirect.tr"));
  //csma.EnablePcapAll ("icmpv6-redirect", true);

  /* Now, do the actual simulation. */
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}

