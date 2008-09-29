#include "tools/Config_Handlers/DnC_Dump.h"
#include "tools/Config_Handlers/XML_File_Intf.h"
#include "DAnCE/Logger/Log_Macros.h"

int main (int argc, char * argv[])
{
  try
    {
      if (argc < 1)
        {
          DANCE_ERROR((LM_ERROR, "[%M] Incorrect count of arguments. Path to deployment plan has not been specified.\n"));
          return 1;
        }
      CIAO::Config_Handlers::XML_File_Intf file (argv[1]);
      file.add_search_path ("CIAO_ROOT", "/docs/schema/");
      file.add_search_path ("TAO_ROOT", "/docs/schema/");
      Deployment::DeploymentPlan * plan = file.release_plan();
      Deployment::DnC_Dump::dump (*plan);
      delete plan;
    }
  catch (const CORBA::Exception& ex)
    {
      ex._tao_print_exception ("DAnCE_DeploymentPlanDump::main\n");
      return -1;
    }
  catch (...)
    {
      DANCE_ERROR((LM_ERROR, "[%M] Unexpected exception\n"));
      return 1;
    }

  return 0;
}
