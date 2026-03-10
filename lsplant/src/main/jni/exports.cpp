#include <jni.h>
#include <lsplant.hpp>
#include <string>
#include <string_view>

using LspInlineHooker = void *(*)(void *target, void *hooker);
using LspInlineUnhooker = bool (*)(void *func);
using LspArtSymbolResolver = void *(*)(const char *symbol_name);
using LspArtSymbolPrefixResolver = void *(*)(const char *symbol_prefix);

extern "C" {

// 初始化
__attribute__((visibility("default")))
bool lsp_native_init(JNIEnv *env,
                     LspInlineHooker inline_hooker,
                     LspInlineUnhooker inline_unhooker,
                     LspArtSymbolResolver art_symbol_resolver,
                     LspArtSymbolPrefixResolver art_symbol_prefix_resolver) {
    lsplant::InitInfo info{
        .inline_hooker = [inline_hooker](void *target, void *hooker) -> void * {
            return inline_hooker ? inline_hooker(target, hooker) : nullptr;
        },
        .inline_unhooker = [inline_unhooker](void *func) -> bool {
            return inline_unhooker ? inline_unhooker(func) : false;
        },
        .art_symbol_resolver = [art_symbol_resolver](std::string_view symbol) -> void * {
            if (!art_symbol_resolver) return nullptr;
            std::string s(symbol);
            return art_symbol_resolver(s.c_str());
        },
        .art_symbol_prefix_resolver = [art_symbol_prefix_resolver](std::string_view symbol) -> void * {
            if (!art_symbol_prefix_resolver) return nullptr;
            std::string s(symbol);
            return art_symbol_prefix_resolver(s.c_str());
        }
    };
    return lsplant::Init(env, info);
}

// hook
__attribute__((visibility("default")))
jobject lsp_native_hook(JNIEnv *env,
                        jobject target_method,
                        jobject hooker_object,
                        jobject callback_method) {
    return lsplant::Hook(env, target_method, hooker_object, callback_method);
}

// unhook
__attribute__((visibility("default")))
bool lsp_native_unhook(JNIEnv *env, jobject target_method) {
    return lsplant::UnHook(env, target_method);
}

// is hooked
__attribute__((visibility("default")))
bool lsp_native_is_hooked(JNIEnv *env, jobject method) {
    return lsplant::IsHooked(env, method);
}

// deoptimize
__attribute__((visibility("default")))
bool lsp_native_deoptimize(JNIEnv *env, jobject method) {
    return lsplant::Deoptimize(env, method);
}

// make class inheritable
__attribute__((visibility("default")))
bool lsp_native_make_class_inheritable(JNIEnv *env, jclass clazz) {
    return lsplant::MakeClassInheritable(env, clazz);
}

}
