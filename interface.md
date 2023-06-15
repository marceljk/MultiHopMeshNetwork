# Interface Specification

A Packet is sent via the mesh network defined in the 'Radiohead' library (http://www.airspayce.com/mikem/arduino/RadioHead/index.html?utm_source=platformio&utm_medium=piohome) by Mike McCauley.

## 1.  Packet Structure

A Packet is composed of the following (similar to MQTT):

- Fixed Header (2 Byte)
    - Control Packet Type (4 Bit, defined below)
    - Control Flags (4 Bit)
    - Packet Length (1 Byte, including header and payload)
- Variable Header
    - depending on Packet Type (defined below)
- Payload 
    - depending on Packet Type (defined below)

### 1.1 Fixed Header

#### 1.1.1 Control Packet Type

| Name       | Value | Direction                       | Description                              | Usage in WaldWW           | Variable Header  | Payload          |
|------------|:-----:|---------------------------------|------------------------------------------|---------------------------|------------------|------------------|
| CONNECT    | 1     | Node -> Gateway                 | Connection Request                       | Initial Connection        | [Required](#L43) | None             |
| CONNACK    | 2     | Gateway -> Node                 | Connection Acknowledgement               | Initial Connection        | [Required](#L48) | None             |
| PUBLISH    | 3     | Node -> Gateway Gateway -> Node | Publish Packet                           | Health-Ping, Measurements | [Required](#L61) | [Required](#L102)|
| PUBACK     | 4     | Node -> Gateway Gateway -> Node | Publish Acknowledgement (QoS Lvl 1 only) | Measurements              | [Required](#L71) | None             |
| SUBSCRIBE  | 8     | Node -> Gateway                 | Subscribe Request                        | None                      | [Required](#L79) | None             |
| SUBACK     | 9     | Gateway -> Node                 | Subscribe Acknowledgement                | None                      | [Required](#L89) | None             |
| DISCONNECT | 14    | Node -> Gateway                 | Disconnect Information                   | After Update                    | None             | None             |

#### 1.1.2 Control Flags

| Bits | Name   | Description                                                                        |
|:----:|--------|------------------------------------------------------------------------------------|
| 1    | DUP    | Duplicate publish control packet. If DUP=1 the client has sent the packet before.  |
| 2    | QOS    | QoS Level. Only Level 0 and 1 supported for now.                                   |
| 1    | RETAIN | Not implemented.                                                                   |

### 1.2 Variable Header

#### 1.2.1 CONNECT

| Bytes | Name             | Description                           |
|:----:|------------------|----------------------------------------|
| 1    | Protocol Version | Protocol Version Number. 0x01 for now. |
| 16   | UUID             | The UUID associated with the ESP32.    |

#### 1.2.2 CONNNACK

| Bytes | Name                | Description                                                              |
|:-----:|---------------------|--------------------------------------------------------------------------|
|   1   | Connect Return code | Return Code answering the connection request. Defined below.             |
|   1   | Network ID          | New ID assigned to the connecting node for communication in the network. |
| 16    | UUID                | The UUID of the Node the networkID has been assigned to                  |

| Value    |     Return Code   Response                                    |     Description                                                                             |
|:--------:|---------------------------------------------------------------|---------------------------------------------------------------------------------------------|
|     0    |     0x00 Connection Accepted                                  |     Connection accepted.                                                                    |
|     1    |     0x01 Connection Refused, unacceptable protocol version    |     The Server does not support the version of the protocol requested by the Client.        |
|     3    |     0x03 Connection Refused, Server unavailable               |     The Network Connection has been made but the server is unavailable.                     |
|     5    |     0x05 Connection Refused, not authorized                   |     The Client is not authorized to connect.                                                |

#### 1.2.3 PUBLISH

The Variable Header contains Topic Name and Packet-ID.

| Bytes | Content              | Description                                                       |
|:-----:|----------------------|-------------------------------------------------------------------|
| 1     | Length of Topic Name | The length of the following topic name in Bytes.                  |
| ?     | Topic Name           | The name of the topic on which to publish the packet (in UTF-8).  |
| 2     | Packet ID           | The unique identifier of the packet.                               |

#### 1.2.4 PUBACK

The Variable Header contains the Packet-ID of the packet being acknowledged. 

| Bytes | Content              | Description                                                       |
|:-----:|----------------------|-------------------------------------------------------------------|
| 2     | Packet ID            | The unique identifier of the approved packet.                     |

#### 1.2.5 SUBSCRIBE

The Variable Header contains Topic Name and Packet-ID.

| Bytes | Content              | Description                                                       |
|:-----:|----------------------|-------------------------------------------------------------------|
| 1     | Length of Topic Name | The length of the following topic name in Bytes.                  |
| ?     | Topic Name           | The name of the topic to subscribe to (in UTF-8).                 |
| 2     | Packet ID            | The unique identifier of the packet.                              |

#### 1.2.6 SUBACK

The Variable Header contains the Packet-ID of the packet being acknowledged. 

| Bytes | Content              | Description                                                       |
|:-----:|----------------------|-------------------------------------------------------------------|
| 2     | Packet ID            | The unique identifier of the approved subscribe-packet.           |

#### 1.2.7 DISCONNECT

| Bytes | Content | Description                           |
|-------|---------|---------------------------------------|
| 16    | UUID    | The UUID of the disconnecting device. |


### 1.3 Payload 

_ONLY FOR THE PUBLISH CONTROL PACKET RELEVANT_

The Payload contains the content of the published message in JSON-format. 



## 2. Topics

The following Topics are supported for now:

```
v1/backend/measurements
```
Payload:
```
{
    "timestamp": !? not yet defined, as timers on the ESP32s would need to be synced somehow !?,
    "sender_uuid": integer,
    "content": {
        "type": string,
        "value": string
    }
}
```

## 3. Security

None

## 4. OTA-Updates


    
