#include "LoggerI.h"
#include <omniORB4/CORBA.h>
#include <omniORB4/Naming.hh>
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        // 1. Ativa o Root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
        PortableServer::POAManager_var poaManager = poa->the_POAManager();
        poaManager->activate();

        // 2. Instancia o servant e obtém a referência CORBA
        Logger_i* loggerServant = new Logger_i();
        Logger_var loggerRef = loggerServant->_this();

        // 3. Publica a referência no Servidor de Nomes
        obj = orb->resolve_initial_references("NameService");
        CosNaming::NamingContext_var nameService = CosNaming::NamingContext::_narrow(obj);

        CosNaming::Name name;
        name.length(1);
        name[0].id = CORBA::string_dup("Logger");

        nameService->rebind(name, loggerRef.in());

        std::cout << "Logger registrado no Servidor de Nomes como \"Logger\"." << std::endl;
        std::cout << "Aguardando eventos..." << std::endl;

        // 4. Aguarda requisições
        loggerServant->_remove_ref();
        orb->run();

        // 5. Finalização (não alcançado em condições normais)
        poa->destroy(true, true);
        orb->destroy();
    }
    catch (const CORBA::SystemException& ex)
    {
        std::cerr << "Exceção CORBA (sistema): " << ex._name() << std::endl;
        return 1;
    }
    catch (const CORBA::Exception& ex)
    {
        std::cerr << "Exceção CORBA: " << ex._name() << std::endl;
        return 1;
    }

    return 0;
}
