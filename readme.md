# BioSafe - Sistema IoT de Monitoramento

BioSafe é um sistema IoT para monitoramento baseado em MQTT utilizando Mosquitto como broker, Node-RED para processamento e visualização de dados, e MySQL para armazenamento persistente de informações.

## Arquitetura do Sistema

### Componentes Principais
- **Mosquitto**: Broker MQTT para comunicação entre dispositivos
- **Node-RED**: Processamento de fluxos de dados e interface dashboard
- **MySQL**: Banco de dados para armazenamento persistente de dados
- **ESP32/ESP8266**: Módulo IoT para controle de acesso com sensor de impressão digital ([Documentação código do esp](./biosafe-esp/README.md))


### Ferramentas de Desenvolvimento e Debug
- **MQTTX Web**: Cliente MQTT baseado em navegador para testes e depuração
- **phpMyAdmin**: Interface web para administração do banco de dados MySQL

## Inicialização do Ambiente

### Iniciar os Serviços
Para iniciar todos os serviços, execute:

```bash
docker-compose up -d
```

### Verificar Status dos Containers
Para verificar se os containers estão em execução:

```bash
docker-compose ps
```

Você pode iniciar serviços específicos se necessário:

```bash
# Apenas serviços essenciais
docker-compose up -d mosquitto node_red mysql
```

## Acesso aos Serviços

### Serviços Principais

| Serviço | URL/Porta | Descrição |
|---------|-----------|-----------|
| Node-RED | http://localhost:1880 | Interface de programação por fluxos |
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

### Criação de Usuários no Mosquitto
Após iniciar o container do Mosquitto, crie novos usuários com o seguinte comando:

```bash
docker exec -it mosquitto mosquitto_passwd -b /mosquitto/config/pwfile <nome_usuario>
```

Para recarregar a configuração após adicionar usuários:

```bash
docker exec -it mosquitto mosquitto_control reload
```

### Conexões com o Broker
- **Dispositivos IoT**: Conectar via MQTT (porta 1883)
- **Aplicações Web**: Conectar via WebSockets (porta 9001) 
- **Autenticação**: Obrigatória para todas as conexões
- **Node-RED**: Configurado para se conectar ao Mosquitto internamente

## Ferramentas de Desenvolvimento

### MQTTX Web
Cliente MQTT baseado em navegador para testar a comunicação MQTT:
- **URL**: http://localhost:80
- **Uso**: Testar publicação e assinatura de tópicos MQTT
- **Conexão**: Host: mosquitto, Porta: 1883 (MQTT) ou 9001 (WebSockets)

### phpMyAdmin
Interface web para gerenciamento do banco de dados:
- **URL**: http://localhost:8081
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
- **Usuário Padrão**: user
- **Senha Padrão**: root

### phpMyAdmin
Interface web para administração do banco de dados:
- **URL**: http://localhost:8081
- **Login**: user / root (ou root / root para acesso administrativo)

### Inicialização do Banco
O script `mysql-init/init.sql` é executado automaticamente durante a primeira inicialização do container MySQL para criar as tabelas e estruturas necessárias.

## Solução de Problemas

### Logs
Para visualizar logs dos serviços:

```bash
# Logs do Mosquitto
docker logs mosquitto

# Logs do Node-RED
docker logs node_red

# Logs do MySQL
docker logs mysql

# Logs das ferramentas de desenvolvimento
docker logs mqttx
docker logs phpmyadmin
```

## Node-RED

### Configuração e Fluxos
- **URL**: http://localhost:1880
- **Workspace**: Os fluxos estão organizados no projeto `biosafe`
- **Arquivos de Fluxo**: Localizados em `./nodered_data/projects/biosafe/flows.json`

### Dashboard
O Node-RED Dashboard está disponível em: `http://localhost:1880/ui`

### Integração com MQTT e MySQL
O Node-RED está configurado para:
- Conectar-se ao broker Mosquitto usando o nome do host interno `mosquitto`
- Armazenar dados recebidos via MQTT no banco de dados MySQL
- Exibir visualizações e controles em tempo real através do dashboard

### Tópicos MQTT Principais
Os seguintes tópicos são utilizados pelo sistema:
- `register/init` - comando para iniciar o cadastro de uma nova biometria no esp
- `register/status` - enviado pelo esp ao concluir o processo de cadastro
- `auth/init` - o esp envia qunado reconhece uma digital
- `auth/status` - nodered envia passando do usuário que está sendo autenticado
- `lock/status` - enviado ao realizar alguma alterção na fechadura para armazenamento de log