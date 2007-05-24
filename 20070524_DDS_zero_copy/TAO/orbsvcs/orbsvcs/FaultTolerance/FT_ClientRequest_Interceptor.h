// -*- C++ -*-

//=============================================================================
/**
 *  @file     FT_ClientRequest_Interceptor.h
 *
 *  $Id$
 *
 *  @author  Balachandran Natarajan <bala@dre.vanderbilt.edu>
 */
//=============================================================================
#ifndef TAO_FT_CLIENT_REQUEST_INTERCEPTOR_H
#define TAO_FT_CLIENT_REQUEST_INTERCEPTOR_H
#include /**/ "ace/pre.h"

#include "orbsvcs/FaultTolerance/FT_ClientORB_export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "tao/PI/PI.h"
#include "tao/PortableInterceptorC.h"
#include "tao/LocalObject.h"
#include "tao/TimeBaseC.h"
#include "ace/Singleton.h"

// This is to remove "inherits via dominance" warnings from MSVC.
// MSVC is being a little too paranoid.
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#endif /* _MSC_VER */

ACE_BEGIN_VERSIONED_NAMESPACE_DECL
namespace ACE_Utils
{
  class UUID;
}
ACE_END_VERSIONED_NAMESPACE_DECL

TAO_BEGIN_VERSIONED_NAMESPACE_DECL

namespace TAO
{
  /**
   * @class FT_TSS
   *
   * @brief Request service context generator class for the client.
   *
   * @@TODO Need to see whether we can have seperate resource factory
   * and put this there.  Let us get this going first
   */
  class FT_TSS
  {
  public:
    CORBA::Long retention_id_;

    bool clean_flag_;
  };


  /**
   * @class FT_ClientRequest_Interceptor
   *
   * @brief Interceptors for the client
   *
   *  @@TODO
   */
  class FT_ClientRequest_Interceptor
    : public virtual PortableInterceptor::ClientRequestInterceptor,
      public virtual TAO_Local_RefCounted_Object
  {
  public:

    FT_ClientRequest_Interceptor (void);

    /// Canonical name of the interceptor.
    virtual char * name (void);


    virtual void destroy (void);

    virtual void send_poll (PortableInterceptor::ClientRequestInfo_ptr);

    virtual void send_request (PortableInterceptor::ClientRequestInfo_ptr ri);

    virtual void receive_reply (PortableInterceptor::ClientRequestInfo_ptr ri);

    virtual void receive_other (PortableInterceptor::ClientRequestInfo_ptr);

    virtual void receive_exception (PortableInterceptor::ClientRequestInfo_ptr ri);

  protected:

    ~FT_ClientRequest_Interceptor (void);

  private:

    void group_version_context (PortableInterceptor::ClientRequestInfo_ptr ri,
                                IOP::TaggedComponent* tp);

    void request_service_context (PortableInterceptor::ClientRequestInfo_ptr ri);

    TimeBase::TimeT request_expiration_time (CORBA::Policy *policy);

  private:

    /// Name of the interceptor
    const char *name_;

    /// UUID for this client
    ACE_Utils::UUID *uuid_;

    /// @@ Need a resource factory for this too
    ACE_Lock *lock_;

    /// Retention id generated by the client
    CORBA::Long retention_id_;
  };
}

TAO_END_VERSIONED_NAMESPACE_DECL

#if defined(_MSC_VER)
#pragma warning(pop)
#endif /* _MSC_VER */

#include /**/ "ace/post.h"
#endif /*TAO_FT_CLIENT_REQUEST_INTERCEPTOR_H*/
