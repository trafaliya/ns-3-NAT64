/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/nat64-helper.h"

#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/ipv4-netfilter.h"

#include "ns3/ip-conntrack-info.h"
#include "ns3/ipv4-conntrack-l3-protocol.h"
#include "ns3/tcp-conntrack-l4-protocol.h"
#include "ns3/udp-conntrack-l4-protocol.h"
#include "ns3/icmpv4-conntrack-l4-protocol.h"

#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv6-header.h"
#include "ns3/ipv6-address.h"
#include "ns3/ipv4-address.h"
#include "ns3/node.h"
#include "ns3/net-device.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/nat64.h"
#include "ns3/ipv6.h"

#include <iomanip>

using namespace ns3;


int 
main ()
{
  bool verbose = true;

  CommandLine cmd;
  cmd.AddValue ("verbose", "Tell application to log if true", verbose);

  

  /* ... */

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}


