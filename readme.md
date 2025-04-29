# BioSafe - Sistema IoT de Monitoramento

BioSafe é um sistema IoT para monitoramento baseado em MQTT utilizando Mosquitto como broker e Node-RED para processamento e visualização de dados.

## Arquitetura do Sistema

O sistema é composto por:
- **Mosquitto**: Broker MQTT para comunicação entre dispositivos
- **Node-RED**: Processamento de fluxos de dados e interface dashboard
- **MQTTX Web**: Cliente MQTT baseado em navegador para testes e depuração

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

## Acesso aos Serviços

| Serviço | URL | Descrição |
|---------|-----|-----------|
| Node-RED | http://localhost:1880 | Interface de programação por fluxos |
| Node-RED Dashboard | http://localhost:1880/ui | Painel de visualização e controle |
| Mosquitto | host -> mosquitto; port -> 1883 | Para conectar usando o nodered |
| MQTTX Web | http://localhost:80 | Cliente MQTT para testes e depuração |

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

## MQTTX Web (apenas para desenvolvimento)

O MQTTX Web é uma interface gráfica que permite:
- Conectar ao broker Mosquitto
- Publicar mensagens em tópicos MQTT
- Assinar tópicos para visualizar mensagens em tempo real
- Testar a comunicação com dispositivos e o Node-RED

### Configuração de Conexão no MQTTX
1. Acesse http://localhost:80
2. Clique em "New Connection"
3. Configure:
   - Name: BioSafe Local
   - Host: mosquitto (ou localhost)
   - Port: 1883 (MQTT) ou 9001 (WebSockets)
   - Username/Password: Credenciais criadas no Mosquitto
   - Para WebSockets, selecione "WebSockets" no campo "Connection Type"

## Node-RED

### Fluxos Principais
Os fluxos do Node-RED estão configurados em `/nodered_data/flows.json`.

### Dashboard
O Node-RED Dashboard está disponível em: `http://localhost:1880/ui`

#### Telas do Dashboard
1. **Monitoramento em Tempo Real**
   - Gráficos de temperatura e umidade
   - Indicadores de status dos sensores
   
2. **Controle de Dispositivos**
   - Botões para ativar/desativar dispositivos
   - Controles de ajuste para parâmetros
   
3. **Configurações e Alertas**
   - Configuração de limiares para alertas
   - Histórico de notificações

### Endpoints da API REST
O Node-RED também expõe os seguintes endpoints HTTP:

| Método | Endpoint | Descrição |
|--------|----------|-----------|
| GET | `/api/status` | Retorna o status geral do sistema |
| GET | `/api/leituras/recentes` | Últimas leituras de sensores |
| POST | `/api/comandos` | Envia comandos para dispositivos |
| GET | `/api/alertas` | Lista os alertas recentes |

## Armazenamento de Dados

- **Persistência MQTT**: Os dados persistidos pelo Mosquitto são armazenados em `/mosquitto/data/mosquitto.db`
- **Dados do Node-RED**: Configurações e estados são armazenados em `/nodered_data/`
- **Fluxos do Node-RED**: Os fluxos são armazenados em `/nodered_data/flows.json`
- **Credenciais**: Armazenadas de forma segura em `/nodered_data/flows_cred.json`

## Solução de Problemas

### Logs
Para visualizar logs do Mosquitto:
```bash
docker logs mosquitto
```

Para visualizar logs do Node-RED:
```bash
docker logs node_red
```

Para visualizar logs do MQTTX Web:
```bash
docker logs mqttx
```

### Problemas Comuns

1. **Erro de Autenticação MQTT**
   - Verifique se o usuário foi criado corretamente
   - Confirme as credenciais nos dispositivos clientes
   - Verifique se o arquivo pwfile tem permissões corretas

2. **Dashboard não Exibe Dados**
   - Verifique se os tópicos MQTT estão sendo publicados
   - Confirme a configuração dos nós de dashboard no Node-RED
   - Verifique a conexão entre o Node-RED e o broker Mosquitto

3. **Problemas de Conexão no MQTTX Web**
   - Use o hostname "mosquitto" ao conectar de outros containers
   - Use "localhost" ao acessar de fora dos containers
   - Para conexões WebSocket, certifique-se de usar a porta 9001

## Desenvolvimento e Expansão

Para adicionar novos sensores ou dispositivos ao sistema:
1. Configure o dispositivo para publicar/assinar nos tópicos MQTT apropriados
2. Adicione os nós correspondentes nos fluxos do Node-RED
3. Configure widgets de dashboard conforme necessário

---

Desenvolvido em: Abril de 2025