/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __NAT64_H__
#define __NAT64_H__


#endif /* __NAT64_H__ */
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Sindhuja Venkatesh
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
 * Authors: Sindhuja Venkatesh <intutivestriker88@gmail.com>
 */
#ifndef IPV4_NAT_H
#define IPV4_NAT_H

#include <stdint.h>
#include <limits.h>
#include <sys/socket.h>
#include "ns3/ptr.h"
#include "ns3/net-device.h"
#include "ns3/packet.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv6-header.h"
#include "ns3/object.h"
#include "ns3/ipv4-netfilter.h"
#include "ns3/ipv4-netfilter-hook.h"
#include "ns3/netfilter-callback-chain.h"

#include "ns3/netfilter-tuple-hash.h"
#include "ns3/netfilter-conntrack-tuple.h"
#include "ns3/netfilter-conntrack-l3-protocol.h"
#include "ns3/netfilter-conntrack-l4-protocol.h"
#include "ns3/ip-conntrack-info.h"
#include "ns3/ipv4.h"
#include "ns3/ipv6.h"


namespace ns3 {

class Packet;
class NetDevice;
class OutputStreamWrapper;

/**
  * \brief Implementation of the Static NAT Rule.
  *
  * This implements the basic static NAT rule structure with some
  * methods to access their attributes.
  */

class Session
{
public:
/**
  *\brief Initialization of the static NAT rule with specific protcol port restricitions.
  *\param localip The ip address of the host on the local network
  *\param locprt The source port number of the host on the local network
  *\param globalip The translated ip address on the global network
  *\param gloprt The translated source port number of the host on the global network
  *\param protocol The protocol used in the connection
  */
  Session();
  Session (Ipv6Address v6ip, uint16_t v6prt,Ipv6Address natv6ip, uint16_t v4prt, Ipv4Address natv4ip, uint16_t assgnprt, Ipv4Address v4ip, uint16_t v4prt1, uint16_t lifetime);

/**
  *\brief This version is used for no port restrictions
  *\param localip The ip address of the host on the local network
  *\param globalip The translated ip address on the global network
  */
  
/**
  *\return The local host Ipv4Address
  */
  Ipv6Address Getv6ip() const;

/**
  *\return The translated global Ipv4Address
  */
  Ipv4Address Getv4ip () const;

  Ipv4Address Getnatv4ip () const;

  Ipv6Address Getnatv6ip () const;

  uint16_t Getassgnprt () const;

/**
  *\return The source port number of the host on the local network
  */
  uint16_t Getv6prt () const;

/**
  *\return The translated source port number of the host on the global network
  */
  uint16_t Getv4prt () const;

/**
  *\return The Protocol the rule is specific to.
  */
  uint16_t Getlifetime () const;

  void Setlifetime(uint16_t);


private:
  Ipv6Address m_v6addr;
  Ipv4Address m_v4addr;
  uint16_t m_v6port;
  uint16_t m_v4port;
  Ipv6Address m_natv6addr;
  Ipv4Address m_natv4addr;
  uint16_t m_assignedport;
  uint16_t m_lifetime;

  // private data member
};

/**
  * \brief Implementation of the Dynamic NAT Tuple.
  *
  * This implements the basic Dynamic NAT tuple entry structure with some
  * methods to access their attributes.
  */

class BIB
{
public:
/**
  *\brief Used to initialize the Dynamic NAT translated tuple entry.
  *\param local The local host ip that is translated
  *\param global The global ip that the host has been translated to
  *\param port The source port that the local host has translated to
  */
  BIB ();
  BIB (Ipv6Address v6ip, uint16_t v6port, Ipv4Address natv4ip, uint16_t natv4port);


/**
  *\return The local host Ipv4Address
  */
  Ipv6Address Getv6Address () const;

/**
  *\return The translated global Ipv4Address
  */
  Ipv4Address Getnatv4Address () const;

/**
  *\return The translated source port number of the host on the global network
  */
  uint16_t Getnatv4Port () const;

  uint16_t Getv6Port () const;

private:
  Ipv6Address m_v6ip;
  Ipv4Address m_natv4ip;
  uint16_t m_v6port;
  uint16_t m_natv4port;
};

/**
  * \brief Implementation of Nat
  *
  * This implements NAT functionality over a Netfilter framework.
  * The NAT is of two major types (static and dynamic).
  */

class Nat64 : public Object
{
public:
  static TypeId GetTypeId (void);

  Nat64 ();

  /**
   * \brief Add rules to the Dynamic NAT Table.
   *
   * \param rule Dynamic NAT rule reference reference to the NAT rule to be added
   *
   * Adds a NAT rule to the lists that have been dedicated for the specific types
   * of rules.
   */

  void AddSessionEntry (const Session& rule);
  void AddBIBentry (const BIB& rule);

  Ipv4Header Convertv6tov4 (Ipv6Header);

  Ipv6Header Convertv4tov6 (Ipv4Header);

  /**
   * \return number of Static NAT rules
   *
   * Returns the number of rules that are currently listed on the list.
   */
  uint32_t GetNSessions (void) const;

