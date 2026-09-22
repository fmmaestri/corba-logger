#include "LoggerI.h"
#include <iostream>
#include <ctime>

// Função auxiliar só usada dentro deste arquivo (static = escopo local),
// converte o enum de severidade para texto legível, usada tanto para
// imprimir no console quanto para montar a mensagem de erro de locate().
static const char* severidadeParaTexto(Logger::TipoSeveridade s)
{
    switch (s)
    {
        case Logger::DEBUG:    return "DEBUG";
        case Logger::WARNING:  return "WARNING";
        case Logger::ERROR:    return "ERROR";
        case Logger::CRITICAL: return "CRITICAL";
    }
    return "DESCONHECIDA"; // nunca deve ser alcançado, só evita warning do compilador
}

Logger_i::Logger_i()
{
    // Nada a inicializar: o std::map e o std::mutex já começam vazios/
    // destravados sozinhos.
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
    // Converte o timestamp (segundos desde 1970) para uma data legível,
    // sempre em UTC (gmtime), já que "hora" é definida como UTC no IDL.
    std::time_t t = static_cast<std::time_t>(hora);
    char horaTexto[32];
    std::strftime(horaTexto, sizeof(horaTexto), "%Y-%m-%d %H:%M:%S", std::gmtime(&t));

    {
        // Bloco travado só durante a escrita no mapa compartilhado: o
        // lock_guard destrava automaticamente ao sair do escopo (RAII),
        // liberando o mutex antes do cout (que é mais lento) abaixo.
        std::lock_guard<std::mutex> lock(mutex_);
        ultimoEndereco_[severidade] = endereco;
    }

    // Requisito do enunciado: cada chamada de log() é impressa na tela.
    std::cout << "[" << horaTexto << " UTC] "
              << severidadeParaTexto(severidade)
              << " origem=" << endereco
              << " pid=" << pid
              << " msg=\"" << msg << "\""
              << std::endl;
}

char* Logger_i::locate(Logger::TipoSeveridade severidade)
{
    // Aqui o lock cobre a função inteira: não há I/O lento dentro dela.
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = ultimoEndereco_.find(severidade);
    if (it == ultimoEndereco_.end())
    {
        // Nenhum evento com essa severidade ainda: monta e lança a
        // exceção definida no IDL. CORBA::string_dup aloca uma cópia da
        // string, já que o objeto de exceção precisa sobreviver além
        // desta função (até ser serializado e enviado ao cliente).
        Logger::NaoEncontrado ex;
        ex.mensagem = CORBA::string_dup(
            (std::string("Nenhum evento recebido com severidade ")
             + severidadeParaTexto(severidade)).c_str());
        throw ex;
    }

    // O retorno de um "string" IDL precisa ser uma cópia alocada com
    // string_dup: o chamador (runtime do CORBA) assume a posse dela.
    return CORBA::string_dup(it->second.c_str());
}
