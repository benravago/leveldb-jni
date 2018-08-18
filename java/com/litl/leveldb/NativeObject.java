package com.litl.leveldb;

import java.io.Closeable;

abstract class NativeObject implements Closeable {

    protected long mPtr;
    private int mRefCount = 0;

    protected NativeObject() {
        // The Java wrapper counts as one reference,
        // will be released when closed
        ref();
    }

    protected NativeObject(long ptr) {
        this();

        if (ptr == 0) {
            throw new OutOfMemoryError("Failed to allocate native object");
        }

        mPtr = ptr;
    }

    synchronized protected long getPtr() {
        return mPtr;
    }

    protected void assertOpen(String message) {
        if (getPtr() == 0) {
            throw new IllegalStateException(message);
        }
    }

    synchronized final void ref() {
        mRefCount++;
    }

    synchronized final void unref() {
        if (mRefCount <= 0) {
            throw new IllegalStateException("Reference count is already 0");
        }

        mRefCount--;

        if (mRefCount == 0) {
            closeNativeObject(mPtr);
            mPtr = 0;
        }
    }

    protected abstract void closeNativeObject(long ptr);

    @Override
    public synchronized void close() {
        if (mPtr != 0) {
            unref();
        }
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (mPtr != 0) {
                Class<?> clazz = getClass();
                var name = clazz.getSimpleName();
                while (name.isEmpty()) {
                    clazz = clazz.getSuperclass();
                    name = clazz.getSimpleName();
                }

                System.out.println("NativeObject " + name + " refcount: " + mRefCount
                    + " id: " + System.identityHashCode(this)
                    + " was finalized before native resource was closed, did you forget to call close()?");
            }
        } finally {
            super.finalize();
        }
    }
}
