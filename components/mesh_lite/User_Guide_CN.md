- [English Version](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/User_Guide.md)

# ESP-MESH-LITE

本指南提供有关 Mesh-Lite 协议的介绍。

## 概述

ESP-MESH-LITE 是一套建立在 Wi-Fi 协议之上的网络协议。ESP-MESH-LITE 允许分布在大范围区域内（室内和室外）的大量设备（下文称节点）在同一个 WLAN（无线局域网）中相互连接。ESP-MESH-LITE 与 [ESP-MESH](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html)（又称 ESP-WIFI-MESH）最大的不同是 ESP-MESH-LITE 允许组网内的子设备独立访问外部网络，传输信息对于父节点无感，大大降低了应用层开发难度，ESP-MESH-LITE 具有自组网和自修复的特性，也就是说 Mesh 网络可以自主地构建和维护。

**注意**： 由于所依赖的 IoT-Bridge 组件的某些特性以及 ESP-IDF 的某些限制，组件在编译时将会给当前使用的 ESP-IDF 打上 [patch](https://github.com/espressif/esp-iot-bridge/tree/master/components/iot_bridge/patch)，为了避免对其他项目的影响，最好为 Mesh-Lite 项目单独维护 ESP-IDF。

本 ESP-MESH-LITE 指南分为以下几个部分：

1. [简介](#简介)
2. [ESP-MESH-LITE 概念](#esp-mesh-lite-概念)
3. [建立网络](#建立网络)
4. [管理网络](#管理网络)
5. [数据传输](#数据传输)
6. [性能](#性能)
7. [ESP-MESH-LITE 与 ESP-MESH 差异](#esp-mesh-lite-与-esp-mesh-差异)
8. [ESPNOW 使用指南](#espnow-使用指南)
9. [移植指南](#移植指南)
10. [更多注意事项](#更多注意事项)

## 简介

乐鑫 ESP-Mesh-Lite 方案已经适配乐鑫多种芯片：

| 芯片     |  ESP-IDF Release/v5.0  |  ESP-IDF Release/v5.1  |  ESP-IDF Release/v5.2  |  ESP-IDF Release/v5.3  |   ESP-IDF Release/v5.4  |
| :------- | :--------------------: | :--------------------: | :--------------------: | :--------------------: | :--------------------: |
| ESP32    | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] |
| ESP32-C3 | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] |
| ESP32-S2 | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] |
| ESP32-S3 | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] |
| ESP32-C2 | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] |
| ESP32-C6 |                        | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] | ![alt text][supported] |
| ESP32-C5 |                        |                        |                        |                        | ![alt text][supported] |
| ESP32-C61|                        |                        |                        |                        | ![alt text][supported] |

[supported]: https://img.shields.io/badge/-%E6%94%AF%E6%8C%81-green "supported"

**注意**： 从 ([a27afb1](https://github.com/espressif/esp-mesh-lite/commit/a27afb1aaed3910ae22bb1b46a3c13264f044f8f)) 之后 esp-mesh-lite 只支持 esp-idf/v5.x 版本。

![传统网络架构示意图](https://docs.espressif.com/projects/esp-idf/zh_CN/v4.4.2/esp32/_images/mesh-traditional-network-architecture.png)

<center>传统 Wi-Fi 网络架构</center>

传统基础设施 Wi-Fi 网络是一个“单点对多点”的网络。这种网络架构的中心节点为接入点 (AP)，其他节点 (station) 均与 AP 直接相连。其中，AP 负责各个 station 之间的仲裁和转发，一些 AP 还会通过路由器与外部 IP 网络交换数据。在传统 Wi-Fi 网络架构中：

1. 由于所有 station 均需与 AP 直接相连，不能距离 AP 太远，因此覆盖区域相对有限。
2. 受到 AP 容量的限制，网络中允许的 station 数量相对有限，很容易超载。

![ESP-MESH-LITE 网络架构示意图](https://docs.espressif.com/projects/esp-idf/zh_CN/v4.4.2/esp32/_images/mesh-esp-wifi-mesh-network-architecture.png)

<center>ESP-MESH-LITE 网络架构示意图</center>

ESP-MESH-LITE 与传统 Wi-Fi 网络的不同之处在于：网络中的节点不需要连接到中心节点，而是可以与相邻节点连接。各节点均通过 Wi-Fi 连接的方式负责相邻节点的数据转发。由于无需受限于距离中心节点的位置，ESP-MESH-LITE 网络的覆盖区域更广。类似地，由于不再受限于中心节点的容量限制，ESP-MESH-LITE 允许更多节点接入，也不易于超载。同时，每个节点会由父节点分配得到 IP 地址，故可以像单个设备接入路由器一样访问网络，其父节点对该数据只做网络层的转发，对应用层无感。


## ESP-MESH-LITE 概念

### 术语

| 术语     | 描述                                                         |
| -------- | ------------------------------------------------------------ |
| 节点     | 任何 **属于** 或 **可以成为** ESP-MESH-LITE 网络一部分的设备 |
| 根节点   | 网络顶部的节点                                               |
| 子节点   | 如节点 X 连接至节点 Y，且 X 相较 Y，到根节点的层级更多，则称 X 为 Y 的子节点 |
| 父节点   | 与子节点对应的概念                                           |
| 后代节点 | 任何可以从根节点追溯到的节点                                 |
| 兄弟节点 | 连接至同一个父节点的所有节点                                 |
| 连接     | AP 和 station 之间的传统 Wi-Fi 关联。ESP-MESH-LITE 中的节点使用 station 接口与另一个节点的 SoftAP 接口产生关联，进而形成连接。连接包括 Wi-Fi 网络中的身份验证和关联过程 |
| 上行连接 | 从节点到其父节点的连接                                       |
| 下行连接 | 从父节点到其一个子节点的连接                                 |
| 子网     | 子网指 ESP-MESH-LITE 网络的一部分，包括一个节点及其所有后代节点。因此，根节点的子网包括 ESP-MESH-LITE 网络中的所有节点 |

### 树型拓扑

ESP-MESH-LITE 建立在传统 Wi-Fi 协议之上，可被视为一种将多个独立 Wi-Fi 网络组合为一个单一 WLAN 网络的组网协议。在 Wi-Fi 网络中，station 在任何时候都仅限于与 AP 建立单个连接（上行连接），而 AP 则可以同时被多个 station 连接（下行连接）。然而，ESP-MESH-LITE 网络则允许节点同时充当 station 和 AP。因此，ESP-MESH-LITE 中的节点可以使用 **其 SoftAP 接口建立多个下行连接**，同时使用 **其 station 接口建立一个上行连接**。这将自然产生一个由多层父子结构组成的树型网络拓扑结构。

![ESP-MESH-LITE 树型拓扑图](https://docs.espressif.com/projects/esp-idf/zh_CN/v4.4.2/esp32/_images/mesh-tree-topology.png)

<center>ESP-MESH-LITE 树型拓扑</center>

ESP-MESH-LITE 中每个节点都会获得由父节点所分配得到 IP 地址，其可以直接向外部网络发起通信，并且该通信数据对于其父节点来说无感。

**备注**

ESP-MESH-LITE 网络中的大小（节点总数）取决于网络中允许的最大层级，以及每个节点可以具有的最大下行连接数。因此，这两个变量可用于配置 ESP-MESH-LITE 网络的大小。

>网络中允许的最大层级：1-15
>
>每个节点可以具有的最大下行连接数：1-10

### 节点类型

![ESP-MESH-LITE 节点类型图](https://docs.espressif.com/projects/esp-idf/zh_CN/v4.4.2/esp32/_images/mesh-node-types.png)

<center>ESP-MESH-LITE 节点类型</center>

**根节点**：网络顶部的节点，是 ESP-MESH-LITE 网络和外部 IP 网络之间的唯一接口。根节点直接连接至传统的 Wi-Fi 路由器，在 ESP-MESH-LITE 网络的节点和外部 IP 网络之间起桥接作用。 **正常情况下，ESP-MESH-LITE 网络中只能有一个根节点**，且根节点的上行连接只能是路由器。如上图所示，节点 A 即为该 ESP-MESH-LITE 网络的根节点。

**叶子节点**：不允许拥有任何子节点（即无下行连接）的节点。如果节点处于 ESP-MESH-LITE 网络的最大允许层级，则该节点将成为叶子节点。叶子节点不会再产生下行连接，确保网络层级不会超出限制。由于建立下行连接必须使用 SoftAP 接口，因此一些没有 SoftAP 接口的节点（仅有 station 接口）也将被分配为叶子节点。如上图所示，位于网络最外层的 L/M/N 节点即为叶子节点。

**中间父节点**：既不是属于根节点也不属于叶子节点的节点即为中间父节点。中间父节点必须有且仅有一个上行连接（即一个父节点），但可以具有 0 个或多个下行连接（即 0 个或多个子节点）。因此，中间父节点可以发送和接收自己的数据包，也可以转发其上行和下行连接的数据包。如上图所示，节点 B 到 J 即为中间父节点。 **注意，E/F/G/I/J 等没有下行连接的中间父节点并不等同于叶子节点**，原因在于这些节点仍允许形成下行连接。

**空闲节点**：尚未加入网络的节点即为空闲节点。空闲节点将尝试与中间父节点形成上行连接，或者在有条件的情况下（参见 [自动根节点选择](#自动根节点选择) ）成为一个根节点。如上图所示，K 和 O 节点即为空闲节点。

### 信标帧

ESP-MESH-LITE 中能够形成下行连接的每个节点（即具有 SoftAP 接口）都会定期传输 Wi-Fi 信标帧。节点可以通过信标帧让其他节点检测自己的存在和状态。空闲节点将侦听信标帧以生成一个潜在父节点列表，并与其中一个潜在父节点形成上行连接。ESP-MESH-LITE 使用“供应商信息元素”来存储元数据，例如：

- Mesh-Lite 版本号
- 节点当前所处的层级
- 当前子节点数量
- 可接受的最大下行连接数量

### 首选父节点

当一个空闲节点有多个候选父节点（潜在父节点）时，空闲节点将与其中的 **首选父节点** 形成上行连接。首选父节点基于以下条件确定：

- 候选父节点所处的层级
- 候选父节点的信号强度

在网络中所处层级较浅的候选父节点（包括根节点）将优先成为首选父节点。这有助于在形成上行连接时控制 ESP-MESH-LITE 网络中的总层级使之最小。例如，在位于第二层和第三层的候选父节点间选择时，位于第二层的候选父节点将始终优先成为首选父节点，除非第二层的候选父节点的 RSSI 强度低于预设阈值。

如果同一层上存在多个候选父节点，RSSI 强度最高的将优先成为首选父节点。

![首选父节点选择示意图](https://docs.espressif.com/projects/esp-idf/zh_CN/v4.4.2/esp32/_images/mesh-preferred-parent-node.png)

<center>首选父节点选择

**上图（A 侧）** 展示了空闲节点 G 如何在 B/C/D/E/F 五个候选父节点中选择首选父节点。首先，B/C 节点优于 D/E/F 节点，因为这两个节点所处的层级更浅。其次，C 节点优于 B 节点，因为 C 节与 G 节点距离更近，RSSI 信号强度更优。

**上图（B 侧）** 展示了空闲节点 G 如何在根节点 A 和其他候选父节点中选择首选父节点。此时，根节点 A 处于空闲节点 G 范围之内（即空闲节点 G 接收到的根节点 A 信标帧 RSSI 强度高于预设阈值），由于根节点 A 处于网络中最浅的层，因此将成为首选父节点。

**备注**

> 用户还可以通过 `esp_mesh_lite_set_allowed_level` 和 `esp_mesh_lite_set_disallowed_level` 自行定义选择节点固定哪个层级或者禁用哪个层级（见 [Mesh-Lite API 详解](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/include/esp_mesh_lite_core.h)）。


## 建立网络

### 一般过程

警告

ESP-MESH-LITE 正式开始构建网络前，必须确保网络中所有节点具有相同的配置（见 [`esp_mesh_lite_config_t`](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/include/esp_mesh_lite_core.h)）。每个节点必须配置 **相同 MESH_LITE 网络 ID、最大层级数量和 SoftAP 配置**。

ESP-MESH-LITE 网络将首先选择根节点，然后逐层形成下行连接，直到所有节点均加入网络。网络的布局可能取决于诸如根节点选择、父节点选择和异步上电复位等因素。但简单来说，一个 ESP-MESH-LITE 网络的构建过程可以概括为以下步骤：

![ESP-MESH-LITE 网络构建过程示意图](https://docs.espressif.com/projects/esp-idf/zh_CN/v4.4.2/esp32/_images/mesh-network-building.png)

<center>ESP-MESH-LITE 网络构建过程

#### 1. 根节点选择

根节点直接进行指定（见 [用户指定根节点](#用户指定根节点)）或通过融合的方式决定最终的根节点（见 [自动根节点选择](#自动根节点选择)）。一旦选定，其余节点开始连接根节点并与其形成上行连接。

#### 2. 第二层形成

一旦根节点连接到路由器，根节点范围内的其余节点以及后面上电的相同配置的节点将会扫描到根节点的存在（根节点在没有达到最大连接数量的情况下，若达到最大连接数量，节点将会选择更高层级的节点作为首选父节点），从而形成第二层网络。一旦连接，第二层节点成为中间父节点（假设最大允许层级大于 2 层），并进而形成下一层。如上图所示，节点 B 到节点 D 都在根节点的连接范围内。因此，节点 B 到节点 D 将与根节点形成上行连接，并成为中间父节点。

#### 3. 其余层形成

剩余的空闲节点将与所处范围内的中间父节点连接，并形成新的层。一旦连接，根据网络的最大允许层级，空闲节点成为中间父节点或叶子节点。此后重复该步骤，直到网络中的所有空闲节点均加入网络或达到网络最大允许层级。如上图所示，节点 E/F/G 分别与节点 B/C/D 连接，并成为中间父节点。

#### 4. 限制树深度

为了防止网络超过最大允许层级，最大允许层级上的节点将在完成连接后成为叶子节点。这样一来，其他空闲节点将无法与这些最大允许层上的叶子节点形成连接，因此不会超过最大允许层级。然而，如果空闲节点无法找到其他潜在父节点，则将无限期地保持空闲状态。如上图所示，网络的最大允许层级为四。因此，节点 H 在完成连接后将成为叶子节点，以防止任何下行连接的形成。

### 自动根节点选择

在自动模式下，根节点的选择取决于相对于路由器的信号强度。

ESP-MESH-LITE 默认首先上电的设备作为根节点，若存在多个设备同时上电，每个设备将首先连接至已配置的 Wi-Fi 路由器，之后每个设备将通过向路由器发出竞选广播，其中带有自己的 MAC 地址以及路由器 RSSI 值，**MAC 地址可以表示网络中的唯一节点**，而 **路由器 RSSI 值** 代表相对于路由器的信号强度。

所有连接到同一路由器的 Mesh-Lite 设备（满足同一 Mesh-Lite ID）收到后，如果节点检测到具有更强的路由器 RSSI 的广播，则停止发送，即 **“弃权”**。经过一定的等待时间后（默认 10 s），所有弃权的设备将与路由器断开连接，重新扫描潜在父节点并选出首选父节点形成上行连接（即连接到根节点上作为二级节点，若二级节点连满，相应的会作为三级节点）。

**提示**

- 该方案也同时可解决根节点冲突。若在 Mesh-Lite 组网建立连接成功之后，出现了同一配置的设备连接到了同一路由器上（手动指定或者意外连接），那么也将通过该方式解决根节点冲突
- 若想避免大量设备同时去连接路由器，建议将子节点上电和根节点上电时间错开，待根节点成功上电之后，再大批量上电子节点，这样子节点就能够及时发现根节点并形成连接。

下图展示了在 ESP-MESH-LITE 网络中，根节点的自动选择过程。

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/components/mesh_lite/docs/_static/root_node_election.png" style="zoom: 50%;" />

<center>根节点选举示例


**1.** 节点 A/B/C 同时上电后连接到路由器上，通过竞选，节点 C 具有最强的路由器 RSSI 值 (-10 dB)。

**2.** 节点 A/B 从路由器上断开，重新扫描潜在父节点，节点 C 将成为节点 A/B 的首选父节点，并与之连接。节点 A/B 将形成网络的第二层。

**3.** 节点 D/E 上电后，首先扫描是否存在潜在父节点，节点 C 将成为节点 D/E 的首选父节点（即最浅的节点），并与这些节点连接。节点 A/B/D/E 形成网络的第二层。

**4.** 节点 F 和节点 G 分别连接节点 D 和节点 E，并完成网络构建过程。

### 用户指定根节点

根节点也可以由用户指定，即直接让指定的根节点与路由器连接。当根节点指定后，如果存在同样连接到路由器的节点，就会主动断开并去寻找新的父节点，以防止根节点冲突的发生。下图展示了在 ESP-MESH-LITE 网络中，根节点的手动选择过程。

![根节点指定过程示例图](https://docs.espressif.com/projects/esp-idf/zh_CN/v4.4.2/esp32/_images/mesh-root-node-designated-example.png)

<center>根节点指定示例（根节点 = A，最大层级 = 4)


**1.** 节点 A 是由用户指定的根节点，因此直接与路由器连接。

**2.** 节点 C 和节点 D 将节点 A 选为自己的首选父节点，并与其形成连接。这两个节点将形成网络的第二层。

**3.** 类似地，节点 B 和节点 E 扫描到有合适的父节点节点 C，将与节点 C 连接，节点 F 将与节点 D 连接。这三个节点将形成网络的第三层。

**4.** 节点 G 将与节点 E 连接，形成网络的第四层。然而，由于该网络的最大允许层级已配置为 4，因此节点 G 将成为叶子节点，以防止形成任何新层。

### 选择父节点

默认情况下，ESP-MESH-LITE 具有可以自组网的特点，也就是每个节点都可以自主选择与其形成上行连接的潜在父节点。自主选择出的父节点被称为首选父节点。用于选择首选父节点的标准旨在减少 ESP-MESH-LITE 网络的层级，并使节点具有更优的连接稳定性（参见 [首选父节点](#首选父节点)）。

### 异步上电复位

ESP-MESH-LITE 网络构建可能会受到节点上电顺序的影响。如果网络中的某些节点为异步上电（即相隔几分钟上电），**网络的最终结构可能与所有节点同步上电时的理想情况不同**。延迟上电的节点将遵循以下规则：

**规则 1**：如果网络中已存在根节点，则延迟节点上电后即使自身的路由器 RSSI 更强也不会尝试去连接路由器。相反，延迟节点与任何其他空闲节点无异，将通过与首选父节点连接来加入网络。如果该延迟节点为用户指定的根节点，其会直接连接至路由器，原根节点（非用户指定根节点，仅仅是 Mesh 组网逻辑竞选出的根节点）便会从路由器断开作为新根节点的子节点。

**规则 2**：如果空闲节点被用户指定层级，则该空闲节点在没有找到指定层级上一层的父节点之前不会尝试形成任何上行连接。空闲节点将无限期地保持空闲，直到其寻找到满足自己所指定层级的父节点。

下方示例展示了异步上电对网络构建的影响。

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/components/mesh_lite/docs/_static/Network_Construction_Example.png" style="zoom:60%;" />

<center>网络构建（异步电源）示例


**1.** 节点 A/C/D/F/G/H 同步上电，均连接到路由器上，节点 A 的 RSSI 最强，竞选 A 为根节点，其他节点与路由器断开并开始扫描潜在父节点。

**2.** 一旦节点 A 成为根节点，其余的节点就开始与其首选父节点逐层形成上行连接，并最终形成一个具有五层的网络。

**3.** 节点 B/E 由于存在上电延迟，因此即使路由器 RSSI 比节点 A 更强（-20 dB 和 -10 dB）也不会尝试成为根节点。相反，这两个上电延迟节点均将与对应的首选父节点 A 和 C 形成上行连接。加入网络后，节点 B/E 均将成为中间父节点。

**同步上电**：如果所有节点均同步上电，节点 E （-10 dB）由于路由器 RSSI 最强而成为根节点。此时形成的网络结构将与异步上电的情况截然不同。

### 环路避免、检测和处理

环路是指特定节点与其后代节点（特定节点子网中的节点）形成上行连接的情况。因此产生的循环连接路径将打破 Mesh 网络的树型拓扑结构。ESP-MESH-LITE 内部具有一定的环路检测机制来避免意外情况下节点与其子网中的节点建立上行连接并形成环路。



## 管理网络

**作为一个自修复网络，ESP-MESH-LITE 可以检测并修正 Mesh 网络中的故障**。当具有一个或多个子节点的父节点断开或父节点与其子节点之间的连接不稳定时，会发生故障。ESP-MESH-LITE 中的子节点将会重新寻找更合适的父节点，并与其形成上行连接，以维持网络互连。ESP-MESH-LITE 可以处理根节点故障和中间父节点故障。

### 根节点故障

对于根节点来说，如果其子节点有变动（连接或断开），都将会启动一轮**候选根节点**选举过程。如果根节点断开，则与其连接的节点（第二层节点）将及时检测到该根节点故障。第二层节点将主动尝试与根节点重连。但是在多次尝试失败后，第二层最新的候选根节点将会直接与路由器连接，成为新的根节点，其余第二层节点将与新的根节点（如果不在范围内的话，也可与相邻父节点连接）形成上行连接。

如果根节点和下面多层的节点（例如根节点、第二层节点和第三层节点）同时断开，则位于最浅层的仍在正常工作的节点多次尝试重连失败后，将会重新扫描潜在父节点，若扫描到将选择新的首选父节点形成上行连接，若扫描不到则连接至路由器。

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/components/mesh_lite/docs/_static/mesh-root-node-failure.png" style="zoom:60%;" />

<center>根节点故障的自修复示意


**1.** 节点 C 是网络的根节点。节点 A/B/D/E 是连接到节点 C 的第二层节点，由于节点 B 的 RSSI 最强，被选为候选根节点。

**2.** 节点 C 断开。在多次重连尝试失败后，第二层候选根节点连接至路由器，其他节点开始从原根节点上断开。

**3.** 节点 B 被选为根节点，并开始接受下行连接。剩余的第二层节点 A/D/E 形成与节点 B 的上行连接，因此网络已经恢复，并且可以继续正常运行。

备注

> 如果除了断开的根节点外，其他节点都被设定不允许作为 Level 1，则无法进行自动修复。

### 中间父节点故障

如果中间父节点断开，则与之断开的子节点将主动尝试与该父节点重连。在多次重连尝试失败后，每个子节点开始扫描潜在父节点（请见 [信标帧](#信标帧)）。

如果存在其他可用的潜在父节点，每个子节点将分别给自己选择一个新的首选父节点（请见 [首选父节点](#首选父节点)），并与它形成上行连接。

下方示例展示了网络从中间父节点断开故障中进行自修复。

![中间父节点故障的自修复示意图](https://docs.espressif.com/projects/esp-idf/zh_CN/v4.4.2/esp32/_images/mesh-parent-node-failure.png)

<center>中间父节点故障的自修复


**1.** 网络中存在节点 A 至 G。

**2.** 节点 C 断开。节点 F/G 检测到节点 C 的断开故障，并尝试与节点 C 重新连接。在多次重连尝试失败后，节点 F/G 将开始选择新的首选父节点。

**3.** 节点 G 因其范围内不存在任何父节点而暂时保持空闲。节点 F 的范围中有 B 和 E 两个节点，但节点 B 因为所处层级更浅而当选新的父节点。节点 F 将与节点 B 连接后，并成为一个中间父节点，节点 G 将于节点 F 相连。这样一来，网络已经恢复了，但结构发生了变化（网络层级增加了 1 层）。

备注

> 如果子节点的层级已被指定，则子节点不会将非指定层级的上一级的节点选为潜在父节点。例如 G 被指定只能作为 Level 3，则 G 节点不会尝试与 F 节点连接，此时，G 节点将无限期地保持空闲状态直到发现一个合适的二级节点。

### 根节点切换（自荐）

对于根节点自动切换的方式大体有两种，一种是同时多个设备连接至路由器后触发的**融合**场景（见 [自动根节点选择](#自动根节点选择)）。

另外有一种情况，对于相同 SSID Password 的 Mesh 路由器，如果根节点连接路由器 A，此时整个 Mesh 组网都在路由器 A 下，如果此时路由器 A 意外掉线，则整个网络均失去上网能力，此时所有节点启动**自荐**模式，若能够发现相同 SSID Password 的 Mesh 路由器 B，则节点会向根节点自荐，附上自己的 MAC 地址以及路由器 RSSI，根节点会选择最优的的自荐节点，最优的自荐节点收到通知后与父节点断开连接去连接路由器 B，同时原根节点通知其子节点重新去扫描发现新的潜在父节点，自己也进入扫描状态。



## 数据传输

ESP-Mesh-Lite 对于每个节点来说，都会使能 LWIP 协议栈，都可以看作直连路由器的一个设备，可以在应用层独立的调用 Socket、MQTT、HTTP 等网络接口。

### 双向数据流

下图展示了 ESP-MESH-LITE 叶子节点网络通信时双向数据流涉及的各种网络层。

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/components/mesh_lite/docs/_static/Schematic_diagram_of_bidirectional_data_flow.jpg" style="zoom:80%;" />

<center>ESP-MESH-LITE 双向数据流



## 性能

ESP-MESH-LITE 网络的性能可以基于以下多个指标进行评估：

**组网时长**：从头开始构建 ESP-MESH-LITE 网络所需的总时长。

**修复时间**：从网络检测到节点断开到执行适当操作（例如生成新的根节点或形成新的连接等）以修复网络所需的时间。

**每跳延迟**：数据每经过一次无线 hop 而经历的延迟，即从父节点向子节点（或从子节点向父节点）发送一个数据包所需的时间。

**网络节点容量**：ESP-MESH-LITE 网络可以同时支持的节点总数。该指标取决于节点可以接受到的最大下行连接数和网络中允许的最大层级。

ESP-MESH-LITE 网络的常见性能指标如下表所示：

- 组网时长：< 60 秒

- 修复时间

    根节点断开：< 50 秒

    子节点断开：< 45 秒

- 每跳延迟：8 到 12 毫秒

**备注**

- 上述性能指标的测试条件见下。

    - 测试设备数量：**50**

    - 最大允许下行连接数量：**6**

    - 最大允许层级：**6**

- 吞吐量取决于组网内设备数量以及层级数，同时还受到周围环境干扰的影响。

- 用户应注意，ESP-MESH-LITE 网络的性能与网络配置和工作环境密切相关。



## ESP-MESH-LITE 与 [ESP-MESH](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/esp-wifi-mesh.html) 差异

- ESP-MESH-LITE 相较于 ESP-MESH 会节省出更多的内存空间，但 ESP-MESH 的自组网和自修复的特性相对来说更加完善。

- ESP-MESH-LITE 跟 ESP-MESH 自动选择父节点方式不同。
    - ESP-MESH：在上电后所有设备处于空闲节点的时候进行竞选，根据 RSSI 选出根节点，之后根节点再连接路由器。
    - ESP-MESH-LITE：默认首先配网完上电的设备作为根节点，若同时有多个设备上电，设备都连到路由器上面后，所有设备开始广播路由器 RSSI，RSSI 最好的作为根节点，其余节点断开与路由器的连接，开始重新扫描发现新的父节点。

- ESP-MESH 只有根节点使能 LWIP 协议栈，所有子节点设备想要与外部网络通信都需要通过根节点的转发。



## ESPNOW 使用指南

为了方便多个功能模块同时使用 ESPNOW 进行通信，Mesh-Lite 组件内部将 ESPNOW 功能进行了封装。用户如需使用 ESPNOW 功能，请按照以下步骤进行：

1. 使用 `esp_mesh_lite_espnow_recv_cb_register()` 接口注册 ESPNOW 接收回调。第一个参数是所要发送和接收的 ESPNOW 数据类型。用户可以使用 `ESPNOW_DATA_TYPE_RESERVE`，或者自行在 `ESPNOW_DATA_TYPE_RESERVE` 后添加自定义类型。

   需要注意的是：`ESPNOW_DATA_TYPE_RESERVE`、`ESPNOW_DATA_TYPE_RM_GROUP_CONTROL`、`ESPNOW_DATA_TYPE_ZERO_PROV` 均已被占用。以下是代码使用示例：

   ```c
   static void espnow_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
   {
       esp_mesh_lite_espnow_event_t evt;
       espnow_recv_cb_t *recv_cb = &evt.info.recv_cb;
   
       if (recv_info->src_addr == NULL || data == NULL || len <= 0) {
           ESP_LOGE(TAG, "Receive cb arg error");
           return;
       }
   
       // The receiving callback function when receiving ESP-NOW. The receiving callback function also runs from the Wi-Fi task. So, do not do lengthy operations in the callback function.
       // Instead, post the necessary data to a queue and handle it from a lower priority task.
   }
   
   esp_mesh_lite_espnow_recv_cb_register(ESPNOW_DATA_TYPE_RESERVE, espnow_recv_cb);
   ```

2. 调用 `esp_mesh_lite_espnow_send` 发送接口进行数据发送。以下是代码使用示例：

   ```c
   void user_send_espnow(uint8_t dst_mac[ESP_NOW_ETH_ALEN], const uint8_t *payload, uint8_t payload_len)
   {
       if (esp_now_is_peer_exist(dst_mac) == false) {
           esp_now_peer_info_t *peer = malloc(sizeof(esp_now_peer_info_t));
           if (peer == NULL) {
               ESP_LOGE(TAG, "Malloc peer information fail");
               return;
           }
           memset(peer, 0, sizeof(esp_now_peer_info_t));
           peer->channel = 0;
           peer->ifidx = ESP_IF_WIFI_STA;
           peer->encrypt = false;
           // memcpy(peer->lmk, CONFIG_ESPNOW_LMK, ESP_NOW_KEY_LEN);
           memcpy(peer->peer_addr, dst_mac, ESP_NOW_ETH_ALEN);
           esp_now_add_peer(peer);
           free(peer);
       }
   
       if (esp_mesh_lite_espnow_send(ESPNOW_DATA_TYPE_RESERVE, dst_mac, payload, payload_len) != ESP_OK) {
           ESP_LOGE(TAG, "Send error");
       }
       return;
   }
   ```

**注意：** 

> 1. 用户无需执行 `esp_now_init()` 和 `esp_now_register_recv_cb(espnow_recv_cb)`，这两个 API 都已在 esp_mesh_lite_espnow_init 中实现。
> 2. 由于 payload 头会增加一个字节的 type 字段，因此单次发送的数据不能超过 249 字节（在 `esp_now_send()` 时需单次发送的数据不能超过 250 字节）。
> 3. 如果用户有加密需求，可以在 `esp_mesh_lite_espnow_init()` 中取消注释 `esp_now_set_pmk()`，并自行设置 PMK。



## 移植指南

用户若想要将原有的工程代码与 ESP-MESH-LITE 相结合，只需要关注配网时路由器连接操作，不要直接调用 esp_wifi_connect() 进行连接，而是要调用 esp_mesh_lite_connect()。

拿 Wi-Fi Provisioning 配网举例，在收到路由器 SSID 和 Password 之后，调用 esp_mesh_lite_connect()，ESP-MESH-LITE 内部会去扫描是否存在合适的父节点，若存在，则连接至首选父节点；若不存在，则连接路由器并作为根节点。

```c
/* Event handler for catching system events */
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    if (event_base == WIFI_PROV_EVENT) {
        switch (event_id) {
            case WIFI_PROV_START:
                ESP_LOGI(TAG, "Provisioning started");
                break;
            case WIFI_PROV_CRED_RECV: {
                wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
                ESP_LOGI(TAG, "Received Wi-Fi credentials"
                         "\n\tSSID     : %s\n\tPassword : %s",
                         (const char *) wifi_sta_cfg->ssid,
                         (const char *) wifi_sta_cfg->password);
#if CONFIG_MESH_LITE_ENABLE
                mesh_lite_sta_config_t config;
                memcpy((char*)config.ssid, (char*)wifi_sta_cfg->ssid, sizeof(config.ssid));
                memcpy((char*)config.password, (char*)wifi_sta_cfg->password, sizeof(config.password));
                config.bssid_set = wifi_sta_cfg->bssid_set;
                if (config.bssid_set) {
                    memcpy((char*)config.bssid, (char*)wifi_sta_cfg->bssid, sizeof(config.bssid));
                }
                esp_mesh_lite_set_router_config(&config);
                esp_mesh_lite_connect();
#else
                esp_wifi_set_storage(WIFI_STORAGE_FLASH);
                esp_wifi_set_config(ESP_IF_WIFI_STA, (wifi_config_t*)wifi_sta_cfg);
                esp_wifi_set_storage(WIFI_STORAGE_RAM);

                esp_wifi_disconnect();
                esp_wifi_connect();
#endif /* CONFIG_MESH_LITE_ENABLE */
                break;
            }
        }
    }
}
```

**备注**

> 具体配网代码修改请参考 [Wi-Fi Provisioning](https://github.com/espressif/esp-mesh-lite/blob/master/examples/rainmaker/led_light/components/app_wifi/app_wifi.c)
>
> 除了配网连接部分的修改，其余的网络应用（Socket、MQTT、HTTP 等）均不需要修改



## 更多注意事项

- 数据传输使用 Wi-Fi WPA2-PSK 加密（SoftAP 需设置密码）
- Mesh 网络内部节点间通信可以通过 `esp_mesh_lite_aes_set_key` 选择使用 AES128 加密（见 [Mesh-Lite API 详解](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/include/esp_mesh_lite_core.h)）

本文图片中使用的路由器与互联网图标来自 [www.flaticon.com](https://smashicons.com/) 的 [Smashicons](https://smashicons.com/)。
