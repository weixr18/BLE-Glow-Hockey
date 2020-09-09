package com.example.bleclient;

import android.app.Activity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;

import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ExpandableListView;
import android.widget.TextView;

//import androidx.appcompat.app.AppCompatActivity;
//import java.io.PrintStream;
//import java.util.Random;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;
import java.lang.ref.WeakReference;

public class GameActivity extends Activity {

    /********************************** Variables ***************************************/

    final private static String TAG = "Game Page";

    /********Inner variables*********/

    final private static int MSG_WHAT = 170411;
    final private static int TIMER_PERIOD = 16;     //ms

    private Timer mTimer = new Timer();
    private TimerTask mTimerTask = getNewTimerTask();
    private myHandler mHandler = null;
    private GameView mGameView = null;

    /********Control related*********/

    //GH_CMASK: Glow Hockey Command Mask
    public static int GH_CMASK_OPRAND = 0x1f;
    public static int GH_CMASK_DATA = 0x7e0;

    //GH_CC: Glow Hockey Command Code

    public static int GH_CC_W_PAUSE = 0x02;
    public static int GH_CC_W_RESUME = 0x03;
    public static int GH_CC_W_CHOOSE_COMPLETE = 0x06;

    public static int GH_CC_N_READY = 0x10;
    public static int GH_CC_N_START = 0x11;
    public static int GH_CC_N_PAUSE = 0x12;
    public static int GH_CC_N_RESUME = 0x13;
    public static int GH_CC_N_SCORE = 0x14;
    public static int GH_CC_N_OVER = 0x15;
    public static int GH_CC_N_CHOOSE_COLOR = 0x16;
    public static int GH_CC_N_NOTIFY_ID = 0x18;
    public static int GH_CC_N_NOTIFY_COLOR = 0x19;

    public enum ClientGameState{
        Connecting,
        Initializing,
        ChooseColor,
        Waiting,
        Running,
        Pause,
        Score,
        Over,
        Replay
    }

    ClientGameState mGameState = ClientGameState.Connecting;

    // game states
    private int playerID;
    private boolean isOver = false;
    private boolean isStart = false;
    private int scoreA = 0;
    private int scoreB = 0;
    private int choosingColorPlayer = 2;



    /**********BLE related***********/
    public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
    public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";

    public static final String UUID_S_GH_COMMAND        = "00030700-0000-1000-8000-00805f9b0131";
    public static final String UUID_S_GH_POSITION       = "00030710-0000-1000-8000-00805f9b0131";
    public static final String UUID_C_COMMAND_N         = "00030701-0000-1000-8000-00805f9b0131";
    public static final String UUID_C_COMMAND_W         = "00030702-0000-1000-8000-00805f9b0131";
    public static final String UUID_C_PLAYER_POSITION_N = "00030711-0000-1000-8000-00805f9b0131";
    public static final String UUID_C_OPPOSITE_POSITION = "00030713-0000-1000-8000-00805f9b0131";
    public static final String UUID_C_BALL_POSITION     = "00030713-0000-1000-8000-00805f9b0131";
    public static final String UUID_C_PLAYER_POSITION_W = "00030714-0000-1000-8000-00805f9b0131";

    // BLE components
    private String mDeviceName;
    private String mDeviceAddress;
    private BluetoothLeService mBluetoothLeService;
    private BluetoothGattService mGHPositionService;
    private BluetoothGattService mGHCommandService;
    private BluetoothGattCharacteristic mGH_COMMAND_N_Characteristic;
    private BluetoothGattCharacteristic mGH_COMMAND_W_Characteristic;
    private BluetoothGattCharacteristic mGH_Player_Position_N_Characteristic;
    private BluetoothGattCharacteristic mGH_Opposite_Position_Characteristic;
    private BluetoothGattCharacteristic mGH_Ball_Position_Characteristic;
    private BluetoothGattCharacteristic mGH_Player_Position_W_Characteristic;
    private boolean mConnected = false;

    /**********Game related***********/

    // player & Ball parameters
    private int player_A_PositionX;
    private int player_A_PositionY;
    private int ballPositionX;
    private int ballPositionY;
    private int player_B_PositionX;
    private int player_B_PositionY;

