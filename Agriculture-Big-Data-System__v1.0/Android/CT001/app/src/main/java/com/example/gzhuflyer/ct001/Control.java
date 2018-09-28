package com.example.gzhuflyer.ct001;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * Created by GzhuFlyer on 2018/3/1.
 */

public class Control extends Activity {

    //定义控制灯和接收信息的变量
    private Button led4Onbtn;
    private Button led4Offbtn;
    private Button led5Onbtn;
    private Button led5Offbtn;
    private TextView msgRcv;


    @Override
    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        setContentView(R.layout.control_second);
        //连接服务器
        connectF();
        //---------------------------------------------
        led4Onbtn = (Button) findViewById(R.id.Led4On);
        led4Offbtn = (Button) findViewById(R.id.Led4Off);
        led5Onbtn = (Button) findViewById(R.id.Led5On);
        led5Offbtn = (Button) findViewById(R.id.Led5Off);
        msgRcv = (TextView) findViewById(R.id.msg_list);
        //-----------------------------------------------
        //-----------------------------------------------
        //发送相应的指令信息，控制单片机灯的亮灭

        led4Onbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendF('z');
            }
        });
        led4Offbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendF('Z');
            }
        });
        led5Onbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendF('j');
            }
        });
        led5Offbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendF('J');
            }
        });

        //--------------------------------------------------------------------

    }

    //---------------------------------------------
    //新建一个socket对象
    Socket socket = null;
    int i = 0;
    public void connectF(){
        @SuppressLint("StaticFieldLeak")
        AsyncTask<Void,String,Void> read = new AsyncTask<Void, String, Void>() {
            //定义一个reader对象用于读取数据
            BufferedReader reader = null;
            @Override
            protected Void doInBackground(Void... voids) {
                try {
                    //传递要登陆的服务器的ip和地址
                    socket = new Socket("119.23.241.38", 1234);
                    //获取socket的读取输入流，将读取到的信息放在reader上面
                    reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                } catch (IOException e) {
                    e.printStackTrace();
                }
                publishProgress("显示接收到的数据\n");
                String line = null;
                try {
                    while((line = reader.readLine())!=null){
                        //将接收到的数据更新后的值传递到line,line再传递到onProgressUpdate函数
                        publishProgress(line);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }

                return null;
            }

            @Override
            protected void onProgressUpdate(String... values) {
                if(values[0].equals("显示接收到的数据\n"))
                    Toast.makeText(Control.this,"连接成功",Toast.LENGTH_LONG).show();
                //传递values的首地址，然后append会打印出整个字符串的值，遇到第一个换行符结束，函数，再继续执行，直到所有字符串打印完成
                msgRcv.append(values[0]+"\n");
                i++;
                Log.d("Control","the value of i is "+i);
                super.onProgressUpdate(values);
            }
        };
        read.execute();
        //Toast.makeText(MainActivity.this,"successed",Toast.LENGTH_LONG).show();
    }

    //-----------------------------------------------
//---------------------------------------------

    //向服务器写数据
    private void sendF(final char c) {
        //新建一个线程执行发送函数，7.0之后的安卓系统如果不新开一个线程，程序执行到这里会发生奔溃
        new Thread(){
            public void run(){
                BufferedWriter writer = null;
                try {
                    writer = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
                    //writer.write(editText.getText().toString()+"\n");
                    writer.write(c);
                    writer.flush();
                    // editText.setText("");
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }.start();


    }
    //------------------------------------------------


}
