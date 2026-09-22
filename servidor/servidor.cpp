#include "LoggerI.h"
#include <omniORB4/CORBA.h>
#include <omniORB4/Naming.hh>   // stub do Servidor de Nomes (CosNaming)
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        // Inicializa o ORB, processando argumentos de rede da linha de
        // comando (ex: -ORBInitRef NameService=corbaname::localhost:2809).
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        // 1. Ativa o Root POA (Portable Object Adapter): é o componente
        //    do ORB responsável por expor objetos servidores na rede.
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
        PortableServer::POAManager_var poaManager = poa->the_POAManager();
        poaManager->activate();

        // 2. Instancia o servant (o objeto C++ que implementa a lógica)
        //    e usa _this() para obter a referência CORBA correspondente,
        //    que é o que de fato vamos publicar e que os clientes usam.
        Logger_i* loggerServant = new Logger_i();
        Logger_var loggerRef = loggerServant->_this();

        // 3. Publica a referência no Servidor de Nomes sob o nome
        //    "Logger", para que os clientes consigam encontrá-la sem
        //    precisar conhecer o endereço de rede do servidor.
        obj = orb->resolve_initial_references("NameService");
        CosNaming::NamingContext_var nameService = CosNaming::NamingContext::_narrow(obj);

        CosNaming::Name name;
        name.length(1);
        name[0].id = CORBA::string_dup("Logger");

        // rebind (em vez de bind) sobrescreve o registro sem lançar
        // exceção caso o nome já exista - útil ao reiniciar o servidor
        // durante os testes.
        nameService->rebind(name, loggerRef.in());

        std::cout << "Logger registrado no Servidor de Nomes como \"Logger\"." << std::endl;
        std::cout << "Aguardando eventos..." << std::endl;

        // 4. _remove_ref() solta nossa referência local ao servant (o
        //    POA já mantém a sua própria); orb->run() bloqueia o programa
        //    aqui, processando chamadas remotas até ser encerrado.
        loggerServant->_remove_ref();
        orb->run();

        // 5. Finalização - na prática não é alcançada em condições
        //    normais, já que orb->run() só retorna se alguém chamar
        //    orb->shutdown() em algum outro ponto.
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