  /**
   * \return number of Dynamic NAT Tuples
   *
   * Returns the number of tuples that are currently listed on the list.
   */
  BIB GetDynamicTuple (uint32_t index) const;
  uint32_t GetNDynamicBIBTuples (void) const;

  /**
   * \param index index in table specifying rule to return
   * \return rule at specified index
   *
   * Returns the specific Static NAT rule that is stored on the given index.
   */
  Session GetSession (uint32_t index) const;

  /**
   * \param index index in table specifying rule to return
   * \return rule at specified index
   *
   * Returns the specific Dynamic NAT tuple that is stored on the given index.
   */
  BIB GetDynamicBIBtuple (uint32_t index) const;

  /**
   * \param index index in table specifying rule to remove
   *
   * Removes the Static NAT rule that is stored on the given index.
   */
  void RemoveSession (uint32_t index);

  /**
   * \param index index in table specifying rule to remove
   *
   * Removes the Dynamic NAT rule that is stored on the given index.
   */
  void RemoveBIBtuple (uint32_t index);

  /**
   * \brief Print the NAT translation table
   *
   * \param stream the ostream the NAT table is printed to
   *
   * Prints out the NAT table.
   */
  void PrintTable (Ptr<OutputStreamWrapper> stream) const;

  /**
   * \brief Add the address pool for Dynamic NAT
   *
   * \param Ipv4address the addresses to be added in the Dynamic Nat pool
   * \param Ipv4Mask the mask of the pool of network address given
   */
  void AddAddressPool (Ipv4Address, Ipv4Mask);

  /**
   * \brief Add the port pool for Dynamic NAT
   *
   * \param numbers for the port pool
   * \param port
   */
  void AddPortPool (uint16_t, uint16_t); //port range

  /**
   * \brief Set the inside interface for the node
   *
   * \param interfaceIndex interface index number of the interface on the node
   */
  void SetInside (int32_t interfaceIndex);

  /**
   * \brief Set the outside interfaces for the node
   *
   * \param interfaceIndex interface index number of the interface on the node
   */
  void SetOutside (int32_t interfaceIndex);

  typedef std::list<Session> SessionTable;
  typedef std::list<BIB> BIBTable;


protected:
  // from Object base class
  virtual void NotifyNewAggregate (void);

private:
  //bool m_isConnected;

  Ptr<Ipv4> m_ipv4;
  Ptr<Ipv6> m_ipv6;

  /**
    * \param hook The hook number e.g., NF_INET_PRE_ROUTING
    * \param p Packet that is handed over to the callback chain for this hook
    * \param in NetDevice which received the packet
    * \param out The outgoing NetDevice
    * \param ccb If not NULL, this callback will be invoked once the hook
    * callback chain has finished processing
    *
    * \returns Netfilter verdict for the Packet. e.g., NF_ACCEPT, NF_DROP etc.
    *
    *  This method is invoke to perform NAT of the packet at the NF_INET_PRE_ROUTING stage.
    */

  uint32_t DoNatPreRouting (Hooks_t hookNumber, Ptr<Packet> p,
                            Ptr<NetDevice> in, Ptr<NetDevice> out, ContinueCallback& ccb);

  /**
     * \param hook The hook number e.g., NF_INET_PRE_ROUTING
     * \param p Packet that is handed over to the callback chain for this hook
     * \param in NetDevice which received the packet
     * \param out The outgoing NetDevice
     * \param ccb If not NULL, this callback will be invoked once the hook
     * callback chain has finished processing
     *
     * \returns Netfilter verdict for the Packet. e.g., NF_ACCEPT, NF_DROP etc.
     *
     *  This method is invoke to perform NAT of the packet at the NF_INET_POST_ROUTING stage.
     */

 // uint32_t DoNatPostRouting (Hooks_t hookNumber, Ptr<Packet> p,
                             //Ptr<NetDevice> in, Ptr<NetDevice> out, ContinueCallback& ccb);
  /**
  *\return The Global Pool Ip address
  */
  Ipv4Address GetNatv4Address () const;

  Ipv6Address GetNatv6Address () const;

  /**
  *\return The Global Pool NetMask
  */
  Ipv4Mask GetNatv4Mask () const;

  /**
  *\return The start port of the port pool
  */
  uint16_t GetStartPort () const;

  /**
  *\return The end port of the port pool
  */
  uint16_t GetEndPort () const;

  /**
  *\return The current port value for the dynamic nat translation
  */
  uint16_t GetCurrentPort () const;

  /**
  *\return The a new port to translate to from the port pool
  */
  uint16_t GetNewOutsidePort ();

  SessionTable m_sessiontable;
  BIBTable m_dynamicBIBtable;
  int32_t m_insideInterface;
  int32_t m_outsideInterface;
  Ipv4Address m_natv4ip;
  Ipv6Address m_natv6ip;
  Ipv4Mask m_natv4mask;
  uint16_t m_startport;
  uint16_t m_endport;
  uint16_t m_currentPort;

};

}
#endif

