/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef NAT64_HELPER_H
#define NAT64_HELPER_H

#include "ns3/ptr.h"
#include "../model/nat64.h"

namespace ns3 {
//class Nat64;
class Node;
/**
 * \brief Helper class that adds ns3::Ipv4Nat objects
 */
class Nat64Helper
{
public:
  /**
   * \brief Constructor.
   */
  Nat64Helper ();

  /**
   * \brief Construct an Nat64Helper from another previously 
   * initialized instance (Copy Constructor).
   */
  Nat64Helper (const Nat64Helper &);

  /**
   * \param node the node on which the NAT will run
   * \returns a newly-created NAT, aggregated to the node
   *
   * This method installs a NAT object and hooks it to a node.  It
   * assumes that an Internet stack has already been aggregated to the node
   */
  virtual Ptr<Nat64> Install (Ptr<Node> node) const;

private:
  /**
   * \internal
   * \brief Assignment operator declared private and not implemented to disallow
   * assignment and prevent the compiler from happily inserting its own.
   */
  Nat64Helper &operator = (const Nat64Helper &o);
};


}

#endif /* __NAT64_HELPER_H__ */

