#pragma once

#include "../AstralPrtctn/json.hpp"
#include <jni.h>
#include <curl/curl.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/md5.h>
#include <iomanip>
#include <sstream>
#include <ctime>

using json = nlohmann::json;
extern JavaVM* jvm;

// Panel configuration
#define PANEL_GAME_ID "XLR8"

std::string g_Token, g_Auth;
bool bValid = false;
std::string EXP = " ";
std::string usedKey = " ";
std::string mod_status = " ";
std::string max_dev = " ";
std::string userType = "VVIP";
int keyUserCount = 0;
time_t expiryTimestamp = 0;

time_t parseExpiryDate(const std::string& dateStr) {
    struct tm tm = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d");

    tm.tm_hour = 23;
    tm.tm_min = 59;
    tm.tm_sec = 59;

    return mktime(&tm);
}

std::string getExpiryCountdown() {
    if (expiryTimestamp == 0) {
        return "Unknown";
    }

    time_t now = time(nullptr);
    if (now > expiryTimestamp) {
        return "EXPIRED";
    }

    time_t diff = expiryTimestamp - now;
    int days = diff / (24 * 3600);
    diff = diff % (24 * 3600);
    int hours = diff / 3600;
    diff = diff % 3600;
    int minutes = diff / 60;
    int seconds = diff % 60;

    std::stringstream ss;
    ss << days << "d " << hours << "h " << minutes << "m " << seconds << "s";
    return ss.str();
}
JNIEnv* AttachCurrentThread3(JavaVM* vm) {
    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        vm->AttachCurrentThread(&env, nullptr);
    }
    return env;
}

const char *GetAndroidID(JNIEnv *env, jobject context) {
    jclass contextClass = env->FindClass("android/content/Context");
    jmethodID getContentResolverMethod = env->GetMethodID(contextClass, "getContentResolver", "()Landroid/content/ContentResolver;");
    jclass settingSecureClass = env->FindClass("android/provider/Settings$Secure");
    jmethodID getStringMethod = env->GetStaticMethodID(settingSecureClass, "getString", "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;");

    auto obj = env->CallObjectMethod(context, getContentResolverMethod);
    auto str = (jstring) env->CallStaticObjectMethod(settingSecureClass, getStringMethod, obj, env->NewStringUTF("android_id"));
    return env->GetStringUTFChars(str, 0);
}

bool isVipKey(const std::string& key) {
    return key.substr(0, 4) == "VIP-";
}

const char *GetDeviceModel(JNIEnv *env) {
    jclass buildClass = env->FindClass("android/os/Build");
    jfieldID modelId = env->GetStaticFieldID(buildClass, "MODEL", "Ljava/lang/String;");

    auto str = (jstring) env->GetStaticObjectField(buildClass, modelId);
    return env->GetStringUTFChars(str, 0);
}

const char *GetDeviceBrand(JNIEnv *env) {
    jclass buildClass = env->FindClass("android/os/Build");
    jfieldID modelId = env->GetStaticFieldID(buildClass, "BRAND", "Ljava/lang/String;");

    auto str = (jstring) env->GetStaticObjectField(buildClass, modelId);
    return env->GetStringUTFChars(str, 0);
}

const char *GetDeviceUniqueIdentifier(JNIEnv *env, const char *uuid) {
    jclass uuidClass = env->FindClass("java/util/UUID");

    auto len = strlen(uuid);

    jbyteArray myJByteArray = env->NewByteArray(len);
    env->SetByteArrayRegion(myJByteArray, 0, len, (jbyte *) uuid);

    jmethodID nameUUIDFromBytesMethod = env->GetStaticMethodID(uuidClass, "nameUUIDFromBytes", "([B)Ljava/util/UUID;");
    jmethodID toStringMethod = env->GetMethodID(uuidClass, "toString", "()Ljava/lang/String;");

    auto obj = env->CallStaticObjectMethod(uuidClass, nameUUIDFromBytesMethod, myJByteArray);
    auto str = (jstring) env->CallObjectMethod(obj, toStringMethod);
    return env->GetStringUTFChars(str, 0);
}

std::string CalcMD5(std::string s) {
    std::string result;
    unsigned char hash[MD5_DIGEST_LENGTH];
    char tmp[4];
    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, s.c_str(), s.length());
    MD5_Final(hash, &md5);
    for (unsigned char i : hash) {
        sprintf(tmp, "%02x", i);
        result += tmp;
    }
    return result;
}

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *) userp;

    mem->memory = (char *) realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        return 0;
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

