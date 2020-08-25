package com.example.ghtest;

import android.content.Intent;
import android.os.Bundle;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.util.Log;
import android.view.View;
import android.widget.Button;

import java.util.Timer;

public class FirstPageActivity extends AppCompatActivity {

    final private static String TAG = "App";

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        Log.d(TAG, "FirstPage Create");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_first_page);


        Button btn1 = findViewById(R.id.button1);
        //给btn1绑定监听事件
        btn1.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                // 给bnt1添加点击响应事件
                Intent intent = new Intent(
                        FirstPageActivity.this,
                        FullscreenActivity.class);
                //启动
                startActivity(intent);
            }
        });


        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
    }

    @Override
    protected void onStop(){
        Log.d(TAG, "FirstPage Stop");
        super.onStop();

    }

    @Override
    protected void onRestart(){
        Log.d(TAG, "FirstPage Restart");
        super.onRestart();
    }

    @Override
    protected void onDestroy(){
        Log.d(TAG, "FirstPage Destroy");
        super.onDestroy();
    }

    @Override
    protected void onPause(){
        Log.d(TAG, "FirstPage Pause");
        super.onPause();
    }

    @Override
    protected void onResume(){
        Log.d(TAG, "FirstPage Resume");
        super.onResume();
    }

}
