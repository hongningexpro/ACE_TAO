// $Id$

// ===========================================================
//
//
// = LIBRARY
//    TAO/examples/Callback_Quoter
//
// = FILENAME
//    Consumer_Input_Handler.cpp
//
// = DESCRIPTION
//    Implementation of the Consumer_Handler class.
//
// = AUTHOR
//    Kirthika Parameswaran <kirthika@cs.wustl.edu>
//
// ===========================================================

#include "Consumer_Handler.h"
#include "ace/Read_Buffer.h"
#include "tao/ORB.h"
#include "ace/Get_Opt.h"
#include "ace/Read_Buffer.h"
#include "ace/OS.h"
#include "ace/Reactor.h"
#include "ace/Event_Handler.h"

Consumer_Handler::Consumer_Handler (void)
  : stock_name_ ("Unknown"),
    threshold_value_ (0),
    server_ (),
    registered_ (0),
    unregistered_ (0),
    ior_ (0),
    shutdown_ (0)
{

}

Consumer_Handler::~Consumer_Handler (void)
{
  // Make sure to cleanup the STDIN handler.

  if (ACE_Event_Handler::remove_stdin_handler
      (TAO_ORB_Core_instance ()->reactor (),
       TAO_ORB_Core_instance ()->thr_mgr ()) == -1)
    ACE_ERROR ((LM_ERROR,
       	       "%p\n",
       	       "remove_stdin_handler"));
}

// Reads the Server factory IOR from a file.

int
Consumer_Handler::read_ior (char *filename)
{
  // Open the file for reading.
  ACE_HANDLE f_handle = ACE_OS::open (filename, 0);

  if (f_handle == ACE_INVALID_HANDLE)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "Unable to open %s for writing: %p\n",
                       filename),
                      -1);

  ACE_Read_Buffer ior_buffer (f_handle);
  char *data = ior_buffer.read ();

  if (data == 0)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "Unable to read ior: %p\n"),
                      -1);

  this->ior_ = ACE_OS::strdup (data);
  ior_buffer.alloc ()->free (data);

  ACE_OS::close (f_handle);

  return 0;
}

// Parses the command line arguments and returns an error status.

int
Consumer_Handler::parse_args (void)
{
  ACE_Get_Opt get_opts (argc_, argv_, "d:f:xk:xs");
  int c;
  int result;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'd':  // debug flag
	TAO_debug_level++; //****
        break;

      case 'k':  // ior provide on command line
        this->ior_ = ACE_OS::strdup (get_opts.optarg);
        break;

      case 'f': // read the IOR from the file.
        result = this->read_ior (get_opts.optarg);
        if (result < 0)
          ACE_ERROR_RETURN ((LM_ERROR,
                             "Unable to read ior from %s : %p\n",
                             get_opts.optarg),
                            -1);
        break;

      case 's': // don't use the naming service
	this->use_naming_service_ = 0;
	break;

      case 'x':
        this->shutdown_ = 1;
        break;

      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s"
                           " [-d]"
                           " [-f ior-file]"
                           " [-k ior]"
                           " [-x]"
			   " [-s]"
                           "\n",
                           this->argv_ [0]),
                          -1);
      }

  // Indicates successful parsing of command line.
  return 0;
}

// this method uses the naming service to obtain the server object refernce.

int
Consumer_Handler::via_naming_service (void)
{
  TAO_TRY
    {
      // Initialization of the naming service.
      if (naming_services_client_.init (orb_.in ()) != 0)
	ACE_ERROR_RETURN ((LM_ERROR,
			   " (%P|%t) Unable to initialize "
			   "the TAO_Naming_Client. \n"),
			  -1);

      CosNaming::Name notifier_ref_name (1);
      notifier_ref_name.length (1);
      notifier_ref_name[0].id = CORBA::string_dup ("Notifier");

      CORBA::Object_var notifier_obj =
	this->naming_services_client_->resolve (notifier_ref_name,
						TAO_TRY_ENV);
      TAO_CHECK_ENV;

      // The CORBA::Object_var object is downcast to Notifier_var using
      // the <_narrow> method.
      this->server_ =
        Notifier::_narrow (notifier_obj.in (),
                           TAO_TRY_ENV);
      TAO_CHECK_ENV;

    }
  TAO_CATCHANY
    {
      TAO_TRY_ENV.print_exception ("Consumer_Handler::via_naming_service\n");
      return -1;
    }
  TAO_ENDTRY;

  return 0;
}

