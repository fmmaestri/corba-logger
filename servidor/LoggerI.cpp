#include "LoggerI.h"
#include <iostream>
#include <ctime>

static const char* severidadeParaTexto(Logger::TipoSeveridade s)
{
    switch (s)
    {
        case Logger::DEBUG:    return "DEBUG";
        case Logger::WARNING:  return "WARNING";
        case Logger::ERROR:    return "ERROR";
        case Logger::CRITICAL: return "CRITICAL";
    }
    return "DESCONHECIDA";
}

Logger_i::Logger_i()
{
}

Logger_i::~Logger_i()
{
}

void Logger_i::log(
    Logger::TipoSeveridade severidade,
    const char* endereco,
    CORBA::UShort pid,
    CORBA::ULong hora,
    const char* msg)
{
    std::time_t t = static_cast<std::time_t>(hora);
    char horaTexto[32];
    std::strftime(horaTexto, sizeof(horaTexto), "%Y-%m-%d %H:%M:%S", std::gmtime(&t));

    {
        std::lock_guard<std::mutex> lock(mutex_);
        ultimoEndereco_[severidade] = endereco;
    }

    std::cout << "[" << horaTexto << " UTC] "
              << severidadeParaTexto(severidade)
              << " origem=" << endereco
              << " pid=" << pid
              << " msg=\"" << msg << "\""
              << std::endl;
}

char* Logger_i::locate(Logger::TipoSeveridade severidade)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = ultimoEndereco_.find(severidade);
    if (it == ultimoEndereco_.end())
    {
        Logger::NaoEncontrado ex;
        ex.mensagem = CORBA::string_dup(
            (std::string("Nenhum evento recebido com severidade ")
             + severidadeParaTexto(severidade)).c_str());
        throw ex;
    }

    return CORBA::string_dup(it->second.c_str());
}
