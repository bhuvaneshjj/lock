package com.made.hand.deadlock;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity
{

    public static Object Lock1 = new Object();
    public static Object Lock2 = new Object();

    boolean active = false;

    // Used to load the 'native-lib' library on application startup.
    static
    {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
        new Thread(new Runnable()
        {
            @Override
            public void run()
            {
                ThreadDemo1 T1 = new ThreadDemo1();
                ThreadDemo2 T2 = new ThreadDemo2();
                T1.start();
                T2.start();
            }
        }).start();

        new Thread(new Runnable()
        {
            @Override
            public void run()
            {
                while (canStart())
                {

                    try
                    {
                        Thread.sleep(10);
                    } catch (InterruptedException e)
                    {
                        e.printStackTrace();
                    }
                }
                active = true;
            }
        }).start();
        long ptr = newObj();
        delObj(ptr);
        printObj(ptr);
    }

    public void livelock_test()
    {
        livelock();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    public native void livelock();
    public native boolean canStart();
    public native long newObj();
    public native void delObj(long ptr);
    public native void printObj(long ptr);

    public boolean isActive()
    {
        return active;
    }

    private static class ThreadDemo1 extends Thread {
        public void run() {
            synchronized (Lock1) {
                System.out.println("Thread 1: Holding lock 1...");

                try { Thread.sleep(10); }
                catch (InterruptedException e) {}
                System.out.println("Thread 1: Waiting for lock 2...");

                synchronized (Lock2) {
                    System.out.println("Thread 1: Holding lock 1 & 2...");
                }
            }
        }
    }
    private static class ThreadDemo2 extends Thread {
        public void run() {
            synchronized (Lock2) {
                System.out.println("Thread 2: Holding lock 2...");

                try { Thread.sleep(10); }
                catch (InterruptedException e) {}
                System.out.println("Thread 2: Waiting for lock 1...");

                synchronized (Lock1) {
                    System.out.println("Thread 2: Holding lock 1 & 2...");
                }
            }
        }
    }
}