// Init function.
int
Consumer_Handler::init (int argc, char **argv)
{
  this->argc_ = argc;
  this->argv_ = argv;

  // Register our <Input_Handler> to handle STDIN events, which will
  // trigger the <handle_input> method to process these events.

  ACE_NEW_RETURN (consumer_input_handler_,
                  Consumer_Input_Handler (this),
                  -1);

  if (ACE_Event_Handler::register_stdin_handler
      (consumer_input_handler_,
       TAO_ORB_Core_instance ()->reactor (),
       TAO_ORB_Core_instance ()->thr_mgr ()) == -1)
       ACE_ERROR_RETURN ((LM_ERROR,
		       "%p\n",
		       "register_stdin_handler"),
		      -1);

  // Register the signal event handler for ^C
   ACE_NEW_RETURN (consumer_signal_handler_,
                  Consumer_Signal_Handler (this),
                  -1);

  if( this->reactor_used ()->register_handler
      (SIGINT,
       consumer_signal_handler_) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
		       "%p\n",
		       "register_handler for SIGINT"),
		      -1);

  TAO_TRY
    {
      // Retrieve the ORB.
      this->orb_ = CORBA::ORB_init (this->argc_,
                                    this->argv_,
                                    0,
                                    TAO_TRY_ENV);
      TAO_CHECK_ENV;

      // Parse command line and verify parameters.
      if (this->parse_args () == -1)
	ACE_ERROR_RETURN ((LM_ERROR,
			   "parse_args failed\n"),
			   -1);

      // use the naming service.
       if (this->use_naming_service_)
	 return  via_naming_service ();


       if (this->ior_ == 0)
         ACE_ERROR_RETURN ((LM_ERROR,
                            "%s: no ior specified\n",
                            this->argv_[0]),
                           -1);

      CORBA::Object_var server_object =
        this->orb_->string_to_object (this->ior_,
                                      TAO_TRY_ENV);
      TAO_CHECK_ENV;

      if (CORBA::is_nil (server_object.in ()))
        ACE_ERROR_RETURN ((LM_ERROR,
                           "invalid ior <%s>\n",
                           this->ior_),
                          -1);
      // The downcasting from CORBA::Object_var to Notifier_var is
      // done using the <_narrow> method.
      this->server_ = Notifier::_narrow (server_object.in (),
                                         TAO_TRY_ENV);
      TAO_CHECK_ENV;
    }
  TAO_CATCHANY
    {
      TAO_TRY_ENV.print_exception ("Consumer_Handler::init");
      return -1;
    }
  TAO_ENDTRY;

  return 0;
}

int
Consumer_Handler::run (void)
{
  CORBA::Environment TAO_TRY_ENV;

  TAO_TRY
    {
      ACE_NEW_RETURN (this->consumer_servant_,
		      Consumer_i (),
		      -1);
      // Set the orb in the consumer_ object.
      this->consumer_servant_->orb (this->orb_.in ());

      ACE_DEBUG ((LM_DEBUG,
		  " Services provided:\n "
		  " * Registration <type 'r'>\n "
		  " * Unregistration <type 'u'>\n "
		  " * Quit <type 'q'>\n "));

      // Get the consumer stub (i.e consumer object) pointer.
      this->consumer_var_ =
	this->consumer_servant_->_this (TAO_TRY_ENV);
      TAO_CHECK_ENV;

      // Run the ORB.
      this->orb_->run ();
    }
  TAO_CATCHANY
    {
      TAO_TRY_ENV.print_exception ("Consumer_Handler::run ()");
      return -1;
    }
  TAO_ENDTRY;

  return 0;
}

ACE_Reactor *
Consumer_Handler::reactor_used (void) const
{
  //*done* @@ Please check with Pradeep and see how to remove the reliance
  // on <TAO_ORB_Core_instance()>.  This is non-portable and we want
  // to try to use only CORBA-compliant code in our examples.

  // Cant do anything as the reactor is not accessible in any other way.
  return TAO_ORB_Core_instance ()->reactor ();
}
