#include <jni.h>
#include <string>
#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include "CPP_Object.h"


using namespace std;

int test();

jboolean done = (jboolean) false;
thread *blockjavathread = NULL;

extern "C"
JNIEXPORT jstring JNICALL
Java_com_made_hand_deadlock_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    string hello = "Hello from C++";
    test();
    return env->NewStringUTF(hello.c_str());
}


const int SIZE = 10;

mutex myMutex, myMutex1, myMutex2;

void shared_cout_thread_even(int i)
{
    lock_guard<mutex> g1(myMutex1);
    lock_guard<mutex> g2(myMutex2);
    cout << " " << i << " ";
}

void shared_cout_thread_odd(int i)
{
    lock_guard<mutex> g2(myMutex2);
    lock_guard<mutex> g1(myMutex1);
    cout << " " << i << " ";
}

void shared_cout_main(int i)
{
    lock_guard<mutex> g(myMutex);
    cout << " " << i << " ";
}

void f(int n)
{
    for(int i = SIZE*(n-1); i < SIZE * n ; i++) {
        if(n % 2 == 0)
            shared_cout_thread_even(i);
        else
            shared_cout_thread_odd(i);
    }
}

int test()
{
    thread t1(f, 1);  // 0-9
    thread t2(f, 2);  // 10-19
    thread t3(f, 3);  // 20-29
    thread t4(f, 4);  // 30-39
    thread t5(f, 5);  // 40-49


    for(int i = 0; i > -SIZE; i--)
        shared_cout_main(i);  // (0, -49)

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    return 0;
}

JNIEXPORT void JNICALL
Java_com_made_hand_deadlock_MainActivity_livelock(JNIEnv *env, jobject instance) {

    jclass myClass = env->FindClass("com.made.hand.deadlock.MainActivity");
    jmethodID mid = env->GetStaticMethodID(myClass, "livelock_test", "()v");
    env->CallStaticIntMethod(myClass, mid, NULL);
}

JNIEXPORT jboolean JNICALL
Java_com_made_hand_deadlock_MainActivity_canStart(JNIEnv *env, jobject instance) {
    jclass myClass = env->FindClass("com.made.hand.deadlock.MainActivity");
    jmethodID mid = env->GetStaticMethodID(myClass, "isActive", "()Z");
    jboolean res = (jboolean) env->CallStaticIntMethod(myClass, mid, NULL);
    if (blockjavathread == NULL)
    {
        blockjavathread = new thread([&]{
            while (!res)
            {
                this_thread::sleep_for(chrono::milliseconds(1000));
                res = (jboolean) env->CallStaticIntMethod(myClass, mid, NULL);
            }
        });
    }
    return done;
}

JNIEXPORT jlong JNICALL
Java_com_made_hand_deadlock_MainActivity_newObj(JNIEnv *env, jobject instance) {
    return (long) (new CPP_Object());
}

JNIEXPORT void JNICALL
Java_com_made_hand_deadlock_MainActivity_delObj(JNIEnv *env, jobject instance, jlong ptr) {
    delete (CPP_Object *)(ptr);
}

JNIEXPORT void JNICALL
Java_com_made_hand_deadlock_MainActivity_printObj(JNIEnv *env, jobject instance, jlong ptr) {

    CPP_Object *obj = (CPP_Object *)ptr;
    obj->print();
}