int ShowSoftKeyboardInput() {
    jint result;
    jint flags = 0;

    JNIEnv *env;
    jvm->AttachCurrentThread(&env, NULL);

    jclass looperClass = env->FindClass("android/os/Looper");
    auto prepareMethod = env->GetStaticMethodID(looperClass, "prepare", "()V");
    env->CallStaticVoidMethod(looperClass, prepareMethod);

    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    jfieldID sCurrentActivityThreadField = env->GetStaticFieldID(activityThreadClass, "sCurrentActivityThread", "Landroid/app/ActivityThread;");
    jobject sCurrentActivityThread = env->GetStaticObjectField(activityThreadClass, sCurrentActivityThreadField);

    jfieldID mInitialApplicationField = env->GetFieldID(activityThreadClass, "mInitialApplication", "Landroid/app/Application;");
    jobject mInitialApplication = env->GetObjectField(sCurrentActivityThread, mInitialApplicationField);

    jclass contextClass = env->FindClass("android/content/Context");
    jfieldID fieldINPUT_METHOD_SERVICE = env->GetStaticFieldID(contextClass, "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
    jobject INPUT_METHOD_SERVICE = env->GetStaticObjectField(contextClass, fieldINPUT_METHOD_SERVICE);
    jmethodID getSystemServiceMethod = env->GetMethodID(contextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jobject callObjectMethod = env->CallObjectMethod(mInitialApplication, getSystemServiceMethod, INPUT_METHOD_SERVICE);

    jclass classInputMethodManager = env->FindClass("android/view/inputmethod/InputMethodManager");
    jmethodID toggleSoftInputId = env->GetMethodID(classInputMethodManager, "toggleSoftInput", "(II)V");

    if (result) {
        env->CallVoidMethod(callObjectMethod, toggleSoftInputId, 2, flags);
    } else {
        env->CallVoidMethod(callObjectMethod, toggleSoftInputId, flags, flags);
    }

    env->DeleteLocalRef(classInputMethodManager);
    env->DeleteLocalRef(callObjectMethod);
    env->DeleteLocalRef(contextClass);
    env->DeleteLocalRef(mInitialApplication);
    env->DeleteLocalRef(activityThreadClass);
    jvm->DetachCurrentThread();

    return result;
}

int PollUnicodeChars() {
    JNIEnv *env;
    jvm->AttachCurrentThread(&env, NULL);

    jclass looperClass = env->FindClass("android/os/Looper");
    auto prepareMethod = env->GetStaticMethodID(looperClass, "prepare", "()V");
    env->CallStaticVoidMethod(looperClass, prepareMethod);

    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    jfieldID sCurrentActivityThreadField = env->GetStaticFieldID(activityThreadClass, "sCurrentActivityThread", "Landroid/app/ActivityThread;");
    jobject sCurrentActivityThread = env->GetStaticObjectField(activityThreadClass, sCurrentActivityThreadField);

    jfieldID mInitialApplicationField = env->GetFieldID(activityThreadClass, "mInitialApplication", "Landroid/app/Application;");
    jobject mInitialApplication = env->GetObjectField(sCurrentActivityThread, mInitialApplicationField);

    jclass keyEventClass = env->FindClass("android/view/KeyEvent");
    jmethodID getUnicodeCharMethod = env->GetMethodID(keyEventClass, "getUnicodeChar", "(I)I");

    ImGuiIO& io = ImGui::GetIO();

    int return_key = env->CallIntMethod(keyEventClass, getUnicodeCharMethod);

    env->DeleteLocalRef(keyEventClass);
    env->DeleteLocalRef(mInitialApplication);
    env->DeleteLocalRef(activityThreadClass);
    jvm->DetachCurrentThread();

    return return_key;
}

std::string getClipboard() {
    std::string result;
    JNIEnv *env;

    if (jvm == nullptr) {
        return result;
    }

    jvm->AttachCurrentThread(&env, NULL);

    auto looperClass = env->FindClass("android/os/Looper");
    auto prepareMethod = env->GetStaticMethodID(looperClass, "prepare", "()V");
    env->CallStaticVoidMethod(looperClass, prepareMethod);

    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    jfieldID sCurrentActivityThreadField = env->GetStaticFieldID(activityThreadClass, "sCurrentActivityThread", "Landroid/app/ActivityThread;");
    jobject sCurrentActivityThread = env->GetStaticObjectField(activityThreadClass, sCurrentActivityThreadField);

    jfieldID mInitialApplicationField = env->GetFieldID(activityThreadClass, "mInitialApplication", "Landroid/app/Application;");
    jobject mInitialApplication = env->GetObjectField(sCurrentActivityThread, mInitialApplicationField);

    auto contextClass = env->FindClass("android/content/Context");
    auto getSystemServiceMethod = env->GetMethodID(contextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

    auto str = env->NewStringUTF("clipboard");
    auto clipboardManager = env->CallObjectMethod(mInitialApplication, getSystemServiceMethod, str);
    env->DeleteLocalRef(str);

    jclass ClipboardManagerClass = env->FindClass("android/content/ClipboardManager");
    auto getText = env->GetMethodID(ClipboardManagerClass, "getText", "()Ljava/lang/CharSequence;");

    jclass CharSequenceClass = env->FindClass("java/lang/CharSequence");
    auto toStringMethod = env->GetMethodID(CharSequenceClass, "toString", "()Ljava/lang/String;");

    auto text = env->CallObjectMethod(clipboardManager, getText);
    if (text) {
        str = (jstring) env->CallObjectMethod(text, toStringMethod);
        result = env->GetStringUTFChars(str, 0);
        env->DeleteLocalRef(str);
        env->DeleteLocalRef(text);
    }
    env->DeleteLocalRef(CharSequenceClass);
    env->DeleteLocalRef(ClipboardManagerClass);
    env->DeleteLocalRef(clipboardManager);
    env->DeleteLocalRef(contextClass);
    env->DeleteLocalRef(mInitialApplication);
    env->DeleteLocalRef(activityThreadClass);
    jvm->DetachCurrentThread();
    return result;
}

std::string Login(const char *user_key) {
    bValid = false;

    JNIEnv *env;
    jvm->AttachCurrentThread(&env, 0);

    auto looperClass = env->FindClass("android/os/Looper");
    auto prepareMethod = env->GetStaticMethodID(looperClass, "prepare", "()V");
    env->CallStaticVoidMethod(looperClass, prepareMethod);

    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    jfieldID sCurrentActivityThreadField =
        env->GetStaticFieldID(activityThreadClass, "sCurrentActivityThread",
                              "Landroid/app/ActivityThread;");
    jobject sCurrentActivityThread =
        env->GetStaticObjectField(activityThreadClass, sCurrentActivityThreadField);

    jfieldID mInitialApplicationField =
        env->GetFieldID(activityThreadClass, "mInitialApplication",
                        "Landroid/app/Application;");
    jobject mInitialApplication =
        env->GetObjectField(sCurrentActivityThread, mInitialApplicationField);

    std::string hwid = user_key;
    hwid += GetAndroidID(env, mInitialApplication);
    hwid += GetDeviceModel(env);
    hwid += GetDeviceBrand(env);

    std::string UUID = GetDeviceUniqueIdentifier(env, hwid.c_str());

    jvm->DetachCurrentThread();

    std::string errMsg = "Unknown error";
    usedKey = user_key;

    userType = isVipKey(user_key) ? "Premium" : "VVIP";

    struct MemoryStruct chunk{};
    chunk.memory = (char *) malloc(1);
    chunk.size = 0;

    CURL *curl = curl_easy_init();

    if (!curl) {
        free(chunk.memory);
        return "❌ Failed to initialize network";
    }

    std::string api_url = oxorany("https://xlreyt.x10.mx/connect");

    curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    char data[4096];
    sprintf(data, "game=%s&user_key=%s&serial=%s", PANEL_GAME_ID, user_key, UUID.c_str());

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        try {
            if (!chunk.memory || chunk.size == 0) {
                errMsg = "Empty server response";
            } else {
                std::string raw(chunk.memory, chunk.size);
                json result = json::parse(raw);

                if (result.is_object() && result.contains("status")) {
                    if (result["status"] == true) {
                        if (result.is_object() && result.contains("data")) {
                            std::string token = result["data"].value("token", "");
                            time_t rng = result["data"].value("rng", (time_t)0);
                            EXP = result["data"].value("EXP", "");

                            expiryTimestamp = parseExpiryDate(EXP);

                            if (rng + 30 > time(0)) {
                                g_Token = token;
                                bValid = true;
                                errMsg = "OK";
                            } else {
                                errMsg = "Session expired. Please retry.";
                            }
                        } else {
                            errMsg = "Unexpected server response shape";
                        }
                    } else {
                        std::string reason = result.value("reason", "Unknown error");

                        if (reason == "Invalid key")
                            errMsg = "❌ Invalid Key";
                        else if (reason == "Key Expired")
                            errMsg = "⌛ Key Expired";
                        else if (reason == "Slot limit reached")
                            errMsg = "⚠️ Device Limit Reached";
                        else if (reason == "Key Banned")
                            errMsg = "🚫 Key Banned";
                        else if (reason == "Missing key or HWID")
                            errMsg = "⚠️ Missing Key or Device ID";
                        else if (reason == "Server under maintenance")
                            errMsg = "🛠 Server Maintenance";
                        else
                            errMsg = "❌ " + reason;
                    }
                } else {
                    errMsg = "Unexpected server response: " + raw;
                }
            }
        } catch (const json::parse_error &e) {
            std::string raw(chunk.memory ? chunk.memory : "");
            errMsg = "Parse error: " + raw;
        } catch (...) {
            if (chunk.memory && chunk.size > 0) {
                errMsg = "⚠️ Server response error: " + std::string(chunk.memory);
            } else {
                errMsg = "⚠️ Server response error";
            }
        }

    } else {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code == 0L) {
            errMsg = "🌐 Network error. Check your connection.";
        } else {
            errMsg = "Server error " + std::to_string(http_code);
        }
    }

    curl_easy_cleanup(curl);
    free(chunk.memory);

    if (bValid)
        return "OK";

    if (errMsg.empty())
        return "Login failed. Please try again.";

    return errMsg;
}
