package com.example.gzhuflyer.ct001;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.NetworkOnMainThreadException;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.nio.channels.NetworkChannel;
import java.security.ProtectionDomain;

public class LoginMainActivity extends AppCompatActivity {

    private EditText accountEdit;
    private EditText passwordEdit;
    private Button login;

    private IntentFilter intentFilter;
    private NetworkChaneReceiver networkChaneReceiver;

    private int networkFlag = 0;    //网络判断标志位，0为没有连接到网络，1为已经连接到网络

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login_main);

        accountEdit = (EditText)findViewById(R.id.account);
        passwordEdit = (EditText)findViewById(R.id.password);

        //----------------监测网络状态
        intentFilter = new IntentFilter();
        intentFilter.addAction("android.net.conn.CONNECTIVITY_CHANGE");
        networkChaneReceiver = new NetworkChaneReceiver();
        registerReceiver(networkChaneReceiver,intentFilter);
        //-------------------------------------------------------


        //------------------------------------------------
        login = (Button) findViewById(R.id.login);
        login.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //获取用户输入的账号信息
                String acconut = accountEdit.getText().toString();
                //获取用户输入的密码信息
                String password = passwordEdit.getText().toString();
                //判断输入的账号密码是否正确，以及是否连接到网络，账号密码正确并且连接到网络的话进入下一个activity
                if(acconut.equals("admin") && password.equals("123456") && networkFlag == 1) {
                    Intent intent = new Intent(LoginMainActivity.this, Control.class);
                    startActivity(intent);
                }else if(networkFlag == 0){
                    Toast.makeText(LoginMainActivity.this,"未连接到网络不能登陆",Toast.LENGTH_SHORT).show();
                }else {
                    Toast.makeText(LoginMainActivity.this,"账号或者密码输入错误",Toast.LENGTH_SHORT).show();
                }
            }
        });
        //---------------------------------------------

    }

    @Override
    protected void onDestroy(){
        super.onDestroy();
        unregisterReceiver(networkChaneReceiver);
    }

    //获取网络状态，即是否连接到网络
    class NetworkChaneReceiver  extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            ConnectivityManager connectivityManager = (ConnectivityManager)
                    getSystemService(Context.CONNECTIVITY_SERVICE);
            NetworkInfo networkInfo = connectivityManager.getActiveNetworkInfo();
            if(networkInfo != null && networkInfo.isAvailable()){
                Toast.makeText(context,"已连接接到网络",Toast.LENGTH_SHORT).show();
                networkFlag = 1;
            }else{
                Toast.makeText(context,"未连接到网路",Toast.LENGTH_SHORT).show();
                networkFlag = 0;
            }
        }
    }
}
