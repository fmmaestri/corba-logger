#ifndef LOGGERI_H
#define LOGGERI_H

// Traz os tipos gerados a partir de Logger.idl (Logger::TipoSeveridade,
// Logger::NaoEncontrado, a classe-esqueleto POA_Logger, etc.), além dos
// tipos básicos do CORBA (CORBA::UShort, CORBA::ULong...).
#include "Logger.hh"
#include <map>
#include <string>
#include <mutex>

// Logger_i é o "servant": o objeto C++ que de fato implementa o Logger
// descrito no IDL. Herda de duas classes:
//  - POA_Logger: esqueleto gerado pelo omniidl, com log()/locate() como
//    métodos virtuais puros que precisamos implementar.
//  - PortableServer::RefCountServantBase: classe base padrão do CORBA que
//    adiciona contagem de referências, usada pelo ORB para saber quando
//    é seguro destruir o objeto.
class Logger_i : public POA_Logger,
                  public PortableServer::RefCountServantBase
{
public:
    Logger_i();
    virtual ~Logger_i();

    // Implementações das operações declaradas em Logger.idl. Os tipos dos
    // parâmetros seguem o mapeamento padrão IDL -> C++: string vira
    // const char*, unsigned short/long viram CORBA::UShort/CORBA::ULong.
    void log(
        Logger::TipoSeveridade severidade,
        const char* endereco,
        CORBA::UShort pid,
        CORBA::ULong hora,
        const char* msg
    );

    char* locate(Logger::TipoSeveridade severidade);

private:
    // Guarda o endereço do último evento recebido para cada severidade.
    std::map<Logger::TipoSeveridade, std::string> ultimoEndereco_;

    // Protege ultimoEndereco_ contra acesso concorrente: como o servidor
    // pode receber chamadas de vários clientes ao mesmo tempo, duas
    // chamadas de log() podem tentar escrever no mapa simultaneamente.
    std::mutex mutex_;
};

#endif
