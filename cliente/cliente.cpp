#include "Logger.hh"
#include <omniORB4/CORBA.h>
#include <omniORB4/Naming.hh>
#include <iostream>
#include <ctime>
#include <unistd.h>

int main(int argc, char* argv[])
{
    try
    {
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        // 1. Obtém referência para o Servidor de Nomes
        CORBA::Object_var obj = orb->resolve_initial_references("NameService");
        CosNaming::NamingContext_var nameService = CosNaming::NamingContext::_narrow(obj);

        // 2. Resolve o nome "Logger" publicado pelo servidor
        CosNaming::Name name;
        name.length(1);
        name[0].id = CORBA::string_dup("Logger");

        obj = nameService->resolve(name);
        Logger_var logger = Logger::_narrow(obj);

        if (CORBA::is_nil(logger))
        {
            std::cerr << "Não foi possível obter referência para o Logger." << std::endl;
            return 1;
        }

        CORBA::ULong agora = static_cast<CORBA::ULong>(std::time(nullptr));
        CORBA::UShort pid = static_cast<CORBA::UShort>(getpid());

        // Endereço fictício deste cliente: pode ser passado como argumento
        // (depois dos argumentos do ORB), simulando clientes espalhados na
        // rede como no diagrama do enunciado. Se omitido, usa um padrão.
        const char* meuEndereco = (argc > 1) ? argv[1] : "192.168.1.1:1500";

        // 3. Testa log() com dados fictícios, para diferentes severidades
        //    (deliberadamente sem nenhum evento WARNING, para exercitar a
        //    exceção de locate() mais abaixo).
        std::cout << "Enviando eventos ficticios do endereco " << meuEndereco << "..." << std::endl;

        logger->log(Logger::DEBUG,    meuEndereco, pid, agora,     "Conexao estabelecida");
        logger->log(Logger::ERROR,    meuEndereco, pid, agora + 1, "Falha ao acessar recurso remoto");
        logger->log(Logger::CRITICAL, meuEndereco, pid, agora + 2, "Servico indisponivel");
        logger->log(Logger::ERROR,    meuEndereco, pid, agora + 3, "Timeout na requisicao");

        // log() é oneway (assíncrono): aguardamos um instante para dar
        // tempo do servidor processar as chamadas antes de consultar.
        sleep(1);

        // 4. Testa locate() para cada severidade, incluindo o caso sem
        //    eventos recebidos (deve lançar Logger::NaoEncontrado).
        std::cout << "\nConsultando o ultimo endereco por severidade:" << std::endl;

        struct { Logger::Severidade valor; const char* nome; } severidades[] = {
            { Logger::DEBUG,    "DEBUG"    },
            { Logger::WARNING,  "WARNING"  },
            { Logger::ERROR,    "ERROR"    },
            { Logger::CRITICAL, "CRITICAL" },
        };

        for (const auto& s : severidades)
        {
            try
            {
                CORBA::String_var endereco = logger->locate(s.valor);
                std::cout << "  " << s.nome << " -> " << endereco.in() << std::endl;
            }
            catch (const Logger::NaoEncontrado& ex)
            {
                std::cout << "  " << s.nome << " -> excecao NaoEncontrado: "
                          << ex.mensagem.in() << std::endl;
            }
        }

        orb->destroy();
    }
    catch (const CORBA::TRANSIENT&)
    {
        std::cerr << "Nao foi possivel conectar (verifique se o servidor e o omniNames estao em execucao)." << std::endl;
        return 1;
    }
    catch (const CORBA::SystemException& ex)
    {
        std::cerr << "Excecao CORBA (sistema): " << ex._name() << std::endl;
        return 1;
    }
    catch (const CORBA::Exception& ex)
    {
        std::cerr << "Excecao CORBA: " << ex._name() << std::endl;
        return 1;
    }

    return 0;
}