    // canvas const parameters
    final private static int STD_SCREEN_WIDTH = 1080;
    final private static int STD_SCREEN_HEIGHT = 1920;
    final private static double PADDING_LEFT_RATE = 0.04;
    final private static double PADDING_RIGHT_RATE = 0.04;
    final private static double PADDING_TOP_RATE = 0.04;
    final private static double PADDING_BOTTOM_RATE = 0.04;
    final private static double BALL_SIZE_RATIO = 0.05;
    final private static double BALL_INITIAL_RATE_X = 0.5;
    final private static double BALL_INITIAL_RATE_Y = 0.5;
    final private static double DOOR_LEFT_RATE = 0.35;
    final private static double DOOR_RIGHT_RATE = 0.65;
    final private static double playerCircleSize_RATE = 0.1;
    final private static double playerInnerCircleSize_RATE = 0.072;

    final private static double BUTTON_TOP_RATE = 0.8;
    final private static double BUTTON_BOTTOM_RATE = 0.9;
    final private static double BUTTON_LL_RATE = 0.2;
    final private static double BUTTON_LR_RATE = 0.45;
    final private static double BUTTON_RL_RATE = 0.55;
    final private static double BUTTON_RR_RATE = 0.8;

    final private static double BUTTON_M_TOP_RATE = 0.55;
    final private static double BUTTON_M_BOTTOM_RATE = 0.63;
    final private static double BUTTON_M_LEFT_RATE = 0.38;
    final private static double BUTTON_M_RIGHT_RATE = 0.62;



    final private static double PLAYER_A_INITIAL_RATE_X = 0.5;
    final private static double PLAYER_A_INITIAL_RATE_Y = 0.8;
    final private static double PLAYER_B_INITIAL_RATE_X = 0.5;
    final private static double PLAYER_B_INITIAL_RATE_Y = 0.2;


    // canvas parameters
    private int tableWidth;
    private int tableHeight;
    private int gamePaddingLeft;
    private int gamePaddingRight;
    private int gamePaddingTop;
    private int gamePaddingBottom;
    private int doorSideLeft;
    private int doorSideRight;
    private int playerCircleSize;
    private int playerInnerCircleSize;
    private int ballSize;
    private int buttonTop;
    private int buttonBottom;
    private int buttonMTop;
    private int buttonMBottom;
    private int buttonMLeft;
    private int buttonMRight;


    private Rect fullZone;
    private Rect gameZone;      // 游戏区域
    private Rect doorZoneB;     //对方门
    private Rect doorZoneA;     //己方门
    private Rect buttonLeft;     //下方左侧按钮
    private Rect buttonRight;     //下方左侧按钮
    private Rect buttonMiddle;  // 中间单个按钮


    private int player_A_Color = Color.rgb(124,249,102);
    private int player_B_Color = Color.rgb(220,240,120);



    /********************************** OnCreate ***********************************/


    /**
     * onCreate
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.d(TAG, "GamePage Create");

        // ???
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        //本地组件设置
        mGameView = new GameView(this);          //游戏视图实例化
        setContentView(mGameView);                       //设置Activity的视图为游戏视图
        mHandler = new myHandler(this);                 //mHandler实例化
        mGameView.setOnTouchListener(listener);          //设置触摸事件监听器

        //蓝牙服务设置

        // 获取设备名称、地址
        final Intent intent = getIntent();
        mDeviceName = intent.getStringExtra(EXTRAS_DEVICE_NAME);
        mDeviceAddress = intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);

        // 绑定蓝牙服务
        Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);

        // UI 初始化
        this.UIInitialize();

        //定时任务开始
        mTimer.schedule(mTimerTask , 0, TIMER_PERIOD);

    }



    /****************************** BLE Components *****************************/


    /**
     * service生命周期管理
     */
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {

            //get Bluetooth Le service instance
            Log.d(TAG, "Service Connected");
            mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }

