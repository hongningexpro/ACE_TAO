// -*- C++ -*-
// $Id$

#include "CoherentWrite_Test_Sender_exec.h"
#include "tao/ORB_Core.h"
#include "ace/Reactor.h"
#include "ace/Log_Msg.h"

namespace CIAO_CoherentWrite_Test_Sender_Impl
{
  //============================================================
  // WriteHandler
  //============================================================
  WriteHandler::WriteHandler (Sender_exec_i &callback)
    : callback_ (callback)
  {
  }

  int
  WriteHandler::handle_exception (ACE_HANDLE)
  {
    this->callback_.start ();
    return 0;
  }

  //============================================================
  // Restarter_exec_i
  //============================================================
  Restarter_exec_i::Restarter_exec_i (Sender_exec_i & callback)
    : callback_ (callback)
  {
  }

  Restarter_exec_i::~Restarter_exec_i (void)
  {
  }

  void
  Restarter_exec_i::restart_write ()
  {
    this->callback_.restart ();
  }

  //============================================================
  // Component Executor Implementation Class: Sender_exec_i
  //============================================================
  Sender_exec_i::Sender_exec_i (void)
    : iterations_ (3),
      run_ (1),
      total_iter (0),
      wh_ (0)
  {
  }

  Sender_exec_i::~Sender_exec_i (void)
  {
  }

  void
  Sender_exec_i::restart (void)
  {
    ++this->run_;
    delete this->wh_;
    ACE_NEW_THROW_EX (this->wh_,
                      WriteHandler (*this),
                      ::CORBA::INTERNAL ());
    this->context_->get_CCM_object()->_get_orb ()->orb_core ()->reactor ()->notify (this->wh_);
  }

  void
  Sender_exec_i::start (void)
  {
    ::CoherentWriteTestConnector::Writer_var writer =
      this->context_->get_connection_info_write_data ();

    CoherentWriteStarter_var starter =
      this->context_->get_connection_start_reader ();
    if (::CORBA::is_nil (starter.in ()) ||
        ::CORBA::is_nil (writer.in ()))
      {
        ACE_ERROR ((LM_ERROR, ACE_TEXT ("ERROR: Unable to start the reader\n")));
        return;
      }

    writer->is_coherent_write (true);
    starter->set_reader_properties (this->iterations_);

    ACE_DEBUG ((LM_DEBUG, "Start run <%d> with <%u> iterations\n",
                          this->run_,
                          this->iterations ()));

    CoherentWriteTestSeq write_many_seq (this->iterations_);
    write_many_seq.length (this->iterations_);
    for (int i = 1; i < this->iterations_ + 1; ++i)
      {
        CoherentWriteTest new_key;
        new_key.symbol = CORBA::string_dup("KEY_1");
        new_key.iteration = ++total_iter;
        write_many_seq[i-1] = new_key;
      }
    writer->write_many (write_many_seq);
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Written <%u> keys uptil now\n"),
                total_iter));
    ACE_OS::sleep (2);
    starter->start_read (this->run_);
  }

  ::CORBA::UShort
  Sender_exec_i::iterations (void)
  {
    return this->iterations_;
  }

  void
  Sender_exec_i::iterations (::CORBA::UShort iterations)
  {
    this->iterations_ = iterations;
  }

  ::CCM_CoherentWriteRestarter_ptr
  Sender_exec_i::get_restart_writer (void)
  {
    return new Restarter_exec_i (*this);
  }

  void
  Sender_exec_i::set_session_context (::Components::SessionContext_ptr ctx)
  {
    this->context_ =
      ::CoherentWrite_Test::CCM_Sender_Context::_narrow (ctx);

    if ( ::CORBA::is_nil (this->context_.in ()))
      {
        throw ::CORBA::INTERNAL ();
      }
  }

  void
  Sender_exec_i::configuration_complete (void)
  {
  }

  void
  Sender_exec_i::ccm_activate (void)
  {
    try
      {
        ACE_NEW_THROW_EX (this->wh_,
                          WriteHandler (*this),
                          ::CORBA::INTERNAL ());
        this->context_->get_CCM_object()->_get_orb ()->orb_core ()->reactor ()->notify (this->wh_);
      }
    catch (const ::CORBA::Exception& ex)
      {
        ex._tao_print_exception ("Exception caught:");
        ACE_ERROR ((LM_ERROR,
          ACE_TEXT ("ERROR: GET_CONNECTION_START_READER : Exception caught\n")));
      }
    catch (...)
      {
        ACE_ERROR ((LM_ERROR,
          ACE_TEXT ("ERROR: GET_CONNECTION_START_READER : Unknown exception caught\n")));
      }
  }

  void
  Sender_exec_i::ccm_passivate (void)
  {
  }

  void
  Sender_exec_i::ccm_remove (void)
  {
  }

  extern "C" SENDER_EXEC_Export ::Components::EnterpriseComponent_ptr
  create_CoherentWrite_Test_Sender_Impl (void)
  {
    ::Components::EnterpriseComponent_ptr retval =
      ::Components::EnterpriseComponent::_nil ();

    ACE_NEW_NORETURN (
      retval,
      Sender_exec_i);

    return retval;
  }
}