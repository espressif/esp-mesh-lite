/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: mesh_lite.proto */

#ifndef PROTOBUF_C_mesh_5flite_2eproto__INCLUDED
#define PROTOBUF_C_mesh_5flite_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1005000 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif

typedef struct MeshLite__NodeData MeshLite__NodeData;
typedef struct MeshLite__Data MeshLite__Data;

/* --- enums --- */

/* --- messages --- */

struct  MeshLite__NodeData {
    ProtobufCMessage base;
    uint32_t node_level;
    uint32_t node_ip;
    ProtobufCBinaryData node_mac;
};
#define MESH_LITE__NODE_DATA__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mesh_lite__node_data__descriptor) \
, 0, 0, {0,NULL} }

struct  MeshLite__Data {
    ProtobufCMessage base;
    size_t n_nodes;
    MeshLite__NodeData **nodes;
};
#define MESH_LITE__DATA__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mesh_lite__data__descriptor) \
, 0,NULL }

/* MeshLite__NodeData methods */
void   mesh_lite__node_data__init
(MeshLite__NodeData         *message);
size_t mesh_lite__node_data__get_packed_size
(const MeshLite__NodeData   *message);
size_t mesh_lite__node_data__pack
(const MeshLite__NodeData   *message,
 uint8_t             *out);
size_t mesh_lite__node_data__pack_to_buffer
(const MeshLite__NodeData   *message,
 ProtobufCBuffer     *buffer);
MeshLite__NodeData *
mesh_lite__node_data__unpack
(ProtobufCAllocator  *allocator,
 size_t               len,
 const uint8_t       *data);
void   mesh_lite__node_data__free_unpacked
(MeshLite__NodeData *message,
 ProtobufCAllocator *allocator);
/* MeshLite__Data methods */
void   mesh_lite__data__init
(MeshLite__Data         *message);
size_t mesh_lite__data__get_packed_size
(const MeshLite__Data   *message);
size_t mesh_lite__data__pack
(const MeshLite__Data   *message,
 uint8_t             *out);
size_t mesh_lite__data__pack_to_buffer
(const MeshLite__Data   *message,
 ProtobufCBuffer     *buffer);
MeshLite__Data *
mesh_lite__data__unpack
(ProtobufCAllocator  *allocator,
 size_t               len,
 const uint8_t       *data);
void   mesh_lite__data__free_unpacked
(MeshLite__Data *message,
 ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*MeshLite__NodeData_Closure)
(const MeshLite__NodeData *message,
 void *closure_data);
typedef void (*MeshLite__Data_Closure)
(const MeshLite__Data *message,
 void *closure_data);

/* --- services --- */

/* --- descriptors --- */

extern const ProtobufCMessageDescriptor mesh_lite__node_data__descriptor;
extern const ProtobufCMessageDescriptor mesh_lite__data__descriptor;

PROTOBUF_C__END_DECLS

#endif  /* PROTOBUF_C_mesh_5flite_2eproto__INCLUDED */
