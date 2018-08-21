package com.litl.leveldb;

import java.nio.file.Path;

public class DB extends NativeObject {

    private final Path mPath;
    private boolean mDestroyOnClose = false;

    public DB(Path path) {
        super();

        if (path == null) {
            throw new NullPointerException();
        }
        mPath = path;
    }

    public void open() {
        mPtr = nativeOpen(mPath.toAbsolutePath().toString());
    }

    @Override
    protected void closeNativeObject(long ptr) {
        nativeClose(ptr);

        if (mDestroyOnClose) {
            destroy(mPath);
        }
    }

    public void destroy() {
        mDestroyOnClose = true;
        if (getPtr() == 0) {
            destroy(mPath);
        }
    }

    public static void destroy(Path path) {
        nativeDestroy(path.toAbsolutePath().toString());
    }

    public void put(byte[] key, byte[] value) {
        assertOpen("Database is closed");
        if (key == null) {
            throw new NullPointerException("key");
        }
        if (value == null) {
            throw new NullPointerException("value");
        }

        nativePut(mPtr, key, value);
    }

    public byte[] get(byte[] key) {
        return get(null, key);
    }

    public byte[] get(Snapshot snapshot, byte[] key) {
        assertOpen("Database is closed");
        if (key == null) {
            throw new NullPointerException();
        }

        return nativeGet(mPtr, snapshot != null ? snapshot.getPtr() : 0, key);
    }

    public void delete(byte[] key) {
        assertOpen("Database is closed");
        if (key == null) {
            throw new NullPointerException();
        }

        nativeDelete(mPtr, key);
    }

    public void write(WriteBatch batch) {
        assertOpen("Database is closed");
        if (batch == null) {
            throw new NullPointerException();
        }

        nativeWrite(mPtr, batch.getPtr());
    }

    public Iteration iterator() {
        return iterator(null);
    }

    public Iteration iterator(final Snapshot snapshot) {
        assertOpen("Database is closed");

        ref();

        if (snapshot != null) {
            snapshot.ref();
        }

        return new Iteration(nativeIterator(mPtr, snapshot != null ? snapshot.getPtr() : 0)) {
            @Override
            protected void closeNativeObject(long ptr) {
                super.closeNativeObject(ptr);
                if (snapshot != null) {
                    snapshot.unref();
                }

                DB.this.unref();
            }
        };
    }

    public abstract class Snapshot extends NativeObject {
        Snapshot(long ptr) {
            super(ptr);
        }
    }

    public Snapshot getSnapshot() {
        assertOpen("Database is closed");
        ref();
        return new Snapshot(nativeGetSnapshot(mPtr)) {
            @Override
            protected void closeNativeObject(long ptr) {
                nativeReleaseSnapshot(DB.this.getPtr(), getPtr());
                DB.this.unref();
            }
        };
    }

    private static native long nativeOpen(String dbpath);

    private static native void nativeClose(long dbPtr);

    private static native void nativePut(long dbPtr, byte[] key, byte[] value);

    private static native byte[] nativeGet(long dbPtr, long snapshotPtr, byte[] key);

    private static native void nativeDelete(long dbPtr, byte[] key);

    private static native void nativeWrite(long dbPtr, long batchPtr);

    private static native void nativeDestroy(String dbpath);

    private static native long nativeIterator(long dbPtr, long snapshotPtr);

    private static native long nativeGetSnapshot(long dbPtr);

    private static native void nativeReleaseSnapshot(long dbPtr, long snapshotPtr);

    static {
        System.loadLibrary("leveldbjni");
    }
}
