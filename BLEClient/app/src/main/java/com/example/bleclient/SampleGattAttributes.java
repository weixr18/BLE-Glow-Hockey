/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.bleclient;

import java.util.HashMap;

/**
 * This class includes a small subset of standard GATT attributes for demonstration purposes.
 */
public class SampleGattAttributes {
    private static HashMap<String, String> attributes = new HashMap();
    public static String HEART_RATE_MEASUREMENT = "00002a37-0000-1000-8000-00805f9b34fb";
    public static String CLIENT_CHARACTERISTIC_CONFIG = "00002902-0000-1000-8000-00805f9b34fb";

    public static String UUID_S_GH_COMMAND        = "00030700-0000-1000-8000-00805f9b0131";
    public static String UUID_S_GH_POSITION       = "00030710-0000-1000-8000-00805f9b0131";
    public static String UUID_C_COMMAND_N         = "00030701-0000-1000-8000-00805f9b0131";
    public static String UUID_C_COMMAND_W         = "00030702-0000-1000-8000-00805f9b0131";
    public static String UUID_C_PLAYER_POSITION_N = "00030711-0000-1000-8000-00805f9b0131";
    public static String UUID_C_OPPOSITE_POSITION = "00030713-0000-1000-8000-00805f9b0131";
    public static String UUID_C_BALL_POSITION     = "00030713-0000-1000-8000-00805f9b0131";
    public static String UUID_C_PLAYER_POSITION_W = "00030714-0000-1000-8000-00805f9b0131";


    static {
        // Sample Services.
        attributes.put("0000180d-0000-1000-8000-00805f9b34fb", "Heart Rate Service");
        attributes.put("0000180a-0000-1000-8000-00805f9b34fb", "Device Information Service");
        // Sample Characteristics.
        attributes.put(HEART_RATE_MEASUREMENT, "Heart Rate Measurement");
        attributes.put("00002a29-0000-1000-8000-00805f9b34fb", "Manufacturer Name String");
    }

    public static String lookup(String uuid, String defaultName) {
        String name = attributes.get(uuid);
        return name == null ? defaultName : name;
    }
}
