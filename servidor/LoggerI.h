#ifndef LOGGERI_H
#define LOGGERI_H

#include "Logger.hh"
#include <map>
#include <string>
#include <mutex>

class Logger_i : public POA_Logger,
                  public PortableServer::RefCountServantBase
{
public:
    Logger_i();
    virtual ~Logger_i();

    void log(
        Logger::TipoSeveridade severidade,
        const char* endereco,
        CORBA::UShort pid,
        CORBA::ULong hora,
        const char* msg
    );

    char* locate(Logger::TipoSeveridade severidade);

private:
    std::map<Logger::TipoSeveridade, std::string> ultimoEndereco_;
    std::mutex mutex_;
};

#endif
