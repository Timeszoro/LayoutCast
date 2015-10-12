package com.github.mmin18.layoutcast.util;

import android.annotation.SuppressLint;
import android.app.Application;
import android.os.Build;
import android.util.Log;

import java.io.File;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.util.Map;
import java.io.IOException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import dalvik.system.BaseDexClassLoader;
import dalvik.system.DexClassLoader;
import dalvik.system.DexFile;


/**
 * Created by mmin18 on 8/8/15.
 */
@SuppressLint("NewApi")
public class ArtUtils {
	private  static final  String TAG = "LayoutCast";
	private static Map<String, Class<?>> mFixedClass = new ConcurrentHashMap<String, Class<?>>();

	public static boolean overrideClassLoader(ClassLoader cl, File dex, File opt) {
		try {
			ClassLoader bootstrap = cl.getParent();
			Field fPathList = BaseDexClassLoader.class.getDeclaredField("pathList");
			fPathList.setAccessible(true);
			Object pathList = fPathList.get(cl);
			Class cDexPathList = bootstrap.loadClass("dalvik.system.DexPathList");
			Field fDexElements = cDexPathList.getDeclaredField("dexElements");
			fDexElements.setAccessible(true);
			Object dexElements = fDexElements.get(pathList);
			DexClassLoader cl2 = new DexClassLoader(dex.getAbsolutePath(), opt.getAbsolutePath(), null, bootstrap);
			Object pathList2 = fPathList.get(cl2);
			Object dexElements2 = fDexElements.get(pathList2);
			Object element2 = Array.get(dexElements2, 0);
			int n = Array.getLength(dexElements) + 1;
			Object newDexElements = Array.newInstance(fDexElements.getType().getComponentType(), n);
			Array.set(newDexElements, 0, element2);
			for (int i = 0; i < n - 1; i++) {
				Object element = Array.get(dexElements, i);
				Array.set(newDexElements, i + 1, element);
			}
			fDexElements.set(pathList, newDexElements);
			return true;
		} catch (Exception e) {
			Log.e("lcast", "fail to override classloader " + cl + " with " + dex, e);
			return false;
		}
	}
	
	public static boolean hookMehtods(Application app,File dex,File opt){
		NDKUtil.resolvDvm(Build.VERSION.SDK_INT);
		ClassLoader appClassLoader = app.getClassLoader();
		List<String> classNames = new ArrayList<String>();
		DexFile dx = null;
		File lcastDir = new File(app.getCacheDir(),"lcast");

		try {
			dx = DexFile.loadDex(dex.getAbsolutePath(), new File(opt.getAbsolutePath(),"patch.dex").getAbsolutePath(), 0);
		} catch (IOException e) {
			Log.d(TAG, "load dex file error");
			e.printStackTrace();
			return false;
		}
		DexClassLoader dexClassLoader = new DexClassLoader(dex.getAbsolutePath(), opt.getAbsolutePath(), null, appClassLoader);
		Enumeration<String> entrys = dx.entries();

		Class<?> dexCls = null;
		Log.d(TAG, "classes in dex file");
		while (entrys.hasMoreElements()) {
			String entry = entrys.nextElement();
			Log.d(TAG, entry);
			dexCls = dx.loadClass(entry, dexClassLoader);
			if (dexCls != null) {
				Method[] methods = dexCls.getDeclaredMethods();
				Log.d(TAG, "method in class " + dexCls.getName());
				for (Method method : methods) {
					Log.d(TAG,method.getName());
				}
					for (Method method : methods) {
					Log.d(TAG,method.getName());
					String clz = entry;
					String key = clz + "@" + appClassLoader.toString();
					Class<?> clazz = mFixedClass.get(key);
					if (clazz == null) {// class not load
						Log.d(TAG,"class not load");
						Class<?> clzztmp = null;
						try {
							clzztmp = appClassLoader.loadClass(clz);
							clazz = Class.forName(clzztmp.getName(), true, clzztmp.getClassLoader());
							Log.d(TAG,"load class" + clazz.getName());

						} catch (ClassNotFoundException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
							return false;
						}
					}
					if (clazz != null) {// initialize class OK
						mFixedClass.put(key, clazz);
						Method src = null;
						try {
							src = clazz.getDeclaredMethod(method.getName(),
									method.getParameterTypes());
						} catch (NoSuchMethodException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
							return false;
						}
						NDKUtil.hook(clazz, src, method);
					}
				}
			}
		}

		return true;
	}

}
