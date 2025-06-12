# BioSafe - Sistema IoT de Monitoramento

BioSafe é um sistema IoT para monitoramento baseado em MQTT utilizando Mosquitto como broker, Node-RED para processamento e visualização de dados, e MySQL para armazenamento persistente de informações, além disso possuímos, um aplicativo para monitoramento e controle.

#### [GitHub do projeto do aplicativo](https://github.com/eduardosdl/biosafe-app)
#### [GitHub do projeto do ESP8266](https://github.com/eduardosdl/biosafe-esp)

## Arquitetura do Sistema

### Componentes Principais
- **Mosquitto**: Broker MQTT para comunicação entre dispositivos
- **Node-RED**: Processamento de fluxos de dados e interface dashboard
- **MySQL**: Banco de dados para armazenamento persistente de dados
- **ESP32/ESP8266**: Módulo IoT para controle de acesso com sensor de impressão digital 

#### **[Docmentação cmunicações MQTT e HTTP](./comunicacao.md)**

### Ferramentas de Desenvolvimento e Debug
- **MQTTX Web**: Cliente MQTT baseado em navegador para testes e depuração
- **phpMyAdmin**: Interface web para administração do banco de dados MySQL

## Inicialização do Ambiente

Siga os passos abaixo para configurar e executar o sistema BioSafe pela primeira vez:

### Passo 1: Configuração das Variáveis de Ambiente

Copie o arquivo `.env.example` e renomeie para `.env`, depois adicione os valores para cada variável:

```bash
cp .env.example .env
```

Edite o arquivo `.env` com suas credenciais:
```env
MYSQL_USER=user
MYSQL_PASSWORD=sua_senha_aqui
MYSQL_ROOT_PASSWORD=sua_senha_root_aqui
MYSQL_DATABASE_NAME=node_do_banco_de_dados_aqui
```

### Passo 2: Configuração do Mosquitto MQTT

Crie um usuário para autenticação no broker MQTT:

```bash
docker compose run --rm mosquitto sh -c "mosquitto_passwd -c /mosquitto/config/pwfile <nome_usuario>"
```

> **Nota**: Substitua `<nome_usuario>` e a senha informada pelos valores desejados.
> Guarde essas credenciais para uso posterior no Node-RED.

### Passo 3: Inicialização dos Containers

Execute o comando para iniciar todos os serviços:

```bash
docker compose up -d
```

Para verificar se todos os containers estão rodando corretamente:

```bash
docker compose ps
```

**Opcional**: Iniciar apenas serviços específicos:
```bash
docker compose up -d mosquitto node_red mysql
```

Para parar os serviços rode:
```bash
docker compsoe down
```

### Passo 4: Configuração do Node-RED

1. Acesse o Node-RED em: http://localhost:1880

2. Instale os módulos necessárias através do menu **Manage palette (Gerenciar paleta)**:
   - `node-red-dashboard`
   - `node-red-node-mysql-dynamic`

3. Importe o fluxo em `./nodered/flows.json`

4. Altere as configurações de conexões nos nós:
   - **Mosquitto**: Use as credenciais criadas no Passo 2
   - **MySQL**: Use as credenciais definidas no arquivo `.env`

### Passo 5: Verificação da Instalação

Após completar os passos anteriores, verifique se todos os serviços estão acessíveis:

- **Node-RED**: http://localhost:1880
- **Dashboard**: http://localhost:1880/ui
- **MQTTX Web**: http://localhost:80
- **phpMyAdmin**: http://localhost:8081

> **Dica**: Se algum serviço não estiver respondendo, verifique os logs com `docker compose logs <nome_do_serviço>`

## Acesso aos Serviços

### Serviços Principais

| Serviço | URL/Porta | Descrição |
|---------|-----------|-----------|
| Node-RED | http://localhost:1880 | Interface de programação por fluxos |
| Node-RED Rest API | http://localhost:1880/**/* | Endpoints para requisições HTTP |
| Node-RED Dashboard | http://localhost:1880/ui | Painel de visualização e controle |
| Mosquitto | TCP: 1883, WebSocket: 9001 | Broker MQTT para comunicação |
| MySQL | localhost:3306 | Banco de dados relacional |

### Ferramentas de Desenvolvimento

| Serviço | URL | Descrição |
|---------|-----|-----------|
| MQTTX Web | http://localhost:80 | Cliente MQTT para testes e depuração |
| phpMyAdmin | http://localhost:8081 | Gerenciamento do banco de dados MySQL |

## Configuração do Mosquitto MQTT

### Portas e Protocolos
- **1883**: MQTT padrão (TCP)
- **9001**: MQTT sobre WebSockets

### Configuração do Broker
O arquivo `mosquitto.conf` contém as seguintes configurações principais:
- Autenticação obrigatória (`allow_anonymous false`)
- Persistência de dados habilitada
- WebSockets habilitado na porta 9001

### Estrutura do Mosquitto
- **Configuração**: `/mosquitto/config/mosquitto.conf`
- **Autenticação**: `/mosquitto/config/pwfile`
- **Dados**: `/mosquitto/data/mosquitto.db`
- **Logs**: `/mosquitto/log/`

### Conexões com o Broker
- **Dispositivos IoT**: Conectar via MQTT (porta 1883)
- **Aplicações Web**: Conectar via WebSockets (porta 9001) 
- **Autenticação**: Obrigatória para todas as conexões
- **Node-RED**: Configurado para se conectar ao Mosquitto internamente

## Ferramentas de Desenvolvimento

### MQTTX Web
Cliente MQTT baseado em navegador para testar a comunicação MQTT:
- **Uso**: Testar publicação e assinatura de tópicos MQTT
- **Conexão**: Host: mosquitto, Porta: 1883 (MQTT) ou 9001 (WebSockets)

### phpMyAdmin
Interface web para gerenciamento do banco de dados:
- **Uso**: Visualização e manipulação da estrutura e dados do banco

## Desenvolvimento e Expansão

Para adicionar novos sensores ou dispositivos ao sistema:
1. Configure o dispositivo para publicar/assinar nos tópicos MQTT apropriados
2. Adicione os nós correspondentes nos fluxos do Node-RED
3. Configure widgets de dashboard conforme necessário
4. Atualize a estrutura do banco de dados se necessário

### Módulo ESP (Dispositivo IoT)
O projeto inclui um módulo IoT baseado em ESP32/ESP8266 para controle de acesso com sensor de impressão digital. Para detalhes sobre a implementação e configuração desse módulo, consulte o README na pasta `biosafe-esp/`.

## MySQL e phpMyAdmin

### Banco de Dados
O sistema utiliza MySQL para armazenamento persistente de dados:
- **Versão**: MySQL 8.0
- **Porta**: 3306
- **Banco Padrão**: digitalStorage

### Inicialização do Banco
O script `mysql-init/init.sql` é executado automaticamente durante a primeira inicialização do container MySQL para criar as tabelas e estruturas necessárias.

## Node-RED

### Configuração e Fluxos
- **URL**: http://localhost:1880
- **Workspace**: Os fluxos estão organizados no projeto `biosafe`
- **Arquivos de Fluxo**: Localizados em `./nodered_data/projects/biosafe/flows.json`

### Integração com MQTT, MySQL e HTTP
O Node-RED está configurado para:
- Conectar-se ao broker Mosquitto usando o nome do host interno `mosquitto`
- Armazenar dados recebidos via MQTT no banco de dados MySQL
- Exibir visualizações e controles em tempo real através do dashboard
- Oferecer uma interface HTTP para busca de dados do MySQL