            // Automatically connects to the device
            mBluetoothLeService.connect(mDeviceAddress);
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            Log.d(TAG, "Service Disconnected");
            mBluetoothLeService = null;
        }
    };


    @Override
    protected void onResume(){
        Log.d(TAG, "GamePage Resume");
        super.onResume();
        // 注册广播接收器
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (mBluetoothLeService != null) {
            final boolean result = mBluetoothLeService.connect(mDeviceAddress);
            Log.d(TAG, "Connect request result=" + result);
        }
    }

    /**
     * 用于GATT广播接收的intent过滤器
     */
    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }


    /**
     * GATT广播接收器
     */
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            // GATT connected
            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                Log.d(TAG, "GATT_CONNECTED");
                mConnected = true;
            }

            // GATT disconnect
            else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                Log.d(TAG, "GATT_DISCONNECTED");
                mConnected = false;
                invalidateOptionsMenu();
            }

            // GATT service discovered
            else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                Log.d(TAG, "GATT_SERVICES_DISCOVERED");

                // get GATT services
                List<BluetoothGattService> gattServices = mBluetoothLeService.getSupportedGattServices();
                for (BluetoothGattService gattService : gattServices) {
                    String uuid = gattService.getUuid().toString();
                    Log.d(TAG, "Service found: "+uuid);
                    if(uuid.equals(UUID_S_GH_POSITION)){
                        mGHPositionService = gattService;
                        Log.d(TAG, "GH Position Service found.");
                    }
                    if(uuid.equals(UUID_S_GH_COMMAND)){
                        mGHCommandService = gattService;
                        Log.d(TAG, "GH Command Service found.");
                    }
                }

                // Get Characteristics
                if(mGHCommandService != null){
                    List<BluetoothGattCharacteristic> GHCommandCharacteristics =
                            mGHCommandService.getCharacteristics();
                    for (BluetoothGattCharacteristic gattCharacteristic : GHCommandCharacteristics) {
                        if(gattCharacteristic.getUuid().toString().equals(UUID_C_COMMAND_N)){
                            mGH_COMMAND_N_Characteristic = gattCharacteristic;
                            // set notification enable True
                            mBluetoothLeService.setCharacteristicNotification(
                                    mGH_COMMAND_N_Characteristic, true);
                        }
                        else if(gattCharacteristic.getUuid().toString().equals(UUID_C_COMMAND_W)){
                            mGH_COMMAND_W_Characteristic = gattCharacteristic;
                        }
                    }
                }
                else {
                    Log.d(TAG, "GH Command Service is null!");
                }

                if(mGHPositionService != null){
                    List<BluetoothGattCharacteristic> GHPositionCharacteristics =
                            mGHPositionService.getCharacteristics();
                    for (BluetoothGattCharacteristic gattCharacteristic : GHPositionCharacteristics) {
                        if(gattCharacteristic.getUuid().toString().equals(UUID_C_PLAYER_POSITION_N)){
                            try{
                                Thread.sleep(100);
                            }
                            catch (Exception e){
                                Log.e(TAG, e.toString());
                            }

                            mGH_Player_Position_N_Characteristic = gattCharacteristic;
                            // set notification enable True
                            mBluetoothLeService.setCharacteristicNotification(
                                    mGH_Player_Position_N_Characteristic, true);
                        }
                        else if(gattCharacteristic.getUuid().toString().equals(UUID_C_OPPOSITE_POSITION)){
                            mGH_Opposite_Position_Characteristic = gattCharacteristic;
                        }
                        else if(gattCharacteristic.getUuid().toString().equals(UUID_C_BALL_POSITION)){
                            mGH_Ball_Position_Characteristic = gattCharacteristic;
                        }
                        else if(gattCharacteristic.getUuid().toString().equals(UUID_C_PLAYER_POSITION_W)){
                            mGH_Player_Position_W_Characteristic = gattCharacteristic;
                        }
                    }
                }
                else {
                    Log.d(TAG, "GH Position Service is null!");
                }
            }

            // data available
            else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {

                String uuid = intent.getStringExtra(BluetoothLeService.EXTRA_UUID);

                if(uuid == null){
                    Log.d(TAG, "UUID is null!!");
                }

                // command
                else if(uuid.equals(UUID_C_COMMAND_N)){

                    int command = intent.getIntExtra(BluetoothLeService.GH_COMMAND, 0);
                    int oprand = command & GH_CMASK_OPRAND;
                    int data = (command & GH_CMASK_DATA) >> 5;
                    //Log.d(TAG, String.format("COMMAND_N received :%x, %x", oprand, data));

                    // notify id
                    if(oprand == GH_CC_N_NOTIFY_ID){
                        playerID = data;
                        mGameState = ClientGameState.Initializing;
                    }
                    // notify color
                    else if(oprand == GH_CC_N_NOTIFY_COLOR){
                        if(mGameState == ClientGameState.ChooseColor){
                            int red = (command >> 8) & 0xff;
                            int green = (command >> 16) & 0xff;
                            int blue = (command >> 24) & 0xff;

                            Log.d(TAG, String.format("Color: R %d G %d B %d", red, green, blue));
                            if(red == 2 || red == 3 || green == 2 || green == 3 || blue == 2 || blue == 3){
                                //do nothing
                            }
                            else{
                                if(choosingColorPlayer == playerID){
                                    player_A_Color = Color.rgb(red, green, blue);
                                }
                                else {
                                    player_B_Color = Color.rgb(red, green, blue);
                                }
                            }

                        }

                    }
                    // ready
                    else if(oprand == GH_CC_N_READY){
                        mGameState = ClientGameState.Waiting;
                    }
                    // choose color
                    else if(oprand == GH_CC_N_CHOOSE_COLOR){
                        mGameState = ClientGameState.ChooseColor;
                        choosingColorPlayer = data;

                    }
                    // start
                    else if(oprand == GH_CC_N_START){
                        mGameState = ClientGameState.Running;
                        Log.d(TAG, "Game start!");
                    }
                    // over
                    else if(oprand == GH_CC_N_OVER){
                        mGameState = ClientGameState.Over;
                        Log.d(TAG, "Game over!");
                    }
                    // score
                    else if(oprand == GH_CC_N_SCORE){
                        Log.d(TAG, "Score!!!!!!!");
                        int player0Score = data & 0x07;
                        int player1Score = (data >> 3) & 0x07;

                        if(playerID == 0){
                            scoreA = player0Score;
                            scoreB = player1Score;
                        }
                        else {
                            scoreA = player1Score;
                            scoreB = player0Score;
                        }
                    }
                }


                // position
                else if(uuid.equals(UUID_C_PLAYER_POSITION_N)){

                   int ballPosition = intent.getIntExtra(BluetoothLeService.BALL_POSITION, 0);
                    int oppositePosition = intent.getIntExtra(BluetoothLeService.OPPOSITE_POSITION, 0);

                    int stdBallPositionX = ballPosition & 0x0fff;
                    int stdBallPositionY = (ballPosition >> 12) & 0x0fff;
                    ballPositionX = stdBallPositionX * tableWidth / STD_SCREEN_WIDTH ;
                    ballPositionY = stdBallPositionY * tableHeight / STD_SCREEN_HEIGHT;


                    int stdOppositePositionX = oppositePosition & 0x0fff;
                    int stdOppositePositionY = (oppositePosition >> 12) & 0x0fff;

                    player_B_PositionX = stdOppositePositionX * tableWidth / STD_SCREEN_WIDTH ;
                    player_B_PositionY = stdOppositePositionY * tableHeight / STD_SCREEN_HEIGHT;

                }


                else{
                    Log.d(TAG, "Receive unknown characteristic.");
                }

            }
        }
    };


    /******************************* UI Managements ******************************/


    /**
     * UI初始化
     */
    private void UIInitialize(){

        int buttonLL;
        int buttonLR;
        int buttonRL;
        int buttonRR;

        //获取屏幕参数
        WindowManager windowManager = getWindowManager();
        Display display = windowManager.getDefaultDisplay();
        DisplayMetrics metrics = new DisplayMetrics();
        display.getMetrics(metrics);

        Log.d(TAG, String.format("Width: %d, Height: %d", metrics.widthPixels, metrics.heightPixels));

        tableWidth = metrics.widthPixels;
        tableHeight = (int) (metrics.heightPixels * 0.9);
        gamePaddingLeft = (int) (tableWidth * PADDING_LEFT_RATE);
        gamePaddingRight = (int) (tableWidth * PADDING_RIGHT_RATE);
        gamePaddingTop = (int) (tableWidth * PADDING_TOP_RATE);
        gamePaddingBottom = (int) (tableWidth * PADDING_BOTTOM_RATE);
        doorSideLeft = (int) (tableWidth * DOOR_LEFT_RATE);
        doorSideRight = (int) (tableWidth * DOOR_RIGHT_RATE);

        ballSize = (int) (tableWidth * BALL_SIZE_RATIO);
        ballPositionX = (int) (tableWidth * BALL_INITIAL_RATE_X);
        ballPositionY = (int) (tableHeight * BALL_INITIAL_RATE_Y);

        buttonTop  = (int) (tableHeight * BUTTON_TOP_RATE);
        buttonBottom  = (int) (tableHeight * BUTTON_BOTTOM_RATE);
        buttonLL  = (int) (tableWidth * BUTTON_LL_RATE);
        buttonLR  = (int) (tableWidth * BUTTON_LR_RATE);
        buttonRL  = (int) (tableWidth * BUTTON_RL_RATE);
        buttonRR  = (int) (tableWidth * BUTTON_RR_RATE);

        buttonMTop = (int) (tableHeight * BUTTON_M_TOP_RATE);
        buttonMBottom = (int) (tableHeight * BUTTON_M_BOTTOM_RATE);
        buttonMLeft = (int) (tableWidth * BUTTON_M_LEFT_RATE);
        buttonMRight = (int) (tableWidth * BUTTON_M_RIGHT_RATE);
        Log.d(TAG, String.format(
                "Button middle (top bottom left right): (%d %d %d %d)",
                buttonMTop, buttonMBottom, buttonMLeft, buttonMRight
        ));



        playerCircleSize = (int) (tableWidth * playerCircleSize_RATE);
        playerInnerCircleSize = (int) (tableWidth * playerInnerCircleSize_RATE);
        player_A_PositionX = (int) (tableWidth * PLAYER_A_INITIAL_RATE_X);
        player_A_PositionY = (int) (tableHeight * PLAYER_A_INITIAL_RATE_Y);
        player_B_PositionX = (int) (tableWidth * PLAYER_B_INITIAL_RATE_X);
        player_B_PositionY = (int) (tableHeight * PLAYER_B_INITIAL_RATE_Y);


        scoreA = scoreB = 0;

        fullZone = new Rect(0,0, tableWidth,tableHeight);
        gameZone = new Rect(
                gamePaddingLeft,
                gamePaddingTop,
                tableWidth - gamePaddingRight,
                tableHeight - gamePaddingBottom
        );
        doorZoneB = new Rect(
                doorSideLeft,
                0,
                doorSideRight,
                gamePaddingTop
        );
        doorZoneA = new Rect(
                doorSideLeft,
                tableHeight - gamePaddingBottom,
                doorSideRight,
                tableHeight
        );

        buttonLeft = new Rect(
                buttonLL,
                buttonTop,
                buttonLR,
                buttonBottom
        );
        buttonRight = new Rect(
                buttonRL,
                buttonTop,
                buttonRR,
                buttonBottom
        );
        buttonMiddle = new Rect(
                buttonMLeft,
                buttonMTop,
                buttonMRight,
                buttonMBottom
        );

    }



    /**
     * 触控事件监听
     */
    final private View.OnTouchListener listener = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            if(mGameState == ClientGameState.Running) {
                float x = event.getX();
                float y = event.getY();
                /**
                 * player A position
                 */
                if (x - gamePaddingLeft < playerCircleSize) {
                    player_A_PositionX = gamePaddingLeft + playerCircleSize;
                } else if (x + gamePaddingRight > tableWidth - playerCircleSize) {
                    player_A_PositionX = tableWidth - playerCircleSize - gamePaddingRight;
                } else {
                    player_A_PositionX = (int) x;
                }

                if (y < tableHeight * 0.5 + playerCircleSize) {
                    player_A_PositionY = (int) (tableHeight * 0.5 + playerCircleSize);
                } else if (y + gamePaddingBottom > tableHeight - playerCircleSize) {
                    player_A_PositionY = tableHeight - playerCircleSize - gamePaddingBottom;
                } else {
                    player_A_PositionY = (int) y;
                }


                return true;
            }

            else if(mGameState == ClientGameState.ChooseColor){

                int x = (int) event.getX();
                int y = (int) event.getY();

                if(choosingColorPlayer == playerID){


                    if((x > buttonMLeft) && (x < buttonMRight) &&
                            (y < buttonMBottom) && (y > buttonMTop)){

                        int send = GH_CC_W_CHOOSE_COMPLETE;
                        mGH_COMMAND_W_Characteristic.setValue(send,
                                BluetoothGattCharacteristic.FORMAT_UINT32,0);
                        mBluetoothLeService.writeCharacteristic(mGH_COMMAND_W_Characteristic);
                        Log.d(TAG, "Choose color complete!");

                    }

                }

            }

            return true;
        }
    };

    /**
     * 视图更新 内置类
     */
    class GameView extends View {
        Paint paint = new Paint();

        //构造函数
        public GameView(Context context) {
            super(context);
            setFocusable(true);
            paint.setStyle(Paint.Style.FILL);
            paint.setAntiAlias(true);
        }

        //画游戏帧
        @Override
        protected void onDraw(Canvas canvas) {
            super.onDraw(canvas);

            //边框和背景
            canvas.drawColor(Color.BLACK);
            paint.setColor(Color.rgb(0, 48, 210));
            canvas.drawRect(fullZone, paint);
            paint.setColor(Color.BLACK);
            canvas.drawRect(gameZone,paint);
            canvas.drawRect(doorZoneB, paint);
            canvas.drawRect(doorZoneA, paint);

            //内部填充
            if (mGameState == ClientGameState.Over) {
                // 游戏结束
                paint.setColor(Color.WHITE);
                paint.setTextSize((int)(tableHeight * 0.07));
                canvas.drawText(
                        "Game over.",
                        (int)(tableWidth * 0.25),
                        (int)(tableHeight * 0.4),
                        paint
                );
                paint.setTextSize((int)(tableHeight * 0.2));
                canvas.drawText(
                        (scoreA) + " -- " + (scoreB),
                        (int)(tableWidth * 0.18),
                        (int)(tableHeight * 0.6),
                        paint
                );

                paint.setColor(Color.WHITE);
                canvas.drawRect(buttonLeft, paint);
                canvas.drawRect(buttonRight, paint);

            }
            else if(mGameState == ClientGameState.Connecting) {
                // 正在连接
                paint.setColor(Color.WHITE);
                paint.setTextSize((int)(tableHeight * 0.04));
                canvas.drawText(
                        "Connecting...",
                        (int)(tableWidth * 0.3),
                        (int)(tableHeight * 0.5),
                        paint
                );
            }
            else if(mGameState == ClientGameState.Initializing){
                // 正在初始化
                paint.setColor(Color.WHITE);
                paint.setTextSize((int)(tableHeight * 0.04));
                canvas.drawText(
                        "Initializing...",
                        (int)(tableWidth * 0.3),
                        (int)(tableHeight * 0.5),
                        paint
                );
            }
            else if(mGameState == ClientGameState.Waiting) {
                // 等待对方连接
                paint.setColor(Color.WHITE);
                paint.setTextSize((int)(tableHeight * 0.04));
                canvas.drawText(
                        "Waiting...",
                        (int)(tableWidth * 0.3),
                        (int)(tableHeight * 0.5),
                        paint
                );
            }
            else if(mGameState == ClientGameState.ChooseColor){
                // 选择颜色

                paint.setColor(Color.WHITE);
                paint.setTextSize((int)(tableHeight * 0.04));
                String text = "";
                if(playerID == choosingColorPlayer){
                    text = "You are choosing color.";
                    canvas.drawText(
                            text,
                            (int)(tableWidth * 0.2),
                            (int)(tableHeight * 0.5),
                            paint
                    );

                    canvas.drawRect(buttonMiddle, paint);

                    paint.setColor(Color.BLACK);
                    paint.setTextSize((int)(tableHeight * 0.07));
                    canvas.drawText(
                            "OK",
                            (int)(tableWidth * (BUTTON_M_LEFT_RATE + 0.05)),
                            (int)(tableHeight * (BUTTON_M_BOTTOM_RATE - 0.02)),
                            paint
                    );

                }
                else {
                    text = "Your rival is choosing color.";
                    canvas.drawText(
                            text,
                            (int)(tableWidth * 0.1),
                            (int)(tableHeight * 0.5),
                            paint
                    );
                }


                //画玩家A
                paint.setColor(player_A_Color);
                canvas.drawCircle(
                        player_A_PositionX,
                        player_A_PositionY,
                        playerCircleSize,
                        paint
                );
                paint.setColor(Color.BLACK);
                canvas.drawCircle(
                        player_A_PositionX,
                        player_A_PositionY,
                        playerInnerCircleSize,
                        paint
                );

                //画玩家B
                paint.setColor(player_B_Color);
                canvas.drawCircle(
                        player_B_PositionX,
                        player_B_PositionY,
                        playerCircleSize,
                        paint
                );
                paint.setColor(Color.BLACK);
                canvas.drawCircle(
                        player_B_PositionX,
                        player_B_PositionY,
                        playerInnerCircleSize,
                        paint
                );

            }
            else if(mGameState == ClientGameState.Replay) {
                // 重玩
                paint.setColor(Color.WHITE);
                paint.setTextSize(40);
                paint.setTextSize((int)(tableHeight * 0.04));
                canvas.drawText(
                        "Waiting...",
                        (int)(tableWidth * 0.3),
                        (int)(tableHeight * 0.5),
                        paint
                );
            }
            else if(mGameState == ClientGameState.Running) {
                //游戏进行中

                // 画比分
                paint.setColor(Color.rgb(100, 100, 100));
                paint.setTextSize((int)(tableHeight * 0.3));

                canvas.drawText(
                        String.valueOf(scoreA),
                        (int)(tableWidth * 0.38),
                        (int)(tableHeight * 0.9),
                        paint
                );

                canvas.drawText(
                        String.valueOf(scoreB),
                        (int)(tableWidth * 0.38),
                        (int)(tableHeight * 0.3),
                        paint
                );

                //画球
                paint.setColor(Color.rgb(240,240,0));
                canvas.drawCircle(ballPositionX, ballPositionY, ballSize, paint);

                //画玩家A
                paint.setColor(player_A_Color);
                canvas.drawCircle(
                        player_A_PositionX,
                        player_A_PositionY,
                        playerCircleSize,
                        paint
                );
                paint.setColor(Color.BLACK);
                canvas.drawCircle(
                        player_A_PositionX,
                        player_A_PositionY,
                        playerInnerCircleSize,
                        paint
                );

                //画玩家B
                paint.setColor(player_B_Color);
                canvas.drawCircle(
                        player_B_PositionX,
                        player_B_PositionY,
                        playerCircleSize,
                        paint
                );
                paint.setColor(Color.BLACK);
                canvas.drawCircle(
                        player_B_PositionX,
                        player_B_PositionY,
                        playerInnerCircleSize,
                        paint
                );

            }
        }
    }


    /******************************* Other Components *******************************/


    /**
     * mHandler，调动mGameView刷新
     */
    final class myHandler extends Handler {
        private WeakReference<GameActivity> mMainActivityWeakReference;

        public myHandler(GameActivity mainActivity) {
            mMainActivityWeakReference = new WeakReference<>(mainActivity);
        }

        @Override
        public void handleMessage(Message msg) {
            if (msg.what == MSG_WHAT) {
                mGameView.invalidate();
            }
            super.handleMessage(msg);
        }
    }


    /**
     * 定时任务，发送位置
     */
    private TimerTask getNewTimerTask(){
        return new TimerTask() {

            @Override
            public void run() {
                if(true) {

                    /**
                     * send to server
                     */
                    if(mGH_Player_Position_W_Characteristic != null
                        && mGameState == ClientGameState.Running){

                        int sendx = (int)(player_A_PositionX * STD_SCREEN_WIDTH / tableWidth);
                        int sendy = (int)(player_A_PositionY * STD_SCREEN_HEIGHT / tableHeight);
                        int position = 0;
                        position |= sendx;
                        position |= (sendy << 12);
                        position |= (playerID << 24);

                        mGH_Player_Position_W_Characteristic.setValue(position,
                                BluetoothGattCharacteristic.FORMAT_UINT32,0);
                        mBluetoothLeService.writeCharacteristic(mGH_Player_Position_W_Characteristic);
                    }

                    mHandler.sendEmptyMessage(MSG_WHAT);        //刷新视图
                }
            }
        };
    }


    /**
     * 异常处理
     */

    @Override
    protected void onStop(){
        mTimerTask.cancel();
        mTimer.cancel();
        Log.d(TAG, "GamePage Stop");
        super.onStop();

    }

    @Override
    protected void onRestart(){
        Log.d(TAG, "GamePage Restart");
        mTimer = new Timer();
        mTimerTask = getNewTimerTask();
        mTimer.schedule(mTimerTask, 0, TIMER_PERIOD);
        super.onRestart();
    }


    @Override
    protected void onPause(){
        Log.d(TAG, "GamePage Pause");
        super.onPause();
        unregisterReceiver(mGattUpdateReceiver);
    }

    @Override
    protected void onDestroy(){
        Log.d(TAG, "GamePage Destroy");
        super.onDestroy();
        unbindService(mServiceConnection);
        mBluetoothLeService = null;
    }


}