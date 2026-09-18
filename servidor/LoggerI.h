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
        Logger::Severidade severidade,
        const char* endereco,
        CORBA::UShort pid,
        CORBA::ULong hora,
        const char* msg
    );

    char* locate(Logger::Severidade severidade);

private:
    std::map<Logger::Severidade, std::string> ultimoEndereco_;
    std::mutex mutex_;
};

#endif
