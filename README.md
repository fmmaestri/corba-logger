# Logger CORBA (T1)

Sistema cliente/servidor CORBA em que o servidor (`Logger`) mantém o
registro de eventos ocorridos em clientes espalhados na rede, conforme o
enunciado da atividade.

## Sobre a implementação CORBA usada

Os slides da disciplina usam **ACE/TAO** (`tao_idl`, `tao_cosnaming`). Esse
projeto usa **omniORB** em vez disso: os pacotes do TAO foram removidos dos
repositórios do Debian/Ubuntu há alguns anos e compilá-lo do zero é um
processo longo e frágil. O omniORB é outra implementação completa de CORBA
(mesma linguagem IDL, mesmo conceito de Servidor de Nomes via `CosNaming`,
mesma estrutura de diretórios `idl/` / `cliente/` / `servidor/` / `bin/`) —
só mudam o compilador de IDL (`omniidl` em vez de `tao_idl`) e pequenos
detalhes de API interna.

## Estrutura de diretórios

```
corba-logger/
├── idl/
│   └── Logger.idl        # interface Logger (arquivo a ser entregue)
├── cliente/
│   ├── cliente.cpp        # testa toda a interface do Logger
│   └── Makefile
├── servidor/
│   ├── LoggerI.h           # arquivo a ser entregue (LoggerI.cpp)
│   ├── LoggerI.cpp
│   ├── servidor.cpp
│   └── Makefile
├── bin/                    # executáveis gerados (cliente, servidor)
└── Makefile                # orquestra a build completa
```

## Dependências (Ubuntu/Debian, dentro do WSL)

```bash
sudo apt update
sudo apt install -y build-essential omniorb omniidl omniorb-nameserver libomniorb4-dev libcos4-dev
```

## Como compilar

Na raiz do projeto:

```bash
make
```

Isso gera os stubs/skeletons a partir do `Logger.idl` (`idl/Logger.hh` e
`idl/LoggerSK.cc`) e os executáveis `bin/cliente` e `bin/servidor`.

## Como executar

Em um terminal, inicie o Servidor de Nomes (uma vez só; mantenha rodando):

```bash
mkdir -p /tmp/omninames && cd /tmp/omninames
omniNames -start 2809
```

Em outro terminal, exporte a variável que diz a todo processo omniORB onde
está o Servidor de Nomes (ajuste o host se não for local) e inicie o
servidor:

```bash
export OMNIORB_CONFIG=/dev/null
cd corba-logger
./bin/servidor -ORBInitRef NameService=corbaname::localhost:2809
```

Em um terceiro terminal, rode o cliente (mesma referência do NameService):

```bash
cd corba-logger
./bin/cliente -ORBInitRef NameService=corbaname::localhost:2809
```

O cliente envia eventos fictícios de várias severidades (`log()`) e depois
consulta `locate()` para cada severidade — incluindo uma que não foi
enviada, para demonstrar a exceção `Logger::NaoEncontrado`.

### Simplificando os parâmetros do ORB

Para não repetir `-ORBInitRef` toda vez, pode-se criar `/etc/omniORB.cfg`
(ou `~/.omniORB.cfg`) com:

```
InitRef = NameService=corbaname::localhost:2809
```

## Antes de entregar

- Preencha os nomes do grupo no comentário no início de [`idl/Logger.idl`](idl/Logger.idl).
- O enunciado pede o envio de `Logger.idl` e `LoggerI.cpp` (também é
  necessário `LoggerI.h`, incluído junto).
