# Documentação de Comunicação - BioSafe IoT

Este documento detalha os métodos de comunicação utilizados na aplicação BioSafe IoT.

## Comunicação MQTT

A comunicação MQTT é utilizada para troca de mensagens entre o ESP32 (dispositivo IoT), o Node-RED (servidor de automação) e o aplicativo móvel.

### Tópicos MQTT

<details>
<summary><b>verify</b> - ESP envia</summary>

#### Descrição
Tópico utilizado pelo ESP para verificar se um usuário está autorizado a acessar o sistema através de impressão digital.

#### Payload
```
Conectado ao MQTT
```
</details>

<details>
<summary><b>register/init</b> - Node-RED envia, ESP escuta</summary>

#### Descrição
Tópico utilizado pelo Node-RED para iniciar o processo de registro de uma nova impressão digital no ESP.

#### Payload
```json
{
  "msg": "init"
}
```
</details>

<details>
<summary><b>register/status</b> - ESP envia, Node-RED escuta</summary>

#### Descrição
Tópico utilizado pelo ESP para informar o status do processo de registro de impressão digital.

#### Payload
```json
{
  "status": "success",
  "id": 1
}
```
- `status`: Status final da operação ("error", "success")
- `id`: id da da digital armazenada (id esse enviado quando autenticada)
</details>

<details>
<summary><b>auth/init</b> - ESP envia, Node-RED recebe</summary>

#### Descrição
Tópico utilizado pelo ESP para iniciar o processo de autenticação de um usuário.

#### Payload
```json
{
  "status": "success",
  "id": 1
}
```
- `status`: Status final da operação ("error", "success")
- `id`: id da da digital armazenada (id informado o `register/status`)
</details>

<details>
<summary><b>auth/status</b> - Node-RED envia, ESP escuta</summary>

#### Descrição
Tópico utilizado pelo Node-RED para informar o resultado do processo de autenticação ao ESP enviando dados do user.

#### Payload
```json
{
  "status": "true",
  "userId": "1",
  "username": "Eduardo"
}
```
- `status`: se a digital foi reconhecida ou não
- `userId`: ID do usuário associado à impressão digital
- `username`: Nome do usuário associado aquela digital
</details>

<details>
<summary><b>lock/status</b> - ESP envia, Node-RED e App recebem</summary>

#### Descrição
Tópico utilizado pelo ESP para informar o status atual da fechadura.

#### Payload
```json
{
  "status": "success",
  "state": "locked",
  "userId": 0,
  "timestamp": 65813
}
```
- `status`: Indica se operação de abertura ou fechamento foi concluída
- `state`: Qual o etado final da fechadura ("locked", "unlocked")
- `userId`: ID do usuário que realizou a última alteração
> - no fechamento é enviado o id 0
- `timestamp`: Data e hora da atualização de status
</details>

<details>
<summary><b>lock/state/open</b> - App envia, ESP recebe</summary>

#### Descrição
Tópico utilizado pelo aplicativo para solicitar a abertura da fechadura.

#### Payload
```json
{
  "msg": "open",
}
```
</details>

<details>
<summary><b>lock/state/closed</b> - App envia, ESP recebe</summary>

#### Descrição
Tópico utilizado pelo aplicativo para solicitar o fechamento da fechadura.

#### Payload
```json
{
  "msg": "closed",
}
```
</details>

## Comunicação HTTP

A comunicação HTTP é utilizada pelo aplicativo móvel para interagir com o servidor backend para gerenciamento de usuários e consulta de status.

### Rotas HTTP

<details>
<summary><b>GET /users</b> - Utilizada pelo App</summary>

#### Descrição
Rota para listar todos os usuários cadastrados no sistema.

#### Resposta
```json
[
    {
        "id": 1,
        "name": "Eduardo",
        "fingerprint_id": 1,
        "created_at": "2025-05-26T04:42:53.000Z",
        "last_access": "2025-06-04T23:43:29.000Z"
    },
    {
        "id": 2,
        "name": "Natan",
        "fingerprint_id": 2,
        "created_at": "2025-05-26T22:06:26.000Z",
        "last_access": "2025-05-26T22:06:45.000Z"
    },
]
```
</details>

<details>
<summary><b>POST /users</b> - Utilizada pelo App</summary>

#### Descrição
Rota para cadastrar um novo usuário no sistema.

#### Body da Requisição
```json
{
  "name": "Pedro Santos"
}
```

#### Resposta
- **Sucesso** - 200 (OK)
- **Falha** - 400 (Bad Request)
</details>

<details>
<summary><b>GET /lock/last</b> - Utilizada pelo App</summary>

#### Descrição
Rota para obter informações sobre o último estado da fechadura e eventos relacionados.

#### Resposta
```json
[
    {
        "last_update": "2025-06-04T23:43:29.000Z",
        "current_state": "locked",
    },
    {
        "last_update": "2025-06-04T23:43:04.000Z",
        "current_state": "unlocked",
        "user": {
            "id": 1,
            "name": "Eduardo",
            "fingerprint_id": 1,
            "created_at": "2025-05-26T04:42:53.000Z",
            "last_access": "2025-06-04T23:43:04.000Z"
        }
    },
]
```
</details>