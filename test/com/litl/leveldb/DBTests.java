package com.litl.leveldb;

import java.util.Arrays;
import java.io.UnsupportedEncodingException;

import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.Files;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import com.litl.leveldb.DB;
import com.litl.leveldb.WriteBatch;

// set LD_LIBRARY_PATH in jUnit RunConfiguration/Environment
// requires libleveldbjni.so, libleveldb.so, libsnappy.so.1

public class DBTests {

    private Path mPath;
    private DB mDb;

    @BeforeAll
    static void setUpClass() throws Exception {
        var dir = Paths.get("db");
        Files.createDirectories(dir);
    }

    @BeforeEach
    void setUp() throws Exception {
        mPath = Paths.get("db","tests");
        mDb = new DB(mPath);
        mDb.open();
    }

    @AfterEach
    void tearDown() throws Exception {
        mDb.close();
        DB.destroy(mPath);
    }

    private byte[] bytes(String str) {
        try {
            return str.getBytes("UTF-8");
        } catch (UnsupportedEncodingException e) {
            throw new RuntimeException(e);
        }
    }

    @Test
    void testBasics() {
        mDb.put( bytes("hello"), bytes("world") );
        mDb.put( bytes("bye"), bytes("moon") );

        byte[] val;

        val = mDb.get(bytes("hello"));
        assertNotNull(val);
        assertTrue(Arrays.equals( val, bytes("world") ));

        val = mDb.get(bytes("bye"));
        assertNotNull(val);
        assertTrue(Arrays.equals( val, bytes("moon") ));

        val = mDb.get(bytes("boo"));
        assertNull(val);

    }

    @Test
    void testBatchAndIterator() {
        String[] keys = { "foo1", "foo2", "foo3", "foo4", "foo5" };
        String[] vals = { "bar1", "bar2", "bar3", "bar4", "bar5" };

        var batch = new WriteBatch();
        try {
            for (var i = 0; i < keys.length; i++) {
                var key = bytes(keys[i]);
                var val = bytes(vals[i]);
                batch.put(key, val);
            }

            mDb.write(batch);
        } finally {
            batch.close();
        }

        var iter = mDb.iterator();
        try {
            var i = 0;
            for (iter.seekToFirst(); iter.isValid(); iter.next()) {
                assertTrue(i < keys.length);

                var key = iter.getKey();
                var val = iter.getValue();

                assertTrue(Arrays.equals(bytes(keys[i]),key));
                assertTrue(Arrays.equals(bytes(vals[i]),val));

                i++;
            }
            assertTrue(i == keys.length);
        } finally {
            iter.close();
        }
    }

    @Test
    void testSnapshots() {
        mDb.put(bytes("hello"), bytes("one"));
        mDb.put(bytes("bye"), bytes("one"));
        mDb.put(bytes("hi"), bytes("one"));

        var snapshot = mDb.getSnapshot();
        var iter = mDb.iterator(snapshot);
        try {
            mDb.put(bytes("hello"), bytes("two"));
            mDb.delete(bytes("bye"));

            assertTrue(Arrays.equals(mDb.get(snapshot, bytes("hello") ), bytes("one") ));
            assertTrue(Arrays.equals(mDb.get(snapshot, bytes("bye") ), bytes("one") ));
            assertTrue(Arrays.equals(mDb.get(snapshot, bytes("hi") ), bytes("one") ));

            var i = 0;
            for (iter.seekToFirst(); iter.isValid(); iter.next()) {
                i++;
                assertTrue(Arrays.equals( iter.getValue(), bytes("one") ));
            }
            assertEquals(3, i);
        } finally {
            iter.close();
            snapshot.close();
        }

        assertTrue(Arrays.equals( mDb.get(bytes("hello")), bytes("two") ));
        assertTrue(Arrays.equals( mDb.get(bytes("hi")), bytes("one") ));
        assertNull(mDb.get(bytes("bye")));
    }

    @Test
    void testSeek() {
        mDb.put( bytes("01"), bytes("foo") );
        mDb.put( bytes("02"), bytes("foo") );
        mDb.put( bytes("11"), bytes("foo") );
        mDb.put( bytes("12"), bytes("foo") );
        mDb.put( bytes("13"), bytes("foo") );
        mDb.put( bytes("21"), bytes("foo") );

        var iter = mDb.iterator();
        try {
            iter.seek(bytes("1"));
            assertTrue(iter.isValid());
            assertTrue(Arrays.equals( bytes("11"), iter.getKey() ));

            iter.seek(bytes("2"));
            assertTrue(iter.isValid());
            assertTrue(Arrays.equals( bytes("21"), iter.getKey() ));

            iter.seek(bytes("3"));
            assertFalse(iter.isValid());
        } finally {
            iter.close();
        }

        iter = mDb.iterator();
        try {
            // Iterate over all values starting with "1"
            var i = 0;
            byte[] searchByte = { '1' };
            for (iter.seek(searchByte); iter.isValid(); iter.next()) {
                var key = iter.getKey();
                if (key[0] != searchByte[0]) {
                    break;
                }

                i++;
            }

            assertEquals(3, i);
        } finally {
            iter.close();
        }
    }

    @Test
    void testWriteBatch() {

        var putBatch = new WriteBatch();
        try {
            putBatch.put( bytes("hello"), bytes("world") );
            putBatch.put( bytes("bye"), bytes("moon") );
            mDb.write(putBatch);
        } finally {
            putBatch.close();
        }

        var iter1 = mDb.iterator();
        try {
            iter1.seekToFirst();
            assertTrue(iter1.isValid());
            assertTrue(Arrays.equals( bytes("bye"), iter1.getKey() ));
            assertTrue(Arrays.equals( bytes("moon"), iter1.getValue() ));
            iter1.next();
            assertTrue(iter1.isValid());
            assertTrue(Arrays.equals( bytes("hello"), iter1.getKey() ));
            assertTrue(Arrays.equals( bytes("world"), iter1.getValue() ));
            iter1.next();
            assertFalse(iter1.isValid());
        } finally {
            iter1.close();
        }

        var deleteBatch = new WriteBatch();
        try {
            deleteBatch.delete(bytes("hello"));
            deleteBatch.delete(bytes("bye"));
            mDb.write(deleteBatch);
        } finally {
            deleteBatch.close();
        }

        var iter2 = mDb.iterator();
        try {
            iter2.seekToFirst();
            assertFalse(iter2.isValid());
        } finally {
            iter2.close();
        }
    }
}
