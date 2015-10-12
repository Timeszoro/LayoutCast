package com.github.mmin18.layoutcast.util;

import java.lang.reflect.Method;

public class NDKUtil {
	public static native void hook(Class<?> clazzToHook, Method src, Method dest);

    public native static void customClassLoader(ClassLoader loader,String dexPath,String optPath);

    public native static void resolvDvm(int apilevel);

    static{
        System.loadLibrary("layout");
    }
}
