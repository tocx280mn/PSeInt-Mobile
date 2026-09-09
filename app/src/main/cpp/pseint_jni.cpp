#include <jni.h>
#include "mobile_runtime.h"

static std::string bytes(JNIEnv *env,jbyteArray value) {
    auto count=env->GetArrayLength(value);
    std::string result(count,'\0');
    if(count) env->GetByteArrayRegion(value,0,count,reinterpret_cast<jbyte*>(&result[0]));
    return result;
}
static jbyteArray bytes(JNIEnv *env,const std::string &value) {
    auto result=env->NewByteArray(value.size());
    if (!result) throw MobileCancelled();
    env->SetByteArrayRegion(result,0,value.size(),reinterpret_cast<const jbyte*>(value.data()));
    return result;
}
static void check(JNIEnv *env) { if (env->ExceptionCheck()) throw MobileCancelled(); }

extern "C" JNIEXPORT jint JNICALL Java_com_example_NativePSeInt_execute(
        JNIEnv *env,jobject,jbyteArray source,jbyteArray flags,jboolean run,jboolean debug,jobject receiver,jboolean testMode) {
    const auto type=env->GetObjectClass(receiver);
    const auto output=env->GetMethodID(type,"output","([B)V");
    const auto input=env->GetMethodID(type,"input","([B)[B");
    const auto diagnostic=env->GetMethodID(type,"diagnostic","(II[BZ)V");
    const auto step=env->GetMethodID(type,"step","(I[[B)V");
    const auto checkpoint=env->GetMethodID(type,"checkpoint","()V");
    if (env->ExceptionCheck()) return 3;
    auto payload=bytes(env,source), settings=bytes(env,flags);
    MobileCallbacks callbacks;
    callbacks.checkpoint=[&] { env->CallVoidMethod(receiver,checkpoint); check(env); };
    callbacks.output=[&](const std::string &text) {
        auto data=bytes(env,text); env->CallVoidMethod(receiver,output,data); env->DeleteLocalRef(data); check(env);
    };
    callbacks.input=[&](const std::string &name) {
        auto data=bytes(env,name);
        auto result=static_cast<jbyteArray>(env->CallObjectMethod(receiver,input,data));
        env->DeleteLocalRef(data); check(env);
        if (!result) throw MobileEndOfInput();
        auto value=bytes(env,result); env->DeleteLocalRef(result); return value;
    };
    callbacks.diagnostic=[&](int line,int code,const std::string &message,bool warning) {
        auto data=bytes(env,message);
        env->CallVoidMethod(receiver,diagnostic,line,code,data,static_cast<jboolean>(warning));
        env->DeleteLocalRef(data); check(env);
    };
    callbacks.step=[&](int line,const std::vector<std::pair<std::string,std::string>> &values) {
        auto byteType=env->FindClass("[B");
        auto data=env->NewObjectArray(values.size()*2,byteType,nullptr);
        env->DeleteLocalRef(byteType);
        if (!data) throw MobileCancelled();
        int index=0;
        for (const auto &pair:values) {
            for (const auto &text:{pair.first,pair.second}) {
                auto entry=bytes(env,text); env->SetObjectArrayElement(data,index++,entry); env->DeleteLocalRef(entry);
            }
        }
        env->CallVoidMethod(receiver,step,line,data); env->DeleteLocalRef(data); check(env);
    };
    return mobileRun(payload,settings,run,debug,callbacks,testMode);
}
