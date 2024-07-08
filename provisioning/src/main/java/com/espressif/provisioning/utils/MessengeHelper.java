// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


package com.espressif.provisioning.utils;

import android.content.SharedPreferences;
import android.util.Log;

import com.google.protobuf.ByteString;

import espressif.WifiConfig;
import espressif.WifiScan;

public class MessengeHelper {

    private static String mesh_softap_ssid, mesh_softap_password;
    private static int mesh_id;
    private  static Boolean mesh_node_actor;

    public static void setMesh_node_actor(Boolean actor) {
        mesh_node_actor = actor;
    }

    public static boolean getMesh_node_actor() {
        return mesh_node_actor;
    }

    public static void setMesh_id(int MeshId) {
        mesh_id = MeshId;
    }

    public static int getMesh_id() {
        return mesh_id;
    }

    public static void setMesh_softap_password(String softap_password) {
        mesh_softap_password = softap_password;
    }

    public static String getMesh_softap_password() {
        return mesh_softap_password;
    }

    public static void setMesh_softap_ssid(String softap_ssid) {
        mesh_softap_ssid = softap_ssid;
    }

    public static String getMesh_softap_ssid() {
        return mesh_softap_ssid;
    }

    // Send Wi-Fi Scan command
    public static byte[] prepareWiFiScanMsg() {

        WifiScan.CmdScanStart configRequest = WifiScan.CmdScanStart.newBuilder()
                .setBlocking(true)
                .setPassive(false)
                .setGroupChannels(0)
                .setPeriodMs(120)
                .build();
        WifiScan.WiFiScanMsgType msgType = WifiScan.WiFiScanMsgType.TypeCmdScanStart;
        WifiScan.WiFiScanPayload payload = WifiScan.WiFiScanPayload.newBuilder()
                .setMsg(msgType)
                .setCmdScanStart(configRequest)
                .build();

        return payload.toByteArray();
    }

    public static byte[] prepareGetWiFiScanStatusMsg() {

        WifiScan.CmdScanStatus configRequest = WifiScan.CmdScanStatus.newBuilder()
                .build();
        WifiScan.WiFiScanMsgType msgType = WifiScan.WiFiScanMsgType.TypeCmdScanStatus;
        WifiScan.WiFiScanPayload payload = WifiScan.WiFiScanPayload.newBuilder()
                .setMsg(msgType)
                .setCmdScanStatus(configRequest)
                .build();
        return payload.toByteArray();
    }

    // Get Wi-Fi scan list
    public static byte[] prepareGetWiFiScanListMsg(int start, int count) {

        WifiScan.CmdScanResult configRequest = WifiScan.CmdScanResult.newBuilder()
                .setStartIndex(start)
                .setCount(count)
                .build();
        WifiScan.WiFiScanMsgType msgType = WifiScan.WiFiScanMsgType.TypeCmdScanResult;
        WifiScan.WiFiScanPayload payload = WifiScan.WiFiScanPayload.newBuilder()
                .setMsg(msgType)
                .setCmdScanResult(configRequest)
                .build();

        return payload.toByteArray();
    }

    // Send Wi-Fi Config
    public static byte[] prepareWiFiConfigMsg(String ssid, String passphrase) {

        WifiConfig.CmdSetConfig cmdSetConfig;
        String mesh_ssid = "";
        String mesh_password = "";
        int mesh_id = 0;
        if (!getMesh_node_actor()) {
            mesh_ssid = getMesh_softap_ssid();
            mesh_password = getMesh_softap_password();
            mesh_id = getMesh_id();
        }

        if (passphrase != null) {
            cmdSetConfig = WifiConfig.CmdSetConfig
                    .newBuilder()
                    .setSsid(ByteString.copyFrom(ssid.getBytes()))
                    .setPassphrase(ByteString.copyFrom(passphrase.getBytes()))
                    .setMeshId(mesh_id)
                    .setSoftapSsid(ByteString.copyFrom(mesh_ssid.getBytes()))
                    .setSoftapPassphrase(ByteString.copyFrom(mesh_password.getBytes()))
                    .build();
        } else {
            cmdSetConfig = WifiConfig.CmdSetConfig
                    .newBuilder()
                    .setSsid(ByteString.copyFrom(ssid.getBytes()))
                    .setMeshId(mesh_id)
                    .setSoftapSsid(ByteString.copyFrom(mesh_ssid.getBytes()))
                    .setSoftapPassphrase(ByteString.copyFrom(mesh_password.getBytes()))
                    .build();
        }
        WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload
                .newBuilder()
                .setCmdSetConfig(cmdSetConfig)
                .setMsg(WifiConfig.WiFiConfigMsgType.TypeCmdSetConfig)
                .build();
        Log.d("MeshLite",  wiFiConfigPayload.toString());
        return wiFiConfigPayload.toByteArray();
    }

    // Apply Wi-Fi config
    public static byte[] prepareApplyWiFiConfigMsg() {

        WifiConfig.CmdApplyConfig cmdApplyConfig = WifiConfig.CmdApplyConfig
                .newBuilder()
                .build();
        WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload
                .newBuilder()
                .setCmdApplyConfig(cmdApplyConfig)
                .setMsg(WifiConfig.WiFiConfigMsgType.TypeCmdApplyConfig)
                .build();
        return wiFiConfigPayload.toByteArray();
    }

    // Get Wi-Fi Config status
    public static byte[] prepareGetWiFiConfigStatusMsg() {

        WifiConfig.CmdGetStatus cmdGetStatus = WifiConfig.CmdGetStatus
                .newBuilder()
                .build();
        WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload
                .newBuilder()
                .setCmdGetStatus(cmdGetStatus)
                .setMsg(WifiConfig.WiFiConfigMsgType.TypeCmdGetStatus)
                .build();
        return wiFiConfigPayload.toByteArray();
    }
}
