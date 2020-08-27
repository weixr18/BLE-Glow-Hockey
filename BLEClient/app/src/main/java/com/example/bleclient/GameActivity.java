package com.example.bleclient;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import androidx.appcompat.app.AppCompatActivity;

//import java.io.PrintStream;
//import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;
import java.lang.ref.WeakReference;

public class GameActivity extends AppCompatActivity {

    // constants
    final private static String TAG = "App";
    final private static int MSG_WHAT = 170411;

    // canvas const parameters

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
    final private static double PLAYER_A_INITIAL_RATE_X = 0.5;
    final private static double PLAYER_A_INITIAL_RATE_Y = 0.8;

    // bounce effect const parameters
    final private static int BALL_MASS = 20;
    final private static int PLAYER_CIRCLE_MASS = 20;
    final private static double BALL_SPEED_MAX_RATE = 1.3;
    final private static double BALL_SPEED_MIN_RATE = 0.25;
    final private static double BOUNCE_DECAY_RATE = 0.8;
    final private static double BOUNCE_ACCELERATE_RATE = 0.4;

    final private static int TIMER_PERIOD = 16;

    // game states
    private boolean isOver = false;
    private boolean isStart = false;
    private int scoreA = 0;
    private int scoreB = 0;

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

    private Rect fullZone;
    private Rect gameZone;      // 游戏区域
    private Rect doorZoneB;     //对方门
    private Rect doorZoneA;     //己方门


    // player circle parameters
    private int playerCircleAPositionX;
    private int playerCircleAPositionY;
    private int playerCircleAPositionLastX;
    private int playerCircleAPositionLastY;
    private int playerCircleASpeedX;
    private int playerCircleASpeedY;

    // Ball parameters
    private int ballPositionX;
    private int ballPositionY;
    private int ballSpeedY;
    private int ballSpeedX;

    private Timer timer = new Timer();
    private myHandler handler = null;
    private GameView gameView = null;



    // 初始化函数
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.d(TAG, "GamePage Create");

        // ???
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        //组件设置
        gameView = new GameView(this);          //游戏视图实例化
        setContentView(gameView);                       //设置Activity的视图为游戏视图
        handler = new myHandler(this);                 //handler实例化
        gameView.setOnTouchListener(listener);          //设置触摸事件监听器
        

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
        ballSpeedX = 0;
        ballSpeedY = 0;

        playerCircleSize = (int) (tableWidth * playerCircleSize_RATE);
        playerInnerCircleSize = (int) (tableWidth * playerInnerCircleSize_RATE);
        playerCircleAPositionX = (int) (tableWidth * PLAYER_A_INITIAL_RATE_X);
        playerCircleAPositionY = (int) (tableHeight * PLAYER_A_INITIAL_RATE_Y);
        playerCircleASpeedX = 0;
        playerCircleASpeedY = 0;

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

