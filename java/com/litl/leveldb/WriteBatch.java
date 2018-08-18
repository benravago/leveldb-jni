package com.litl.leveldb;

public class WriteBatch extends NativeObject {

    public WriteBatch() {
        super(nativeCreate());
    }

    @Override
    protected void closeNativeObject(long ptr) {
        nativeDestroy(ptr);
    }

    public void delete(byte[] key) {
        assertOpen("WriteBatch is closed");
        if (key == null) {
            throw new NullPointerException("key");
        }

        nativeDelete(mPtr, key);
    }

    public void put(byte[] key, byte[] value) {
        assertOpen("WriteBatch is closed");
        if (key == null) {
            throw new NullPointerException("key");
        }
        if (value == null) {
            throw new NullPointerException("value");
        }

        nativePut(mPtr, key, value);
    }

    public void clear() {
        assertOpen("WriteBatch is closed");
        nativeClear(mPtr);
    }

    private static native long nativeCreate();

    private static native void nativeDestroy(long ptr);

    private static native void nativeDelete(long ptr, byte[] key);

    private static native void nativePut(long ptr, byte[] key, byte[] val);

    private static native void nativeClear(long ptr);
}
