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

import android.annotation.SuppressLint;
import android.app.Activity;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;

import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import static android.bluetooth.le.ScanSettings.SCAN_MODE_LOW_LATENCY;

/**
 * Activity for scanning and displaying available Bluetooth LE devices.
 */
@SuppressLint("NewApi")
public class DeviceScanActivity extends Activity {
    private static final String TAG = "Scan Page";
	

    //private static final int REQUEST_ENABLE_BT = 1;

    private static final long SCAN_TIME = 10000;

    private boolean mScanning = false;
    private Handler mHandler;
    private BluetoothAdapter mBluetoothAdapter;
    private BluetoothLeScanner mBLEScanner;
    private ScanCallback mScanCallback;
    private ScanSettings mScanSettings;

    private List<BluetoothDevice> mDeviceList;
    private DeviceAdapter mDeviceListAdapter;
    private ListView mListView;
    private AdapterView.OnItemClickListener mItemClickListener;



    /**
     * 页面创建
     * */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "Create");

        // 窗体及必要组件初始化
        super.onCreate(savedInstanceState);
        getActionBar().setTitle(R.string.title_devices);
        setContentView(R.layout.device_scan_activity);

        mHandler = new Handler();
        mDeviceList = new ArrayList<>();
        mDeviceListAdapter = new DeviceAdapter(
                DeviceScanActivity.this,
                R.layout.listitem_device,
                mDeviceList
        );
        mItemClickListener = getItemClickListener();
        mListView = this.findViewById(R.id.list_view);
        mListView.setAdapter(mDeviceListAdapter);
        mListView.setOnItemClickListener(mItemClickListener);


        // 检查设备是否支持ble
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
            finish();
        }
        Log.d(TAG, "BLE support checked");


        /**
         * 初始化ble设配器 mBluetoothAdapter
         */
        final BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();
        //如果没打开蓝牙，不进行扫描操作
        if (mBluetoothAdapter == null || !mBluetoothAdapter.isEnabled()) {
            Toast.makeText(this, R.string.error_bluetooth_not_supported, Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        //设置扫描回调 mScanCallback
        mScanCallback = getScanCallback();

        // 创建 ScanSettings 的 build 对象用于设置扫描参数 mScanSettings
        ScanSettings.Builder builder = new ScanSettings.Builder()
                .setScanMode(SCAN_MODE_LOW_LATENCY);//设置高功耗模式
        //定义回调类型
        builder.setCallbackType(ScanSettings.CALLBACK_TYPE_ALL_MATCHES);
        //设置蓝牙LE扫描滤波器硬件匹配的匹配模式
        builder.setMatchMode(ScanSettings.MATCH_MODE_STICKY);
        //若芯片组支持批处理芯片上的扫描
        if (mBluetoothAdapter.isOffloadedScanBatchingSupported()) {
            //设置蓝牙LE扫描的报告延迟的时间（以毫秒为单位）
            //设置为0以立即通知结果
            builder.setReportDelay(0L);
        }
        mScanSettings = builder.build();

        /**
         * 开始扫描
         */
        if (!mScanning){
            mScanning = true;
            if (mBLEScanner == null){
                mBLEScanner = mBluetoothAdapter.getBluetoothLeScanner();
            }

            //开始扫描
            Log.d(TAG, "Scanning start!");
            mBLEScanner.startScan(null, mScanSettings, mScanCallback);

            //设置结束扫描
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {

                    mScanning = false;
                    mBLEScanner.stopScan(mScanCallback);
                    Log.d(TAG, "Scanning stop!");

                }
            },SCAN_TIME);
        }

    }


    /**
     * 扫描回调
     */
    private ScanCallback getScanCallback() {
        return new ScanCallback() {
            //当一个蓝牙ble广播被发现时回调
            @Override
            public void onScanResult(int callbackType, ScanResult result) {
                super.onScanResult(callbackType, result);
                //扫描类型有开始扫描时传入的ScanSettings相关
                //对扫描到的设备进行操作。如：获取设备信息。
                BluetoothDevice device = result.getDevice();
                if(!mDeviceList.contains(device)){
                    mDeviceList.add(device);
                }
                //adapter更新视图
                mDeviceListAdapter.notifyDataSetChanged();
            }

            //批量返回扫描结果
            //@param results 以前扫描到的扫描结果列表。
            @Override
            public void onBatchScanResults(List<ScanResult> results) {
                super.onBatchScanResults(results);

                Log.d(TAG, "multi device found!!");

            }

            //当扫描不能开启时回调
            @Override
            public void onScanFailed(int errorCode) {
                super.onScanFailed(errorCode);
                /** 扫描太频繁会返回
                 * ScanCallback.SCAN_FAILED_APPLICATION_REGISTRATION_FAILED，
                 * 表示app无法注册，无法开始扫描。
                 */
                Log.d(TAG, "Error when scanning");
            }
        };
    }


    /**
     * 点击回调
     * */
    AdapterView.OnItemClickListener getItemClickListener(){
        return new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView parent, View view, int position, long id){
                if(id == -1) {
                    // headerView or footerView
                    return;
                }

                Log.d(TAG, String.format("ListItemClick : %d", position));
                int realPosition = (int) id;
                Object device = parent.getItemAtPosition(realPosition);
                if(device.getClass() == BluetoothDevice.class){

                    Log.d(TAG, "Address : " + ((BluetoothDevice) device).getAddress());

                    final Intent intent = new Intent(
                            DeviceScanActivity.this,
                            DeviceControlActivity.class);
                    intent.putExtra(DeviceControlActivity.EXTRAS_DEVICE_NAME, ((BluetoothDevice) device).getName());
                    intent.putExtra(DeviceControlActivity.EXTRAS_DEVICE_ADDRESS, ((BluetoothDevice) device).getAddress());
                    if (mScanning) {
                        mBLEScanner.stopScan(mScanCallback);
                        mScanning = false;
                    }
                    startActivity(intent);
                }

            }
        };
    }


    /**
     * 列表适配器
     */
    public class DeviceAdapter extends ArrayAdapter<BluetoothDevice> {
        private int mResourceId;
        private List<BluetoothDevice> mDeviceList;

        // 适配器的构造函数，把要适配的数据传入这里
        public DeviceAdapter(Context context, int textViewResourceId, List<BluetoothDevice> deviceList){
            super(context, textViewResourceId, deviceList);
            mResourceId = textViewResourceId;
            mDeviceList = deviceList;
        }

        // convertView 参数用于将之前加载好的布局进行缓存
        @Override
        public View getView(int position, View convertView, ViewGroup parent){
            BluetoothDevice device = getItem(position); //获取当前项的实例

            // 加个判断，以免ListView每次滚动时都要重新加载布局，以提高运行效率
            View view;
            ViewHolder viewHolder;
            if (convertView==null){

                // 避免ListView每次滚动时都要重新加载布局，以提高运行效率
                view=LayoutInflater.from(getContext()).inflate(
                        mResourceId, parent,false
                );
                // 避免每次调用getView()时都要重新获取控件实例
                viewHolder = new ViewHolder();
                viewHolder.deviceAddress=view.findViewById(R.id.device_address);
                viewHolder.deviceName=view.findViewById(R.id.device_name);

                // 将ViewHolder存储在View中（即将控件的实例存储在其中）
                view.setTag(viewHolder);
            } else{
                view = convertView;
                viewHolder = (ViewHolder) view.getTag();
            }

            // 获取控件实例，并调用set...方法使其显示出来
            if(viewHolder.deviceAddress != null){
                viewHolder.deviceAddress.setText(device.getAddress());
            }
            if(viewHolder.deviceName != null){
                viewHolder.deviceName.setText(device.getName());
            }

            return view;
        }

        public void addDevice(BluetoothDevice device) {
            if(!mDeviceList.contains(device)) {
                mDeviceList.add(device);
            }
        }

        public BluetoothDevice getDevice(int position) {
            return mDeviceList.get(position);
        }

        public void clear() {
            if(mDeviceList!=null) mDeviceList.clear();
        }

        @Override
        public int getCount() {
            return mDeviceList==null ? 0 : mDeviceList.size();
        }

        @Override
        public BluetoothDevice getItem(int i) {
            return mDeviceList.get(i);
        }

        @Override
        public long getItemId(int i) {
            return i;
        }



    }

    static class ViewHolder {
        TextView deviceName;
        TextView deviceAddress;
    }


}