        //定时任务开始
        timer.schedule(timerTask , 0, TIMER_PERIOD);

    }

    @Override
    protected void onStop(){
        timerTask.cancel();
        timer.cancel();
        Log.d(TAG, "GamePage Stop");
        super.onStop();

    }

    @Override
    protected void onRestart(){
        Log.d(TAG, "GamePage Restart");
        timer = new Timer();
        timerTask = getNewTimerTask();
        timer.schedule(timerTask, 0, TIMER_PERIOD);
        super.onRestart();
    }

    @Override
    protected void onDestroy(){
        Log.d(TAG, "GamePage Destroy");
        super.onDestroy();
    }

    @Override
    protected void onPause(){
        Log.d(TAG, "GamePage Pause");
        super.onPause();
    }

    @Override
    protected void onResume(){
        Log.d(TAG, "GamePage Resume");
        super.onResume();
    }

    //定时器handler，调动gameView刷新
    final class myHandler extends Handler {
        private WeakReference<GameActivity> mMainActivityWeakReference;

        public myHandler(GameActivity mainActivity) {
            mMainActivityWeakReference = new WeakReference<>(mainActivity);
        }

        @Override
        public void handleMessage(Message msg) {
            if (msg.what == MSG_WHAT) {
                gameView.invalidate();
            }
            super.handleMessage(msg);
        }
    }

    //GameView内置视图类
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
            if (isOver) {
                //游戏结束
                paint.setColor(Color.RED);
                paint.setTextSize(40);
                canvas.drawText("Game Over!", 50, 200, paint);
            } else {
                //游戏进行中
                //画球
                paint.setColor(Color.rgb(240,240,0));
                canvas.drawCircle(ballPositionX, ballPositionY, ballSize, paint);

                //画玩家
                paint.setColor(Color.rgb(124,249,102));
                canvas.drawCircle(
                        playerCircleAPositionX,
                        playerCircleAPositionY,
                        playerCircleSize,
                        paint
                );
                paint.setColor(Color.BLACK);
                canvas.drawCircle(
                        playerCircleAPositionX,
                        playerCircleAPositionY,
                        playerInnerCircleSize,
                        paint
                );

            }
        }
    }

    //定时任务

    private TimerTask getNewTimerTask(){
        return new TimerTask() {

            @Override
            public void run() {
                if(!isOver) {

                    // 小球边框碰撞事件
                    if (ballPositionX - gamePaddingLeft <= ballSize) {
                        ballSpeedX = Math.abs(ballSpeedX);
                    } else if (ballPositionX + gamePaddingRight >= tableWidth - ballSize) {
                        ballSpeedX = -Math.abs(ballSpeedX);
                    }

                    if (ballPositionY - gamePaddingTop <= ballSize) {
                        // 己方得分
                        if (ballPositionX > doorSideLeft && ballPositionX < doorSideRight) {
                            scoreA += 1;
                            Log.d(TAG, String.format("You Scored! current score : %d -- %d", scoreA, scoreB));
                            if (scoreA >= 7) {
                                isOver = true;
                            } else {
                                isStart = false;
                                ballPositionX = (int) (tableWidth * BALL_INITIAL_RATE_X);
                                ballPositionY = (int) (tableHeight * BALL_INITIAL_RATE_Y);
                                ballSpeedX = 0;
                                ballSpeedY = 0;
                                playerCircleAPositionX = (int) (tableWidth * PLAYER_A_INITIAL_RATE_X);
                                playerCircleAPositionY = (int) (tableHeight * PLAYER_A_INITIAL_RATE_Y);
                                return;
                            }
                        }
                        ballSpeedY = Math.abs(ballSpeedY);
                    } else if (ballPositionY + gamePaddingBottom >= tableHeight - ballSize) {

                        //对方得分
                        if (ballPositionX > doorSideLeft && ballPositionX < doorSideRight) {
                            scoreB += 1;
                            Log.d(TAG, String.format("Your rival Scored! current score : %d -- %d", scoreA, scoreB));
                            if (scoreB >= 7) {
                                isOver = true;
                            } else {
                                isStart = false;
                                ballPositionX = (int) (tableWidth * BALL_INITIAL_RATE_X);
                                ballPositionY = (int) (tableHeight * BALL_INITIAL_RATE_Y);
                                ballSpeedX = 0;
                                ballSpeedY = 0;
                                playerCircleAPositionX = (int) (tableWidth * PLAYER_A_INITIAL_RATE_X);
                                playerCircleAPositionY = (int) (tableHeight * PLAYER_A_INITIAL_RATE_Y);
                                return;
                            }
                        }
                        ballSpeedY = -Math.abs(ballSpeedY);
                    }


                    // 小球和玩家碰撞事件
                    double ball_playerA_dis = Math.sqrt(
                            Math.pow(ballPositionX - playerCircleAPositionX, 2)
                                    + Math.pow(ballPositionY - playerCircleAPositionY, 2)
                    );
                    if (ball_playerA_dis < ballSize + playerCircleSize) {

                        Log.d(TAG, "Bounce!!");
                        if (!isStart) {
                            isStart = true;
                        }

                        ballSpeedX += (int) (2 * PLAYER_CIRCLE_MASS * (playerCircleASpeedX - ballSpeedX)
                                / (PLAYER_CIRCLE_MASS + BALL_MASS) * BOUNCE_DECAY_RATE);
                        ballSpeedX += (int) (ballPositionX - playerCircleAPositionX) * BOUNCE_ACCELERATE_RATE *
                                (ballSize + playerCircleSize - ball_playerA_dis) / (ballSize + playerCircleSize);

                        ballSpeedY += (int) (2 * PLAYER_CIRCLE_MASS * (playerCircleASpeedY - ballSpeedY)
                                / (PLAYER_CIRCLE_MASS + BALL_MASS) * BOUNCE_DECAY_RATE);
                        ballSpeedY += (int) (ballPositionY - playerCircleAPositionY) * BOUNCE_ACCELERATE_RATE *
                                (ballSize + playerCircleSize - ball_playerA_dis) / (ballSize + playerCircleSize);
                    }

                    // 防止speed过大
                    if (ballSpeedX > ballSize * BALL_SPEED_MAX_RATE) {
                        ballSpeedX = (int) (ballSize * BALL_SPEED_MAX_RATE);
                    } else if (ballSpeedX < -ballSize * BALL_SPEED_MAX_RATE) {
                        ballSpeedX = -(int) (ballSize * BALL_SPEED_MAX_RATE);
                    }

                    if (ballSpeedY > ballSize * BALL_SPEED_MAX_RATE) {
                        ballSpeedY = (int) (ballSize * BALL_SPEED_MAX_RATE);
                    } else if (ballSpeedY < -ballSize * BALL_SPEED_MAX_RATE) {
                        ballSpeedY = -(int) (ballSize * BALL_SPEED_MAX_RATE);
                    }

                    //防止speed过小
                    if (isStart) {
                        if (ballSpeedX >= 0 && ballSpeedX < ballSize * BALL_SPEED_MIN_RATE) {
                            ballSpeedX = (int) (ballSize * BALL_SPEED_MIN_RATE);
                        } else if (ballSpeedX < 0 && ballSpeedX > -ballSize * BALL_SPEED_MIN_RATE) {
                            ballSpeedX = -(int) (ballSize * BALL_SPEED_MIN_RATE);
                        }
                        if (ballSpeedY >= 0 && ballSpeedY < ballSize * BALL_SPEED_MIN_RATE) {
                            ballSpeedY = (int) (ballSize * BALL_SPEED_MIN_RATE);
                        } else if (ballSpeedY < 0 && ballSpeedY > -ballSize * BALL_SPEED_MIN_RATE) {
                            ballSpeedY = -(int) (ballSize * BALL_SPEED_MIN_RATE);
                        }
                    }

                    ballPositionY += ballSpeedY;
                    ballPositionX += ballSpeedX;


                    handler.sendEmptyMessage(MSG_WHAT);        //刷新视图
                }
            }
        };
    }

    private TimerTask timerTask = getNewTimerTask();

    // 触控事件监听
    final private View.OnTouchListener listener = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            if(!isOver) {
                float x = event.getX();
                float y = event.getY();

                /**
                 * player A position
                 */

                playerCircleAPositionLastX = playerCircleAPositionX;
                playerCircleAPositionLastY = playerCircleAPositionY;

                if (x - gamePaddingLeft < playerCircleSize) {
                    playerCircleAPositionX = gamePaddingLeft + playerCircleSize;
                } else if (x + gamePaddingRight > tableWidth - playerCircleSize) {
                    playerCircleAPositionX = tableWidth - playerCircleSize - gamePaddingRight;
                } else {
                    playerCircleAPositionX = (int) x;
                }

                if (y < tableHeight * 0.5 + playerCircleSize) {
                    playerCircleAPositionY = (int) (tableHeight * 0.5 + playerCircleSize);
                } else if (y + gamePaddingBottom > tableHeight - playerCircleSize) {
                    playerCircleAPositionY = tableHeight - playerCircleSize - gamePaddingBottom;
                } else {
                    playerCircleAPositionY = (int) y;
                }

                playerCircleASpeedX = playerCircleAPositionX - playerCircleAPositionLastX;
                playerCircleASpeedY = playerCircleAPositionY - playerCircleAPositionLastY;

                return true;
            }
            return true;
        }
    };

}