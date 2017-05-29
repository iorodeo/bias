#include <iostream>
#include <vector>
#include "SpinnakerC.h"

int main(int argc, char *argv[])
{
    std::cout << "running: " << __FILE__ << std::endl;

	spinError err = SPINNAKER_ERR_SUCCESS;
	spinSystem hSystem = NULL;

    // Get System instance
	err = spinSystemGetInstance(&hSystem);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
        std::cout << "unable to retrieve system instance: " <<  err << std::endl;
		return err;
	}

	spinInterfaceList hInterfaceList = NULL;
	size_t numInterfaces = 0;

	// Create empty interface list 
	err = spinInterfaceListCreateEmpty(&hInterfaceList);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
        std::cout << "unable to create empty interface list: " <<  err << std::endl;
		return err;
	}

    // Retrieve interfaces from system
	err = spinSystemGetInterfaces(hSystem, hInterfaceList);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
	    std::cout << "unable to retrieve interface list: " <<  err << std::endl;
		return err;
	}

    // Get number of interfaces
	err = spinInterfaceListGetSize(hInterfaceList, &numInterfaces);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
        std::cout << "unable to retrieve number of interfaces: " << err << std::endl;
		return err;
	}

    /////////////////////////////////////////////////////////////////////////////////
    // TODO
    //
    // Need to clear various lists
    //
    /////////////////////////////////////////////////////////////////////////////////

    //Release System instance
	err = spinSystemReleaseInstance(hSystem);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
	    std::cout << "unable to release system instance: " <<  err << std::endl;
		return err;
	}


    return 0;
}
