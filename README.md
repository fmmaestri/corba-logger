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

O pacote `omniorb-nameserver` já registra um serviço do sistema
(`omniorb-nameserver.service`) que sobe um `omniNames` automaticamente na
porta 2809 sempre que o WSL inicia — não é necessário subir um manualmente
(veja como conferir isso no passo 1 abaixo).

Se não souber a senha do usuário Linux do WSL (comum quando você só usa PIN
pra entrar no Windows), dá pra rodar `sudo` como root sem senha nenhuma,
direto do PowerShell/Prompt do Windows:
`wsl -d Ubuntu -u root -e bash -c "apt update && apt install -y ..."`.

## Como compilar

Na raiz do projeto:

```bash
make
```

Isso gera os stubs/skeletons a partir do `Logger.idl` (`idl/Logger.hh` e
`idl/LoggerSK.cc`) e os executáveis `bin/cliente` e `bin/servidor`. Se mudar
algo e quiser recompilar do zero: `make clean && make`.

## Como executar

### 1. Conferir se o Servidor de Nomes está rodando

```bash
ps aux | grep omniNames
```

Se aparecer uma linha com `omniNames -start -always`, já está tudo certo
(suba direto pro passo 2). Se não aparecer nada, suba um manualmente e
deixe rodando num terminal:

```bash
mkdir -p /tmp/omninames && cd /tmp/omninames
omniNames -start 2809
```

### 2. Rodar o servidor (num terminal, deixe aberto)

```bash
cd corba-logger
./bin/servidor -ORBInitRef NameService=corbaname::localhost:2809
```

Ele publica a referência do Logger no Servidor de Nomes e fica bloqueado
esperando chamadas — é o comportamento esperado.

### 3. Rodar o cliente (em outro terminal)

```bash
cd corba-logger
./bin/cliente -ORBInitRef NameService=corbaname::localhost:2809
```

O cliente envia eventos fictícios de várias severidades (`log()`) e depois
consulta `locate()` para cada severidade — incluindo uma que não foi
enviada, para demonstrar a exceção `Logger::NaoEncontrado`.

### 4. Simulando vários clientes espalhados na rede

O `cliente.cpp` aceita, como argumento extra (depois dos argumentos do
ORB), o endereço fictício que aquele "cliente" vai usar nas chamadas de
`log()` — simulando o diagrama do enunciado (três clientes em máquinas
diferentes). Basta abrir um terminal por cliente e rodar, por exemplo:

```bash
./bin/cliente -ORBInitRef NameService=corbaname::localhost:2809 192.168.1.1:1500
```
```bash
./bin/cliente -ORBInitRef NameService=corbaname::localhost:2809 192.168.1.2:1600
```
```bash
./bin/cliente -ORBInitRef NameService=corbaname::localhost:2809 192.168.1.3:1500
```

Se rodar os três ao mesmo tempo, o terminal do servidor (passo 2) mostra os
eventos dos três clientes chegando intercalados — dá pra ver a concorrência
acontecendo de verdade. Se nenhum endereço for passado, o cliente usa
`192.168.1.1:1500` como padrão.

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
