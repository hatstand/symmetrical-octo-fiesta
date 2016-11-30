package com.purplehatstands.wwf.cheating;

import android.content.res.AssetManager;
import android.util.Log;
import com.google.common.collect.ImmutableList;
import com.google.protobuf.InvalidProtocolBufferException;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import words.Messages.Response;

/** TODO: http://go/java-style#javadoc */
class Grid {

  private static final String TAG = "cheating";
  private static final String MODEL_PATH = "model";
  private static final String WORD_LIST_PATH = "word_list";

  static {
    System.loadLibrary("cheating");
  }

  private String[] words = null;

  Grid(AssetManager assetManager) {
    try {
      InputStream stream = assetManager.open(WORD_LIST_PATH, AssetManager.ACCESS_STREAMING);
      BufferedReader reader = new BufferedReader(new InputStreamReader(stream));
      String line;
      ImmutableList.Builder<String> list = ImmutableList.builder();
      while ((line = reader.readLine()) != null) {
        list.add(line);
      }
      this.words = list.build().toArray(new String[0]);
      Log.d(TAG, "Loaded word list of size: " + this.words.length);
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

  byte[] recogniseGrid(AssetManager assetManager, byte[] pngData) {
    return recogniseGrid(assetManager, MODEL_PATH, pngData);
  }

  byte[] recogniseRack(AssetManager assetManager, byte[] pngData) {
    return recogniseRack(assetManager, MODEL_PATH, pngData);
  }

  Response solve(AssetManager assetManager, byte[] pngData) {
    byte[] serialized = solve(assetManager, MODEL_PATH, pngData, words);
    try {
      return Response.parseFrom(serialized);
    } catch (InvalidProtocolBufferException e) {
      e.printStackTrace();
    }
    return Response.getDefaultInstance();
  }

  private native byte[] recogniseGrid(AssetManager assetManager, String assetPath, byte[] pngData);

  private native byte[] recogniseRack(AssetManager assetManager, String assetPath, byte[] pngData);

  private native byte[] solve(
      AssetManager assetManager, String assetPath, byte[] pngData, String[] words);
}
